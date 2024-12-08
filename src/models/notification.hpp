#pragma once

#include <string>
#include <optional>

namespace NeverForgetBot {

using namespace std;

struct Notification {
    string id;
    string eventId;
    string time;
    optional<string> sentTime;
    string createdAt;
    string updatedAt;
};

}
