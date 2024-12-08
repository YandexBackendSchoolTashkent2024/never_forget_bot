#include <iostream>
#include <cstdlib>

#include <tgbot/tgbot.h>

#include "db/db.hpp"
#include "bot/utils/utils.hpp"
#include "bot/handlers/command_handlers.hpp"


#include "chrono/periodic_task.hpp"


#include "parse_msg/checker.hpp"
#include "parse_msg/parse.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <optional>

int main() {
    // Load environment variables
    const char* botToken = std::getenv("TOKEN");
    const char* dbHost = std::getenv("POSTGRES_HOST");
    const char* dbPort = std::getenv("POSTGRES_PORT");
    const char* dbName = std::getenv("POSTGRES_DB");
    const char* dbUser = std::getenv("POSTGRES_USER");
    const char* dbPassword = std::getenv("POSTGRES_PASSWORD");

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

    NeverForgetBot::Database db(connectionStr);
    
    TgBot::Bot bot(botToken);


    // here crono job starting 
    chrono_task::start_periodic_task(db, bot);
    //

    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onStartCommand(message, bot);
        NeverForgetBot::Utils::saveUserIfNotExists(message, bot, db);
    });

    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onHelpCommand(message, bot);
    });

    bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Invalid command");
    });

    bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
        std::string message_text;
        try {
            Checker checker = processMessage(message->text);

            message_text += "Event Name: " + checker.getNameEvent() + "\n";
            message_text += "Event Time: " + checker.getTime() + "\n";
            message_text += std::string("Event Type: ") +
                            (checker.getType() == Checker::EventType::ONE_TIME ? "one-time" : "while-not-done") + "\n";

            const auto& notifications = checker.getNotifications();
            if (!notifications.empty()) {
                message_text += "Notifications: ";
                for (const auto& n : notifications) {
                    message_text += n + " ";
                }
                message_text += "\n";
            } else {
                message_text += "Notifications: null\n";
            }
        } catch (const std::exception& e) {
            message_text = "Error processing your message: " + std::string(e.what());
        }
        if (!message_text.empty()) {
            bot.getApi().sendMessage(message->chat->id, message_text);
        } else {
            bot.getApi().sendMessage(message->chat->id, "I couldn't understand your message. Please try again.");
        }
    });

    NeverForgetBot::Utils::startLongPolling(bot);
    // here crono job ending

    chrono_task::stop_periodic_task();
    return 0;
}
