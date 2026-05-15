#include "localization.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>

namespace localization {
    static std::unordered_map<std::string, const char *> fallback{
        { "Title_Welcome",      "Welcome to my game framework!" },
        { "Title_Prompt",       "[ Press SPACE to start ]" },

        { "GO_GameOver",        "Game Over!" },
        { "GO_TopScores",       "Top scores:" },
        { "GO_PromptRestart",   "[ Press SPACE to restart ]" },
        { "GO_PromptMenu",      "[ Press TAB to go to main menu ]" },
    };

    static std::unordered_map<std::string, const char *> displayedStrings{};

    static constexpr char filepath[] = "loc.txt";

    void loadResourceFile()
    {
        std::ifstream file(filepath);

        if (!file.is_open())
            return;

        std::string line;
        while (std::getline(file, line))
        {
            const char *c = line.c_str();

            // Skipping whitespace at the start of the line
            const char *keyStart = nullptr;

            while (*c != '\0') {
                if (*c != ' ' && *c != '\t') {
                    keyStart = c;
                    break;
                }

                c++;
            }

            // Line is an empty string
            if (keyStart == nullptr)
                continue;

            // Skipping comments
            if (*c == '#')
                continue;

            // Reading the key until colon
            const char *keyEnd = nullptr;

            while (*c != '\0') {
                if (*c == ':') {
                    keyEnd = c;
                    break;
                }

                c++;
            }

            c++;

            // Invalid string (no key terminator)
            if (keyEnd == nullptr)
                continue;

            // Skipping whitespace after the colon
            const char *valueStart = nullptr;

            while (*c != '\0') {
                if (*c != ' ' && *c != '\t') {
                    valueStart = c;
                    break;
                }

                c++;
            }

            // Constructing the key
            std::string key(keyStart, keyEnd);

            // Constructing the value
            size_t valueLength = (line.c_str() + line.length()) - valueStart;

            char *value = new char[valueLength + 1];

            std::memcpy(value, valueStart, valueLength);
            value[valueLength] = '\0';

            // Adding the pair to the dictionary
            displayedStrings[key] = value;
        }

        file.close();
    }

    const char *getString(const std::string &key)
    {
        if (displayedStrings.find(key) != displayedStrings.end())
            return displayedStrings[key];

        if (fallback.find(key) != fallback.end())
            return fallback[key];

        return nullptr;
    }

    void cleanup()
    {
        // Deleting allocated char arrays
        for (const auto &pair : displayedStrings)
            delete[] pair.second;
    }
}
