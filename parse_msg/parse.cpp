#include "../parse_msg/checker.hpp"
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>
#include "parse_request.h"  // Include the header for sendPostRequest

// Function that takes a message, sends a POST request, and returns a Checker object
Checker processMessage(const std::string& msg)
{
    try
    {
        // Call the function to send the POST request and get the response
        auto response = sendPostRequest(msg); // response is a pair of unordered_map and vector

        // Extract event data and notifications from the response
        const auto& eventData = response.first;
        const auto& notifications = response.second;

        // Create the Checker instance and return it
        Checker checker(eventData, notifications);
        return checker;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        throw;  // Rethrow the exception to handle it in the caller if needed
    }
}
