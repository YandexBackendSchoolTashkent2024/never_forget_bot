#include "db.hpp"
#include <pqxx/row.hxx>
#include <strstream>

namespace NeverForgetBot {

Notification Database::delayNotification(const std::string &notification_id, const std::string &interval) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
    }

    try {
        pqxx::work txn(*conn);

        std::ostrstream query;
        query << "UPDATE notifications ";
        query << "SET time = NOW() + ";
        query << interval << "::INTERVAL ";
        query << "WHERE id = " << notification_id;
        query << "RETURNING *";

        pqxx::result result = txn.exec(query.str());
        cout << query.str();

        txn.commit();

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

        return updNotification;
    } catch (const std::exception& e) {
        std::cerr << "Failed to delay notification: " << e.what() << std::endl;
    }
}

}
