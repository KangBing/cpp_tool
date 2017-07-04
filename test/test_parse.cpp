/* 
* @Author: kangyabing
* @Date:   2017-07-04 09:41:15
* @Last Modified by:   kangyabing
* @Last Modified time: 2017-07-04 10:57:17
*/

#include "log/logging.h"
#include "common/parse.h"
using namespace kang;

//g++-4.8 -o test_parse test_parse.cpp -std=c++11 -O2 -Wall -Wextra -I../

int main(int argc, char* argv[]){
    (void) argc;
    std::string file_path = argv[0];
    file_path.append(".ini");
    Parse parse;
    if(!parse.Init(file_path)){
        LOG(ERROR)<<"parse fail";
    }
    else{
        parse.DebugString();
        const char* p1 = parse.Get("Field1", "key1");
        const char* p3 = parse.Get("Field1", "key3");
        if(p1){
            LOG(INFO)<<"field [Field1] key [key1] has value ["<<p1<<"]";
        }
        if(!p3){
            LOG(INFO)<<"field [Field1] doesn't has key [key3]";
        }

    }
    return 0;
}