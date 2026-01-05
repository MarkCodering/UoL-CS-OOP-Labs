#include "midterm/utils/Paths.h"

#include <fstream>

namespace midterm {

bool fileExists(const std::string& path)
{
    std::ifstream in(path);
    return in.good();
}

std::string joinPath(const std::string& base, const std::string& leaf)
{
    if (base.empty()) return leaf;
    const char last = base.back();
    if (last == '/' || last == '\\') return base + leaf;
    return base + "/" + leaf;
}

std::string dataPath(const std::string& filename)
{
    // Prefer the midterm directory when running from repo root.
    if (fileExists(joinPath("midterm", "main.cpp")))
    {
        return joinPath("midterm", filename);
    }
    return filename;
}

} // namespace midterm

