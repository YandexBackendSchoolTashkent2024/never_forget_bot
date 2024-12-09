#include "callback.hpp"

namespace NeverForgetBot::CallbackHandlers {

void onEventStatusChangeSelection(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db) {
    try {
        std::string data = query->data;
        int colon_idx = data.find(":");

        std::string action = data.substr(7, colon_idx-7);
        std::cout << action << '\n';
        std::string notification_id = data.substr(colon_idx + 1);
        std::cout << notification_id << '\n';

        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);

        std::optional<std::string> result = db.changeEventStatus(action, notification_id);

        bot.getApi().sendMessage(query->message->chat->id, result.value_or("Что то пошло не так.."));
    } catch (std::exception &e) {
        std::cerr << "Event status change failed: " << e.what() << '\n';
    }
}

}

