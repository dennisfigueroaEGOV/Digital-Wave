#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <algorithm>
#include <list>
#include <stdexcept>
#include <unordered_map>

class HashTable {
private:
    struct Entry {
        std::string key;
        int value;
        bool isOccupied;
    };

    std::vector<Entry> hashTableEntries;
    int tableSize;
    std::list<std::string> recentKeys;

    int computeHash(const std::string& key) const {
        unsigned long long hashValue = 0;
        for (char character : key) {
            hashValue = (hashValue * 31 + static_cast<unsigned char>(character)) % tableSize;
        }
        return static_cast<int>(hashValue);
    }

    void updateRecentKeys(const std::string& key) {
        recentKeys.remove(key);
        recentKeys.push_back(key);
    }

public:
    HashTable(int initialTableSize) : tableSize(initialTableSize) {
        hashTableEntries.resize(tableSize);
    }

    void insert(const std::string& key, int value) {
        int index = computeHash(key);
        while (hashTableEntries[index].isOccupied && hashTableEntries[index].key != key) {
            index = (index + 1) % tableSize;
        }
        hashTableEntries[index] = {key, value, true};
        updateRecentKeys(key);
    }

    void remove(const std::string& key) {
        int index = computeHash(key);
        while (hashTableEntries[index].isOccupied) {
            if (hashTableEntries[index].key == key) {
                hashTableEntries[index].isOccupied = false;
                recentKeys.remove(key);
                return;
            }
            index = (index + 1) % tableSize;
        }
    }

    int get(const std::string& key) const {
        int index = computeHash(key);
        while (hashTableEntries[index].isOccupied) {
            if (hashTableEntries[index].key == key) {
                return hashTableEntries[index].value;
            }
            index = (index + 1) % tableSize;
        }
        throw std::runtime_error("Key not found");
    }
    std::pair<std::string, int> get_last() const {
        if (recentKeys.empty()) {
            throw std::runtime_error("Hash table is empty");
        }
        const std::string& lastKey = recentKeys.back();
        return {lastKey, get(lastKey)};
    }

    std::pair<std::string, int> get_first() const {
        if (recentKeys.empty()) {
            throw std::runtime_error("Hash table is empty");
        }
        const std::string& firstKey = recentKeys.front();
        return {firstKey, get(firstKey)};
    }
};

std::vector<std::string> readFileContents(const std::string& filename) {
    std::ifstream inputFile(filename);
    std::vector<std::string> wordList;
    std::string currentWord;

    if (!inputFile) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return wordList;
    }

    while (inputFile >> currentWord) {
        currentWord.erase(
            std::remove_if(currentWord.begin(), currentWord.end(),
                           [](char c) { return std::ispunct(c); }),
            currentWord.end()
        );

        for (char& character : currentWord) {
            character = std::tolower(character);
        }

        if (!currentWord.empty()) {
            wordList.push_back(currentWord);
        }
    }

    inputFile.close();
    return wordList;
}

int main() {
    std::vector<std::string> words = readFileContents(TEXT_FILE_PATH);

    std::unordered_map<std::string, int> wordFrequency;
    for (const auto& word : words) {
        wordFrequency[word]++;
    }

    HashTable hashTable(wordFrequency.size() * 2);

    for (const auto& [word, frequency] : wordFrequency) {
        hashTable.insert(word, frequency);
    }

    try {
        hashTable.insert("test", 100);

        std::cout << "Frequency of 'the': " << hashTable.get("the") << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    auto [lastWord, lastFreq] = hashTable.get_last();
    std::cout << "Last inserted word: '" << lastWord << "' with frequency: " << lastFreq << std::endl;

    auto [firstWord, firstFreq] = hashTable.get_first();
    std::cout << "First inserted word: '" << firstWord << "' with frequency: " << firstFreq << std::endl;

    return 0;
}