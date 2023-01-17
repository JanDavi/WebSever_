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

#include "../HttpConn.h"


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
    
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int connFd = accept(listenFd, (struct sockaddr*)&clientAddr, &addrLen);
    HttpConn httpConn;
    httpConn.init(connFd, clientAddr);
    int saveErrno = -1;
    httpConn.read(&saveErrno);
    httpConn.process();
    httpConn.write(&saveErrno);
    return 0;
}