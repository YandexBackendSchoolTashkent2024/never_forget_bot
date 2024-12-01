#include <iostream>
#include <string>
#include <curl/curl.h>
#include "picojson.h"  // Include picojson header
#include <fstream>
#include "generate_aim.h"  // Include the header file for the generateAIMToken function
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iomanip>

// Callback function to handle the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = (std::string*)userp;
    response->append((char*)contents, totalSize);
    return totalSize;
}

// Function to remove markdown backticks (```)
std::string cleanJsonString(const std::string& rawJsonStr) {
    std::string cleanJsonStr = rawJsonStr;

    // Remove leading backticks (if any)
    if (cleanJsonStr.substr(0, 3) == "```") {
        cleanJsonStr = cleanJsonStr.substr(3);
    }

    // Remove trailing backticks (if any)
    if (cleanJsonStr.size() >= 3 && cleanJsonStr.substr(cleanJsonStr.size() - 3) == "```") {
        cleanJsonStr = cleanJsonStr.substr(0, cleanJsonStr.size() - 3);
    }

    // Trim any leading or trailing whitespace
    cleanJsonStr.erase(0, cleanJsonStr.find_first_not_of(" \t\n\r"));
    cleanJsonStr.erase(cleanJsonStr.find_last_not_of(" \t\n\r") + 1);

    return cleanJsonStr;
}

std::pair<std::unordered_map<std::string, std::string>, std::vector<std::string>> sendPostRequest(const std::string& msg) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string full_msg;

    // Get current time from system clock
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    full_msg = msg + " Current time is: " + std::ctime(&time_t_now);
    // Get the IAM token from the generateAIMToken function
    std::string iam_token = generateAIMToken();
    if (iam_token.empty()) {
        std::cerr << "Error: Failed to retrieve AIM token!" << std::endl;
        return {};
    }

    // Load the JSON payload from a file
    std::ifstream file("parse_msg/prompt.json");
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open prompt.json file!" << std::endl;
        return {};
    }
    std::string json_payload((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Parse the JSON payload into a picojson object
    picojson::value json_obj;
    std::string err = picojson::parse(json_obj, json_payload);
    if (!err.empty()) {
        std::cerr << "Error parsing JSON payload: " << err << std::endl;
        return {};
    }

    // Modify the messages[1].text field
    picojson::array& messages = json_obj.get("messages").get<picojson::array>();
    if (messages.size() > 1) {
        // Correctly update the "text" field using set method
    // Correctly update the "text" field using picojson's constructor
    messages[1].get("text") = picojson::value(full_msg);  // Assign new value to "text"
    } else {
        std::cerr << "Error: messages[1] not found in the JSON payload!" << std::endl;
        return {};
    }



    // Convert the modified JSON back to a string using picojson::value::serialize()
    std::string modified_json_payload = json_obj.serialize();

    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the POST request
        curl_easy_setopt(curl, CURLOPT_URL, "https://llm.api.cloud.yandex.net/foundationModels/v1/completion");

        // Set the headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + iam_token).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data (modified JSON payload)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, modified_json_payload.c_str());

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the POST request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return {};
        } else {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            // Parse the JSON response using picojson
            picojson::value response_json;
            err = picojson::parse(response_json, readBuffer);
            if (!err.empty()) {
                std::cerr << "Error parsing JSON response: " << err << std::endl;
                return {};
            }

            // Extract the event data from the response
            std::unordered_map<std::string, std::string> eventData;

            // Access the 'alternatives' array and parse the 'message.text' field
            if (response_json.get("result").contains("alternatives")) {
                picojson::array alternatives = response_json.get("result").get("alternatives").get<picojson::array>();
                if (!alternatives.empty()) {
                    std::string eventJsonStr = alternatives[0].get("message").get("text").to_str();

                    // Clean the event JSON string by removing the markdown backticks
                    std::string cleanedJsonStr = cleanJsonString(eventJsonStr);

                    // Now parse the cleaned JSON string
                    picojson::value eventJson;
                    std::string eventErr = picojson::parse(eventJson, cleanedJsonStr);
                    if (!eventErr.empty()) {
                        std::cerr << "Error parsing event JSON: " << eventErr << std::endl;
                        return {};
                    }

                    // Extract the event details
                    if (eventJson.contains("name_event")) {
                        eventData["name_event"] = eventJson.get("name_event").to_str();
                    }
                    if (eventJson.contains("time")) {
                        eventData["time"] = eventJson.get("time").to_str();
                    }
                    if (eventJson.contains("type")) {
                        eventData["type"] = eventJson.get("type").to_str();
                    }
                }
            }

            // Extract notifications if present
            std::vector<std::string> notifications;
            if (response_json.get("result").contains("alternatives")) {
                picojson::array alternatives = response_json.get("result").get("alternatives").get<picojson::array>();
                if (!alternatives.empty()) {
                    std::string eventJsonStr = alternatives[0].get("message").get("text").to_str();

                    // Clean the event JSON string by removing the markdown backticks
                    std::string cleanedJsonStr = cleanJsonString(eventJsonStr);

                    // Parse the cleaned event JSON string to get notifications
                    picojson::value eventJson;
                    std::string eventErr = picojson::parse(eventJson, cleanedJsonStr);
                    if (!eventErr.empty()) {
                        std::cerr << "Error parsing event JSON: " << eventErr << std::endl;
                    } else if (eventJson.contains("notification")) {
                        picojson::array notifArray = eventJson.get("notification").get<picojson::array>();
                        for (const auto& notif : notifArray) {
                            notifications.push_back(notif.to_str());
                        }
                    }
                }
            }

            return {eventData, notifications};
        }
    } else {
        std::cerr << "Error: Failed to initialize CURL!" << std::endl;
        return {};
    }
}
