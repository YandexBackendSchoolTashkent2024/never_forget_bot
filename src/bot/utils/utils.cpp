#include "utils.hpp"

namespace NeverForgetBot::Utils {

void startLongPolling(TgBot::Bot& bot) {
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();
        bot.getApi().setMyCommands(getBotCommands());
        bot.getApi().setMyDescription(getBotDescription());
        bot.getApi().setMyShortDescription("An awesome bot to make you forget about forgetting");

        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
}

std::vector<TgBot::BotCommand::Ptr> getBotCommands() {
    std::unordered_map<std::string, std::string> mp = {
        { "/upcoming_events", "получить список предстоящих событий" },
        { "/settings", "отобразить меню настроек" },
        { "/help", "инструкция по использованию бота" },
        { "/start", "запуск бота" }
    };

    std::vector<TgBot::BotCommand::Ptr> commands;
    for (auto [cmd, description] : mp) {
        auto command = std::make_shared<TgBot::BotCommand>();
        command->command = cmd;
        command->description = description;
        commands.push_back(command);
    }

    return commands;
}

std::string getBotDescription() {
    return "Добро пожаловать в Never Forget Bot!\n\n"
        "Создавайте напоминания и получайте своевременные уведомления без лишней траты времени.\n\n"
        "Начнем! 🎯";
}

} // namespace NeverForgetBot::Utils
