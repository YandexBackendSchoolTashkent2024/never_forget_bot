#pragma once

#include <tgbot/tgbot.h>

namespace NeverForgetBot::Commands {

void onHelpCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);
void onStartCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);

} // namespace NeverForgetBot::Commands
