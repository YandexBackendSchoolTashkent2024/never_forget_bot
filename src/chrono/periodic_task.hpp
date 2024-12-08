// periodic_task.hpp
#ifndef PERIODIC_TASK_HPP
#define PERIODIC_TASK_HPP

#include <atomic>

#include "../db/db.hpp"  // Include the header for the Database class

#include <tgbot/tgbot.h>

namespace chrono_task {

extern std::atomic<bool> keep_running; // Flag to control the background thread

// Function declarations
void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot);  // Accept the Database object as a parameter
void start_periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot);  // Pass Database to the start function
void stop_periodic_task();

} // namespace chrono_task

#endif // PERIODIC_TASK_HPP
