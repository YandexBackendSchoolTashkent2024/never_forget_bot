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

std::string formatTimeWithTimezone(long telegram_id, const std::string& time, NeverForgetBot::Database& db) {
    try {
        int timezone = db.getUserTimeZone(telegram_id);

        std::string timestamp = time.substr(0, 19);

        std::tm timeStruct = {};
        std::istringstream ss(timestamp);
        ss >> std::get_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error("Failed to parse timestamp");
        }

        time_t timeEpoch = std::mktime(&timeStruct);
        if (timeEpoch == -1) {
            throw std::runtime_error("Failed to convert to time_t");
        }
        timeEpoch += timezone * 3600;

        std::tm* updatedTimeStruct = std::gmtime(&timeEpoch);
        if (!updatedTimeStruct) {
            throw std::runtime_error("Failed to convert back to tm struct");
        }

        std::ostringstream output;
        output << std::put_time(updatedTimeStruct, "%Y-%m-%d %H:%M:%S");
        return output.str();
    } catch (const std::exception& e) {
        std::cerr << "Error formatting time: " << e.what() << std::endl;
        return "Invalid time";
    }
}

}