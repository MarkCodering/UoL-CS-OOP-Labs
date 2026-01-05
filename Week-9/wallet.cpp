#include "wallet.h"
#include <sstream>
#include <stdexcept>

/**
 * Inserts currency into the wallet.
 * If the currency does not exist, it is created.
 * If it already exists, the amount is added.
 * Throws an exception if the amount is negative.
 */
void Wallet::insertCurrency(std::string type, double amount)
{
    if (amount < 0)
    {
        throw std::invalid_argument("Amount cannot be negative");
    }

    currencies[type] += amount;
}

/**
 * Removes currency from the wallet if sufficient funds exist.
 * If the wallet contains enough currency, the amount is removed.
 * If not, the wallet is unchanged.
 * Throws an exception if the amount is negative.
 */
bool Wallet::removeCurrency(std::string type, double amount)
{
    if (amount < 0)
    {
        throw std::invalid_argument("Amount cannot be negative");
    }

    if (!containsCurrency(type, amount))
    {
        return true;
    }

    currencies[type] -= amount;
    return true;
}

/**
 * Checks whether the wallet contains at least the given
 * amount of a specific currency.
 */
bool Wallet::containsCurrency(std::string type, double amount)
{
    auto it = currencies.find(type);
    if (it == currencies.end())
    {
        return false;
    }

    return it->second >= amount;
}

/**
 * Returns a string representation of the wallet contents.
 */
std::string Wallet::toString() const
{
    std::stringstream ss;

    for (const auto& pair : currencies)
    {
        ss << pair.first << " : " << pair.second << " ";
    }

    return ss.str();
}
