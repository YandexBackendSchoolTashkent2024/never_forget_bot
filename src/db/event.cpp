#include "db.hpp"
#include <iostream>
#include <pqxx/pqxx>
#include <vector>
#include <tuple>

namespace NeverForgetBot {

std::vector<std::tuple<std::string, std::string, std::string, std::string>> Database::fetchPendingNotifications() {
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> notifications;

    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return notifications;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = R"(
            SELECT n.id, u.telegram_id, e.name, n.time
            FROM "notification" n
            JOIN "event" e ON e.id = n.event_id
            JOIN "user" u ON u.id = e.user_id
            WHERE n.sent_time IS NULL AND e.status = 'PENDING'
            AND e.time <= NOW() + INTERVAL '1 minute'
        )";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cout << "No pending notifications found\n";
        }

        for (const auto& row : r) {
            std::string notification_id = row[0].as<std::string>();
            std::string telegram_id = row[1].as<std::string>();
            std::string event_name = row[2].as<std::string>();
            std::string event_time = row[3].as<std::string>();
            notifications.emplace_back(notification_id, telegram_id, event_name, event_time);
        }

        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "Fetch pending notifications failed: " << e.what() << std::endl;
    }

    return notifications;
}

bool Database::updateSentTimeForNotification(const std::string& notification_id) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return false;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = R"(
            UPDATE "notification"
            SET "sent_time" = NOW()
            WHERE "id" = )" + txn.quote(notification_id) + ";";

        txn.exec(query);
        txn.commit();
        std::cout << "Notification with ID " << notification_id << " updated with sent time.\n";
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Update sent time failed: " << e.what() << std::endl;
        return false;
    }
}

} // namespace NeverForgetBot
