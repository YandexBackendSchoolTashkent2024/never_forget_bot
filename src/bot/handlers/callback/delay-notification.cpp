#include "callback.hpp"

namespace NeverForgetBot::CallbackHandlers {

void onDelayNotificationSelection(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db) {
    try {
        std::string data = query->data;
        int colon_idx = data.find(":");

        std::string interval = data.substr(6, colon_idx - 6);
        std::string notification_id = data.substr(colon_idx + 1);

        Notification notification = db.delayNotification(notification_id, interval);

        bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);

        std::string message = "Уведомление успешно отложено на *" +
        interval.substr(0, interval.length() - 3) + "* минут. Напомним *" +
        Utils::formatTimeWithTimezone(query->message->chat->id, notification.time, db).value_or(notification.time) + "*";

        bot.getApi().sendMessage(query->message->chat->id, message, nullptr, nullptr, nullptr, "Markdown");
    } catch (std::exception &e) {
        std::cerr << "Notification delay failed: " << e.what() << '\n';
    }
}

}

