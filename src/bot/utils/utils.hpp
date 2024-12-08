#pragma once

#include <tgbot/tgbot.h>
#include <map>
#include <string>
#include <vector>
#include <optional>

#include "../../db/db.hpp"

#include "../../parse_msg/checker.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace NeverForgetBot::Utils {

    std::vector<TgBot::BotCommand::Ptr> getBotCommands();

    std::string getBotDescription();

    void saveUserIfNotExists(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db);

    void startLongPolling(TgBot::Bot& bot);

    std::string formatTimeWithTimezone(long telegram_id, const std::string& time, NeverForgetBot::Database& db);
    void saveEvent(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db, Checker &event) ;

    std::string adjustEventTime(const std::string& event_time, int user_timezone);

}