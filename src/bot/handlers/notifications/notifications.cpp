#include <tgbot/tgbot.h>

#include "../../../models/event.hpp"
#include "../../../models/notification.hpp"

namespace NeverForgetBot::Notifications {

TgBot::InlineKeyboardMarkup::Ptr createNotificationKeyboard(const std::string& notification_id) {
    auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();

    keyboard->inlineKeyboard.push_back({
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"Сделано", "", "status_COMPLETED:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"Не сделано", "", "status_NOT_COMPLETED:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"Удалить", "", "status_DELETED:" + notification_id})
    });

    keyboard->inlineKeyboard.push_back({
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+15 минут", "", "delay_15min:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+30 минут", "", "delay_30min:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+1 час", "", "delay_60min:" + notification_id})
    });

    keyboard->inlineKeyboard.push_back({
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+3 часа", "", "delay_180min:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+6 часов", "", "delay_360min:" + notification_id}),
        TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"+12 часов", "", "delay_720min:" + notification_id})
    });

    return keyboard;
}

void sendNotification(long chatId, TgBot::Bot &bot, const Notification& notification, const Event& event) {
    std::string message =
        "Детали уведомления:\n\n" +
        std::string("Событие: ") + event.time + "\n" +
        "Запланировано: " + notification.time + "\n" +
        "Создано: " + notification.createdAt + "\n";

    message += "Отправлено: " + notification.sentTime.value_or("❌") + "\n";

    auto keyboard = createNotificationKeyboard(notification.id);

    bot.getApi().sendMessage(chatId, message, nullptr, nullptr, keyboard, "Markdown");
}

} // namespace NeverForgetBot::Notifications
