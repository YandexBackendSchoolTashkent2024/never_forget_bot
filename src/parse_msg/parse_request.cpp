#include <chrono>
#include <fstream>
#include <iterator>
#include <iostream>
#include <curl/curl.h>
#include "picojson.h" 
#include <unordered_map>
#include <vector>
#include <iomanip>
#include "generate_aim.h"

// Функция обратного вызова для записи ответа CURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = (std::string*)userp;
    response->append((char*)contents, totalSize);
    return totalSize;
}

// Очистка строки JSON от лишних символов
std::string cleanJsonString(const std::string& rawJsonStr) {
    std::string cleanJsonStr = rawJsonStr;

    if (cleanJsonStr.substr(0, 3) == "```") {
        cleanJsonStr = cleanJsonStr.substr(3);
    }

    if (cleanJsonStr.size() >= 3 && cleanJsonStr.substr(cleanJsonStr.size() - 3) == "```") {
        cleanJsonStr = cleanJsonStr.substr(0, cleanJsonStr.size() - 3);
    }

    cleanJsonStr.erase(0, cleanJsonStr.find_first_not_of(" \t\n\r"));
    cleanJsonStr.erase(cleanJsonStr.find_last_not_of(" \t\n\r") + 1);

    return cleanJsonStr;
}

// Отправка POST-запроса и обработка ответа
std::unordered_map<std::string, std::string> sendPostRequest(const std::string& msg) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string full_msg;

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    full_msg = msg + " Текущее время: " + std::ctime(&time_t_now);

    std::string iam_token = generateAIMToken();
    if (iam_token.empty()) {
        std::cerr << "Ошибка: не удалось получить AIM токен!" << std::endl;
        return {};
    }

    std::ifstream file("src/parse_msg/prompt.json");
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл prompt.json!" << std::endl;
        return {};
    }
    std::string json_payload((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    picojson::value json_obj;
    std::string err = picojson::parse(json_obj, json_payload);
    if (!err.empty()) {
        std::cerr << "Ошибка разбора JSON payload: " << err << std::endl;
        return {};
    }

    if (json_obj.is<picojson::object>()) {
        picojson::array& messages = json_obj.get("messages").get<picojson::array>();
        if (messages.size() > 1 && messages[1].is<picojson::object>()) {
            picojson::object& messageObj = messages[1].get<picojson::object>();
            if (messageObj.find("text") != messageObj.end()) {
                messageObj["text"] = picojson::value(full_msg);
            } else {
                std::cerr << "Ошибка: поле 'text' не найдено в messages[1]!" << std::endl;
                return {};
            }
        } else {
            std::cerr << "Ошибка: messages[1] не является объектом или отсутствует!" << std::endl;
            return {};
        }
    }

    std::string modified_json_payload = json_obj.serialize();

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://llm.api.cloud.yandex.net/foundationModels/v1/completion");

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + iam_token).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, modified_json_payload.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Ошибка выполнения curl_easy_perform(): " << curl_easy_strerror(res) << std::endl;
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return {};
        } else {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            picojson::value response_json;
            err = picojson::parse(response_json, readBuffer);
            if (!err.empty()) {
                std::cerr << "Ошибка разбора JSON ответа: " << err << std::endl;
                return {};
            }

            std::unordered_map<std::string, std::string> eventData;

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
                                    std::cerr << "Ошибка разбора JSON события: " << eventErr << std::endl;
                                    return {};
                                }

                                if (eventJson.contains("name_event")) {
                                    eventData["name_event"] = eventJson.get("name_event").to_str();
                                }
                                if (eventJson.contains("time")) {
                                    eventData["time"] = eventJson.get("time").to_str();
                                }
                                if (eventJson.contains("notification_time")) {
                                    eventData["notification_time"] = eventJson.get("notification_time").to_str();
                                }
                                if (eventJson.contains("type")) {
                                    eventData["type"] = eventJson.get("type").to_str();
                                }

                                if (eventJson.contains("time_type")) {
                                    eventData["time_type"] = eventJson.get("time_type").to_str();
                                }

                                if (eventJson.contains("notification_time_type")) {
                                    eventData["notification_time_type"] = eventJson.get("notification_time_type").to_str();
                                }

                            }
                        }
                    }
                }
            }

            return eventData;
        }
    } else {
        std::cerr << "Ошибка: не удалось инициализировать CURL!" << std::endl;
        return {};
    }
}