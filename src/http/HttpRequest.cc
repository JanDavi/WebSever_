#include "./HttpRequest.h"
#include "HttpRequest.h"

using namespace web;

HttpRequest::HttpRequest(std::string req) : packets_(req)
{
    
}

void HttpRequest::divide()
{
    auto lines = utils::split(packets_, "\r\n");
    line_ = lines[0];

    int i = 1;
    for(; i < lines.size() && lines[i] != ""; ++i)
    {
        header_.emplace_back(lines[i]);
    }

    ++i;
     for(; i < lines.size() && lines[i] != "\r\n"; ++i)
    {
        body_ += lines[i];
    }
}

void HttpRequest::parseRequestLine()
{
    std::regex pattern("^(.*) (.*) HTTP/(.*)$");
    // std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(std::regex_match(line_, subMatch, pattern)) {   
        requestLine_["method"] = subMatch[1];
        requestLine_["path"] = subMatch[2];
        requestLine_["version"] = subMatch[3];
    }
}

void HttpRequest::parseRequestHeader()
{
    std::regex pattern("^(.*): (.*)$");
    std::smatch subMatch;
    for(auto& line : header_)
    {
        if(std::regex_match(line, subMatch, pattern)) {   
            requestHeader_[subMatch[1]] = subMatch[2];
        }
    }
}

void HttpRequest::parseRequestBody()
{
    auto params = web::utils::split(body_ + "&", "&");
    std::regex pattern("^(.*)=(.*)$");
    std::smatch subMatch;
    for(auto& param : params)
    {
        if(std::regex_match(param, subMatch, pattern)) {   
            requestBody_[subMatch[1]] = subMatch[2];
        }
    }
}

void web::HttpRequest::parse()
{
    divide();
    parseRequestLine();
    parseRequestHeader();
    parseRequestBody();
}

void web::HttpRequest::printPackets()
{
    std::cout << "packets:" << std::endl;
    std::cout << packets_ << std::endl;

    std::cout << "request line:" << std::endl;
    for(auto& p : requestLine_)
    {
        std::cout << p.first << ":" << p.second << std::endl;
    }

    std::cout << "request header:" << std::endl;
    for(auto& p : requestHeader_)
    {
        std::cout << p.first << ":" << p.second << std::endl;
    }

    std::cout << "request body:" << std::endl;
    for(auto& p : requestBody_)
    {
        std::cout << p.first << ":" << p.second << std::endl;
    }
}

bool web::HttpRequest::isKeepAlive() const
{
    auto iter = requestHeader_.find("Connection");
    return iter != requestHeader_.end() && iter->second == "keep-alive";
}

std::string HttpRequest::method() const
{
    auto iter = requestLine_.find("method");
    if(iter != requestLine_.end())
        return iter->second;
    else
        return "";
}

std::string HttpRequest::path() const
{
    auto iter = requestLine_.find("path");
    if(iter != requestLine_.end())
        return iter->second;
    else
        return "";
}

std::string HttpRequest::version() const
{
    auto iter = requestLine_.find("version");
    if(iter != requestLine_.end())
        return iter->second;
    else
        return "";
}

std::string HttpRequest::getAllPostData() const
{
    return body_;
}

std::string HttpRequest::getPostData(const std::string& key) const
{
    auto iter = requestBody_.find(key);
    if(iter != requestBody_.end())
        return iter->second;
    else
        return "";
}