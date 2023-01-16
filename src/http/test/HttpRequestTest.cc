#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include "../HttpRequest.h"

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

    char buf[4096];
    ret = read(connFd, buf, 4096 - 1);
    
    std::string packets(buf, strlen(buf)); 
    web::HttpRequest req(packets);
    req.parse();
    req.printPackets();

    std::string resp_line;
    resp_line += "HTTP/1.1 200 OK\r\n";

    std::string resp_body;
    resp_body += "<html><title>Error</title>";
    resp_body += "<body bgcolor=\"ffffff\">";
    resp_body += "<p> good </p>";
    resp_body += "<hr><em>TinyWebServer</em></body></html>";

    std::string resp_header;
    resp_header += "Connection: keep-alive\r\n";
    resp_header += "keep-alive: max=6, timeout=120\r\n";
    resp_header += "Content-type: text/html\r\n";
    resp_header += "Content-length: " + std::to_string(resp_body.size()) + "\r\n\r\n";

    std::string resp = resp_line + resp_header + resp_body;
    write(connFd, resp.c_str(), strlen(resp.c_str()));    
    return 0;
}