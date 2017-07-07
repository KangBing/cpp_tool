#pragma once

#include <assert.h>
#include <ctime>
#include <sstream>
#include <iostream>

namespace kang{

#define LOG_INFO    LogINFO(__FILE__, __LINE__)
#define LOG_ERROR   LogERROR(__FILE__, __LINE__)
#define LOG_WARNING LogWARNING(__FILE__, __LINE__)
#define LOG_FATAL   LogFATAL(__FILE__, __LINE__)

#define LOG(severity) LOG_##severity.stream()

#define CHECK(x)    \
    if(!(x))        \
        LOG(FATAL)<<"Check failed: " #x <<". "
#define CHECK_LT(x, y) CHECK((x) < (y))
#define CHECK_GT(x, y) CHECK((x) > (y))
#define CHECK_LE(x, y) CHECK((x) <= (y))
#define CHECK_GE(x, y) CHECK((x) >= (y))
#define CHECK_EQ(x, y) CHECK((x) == (y))
#define CHECK_NE(x, y) CHECK((x) != (y))

/*
 * \brief 初始化日志，例如设置日志目录等操作
 */
inline void InitLogging(const char* argv0){
    // TODO 
    (void)argv0;
}

class DateHelper{
public:
    DateHelper() = default;
    ~DateHelper() = default;
    const char* ReadbleDate(){
        time_t seconds = time(NULL);
        struct tm tm_time;
        localtime_r(&seconds, &tm_time);
        sprintf(buffer_, "%04d-%02d-%02d %02d:%02d:%02d",
                tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        return buffer_;
    }
private:
    char buffer_[20];
};
/* thread_local 每个线程一个实例，不重复构造和析构
 */
thread_local std::stringstream thread_stream;
thread_local DateHelper thread_date_helper;

class LogBase{
public:
    LogBase(const char* file, int line, const char* severity = nullptr){
        thread_stream.str("");
        thread_stream.clear();
        thread_stream<<"["<<thread_date_helper.ReadbleDate()<<" "
                    <<file<<":"<<line<<" "<<severity<<"] ";
    }
    ~LogBase(){
        /*输出到标准错误*/
        std::cerr<<thread_stream.str()<<"\n";
        if(is_exit_){
            abort();
        }
    }
    std::stringstream& stream(){
        return thread_stream;
    }
protected:
    bool is_exit_{false};
};

class LogINFO: public LogBase{
public:
    LogINFO(const char* file, int line): LogBase(file, line, "INFO"){}
    ~LogINFO(){}
};

class LogERROR: public LogBase{
public:
    LogERROR(const char* file, int line): LogBase(file, line, "ERROR"){}
    ~LogERROR(){}
};

class LogWARNING: public LogBase{
public:
    LogWARNING(const char* file, int line): LogBase(file, line, "WARNING"){}
    ~LogWARNING(){}
};

class LogFATAL: public LogBase{
public:
    LogFATAL(const char* file, int line): LogBase(file, line, "FATAL"){}
    ~LogFATAL(){is_exit_ = true;}
};

}