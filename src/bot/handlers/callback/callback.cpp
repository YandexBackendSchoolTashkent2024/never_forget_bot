#include "callback.hpp"

namespace NeverForgetBot::CallbackHandlers {

    void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, Database& db){
        std::string callbackData = query->data;

        if (callbackData.find("user_timez-") == 0) {
            std::string timeZone = callbackData.substr(11);
            onTimeZoneSelection(timeZone, query, bot, db);
        } else if (callbackData.find("delay") == 0) {
            onDelayNotificationSelection(query, bot, db);
        } else if (callbackData.find("status") == 0) {
            onEventStatusChangeSelection(query, bot, db);
        } else {
            bot.getApi().sendMessage(query->message->chat->id, "Unknown selection.");
        }
    }

}
