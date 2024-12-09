#include "db.hpp"

namespace NeverForgetBot {

std::optional<std::string> Database::insertEvent(long telegram_id, const std::optional<std::string>& username, const std::optional<std::string>& name) {
    if (!conn || !conn->is_open()) {
        std::cerr << "Database connection is not open\n";
        return std::nullopt;
    }

    // Retrieve user_id from telegram_id
    std::optional<std::string> user_id = db.getUserIdByTelegramId(telegram_id);
    if (!user_id.has_value()) {
        std::cerr << "Database connection is not open\n";
        bot.getApi().sendMessage(message->chat->id, "User not found, please /start the bot first.");
        return;
    }
    else{
        std::cerr << "Database connection is not open\n";
        bot.getApi().sendMessage(message->chat->id, "User id: ">> user_id);

    }
    return;
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
