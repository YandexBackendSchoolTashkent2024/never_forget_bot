#include "checker.hpp"
#include <regex>
#include <iostream>
#include <stdexcept>

Checker::Checker(const std::unordered_map<std::string, std::string> &input) {
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
                // std::cerr<<timeIt<<std::endl;
                throw std::invalid_argument("Пожалуйста, укажите время события");
            }
            std::cout<<time<<std::endl;
            throw std::invalid_argument("Недопустимый формат временной метки: " + time);
        }
    } else {
        time = "";
    }

    auto notificationTimeIt = input.find("time");
    if (notificationTimeIt != input.end() && !notificationTimeIt->second.empty()) {
        notification_time = notificationTimeIt->second;
        if (!validateTimestamp(notification_time)) {
            if (notification_time=="null") {
                throw std::invalid_argument("Пожалуйста, укажите время уведомления");
            }
            std::cout<<notification_time<<std::endl;
            throw std::invalid_argument("Недопустимый формат временной метки: " + notification_time);
        }
    } else {
        notification_time = "";
    }

    auto timeTypeIt = input.find("time_type");
    if (timeTypeIt != input.end() && !timeTypeIt->second.empty()) {
        time_type = timeTypeIt->second;
        if (time_type != "RELATIVE") {
            time_type = "ABSOLUTE";
        }
    } else {
        time_type = "ABSOLUTE";
    }    

    auto notificationTimeTypeIt = input.find("notification_time_type");
    if (notificationTimeTypeIt != input.end() && !notificationTimeTypeIt->second.empty()) {
        notification_time_type = notificationTimeTypeIt->second;
        if (notification_time_type != "RELATIVE") {
            notification_time_type = "ABSOLUTE";
        }
    } else {
        notification_time_type = "ABSOLUTE";
    }    

    auto typeIt = input.find("type");
    if (typeIt != input.end() && !typeIt->second.empty()) {
        type = typeIt->second;
        if (type != "WHILE_NOT_DONE") {
            type = "ONE_TIME";
        }
    } else {
        type = "ONE_TIME";
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

std::string Checker::getTimeType() const {
    return time_type;
}

std::string Checker::getType() const {
    return type;
}

std::string Checker::getNotificationTime() const {
    return notification_time;
}

std::string Checker::getNotificationTimeType() const {
    return notification_time_type;
}

bool Checker::validateTimestamp(const std::string &timestamp) const {
    std::regex timestampRegex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$)");
    return std::regex_match(timestamp, timestampRegex);
}