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
#include <sys/uio.h>     // readv/writev

#include "../HttpRequest.h"
#include "../HttpResponse.h"
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
    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));
    int ret = bind(listenFd, (struct sockaddr*)&listenAddr, sizeof(listenAddr));
    ret = listen(listenFd, 5);
    printf("listening at %d \n", listenFd);
    int connFd = accept(listenFd, NULL, NULL);
    
    do
    {   
        HttpRequest req;
        Buffer buffer;
        int saveErrno = -1;

        // Buffer从connFd读取数据，并存到Buffer中
        buffer.readFd(connFd, &saveErrno);
        // HttpRequest解析报文
        req.parse(buffer);
        // 打印解析结果
        req.printPackets();

        HttpResponse response_;
        // 需要将相应路径初始化HttpResponse对象
        string srcDir = "/root/WebSever_/resources";
        response_.init(srcDir, req.path(), true, 200);
        
        Buffer writeBuf_;
        response_.makeResponse(writeBuf_);

        // 状态行和响应头放到iov_[0]
        struct iovec iov_[2];
        int iovCnt_ = 1;
        iov_[0].iov_base = static_cast<char*>(writeBuf_.readPos());
        iov_[0].iov_len = writeBuf_.readableBytes();

        // 如果有文件，则将文件放到iov_[1]
        if(response_.fileLen() > 0 && response_.file() != nullptr)
        {
            iov_[1].iov_base = response_.file();
            iov_[1].iov_len = response_.fileLen();
            iovCnt_ = 2;
        }
        writev(connFd, iov_, iovCnt_);
    }while(0);
    return 0;
}