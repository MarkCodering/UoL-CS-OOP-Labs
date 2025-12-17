// main.cpp (scaffold - you must connect your own classes/methods)
// CM2005 OOP Midterm Coursework - Menu + Input Handling Skeleton

#include <iostream>
#include <string>
#include <limits>
#include <optional>

// TODO: include your own headers here
// #include "MarketDataStore.h"     // reads provided market CSV (read-only)
// #include "CandleStickService.h"  // computes OHLC summaries
// #include "UserService.h"         // register/login + hashing + user CSV
// #include "WalletService.h"       // deposit/withdraw + wallet CSV
// #include "TradeService.h"        // simulate trades + transaction CSV

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
        std::cout << prompt;
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

static void handleTask1Candles(/*CandleStickService& cs, MarketDataStore& md*/)
{
    // Spec: enter product like "ETH/USDT", default yearly summary, optionally daily/monthly/yearly
    ui::printDivider();
    std::cout << "TASK 1 - Candlestick Summary\n";
    ui::printDivider();

    std::string product = ui::readLineNonEmpty("Enter product (e.g., ETH/USDT): ");
    int gran = ui::readIntInRange("Select timeframe: 1=Daily, 2=Monthly, 3=Yearly (default 3): ", 1, 3);

    // Optional: asks vs bids
    int side = ui::readIntInRange("Select side: 1=Asks, 2=Bids: ", 1, 2);

    // TODO: call your service
    // auto candles = cs.compute(product, side==1?OrderSide::Ask:OrderSide::Bid, granularity);
    // cs.printTable(candles);

    std::cout << "[TODO] Call your candlestick computation + print table here.\n";
}

static void handleWallet(Session& s /*, WalletService& wallet*/)
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
        // TODO: wallet.deposit(s.username, amt);
        std::cout << "[TODO] Deposit recorded.\n";
    }
    else if (choice == 2)
    {
        double amt = ui::readDouble("Withdraw amount: ");
        // TODO: wallet.withdraw(s.username, amt); (validate sufficient funds)
        std::cout << "[TODO] Withdrawal recorded.\n";
    }
    else if (choice == 3)
    {
        // TODO: auto bal = wallet.getBalance(s.username);
        std::cout << "[TODO] Balance: <print from your wallet service>\n";
    }
}

static void handleRecentTransactions(Session& s /*, WalletService& wallet or TradeService& trades*/)
{
    ui::printDivider();
    std::cout << "TASK 3 - Recent Transactions\n";
    ui::printDivider();

    // Optional filter by product
    std::cout << "Filter by product? (leave empty for all)\n";
    std::string product;
    std::getline(std::cin, product);

    // TODO: auto tx = trades.getRecent(s.username, 5, product);
    std::cout << "[TODO] Print last 5 transactions here.\n";
}

static void handleSimulateTrading(Session& s /*, TradeService& trades*/)
{
    ui::printDivider();
    std::cout << "TASK 4 - Simulate Trading\n";
    ui::printDivider();

    // TODO: trades.simulateForAllProducts(s.username);
    // Must create at least FIVE new ask and bid orders for ALL products
    // Use current system timestamp
    // Update wallet accordingly, append to user CSV
    std::cout << "[TODO] Simulate trading for all products and persist results.\n";
}

static void handleStats(Session& s /*, TradeService& trades*/)
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
        // TODO: trades.printOverallCounts(s.username);
        std::cout << "[TODO] Overall counts.\n";
    }
    else if (choice == 2)
    {
        std::string product = ui::readLineNonEmpty("Enter product: ");
        // TODO: trades.printCountsForProduct(s.username, product);
        std::cout << "[TODO] Per-product counts.\n";
    }
    else if (choice == 3)
    {
        // Keep it simple: ask user for start/end date strings and let your service parse/validate
        std::string start = ui::readLineNonEmpty("Start date (YYYY-MM-DD): ");
        std::string end   = ui::readLineNonEmpty("End date (YYYY-MM-DD): ");
        // TODO: trades.printSpendInRange(s.username, start, end);
        std::cout << "[TODO] Total spend in timeframe.\n";
    }
}

int main()
{
    printWelcome();

    // TODO: instantiate your real services here (with file paths)
    // MarketDataStore market("market.csv");
    // UserService users("users.csv");
    // WalletService wallet("wallet.csv");
    // TradeService trades("trades.csv", market, wallet);
    // CandleStickService cs(market);

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

                // TODO: if (users.login(username, password)) { session.loggedIn=true; ... }
                std::cout << "[TODO] Login via your UserService here.\n";

                // TEMP placeholder to allow navigation while you build:
                session.loggedIn = true;
                session.username = username;
                session.email = "placeholder@email.com";
            }
            else if (choice == 2)
            {
                std::string fullName = ui::readLineNonEmpty("Full name: ");
                std::string email    = ui::readLineNonEmpty("Email: ");
                std::string password = ui::readLineNonEmpty("Password: ");

                // TODO: auto newUser = users.register(fullName, email, password);
                //       show generated 10-digit username
                std::cout << "[TODO] Register via your UserService here (hash password, persist CSV).\n";

                // TEMP placeholder:
                session.loggedIn = true;
                session.username = "0000000000";
                session.fullName = fullName;
                session.email = email;
            }
        }
        else
        {
            printMainMenu(session);
            int choice = ui::readIntInRange("Choose: ", 0, 9);

            switch (choice)
            {
                case 1: handleTask1Candles(/*cs, market*/); break;
                case 2: handleWallet(session /*, wallet*/); break;
                case 3: handleRecentTransactions(session /*, trades*/); break;
                case 4: handleSimulateTrading(session /*, trades*/); break;
                case 5: handleStats(session /*, trades*/); break;
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
