#include "midterm/core/Models.h"

#include "midterm/utils/StringUtil.h"

namespace midterm {

std::string sideToString(OrderSide side)
{
    return side == OrderSide::Ask ? "ask" : "bid";
}

bool parseSide(const std::string& s, OrderSide& out)
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

} // namespace midterm

