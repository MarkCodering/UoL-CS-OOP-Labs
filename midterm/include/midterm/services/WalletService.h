#pragma once

#include <map>
#include <string>

namespace midterm::services {

class WalletService
{
public:
    explicit WalletService(const std::string& file);

    double getBalance(const std::string& username) const;
    bool deposit(const std::string& username, double amount);
    bool withdraw(const std::string& username, double amount);

private:
    std::string file_;
    std::map<std::string, double> balances_;

    void load();
    void persist() const;
};

} // namespace midterm::services

