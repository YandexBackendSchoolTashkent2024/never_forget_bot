#include "commands.hpp"

namespace NeverForgetBot::Commands {
    void onUpcommingEventsCommand(TgBot::Message::Ptr message, TgBot::Bot& bot , Database &db) {
        long telegram_id = message->from->id;

        std::vector<Event> uppcommingEvents = db.getEventsOrderedByTimeDesc(telegram_id);

        Events::send_events(bot, telegram_id, uppcommingEvents, db);
    }
}
