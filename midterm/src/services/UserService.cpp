#include "midterm/services/UserService.h"

#include "midterm/utils/StringUtil.h"

#include <fstream>
#include <random>
#include <sstream>

namespace midterm::services {

UserService::UserService(const std::string& file) : file_(file)
{
    load();
}

bool UserService::registerUser(const std::string& fullName, const std::string& email, const std::string& password, User& out)
{
    User u;
    u.username     = generateUsername();
    u.fullName     = fullName;
    u.email        = email;
    u.passwordHash = hashPassword(password);

    if (users_.count(u.username)) return false;
    users_[u.username] = u;
    persist();
    out = u;
    return true;
}

bool UserService::login(const std::string& username, const std::string& password, User& out) const
{
    auto it = users_.find(username);
    if (it == users_.end()) return false;
    if (it->second.passwordHash != hashPassword(password)) return false;
    out = it->second;
    return true;
}

std::string UserService::hashPassword(const std::string& pw)
{
    // Simple hash for demo purposes.
    return std::to_string(std::hash<std::string>{}(pw));
}

std::string UserService::generateUsername()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> digit(0, 9);
    std::string out(10, '0');
    for (char& c : out) c = static_cast<char>('0' + digit(rng));
    return out;
}

void UserService::load()
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

void UserService::persist() const
{
    std::ofstream out(file_);
    out << "username,fullName,email,passwordHash\n";
    for (const auto& kv : users_)
    {
        const std::string& username = kv.first;
        const User& user            = kv.second;
        out << username << ',' << user.fullName << ',' << user.email << ',' << user.passwordHash << '\n';
    }
}

} // namespace midterm::services

