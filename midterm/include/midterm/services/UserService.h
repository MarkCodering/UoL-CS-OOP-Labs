#pragma once

#include <map>
#include <string>

#include "midterm/core/Models.h"

namespace midterm::services {

class UserService
{
public:
    explicit UserService(const std::string& file);

    bool registerUser(const std::string& fullName, const std::string& email, const std::string& password, User& out);
    bool login(const std::string& username, const std::string& password, User& out) const;

private:
    std::string file_;
    std::map<std::string, User> users_;

    static std::string hashPassword(const std::string& pw);
    static std::string generateUsername();

    void load();
    void persist() const;
};

} // namespace midterm::services

