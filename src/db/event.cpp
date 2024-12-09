#include "../bot/handlers/events/events.hpp"

namespace NeverForgetBot {

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
        }

        std::string user_id = user_result[0]["id"].as<std::string>();

        conn->prepare("get_upcoming_events",
            "SELECT id, user_id, name, time, status, created_at, updated_at "
            "FROM event "
            "WHERE user_id = $1 "
            "AND status = 'PENDING' "
            "ORDER BY time DESC"
        );
        pqxx::result r = txn.exec_prepared("get_upcoming_events", user_id);

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

    } catch (const std::exception &e) {
        std::cerr << "Select events failed: " << e.what() << std::endl;
    }

    return events;
}

std::optional<std::string> Database::changeEventStatus(const std::string &notification_id, const std::string &action) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);
        conn->prepare("update_event_status",
            "UPDATE event "
            "SET status = $1, updated_at = NOW() "
            "FROM notification "
            "WHERE notification.id = $2 "
            "AND notification.event_id = event.id "
            "RETURNING event.id"
        );
        pqxx::result result = txn.exec_prepared("update_event_status", action, notification_id);

        if (result.empty()) {
            std::cerr << "Failed to update event status for notification: " << notification_id << std::endl;
            return std::nullopt;
        }

        txn.commit();

        return result[0]["id"].as<std::string>();
    } catch (std::exception &e) {
        std::cerr << "Failed to update event status for notification: " << notification_id << std::endl;
        return std::nullopt;
    }
}

std::optional<std::string> Database::insertEvent(const std::optional<std::string>& user_id,
                                                 const std::string& event_name,
                                                 const std::string& event_time,
                                                 const std::string& event_type ) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);

        // Convert `user_id` to a SQL-compatible value
        std::string user_id_str = user_id.has_value() ? txn.quote(user_id.value()) : "NULL";
        std::cout << "Event type being inserted: " << event_type << std::endl;

        // Construct the query
        std::string query = "INSERT INTO event (user_id, name, time, type, status) VALUES ("
                    + user_id_str + ", " 
                    + txn.quote(event_name) + ", "
                    + txn.quote(event_time) + ", "
                    + txn.quote(event_type) + ", "
                    + "'PENDING'" + ") RETURNING id;";


        pqxx::result r = txn.exec(query);
        txn.commit();

        if (!r.empty()) {
            return r[0][0].as<std::string>();
        } else {
            std::cerr << "Insert event failed: No ID returned\n";
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        std::cerr << "Insert event failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<std::string> Database::insertNotification(const std::string& event_id, 
                                                         const std::string& notification_time) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);

        // Construct the query
        std::string query = "INSERT INTO notification (event_id, time) VALUES ("
                            + txn.quote(event_id) + ", "
                            + txn.quote(notification_time) + ") RETURNING id;";

        // Execute the query
        pqxx::result r = txn.exec(query);
        txn.commit();

        if (!r.empty()) {
            return r[0][0].as<std::string>();
        } else {
            std::cerr << "Insert notification failed: No ID returned\n";
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        std::cerr << "Insert notification failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}

}
