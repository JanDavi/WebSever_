#ifndef SRC_HTTP_HTTPRESPONSE_H
#define SRC_HTTP_HTTPRESPONSE_H

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap

#include "../buffer/Buffer.h"

namespace web
{

class HttpResponse
{
private:
    // 往buf添加状态行，响应头，响应体，错误信息
    void addResponseLine(web::Buffer& buff);
    void addResponseHeader(web::Buffer& buff);
    void addResponseBody(web::Buffer& buff);
    void addErrorContent(web::Buffer& buff, std::string msg);
    void errorHtml();

    // 返回请求文件的文件类型
    std::string getFileType();

    // 状态码
    int code_;

    // 长连接标志
    bool isKeepAlive_;

    // 文件路径（相对）和 资源文件所在目录
    std::string path_;
    std::string srcDir_;
    
    // mmap返回的文件指针
    char* mmFile_; 
    // 文件信息：主要关心文件大小、文件类型、文件权限
    struct stat mmFileStat_;

    // 一些映射
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
public:
    HttpResponse();
    ~HttpResponse() { unmapFile(); }

    // 解除mmap建立的映射关心
    void unmapFile();
    
    // 初始化
    void init(const std::string& srcDir, const std::string& path, bool isKeepAlive, int code);
    // 构造HTTP响应报文
    void makeResponse(web::Buffer& buff);
    // 返回请求文件（mmap返回的指针）
    char* file();
    // 返回请求文件的大小
    size_t fileLen();
};

}// namespace web

#endif // !SRC_HTTP_HTTPRESPONSE_H