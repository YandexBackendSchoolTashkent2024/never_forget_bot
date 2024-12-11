#pragma once

#include <pqxx/pqxx>
#include <tgbot/tgbot.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <optional>
#include <type_traits>
#include <iomanip>
#include <sstream>
#include <locale>
#include <codecvt>
#include <type_traits>

#include "../models/event.hpp"
#include "../models/user.hpp"
#include "../models/notification.hpp"


namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);

    ~Database();

    std::vector<std::vector<std::string>> fetchPendingNotifications();

    std::vector<std::string> fetchAllIds();

    void updateSentTimeForNotification(const std::string& notification_id);

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

    std::optional<std::string> insertEvent(const std::optional<std::string>& user_id, 
                                       const std::string& event_name, 
                                       const std::string& event_time, 
                                       const std::string& event_type);

    std::optional<std::string> insertNotification(const std::string& event_id, const std::string& notification_time);

    void updateUserTimeZone(long telegram_id, const int& offset);

    int getUserTimeZone(long telegram_id);

    std::vector<Event> getEventsOrderedByTimeDesc(long telegram_id);

    Notification delayNotification(const std::string &notification_id, const std::string &interval);

    std::optional<std::string> changeEventStatus(const std::string &event_id, const std::string &action);

    std::optional<std::string> getUserIdByTelegramId(long telegram_id);

private:
    pqxx::connection* conn;
};

}
