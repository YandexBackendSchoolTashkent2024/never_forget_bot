#include <chrono>
#include <fstream>
#include <iterator>
#include <iostream>
#include <curl/curl.h>
#include <jwt-cpp/jwt.h>
#include "picojson.h"
#include "common.h"

// Функция для извлечения токена IAM из JSON-ответа
std::string extractIamToken(const std::string& response) {
    picojson::value v;
    std::string err = picojson::parse(v, response);
    if (!err.empty()) {
        std::cerr << "Не удалось разобрать JSON: " << err << std::endl;
        return "";
    }
    auto iamToken = v.get("iamToken").to_str();
    return iamToken;
}

// Функция для генерации AIM токена
std::string generateAIMToken() {
    const char* privateKey = std::getenv("PRIVATE_KEY");
    const char* serviceAccountId = std::getenv("SERVICE_ACCOUNT_ID");
    const char* keyId = std::getenv("ID");

    // Проверка наличия необходимых переменных окружения
    if (!privateKey || !serviceAccountId || !keyId) {
        std::cerr << "Отсутствует одна из обязательных переменных окружения: PRIVATE_KEY, SERVICE_ACCOUNT_ID или ID.\n";
        return "";
    }

    auto now = std::chrono::system_clock::now();
    auto expires_at = now + std::chrono::hours(1);
    picojson::array audience_array;
    audience_array.push_back(picojson::value("https://iam.api.cloud.yandex.net/iam/v1/tokens"));

    auto algorithm = jwt::algorithm::ps256("", privateKey);

    // Создание JWT токена
    auto encoded_token = jwt::create()
        .set_key_id(keyId)
        .set_issuer(serviceAccountId)
        .set_audience(audience_array)
        .set_issued_at(now)
        .set_expires_at(expires_at)
        .sign(algorithm);

    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    std::string readBuffer;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://iam.api.cloud.yandex.net/iam/v1/tokens");
        
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        
        std::string json_data = "{\"jwt\": \"" + encoded_token + "\"}";
        
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() не удалось выполнить: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Инициализация CURL не удалась!" << std::endl;
    }

    curl_global_cleanup();

    return extractIamToken(readBuffer);
}
