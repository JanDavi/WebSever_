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

void web::Buffer::retrieveUntil(const char* end)
{
    retrieve(end - readPos());
}