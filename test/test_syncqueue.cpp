/* 
* @Author: kangyabing
* @Date:   2017-06-13 16:28:48
* @Last Modified by:   kangyabing
* @Last Modified time: 2017-06-16 22:37:43
*/


#include <atomic>
#include <iostream>
#include <stdlib.h> 

#include "container/SyncQueue.h"
#include "log/logging.h"

using namespace kang;

using namespace std;
//g++-4.8 test_syncqueue.cpp -o test_syncqueue -lpthread -std=c++11 -O2 -Wall -Wextra -I../

atomic<int> produce_time; 
atomic<int> consume_time;

atomic<int> produce_sum;
atomic<int> consume_sum;

SyncQueue<int> my_queue(150);
const int kMaxNum = 1e8;

void Producer(){
    int num, times;
    while(true) {
        num = 6;//rand() % 10;
        my_queue.Put(num);
        produce_sum += num;
        times = ++produce_time;
        if(0 == times % 10000){
            LOG(INFO)<<"Produce time ["<<times<<"]"<<endl;
        }
        if(times >= kMaxNum){
            return;
        }
    }
}

void Customer(){
    int num, times;
    while(true){
        my_queue.Take(num);
        if(my_queue.IsStop()){
            LOG(INFO)<<"Customer end";
            return;
        }
        consume_sum += num;
        times = ++consume_time;
        if(0 == times % 10000){
            LOG(INFO)<<"Consume time ["<<times<<"]"<<endl;
        }
    }
};

int main(){
    const int thread_num = 100;
    const int produce_thread_num = 50;

    produce_time = 0;
    consume_time = 0;
    produce_sum = 0;
    consume_sum = 0;
    std::thread threads[thread_num];

    time_t start_time = time(NULL);
    for(int i = 0; i < produce_thread_num; ++i){
        threads[i] = std::thread(Producer);
    }
    for(int i = produce_thread_num; i < thread_num; ++i){
        threads[i] = std::thread(Customer);
    }

    for(int i = 0; i < produce_thread_num; ++i){
        threads[i].join();
    }
    while(produce_time != consume_time){}
    time_t end_time = time(NULL);
    LOG(INFO)<<"use time :["<<end_time-start_time<<"] seconds";
    LOG(INFO)<<"produce_time ["<<produce_time<<"]";
    LOG(INFO)<<"consume_time ["<<consume_time<<"]";
    LOG(INFO)<<"produce_sum ["<<produce_sum<<"]";
    LOG(INFO)<<"consume_sum ["<<consume_sum<<"]";
    my_queue.Stop();
    LOG(INFO)<<"queue stop";
    my_queue.Stop();
    for(int i = produce_thread_num; i < thread_num; ++i){
        threads[i].join();
    }

    return 0;
}