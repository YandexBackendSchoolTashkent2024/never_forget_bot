#include "db.hpp"

namespace NeverForgetBot {

Notification Database::delayNotification(const std::string &notification_id, const std::string &interval) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        throw std::runtime_error("Database connection is not open");
    }

    try {
        pqxx::work txn(*conn);

        conn->prepare("delay_notification",
            "UPDATE notifications "
            "SET time = NOW() + $1::INTERVAL "
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
