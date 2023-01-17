#include "./Buffer.h"
#include "Buffer.h"


web::Buffer::Buffer(int initSize)
    : buffer_(initSize), readPos_(0), writePos_(0)
{}

void web::Buffer::append(web::Buffer& buf)
{
    append(buf.beginPos(), buf.readableBytes());
}

void web::Buffer::append(const std::string& str)
{
    append(str.data(), str.length());
}

void web::Buffer::append(const char* str, size_t len)
{
    std::copy(str, str + len, writePos());
    writePos_ += len;
}

void web::Buffer::ensureWrite(size_t len)
{
    if(writeableBytes() < len)
    {
        makeSpace(len);
    }
}

void web::Buffer::makeSpace(size_t len)
{
    if(prependableBytes() + writeableBytes() < len)
    {
        buffer_.resize(writePos_ + len + 1);
    }
    else
    {
        std::copy(readPos(), writePos(), beginPos());
        writePos_ -= readPos_;
        readPos_ = 0;
    }
}

void web::Buffer::retrieve(size_t len)
{
    readPos_ += len;
}

void web::Buffer::retrieveAll()
{
    readPos_ = 0;
    writePos_ = 0;
}

std::string web::Buffer::retrieveAllToStr()
{
    std::string str(readPos(), readableBytes());
    retrieveAll();
    return str;
}

ssize_t web::Buffer::writeFd(int fd, int *saveErrno)
{
    ssize_t len = write(fd, readPos(), readableBytes());
    if(len < 0)
    {
        *saveErrno = errno;
        return len;
    }
    retrieve(len);
    return len;
}

ssize_t web::Buffer::readFd(int fd, int *saveErrno)
{
    char buff[65535];
    struct iovec iov[2];

    const size_t writable = writeableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = writePos();
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        writePos_ += len;
    }
    else {
        writePos_ = buffer_.size();
        append(buff, len - writable);
    }
    return len;
}

void web::Buffer::retrieveUntil(const char* end)
{
    retrieve(end - readPos());
}