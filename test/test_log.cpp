/* 
* @Author: kangyabing
* @Date:   2017-06-16 10:54:52
* @Last Modified by:   kangyabing
* @Last Modified time: 2017-07-07 10:40:06
*/

#include <log/logging.h>
using namespace kang;

//g++-4.8 -o test_log test_log.cpp -std=c++11 -O2 -Wall -Wextra -I../

int main(){
    LOG(INFO)<<"This is a test info log";
    LOG(WARNING)<<"This is a test warning log";
    LOG(ERROR)<<"This is a test error log";
    CHECK(1 < 2)<<"hello world";
    CHECK_GT(1, 2)"hello world";
    LOG(FATAL)<<"This is a fatal log";
    return 0;
}