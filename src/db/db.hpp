#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>
#include "../models/event.hpp"

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);
    std::vector<Event> getEventsOrderedByTimeDesc(long telegram_id);


private:
    pqxx::connection* conn;
};

}
