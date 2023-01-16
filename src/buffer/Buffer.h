#ifndef SRC_BUFFER_H
#define SRC_BUFFER_H

#include <vector>
#include <iostream>
#include <atomic>
#include <sys/uio.h> //readv

namespace web
{

class Buffer
{
private:
    char* readPos() { return beginPos() + readPos_; }
    char* writePos() { return beginPos() + writePos_; }
    char* beginPos() { return &buffer_[0]; }
    void ensureWrite(size_t len);
    void makeSpace(size_t len);
    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;

public:
    Buffer(int initSize = 1024);
    ~Buffer() = default;

    size_t readableBytes() const { return writePos_ - readPos_; }
    size_t writeableBytes() const { return static_cast<size_t>buffer_.size() - writePos_; }
    size_t prependableBytes() const { return readPos_; }
    void append(const Buffer& buf);
    void append(const std::string& str);
    void append(const char* str, size_t len);
};

};

#endif // !SRC_BUFFER_H
