#include "db.hpp"
#include "../bot/handlers/events/events.hpp"
#include <strstream>

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

    void Database::updateSentTimeForNotification(const std::string& notification_id) {
        if (!conn || !conn->is_open()) {
            std::cerr << "Database connection is not open\n";
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
        } catch (const std::exception &e) {
            std::cerr << "Update sent time failed: " << e.what() << std::endl;
        }
    }

    std::vector<Event> Database::getEventsOrderedByTimeDesc(long telegram_id) {
        std::vector<Event> events;

        if (!conn || !conn->is_open()) {
            std::cerr << "Database connection is not open\n";
        }

        try {
            pqxx::work txn(*conn);
            
            std::string user_id_query = "SELECT id FROM \"user\" WHERE telegram_id = " + txn.quote(telegram_id) + ";";
            pqxx::result user_result = txn.exec(user_id_query);
            
            if (user_result.empty()) {
                std::cerr << "No user found with telegram_id: " << telegram_id << std::endl;
                txn.commit();
                throw std::runtime_error("No user");
            }

            std::string user_id = user_result[0]["id"].as<std::string>();

            std::string events_query = "SELECT id, user_id, name, time, status, created_at, updated_at "
                                    "FROM \"event\" "
                                    "WHERE user_id = " + txn.quote(user_id) + " "
                                    "ORDER BY time DESC;";
            pqxx::result r = txn.exec(events_query);
            txn.commit();

            for (const auto& row : r) {
                Event event;
                event.id = row["id"].as<std::string>();
                event.user_id = row["user_id"].as<std::string>();
                event.name = row["name"].as<std::string>();
                event.time = row["time"].as<std::string>();
                std::string status_str = row["status"].as<std::string>();
                event.setStatus(status_str);
                event.created_at = row["created_at"].as<std::string>();
                event.updated_at = row["updated_at"].as<std::string>();

                events.push_back(event);
            }

        } catch (const std::exception& e) {
            std::cerr << "Select events failed: " << e.what() << std::endl;
        }

        return events;
    }

}
