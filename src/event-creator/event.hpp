#pragma once

#include <string>
#include <chrono>
#include <optional>

struct Event {
    std::string user_id;  // Replace with appropriate ID type
    std::string name;
    std::chrono::system_clock::time_point time;
    int type;
    int status;
};

struct Notification {
    int event_id;
    std::chrono::system_clock::time_point time;
    std::optional<std::chrono::system_clock::time_point> sent_time;
};
