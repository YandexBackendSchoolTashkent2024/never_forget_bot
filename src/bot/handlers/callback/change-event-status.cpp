#include "callback.hpp"

namespace NeverForgetBot::CallbackHandlers {

void onEventStatusChangeSelection(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db) {
    try {
        std::string data = query->data;
        int colon_idx = data.find(":");

        std::string action = data.substr(7, colon_idx - 7);
        std::string notification_id = data.substr(colon_idx + 1);

        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);

        std::string result = (
            db.changeEventStatus(notification_id, action).has_value() ? 
            "Успешно!" :
            "Что то пошло не так..."
        );

        bot.getApi().sendMessage(query->message->chat->id, result);
    } catch (std::exception &e) {
        std::cerr << "Event status change failed: " << e.what() << '\n';
    }
}

}

