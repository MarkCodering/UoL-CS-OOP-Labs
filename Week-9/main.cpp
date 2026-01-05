#include "wallet.h"
#include <iostream>

int main()
{
    Wallet wallet;

    std::cout << "Inserting 100 BTC" << std::endl;
    wallet.insertCurrency("BTC", 100);
    std::cout << wallet.toString() << std::endl;

    std::cout << "Checking if wallet contains 50 BTC" << std::endl;
    std::cout << wallet.containsCurrency("BTC", 50) << std::endl;

    std::cout << "Removing 30 BTC" << std::endl;
    wallet.removeCurrency("BTC", 30);
    std::cout << wallet.toString() << std::endl;

    std::cout << "Checking if wallet contains 100 BTC" << std::endl;
    std::cout << wallet.containsCurrency("BTC", 100) << std::endl;

    return 0;
}
