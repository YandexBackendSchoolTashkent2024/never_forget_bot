#include <cstdlib>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/utils/daemon_run.hpp>
#include <tgbot/tgbot.h>

#include "bot/handlers/command_handlers.hpp"
#include "bot/utils/utils.hpp"

#include "parse_msg/checker.hpp"
#include "parse_msg/parse.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <optional>


int main(int argc, char* argv[]) {
    std::string db_name(getenv("POSTGRES_DB"));
    std::string token(getenv("TOKEN"));
    printf("Token: %s\n", token.c_str());
    printf("Postgres DB: %s\n", db_name.c_str());

    auto component_list = userver::components::MinimalServerComponentList()
                      .Append<userver::components::Postgres>(db_name)
                      .Append<userver::components::HttpClient>();

    TgBot::Bot bot(token);

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onStartCommand(message, bot);
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

    // Start long polling
    NeverForgetBot::Utils::startLongPolling(bot);

    // Run the server
    userver::utils::DaemonMain(argc, argv, component_list);

    return 0;
}

