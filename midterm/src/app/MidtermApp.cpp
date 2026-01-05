#include "midterm/app/MidtermApp.h"

#include "midterm/core/Models.h"
#include "midterm/market/MarketDataStore.h"
#include "midterm/services/CandleStickService.h"
#include "midterm/services/TradeService.h"
#include "midterm/services/UserService.h"
#include "midterm/services/WalletService.h"
#include "midterm/ui/ConsoleUI.h"
#include "midterm/utils/Paths.h"
#include "midterm/utils/StringUtil.h"

#include <iomanip>
#include <iostream>

namespace midterm {
namespace {

void printWelcome()
{
    ui::printDivider();
    std::cout << "CM2005 - Currency Exchange Technical Analysis Toolkit\n";
    std::cout << "Midterm Coursework - Console Demo App\n";
    ui::printDivider();
}

void printAuthMenu()
{
    std::cout << "\n[AUTH]\n";
    std::cout << "1) Login\n";
    std::cout << "2) Register\n";
    std::cout << "0) Exit\n";
}

void printMainMenu(const Session& s)
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

void handleTask1Candles(services::CandleStickService& cs, const market::MarketDataStore& md)
{
    ui::printDivider();
    std::cout << "TASK 1 - Candlestick Summary\n";
    ui::printDivider();

    std::string product = ui::readLineNonEmpty("Enter product (e.g., ETH/USDT): ");
    int gran            = ui::readIntInRange("Select timeframe: 1=Daily, 2=Monthly, 3=Yearly (default 3): ", 1, 3);
    int side            = ui::readIntInRange("Select side: 1=Asks, 2=Bids: ", 1, 2);

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

void handleWallet(const Session& s, services::WalletService& wallet)
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

void handleRecentTransactions(const Session& s, services::TradeService& trades)
{
    ui::printDivider();
    std::cout << "TASK 3 - Recent Transactions\n";
    ui::printDivider();

    std::cout << "Filter by product? (leave empty for all)\n";
    std::string product;
    std::getline(std::cin, product);

    auto tx = trades.getRecent(s.username, 5, trim(product));
    trades.printRecent(tx);
}

void handleSimulateTrading(const Session& s, services::TradeService& trades)
{
    ui::printDivider();
    std::cout << "TASK 4 - Simulate Trading\n";
    ui::printDivider();

    trades.simulateForAllProducts(s.username);
}

void handleStats(const Session& s, services::TradeService& trades)
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
        std::string start = ui::readLineNonEmpty("Start date (YYYY-MM-DD): ");
        std::string end   = ui::readLineNonEmpty("End date (YYYY-MM-DD): ");
        trades.printSpendInRange(s.username, start, end);
    }
}

} // namespace

int MidtermApp::run()
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

    market::MarketDataStore market(marketCsv, fallbackMarket);
    services::UserService users(usersCsv);
    services::WalletService wallet(walletCsv);
    services::TradeService trades(tradesCsv, market, wallet);
    services::CandleStickService cs(market);

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
                    session.email    = u.email;
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
                    session.email    = newUser.email;
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

} // namespace midterm

