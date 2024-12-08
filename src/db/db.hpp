#pragma once

#include <pqxx/pqxx>
#include <string>
#include <optional>

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();

    // Inserts a user and returns the generated UUID
    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

private:
    pqxx::connection* conn;
};

}
