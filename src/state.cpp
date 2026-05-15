#include "state.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <fstream>
#include <algorithm>

namespace state {
    static uint32_t highscores[highscoresSize];
    const uint32_t *highscoresPtr = highscores;

    static constexpr char filepath[] = "save.bin";

    void submitScore(uint32_t newScore)
    {
        for (int i = 0; i < highscoresSize; i++) {
            uint32_t score = highscores[i];

            if (score >= newScore)
                continue;

            std::memmove(
                highscores + i + 1, 
                highscores + i, 
                (10 - i - 1) * sizeof(uint32_t)
            );

            highscores[i] = newScore;
            return;
        }
    }

    void saveScores()
    {
        std::ofstream file(filepath, std::ios::binary | std::ios::trunc);

        if (!file.is_open())
            return;

        // Writing byte-by-byte because I want to ensure little-endianness.
        // This is probably not relevant for this particular game, but in my 
        // opinion it is a good habit to always keep things like this in mind
        for (uint32_t entry : highscores) {
            for (size_t byte = 0; byte < sizeof(uint32_t); byte++) {
                file.put(static_cast<unsigned char>(entry));

                if (!file)
                    return;

                entry >>= 8;
            }
        }

        file.close();
    }

    void loadScores()
    {
        std::ifstream file(filepath, std::ios::binary);

        if (!file.is_open())
            return;

        for (size_t i = 0; i < highscoresSize; i++) {
            for (size_t offset = 0; offset < sizeof(uint32_t); offset++) {
                char byteCharSigned;

                if (!file.get(byteCharSigned))
                    return;

                // This prevents sign-extension
                uint32_t byteCharUnsigned 
                    = static_cast<unsigned char>(byteCharSigned);

                uint32_t byte = static_cast<uint32_t>(byteCharUnsigned);
                byte <<= 8 * offset;
                highscores[i] |= byte;
            }
        }

        file.close();
    }
}
