#include "utils.hpp"
#include <filesystem>
#include <optional>

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
        try {
            bot.getApi().deleteWebhook();
            bot.getApi().setMyCommands(getBotCommands());
            bot.getApi().setMyDescription(getBotDescription());
            bot.getApi().setMyShortDescription("An awesome bot to make you forget about forgetting");
        } catch (std::exception& e) {
            printf("error: %s\n", e.what());
        }
        TgBot::TgLongPoll longPoll(bot);
        std::cerr << "Starting long poll" << std::endl;
        while (true) {
            printf("Long poll continue\n");
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
        { "/start", "запуск бота" },
        { "/change_tz","изменить"}
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

void saveEvent(TgBot::Message::Ptr message, TgBot::Bot &bot, NeverForgetBot::Database &db, Checker &event) {
    long telegram_id = message->from->id;
    std::string event_name = event.getNameEvent();
    std::string event_time = event.getTime();
    std::string event_type;
    vector<std::string> notifications = event.getNotifications();
    std::string notification_time;
    auto user_id = db.getUserIdByTelegramId(telegram_id);

    std::string msg = "Детали созданного события:";
    msg += "\nНазвание события: " + event_name;
    msg += "\nВремя события: " + manual_format_in_russian(event_time);
    if (notifications.empty()) {
        msg += "\nВремя уведомления: " + manual_format_in_russian(event_time); 
    }

    int user_tz = db.getUserTimeZone(telegram_id);
    event_time = adjustEventTime(event_time, user_tz);
    if (!notifications.empty()){
        notification_time = notifications[0];
        msg += "\nВремя уведомления: " + manual_format_in_russian(notification_time); 
        notification_time = adjustEventTime(notification_time, user_tz);
    }
    else {
        notification_time = event_time;
    }

    try {
        event_type = event.getType() == Checker::EventType::WHILE_NOT_DONE ? "WHILE_NOT_DONE" : "ONE_TIME";
    } catch (const std::invalid_argument &e) {
        event_type = "ONE_TIME";
    }

    std::cout << "Время события: " << event_time << "\nВремя уведомления: " << notification_time << std::endl;
    if (user_id.has_value()) {
        auto event_id = db.insertEvent(user_id, event_name, event_time, event_type);
        if (event_id.has_value()) {
            db.insertNotification(event_id.value(), notification_time);

            bot.getApi().sendMessage(message->chat->id, "Событие успешно добавлено");
            bot.getApi().sendMessage(message->chat->id, msg);
        }
        else {
            bot.getApi().sendMessage(message->chat->id, "Произошла ошибка. Не удалось сохранить событие");
        }
    } else {
        bot.getApi().sendMessage(message->chat->id, "Не удалось получить идентификатор пользователя");
    }
}

std::string adjustEventTime(const std::string& event_time, int user_timezone) {
    std::tm tm = {};
    std::istringstream ss(event_time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
        throw std::runtime_error("Ошибка разбора времени события: " + event_time);
    }

    std::time_t utc_time = std::mktime(&tm);
    if (utc_time == -1) {
        throw std::runtime_error("Ошибка преобразования времени в UTC");
    }

    utc_time -= user_timezone * 3600;

    std::tm* adjusted_tm = std::gmtime(&utc_time);
    if (!adjusted_tm) {
        throw std::runtime_error("Ошибка преобразования времени в структуру std::tm");
    }

    std::ostringstream adjusted_time_ss;
    adjusted_time_ss << std::put_time(adjusted_tm, "%Y-%m-%dT%H:%M:%SZ");
    return adjusted_time_ss.str();
}

std::optional<std::string> convertToISO(long telegram_id, const std::string& timestamp_time, NeverForgetBot::Database& db) {
    int timezone = db.getUserTimeZone(telegram_id);

    auto space_idx = timestamp_time.find(" ");
    if (space_idx == std::string::npos) {
        return std::nullopt;
    }

    try {
        std::string date = timestamp_time.substr(0, space_idx);
        std::string time = timestamp_time.substr(space_idx + 1);

        std::tm tm_time = {};
        std::istringstream ss(timestamp_time);
        ss >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            return std::nullopt;
        }

        std::time_t time_utc = std::mktime(&tm_time);
        if (time_utc == -1) {
            return std::nullopt;
        }

        time_utc += timezone * 3600;
        std::tm* adjusted_tm = std::gmtime(&time_utc);
        if (!adjusted_tm) {
            return std::nullopt;
        }

        std::ostringstream iso_ss;
        iso_ss << std::put_time(adjusted_tm, "%Y-%m-%dT%H:%M:%SZ");
        return iso_ss.str();
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string manual_format_in_russian(const std::string& iso_datetime) {
    const std::vector<std::string> russian_months = {
        "января", "февраля", "марта", "апреля", "мая", "июня",
        "июля", "августа", "сентября", "октября", "ноября", "декабря"
    };

    try {
        std::tm tm = {};
        std::istringstream ss(iso_datetime);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

        if (ss.fail()) {
            throw std::invalid_argument("Ошибка разбора даты/времени!");
        }

        std::ostringstream oss;
        oss << tm.tm_mday << " " << russian_months[tm.tm_mon] << " " << (1900 + tm.tm_year) << " г., ";
        oss << std::setw(2) << std::setfill('0') << tm.tm_hour << ":"
            << std::setw(2) << std::setfill('0') << tm.tm_min;

        return oss.str();
    } catch (const std::exception& e) {
        return "Ошибка обработки даты";
    }
}
}
