/* 
* @Author: kangyabing
* @Date:   2017-06-13 16:28:48
* @Last Modified by:   kangyabing
* @Last Modified time: 2017-06-16 16:22:40
*/


#include <atomic>
#include <iostream>
#include <stdlib.h> 

#include "container/SyncQueue.h"
#include "log/logging.h"

using namespace kang;

using namespace std;
//g++-4.8 test_syncqueue_timeout.cpp -o test_syncqueue_timeout -lpthread -std=c++11 -O2 -Wall -Wextra -I../

atomic<int> produce_time; 
atomic<int> consume_time;

atomic<int> produce_sum;
atomic<int> consume_sum;

SyncQueue<int> my_queue(15);

void Producer(){
    int num;
    while(true) {
        num = ++consume_sum;
        if(!my_queue.Put(num, 3000)){
            LOG(INFO)<<"Producer TimeOut";
            continue;
        }
        LOG(INFO)<<"Produce ["<<num<<"]";
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 10));       
    }
}

void Customer(){
    int num;
    while(true){
        if(!my_queue.Take(num, 3000)){
            LOG(INFO)<<"Customer TimeOut";
            continue;
        }
        LOG(INFO)<<"Consume ["<<num<<"]";
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 10));
    }
};

int main(){
    const int thread_num = 10;
    const int produce_thread_num = 5;

    std::thread threads[thread_num];


    for(int i = 0; i < produce_thread_num; ++i){
        threads[i] = std::thread(Producer);
    }
    for(int i = produce_thread_num; i < thread_num; ++i){
        threads[i] = std::thread(Customer);
    }

    for(int i = 0; i < produce_thread_num; ++i){
        threads[i].join();
    }
    for(int i = produce_thread_num; i < thread_num; ++i){
        threads[i].join();
    }

    return 0;
}