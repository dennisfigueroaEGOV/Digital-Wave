#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string parseJsonString(const std::string& jsonData, const std::string& key) {
    size_t keyPosition = jsonData.find("\"" + key + "\":");
    if (keyPosition == std::string::npos) return "";
    size_t valueStartPosition = jsonData.find("\"", keyPosition + key.length() + 2);
    if (valueStartPosition == std::string::npos) return "";
    size_t valueEndPosition = jsonData.find("\"", valueStartPosition + 1);
    if (valueEndPosition == std::string::npos) return "";
    return jsonData.substr(valueStartPosition + 1, valueEndPosition - valueStartPosition - 1);
}

std::string parseJsonNumber(const std::string& jsonData, const std::string& key) {
    size_t keyPosition = jsonData.find("\"" + key + "\":");
    if (keyPosition == std::string::npos) return "";
    size_t valueStartPosition = keyPosition + key.length() + 2;
    size_t valueEndPosition = jsonData.find_first_of(",}", valueStartPosition);
    if (valueEndPosition == std::string::npos) return "";
    return jsonData.substr(valueStartPosition, valueEndPosition - valueStartPosition);
}

bool parseJsonBool(const std::string& jsonData, const std::string& key) {
    size_t keyPosition = jsonData.find("\"" + key + "\":");
    if (keyPosition == std::string::npos) return false;
    size_t valueStartPosition = keyPosition + key.length() + 2;
    return (jsonData.substr(valueStartPosition, 4) == "true");
}

void parseTrade(const std::string& trade) {
    std::cout << "{\n";
    std::cout << "  \"a\": " << parseJsonNumber(trade, "a") << ", // Aggregate tradeId\n";
    std::cout << "  \"p\": \"" << parseJsonString(trade, "p") << "\", // Price\n";
    std::cout << "  \"q\": \"" << parseJsonString(trade, "q") << "\", // Quantity\n";
    std::cout << "  \"f\": " << parseJsonNumber(trade, "f") << ", // First tradeId\n";
    std::cout << "  \"l\": " << parseJsonNumber(trade, "l") << ", // Last tradeId\n";
    std::cout << "  \"T\": " << parseJsonNumber(trade, "T") << ", // Timestamp\n";
    std::cout << "  \"m\": " << (parseJsonBool(trade, "m") ? "true" : "false") << ", // Was the buyer the maker?\n";
    std::cout << "}\n";
}

std::vector<std::string> extractTrades(const std::string& json) {
    std::vector<std::string> extractedTrades;
    size_t searchPosition = 0;
    while ((searchPosition = json.find("{", searchPosition)) != std::string::npos) {
        size_t endPosition = json.find("}", searchPosition);
        if (endPosition == std::string::npos) break;
        extractedTrades.push_back(json.substr(searchPosition, endPosition - searchPosition + 1));
        searchPosition = endPosition + 1;
    }
    return extractedTrades;
}

int main() {
    // Fetching trades from endpoint: O(1)
    // Extracting trades: O(n)
    // Parsing each trade: O(t) -> O(n)
    // Total time complexity: O(n)
    // Basically demonstrates linear time complexity relative to the size of the json string from response.

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://fapi.binance.com/fapi/v1/aggTrades?symbol=BTCUSDT";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Calling curl_easy_perform() has failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::vector<std::string> trades = extractTrades(readBuffer);

            std::cout << "Number of trades extracted: " << trades.size() << std::endl;

            auto start = std::chrono::high_resolution_clock::now();

            for (const auto& trade : trades) {
                parseTrade(trade);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            std::cout << "Time to parse: " << duration.count() << " microseconds" << std::endl;
            if (!trades.empty()) {
                std::cout << "Average time per trade: " << duration.count() / trades.size() << " microseconds" << std::endl;
            } else {
                std::cout << "No trades" << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }

    return 0;
}