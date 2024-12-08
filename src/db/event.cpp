// src/db/db.cpp

#include "db.hpp"
#include <iostream>

namespace NeverForgetBot {

std::vector<Event> Database::getEventsOrderedByTimeDesc(long telegram_id) {
    std::vector<Event> events;

    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return events;
    }

    try {
        pqxx::work txn(*conn);
        
        std::string user_id_query = "SELECT id FROM \"user\" WHERE telegram_id = " + txn.quote(telegram_id) + ";";
        pqxx::result user_result = txn.exec(user_id_query);
        
        if (user_result.empty()) {
            std::cerr << "No user found with telegram_id: " << telegram_id << std::endl;
            txn.commit();
            return events;
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
            event.userId = row["user_id"].as<std::string>();
            event.name = row["name"].as<std::string>();
            event.time = row["time"].as<std::string>();
            event.status = row["status"].as<std::string>();
            event.createdAt = row["created_at"].as<std::string>();
            event.updatedAt = row["updated_at"].as<std::string>();

            events.push_back(event);
        }

    } catch (const std::exception& e) {
        std::cerr << "Select events failed: " << e.what() << std::endl;
    }

    return events;
}

}
