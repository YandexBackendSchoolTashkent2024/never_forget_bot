#include "../../utils/utils.hpp"
#include <string>

namespace NeverForgetBot::Events {

    void send_events(TgBot::Bot &bot, long chat_id, const std::vector<Event> &events, Database &db);

} // namespace NeverForgetBot::Events
