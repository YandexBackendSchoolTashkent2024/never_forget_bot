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
    if (nameEventIt != input.end() && !nameEventIt->second.empty()) {
        nameEvent = nameEventIt->second;
        if (nameEvent == "null") {
            throw std::invalid_argument("Please provide event name");

        }
    } else {
        throw std::invalid_argument("Missing or empty field: name_event");
    }

    // Parse 'time' if it exists. If it doesn't, set it to empty and do not throw.
    auto timeIt = input.find("time");
    if (timeIt != input.end() && !timeIt->second.empty()) {
        time = timeIt->second;
        if (!validateTimestamp(time)) {
            if (time=="null") {
                throw std::invalid_argument("Please provide event time");
            }
            throw std::invalid_argument("Invalid timestamp format: " + time);
        }
    } else {
        // Time is optional: if not provided or empty, we just leave it as an empty string
        // to indicate "no time set".
        time = "";
    }

    // Parse and validate 'type'
    auto typeIt = input.find("type");
    if (typeIt != input.end() && !typeIt->second.empty()) {
        try {
            type = stringToEventType(typeIt->second);
        } catch (const std::invalid_argument &e) {
            type = EventType::ONE_TIME; // Default to ONE_TIME if invalid
        }
    } else {
        type = EventType::ONE_TIME; // Default to ONE_TIME if missing
    }


    // Validate notifications if provided
    std::cout << "Processing notifications..." << std::endl;
    for (const auto &notif : notifications) {
        std::cout << "Validating notification: " << notif << std::endl;
        if (!validateTimestamp(notif)) {
            throw std::invalid_argument("Invalid notification timestamp: " + notif);
        }
    }
}

// Validate the entire structure
bool Checker::isValid() const {
    return !nameEvent.empty();
    // Note: We do not require time to be set for the structure to be considered valid
}

// Getters
std::string Checker::getNameEvent() const {
    return nameEvent;
}

std::string Checker::getTime() const {
    // If time is empty, you can either return an empty string or "null" string.
    // Let's return empty string to indicate no time provided.
    return time;
}

Checker::EventType Checker::getType() const {
    return type;
}

// Updated getNotifications() method
std::vector<std::string> Checker::getNotifications() const {
    return notifications;
}

// Helper: Convert string to EventType
Checker::EventType Checker::stringToEventType(const std::string &typeStr) const {
    if (typeStr == "WHILE_NOT_DONE") {
        return EventType::WHILE_NOT_DONE;
    } else if (typeStr == "ONE_TIME") {
        return EventType::ONE_TIME;
    } else {
        throw std::invalid_argument("Invalid event type: " + typeStr);
    }
}


// Helper: Validate timestamp format
bool Checker::validateTimestamp(const std::string &timestamp) const {
    std::regex timestampRegex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$)");
    return std::regex_match(timestamp, timestampRegex);
}