#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class Checker
{
public:
    enum EventType
    {
        WHILE_NOT_DONE,
        ONE_TIME
    };

    // Updated constructor to accept unordered_map and vector
    Checker(const std::unordered_map<std::string, std::string> &input,
            const std::vector<std::string> &notifications);

    bool isValid() const;
    std::string getNameEvent() const;
    std::string getTime() const;
    EventType getType() const;
    std::vector<std::string> getNotifications() const;

private:
    std::string nameEvent;
    std::string time;
    EventType type;
    std::vector<std::string> notifications;

    EventType stringToEventType(const std::string &typeStr) const;
    bool validateTimestamp(const std::string &timestamp) const;
};
