#include "checker.hpp"
#include <regex>
#include <iostream>
#include <stdexcept>

// Constructor
Checker::Checker(const std::unordered_map<std::string, std::string> &input,
                 const std::vector<std::string> &notifications)
    : notifications(notifications) // Directly initialize notifications
{
    std::cout << "Checker Constructor Called" << std::endl;

    // Parse and validate 'name_event'
    auto nameEventIt = input.find("name_event");
    if (nameEventIt != input.end() && !nameEventIt->second.empty())
    {
        nameEvent = nameEventIt->second;
    }
    else
    {
        throw std::invalid_argument("Missing or empty field: name_event");
    }

    // Parse and validate 'time'
    auto timeIt = input.find("time");
    if (timeIt != input.end() && !timeIt->second.empty())
    {
        time = timeIt->second;
        if (!validateTimestamp(time))
        {
            throw std::invalid_argument("Invalid timestamp format: " + time);
        }
    }
    else
    {
        throw std::invalid_argument("Missing or empty field: time");
    }

    // Parse and validate 'type'
    auto typeIt = input.find("type");
    if (typeIt != input.end())
    {
        type = stringToEventType(typeIt->second);
    }
    else
    {
        throw std::invalid_argument("Missing field: type");
    }

    // Validate notifications if provided
    std::cout << "Processing notifications..." << std::endl;
    for (const auto &notif : notifications)
    {
        std::cout << "Validating notification: " << notif << std::endl;
        if (!validateTimestamp(notif))
        {
            throw std::invalid_argument("Invalid notification timestamp: " + notif);
        }
    }
}

// Validate the entire structure
bool Checker::isValid() const
{
    return !nameEvent.empty() && !time.empty();
}

// Getters
std::string Checker::getNameEvent() const
{
    return nameEvent;
}

std::string Checker::getTime() const
{
    return time;
}

Checker::EventType Checker::getType() const
{
    return type;
}

// Updated getNotifications() method
std::vector<std::string> Checker::getNotifications() const
{
    return notifications;
}

// Helper: Convert string to EventType
Checker::EventType Checker::stringToEventType(const std::string &typeStr) const
{
    if (typeStr == "while-not-done")
    {
        return EventType::WHILE_NOT_DONE;
    }
    else if (typeStr == "one-time")
    {
        return EventType::ONE_TIME;
    }
    else
    {
        throw std::invalid_argument("Invalid event type: " + typeStr);
    }
}

// Helper: Validate timestamp format
bool Checker::validateTimestamp(const std::string &timestamp) const
{
    std::regex timestampRegex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$)");
    return std::regex_match(timestamp, timestampRegex);
}
