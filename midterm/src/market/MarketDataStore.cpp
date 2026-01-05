#include "midterm/market/MarketDataStore.h"

#include "midterm/utils/StringUtil.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

namespace midterm::market {

MarketDataStore::MarketDataStore(const std::string& primary, const std::string& fallback)
{
    if (!primary.empty()) load(primary);
    if (orders_.empty() && !fallback.empty()) load(fallback);
    if (orders_.empty()) seedFallback();
}

bool MarketDataStore::empty() const
{
    return orders_.empty();
}

std::vector<std::string> MarketDataStore::products() const
{
    std::set<std::string> uniq;
    for (const auto& o : orders_) uniq.insert(o.product);
    return std::vector<std::string>(uniq.begin(), uniq.end());
}

bool MarketDataStore::hasProduct(const std::string& product) const
{
    return std::any_of(orders_.begin(), orders_.end(), [&](const Order& o) { return o.product == product; });
}

std::vector<Order> MarketDataStore::filter(const std::string& product, OrderSide side) const
{
    std::vector<Order> out;
    for (const auto& o : orders_)
    {
        if (o.product == product && o.side == side) out.push_back(o);
    }
    return out;
}

double MarketDataStore::averagePrice(const std::string& product, OrderSide side) const
{
    double total = 0.0;
    size_t count = 0;
    for (const auto& o : orders_)
    {
        if (o.product == product && o.side == side)
        {
            total += o.price;
            ++count;
        }
    }
    return count == 0 ? 0.0 : total / static_cast<double>(count);
}

void MarketDataStore::seedFallback()
{
    std::cout << "[info] Using tiny built-in market dataset (market.csv not found).\n";
    orders_.push_back({"2020-01-01 09:00:00", "ETH/USDT", OrderSide::Bid, 1350.0, 0.5});
    orders_.push_back({"2020-01-01 09:05:00", "ETH/USDT", OrderSide::Ask, 1355.0, 0.4});
    orders_.push_back({"2020-01-02 10:00:00", "BTC/USDT", OrderSide::Bid, 19500.0, 0.01});
    orders_.push_back({"2020-01-02 10:05:00", "BTC/USDT", OrderSide::Ask, 19550.0, 0.008});
    orders_.push_back({"2020-01-03 11:00:00", "ETH/USDT", OrderSide::Bid, 1375.0, 0.3});
    orders_.push_back({"2020-01-03 11:05:00", "ETH/USDT", OrderSide::Ask, 1380.0, 0.25});
}

void MarketDataStore::load(const std::string& file)
{
    std::ifstream in(file);
    if (!in) return;

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string ts, product, sideStr, priceStr, amountStr;
        if (!std::getline(ss, ts, ',')) continue;
        if (!std::getline(ss, product, ',')) continue;
        if (!std::getline(ss, sideStr, ',')) continue;
        if (!std::getline(ss, priceStr, ',')) continue;
        if (!std::getline(ss, amountStr, ',')) continue;

        OrderSide side{};
        if (!parseSide(sideStr, side)) continue;

        try
        {
            Order o;
            o.timestamp = trim(ts);
            o.product   = trim(product);
            o.side      = side;
            o.price     = std::stod(priceStr);
            o.amount    = std::stod(amountStr);
            orders_.push_back(o);
        }
        catch (...)
        {
            continue;
        }
    }

    if (!orders_.empty())
    {
        std::cout << "[info] Loaded market data from " << file << " (" << orders_.size() << " rows).\n";
    }
}

} // namespace midterm::market

