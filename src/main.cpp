#include <iostream>
#include <cstdlib>
#include <tgbot/tgbot.h>
#include "db/db.hpp"
#include "bot/utils/utils.hpp"
#include "bot/commands/commands.hpp"
#include "bot/handlers/notifications/notifications.hpp"
#include "bot/handlers/callback/callback.hpp"
#include "chrono/periodic_task.hpp"
#include "parse_msg/checker.hpp"
#include "parse_msg/parse.hpp"
#include <vector>

int main() {
    const char* botToken = std::getenv("TOKEN");
    const char* dbHost = std::getenv("POSTGRES_HOST");
    const char* dbPort = std::getenv("POSTGRES_PORT");
    const char* dbName = std::getenv("POSTGRES_DB");
    const char* dbUser = std::getenv("POSTGRES_USER");
    const char* dbPassword = std::getenv("POSTGRES_PASSWORD");

    if (!botToken || !dbHost || !dbPort || !dbName || !dbUser || !dbPassword) {
        std::cerr << "Отсутствуют переменные окружения. Пожалуйста, проверьте ваш .env файл.\n";
        return 1;
    }

    std::string connectionStr = "host=" + std::string(dbHost) +
                                " port=" + std::string(dbPort) +
                                " dbname=" + std::string(dbName) +
                                " user=" + std::string(dbUser) +
                                " password=" + std::string(dbPassword);

    NeverForgetBot::Database db(connectionStr);

    TgBot::Bot bot(botToken);

    chrono_task::start_periodic_task(db, bot);

    bot.getEvents().onCommand("start", [&bot, &db](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onStartCommand(message, bot);
        NeverForgetBot::Utils::saveUserIfNotExists(message, bot, db);
    });

    bot.getEvents().onCommand("change_timezone",[&bot, &db](TgBot::Message::Ptr message){
        NeverForgetBot::Commands::onChangeTzCommand(message, bot);
    });

    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onHelpCommand(message, bot);
    });

    bot.getEvents().onCommand("upcoming_events", [&bot, &db](TgBot::Message::Ptr message) {
        NeverForgetBot::Commands::onUpcomingEventsCommand(message, bot,db);
    });

    bot.getEvents().onCommand("secret_internals_do_not_invoke_or_you_will_be_fired", [&bot, &db](TgBot::Message::Ptr message) {
        std::vector<std::string> chatIds = db.fetchAllIds();

        for (const auto& id : chatIds) {
            bot.getApi().sendMessage(
                id,
                "*Спасибо ШБР!!!\n\n🐤 Young && Yandex 🐤*",
                nullptr,
                nullptr,
                nullptr,
                "Markdown"
            );
            bot.getApi().sendSticker(
                id,
                "CAACAgIAAxkBAAIEhGdZVfPK--rajyQtqxzgVe7Dc0-wAAI4XgACCPBASvIkNTG01rOqNgQ"
            );
        }
    });

    bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Неверная команда");
    });

    bot.getEvents().onCallbackQuery([&bot,&db](TgBot::CallbackQuery::Ptr query) {
        NeverForgetBot::CallbackHandlers::onCallbackQuery(query, bot,db);
    });

    bot.getEvents().onNonCommandMessage([&bot, &db](TgBot::Message::Ptr message) {
        std::string message_text;
        try {
            TgBot::Message::Ptr currMessage = bot.getApi().sendMessage(message->chat->id, "Обрабатываю ваш запрос...");
            Checker checker = processMessage(message->text);

            NeverForgetBot::Utils::saveEvent(message, bot, db, checker, currMessage);

        } catch (const std::exception& e) {
            message_text = "Ошибка обработки вашего сообщения: " + std::string(e.what());
            bot.getApi().sendMessage(message->chat->id, message_text);
        }
    });

    NeverForgetBot::Utils::startLongPolling(bot);

    chrono_task::stop_periodic_task();
    return 0;
}
