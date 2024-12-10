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

        std::string time_unit;
        int num_interval = stoi(interval);
        int num_delay = num_interval < 60 ? num_interval : num_interval / 60;
        if (num_interval < 60) {
            time_unit = "минут";
        } else if (num_interval == 60) {
            time_unit = "час";
        } else if (num_interval == 180) {
            time_unit = "часа";
        } else {
            time_unit = "часов";
        }

        std::string message = "Уведомление успешно отложено на *" +
        to_string(num_delay) + " " + time_unit + "*. Напомним *" +
        Utils::formatDateInRussian(Utils::convertToISO(query->message->chat->id, notification.time, db).value_or(notification.time)) + "*";

        bot.getApi().sendMessage(query->message->chat->id, message, nullptr, nullptr, nullptr, "Markdown");
    } catch (std::exception &e) {
        std::cerr << "Notification delay failed: " << e.what() << '\n';
    }
}

}

