#include "midterm/services/TradeService.h"

#include "midterm/market/MarketDataStore.h"
#include "midterm/services/WalletService.h"
#include "midterm/utils/DateUtil.h"
#include "midterm/utils/StringUtil.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

namespace midterm::services {

TradeService::TradeService(const std::string& file, const market::MarketDataStore& market, WalletService& wallet)
    : file_(file), market_(market), wallet_(wallet)
{
    load();
}

bool TradeService::recordTrade(const std::string& username, const std::string& product, OrderSide side,
                               double price, double amount, const std::string& ts)
{
    if (price <= 0 || amount <= 0) return false;
    if (!market_.hasProduct(product)) return false;

    double total = price * amount;
    if (side == OrderSide::Bid)
    {
        if (!wallet_.withdraw(username, total))
        {
            std::cout << "[warn] Insufficient funds for bid: need " << total << "\n";
            return false;
        }
    }
    else
    {
        wallet_.deposit(username, total);
    }

    trades_.push_back({ts, username, product, side, price, amount, total});
    persist();
    return true;
}

bool TradeService::recordTrade(const std::string& username, const std::string& product, OrderSide side,
                               double price, double amount)
{
    return recordTrade(username, product, side, price, amount, nowTimestamp());
}

std::vector<Trade> TradeService::getRecent(const std::string& username, size_t count, const std::string& productFilter) const
{
    std::vector<Trade> out;
    for (auto it = trades_.rbegin(); it != trades_.rend(); ++it)
    {
        if (it->username != username) continue;
        if (!productFilter.empty() && it->product != productFilter) continue;
        out.push_back(*it);
        if (out.size() == count) break;
    }
    std::reverse(out.begin(), out.end());
    return out;
}

void TradeService::printRecent(const std::vector<Trade>& trades) const
{
    if (trades.empty())
    {
        std::cout << "No transactions to show.\n";
        return;
    }

    std::cout << std::left << std::setw(20) << "Timestamp"
              << std::setw(12) << "Product"
              << std::setw(6) << "Side"
              << std::setw(10) << "Price"
              << std::setw(10) << "Amount"
              << std::setw(12) << "Total" << '\n';
    for (const auto& t : trades)
    {
        std::cout << std::left << std::setw(20) << t.timestamp
                  << std::setw(12) << t.product
                  << std::setw(6) << sideToString(t.side)
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2) << t.price
                  << std::setw(10) << t.amount
                  << std::setw(12) << t.total << '\n';
    }
}

void TradeService::printOverallCounts(const std::string& username) const
{
    size_t asks = 0, bids = 0;
    for (const auto& t : trades_)
    {
        if (t.username != username) continue;
        if (t.side == OrderSide::Ask) ++asks; else ++bids;
    }
    std::cout << "Asks: " << asks << ", Bids: " << bids << '\n';
}

void TradeService::printCountsForProduct(const std::string& username, const std::string& product) const
{
    size_t asks = 0, bids = 0;
    for (const auto& t : trades_)
    {
        if (t.username != username || t.product != product) continue;
        if (t.side == OrderSide::Ask) ++asks; else ++bids;
    }
    std::cout << product << " -> Asks: " << asks << ", Bids: " << bids << '\n';
}

void TradeService::printSpendInRange(const std::string& username, const std::string& start, const std::string& end) const
{
    double spend = 0.0;
    for (const auto& t : trades_)
    {
        if (t.username != username) continue;
        if (t.side != OrderSide::Bid) continue;
        if (dateInRange(t.timestamp, start, end)) spend += t.total;
    }
    std::cout << "Total spent on bids between " << start << " and " << end << ": "
              << std::fixed << std::setprecision(2) << spend << '\n';
}

void TradeService::simulateForAllProducts(const std::string& username)
{
    auto products = market_.products();
    if (products.empty())
    {
        std::cout << "No market data available to simulate trades.\n";
        return;
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> amt(0.1, 1.0);
    std::uniform_real_distribution<double> jitter(0.98, 1.02);

    size_t created = 0;
    for (const auto& product : products)
    {
        double baseBid = market_.averagePrice(product, OrderSide::Bid);
        double baseAsk = market_.averagePrice(product, OrderSide::Ask);
        if (baseBid <= 0) baseBid = baseAsk > 0 ? baseAsk * 0.99 : 100.0;
        if (baseAsk <= 0) baseAsk = baseBid * 1.01;

        for (int i = 0; i < 5; ++i)
        {
            double priceBid  = baseBid * jitter(rng);
            double amountBid = amt(rng);
            if (recordTrade(username, product, OrderSide::Bid, priceBid, amountBid)) ++created;

            double priceAsk  = baseAsk * jitter(rng);
            double amountAsk = amt(rng);
            if (recordTrade(username, product, OrderSide::Ask, priceAsk, amountAsk)) ++created;
        }
    }

    std::cout << "Simulated " << created << " trades across " << products.size() << " products.\n";
}

void TradeService::load()
{
    std::ifstream in(file_);
    if (!in) return;

    std::string line;
    bool first = true;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        if (first && line.find("timestamp") != std::string::npos)
        {
            first = false;
            continue;
        }
        first = false;

        std::stringstream ss(line);
        std::string ts, username, product, sideStr, priceStr, amountStr, totalStr;
        if (!std::getline(ss, ts, ',')) continue;
        if (!std::getline(ss, username, ',')) continue;
        if (!std::getline(ss, product, ',')) continue;
        if (!std::getline(ss, sideStr, ',')) continue;
        if (!std::getline(ss, priceStr, ',')) continue;
        if (!std::getline(ss, amountStr, ',')) continue;
        if (!std::getline(ss, totalStr, ',')) totalStr = "0";

        OrderSide side{};
        if (!parseSide(sideStr, side)) continue;

        try
        {
            Trade t;
            t.timestamp = trim(ts);
            t.username  = trim(username);
            t.product   = trim(product);
            t.side      = side;
            t.price     = std::stod(priceStr);
            t.amount    = std::stod(amountStr);
            t.total     = std::stod(totalStr);
            trades_.push_back(t);
        }
        catch (...)
        {
            continue;
        }
    }
}

void TradeService::persist() const
{
    std::ofstream out(file_);
    out << "timestamp,username,product,side,price,amount,total\n";
    for (const auto& t : trades_)
    {
        out << t.timestamp << ',' << t.username << ',' << t.product << ',' << sideToString(t.side) << ','
            << t.price << ',' << t.amount << ',' << t.total << '\n';
    }
}

} // namespace midterm::services

