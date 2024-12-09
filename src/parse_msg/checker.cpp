#include "checker.hpp"
#include <regex>
#include <iostream>
#include <stdexcept>

Checker::Checker(const std::unordered_map<std::string, std::string> &input,
                 const std::vector<std::string> &notifications)
    : notifications(notifications)
{
    std::cout << "Checker Constructor Called" << std::endl;

    auto nameEventIt = input.find("name_event");
    if (nameEventIt != input.end() && !nameEventIt->second.empty()) {
        nameEvent = nameEventIt->second;
    } else {
        throw std::invalid_argument("Missing or empty field: name_event");
    }

    auto timeIt = input.find("time");
    if (timeIt != input.end() && !timeIt->second.empty()) {
        time = timeIt->second;
        if (!validateTimestamp(time)) {
            throw std::invalid_argument("Invalid timestamp format: " + time);
        }
    } else {
        time = "";
    }

    auto typeIt = input.find("type");
    if (typeIt != input.end()) {
        type = stringToEventType(typeIt->second);
    } else {
        throw std::invalid_argument("Missing field: type");
    }

    std::cout << "Processing notifications..." << std::endl;
    for (const auto &notif : notifications) {
        std::cout << "Validating notification: " << notif << std::endl;
        if (!validateTimestamp(notif)) {
            throw std::invalid_argument("Invalid notification timestamp: " + notif);
        }
    }
}

bool Checker::isValid() const {
    return !nameEvent.empty();
}

std::string Checker::getNameEvent() const {
    return nameEvent;
}

std::string Checker::getTime() const {
    return time;
}

Checker::EventType Checker::getType() const {
    return type;
}

std::vector<std::string> Checker::getNotifications() const {
    return notifications;
}

Checker::EventType Checker::stringToEventType(const std::string &typeStr) const {
    if (typeStr == "while-not-done") {
        return EventType::WHILE_NOT_DONE;
    } else if (typeStr == "one-time") {
        return EventType::ONE_TIME;
    } else {
        throw std::invalid_argument("Invalid event type: " + typeStr);
    }
}

bool Checker::validateTimestamp(const std::string &timestamp) const {
    if (timestamp.empty()) {
        return true;
    }

    std::regex timestampRegex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$)");
    return std::regex_match(timestamp, timestampRegex);
}
