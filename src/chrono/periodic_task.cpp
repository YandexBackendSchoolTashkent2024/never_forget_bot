#include "periodic_task.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace chrono_task {

    // Flag to control the background thread
    std::atomic<bool> keep_running(true);

    // Periodic task function that accepts Database and Bot references
    void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot) {
        while (keep_running) {
            // Example logic using the Database object
            std::cout << "Background task executed at: " << std::time(nullptr) << " (Unix timestamp)" << std::endl;

            
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
