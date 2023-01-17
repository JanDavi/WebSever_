#ifndef SRC_HTTP_HTTPCONN_H
#define SRC_HTTP_HTTPCONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>   

#include "./HttpRequest.h"
#include "./HttpResponse.h"
#include "../buffer/Buffer.h"

namespace web
{

class HttpConn
{
private:
    int fd_;
    struct sockaddr_in addr_;
    bool isClose_;

    int iovCnt_;
    struct iovec iov_[2];

    web::Buffer readBuf_;
    web::Buffer writeBuf_;

    web::HttpRequest request_;
    web::HttpResponse response_;

public:
    HttpConn();
    ~HttpConn();
    void init(int sockfd, const sockaddr_in& addr);
    ssize_t read(int* saveErrno);
    ssize_t write(int* saveErrno);
    void Close();
    int getFd() const;
    int getPort() const;
    const char* getIP() const;
    sockaddr_in getAddr() const;
    bool process();

    int toWriteBytes() { 
        return iov_[0].iov_len + iov_[1].iov_len; 
    }
    bool isKeepAlive() const {
        return request_.isKeepAlive();
    }

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;
};

}; // namespace web

#endif // !SRC_HTTP_HTTPCONN_H
