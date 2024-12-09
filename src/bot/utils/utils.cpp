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

void saveEvent(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db, Checker event) {
    long telegram_id = message->from->id;
    std::string event_name = event.getNameEvent();
    std::string event_time = event.getTime();
    std::string event_type;
    vector<std::string> notifications = event.getNotifications();
    
    std::string notification_time = notifications[0];
    try {
        event_type = event.getType() == Checker::EventType::WHILE_NOT_DONE ? "WHILE_NOT_DONE" : "ONE_TIME";
    } catch (const std::invalid_argument &e) {
        event_type = "ONE_TIME";
    }

    auto user_id = db.getUserIdByTelegramId(telegram_id);
    int user_tz = db.getUserTimeZone(telegram_id);

    event_time = adjustEventTime(event_time, user_tz);
    notification_time = adjustEventTime(notification_time, user_tz);
    std::cout<<"Event time: "<<event_time<<"\nNotification time: "<<notification_time<<endl;
    if (user_id.has_value()) {
        auto event_id = db.insertEvent(user_id, event_name, event_time, event_type);
        if (event_id.has_value()) {
            db.insertNotification(event_id.value(), notification_time);

            bot.getApi().sendMessage(message->chat->id, "Event has been added successfully");
        }
        else {
            bot.getApi().sendMessage(message->chat->id, "Something went wrong. Could not save event");
        }
        
    } else {
        bot.getApi().sendMessage(message->chat->id, "Failed to get user id");
    }
}

std::string adjustEventTime(const std::string& event_time, int user_timezone) {
    std::tm tm = {};
    std::istringstream ss(event_time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse event time: " + event_time);
    }

    std::time_t utc_time = std::mktime(&tm);
    if (utc_time == -1) {
        throw std::runtime_error("Failed to convert time to UTC");
    }

    utc_time -= user_timezone * 3600;

    std::tm* adjusted_tm = std::gmtime(&utc_time);
    if (!adjusted_tm) {
        throw std::runtime_error("Failed to convert adjusted time to std::tm");
    }

    std::ostringstream adjusted_time_ss;
    adjusted_time_ss << std::put_time(adjusted_tm, "%Y-%m-%dT%H:%M:%SZ");
    return adjusted_time_ss.str();
}

}