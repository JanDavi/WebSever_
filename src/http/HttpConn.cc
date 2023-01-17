#include "./HttpConn.h"
#include "HttpConn.h"

const char* web::HttpConn::srcDir = "/root/WebSever_/resources";
std::atomic<int> web::HttpConn::userCount;
bool web::HttpConn::isET = false;

// 构造函数
web::HttpConn::HttpConn()
{
    fd_ = -1;
    isClose_ = true;
    addr_ = { 0 };
}

// 析构函数
web::HttpConn::~HttpConn()
{
    Close();
}

// 初始化缓冲区、socket
void web::HttpConn::init(int sockfd, const sockaddr_in& addr)
{
    printf("web::HttpConn::init\n");
    fd_ = sockfd;
    addr_ = addr;
    userCount++;
    writeBuf_.retrieveAll();
    readBuf_.retrieveAll();
    isClose_ = false;
}

// 从fd读数据，往readBuf_放 , 等待HttpRequest解析
// (LT：读一次，ET读到没有数据可以读为止)
ssize_t web::HttpConn::read(int *saveErrno)
{
    printf("web::HttpConn::read\n");
    ssize_t len = -1;
    do
    {
        len = readBuf_.readFd(fd_, saveErrno);
        if(len <= 0)
        {
            break;
        }   
    }while(isET);
    printf("end:web::HttpConn::read\n");
    return len;
}

// 将iovec向量的数据和writeBufe_的数据向fd写入
ssize_t web::HttpConn::write(int *saveErrno)
{
    printf("web::HttpConn::write\n");
    ssize_t len = -1;
    do
    {
        len = writev(fd_, iov_, iovCnt_);
        if(len <= 0) // writev出错
        {
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len + iov_[1].iov_len == 0) // 数据已经写完
        {
            break;
        }
        else if(static_cast<size_t>(len) > iov_[0].iov_len) 
        {
            // iov_[0]已经全部写入，iov_[1]部分写入
            // 调整iov向量
            iov_[1].iov_base = (uint8_t*)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len); 
            if(iov_[0].iov_len)
            {
                writeBuf_.retrieveAll(); // 因为iov_[0]是放在writeBuf_,所以记得要清空
                iov_[0].iov_len = 0;
            }
        }
        else
        {
            // iov_[0]已经部分写入，iov_[1]全部都没写入
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            writeBuf_.retrieve(len); // 移动写缓存指针
        }       
    }while(isET || toWriteBytes() > 10240); // 当数据太多或者是ET模式是，需要一直写
    printf("end:web::HttpConn::write\n");
    return len;
}

// 关闭http连接，释放当前连接占有的资源
void web::HttpConn::Close()
{   
    printf("web::HttpConn::Close\n");
    response_.unmapFile();
    if(!isClose_)
    {
        isClose_ = true;
        userCount--;
        close(fd_);
    }
}

// 返回当前连接的fd，从该fd进行读写
int web::HttpConn::getFd() const
{
    return fd_;
}

// 返回当前连接（客户端）端口
int web::HttpConn::getPort() const
{
    return addr_.sin_port;
}

// 返回当前连接（客户端）IP的点分十进制
const char *web::HttpConn::getIP() const
{
    return inet_ntoa(addr_.sin_addr);
}

// 返回地址
struct sockaddr_in web::HttpConn::getAddr() const
{
    return addr_;
}


// 调用HttpRequest解析HTTP请求报文
// 调用HttpResponse构造HTTP响应报文和获取文件，
// 响应报文最终放到iovec向量，用writev进行发送
bool web::HttpConn::process()
{
    printf("web::HttpConn::process\n");
    request_.init();
    if(readBuf_.readableBytes() <= 0) // readBuf_没有数据可以读
    {
        return false;
    }
    else if(request_.parse(readBuf_)) // 解析请求报文成功
    {
        response_.init(srcDir, request_.path(), request_.isKeepAlive(), 200);
    }
    else // 请求报文解析出错，构造Bad Request响应报文
    {
        response_.init(srcDir, request_.path(), false, 400);
    }

    // 将响应报文写到writeBuf_
    response_.makeResponse(writeBuf_);
    
    // 状态行和响应头放到iov_[0]
    iov_[0].iov_base = static_cast<char*>(writeBuf_.readPos());
    iov_[0].iov_len = writeBuf_.readableBytes();

    // 如果有文件，则将文件放到iov_[1]
    if(response_.fileLen() > 0 && response_.file() != nullptr)
    {
        iov_[1].iov_base = response_.file();
        iov_[1].iov_len = response_.fileLen();
        iovCnt_ = 2;
    }
    printf("end:web::HttpConn::process\n");
    return true;
}
