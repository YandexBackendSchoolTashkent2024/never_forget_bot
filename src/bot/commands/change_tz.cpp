#include "commands.hpp"

namespace NeverForgetBot::Commands {

    void onChangeTzCommand(TgBot::Message::Ptr message, TgBot::Bot& bot) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

        std::vector<TgBot::InlineKeyboardButton::Ptr> uzbekistanRow;
        std::unordered_map<std::string, std::string> UzbTimeZone = {
            {"Ташкент", "user_timez-Tashkent"},
            {"Самарканд", "user_timez-Samarkand"},
            {"Бухара", "user_timez-Bukhara"}
        };
        for (const auto& [city, tz] : UzbTimeZone) {
            std::string cityWithTz = city;
            auto button = std::make_shared<TgBot::InlineKeyboardButton>();
            button->text = cityWithTz;
            button->callbackData = tz;
            uzbekistanRow.push_back(button);
        }

        std::vector<TgBot::InlineKeyboardButton::Ptr> russiaRow;
        std::unordered_map<std::string, std::string> RusTimeZone = {
            {"Москва", "user_timez-Moscow"},
            {"Санкт-Петербург", "user_timez-St_Petersburg"},
            {"Новосибирск", "user_timez-Novosibirsk"}
        };
        for (const auto& [city, tz] : RusTimeZone) {
            std::string cityWithTz = city;
            auto button = std::make_shared<TgBot::InlineKeyboardButton>();
            button->text = cityWithTz;
            button->callbackData = tz;
            russiaRow.push_back(button);
        }

        keyboard->inlineKeyboard.push_back(uzbekistanRow);
        keyboard->inlineKeyboard.push_back(russiaRow);

        bot.getApi().sendMessage(
            message->chat->id,
            "Выберите город:",
            nullptr,  // LinkPreviewOptions
            nullptr,  // ReplyParameters
            keyboard,  // Include the keyboard here
            "Markdown"
        );
    }
}