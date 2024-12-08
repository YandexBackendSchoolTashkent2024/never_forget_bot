#pragma once
#include "../../db/db.hpp"

#include <tgbot/tgbot.h>
namespace NeverForgetBot::Commands {

void onHelpCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);
void onStartCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);
void onChangeTzCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);
void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database &db);
} // namespace NeverForgetBot::Commands
