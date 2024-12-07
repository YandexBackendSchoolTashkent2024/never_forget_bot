#pragma once

#include <tgbot/tgbot.h>
#include <map>
#include <string>
#include <vector>

namespace NeverForgetBot::Utils {

    std::vector<TgBot::BotCommand::Ptr> getBotCommands();

    std::string getBotDescription();

    void startLongPolling(TgBot::Bot& bot);

} // namespace NeverForgetBot::Utils
