#pragma once

#include <tgbot/tgbot.h>
#include "../../../db/db.hpp"
#include "../../utils/utils.hpp"


namespace NeverForgetBot::Notifications {

void sendNotification(
    long chatId,
    TgBot::Bot &bot,
    const std::string &notification_id,
    const std::string &notification_time,
    const std::string &event_name,
    const std::string &event_time,
    Database &db
);

void createNotificationKeyboard();

} // namespace NeverForgetBot::Notifications
