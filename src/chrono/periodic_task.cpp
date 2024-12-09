#include "periodic_task.hpp"

namespace chrono_task {

std::atomic<bool> keep_running(true);

void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot) {
    while (keep_running) {
        std::cout << "Background task executed at: " << std::time(nullptr) << " (Unix timestamp)" << std::endl;

        auto notifications = db.fetchPendingNotifications();

        for (const auto& n : notifications) {
            const auto& [notification_id, telegram_id, event_name, event_time, notification_time] = std::tie(
                n[0], n[1], n[2], n[3], n[4]
            );

            std::cout << "Sending notification to user: " << telegram_id
                << " for event: " << event_name
                << " scheduled at: " << event_time << std::endl;

            try {
                NeverForgetBot::Notifications::sendNotification(
                    std::stol(telegram_id),
                    bot,
                    notification_id,
                    notification_time,
                    event_name,
                    event_time,
                    db
                );

                try {
                    db.updateSentTimeForNotification(notification_id);
                    std::cout << "✅ Successfully updated sent_time for notification: " << notification_id << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "❌ Error updating sent_time for notification: " << notification_id
                        << ". Reason: " << e.what() << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "⚠️ Error sending notification to user " << telegram_id << ": " << e.what() << std::endl;
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
