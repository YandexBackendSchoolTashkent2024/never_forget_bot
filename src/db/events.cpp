#include "db.hpp"

namespace NeverForgetBot {

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
            return r[0][0].as<std::string>(); // Return the ID of the new notification
        } else {
            std::cerr << "Insert notification failed: No ID returned\n";
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        std::cerr << "Insert notification failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}


} // namespace NeverForgetBot