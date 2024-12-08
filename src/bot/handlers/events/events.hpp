#include <tgbot/tgbot.h>

#include "../../../models/event.hpp"

namespace NeverForgetBot::Events {

    void send_event(TgBot::Bot& bot, const Event& event);

    void send_events(TgBot::Bot &bot, long long chat_id, const std::vector<Event> &events);

}
