#include <iostream>
#include <vector>
#include <string>
#include <limits>

enum class OrderBookType { bid, ask };

class OrderBookEntry
{
public:
    std::string timestamp;
    std::string product;
    OrderBookType orderType;
    double price;
    double amount;

    // Constructor using member initializer list
    OrderBookEntry(std::string _timestamp,
                   std::string _product,
                   OrderBookType _orderType,
                   double _price,
                   double _amount)
        : timestamp(_timestamp),
          product(_product),
          orderType(_orderType),
          price(_price),
          amount(_amount)
    {
    }
};

// ------------------ Challenge Functions ------------------

double computeAveragePrice(std::vector<OrderBookEntry>& entries)
{
    double sum = 0;
    for (const OrderBookEntry& e : entries)
    {
        sum += e.price;
    }
    return sum / entries.size();
}

double computeLowPrice(std::vector<OrderBookEntry>& entries)
{
    double low = std::numeric_limits<double>::max();
    for (const OrderBookEntry& e : entries)
    {
        if (e.price < low)
            low = e.price;
    }
    return low;
}

double computeHighPrice(std::vector<OrderBookEntry>& entries)
{
    double high = std::numeric_limits<double>::lowest();
    for (const OrderBookEntry& e : entries)
    {
        if (e.price > high)
            high = e.price;
    }
    return high;
}

double computePriceSpread(std::vector<OrderBookEntry>& entries)
{
    return computeHighPrice(entries) - computeLowPrice(entries);
}

// ------------------ Main Program ------------------

int main()
{
    // Creating example OrderBookEntry objects
    OrderBookEntry obe1{
        "2020/03/17 17:01:24.884492",
        "ETH/BTC",
        OrderBookType::bid,
        0.02186299,
        0.1
    };

    OrderBookEntry obe2{
        "2020/03/17 17:01:25.000000",
        "ETH/BTC",
        OrderBookType::ask,
        0.02187308,
        7.44564869
    };

    // Store in vector
    std::vector<OrderBookEntry> entries;
    entries.push_back(obe1);
    entries.push_back(obe2);

    // Print all prices using const reference
    std::cout << "Prices in vector:\n";
    for (const OrderBookEntry& e : entries)
    {
        std::cout << e.price << std::endl;
    }

    // Use challenge functions
    std::cout << "\nAverage price: " << computeAveragePrice(entries) << "\n";
    std::cout << "Low price: " << computeLowPrice(entries) << "\n";
    std::cout << "High price: " << computeHighPrice(entries) << "\n";
    std::cout << "Spread: " << computePriceSpread(entries) << "\n";

    return 0;
}
