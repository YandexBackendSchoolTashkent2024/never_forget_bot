#include <tgbot/Bot.h>

namespace NeverForgetBot::Notifications {

void sendNotification(
    long chatId,
    TgBot::Bot &bot,
    std::string &notification_id,
    std::string &notification_time,
    std::string &event_name,
    std::string &event_time
);

void createNotificationKeyboard();

} // namespace NeverForgetBot::Notifications
