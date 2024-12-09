#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>
#include "../models/event.hpp"
#include <tgbot/Bot.h>
#include <type_traits>

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> fetchPendingNotifications();

    void updateSentTimeForNotification(const std::string& notification_id);

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

    void updateUserTimeZone(long telegram_id, const int& offset);
  
    int getUserTimeZone(long telegram_id);
  
    std::vector<Event> getEventsOrderedByTimeDesc(long telegram_id);
  
    std::optional<std::string> deleteEvent(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);

    void updateEventStatus(const std::string event_id,const std::string status);
private:
    pqxx::connection* conn;
};

}
