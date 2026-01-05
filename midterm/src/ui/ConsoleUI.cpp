#include "midterm/ui/ConsoleUI.h"

#include <iostream>
#include <limits>

namespace midterm::ui {

void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readIntInRange(const std::string& prompt, int lo, int hi)
{
    while (true)
    {
        std::cout << prompt;
        int x;
        if (std::cin >> x)
        {
            clearInput();
            if (x >= lo && x <= hi) return x;
        }
        else
        {
            clearInput();
        }
        std::cout << "Invalid input. Enter a number in [" << lo << ", " << hi << "].\n";
    }
}

double readDouble(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;
        double x;
        if (std::cin >> x)
        {
            clearInput();
            return x;
        }
        clearInput();
        std::cout << "Invalid input. Enter a numeric value.\n";
    }
}

std::string readLineNonEmpty(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        if (!s.empty()) return s;
        std::cout << "Input cannot be empty.\n";
    }
}

void printDivider()
{
    std::cout << "------------------------------------------------------------\n";
}

} // namespace midterm::ui

