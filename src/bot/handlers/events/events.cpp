#include "events.hpp"
#include <string>

namespace NeverForgetBot::Events {

    void send_event(TgBot::Bot& bot, long chat_id, const Event& event) {
        std::string status_str;

        switch (event.status) {
            case EventStatus::PENDING:
                status_str = "В ожидании";
                break;
            case EventStatus::COMPLETED:
                status_str = "Завершено";
                break;
            case EventStatus::NOT_COMPLETED:
                status_str = "Не завершено";
                break;
            case EventStatus::DELETED:
                status_str = "удаленно";
                break;
        }

        std::string confirmation_message =
            "Событие создано успешно:\n\n*" +
            event.name + "*\n" +
            "Время: " + event.time + "\n" +
            "Статус: " + status_str;

        bot.getApi().sendMessage(chat_id, confirmation_message, nullptr, nullptr, nullptr, "Markdown");
    }

    void send_events(TgBot::Bot& bot, long chat_id, const std::vector<Event>& events,Database &db) {
        if (events.empty()) {
            bot.getApi().sendMessage(chat_id, "Нет предстоящих событий. Самое время создать новое 🙂.");
            return;
        }

        std::string message = "Предстоящие события:\n\n";

        for (const auto& event : events) {
            std::string status_str;

            switch (event.status) {
                case EventStatus::PENDING:
                    status_str = "в процессе исполнения";
                    break;
                case EventStatus::COMPLETED:
                    status_str = "завершено";
                    break;
                case EventStatus::NOT_COMPLETED:
                    status_str = "не завершено";
                    break;
                case EventStatus::DELETED:
                    status_str = "удалено";
                    break;
            }

            message +=
                "*" + event.name + "*\n" +
                "Дата события: " + Utils::formatTimeWithTimezone(chat_id, event.time, db).value_or(event.time) + "\n" +
                "Статус: " + status_str + "\n\n";

            
            TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

        
            TgBot::InlineKeyboardButton::Ptr doneButton(new TgBot::InlineKeyboardButton);
            doneButton->text = "✅ Done";
            doneButton->callbackData = "upcomming-event-done-" + event.id;


            TgBot::InlineKeyboardButton::Ptr deleteButton(new TgBot::InlineKeyboardButton);
            deleteButton->text = "❌ Delete";
            deleteButton->callbackData = "upcomming-event-delete-" + event.id;

    
            std::vector<TgBot::InlineKeyboardButton::Ptr> row = {doneButton, deleteButton};
            keyboard->inlineKeyboard.push_back(row);

            // Send message with inline keyboard
            bot.getApi().sendMessage(
                chat_id,
                message,
                nullptr,  
                nullptr,    
                keyboard,  
                "Markdown"
            );
        }
}



} // namespace NeverForgetBot::Events
