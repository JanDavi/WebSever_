#include "./HttpResponse.h"
#include "HttpResponse.h"

// 文件类型与Contente-Type的值的映射
const std::unordered_map<std::string, std::string> web::HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

// 状态码与状态描述的映射
const std::unordered_map<int, std::string> web::HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

// 4XX状态码与对应页面的映射
const std::unordered_map<int, std::string> web::HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

// 添加响应行
void web::HttpResponse::addResponseLine(web::Buffer &buff)
{
    // printf("web::HttpResponse::addResponseLine\n");
    std::string status;
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    }
    else {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
    // if(CODE_STATUS.count(code_) == 0)
    // {
    //     code_ = 400;
    // }
    // //简短的状态描述语句
    // std::string status = CODE_STATUS.find(code_)->second;
    buff.append("HTTP/1.1 " + std::to_string(code_) + " " + status);
}

// 添加响应头:只添加Connection和Content-type字段
void web::HttpResponse::addResponseHeader(web::Buffer &buff)
{
    // printf("web::HttpResponse::addResponseHeader\n");
    buff.append("Connection: ");
    if(isKeepAlive_) // 长连接
    {
        buff.append("keep-alive\r\n");
        buff.append("keep-alive: max=6, timeout=120\r\n");
    }
    else // 短连接
    {
        buff.append("close\r\n");
    }
    // todo：type or Type？？？
    buff.append("Content-type: " + getFileType() + "\r\n");
}

// mmap请求文件，读出大小，填入Content-length字段
void web::HttpResponse::addResponseBody(web::Buffer &buff)
{
    // printf("web::HttpResponse::addResponseBody\n");
    int srcFd = open((srcDir_ + path_).data(), O_RDONLY);
    if(srcFd < 0)
    {
        printf("failed to open file : %s\n", (srcDir_ + path_).data());
        addErrorContent(buff, "File Not Found!");
        return ;
    }
    int* mmRet = (int*)mmap(nullptr, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if(*mmRet == -1)
    {
        addErrorContent(buff, "File Not Found!");
        return ;
    }
    printf("open file : %s\n", (srcDir_ + path_).data());
    mmFile_ = (char*)mmRet;
    close(srcFd);
    buff.append("Content-length: " + std::to_string(mmFileStat_.st_size) + "\r\n");
}

// 解除mmap建立的映射
void web::HttpResponse::unmapFile()
{
    // printf("web::HttpResponse::unmapFile\n");
    if(mmFile_)
    {
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}

// 直接构造错误的HTML页面，嵌入传入的msg
void web::HttpResponse::addErrorContent(web::Buffer &buff, std::string msg)
{
    printf("web::HttpResponse::addErrorContent\n");
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        status = "Bad Request";
    }
    body += std::to_string(code_) + " : " + status  + "\n";
    body += "<p>" + msg + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";
    buff.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buff.append(body);
}

// 如果状态码是4XX，更改即将读取的文件的路径和文件信息
void web::HttpResponse::errorHtml()
{
    // printf("web::HttpResponse::errorHtml\n");
    if(CODE_PATH.count(code_) == 1)
    {
        // 更改路径，并读取对应的文件信息
        path_ = CODE_PATH.find(code_)->second;
        stat((srcDir_ + path_).data(), &mmFileStat_);
    }
}

// 判断文件类型
std::string web::HttpResponse::getFileType()
{
    // 以文件名最后一个 “.” 后面的后缀名作为判断文件类型的依据
    // 若文件名没有“.”或者对应后缀名并不在预设范围内，
    // 当"text/plain"返回
    // auto idx = path_.find_last_of(".");
    // std::string suffix = path_.substr(idx);
    // if(idx == std::string::npos || SUFFIX_TYPE.count(suffix) == 0) // 文件名没有 “.”
    // {
    //     return "text/plain";
    // }
    // else
    // {
    //     return SUFFIX_TYPE.find(suffix)->second;
    // }
    /* 判断文件类型 */
    std::string::size_type idx = path_.find_last_of('.');
    if(idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = path_.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

// 构造函数：初始胡一些变量
web::HttpResponse::HttpResponse()
{
    code_ = -1;
    srcDir_ = path_ = "";
    mmFile_ = nullptr;
    mmFileStat_ = {0};
    isKeepAlive_ = false;
}

// 重置变量的值
void web::HttpResponse::init(const std::string &srcDir, const std::string &path, bool isKeepAlive, int code)
{
    // printf("web::HttpResponse::init\n");
    if(mmFile_)
    {
        unmapFile();
    }
    code_ = code;
    isKeepAlive_ = isKeepAlive;
    path_ = path;
    srcDir_ = srcDir;
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
}

// 构造HTTP响应报文
void web::HttpResponse::makeResponse(web::Buffer &buff)
{
    // printf("web::HttpResponse::makeResponse\n");
    // 判断请求文件是否符合
    // 若获取文件信息失败或者请求的文件是一个目录
    if(stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode))
    {
        printf("%s : File Not Found!!!!!!!!!!!!!!!!!!\n", (srcDir_+path_).data());
        code_ = 404;
    }
    else if(!(mmFileStat_.st_mode & S_IROTH))// 检查文件权限：是否可由持有者用户组之外的用户读
    {
        code_ = 403;
    }
    else if(code_ == -1) {
        code_ = 200;
    }
    errorHtml();
    addResponseLine(buff);
    addResponseHeader(buff);
    addResponseBody(buff);
    // printf("end : web::HttpResponse::makeResponse\n");
}

// 返回请求文件（mmap返回的指针）
char* web::HttpResponse::file()
{
    return mmFile_;
}

// 返回请求文件的大小
size_t web::HttpResponse::fileLen()
{
    return mmFileStat_.st_size;
}
