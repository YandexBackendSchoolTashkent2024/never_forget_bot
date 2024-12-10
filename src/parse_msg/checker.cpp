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
        if (nameEvent == "null") {
            throw std::invalid_argument("Пожалуйста, укажите название события");

        }
    } else {
        throw std::invalid_argument("Отсутствует или пустое поле: name_event");
    }

    auto timeIt = input.find("time");
    if (timeIt != input.end() && !timeIt->second.empty()) {
        time = timeIt->second;
        if (!validateTimestamp(time)) {
            if (time=="null") {
                throw std::invalid_argument("Пожалуйста, укажите время события");
            }
            std::cout<<time<<std::endl;
            throw std::invalid_argument("Недопустимый формат временной метки: " + time);
        }
    } else {
        time = "";
    }

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

    std::cout << "Processing notifications..." << std::endl;
    for (const auto &notif : notifications) {
        std::cout << "Validating notification: " << notif << std::endl;
        if (!validateTimestamp(notif)) {
            throw std::invalid_argument("Недопустимая временная метка уведомления: " + notif);
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
    if (typeStr == "WHILE_NOT_DONE") {
        return EventType::WHILE_NOT_DONE;
    } else if (typeStr == "ONE_TIME") {
        return EventType::ONE_TIME;
    } else {
        throw std::invalid_argument("Недопустимый тип события: " + typeStr);
    }
}

bool Checker::validateTimestamp(const std::string &timestamp) const {
    std::regex timestampRegex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$)");
    return std::regex_match(timestamp, timestampRegex);
}