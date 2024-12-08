// src/db/db.cpp

#include "db.hpp"
#include <iostream>
#include "../bot/handlers/events/events.hpp"

namespace NeverForgetBot {

EventStatus mapStatus(const std::string& statusStr) {
    if (statusStr == "PENDING") {
        return EventStatus::PENDING;
    } else if (statusStr == "COMPLETED") {
        return EventStatus::COMPLETED;
    } else if (statusStr == "NOT_COMPLETED") {
        return EventStatus::NOT_COMPLETED;
    } else {
        throw std::invalid_argument("Unknown status: " + statusStr);
    }
}

// If status is stored as an integer
EventStatus mapStatus(int statusInt) {
    switch (statusInt) {
        case 0:
            return EventStatus::PENDING;
        case 1:
            return EventStatus::COMPLETED;
        case 2:
            return EventStatus::NOT_COMPLETED;
        default:
            throw std::invalid_argument("Unknown status code: " + std::to_string(statusInt));
    }
}

void Database::getEventsOrderedByTimeDesc(TgBot::Bot &bot, long telegram_id) {
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
            event.status = mapStatus(status_str);
            event.created_at = row["created_at"].as<std::string>();
            event.updated_at = row["updated_at"].as<std::string>();

            events.push_back(event);
        }

    } catch (const std::exception& e) {
        std::cerr << "Select events failed: " << e.what() << std::endl;
    }

    Events::send_events(bot, telegram_id, events);
}

}
