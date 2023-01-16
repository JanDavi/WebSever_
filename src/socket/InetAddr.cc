#include "./InetAddr.h"
#include "InetAddr.h"

using namespace web;

InetAddr::InetAddr(uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port);
}

InetAddr::InetAddr(std::string ip, uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &(addr_.sin_addr));
    addr_.sin_port = htons(port);
}
