// periodic_task.hpp
#ifndef PERIODIC_TASK_HPP
#define PERIODIC_TASK_HPP

#include <atomic>

#include "../db/db.hpp"

#include <tgbot/tgbot.h>

namespace chrono_task {

extern std::atomic<bool> keep_running;

void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot);
void start_periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot);
void stop_periodic_task();

} // namespace chrono_task

#endif // PERIODIC_TASK_HPP
