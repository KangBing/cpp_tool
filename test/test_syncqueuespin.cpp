/* 
* @Author: kangyabing
* @Date:   2017-06-21 16:28:48
* @Last Modified by:   kangyabing
* @Last Modified time: 2017-06-23 20:07:50
*/


#include <atomic>
#include <iostream>
#include <stdlib.h> 

#include "container/SyncQueueSpin.h"
#include "log/logging.h"

using namespace kang;


using namespace std;
//g++-4.8 -I../ -O1 test_syncqueuespin.cpp -o test_syncqueuespin -std=c++0x -lpthread 
atomic<int> produce_num; 
atomic<int> consume_num;

SyncQueueSpin<int> my_queue(3);
const int kMaxNum = 10000000;

void Producer(){
    while(true) {
        int num = ++produce_num;
        if(num > kMaxNum){
            return;
        }
        if(0 == num % 10000){
            cout<<"Produce num ["<<num<<"]"<<endl;
        }
        my_queue.Put(num);
    }
}

void Customer(){
    int num;
    while(true){
        my_queue.Take(num);
        ++consume_num;
        if(0 == num % 10000){
            cout<<"Consume num ["<<num<<"]"<<endl;
        }
    }
};

int main(){
    int thread_num = 10;
    int produce_thread_num = 5;
    produce_num = 0;
    consume_num = 0;
    std::thread threads[100];

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
    time_t end_time = time(NULL);
    std::cout<<"use time :"<<end_time-start_time<<std::endl;
    cout<<"consume_num ["<<consume_num<<"]"<<endl;
    for(int i = produce_thread_num; i < thread_num; ++i){
        threads[i].join();
    }

    return 0;
}