#include <iostream>

#include "db.hpp"

namespace NeverForgetBot {

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

}
