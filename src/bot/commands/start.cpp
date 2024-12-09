#include "commands.hpp"

namespace NeverForgetBot::Commands {

    void onStartCommand(TgBot::Message::Ptr message, TgBot::Bot &bot) {
        bot.getApi().sendMessage(message->chat->id, NeverForgetBot::Commands::Messages::kStartMessage);
    }
}