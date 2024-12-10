#include "db.hpp"

namespace NeverForgetBot {

std::vector<std::vector<std::string>> Database::fetchPendingNotifications() {
    std::vector<std::vector<std::string>> notifications;

    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return notifications;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = R"(
            SELECT n.id, e.id, u.telegram_id, e.name, e.time, n.time
            FROM "notification" n
            JOIN "event" e ON e.id = n.event_id
            JOIN "user" u ON u.id = e.user_id
            WHERE n.sent_time IS NULL AND e.status = 'PENDING'
            AND n.time <= NOW()
        )";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cout << "No pending notifications found\n";
        }

        for (const auto& row : r) {
            std::string notification_id = row[0].as<std::string>();
            std::string event_id = row[1].as<std::string>();
            std::string telegram_id = row[2].as<std::string>();
            std::string event_name = row[3].as<std::string>();
            std::string event_time = row[4].as<std::string>();
            std::string notification_time = row[5].as<std::string>();

            notifications.push_back({
                notification_id,
                event_id,
                telegram_id,
                event_name,
                event_time,
                notification_time
            });
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
        //todo error
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
        //todo error
    }
}

Notification Database::delayNotification(const std::string &notification_id, const std::string &interval) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        throw std::runtime_error("Database connection is not open");
    }

    try {
        pqxx::work txn(*conn);

        conn->prepare("delay_notification",
            "UPDATE notification "
            "SET time = time + $1::INTERVAL, "
            "sent_time = NULL "
            "WHERE id = $2 "
            "RETURNING id, time, sent_time, created_at, updated_at"
        );
        pqxx::result result = txn.exec_prepared("delay_notification", interval, notification_id);

        if (result.empty()) {
            throw std::runtime_error("No notification found with id: " + notification_id);
        }

        pqxx::row row = result[0];

        Notification updNotification;
        updNotification.id = row["id"].as<std::string>();
        updNotification.time = row["time"].as<std::string>();

        if (!row["sent_time"].is_null()) {
            updNotification.sentTime = row["sent_time"].as<std::string>();
        } else {
            updNotification.sentTime.reset();
        }

        updNotification.createdAt = row["created_at"].as<std::string>();
        updNotification.updatedAt = row["updated_at"].as<std::string>();

        txn.commit();

        return updNotification;
    } catch (const std::exception& e) {
        std::cerr << "Failed to delay notification: " << e.what() << std::endl;
        throw;
    }
}

}
