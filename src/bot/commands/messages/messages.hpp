#include <string>

namespace NeverForgetBot::Commands::Messages {
    static const std::string kStartMessage = "Привет! Добро пожаловать в Never Forget Bot! 👋\n\n"
    "Я здесь, чтобы убедиться, что вы никогда не пропустите важные события или напоминания. Просто отправьте мне сообщение с подробностями о том, что вам нужно запомнить, и я позабочусь обо всем остальном. 🎯\n\n"
    "Вот как начать:\n"
    "1️⃣ Введите свое событие или напоминание на естественном языке (например, «Напомни мне сходить в магазин завтра в 3 часа дня»)\n"
    "2️⃣ Я подтвержду детали и сохраню событие для вас\n"
    "3️⃣ Я уведомлю вас в нужный день и нужный час 😎\n\n"
    "Давайте оставим забывание в прошлом. 🚀";

    static const std::string kHelpMessage =  R"markdown(
🔥Never Forget Bot🔥

Доступные команды:
/start - инициализация бота
/help - вывод списка доступных комманд
/settings - настройки пользователя
/upcoming\_events - получение списка ближайших событий
/change\_timezone - изменение часового пояса

Вот что я умею делать:
📅 *Создание событий и напоминаний*: предоставьте подробную информацию о событии (например, «Запланируй встречу с Димой завтра в 10:00»), и я создам его для вас.
⏰ *Своевременные уведомления*: я позабочусь о том, чтобы вы были уведомлены в подходящее время, чтобы оставаться в курсе своих обязательств.

Примеры команд, которые вы можете использовать:
• Напомни отправить отчет по проекту в четверг в 15:00
• Позвонить доктору через 30 минут
• Подготовиться к презентации 20 декабря в 9 утра
    )markdown";

}
