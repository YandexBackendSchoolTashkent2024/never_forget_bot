#pragma once

#include <string>
#include <optional>

namespace NeverForgetBot {

struct Event {
    std::string id;
    std::string userId;
    std::string name;
    std::string time;
    std::string type;    // Assuming event_type is represented as a string
    std::string status;  // Assuming event_status is represented as a string
    std::string createdAt;
    std::string updatedAt;
};

} // namespace NeverForgetBot
