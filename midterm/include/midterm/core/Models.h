#pragma once

#include <string>

namespace midterm {

enum class OrderSide
{
    Ask,
    Bid
};

std::string sideToString(OrderSide side);
bool parseSide(const std::string& s, OrderSide& out);

struct Order
{
    std::string timestamp;
    std::string product;
    OrderSide side;
    double price{};
    double amount{};
};

struct User
{
    std::string username;
    std::string fullName;
    std::string email;
    std::string passwordHash;
};

struct Trade
{
    std::string timestamp;
    std::string username;
    std::string product;
    OrderSide side;
    double price{};
    double amount{};
    double total{};
};

struct Candle
{
    std::string period;
    double open{};
    double high{};
    double low{};
    double close{};
    double volume{};
};

// A simple session model for "who is logged in".
struct Session
{
    bool loggedIn = false;
    std::string username;  // 10-digit id in spec
    std::string fullName;
    std::string email;
};

} // namespace midterm

