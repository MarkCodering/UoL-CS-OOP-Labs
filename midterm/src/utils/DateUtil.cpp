#include "midterm/utils/DateUtil.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace midterm {

std::string nowTimestamp()
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

bool parseDateParts(const std::string& ts, int& y, int& m, int& d)
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

std::string bucketKey(const std::string& ts, int granularity)
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

bool dateInRange(const std::string& ts, const std::string& start, const std::string& end)
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

} // namespace midterm

