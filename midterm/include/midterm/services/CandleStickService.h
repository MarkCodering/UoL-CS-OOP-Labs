#pragma once

#include <string>
#include <vector>

#include "midterm/core/Models.h"

namespace midterm::market {
class MarketDataStore;
}

namespace midterm::services {

class CandleStickService
{
public:
    explicit CandleStickService(const market::MarketDataStore& market);

    std::vector<Candle> compute(const std::string& product, OrderSide side, int granularity) const;
    void printTable(const std::vector<Candle>& candles) const;

private:
    const market::MarketDataStore& market_;
};

} // namespace midterm::services

