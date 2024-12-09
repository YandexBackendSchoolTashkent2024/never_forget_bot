#pragma once

#include <tgbot/tgbot.h>
#include <atomic>
#include <ctime>
#include <thread>
#include <chrono>

#include "../db/db.hpp"


namespace chrono_task {

extern std::atomic<bool> keep_running;

void periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot);

void start_periodic_task(NeverForgetBot::Database& db, TgBot::Bot& bot);

void stop_periodic_task();

} // namespace chrono_task
