#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>
#include "../models/event.hpp"
#include <tgbot/Bot.h>

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> fetchPendingNotifications();

        // Update sent time for a notification
        bool updateSentTimeForNotification(const std::string& notification_id);

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

    std::optional<std::string> insertEvent(const std::optional<std::string>& user_id, 
                                       const std::string& event_name, 
                                       const std::string& event_time, 
                                       const std::string& event_type);

    std::optional<std::string> insertNotification(const std::string& event_id, const std::string& notification_time);



    bool updateUserTimeZone(long telegram_id, const int& offset);
  
    int getUserTimeZone(long telegram_id);
  
    void getEventsOrderedByTimeDesc(TgBot::Bot &bot, long telegram_id);
  
    std::optional<std::string> deleteEvent(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

    std::optional<std::string> getUserIdByTelegramId(long telegram_id);


private:
    pqxx::connection* conn;
};

}
