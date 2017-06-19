/* 
* @Author: kangyabing
* @Date:   2017-06-16 10:54:52
* @Last Modified by:   kangyabing
* @Last Modified time: 2017-06-16 11:37:37
*/

#include <log/logging.h>
using namespace kang;

//g++-4.8 -o test_log test_log.cpp -std=c++11 -O2 -Wall -Wextra -I../

int main(){
    LOG(INFO)<<"This is a test info log";
    LOG(WARNING)<<"This is a test warning log";
    LOG(ERROR)<<"This is a test error log";
    return 0;
}