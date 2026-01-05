#pragma once

#include <string>

namespace midterm {

bool fileExists(const std::string& path);
std::string joinPath(const std::string& base, const std::string& leaf);
std::string dataPath(const std::string& filename);

} // namespace midterm

