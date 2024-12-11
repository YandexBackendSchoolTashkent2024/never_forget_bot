#include "db.hpp"

namespace NeverForgetBot {

std::optional<std::string> Database::insertUser(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Подключение к базе данных не открыто\n";
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
            std::cerr << "Неожиданное количество строк возвращено\n";
            return std::nullopt;
        }
    } catch (const std::exception &e) {
        std::cerr << "Не удалось добавить пользователя: " << e.what() << std::endl;
        return std::nullopt;
    }
}

void Database::updateUserTimeZone(long telegram_id, const int& tzoffset) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Подключение к базе данных не открыто\n";
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "UPDATE \"user\" SET \"timezone\" = " + txn.quote(tzoffset) + ", \"updated_at\" = NOW() "
            "WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        txn.exec(query);
        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "Не удалось обновить часовой пояс: " << e.what() << std::endl;
    }
}

int Database::getUserTimeZone(long telegram_id) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Подключение к базе данных не открыто\n";
        throw std::runtime_error("Подключение к базе данных не открыто");
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "SELECT \"timezone\" FROM \"user\" WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cerr << "Пользователь с telegram_id: " << telegram_id << " не найден\n";
            throw std::runtime_error("Пользователь не найден");
        }

        return r[0][0].as<int>();
    } catch (const std::exception &e) {
        std::cerr << "Не удалось получить часовой пояс: " << e.what() << std::endl;
        throw;
    }
}

std::optional<std::string> Database::getUserIdByTelegramId(long telegram_id) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Подключение к базе данных не открыто\n";
        return std::nullopt;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = 
            "SELECT \"id\" FROM \"user\" WHERE \"telegram_id\" = " + txn.quote(telegram_id) + ";";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cerr << "Пользователь с telegram_id: " << telegram_id << " не найден\n";
            return std::nullopt;
        }

        // Предполагается, что "id" хранится как строка UUID в базе данных
        std::optional<std::string> user_id = r[0][0].as<std::string>();
        txn.commit();
        return user_id.value();
    } catch (const std::exception &e) {
        std::cerr << "Не удалось получить user_id: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::vector<std::string> Database::fetchAllIds() {
    std::vector<std::string> chatIds;

    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return chatIds;
    }

    try {
        pqxx::work txn(*conn);
        std::string query = R"(
            SELECT telegram_id
            FROM "user"
        )";

        pqxx::result r = txn.exec(query);

        if (r.empty()) {
            std::cout << "No users found\n";
            std::runtime_error("No users found\n");
        }

        for (const auto& row : r) {
            std::string chatId = row[0].as<std::string>();
            chatIds.push_back(chatId);
        }

        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "Fetch telegram ids failed: " << e.what() << std::endl;
    }

    return chatIds;
}

}
