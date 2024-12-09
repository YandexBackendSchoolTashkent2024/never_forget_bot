#include "events.hpp"

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

        bot.getApi().sendMessage(chat_id, confirmation_message, nullptr, nullptr, nullptr, "Markdown");
    }

    void send_events(TgBot::Bot &bot, long chat_id, const std::vector<Event> &events, Database &db) {
        if (events.empty()) {
            bot.getApi().sendMessage(chat_id, "Нет предстоящих событий. Самое время создать новое 🙂.");
            return;
        }

        std::string message = "Предстоящие события:\n\n";
        for (const auto& event : events) {
            std::string status_str;

            message +=
                "*" + event.name + "*\n" +
                "Время: " + Utils::formatTimeWithTimezone(chat_id, event.time, db).value_or(event.time) + "\n" +
        }

        bot.getApi().sendMessage(chat_id, message, nullptr, nullptr, nullptr, "Markdown");
    }

} // namespace NeverForgetBot::Events
