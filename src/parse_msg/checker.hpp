#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class Checker
{
public:
    Checker(const std::unordered_map<std::string, std::string> &input);

    bool isValid() const;
    std::string getNameEvent() const;
    std::string getTime() const;
    std::string getTimeType() const;
    std::string getNotificationTimeType() const;
    std::string getType() const;
    std::string getNotificationTime() const;

private:
    std::string nameEvent;
    std::string time;
    std::string time_type;
    std::string notification_time_type;
    std::string type;
    std::string notification_time;
    bool validateTimestamp(const std::string &timestamp) const;
};