#include "./utils.h"
#include "utils.h"

std::vector<std::string> web::utils::split(const std::string &str, const std::string &pattern)
{
    if(str == "")return {};
    std::string temp = str + pattern;
    std::vector<std::string> res;
    auto pos = temp.find(pattern);
    while(pos != temp.npos)
    {   
        res.emplace_back(temp.substr(0, pos));
        temp = temp.substr(pos + pattern.size(), temp.size());
        pos = temp.find(pattern);
    }
    return res;
}

