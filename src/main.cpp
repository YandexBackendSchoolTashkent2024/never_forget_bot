#include <iostream>
#include <cstdlib>

#include <tgbot/tgbot.h>

#include "db/db.hpp"
#include "bot/utils/utils.hpp"
#include "bot/handlers/command_handlers.hpp"

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

    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onStartCommand(message, bot);
        NeverForgetBot::Utils::saveUserIfNotExists(message, bot, db);
    });

    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onHelpCommand(message, bot);
    });
    
    bot.getEvents().onCommand("upcoming_events", [&bot, &db](TgBot::Message::Ptr message) {
        long telegram_id = message->from->id;

        std::vector<NeverForgetBot::Event> events = db.getEventsOrderedByTimeDesc(telegram_id);

        if (events.empty()) {
            bot.getApi().sendMessage(message->chat->id, "No events found.");
            return;
        }

        std::string response = "Your Events:\n\n";
        for (const auto& event : events) {
            response += "ID: " + event.id + "\n";
            response += "Name: " + event.name + "\n";
            response += "Time: " + event.time + "\n";
            response += "Status: " + event.status + "\n";
            response += "Created At: " + event.createdAt + "\n";
            response += "Updated At: " + event.updatedAt + "\n\n";
        }

        bot.getApi().sendMessage(message->chat->id, response);
    });

    bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Invalid command");
    });

    bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "You typed: " + message->text);
    });

    NeverForgetBot::Utils::startLongPolling(bot);

    return 0;
}
