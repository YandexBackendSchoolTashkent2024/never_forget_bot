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
    std::ifstream file("src/parse_msg/prompt.json");
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
    if (json_obj.is<picojson::object>()) {
        picojson::array& messages = json_obj.get("messages").get<picojson::array>();
        if (messages.size() > 1 && messages[1].is<picojson::object>()) {
            picojson::object& messageObj = messages[1].get<picojson::object>();
            if (messageObj.find("text") != messageObj.end()) {
                messageObj["text"] = picojson::value(full_msg);
            } else {
                std::cerr << "Error: 'text' field not found in messages[1]!" << std::endl;
                return {};
            }
        } else {
            std::cerr << "Error: messages[1] is not an object or doesn't exist!" << std::endl;
            return {};
        }
    }

    // Convert the modified JSON back to a string
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

            // Extract the event data and notifications
            std::unordered_map<std::string, std::string> eventData;
            std::vector<std::string> notifications;

            if (response_json.is<picojson::object>()) {
                auto& resultObj = response_json.get("result").get<picojson::object>();
                if (resultObj.find("alternatives") != resultObj.end() &&
                    resultObj["alternatives"].is<picojson::array>()) {
                    picojson::array& alternatives = resultObj["alternatives"].get<picojson::array>();
                    if (!alternatives.empty() && alternatives[0].is<picojson::object>()) {
                        auto& altObj = alternatives[0].get<picojson::object>();
                        if (altObj.find("message") != altObj.end() &&
                            altObj["message"].is<picojson::object>()) {
                            auto& msgObj = altObj["message"].get<picojson::object>();
                            if (msgObj.find("text") != msgObj.end() &&
                                msgObj["text"].is<std::string>()) {
                                std::string cleanedJsonStr = cleanJsonString(msgObj["text"].get<std::string>());
                                picojson::value eventJson;
                                std::string eventErr = picojson::parse(eventJson, cleanedJsonStr);
                                if (!eventErr.empty()) {
                                    std::cerr << "Error parsing event JSON: " << eventErr << std::endl;
                                    return {};
                                }

                                if (eventJson.contains("name_event")) {
                                    eventData["name_event"] = eventJson.get("name_event").to_str();
                                }
                                if (eventJson.contains("time")) {
                                    eventData["time"] = eventJson.get("time").to_str();
                                }
                                if (eventJson.contains("type")) {
                                    eventData["type"] = eventJson.get("type").to_str();
                                }

                                if (eventJson.contains("notification") &&
                                    eventJson.get("notification").is<picojson::array>()) {
                                    for (const auto& notif : eventJson.get("notification").get<picojson::array>()) {
                                        notifications.push_back(notif.to_str());
                                    }
                                }
                            }
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
