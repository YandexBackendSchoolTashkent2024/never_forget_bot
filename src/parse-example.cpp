#include "../parse_msg/checker.hpp"
#include "../parse_msg/parse.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <optional>


int main()
{
    try
    {
        // Define the message you want to pass to the JSON payload
        std::string msg = "Эй, мне надо не забыть про вебинар \"Технологии будущего\"! Он начинается завтра 18:00, так что напомни мне хотя бы пол часа до старта вебинар, хорошо?";

        // Call the processMessage function to get the Checker object
        Checker checker = processMessage(msg);

        // Validate and display event details
        if (checker.isValid())
        {
            std::cout << "Event Name: " << checker.getNameEvent() << "\n";
            std::cout << "Event Time: " << checker.getTime() << "\n";
            std::cout << "Event Type: "
                      << (checker.getType() == Checker::EventType::ONE_TIME ? "one-time" : "while-not-done") << "\n";

            const auto &notifications = checker.getNotifications();
            if (!notifications.empty())
            {
                std::cout << "Notifications: ";
                for (const auto &n : notifications)
                {
                    std::cout << n << " ";
                }
                std::cout << "\n";
            }
            else
            {
                std::cout << "Notifications: null\n";
            }
        }
        else
        {
            std::cerr << "Input is not valid\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}