#include "command_handlers.hpp"
#include <string>
#include <unordered_map>

namespace NeverForgetBot::Commands {

static const std::string kStartMessage = "Привет! Добро пожаловать в Never Forget Bot! 👋\n\n"
"Я здесь, чтобы убедиться, что вы никогда не пропустите важные события или напоминания. Просто отправьте мне сообщение с подробностями о том, что вам нужно запомнить, и я позабочусь обо всем остальном. 🎯\n\n"
"Вот как начать:\n"
"1️⃣ Введите свое событие или напоминание на естественном языке (например, «Напомни мне сходить в магазин завтра в 3 часа дня»)\n"
"2️⃣ Я подтвержду детали и сохраню событие для вас\n"
"3️⃣ Я уведомлю вас в нужный день и нужный час 😎\n\n"
"Давайте оставим забывание в прошлом. 🚀";

static const std::string kHelpMessage = R"markdown(
🔥Never Forget Bot🔥

Доступные команды:
/start - инициализация бота
/help - вывод списка доступных комманд
/settings - настройки пользователя
/upcoming\_events - получение списка ближайших событий
/cancel\_event - отмена события

Вот что я умею делать:
📅 *Создание событий и напоминаний*: предоставьте подробную информацию о событии (например, «Запланируй встречу с Димой завтра в 10:00»), и я создам его для вас.
⏰ *Своевременные уведомления*: я позабочусь о том, чтобы вы были уведомлены в подходящее время, чтобы оставаться в курсе своих обязательств.

Примеры команд, которые вы можете использовать:
• Напомни отправить отчет по проекту в четверг в 15:00
• Позвонить доктору через 30 минут
• Подготовиться к презентации 20 декабря в 9 утра
)markdown";

void onHelpCommand(TgBot::Message::Ptr message, TgBot::Bot &bot) {
    bot.getApi().sendMessage(message->chat->id, kHelpMessage, nullptr, nullptr, nullptr, "Markdown");
}

void onStartCommand(TgBot::Message::Ptr message, TgBot::Bot &bot) {
    bot.getApi().sendMessage(message->chat->id, kStartMessage);
}

void onChangeTzCommand(TgBot::Message::Ptr message, TgBot::Bot& bot) {
        // Create an inline keyboard with options for time zones or cities
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

        // Uzbekistan cities (with time zone information)
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
            button->callbackData = tz; // Use time zone offset as callback data
            uzbekistanRow.push_back(button);
        }

        // Russia cities (with time zone information)
        std::vector<TgBot::InlineKeyboardButton::Ptr> russiaRow;
        std::unordered_map<std::string, std::string> RusTimeZone = {
            {"Масква", "user_timez-Moscow"},
            {"Санкт-Петербург", "user_timez-St_Petersburg"},
            {"Новосибирск", "user_timez-Novosibirsk"}
        };
        for (const auto& [city, tz] : RusTimeZone) {
            std::string cityWithTz = city;
            auto button = std::make_shared<TgBot::InlineKeyboardButton>();
            button->text = cityWithTz;
            button->callbackData = tz; // Use time zone offset as callback data
            russiaRow.push_back(button);
        }

        // Add rows to the keyboard
        keyboard->inlineKeyboard.push_back(uzbekistanRow);
        keyboard->inlineKeyboard.push_back(russiaRow);

        // Send the message with the inline keyboard
        bot.getApi().sendMessage(
            message->chat->id,
            "Выберите город:",
            nullptr,  // LinkPreviewOptions
            nullptr,  // ReplyParameters
            keyboard,  // Include the keyboard here
            "Markdown"
        );
}

void handleTimeZoneSelection(const std::string& timeZone, TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db) {
    std::unordered_map<std::string,std::pair<std::string,int>> timeZoneInfo = {
        {"Tashkent", {"Ташкент",5}},
        {"Samarkand", {"Самарканд",5}},
        {"Bukhara", {"Бухара",5}},
        {"Moscow", {"Масква",3}},
        {"St_Petersburg", {"Санкт-Петербург",3}},
        {"Novosibirsk", {"Новосибирск",7}}
    };

    if (timeZoneInfo.find(timeZone) != timeZoneInfo.end()) {
        std::string city = timeZoneInfo[timeZone].first;
        int offset = timeZoneInfo[timeZone].second;
        db.updateUserTimeZone(query->message->chat->id,offset);
        std::string response = "Вы выбрали " + city + " (Таймзона : " + std::to_string(offset) + ").";
        bot.getApi().sendMessage(query->message->chat->id, response);
    } else {
        bot.getApi().sendMessage(query->message->chat->id, "Неопределенная таймзона");
    }
}

void onCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot& bot, NeverForgetBot::Database& db) {
   std::string callbackData = query->data;

    if (callbackData.find("user_timez-") == 0) {
        std::string timeZone = callbackData.substr(11);
        handleTimeZoneSelection(timeZone, query, bot, db);
    } else {
        bot.getApi().sendMessage(query->message->chat->id, "Unknown selection.");
    }
}


} // namespace NeverForgetBot::Commands

