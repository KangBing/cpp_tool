#pragma once

#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>

#include "lock/SpinLock.h"

namespace kang{
/**
 * \brief 同步队列，Producer线程向队列添加任务，Consumer从队列取走任务
 */
template<typename T>
class SyncQueueSpin
{
public:
    /**
     * \brief 构造队列
     * \param max_size 队列最大容量
     */
    SyncQueueSpin(int max_size = 100) 
        :max_size_(max_size){
            vector_.resize(max_size_);
    }

    /**
     * \brief 向队列添加任务
     * @param  x       任务
     * @param  timeout 超时时间，单位ms
     * @return         成功返回true，失败返回false（失败可能是超时或队列终止）
     */
    bool Put(const T&x, size_t timeout = 0){
        return Add(x, timeout);
    }

    bool Put(T&& x, size_t timeout = 0){
        return Add(std::forward<T>(x), timeout);
    }

    /**
     * \brief 从队列取走任务
     * @param  t       任务
     * @param  timeout 超时时间，单位ms
     * @return         成功返回true，失败返回false（失败可能是超时或队列终止）
     */
    bool Take(T& t, size_t timeout = 0){
        while(true){
            while(Empty()){
                if(stop_){
                    return false;
                }
            }
            spin_lock_.Lock();
            if(stop_){
                spin_lock_.UnLock();
                return false;
            }
            if(Empty()){
                spin_lock_.UnLock();
                continue;
            }
            t = std::move(vector_[start_]);
            start_ = (start_ + 1) % max_size_;
            --cur_size_;
            spin_lock_.UnLock();
            return true;
        }

    }

    bool Take(std::vector<T>& _vector, size_t timeout = 0){
        while(true){
            while(Empty()){
                if(stop_){
                    return false;
                }
            }
            spin_lock_.Lock();
            if(stop_){
                spin_lock_.UnLock();
                return false;
            }
            if(Empty()){
                spin_lock_.UnLock();
                continue;
            }
            _vector.reserve(cur_size_);
            while(cur_size_ > 0){
                _vector.emplace_back(std::move(vector_[start_]));
                --cur_size_;
                start_ = (start_ + 1) % max_size_;
            }
            spin_lock_.UnLock();
            return true;
            
        }
    }

   
    /**
     * 停止使用队列，唤醒所有等待队列的线程
     */
    void Stop(){
        stop_ = true;
    }

    bool Empty(){
        return 0 == cur_size_;
    }

    bool Full(){
        return cur_size_ == max_size_;
    }

    size_t Size(){
        return cur_size_;
    }
    bool IsStop(){
        return stop_;
    }

private:
    /**
     * \brief 内部调用，调用之前需加锁
     * \return bool 没有满，则返回true
     */
    inline bool NotFull() const{
        return !(cur_size_ == max_size_);
    }

    /**
     * \brief 内部调用，调用之前需加锁
     * \return bool 非空，则返回true
     */
    inline bool NotEmpty() const{
        return !(0 == cur_size_);
    }

    template<typename F>
    bool Add(F&& x, size_t timeout){

        while(true){
            while(Full()){
                if(stop_){
                    return false;
                }
            }
            spin_lock_.Lock();
            if(stop_){
                spin_lock_.UnLock();
                return false;
            }
            if(Full()){
                spin_lock_.UnLock();
                continue;
            }
            vector_[end_] = std::forward<F>(x);
            end_ = (end_ + 1) % max_size_;
            ++cur_size_;
            spin_lock_.UnLock();
            return true;
        }

    }

private:
    
    std::vector<T> vector_;
    SpinLock spin_lock_;
    volatile size_t start_ = 0;
    volatile size_t end_ = 0;
    volatile size_t max_size_ = 100;
    volatile size_t cur_size_ = 0;
    volatile bool stop_ = false;
};

}
