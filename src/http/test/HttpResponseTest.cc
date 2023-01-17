#include "../../socket/InetAddr.h"
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
using namespace std;
using namespace web;

int main()
{
     
    struct stat mmFileStat_;
    const string path = "/root/WebSever_/resources/index.html";
    int srcFd = open(path.c_str(), O_RDONLY);
    stat(path.data(), &mmFileStat_);
    char* mmFile_ = (char*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    cout << *mmFile_ << endl;
    close(srcFd);
    string resp_body(mmFile_, mmFileStat_.st_size);
    // // 判断文件类型
    // if(S_ISREG(mmFileStat_.st_mode))
    //     printf("%s is a regular file\n", path.c_str());
    // if(S_ISDIR(mmFileStat_.st_mode))
    //     printf("%s is a diretory\n", path.c_str());
    // if(S_ISFIFO(mmFileStat_.st_mode))
    //     printf("%s is a pipe\n", path.c_str());
    // if(S_ISFIFO(mmFileStat_.st_mode))
    //     printf("%s is a pipe\n", path.c_str());
    // if(S_ISSOCK(mmFileStat_.st_mode))
    //     printf("%s is a socket\n", path.c_str());

    // string str(10, '-');
    // // 判断文件权限
    // //本用户的文件权限
    // if(mmFileStat_.st_mode & S_IRUSR) str[1] = 'r';
    // if(mmFileStat_.st_mode & S_IWUSR) str[2] = 'w';
    // if(mmFileStat_.st_mode & S_IXUSR) str[3] = 'x';
    
    // //本用户的组的文件权限
    // if(mmFileStat_.st_mode & S_IRGRP) str[4] = 'r';
    // if(mmFileStat_.st_mode & S_IWGRP) str[5] = 'w';
    // if(mmFileStat_.st_mode & S_IXGRP) str[6] = 'x';
    
    // //其他用户的文件权限
    // if(mmFileStat_.st_mode & S_IROTH) str[7] = 'r';
    // if(mmFileStat_.st_mode & S_IWOTH) str[8] = 'w';
    // if(mmFileStat_.st_mode & S_IXOTH) str[9] = 'x';
    
    // printf("%s\n", str.c_str());
    // printf("%s\n", mmFile_);

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

    cout << mmFileStat_.st_size << endl;

    std::string resp_line = "HTTP/1.1 200 OK\r\n";

    // std::string resp_body;
    
    

    std::string resp_header;
    resp_header += "Connection: keep-alive\r\n";
    resp_header += "keep-alive: max=6, timeout=120\r\n";
    resp_header += "Content-type: text/html\r\n";
    resp_header += "Content-length: " + std::to_string(resp_body.size()) + "\r\n\r\n";

    std::string resp = resp_line + resp_header + resp_body;
    write(connFd, resp.c_str(), strlen(resp.c_str()));    
    munmap(mmFile_, mmFileStat_.st_size);
    return 0;
}