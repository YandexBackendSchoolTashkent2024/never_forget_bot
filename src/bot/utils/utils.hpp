#pragma once

#include <tgbot/tgbot.h>
#include <map>
#include <string>
#include <vector>
#include <optional>

#include "../../db/db.hpp"

namespace NeverForgetBot::Utils {

    std::vector<TgBot::BotCommand::Ptr> getBotCommands();

    std::string getBotDescription();

    void saveUserIfNotExists(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db);

    void startLongPolling(TgBot::Bot& bot);

}