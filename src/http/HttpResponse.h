#ifndef SRC_HTTP_HTTPRESPONSE_H
#define SRC_HTTP_HTTPRESPONSE_H

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap

namespace web
{

class HttpResponse
{
private:
    void addResponseLine();
    void addResponseHeader();
    void addResponseBody();

    int code_;
    bool isKeepAlive_;

    std::string path_;
    std::string srcDir_;
    
    char* mmFile_; 
    struct stat mmFileStat_;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
public:
    HttpResponse();
    ~HttpResponse() = default;
    void init(const std::string& srcDir, const std::string& path, bool isKeepAlive, int code);
    errorContent();

};

}// namespace web

#endif // !SRC_HTTP_HTTPRESPONSE_H