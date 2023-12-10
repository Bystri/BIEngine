#pragma once

#include <string>

namespace BIEngine {

template <typename... Args>
extern std::string Format(const std::string& format, Args... args);

extern bool WildcardMatch(const char* pat, const char* str);

} // namespace BIEngine