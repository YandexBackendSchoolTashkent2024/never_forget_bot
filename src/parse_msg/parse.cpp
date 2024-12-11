#include "../parse_msg/checker.hpp"
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>
#include "parse_request.h" 

Checker processMessage(const std::string& msg)
{
    try
    {
        auto response = sendPostRequest(msg);

        const auto& eventData = response;

        Checker checker(eventData);
        return checker;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        throw; 
    }
}