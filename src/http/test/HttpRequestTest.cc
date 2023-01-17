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
    
    
    while (true)
    {
        int connFd = accept(listenFd, NULL, NULL);
        char buf[4096];
        ret = read(connFd, buf, 4096 - 1);
        Buffer buffer;
        buffer.append(buf, strlen(buf));
        HttpRequest req;
        req.init();
        req.parse(buffer);
        req.printPackets();

        struct stat mmFileStat_;
        string path = "/root/WebSever_/resources" + req.path();
        int srcFd = open(path.c_str(), O_RDONLY);
        stat(path.data(), &mmFileStat_);
        char* mmFile_ = (char*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
        close(srcFd);
        
        string resp_body(mmFile_, mmFileStat_.st_size);
        std::string resp_line = "HTTP/1.1 200 OK\r\n";
        std::string resp_header;
        resp_header += "Connection: keep-alive\r\n";
        resp_header += "keep-alive: max=6, timeout=120\r\n";
        if(req.path() == "/favicon.webp")
            resp_header += "Content-type: image/webp\r\n";
        else
            resp_header += "Content-type: text/html\r\n";
        resp_header += "Content-length: " + std::to_string(resp_body.size()) + "\r\n\r\n";
        std::string resp = resp_line + resp_header + resp_body;
        write(connFd, resp.c_str(), strlen(resp.c_str())); 
    }
    return 0;
}