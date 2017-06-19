#pragma once

#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>

namespace kang{
/**
 * \brief 同步队列，Producer线程向队列添加任务，Consumer从队列取走任务
 */
template<typename T>
class SyncQueue
{
public:
    /**
     * \brief 构造队列
     * \param max_size 队列最大容量
     */
    SyncQueue(int max_size = 100) 
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
        std::unique_lock<std::mutex> locker(mutex_);
        bool ret = WaitFor(locker, timeout, true);        
        if(stop_ || !ret){
            return false;
        }
        t = std::move(vector_[start_]);
        start_ = (start_ + 1) % max_size_;
        --cur_size_;
        not_full_.notify_one();
        return true;
    }

    bool Take(std::vector<T>& _vector, size_t timeout = 0){
        std::unique_lock<std::mutex> locker(mutex_);
        bool ret = WaitFor(locker, timeout, true);
        if (stop_ || !ret){
            return false;
        }
        _vector.reserve(cur_size_);
        while(cur_size_ > 0){
            _vector.emplace_back(std::move(vector_[start_]));
            --cur_size_;
            start_ = (start_ + 1) % max_size_;
        }
        not_full_.notify_one();
        return true;
    }

   
    /**
     * 停止使用队列，唤醒所有等待队列的线程
     */
    void Stop(){
        {
            std::lock_guard<std::mutex> locker(mutex_);
            stop_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

    bool Empty(){
        std::lock_guard<std::mutex> locker(mutex_);
        return 0 == cur_size_;
    }

    bool Full(){
        std::lock_guard<std::mutex> locker(mutex_);
        return cur_size_ == max_size_;
    }

    size_t Size(){
        std::lock_guard<std::mutex> locker(mutex_);
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

    /**
     * 等待队列非空或非满
     * @param  locker    锁，这里是引用
     * @param  timeout   超时时间，单位ms
     * @param  not_empty 是否等待队列非空
     * @return           超时返回false，否则返回true
     */
    inline bool WaitFor(std::unique_lock<std::mutex>& locker,const size_t& timeout, bool not_empty){
        if(0 == timeout){
            if(not_empty){
                not_empty_.wait(locker, 
                    [this]{return stop_ ||  NotEmpty(); });
            }
            else{
                not_full_.wait(locker, 
                    [this]{return stop_ || NotFull(); });
            }
            return true;
        }
        else{
            if(not_empty){
                return not_empty_.wait_for(locker,
                    std::chrono::milliseconds(timeout),
                    [this]{return stop_ ||  NotEmpty(); });
            }
            else{
                return not_full_.wait_for(locker,
                    std::chrono::milliseconds(timeout),
                    [this]{return stop_ ||  NotFull(); });
            }
        }
    }

    template<typename F>
    bool Add(F&& x, size_t timeout){
        std::unique_lock<std::mutex> locker(mutex_);
        bool ret = WaitFor(locker, timeout, false);        
        if(stop_ || !ret){
            return false;
        }
        vector_[end_] = std::forward<F>(x);
        end_ = (end_ + 1) % max_size_;
        ++cur_size_;
        not_empty_.notify_one();
        return true;
    }

private:
    
    std::vector<T> vector_;
    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    size_t start_ = 0;
    size_t end_ = 0;
    size_t max_size_ = 100;
    size_t cur_size_ = 0;
    bool stop_ = false;
};

}
