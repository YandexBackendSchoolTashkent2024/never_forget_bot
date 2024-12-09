#include <string>
#include <tgbot/tgbot.h>
#include "../../../db/db.hpp"

namespace NeverForgetBot::CallbackHandlers {

    void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, Database& db);

    void onTimeZoneSelection(const std::string& timeZone, TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, Database& db);

    void onDelayNotificationSelection(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db);

}

