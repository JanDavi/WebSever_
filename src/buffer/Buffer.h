#ifndef SRC_BUFFER_H
#define SRC_BUFFER_H

#include <vector>
#include <iostream>
#include <atomic>
#include <sys/uio.h> //readv
#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>

namespace web
{

class Buffer
{
private:    
    void ensureWrite(size_t len);
    void makeSpace(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;

public:
    Buffer(int initSize = 1024);
    ~Buffer() = default;
    
    char* readPos() { return beginPos() + readPos_; }
    char* writePos() { return beginPos() + writePos_; }
    char* beginPos() { return &*buffer_.begin(); }

    const char* readPos() const { return beginPos() + readPos_; }
    const char* writePos() const { return beginPos() + writePos_; }
    const char* beginPos() const { return &*buffer_.begin(); }

    size_t readableBytes() const { return writePos_ - readPos_; }
    size_t writeableBytes() const { return static_cast<size_t>(buffer_.size()) - writePos_; }
    size_t prependableBytes() const { return readPos_; }
    
    void append(Buffer& buf);
    void append(const std::string& str);
    void append(const char* str, size_t len);

    void retrieve(size_t len);
    void retrieveAll();
    void retrieveUntil(const char* end);
    std::string retrieveAllToStr();

    ssize_t writeFd(int fd, int* saveErrno);
    ssize_t readFd(int fd, int* saveErrno);
};

};

#endif // !SRC_BUFFER_H
