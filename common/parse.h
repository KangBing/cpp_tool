#pragma once

#include <unordered_map>
#include <string>
#include <fstream>

#include "log/logging.h"

namespace kang{

class Parse{
public:
    Parse() = default;
    ~Parse() = default;
    /**
     * 初始化配置文件，将配置文件内容读到map_map_container_
     * @param  file_path 配置文件路径
     * @return           成功返回true，失败返回false
     */
    bool Init(const std::string& file_path);

    /**
     * 根据field和key，返回对应的value
     * @param  field [description]
     * @param  key   [description]
     * @return       成功返回value，失败返回空的字符串
     */
    const char* Get(const std::string& field, const std::string& key);

    void DebugString();
private:
    /**
     * 从buffer中解析出field，即[field]中解析出field
     * @param  buffer 缓存要解析的字符串
     * @param  field  要返回的字符串
     * @return        成功解析返回true，否则返回false
     */
    bool ParseField(const std::string& buffer, std::string& field);
    bool ParseKeyValue(const std::string& buffer, std::string& key, std::string& value);

    /**
     * 判断是否是注释，以#开头的是注释
     * @param  buffer [description]
     * @return        是注释返回true，否则返回true
     */
    bool IsComment(const std::string& buffer){
        return 0 == buffer.find("#");
    }
    /**
     * 判断是不是空的行
     * @param  buffer [description]
     * @return        [description]
     */
    bool IsInvalid(const std::string& buffer){
        for(const auto& item:buffer){
            if(!isspace(item)){
                return false;
            }
        }

        return true;
    }
    bool IsField(const std::string& buffer){
        return std::string::npos != buffer.find("[") && std::string::npos != buffer.find("]");
    }
private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map_map_container_;
};

bool Parse::Init(const std::string& file_path){
    std::ifstream fin(file_path);
    std::string buffer;
    std::string field;
    while(std::getline(fin, buffer)){
        if(IsComment(buffer) || IsInvalid(buffer)){
                continue;
        }
        if(IsField(buffer)){
            if(!ParseField(buffer, field)){
                return false;
            }
            map_map_container_[field] = {};
        }
        else{
            std::string key, value;
            if(!ParseKeyValue(buffer, key, value)){
                return false;
            }
            map_map_container_[field][key] = value;
        }
    }
    return true;
}

const char* Parse::Get(const std::string& field, const std::string& key){
    const auto& iter_map = map_map_container_.find(field);
    if(iter_map == map_map_container_.end()){
        return nullptr;
    }

    const auto& iter = iter_map->second.find(key);
    if(iter == iter_map->second.end()){
        return nullptr;
    }

    return iter->second.data();
}

bool Parse::ParseField(const std::string& buffer, std::string& field){
    const size_t begin = buffer.find_first_of("[");
    const size_t end = buffer.find_last_of("]");
    if(std::string::npos == begin || std::string::npos == end || end <= begin + 1){
        LOG(ERROR)<<"ParseField fail. buffer ["<<buffer<<"]";
        return false;
    }

    field = buffer.substr(begin + 1, end - begin - 1);
    return true;
}

bool Parse::ParseKeyValue(const std::string& buffer, std::string& key, std::string& value){
    size_t begin = 0;
    size_t end = buffer.size() - 1;
    while(begin < buffer.size() && isspace(buffer[begin])){
        ++begin;
    }
    while(end > 0 && isspace(buffer[end])){
        --end;
    }
    do{
        if(begin >= end) break;

        const size_t mid = buffer.find("=");
        if(std::string::npos == mid) break;

        if(begin > mid || mid > end) break;

        key = buffer.substr(begin, mid);
        value = buffer.substr(mid + 1, end + 1);
        if(key.length() == 0 || value.length() == 0) break;
        return true;
    }while(0);
    LOG(ERROR)<<"ParseKeyValue fail. buffer ["<<buffer<<"]";
    return false;    
}

void Parse::DebugString(){
    std::stringstream ss;
    for(const auto& kv_map : map_map_container_){
        ss<<"["<<kv_map.first<<"]\n";
        for(const auto& kv: kv_map.second){
            ss<<kv.first<<"="<<kv.second<<"\n";
        }
    }
    std::cout<<ss.str();
}

} // end namespace