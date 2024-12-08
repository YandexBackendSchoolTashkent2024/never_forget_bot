#include <tgbot/Bot.h>

#include "../../../models/notification.hpp"
#include "../../../models/event.hpp"

namespace NeverForgetBot::Notifications {

    void sendNotification(long chatId, TgBot::Bot &bot, const Notification& notification, const Event& event);

    void createNotificationKeyboard();

}
