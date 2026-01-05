#include "midterm/services/WalletService.h"

#include "midterm/utils/StringUtil.h"

#include <fstream>
#include <sstream>

namespace midterm::services {

WalletService::WalletService(const std::string& file) : file_(file)
{
    load();
}

double WalletService::getBalance(const std::string& username) const
{
    auto it = balances_.find(username);
    if (it == balances_.end()) return 0.0;
    return it->second;
}

bool WalletService::deposit(const std::string& username, double amount)
{
    if (amount <= 0) return false;
    balances_[username] += amount;
    persist();
    return true;
}

bool WalletService::withdraw(const std::string& username, double amount)
{
    if (amount <= 0) return false;
    double bal = getBalance(username);
    if (bal + 1e-9 < amount) return false;
    balances_[username] = bal - amount;
    persist();
    return true;
}

void WalletService::load()
{
    std::ifstream in(file_);
    if (!in) return;

    std::string line;
    bool first = true;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        if (first && line.find("username") != std::string::npos)
        {
            first = false;
            continue;
        }
        first = false;

        std::stringstream ss(line);
        std::string username, balanceStr;
        if (!std::getline(ss, username, ',')) continue;
        if (!std::getline(ss, balanceStr, ',')) continue;
        try
        {
            balances_[trim(username)] = std::stod(balanceStr);
        }
        catch (...)
        {
            continue;
        }
    }
}

void WalletService::persist() const
{
    std::ofstream out(file_);
    out << "username,balance\n";
    for (const auto& kv : balances_)
    {
        out << kv.first << ',' << kv.second << '\n';
    }
}

} // namespace midterm::services

