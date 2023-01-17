#ifndef SRC_SOCKET_INETADDR_H
#define SRC_SOCKET_INETADDR_H

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace web
{

class InetAddr
{
private:
    struct sockaddr_in addr_;
public:
    InetAddr(uint16_t port = 0);
    
    InetAddr(std::string ip, uint16_t port);
    
    explicit InetAddr(const struct sockaddr_in& addr) 
    : addr_(addr)
    {}

    ~InetAddr() = default;

    struct sockaddr_in* addr() { return &addr_; }
};

}; // namespace web
#endif // !SRC_SOCKET_INETADDR_H