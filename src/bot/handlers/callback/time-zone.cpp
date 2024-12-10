#include "callback.hpp"
namespace NeverForgetBot::CallbackHandlers {

    void onTimeZoneSelection(const std::string& timeZone, TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db) {
        std::unordered_map<std::string, std::pair<std::string, int>> timeZoneInfo = {
            {"Tashkent", {"Ташкент", 5}},
            {"Samarkand", {"Самарканд", 5}},
            {"Bukhara", {"Бухара", 5}},
            {"Moscow", {"Москва", 3}},
            {"St_Petersburg", {"Санкт-Петербург", 3}},
            {"Novosibirsk", {"Новосибирск", 7}}
        };

        if (timeZoneInfo.find(timeZone) != timeZoneInfo.end()) {
            std::string city = timeZoneInfo[timeZone].first;
            int offset = timeZoneInfo[timeZone].second;

            db.updateUserTimeZone(query->message->chat->id, offset);

            std::string response = "Вы выбрали " + city + " (Таймзона : " + std::to_string(offset) + ").";

            bot.getApi().sendMessage(query->message->chat->id, response);
        } else {
            bot.getApi().sendMessage(query->message->chat->id, "Неопределенная таймзона");
        }
    }

}

