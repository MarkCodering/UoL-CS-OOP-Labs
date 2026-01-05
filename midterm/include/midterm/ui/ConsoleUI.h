#pragma once

#include <string>

namespace midterm::ui {

void clearInput();
int readIntInRange(const std::string& prompt, int lo, int hi);
double readDouble(const std::string& prompt);
std::string readLineNonEmpty(const std::string& prompt);
void printDivider();

} // namespace midterm::ui

