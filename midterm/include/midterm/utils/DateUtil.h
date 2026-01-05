#pragma once

#include <string>

namespace midterm {

std::string nowTimestamp();
bool parseDateParts(const std::string& ts, int& y, int& m, int& d);
std::string bucketKey(const std::string& ts, int granularity);
bool dateInRange(const std::string& ts, const std::string& start, const std::string& end);

} // namespace midterm

