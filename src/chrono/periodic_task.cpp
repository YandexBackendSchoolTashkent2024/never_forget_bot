#include "periodic_task.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "../db/db.hpp"


namespace chrono_task {

    // Flag to control the background thread
    std::atomic<bool> keep_running(true);

    // Periodic task function that accepts Database and Bot references
    void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot) {
    while (keep_running) {
        std::cout << "Background task executed at: " << std::time(nullptr) << " (Unix timestamp)" << std::endl;

        // Step 1: Fetch pending notifications that need to be sent
        auto notifications = db.fetchPendingNotifications();
        
        // Step 2: Loop through each notification and send it
        for (const auto& notification : notifications) {
            const auto& [notification_id, telegram_id, event_name, event_time] = notification;
            std::cout << "Sending notification to user: " << telegram_id
                      << " for event: " << event_name
                      << " scheduled at: " << event_time << std::endl;
            
            // Send notification logic here, for example using `bot.sendMessage`
            // Example:
            // bot.getApi().sendMessage(telegram_id, "Reminder: " + event_name);
            try {
    // Create the formatted message
    std::string formattedMessage = std::string("🔔 *Reminder Alert!* 🔔\n\n") + 
                                   "*Event:* " + event_name + "\n" + 
                                   "*Scheduled At:* " + event_time + "\n\n" +
                                   "Stay tuned and be ready for the event! 😊\n\n" +
                                   "This is your friendly reminder from your bot. 💬";

    // Send the message
    auto sentMessage = bot.getApi().sendMessage(telegram_id, formattedMessage);
bool sentSuccessfully = (sentMessage != nullptr);

    if (sentSuccessfully) {
        std::cout << "✅ Successfully sent reminder to user " << telegram_id << std::endl;
        
        // Step 3: After successfully sending, update the sent_time in the database
        try {
            db.updateSentTimeForNotification(notification_id);
            std::cout << "✅ Successfully updated sent_time for notification: " << notification_id << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "❌ Error updating sent_time for notification: " << notification_id
                    << ". Reason: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "❌ Failed to send reminder to user " << telegram_id << std::endl;
    }

} catch (const std::exception& e) {
    std::cerr << "⚠️ Error sending notification to user " << telegram_id << ": " << e.what() << std::endl;
}


        }

        // Wait for the next interval (1 minute)
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}


    // Start the periodic task in a separate thread, passing Database and Bot as references
    void start_periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot) {
        std::thread background_thread(periodic_task, std::ref(db), std::ref(bot));  // Pass the db and bot objects as references using std::ref
        background_thread.detach();  // Detach the thread so it runs independently
    }

    // Stop the periodic task by setting the keep_running flag to false
    void stop_periodic_task() {
        keep_running = false;
    }

} // namespace chrono_task
