#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <iostream>
#include <unistd.h> 
#include <cstdio> 
#include <cstdlib> 
#include <cstring> 
#include <fcntl.h> 
#include <linux/fb.h> 
#include <sys/mman.h> 
#include <sys/ioctl.h>
#include <sys/stat.h>    // stat

#include "../HttpRequest.h"
#include "../../buffer/Buffer.h"

using namespace std;
using namespace web;

const std::unordered_map<std::string, std::string> SUFFIX_TYPE = {
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

int main()
{
    struct sockaddr_in listenAddr;
    bzero(&listenAddr, sizeof(listenAddr));
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(2024);
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    //设置端口复用 
    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));
    int ret = bind(listenFd, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
    ret = listen(listenFd, 5);
    printf("listening at %d \n", listenFd);
    
    int connFd = accept(listenFd, NULL, NULL);
    HttpRequest req;
    Buffer buffer;
    do
    {
        char buf[65535];
        ret = read(connFd, buf, 65534);
        buffer.append(buf, strlen(buf));
        req.parse(buffer);
        
        // 以下操作是HttpResponse的任务
        struct stat mmFileStat_;
        string path = "/root/WebSever_/resources" + req.path();
        int srcFd = open(path.c_str(), O_RDONLY);
        printf("open file: %s\n", path.c_str());
        stat(path.data(), &mmFileStat_);
        char* mmFile_ = (char*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
        close(srcFd);
        
        string resp_body(mmFile_, mmFileStat_.st_size);
        std::string resp_line = "HTTP/1.1 200 OK\r\n";
        std::string resp_header;
        resp_header += "Connection: keep-alive\r\n";
        resp_header += "keep-alive: max=6, timeout=120\r\n";
        resp_header += "Content-type: ";
        // 以文件名最后一个 “.” 后面的后缀名作为判断文件类型的依据
        // 若文件名没有“.”或者对应后缀名并不在预设范围内，
        // 当"text/plain"返回
        auto idx = path.find_last_of(".");
        std::string suffix = path.substr(idx);
        if(idx == std::string::npos || SUFFIX_TYPE.count(suffix) == 0) // 文件名没有 “.”
        {
            resp_header += "text/plain";
        }
        else
        {
            resp_header += SUFFIX_TYPE.find(suffix)->second;
        }
        resp_header += "\r\n";
        resp_header += "Content-length: " + std::to_string(resp_body.size()) + "\r\n\r\n";
        cout << "header:--------" << endl;
        cout << resp_header << endl;
        std::string resp = resp_line + resp_header + resp_body;
        write(connFd, resp.c_str(), strlen(resp.c_str())); 
    }while(0);
    return 0;
}