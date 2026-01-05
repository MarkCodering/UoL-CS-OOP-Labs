#include "midterm/utils/StringUtil.h"

#include <cctype>

namespace midterm {

std::string ltrim(const std::string& s)
{
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(i);
}

std::string rtrim(const std::string& s)
{
    if (s.empty()) return s;
    size_t i = s.size() - 1;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
    {
        if (i == 0) return "";
        --i;
    }
    return s.substr(0, i + 1);
}

std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}

std::string toLower(std::string s)
{
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

} // namespace midterm

