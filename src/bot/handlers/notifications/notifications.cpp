#include <tgbot/tgbot.h>

#include "../../../models/event.hpp"
#include "../../../models/notification.hpp"

namespace NeverForgetBot::Notifications {

TgBot::InlineKeyboardMarkup::Ptr createNotificationKeyboard(const std::string& notification_id) {
    auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();

    keyboard->inlineKeyboard.push_back({
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"Сделано", "", "status_complete:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"Не сделано", "", "status_incomplete:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"Удалить", "", "stats_delete:" + notification_id})
    });

    keyboard->inlineKeyboard.push_back({
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+15", "", "delay_15:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+30 mins", "", "delay_30:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+1 hour", "", "delay_60:" + notification_id})
    });

    keyboard->inlineKeyboard.push_back({
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+3 часа", "", "delay_180:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+6 часов", "", "delay_360:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+12 часов", "", "delay_720:" + notification_id})
    });

    return keyboard;
}

void sendNotification(long chatId, TgBot::Bot &bot, const Notification& notification, const Event& event) {
    std::string message = 
        std::string("🔔 Напоминание! 🔔\n\n") + 
        "✨ Событие: " + event.name + "\n" + 
        "📅 Запланировано на: " + event.time + "\n\n" +
        "Ваш дружелюбный бот всегда на страже ваших дел. 💡";

    auto keyboard = createNotificationKeyboard(notification.id);

    bot.getApi().sendMessage(chatId, message, nullptr, nullptr, keyboard, "Markdown");
}

} // namespace NeverForgetBot::Notifications
