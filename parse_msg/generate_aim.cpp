#include <chrono>
#include <fstream>
#include <iterator>
#include <iostream>
#include <curl/curl.h>
#include "jwt-cpp/jwt.h"
#include "picojson.h"
#include "common.h"

// Helper function to parse JSON and extract iamToken
std::string extractIamToken(const std::string& response) {
    picojson::value v;
    std::string err = picojson::parse(v, response);
    if (!err.empty()) {
        std::cerr << "Failed to parse JSON: " << err << std::endl;
        return "";
    }
    auto iamToken = v.get("iamToken").to_str();
    return iamToken;
}

std::string generateAIMToken() {
    // Read private key and other data from JSON file
    std::ifstream key_file("parse_msg/bot-key.json");
    std::string content((std::istreambuf_iterator<char>(key_file)),
                        (std::istreambuf_iterator<char>()));

    picojson::value v;
    std::string err = picojson::parse(v, content);
    auto privateKey = v.get("private_key").to_str();
    auto serviceAccountId = v.get("service_account_id").to_str();
    auto keyId = v.get("id").to_str();

    // Generate JWT token
    auto now = std::chrono::system_clock::now();
    auto expires_at = now + std::chrono::hours(1);
    picojson::array audience_array;
    audience_array.push_back(picojson::value("https://iam.api.cloud.yandex.net/iam/v1/tokens"));
    auto algorithm = jwt::algorithm::ps256("", privateKey);

    auto encoded_token = jwt::create()
        .set_key_id(keyId)
        .set_issuer(serviceAccountId)
        .set_audience(audience_array)
        .set_issued_at(now)
        .set_expires_at(expires_at)
        .sign(algorithm);

    // std::cout << "JWT Token: " << encoded_token << std::endl;

    // Prepare curl request for AIM token
    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    std::string readBuffer;

    if (curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://iam.api.cloud.yandex.net/iam/v1/tokens");
        
        // Set POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        
        // Prepare JSON data
        std::string json_data = "{\"jwt\": \"" + encoded_token + "\"}";
        
        // Set POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());

        // Set headers (Content-Type: application/json)
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Extract and return the iamToken from the response
            // std::cout << "AIM Token Response: " << readBuffer << std::endl;
        }

        // Clean up headers
        curl_slist_free_all(headers);

        // Cleanup CURL
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "CURL initialization failed!" << std::endl;
    }

    curl_global_cleanup();

    // Extract and return iamToken
    return extractIamToken(readBuffer);
}
