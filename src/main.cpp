#include <tgbot/tgbot.h>
#include "db.hpp"
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

int main() {
    // Load environment variables
    const char* botToken = std::getenv("TELEGRAM_BOT_TOKEN");
    const char* dbHost = std::getenv("DB_HOST");
    const char* dbPort = std::getenv("DB_PORT");
    const char* dbName = std::getenv("DB_NAME");
    const char* dbUser = std::getenv("DB_USER");
    const char* dbPassword = std::getenv("DB_PASSWORD");

    if (!botToken || !dbHost || !dbPort || !dbName || !dbUser || !dbPassword) {
        std::cerr << "Missing environment variables. Please check your .env file.\n";
        return 1;
    }

    // Construct DB connection string
    std::string connectionStr = "host=" + std::string(dbHost) +
                                " port=" + std::string(dbPort) +
                                " dbname=" + std::string(dbName) +
                                " user=" + std::string(dbUser) +
                                " password=" + std::string(dbPassword);

    // Initialize Database
    Database db(connectionStr);

    // Initialize Bot
    TgBot::Bot bot(botToken);

    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        long telegram_id = message->from->id;
        std::optional<std::string> username;
        std::optional<std::string> name;

        if (!message->from->username.empty()) {
            username = message->from->username;
        }

        if (!message->from->firstName.empty() || !message->from->lastName.empty()) {
            std::string fullName = message->from->firstName;
            if (!message->from->lastName.empty()) {
                fullName += " " + message->from->lastName;
            }
            name = fullName;
        }

        auto userId = db.insertUser(telegram_id, username, name);

        if (userId.has_value()) {
            bot.getApi().sendMessage(message->chat->id, "Your ID has been saved: " + userId.value());
        } else {
            bot.getApi().sendMessage(message->chat->id, "Failed to save your ID.");
        }

        // Here you can store the user's chat ID if you want to send periodic messages to this user.
        // For example, you might store it in a global variable or database.
    });

    // Launch a background thread that sends "hello world" every minute
    // Replace SOME_CHAT_ID with a valid chat ID where your bot can send messages
    // const int64_t SOME_CHAT_ID = 266798451; // Replace with the actual chat ID
    // std::thread([&bot, SOME_CHAT_ID]() {
    //     while (true) {
    //         std::this_thread::sleep_for(std::chrono::minutes(1));
    //         try {
    //             bot.getApi().sendMessage(SOME_CHAT_ID, "Hello world");
    //         } catch (const std::exception &e) {
    //             std::cerr << "Error sending message: " << e.what() << std::endl;
    //         }
    //     }
    // }).detach();

    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username.c_str() << std::endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
