#include "../../../db/db.hpp"
#include "../../utils/utils.hpp"

namespace NeverForgetBot::CallbackHandlers {

    void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, Database& db);

    void onTimeZoneSelection(const std::string& timeZone, TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, Database& db);

    void onDelayNotificationSelection(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database &db);

    void onEventStatusChangeSelection(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database &db);

}
