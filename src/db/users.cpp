#include "db.hpp"

namespace NeverForgetBot {

std::optional<std::string> Database::insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = "INSERT INTO \"user\" (telegram_id, username, name) VALUES ("
                            + txn.quote(telegram_id) + ", ";

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

        query += ") ON CONFLICT (telegram_id) DO UPDATE SET telegram_id = EXCLUDED.telegram_id RETURNING telegram_id;";

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

bool Database::updateUserTimeZone(long telegram_id, const int& offset) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return false;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "UPDATE \"user\" SET \"timezone\" = " + txn.quote(offset) + ", \"updated_at\" = NOW() "
            "WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        txn.exec(query);
        txn.commit();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Update timezone failed: " << e.what() << std::endl;
        return false;
    }
}

int Database::getUserTimeZone(long telegram_id) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        throw std::runtime_error("Database connection is not open");
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "SELECT \"timezone\" FROM \"user\" WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cerr << "No user found with telegram_id: " << telegram_id << "\n";
            throw std::runtime_error("User not found");
        }

        return r[0][0].as<int>();
    } catch (const std::exception &e) {
        std::cerr << "Get timezone failed: " << e.what() << std::endl;
        throw;
    }
}

std::optional<std::string> Database::getUserIdByTelegramId(long telegram_id) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "SELECT \"id\" FROM \"user\" WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cerr << "No user found with telegram_id: " << telegram_id << "\n";
            return std::nullopt;
        }

        // Assuming "id" is stored as a UUID string in the DB
        std::string user_id = r[0][0].as<std::string>();
        txn.commit();
        return user_id;
    } catch (const std::exception &e) {
        std::cerr << "Get user_id failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}


}
