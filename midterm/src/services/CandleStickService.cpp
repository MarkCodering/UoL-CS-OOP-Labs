#include "midterm/services/CandleStickService.h"

#include "midterm/market/MarketDataStore.h"
#include "midterm/utils/DateUtil.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>

namespace midterm::services {

CandleStickService::CandleStickService(const market::MarketDataStore& market) : market_(market) {}

std::vector<Candle> CandleStickService::compute(const std::string& product, OrderSide side, int granularity) const
{
    auto orders = market_.filter(product, side);
    std::sort(orders.begin(), orders.end(), [](const Order& a, const Order& b) { return a.timestamp < b.timestamp; });

    struct Builder
    {
        bool initialized = false;
        double open = 0, high = 0, low = 0, close = 0, volume = 0;
    };

    std::map<std::string, Builder> buckets;
    for (const auto& o : orders)
    {
        std::string key = bucketKey(o.timestamp, granularity);
        auto& b = buckets[key];
        if (!b.initialized)
        {
            b.initialized = true;
            b.open = b.high = b.low = b.close = o.price;
        }
        b.close = o.price;
        b.high  = std::max(b.high, o.price);
        b.low   = std::min(b.low, o.price);
        b.volume += o.amount;
    }

    std::vector<Candle> out;
    for (const auto& kv : buckets)
    {
        const std::string& key = kv.first;
        const Builder& b       = kv.second;
        out.push_back({key, b.open, b.high, b.low, b.close, b.volume});
    }
    return out;
}

void CandleStickService::printTable(const std::vector<Candle>& candles) const
{
    if (candles.empty())
    {
        std::cout << "No data available for that selection.\n";
        return;
    }

    std::cout << std::left << std::setw(12) << "Period"
              << std::right << std::setw(10) << "Open"
              << std::setw(10) << "High"
              << std::setw(10) << "Low"
              << std::setw(10) << "Close"
              << std::setw(12) << "Volume" << '\n';
    for (const auto& c : candles)
    {
        std::cout << std::left << std::setw(12) << c.period
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2) << c.open
                  << std::setw(10) << c.high
                  << std::setw(10) << c.low
                  << std::setw(10) << c.close
                  << std::setw(12) << std::setprecision(4) << c.volume << '\n';
    }
}

} // namespace midterm::services

