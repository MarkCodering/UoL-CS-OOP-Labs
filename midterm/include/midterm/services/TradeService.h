#pragma once

#include <string>
#include <vector>

#include "midterm/core/Models.h"

namespace midterm::market {
class MarketDataStore;
}

namespace midterm::services {
class WalletService;
}

namespace midterm::services {

class TradeService
{
public:
    TradeService(const std::string& file, const market::MarketDataStore& market, WalletService& wallet);

    bool recordTrade(const std::string& username, const std::string& product, OrderSide side,
                     double price, double amount, const std::string& ts);

    bool recordTrade(const std::string& username, const std::string& product, OrderSide side, double price, double amount);

    std::vector<Trade> getRecent(const std::string& username, size_t count, const std::string& productFilter = "") const;
    void printRecent(const std::vector<Trade>& trades) const;

    void printOverallCounts(const std::string& username) const;
    void printCountsForProduct(const std::string& username, const std::string& product) const;
    void printSpendInRange(const std::string& username, const std::string& start, const std::string& end) const;

    void simulateForAllProducts(const std::string& username);

private:
    std::string file_;
    const market::MarketDataStore& market_;
    WalletService& wallet_;
    std::vector<Trade> trades_;

    void load();
    void persist() const;
};

} // namespace midterm::services

