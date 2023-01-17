#include "./HttpRequest.h"
#include "HttpRequest.h"

using namespace web;

const std::unordered_set<std::string> web::HttpRequest::DEFAULT_HTML 
{
    "/index", "/register", "/login",
    "/welcome", "/video", "/picture", 
};

bool web::HttpRequest::parseRequestLine(const std::string& line)
{
    // 正则表达式解析请求行的请求方法、URL、版本
    std::regex pattern("^(.*) (.*) HTTP/(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)) {   
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = REQUEST_HEADERS;
        return true;
    }
    printf("RquestLine Error");
    return false;
}

void web::HttpRequest::parsePath()
{
    if(path_ == "/")
    {
        path_ = "/index.html";
    }
    else if(path_ == "/favicon.ico")
    {
        path_ = "/favicon.webp";
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
}

void HttpRequest::parseRequestHeader(const std::string& line)
{
    std::regex pattern("^(.*): (.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)) {   
        requestHeader_[subMatch[1]] = subMatch[2];
    }
    else    // 这种情况就是空行
    {
        state_ = REQUEST_BODY;
    }
}

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

void web::HttpRequest::init()
{
    method_ = path_ = version_ = "";
    state_ = REQUEST_LINE;
    requestHeader_.clear();
    requestBody_.clear();
}

bool web::HttpRequest::parse(web::Buffer &buff)
{
    // 传入的buff即为一个HTTP请求报文

    // 每一行的都是以\r\n结尾，空行就只有\r\n
    const char CRLF[] = "\r\n";
    if(buff.readableBytes() <= 0);
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
    // printf("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

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

bool web::HttpRequest::isKeepAlive() const
{
    auto iter = requestHeader_.find("Connection");
    return iter != requestHeader_.end() && iter->second == "keep-alive";
}

std::string HttpRequest::method() const
{
    return method_;
}

std::string HttpRequest::path() const
{
    return path_;
}

std::string HttpRequest::version() const
{
    return version_;
}

std::string HttpRequest::getPostData(const std::string& key) const
{
    auto iter = requestBody_.find(key);
    if(iter != requestBody_.end())
        return iter->second;
    else
        return "";
}