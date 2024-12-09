#include "commands.hpp"

namespace NeverForgetBot::Commands {

    void onHelpCommand(TgBot::Message::Ptr message, TgBot::Bot &bot) {
        bot.getApi().sendMessage(message->chat->id, NeverForgetBot::Commands::Messages::kHelpMessage, nullptr, nullptr, nullptr, "Markdown");
    }
}