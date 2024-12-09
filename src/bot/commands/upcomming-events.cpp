#include "commands.hpp"
#include <unordered_map>

namespace NeverForgetBot::Commands {
    void onUpcommingEventsCommand(TgBot::Message::Ptr message, TgBot::Bot& bot, Database& db) {
        long telegram_id = message->from->id;

        auto upcomingEvents = db.getEventsOrderedByTimeDesc(telegram_id);

        if (upcomingEvents.empty()) {
            bot.getApi().sendMessage(telegram_id, "Нет предстоящих событий. Самое время создать новое 🙂.");
            return;
        }

        static const std::unordered_map<EventStatus, std::string> statusMap = {
            {EventStatus::PENDING, "в процессе исполнения"},
            {EventStatus::COMPLETED, "завершено"},
            {EventStatus::NOT_COMPLETED, "не завершено"},
            {EventStatus::DELETED, "удаленно"}
        };

        for (const auto& event : upcomingEvents) {
            std::string status_str = statusMap.at(event.status);

            std::string eventMessage = 
                "**" + event.name + "**\n" +
                "Время: " + event.time + "\n" +
                "Статус: " + status_str + "\n\n";

            auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
            keyboard->inlineKeyboard.push_back({
                TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"✅ Выполнено", "","upcomming-event-done-" + event.id}),
                TgBot::InlineKeyboardButton::Ptr(new TgBot::InlineKeyboardButton{"❌ Удалить", "","upcomming-event-delete-" + event.id})
            });

            bot.getApi().sendMessage(
                telegram_id,
                eventMessage,
                nullptr,  // Disable web page preview
                0,      // Reply to message ID
                keyboard,
                "Markdown"
            );
        }
    }
}
