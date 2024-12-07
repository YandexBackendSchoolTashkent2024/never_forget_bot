#include "db.hpp"
#include <iostream>

Database::Database(const std::string& connectionStr) {
    try {
        conn = new pqxx::connection(connectionStr);
        if (!conn->is_open()) {
            std::cerr << "Cannot open database connection\n";
            delete conn;
            conn = nullptr;
        } else {
            std::cout << "Connected to database successfully\n";
        }
    } catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        conn = nullptr;
    }
}

Database::~Database() {
    if (conn && conn->is_open()) {
        conn->disconnect();
    }
    delete conn;
}

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