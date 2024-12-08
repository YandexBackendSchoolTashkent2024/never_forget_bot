#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <optional>

namespace NeverForgetBot {

class Database {
public:
    Database(const std::string& connectionStr);
    ~Database();
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> fetchPendingNotifications();

        // Update sent time for a notification
        bool updateSentTimeForNotification(const std::string& notification_id);

    std::optional<std::string> insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name);
    bool updateUserTimeZone(long telegram_id, const std::string& timezone);
private:
    pqxx::connection* conn;
};

}
