#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>
#include "../models/event.hpp"
#include <tgbot/Bot.h>

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);
    void getEventsOrderedByTimeDesc(TgBot::Bot &bot, long telegram_id);


    std::optional<std::string> deleteEvent(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

    bool updateUserTimeZone(long telegram_id, const std::string& timezone);

private:
    pqxx::connection* conn;
};

}
