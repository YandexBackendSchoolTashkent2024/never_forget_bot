#include "events.hpp"
#include <tgbot/Bot.h>

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
        }

        std::string confirmation_message =
            "Событие создано успешно:\n\n*" +
            event.name + "*\n" +
            "Время: " + event.time + "\n" +
            "Статус: " + status_str;

        bot.getApi().sendMessage(chat_id, confirmation_message, nullptr, nullptr, "Markdown");
    }

    void send_events(TgBot::Bot &bot, long chat_id, const std::vector<Event> &events) {
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
            }

            message +=
                "**" + event.name + "**\n" +
                "Время: " + event.time + "\n" +
                "Статус: " + status_str + "\n\n";
        }

        bot.getApi().sendMessage(chat_id, message);
    }

} // namespace NeverForgetBot::Events
