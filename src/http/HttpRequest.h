#ifndef SRC_HTTP_HTTPREQUEST_H
#define SRC_HTTP_HTTPREQUEST_H

#include <string>
#include <regex>
#include <unordered_map>
#include <map>
#include <iostream>

#include "../utils/utils.h"

namespace web
{
class HttpRequest
{
private:
    std::string packets_, line_, body_;
    std::vector<std::string> header_;
    std::unordered_map<std::string, std::string> requestLine_;
    std::unordered_map<std::string, std::string> requestHeader_;
    std::unordered_map<std::string, std::string> requestBody_;

    void divide();
    void parseRequestLine();
    void parseRequestHeader();
    void parseRequestBody();
public:
    HttpRequest(std::string req);
    ~HttpRequest() = default;

    void parse();
    // for debug
    void printPackets();

    bool isKeepAlive() const;
    std::string method() const;
    std::string path() const;
    std::string version() const;
    std::string getAllPostData() const;
    std::string getPostData(const std::string& key) const;
};
}; // namespace web

#endif // !SRC_HTTP_HTTPREQUEST_H