// main.cpp (scaffold - you must connect your own classes/methods)
// CM2005 OOP Midterm Coursework - Menu + Input Handling Skeleton

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

static bool fileExists(const std::string& path)
{
    std::ifstream in(path);
    return in.good();
}

static std::string joinPath(const std::string& base, const std::string& leaf)
{
    if (base.empty()) return leaf;
    const char last = base.back();
    if (last == '/' || last == '\\') return base + leaf;
    return base + "/" + leaf;
}

enum class OrderSide
{
    Ask,
    Bid
};

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

static std::string ltrim(const std::string& s)
{
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(i);
}

static std::string rtrim(const std::string& s)
{
    if (s.empty()) return s;
    size_t i = s.size() - 1;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
    {
        if (i == 0) return "";
        --i;
    }
    return s.substr(0, i + 1);
}

static std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}

static std::string toLower(std::string s)
{
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

static std::string sideToString(OrderSide s)
{
    return s == OrderSide::Ask ? "ask" : "bid";
}

static bool parseSide(const std::string& s, OrderSide& out)
{
    auto low = toLower(trim(s));
    if (low == "ask")
    {
        out = OrderSide::Ask;
        return true;
    }
    if (low == "bid")
    {
        out = OrderSide::Bid;
        return true;
    }
    return false;
}

static std::string nowTimestamp()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t   = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

static bool parseDateParts(const std::string& ts, int& y, int& m, int& d)
{
    // Handles YYYY/MM/DD and YYYY-MM-DD prefixes.
    char sep1 = 0, sep2 = 0;
    int yy = 0, mm = 0, dd = 0;
    if (std::sscanf(ts.c_str(), "%d%c%d%c%d", &yy, &sep1, &mm, &sep2, &dd) == 5)
    {
        if ((sep1 == '/' || sep1 == '-') && sep1 == sep2)
        {
            y = yy;
            m = mm;
            d = dd;
            return true;
        }
    }
    return false;
}

static std::string bucketKey(const std::string& ts, int granularity)
{
    int y = 0, m = 0, d = 0;
    if (!parseDateParts(ts, y, m, d)) return ts;

    std::ostringstream oss;
    if (granularity == 1)
    {
        oss << std::setfill('0') << std::setw(4) << y << '-' << std::setw(2) << m << '-' << std::setw(2) << d;
    }
    else if (granularity == 2)
    {
        oss << std::setfill('0') << std::setw(4) << y << '-' << std::setw(2) << m;
    }
    else
    {
        oss << std::setfill('0') << std::setw(4) << y;
    }
    return oss.str();
}

static bool dateInRange(const std::string& ts, const std::string& start, const std::string& end)
{
    int y = 0, m = 0, d = 0;
    int ys = 0, ms = 0, ds = 0;
    int ye = 0, me = 0, de = 0;
    if (!parseDateParts(ts, y, m, d)) return false;
    if (!parseDateParts(start, ys, ms, ds)) return false;
    if (!parseDateParts(end, ye, me, de)) return false;

    auto toInt = [](int yy, int mm, int dd) { return yy * 10000 + mm * 100 + dd; };
    int v  = toInt(y, m, d);
    int vs = toInt(ys, ms, ds);
    int ve = toInt(ye, me, de);
    return v >= vs && v <= ve;
}

static std::string dataPath(const std::string& filename)
{
    // Prefer the midterm directory when running from repo root.
    if (fileExists(joinPath("midterm", "main.cpp")))
    {
        return joinPath("midterm", filename);
    }
    return filename;
}

class MarketDataStore
{
public:
    explicit MarketDataStore(const std::string& primary, const std::string& fallback = "")
    {
        if (!primary.empty()) load(primary);
        if (orders_.empty() && !fallback.empty()) load(fallback);
        if (orders_.empty()) seedFallback();
    }

    bool empty() const { return orders_.empty(); }

    std::vector<std::string> products() const
    {
        std::set<std::string> uniq;
        for (const auto& o : orders_) uniq.insert(o.product);
        return std::vector<std::string>(uniq.begin(), uniq.end());
    }

    bool hasProduct(const std::string& product) const
    {
        return std::any_of(orders_.begin(), orders_.end(), [&](const Order& o) { return o.product == product; });
    }

    std::vector<Order> filter(const std::string& product, OrderSide side) const
    {
        std::vector<Order> out;
        for (const auto& o : orders_)
        {
            if (o.product == product && o.side == side) out.push_back(o);
        }
        return out;
    }

    double averagePrice(const std::string& product, OrderSide side) const
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

private:
    std::vector<Order> orders_;

    void seedFallback()
    {
        std::cout << "[info] Using tiny built-in market dataset (market.csv not found).\n";
        orders_.push_back({"2020-01-01 09:00:00", "ETH/USDT", OrderSide::Bid, 1350.0, 0.5});
        orders_.push_back({"2020-01-01 09:05:00", "ETH/USDT", OrderSide::Ask, 1355.0, 0.4});
        orders_.push_back({"2020-01-02 10:00:00", "BTC/USDT", OrderSide::Bid, 19500.0, 0.01});
        orders_.push_back({"2020-01-02 10:05:00", "BTC/USDT", OrderSide::Ask, 19550.0, 0.008});
        orders_.push_back({"2020-01-03 11:00:00", "ETH/USDT", OrderSide::Bid, 1375.0, 0.3});
        orders_.push_back({"2020-01-03 11:05:00", "ETH/USDT", OrderSide::Ask, 1380.0, 0.25});
    }

    void load(const std::string& file)
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
};

class UserService
{
public:
    explicit UserService(const std::string& file) : file_(file) { load(); }

    bool registerUser(const std::string& fullName, const std::string& email, const std::string& password, User& out)
    {
        User u;
        u.username = generateUsername();
        u.fullName = fullName;
        u.email    = email;
        u.passwordHash = hashPassword(password);

        if (users_.count(u.username)) return false;
        users_[u.username] = u;
        persist();
        out = u;
        return true;
    }

    bool login(const std::string& username, const std::string& password, User& out) const
    {
        auto it = users_.find(username);
        if (it == users_.end()) return false;
        if (it->second.passwordHash != hashPassword(password)) return false;
        out = it->second;
        return true;
    }

private:
    std::string file_;
    std::map<std::string, User> users_;

    static std::string hashPassword(const std::string& pw)
    {
        // Simple hash for demo purposes.
        return std::to_string(std::hash<std::string>{}(pw));
    }

    static std::string generateUsername()
    {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> digit(0, 9);
        std::string out(10, '0');
        for (char& c : out) c = static_cast<char>('0' + digit(rng));
        return out;
    }

    void load()
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
            std::string username, fullName, email, hash;
            if (!std::getline(ss, username, ',')) continue;
            if (!std::getline(ss, fullName, ',')) continue;
            if (!std::getline(ss, email, ',')) continue;
            if (!std::getline(ss, hash, ',')) continue;

            User u{trim(username), trim(fullName), trim(email), trim(hash)};
            users_[u.username] = u;
        }
    }

    void persist() const
    {
        std::ofstream out(file_);
        out << "username,fullName,email,passwordHash\n";
        for (const auto& kv : users_)
        {
            const std::string& username = kv.first;
            const User& user = kv.second;
            out << username << ',' << user.fullName << ',' << user.email << ',' << user.passwordHash << '\n';
        }
    }
};

class WalletService
{
public:
    explicit WalletService(const std::string& file) : file_(file) { load(); }

    double getBalance(const std::string& username) const
    {
        auto it = balances_.find(username);
        if (it == balances_.end()) return 0.0;
        return it->second;
    }

    bool deposit(const std::string& username, double amount)
    {
        if (amount <= 0) return false;
        balances_[username] += amount;
        persist();
        return true;
    }

    bool withdraw(const std::string& username, double amount)
    {
        if (amount <= 0) return false;
        double bal = getBalance(username);
        if (bal + 1e-9 < amount) return false;
        balances_[username] = bal - amount;
        persist();
        return true;
    }

private:
    std::string file_;
    std::map<std::string, double> balances_;

    void load()
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

    void persist() const
    {
        std::ofstream out(file_);
        out << "username,balance\n";
        for (const auto& kv : balances_)
        {
            const std::string& username = kv.first;
            double bal = kv.second;
            out << username << ',' << bal << '\n';
        }
    }
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

class CandleStickService
{
public:
    explicit CandleStickService(const MarketDataStore& market) : market_(market) {}

    std::vector<Candle> compute(const std::string& product, OrderSide side, int granularity) const
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
            const Builder& b = kv.second;
            out.push_back({key, b.open, b.high, b.low, b.close, b.volume});
        }
        return out;
    }

    void printTable(const std::vector<Candle>& candles) const
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

private:
    const MarketDataStore& market_;
};

class TradeService
{
public:
    TradeService(const std::string& file, const MarketDataStore& market, WalletService& wallet)
        : file_(file), market_(market), wallet_(wallet)
    {
        load();
    }

    bool recordTrade(const std::string& username, const std::string& product, OrderSide side,
                     double price, double amount, const std::string& ts = nowTimestamp())
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

    std::vector<Trade> getRecent(const std::string& username, size_t count, const std::string& productFilter = "") const
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

    void printRecent(const std::vector<Trade>& trades) const
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

    void printOverallCounts(const std::string& username) const
    {
        size_t asks = 0, bids = 0;
        for (const auto& t : trades_)
        {
            if (t.username != username) continue;
            if (t.side == OrderSide::Ask) ++asks; else ++bids;
        }
        std::cout << "Asks: " << asks << ", Bids: " << bids << '\n';
    }

    void printCountsForProduct(const std::string& username, const std::string& product) const
    {
        size_t asks = 0, bids = 0;
        for (const auto& t : trades_)
        {
            if (t.username != username || t.product != product) continue;
            if (t.side == OrderSide::Ask) ++asks; else ++bids;
        }
        std::cout << product << " -> Asks: " << asks << ", Bids: " << bids << '\n';
    }

    void printSpendInRange(const std::string& username, const std::string& start, const std::string& end) const
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

    void simulateForAllProducts(const std::string& username)
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
                double priceBid = baseBid * jitter(rng);
                double amountBid = amt(rng);
                if (recordTrade(username, product, OrderSide::Bid, priceBid, amountBid)) ++created;

                double priceAsk = baseAsk * jitter(rng);
                double amountAsk = amt(rng);
                if (recordTrade(username, product, OrderSide::Ask, priceAsk, amountAsk)) ++created;
            }
        }

        std::cout << "Simulated " << created << " trades across " << products.size() << " products.\n";
    }

private:
    std::string file_;
    const MarketDataStore& market_;
    WalletService& wallet_;
    std::vector<Trade> trades_;

    void load()
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

    void persist() const
    {
        std::ofstream out(file_);
        out << "timestamp,username,product,side,price,amount,total\n";
        for (const auto& t : trades_)
        {
            out << t.timestamp << ',' << t.username << ',' << t.product << ',' << sideToString(t.side) << ','
                << t.price << ',' << t.amount << ',' << t.total << '\n';
        }
    }
};

namespace ui {

inline void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

inline int readIntInRange(const std::string& prompt, int lo, int hi)
{
    while (true)
    {
        std::cout << prompt;/*  */
        int x;
        if (std::cin >> x)
        {
            ui::clearInput();
            if (x >= lo && x <= hi) return x;
        }
        else
        {
            ui::clearInput();
        }
        std::cout << "Invalid input. Enter a number in [" << lo << ", " << hi << "].\n";
    }
}

inline double readDouble(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;
        double x;
        if (std::cin >> x)
        {
            ui::clearInput();
            return x;
        }
        ui::clearInput();
        std::cout << "Invalid input. Enter a numeric value.\n";
    }
}

inline std::string readLineNonEmpty(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        if (!s.empty()) return s;
        std::cout << "Input cannot be empty.\n";
    }
}

inline void printDivider()
{
    std::cout << "------------------------------------------------------------\n";
}

} // namespace ui

// A simple session model for "who is logged in"
struct Session
{
    bool loggedIn = false;
    std::string username;  // 10-digit id in spec
    std::string fullName;
    std::string email;
};

static void printWelcome()
{
    ui::printDivider();
    std::cout << "CM2005 - Currency Exchange Technical Analysis Toolkit\n";
    std::cout << "Midterm Coursework - Console Demo App\n";
    ui::printDivider();
}

static void printAuthMenu()
{
    std::cout << "\n[AUTH]\n";
    std::cout << "1) Login\n";
    std::cout << "2) Register\n";
    std::cout << "0) Exit\n";
}

static void printMainMenu(const Session& s)
{
    ui::printDivider();
    std::cout << "Logged in as: " << s.username << " (" << s.email << ")\n";
    ui::printDivider();
    std::cout << "\n[MAIN MENU]\n";
    std::cout << "1) Task 1 - View candlestick summary (asks/bids)\n";
    std::cout << "2) Task 3 - Wallet: deposit/withdraw/view balance\n";
    std::cout << "3) Task 3 - View recent transactions\n";
    std::cout << "4) Task 4 - Simulate trading (create new asks/bids)\n";
    std::cout << "5) Task 3 - Activity stats (asks/bids counts, spend in timeframe)\n";
    std::cout << "9) Logout\n";
    std::cout << "0) Exit\n";
}

static void handleTask1Candles(CandleStickService& cs, const MarketDataStore& md)
{
    // Spec: enter product like "ETH/USDT", default yearly summary, optionally daily/monthly/yearly
    ui::printDivider();
    std::cout << "TASK 1 - Candlestick Summary\n";
    ui::printDivider();

    std::string product = ui::readLineNonEmpty("Enter product (e.g., ETH/USDT): ");
    int gran = ui::readIntInRange("Select timeframe: 1=Daily, 2=Monthly, 3=Yearly (default 3): ", 1, 3);

    // Optional: asks vs bids
    int side = ui::readIntInRange("Select side: 1=Asks, 2=Bids: ", 1, 2);

    if (!md.hasProduct(product))
    {
        std::cout << "Unknown product. Available: ";
        auto prods = md.products();
        for (size_t i = 0; i < prods.size(); ++i)
        {
            std::cout << prods[i] << (i + 1 < prods.size() ? ", " : "\n");
        }
        return;
    }

    auto candles = cs.compute(product, side == 1 ? OrderSide::Ask : OrderSide::Bid, gran);
    cs.printTable(candles);
}

static void handleWallet(Session& s, WalletService& wallet)
{
    ui::printDivider();
    std::cout << "TASK 3 - Wallet Operations\n";
    ui::printDivider();

    std::cout << "1) Deposit\n";
    std::cout << "2) Withdraw\n";
    std::cout << "3) View balance\n";
    std::cout << "0) Back\n";

    int choice = ui::readIntInRange("Choose: ", 0, 3);
    if (choice == 0) return;

    if (choice == 1)
    {
        double amt = ui::readDouble("Deposit amount: ");
        if (wallet.deposit(s.username, amt))
        {
            std::cout << "Deposit recorded. New balance: " << wallet.getBalance(s.username) << "\n";
        }
        else
        {
            std::cout << "Deposit failed. Amount must be positive.\n";
        }
    }
    else if (choice == 2)
    {
        double amt = ui::readDouble("Withdraw amount: ");
        if (wallet.withdraw(s.username, amt))
        {
            std::cout << "Withdrawal recorded. New balance: " << wallet.getBalance(s.username) << "\n";
        }
        else
        {
            std::cout << "Withdrawal failed (check amount and balance).\n";
        }
    }
    else if (choice == 3)
    {
        auto bal = wallet.getBalance(s.username);
        std::cout << "Balance: " << std::fixed << std::setprecision(2) << bal << "\n";
    }
}

static void handleRecentTransactions(Session& s, TradeService& trades)
{
    ui::printDivider();
    std::cout << "TASK 3 - Recent Transactions\n";
    ui::printDivider();

    // Optional filter by product
    std::cout << "Filter by product? (leave empty for all)\n";
    std::string product;
    std::getline(std::cin, product);

    auto tx = trades.getRecent(s.username, 5, trim(product));
    trades.printRecent(tx);
}

static void handleSimulateTrading(Session& s, TradeService& trades)
{
    ui::printDivider();
    std::cout << "TASK 4 - Simulate Trading\n";
    ui::printDivider();

    trades.simulateForAllProducts(s.username);
}

static void handleStats(Session& s, TradeService& trades)
{
    ui::printDivider();
    std::cout << "TASK 3 - Activity Stats\n";
    ui::printDivider();

    std::cout << "1) Overall asks/bids counts\n";
    std::cout << "2) Per-product asks/bids counts\n";
    std::cout << "3) Total money spent in timeframe\n";
    std::cout << "0) Back\n";

    int choice = ui::readIntInRange("Choose: ", 0, 3);
    if (choice == 0) return;

    if (choice == 1)
    {
        trades.printOverallCounts(s.username);
    }
    else if (choice == 2)
    {
        std::string product = ui::readLineNonEmpty("Enter product: ");
        trades.printCountsForProduct(s.username, product);
    }
    else if (choice == 3)
    {
        // Keep it simple: ask user for start/end date strings and let your service parse/validate
        std::string start = ui::readLineNonEmpty("Start date (YYYY-MM-DD): ");
        std::string end   = ui::readLineNonEmpty("End date (YYYY-MM-DD): ");
        trades.printSpendInRange(s.username, start, end);
    }
}

int main()
{
    printWelcome();

    std::string marketCsv = dataPath("20200601.csv");
    if (!fileExists(marketCsv))
    {
        marketCsv = dataPath("market.csv");
    }
    std::string usersCsv  = dataPath("users.csv");
    std::string walletCsv = dataPath("wallet.csv");
    std::string tradesCsv = dataPath("trades.csv");

    std::string fallbackMarket = "merklerex_start_topic_3/src/20200317.csv";
    if (!fileExists(fallbackMarket))
    {
        fallbackMarket = "merklerex_start_topic_2/src/20200317.csv";
    }
    if (!fileExists(fallbackMarket) && fileExists("../merklerex_start_topic_3/src/20200317.csv"))
    {
        fallbackMarket = "../merklerex_start_topic_3/src/20200317.csv";
    }

    MarketDataStore market(marketCsv, fallbackMarket);
    UserService users(usersCsv);
    WalletService wallet(walletCsv);
    TradeService trades(tradesCsv, market, wallet);
    CandleStickService cs(market);

    Session session;

    while (true)
    {
        if (!session.loggedIn)
        {
            printAuthMenu();
            int choice = ui::readIntInRange("Choose: ", 0, 2);

            if (choice == 0) break;

            if (choice == 1)
            {
                std::string username = ui::readLineNonEmpty("Username (10 digits): ");
                std::string password = ui::readLineNonEmpty("Password: ");

                User u;
                if (users.login(username, password, u))
                {
                    session.loggedIn = true;
                    session.username = u.username;
                    session.fullName = u.fullName;
                    session.email = u.email;
                    std::cout << "Welcome back, " << u.fullName << "!\n";
                }
                else
                {
                    std::cout << "Invalid credentials.\n";
                }
            }
            else if (choice == 2)
            {
                std::string fullName = ui::readLineNonEmpty("Full name: ");
                std::string email    = ui::readLineNonEmpty("Email: ");
                std::string password = ui::readLineNonEmpty("Password: ");

                User newUser;
                if (users.registerUser(fullName, email, password, newUser))
                {
                    session.loggedIn = true;
                    session.username = newUser.username;
                    session.fullName = newUser.fullName;
                    session.email = newUser.email;
                    std::cout << "Registered successfully. Your username is " << newUser.username << "\n";
                }
                else
                {
                    std::cout << "Registration failed. Please try again.\n";
                }
            }
        }
        else
        {
            printMainMenu(session);
            int choice = ui::readIntInRange("Choose: ", 0, 9);

            switch (choice)
            {
                case 1: handleTask1Candles(cs, market); break;
                case 2: handleWallet(session, wallet); break;
                case 3: handleRecentTransactions(session, trades); break;
                case 4: handleSimulateTrading(session, trades); break;
                case 5: handleStats(session, trades); break;
                case 9:
                    session = Session{};
                    std::cout << "Logged out.\n";
                    break;
                case 0:
                    return 0;
            }
        }
    }

    std::cout << "Goodbye.\n";
    return 0;
}
