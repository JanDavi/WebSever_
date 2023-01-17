#ifndef SRC_HTTP_HTTPREQUEST_H
#define SRC_HTTP_HTTPREQUEST_H

#include <string>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <iostream>

#include "../utils/utils.h"
#include "../buffer/Buffer.h"

namespace web
{
class HttpRequest
{
private:
    // 解析状态
    enum PARSE_STATE {
        REQUEST_LINE,
        REQUEST_HEADERS,
        REQUEST_BODY,
        FINISH,        
    };

    // todo : 解析结果
    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    // 请求方法
    std::string method_;
    // 请求路径
    std::string path_;
    // 版本
    std::string version_;
    // 请求头
    std::unordered_map<std::string, std::string> requestHeader_;
    // 请求体
    std::unordered_map<std::string, std::string> requestBody_;
    // 默认html页面
    static const std::unordered_set<std::string> DEFAULT_HTML;
    
    // 解析状态
    PARSE_STATE state_;
    
    // 解析请求行
    bool parseRequestLine(const std::string& line);
    // 解析请求路径（路径与文件的映射）
    void parsePath();
    // 解析请求头
    void parseRequestHeader(const std::string& line);
    // todo : 解析请求体
    void parseRequestBody(const std::string& line);

public:
    HttpRequest() { init(); }
    ~HttpRequest() = default;
    
    // 初始化
    void init();

    // 唯一暴露的解析接口
    bool parse(web::Buffer& buff);

    // for debug
    void printPackets();

    // 返回是否为长连接
    bool isKeepAlive() const;

    // 返回请求行的方法、url、版本
    std::string method() const;
    std::string path() const;
    std::string version() const;

    // 传入key返回请求体对应的value
    std::string getPostData(const std::string& key) const;
};
}; // namespace web

#endif // !SRC_HTTP_HTTPREQUEST_H