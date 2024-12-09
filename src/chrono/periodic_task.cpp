#include "periodic_task.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "../db/db.hpp"
#include "../bot/handlers/notifications/notifications.hpp"
#include "../bot/utils/utils.hpp"

namespace chrono_task {

std::atomic<bool> keep_running(true);


void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot) {
    while (keep_running) {
        std::time_t now = std::time(nullptr);
        std::cout << "Background task executed at: " << now << " (Unix timestamp)" << std::endl;

        auto notifications = db.fetchPendingNotifications();
        
        for (const auto& notification_tuple : notifications) {
            const auto& [notification_id, telegram_id_str, event_name, event_time_str] = notification_tuple;
            std::cout << "Preparing to send notification to user: " << telegram_id_str
                      << " for event: " << event_name
                      << " scheduled at: " << event_time_str << std::endl;
            
            try {
                long telegram_id = std::stol(telegram_id_str);

                std::string timeWithTimeZone;
                try {
                    timeWithTimeZone = NeverForgetBot::Utils::formatTimeWithTimezone(telegram_id, event_time_str, db);
                } catch (const std::exception& e) {
                    std::cerr << "Error formatting time with timezone: " << e.what() << std::endl;
                    timeWithTimeZone = event_time_str;
                }

                NeverForgetBot::Notification notification;

                NeverForgetBot::Event event;
                event.time = timeWithTimeZone;
                event.name = event_name;

                NeverForgetBot::Notifications::sendNotification(telegram_id, bot, notification, event);

                try {
                    db.updateSentTimeForNotification(notification_id);
                    std::cout << "✅ Successfully updated sent_time for notification: " << notification_id << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "❌ Error updating sent_time for notification: " << notification_id
                              << ". Reason: " << e.what() << std::endl;
                }

            } catch (const std::exception& e) {
                std::cerr << "⚠️ Error preparing notification for user " << telegram_id_str 
                          << ": " << e.what() << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void start_periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot) {
    std::thread background_thread(periodic_task, std::ref(db), std::ref(bot));
    background_thread.detach();
}

void stop_periodic_task() {
    keep_running = false;
}

} // namespace chrono_task
