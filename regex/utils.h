#ifndef NXS_REGEX_UTILS_H
#define NXS_REGEX_UTILS_H

#include <codecvt>
#include <locale>
#include <string>

namespace nxs::regex {

std::string To_UTF8(const std::u32string& s);
std::u32string To_UTF32(const std::string& s);
} // namespace nxs::regex

#endif
