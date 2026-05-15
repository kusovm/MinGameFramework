#include "utils.hpp"

#include <cstdint>
#include <random>

int getRandomValue(int min, int max)
{
    static std::random_device randomDevice;
    static std::mt19937 engine(randomDevice());

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(engine);
}

// This is using recursion because this was one 
// of the criteria for the original assignment
char *writeUInt32ToCString(
    char *ptrToChar, 
    uint32_t number, 
    bool insertNullTerminator)
{
    char digit = '0' + (number % 10);

    uint32_t truncatedNumber = number / 10;
    if (truncatedNumber > 0)
        ptrToChar = writeUInt32ToCString(ptrToChar, truncatedNumber, false);

    *ptrToChar = digit;

    if (insertNullTerminator) {
        ptrToChar++;
        *ptrToChar = '\0';
    }

    return ++ptrToChar;
}
