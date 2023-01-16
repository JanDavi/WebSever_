#include "./Buffer.h"
#include "Buffer.h"


web::Buffer::Buffer(int initSize)
    : buffer_(initSize), readPos_(0), writePos_(0)
{}

void web::Buffer::append(const char* str, size_t len)
{
    std::copy(str, str + len, writePos());
}

void web::Buffer::ensureWrite(size_t len)
{
    if(writeableBytes() <= len)
    {

    }
    else if(prependableBytes() + writeableBytes() <= len)
    {

    }
    else
    {
        
    }
}

