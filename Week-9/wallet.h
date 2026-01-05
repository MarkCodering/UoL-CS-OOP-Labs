#pragma once

#include <string>
#include <map>

/**
 * Wallet class
 * Stores different currencies and their amounts.
 */
class Wallet
{
public:
    // Insert currency into the wallet
    void insertCurrency(std::string type, double amount);

    // Remove currency from the wallet
    bool removeCurrency(std::string type, double amount);

    // Check if wallet contains at least the given amount of currency
    bool containsCurrency(std::string type, double amount);

    // Convert wallet contents to a string for display
    std::string toString() const;

private:
    // Map storing currency type -> amount
    std::map<std::string, double> currencies;
};
