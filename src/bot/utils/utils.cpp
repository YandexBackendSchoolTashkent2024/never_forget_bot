#include "utils.hpp"
#include "../../models/user.hpp"

namespace NeverForgetBot::Utils {

void saveUserIfNotExists(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db) {
    long telegram_id = message->from->id;
    std::optional<std::string> username;
    std::optional<std::string> name;

    if (!message->from->username.empty()) {
        username = message->from->username;
    }

    if (!message->from->firstName.empty() || !message->from->lastName.empty()) {
        std::string fullName = message->from->firstName;
        if (!message->from->lastName.empty()) {
            fullName += " " + message->from->lastName;
        }
        name = fullName;
    }

    db.insertUser(telegram_id, username, name);
};

void startLongPolling(TgBot::Bot& bot) {
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();
        bot.getApi().setMyCommands(getBotCommands());
        bot.getApi().setMyDescription(getBotDescription());
        bot.getApi().setMyShortDescription("An awesome bot to make you forget about forgetting");

        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
}

std::vector<TgBot::BotCommand::Ptr> getBotCommands() {
    std::unordered_map<std::string, std::string> mp = {
        { "/upcoming_events", "получить список предстоящих событий" },
        { "/settings", "отобразить меню настроек" },
        { "/help", "инструкция по использованию бота" },
        { "/start", "запуск бота" }
    };

    std::vector<TgBot::BotCommand::Ptr> commands;
    for (auto [cmd, description] : mp) {
        auto command = std::make_shared<TgBot::BotCommand>();
        command->command = cmd;
        command->description = description;
        commands.push_back(command);
    }

    return commands;
}

std::string getBotDescription() {
    return "Добро пожаловать в Never Forget Bot!\n\n"
        "Создавайте напоминания и получайте своевременные уведомления без лишней траты времени.\n\n"
        "Начнем! 🎯";
}

void saveEvent(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db, Checker event) {
    long telegram_id = message->from->id;
    std::string event_name;
    std::string event_time;
    std::string event_type;
    std::vector<std::string> notifications;

    event_name = event.getNameEvent();
    event_time = event.getTime();
    event_type = event.getType();
    notifications = event.getNotifications();
    // auto userId = db.insertUser(telegram_id, username, name);

    db.insertEvent(telegram_id, event_name, event_time);
    if (event_name.size()) {
        bot.getApi().sendMessage(message->chat->id, "Saved event successfully");
    } else {
        bot.getApi().sendMessage(message->chat->id, "Failed to save event");
    }
};

} // namespace NeverForgetBot::Utils
