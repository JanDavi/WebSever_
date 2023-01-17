#include "./HttpRequest.h"
#include "HttpRequest.h"

using namespace web;

// 默认访问路径
const std::unordered_set<std::string> web::HttpRequest::DEFAULT_HTML 
{
    "/index", "/register", "/login",
    "/welcome", "/video", "/picture", 
};

// 正则表达式解析请求行的请求方法、URL、版本
bool web::HttpRequest::parseRequestLine(const std::string& line)
{
    printf("web::HttpRequest::parseRequestLine\n");
    std::regex pattern("^(.*) (.*) HTTP/(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)) {   
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = REQUEST_HEADERS;
        printf("method: %s, path: %s, version: %s\n", method_.c_str(), path_.c_str(), version_.c_str());
        return true;
    }
    printf("RquestLine Error");
    return false;
}

// 请求url与对应html的映射（不在DEFAULT_HTML里的不改变）
void web::HttpRequest::parsePath()
{
    printf("web::HttpRequest::parsePath\n");
    if(path_ == "/")
    {
        path_ = "/index.html";
    }
    else
    {
        for(auto &item: DEFAULT_HTML) 
        {
            if(item == path_) 
            {
                path_ += ".html";
                break;
            }
        }
    }
    printf("file path : %s\n", path_.c_str());
}

// 正则表达式解析（一行）请求头
void HttpRequest::parseRequestHeader(const std::string& line)
{
    printf("HttpRequest::parseRequestHeader\n");
    std::regex pattern("^(.*): (.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)) {   
        requestHeader_[subMatch[1]] = subMatch[2];
        std::cout << subMatch[1] << " : " << subMatch[2] << std::endl;
    }
    else    // 这种情况就是空行
    {
        state_ = REQUEST_BODY;
    }
}

// todo：解析请求体
void HttpRequest::parseRequestBody(const std::string& line)
{
    // auto params = web::utils::split(body_ + "&", "&");
    // std::regex pattern("^(.*)=(.*)$");
    // std::smatch subMatch;
    // for(auto& param : params)
    // {
    //     if(std::regex_match(param, subMatch, pattern)) {   
    //         requestBody_[subMatch[1]] = subMatch[2];
    //     }
    // }
}

// 初始化
void web::HttpRequest::init()
{
    printf("web::HttpRequest::init\n");
    method_ = path_ = version_ = "";
    state_ = REQUEST_LINE;
    requestHeader_.clear();
    requestBody_.clear();
}

// 对外暴露的解析接口
bool web::HttpRequest::parse(web::Buffer &buff)
{
    printf("web::HttpRequest::parse\n");
    // 传入的buff即为一个HTTP请求报文
    // 每一行的都是以\r\n结尾，空行就只有\r\n
    const char CRLF[] = "\r\n";
    if(buff.readableBytes() <= 0)
    {
        return false;
    }
    while (buff.readableBytes() && state_ != FINISH)
    {
        // search:输入两个区间s1,s2，返回s2在s2的开始位置，相当于string.find()
        const char* lineEnd = std::search(buff.readPos(), buff.writePos(), &CRLF[0], CRLF + 2);
        
        // 一行报文
        std::string line((const char*)buff.readPos(), lineEnd);
        
        // 根据当前解析状态来调用相应的解析函数
        switch (state_)
        {
        case REQUEST_LINE:
            if(!parseRequestLine(line))
            {
                return false;
            }
            parsePath();
            break;
        case REQUEST_HEADERS:
            parseRequestHeader(line);
            if(buff.readableBytes() <= 2)
            {
                state_ = FINISH;
            }
            break;
        case REQUEST_BODY:
            parseRequestBody(line);
            break;
        default:
            break;
        }

        if(lineEnd == buff.writePos())
        {
            break;
        }
        buff.retrieveUntil(lineEnd + 2);
    }
    printf("[%s], [%s], [%s]\n", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

// 打印解析结果
void web::HttpRequest::printPackets()
{
    std::cout << "request line:------------------" << std::endl;
    std::cout << "method: " << method_ << std::endl;
    std::cout << "path: " << path_ << std::endl;
    std::cout << "version: " << version_ << std::endl;
    std::cout << "-------------------------------" << std::endl;

    std::cout << "request header:----------------" << std::endl;
    for(auto& p : requestHeader_)
    {
        std::cout << p.first << ":" << p.second << std::endl;
    }
    std::cout << "-------------------------------" << std::endl;

    std::cout << "request body:------------------" << std::endl;
    for(auto& p : requestBody_)
    {
        std::cout << p.first << ":" << p.second << std::endl;
    }
    std::cout << "-------------------------------" << std::endl;
}

// 返回是否为长连接
bool web::HttpRequest::isKeepAlive() const
{
    printf("web::HttpRequest::isKeepAlive()\n");
    auto iter = requestHeader_.find("Connection");
    return iter != requestHeader_.end() && iter->second == "keep-alive";
}

// 返回请求方法
std::string HttpRequest::method() const
{
    return method_;
}

// 返回请求url
std::string HttpRequest::path() const
{
    return path_;
}

// 返回http版本
std::string HttpRequest::version() const
{
    return version_;
}

// todo：返回请求体中的数据
std::string HttpRequest::getPostData(const std::string& key) const
{
    auto iter = requestBody_.find(key);
    if(iter != requestBody_.end())
        return iter->second;
    else
        return "";
}