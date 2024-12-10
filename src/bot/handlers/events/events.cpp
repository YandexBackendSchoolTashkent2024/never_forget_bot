#include "events.hpp"
#include <string>

namespace NeverForgetBot::Events {

void send_events(TgBot::Bot &bot, long chat_id, const std::vector<Event> &events, Database &db) {
    if (events.empty()) {
        bot.getApi().sendMessage(chat_id, "Нет предстоящих событий. Самое время создать новое 🙂.");
        return;
    }

    bot.getApi().sendMessage(chat_id, "Предстоящие события:");
    for (const auto& event : events) {
        std::string message =
            "*" + event.name + "*\n" +
            "Дата события: *" +
            Utils::formatDateInRussian(Utils::convertToISO(chat_id, event.time, db).value_or(event.time)) + "*";

        auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();

        keyboard->inlineKeyboard.push_back({
            TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"✅ Сделано", "", "status_COMPLETED:" + event.id}),
            TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"❌ Удалить", "", "status_DELETED:" + event.id})
        });

        bot.getApi().sendMessage(chat_id, message, nullptr, nullptr, keyboard, "Markdown");
    }
}

} // namespace NeverForgetBot::Events
