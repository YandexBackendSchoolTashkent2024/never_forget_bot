#pragma once

#include <pqxx/pqxx>
#include <string>
#include <optional>

struct User {
    std::string id;
    long telegram_id;
    std::optional<std::string> username;
    std::optional<std::string> name;
    std::string created_at;
    std::string updated_at;
};

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();

    // Inserts a user and returns the generated UUID
    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

private:
    pqxx::connection* conn;
};
