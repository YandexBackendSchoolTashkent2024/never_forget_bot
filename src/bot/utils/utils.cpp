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
    std::string event_name = event.getNameEvent();
    std::string event_time = event.getTime();
    std::string event_type;

    // Ensure `event_type` is valid
    try {
        event_type = event.getType() == Checker::EventType::WHILE_NOT_DONE ? "WHILE_NOT_DONE" : "ONE_TIME";
    } catch (const std::invalid_argument &e) {
        event_type = "ONE_TIME"; // Default to ONE_TIME
    }

    auto user_id = db.getUserIdByTelegramId(telegram_id);
    int user_tz = db.getUserTimeZone(telegram_id);

    event_time = adjustEventTime(event_time, user_tz);
    if (user_id.has_value()) {
        auto event_id = db.insertEvent(user_id, event_name, event_time, event_type);
        if (event_id.has_value()) {
            bot.getApi().sendMessage(message->chat->id, "Event has been added successfully");
        }
        else {
            bot.getApi().sendMessage(message->chat->id, "Something went wrong. Could not save event");
        }
        
    } else {
        bot.getApi().sendMessage(message->chat->id, "Failed to get user id");
    }
}

// Function to adjust event time based on user timezone
std::string adjustEventTime(const std::string& event_time, int user_timezone) {
    // Parse the event time string into a std::tm structure
    std::tm tm = {};
    std::istringstream ss(event_time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse event time: " + event_time);
    }

    // Convert the std::tm structure to time_t (UTC)
    std::time_t utc_time = std::mktime(&tm);
    if (utc_time == -1) {
        throw std::runtime_error("Failed to convert time to UTC");
    }

    // Adjust for the user's timezone (subtract timezone offset in seconds)
    utc_time -= user_timezone * 3600;

    // Convert the adjusted time_t back to std::tm
    std::tm* adjusted_tm = std::gmtime(&utc_time);
    if (!adjusted_tm) {
        throw std::runtime_error("Failed to convert adjusted time to std::tm");
    }

    // Format the adjusted time back to ISO 8601 string
    std::ostringstream adjusted_time_ss;
    adjusted_time_ss << std::put_time(adjusted_tm, "%Y-%m-%dT%H:%M:%SZ");
    return adjusted_time_ss.str();
}


} // namespace NeverForgetBot::Utils
