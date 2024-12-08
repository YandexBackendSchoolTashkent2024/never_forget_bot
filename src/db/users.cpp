#include "db.hpp"

namespace NeverForgetBot {

std::optional<std::string> Database::insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = "INSERT INTO \"user\" (telegram_id, username, name) VALUES (" + txn.quote(telegram_id) + ", ";

        if (username.has_value()) {
            query += txn.quote(username.value());
        } else {
            query += "NULL";
        }

        query += ", ";

        if (name.has_value()) {
            query += txn.quote(name.value());
        } else {
            query += "NULL";
        }

        query += ") RETURNING telegram_id;";

        pqxx::result r = txn.exec(query);
        txn.commit();

        if (r.size() == 1) {
            return r[0][0].as<std::string>();
        } else {
            std::cerr << "Unexpected number of rows returned\n";
            return std::nullopt;
        }
    } catch (const std::exception &e) {
        std::cerr << "Insert user failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool Database::updateUserTimeZone(long telegram_id, const std::string& timezone) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return false;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "UPDATE \"user\" SET \"timezone\" = " + txn.quote(timezone) + ", \"updated_at\" = NOW() "
            "WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        txn.exec(query);
        txn.commit();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Update timezone failed: " << e.what() << std::endl;
        return false;
    }
}
}
