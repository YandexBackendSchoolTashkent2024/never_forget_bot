#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);
    bool updateUserTimeZone(long telegram_id, const std::string& timezone);
private:
    pqxx::connection* conn;
};

}
