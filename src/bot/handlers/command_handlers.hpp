#pragma once
#include "../../db/db.hpp"

#include <tgbot/tgbot.h>
namespace NeverForgetBot::Commands {

void onHelpCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);

void onStartCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);
<<<<<<< HEAD
void onChangeTzCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);
void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database &db);
=======

void onChangeTzCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);

void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database &db);

>>>>>>> 4c06fda42d1d301d7e03f2a25e235fa89d3d2cd7
} // namespace NeverForgetBot::Commands
