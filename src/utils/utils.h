#ifndef SRC_UTILS_H
#define SRC_UTILS_H

#include <vector>
#include <string>

namespace web
{
namespace utils
{

std::vector<std::string> split(const std::string& str, const std::string& pattern);


}; // namespace utils   
}; // namespace web

#endif // !SRC_UTILS_H