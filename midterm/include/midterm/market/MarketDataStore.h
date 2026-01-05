#pragma once

#include <string>
#include <vector>

#include "midterm/core/Models.h"

namespace midterm::market {

class MarketDataStore
{
public:
    explicit MarketDataStore(const std::string& primary, const std::string& fallback = "");

    bool empty() const;

    std::vector<std::string> products() const;
    bool hasProduct(const std::string& product) const;

    std::vector<Order> filter(const std::string& product, OrderSide side) const;
    double averagePrice(const std::string& product, OrderSide side) const;

private:
    std::vector<Order> orders_;

    void seedFallback();
    void load(const std::string& file);
};

} // namespace midterm::market

