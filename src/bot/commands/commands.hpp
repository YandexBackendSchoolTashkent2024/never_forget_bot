#pragma once
#include "../../db/db.hpp"
#include "./messages/messages.hpp"
#include "../handlers/events/events.hpp"

namespace NeverForgetBot::Commands {

    void onHelpCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);

    void onStartCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);

    void onChangeTzCommand(TgBot::Message::Ptr message, TgBot::Bot &bot);

    void onUpcomingEventsCommand(TgBot::Message::Ptr message, TgBot::Bot& bot , Database &db);

}
