// This module contains miscellaneous things that are way too small to be 
// their own modules

#pragma once

#include <cstdint>
#include <limits>
#include <Windows.h>

int getRandomValue(int min, int max);

char *writeUInt32ToCString(
    char *ptrToChar, 
    uint32_t number, 
    bool insertNullTerminator = true);

template <typename T>
constexpr size_t getUnsignedMaxDisplayLength()
{
    size_t length = 0;

    auto maxValue = (std::numeric_limits<T>::max)();

    while (maxValue > 0) {
        maxValue /= 10;
        length++;
    }

    return length;
}

struct Vec2
{
    int x;
    int y;

    Vec2 &operator+=(Vec2 rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    Vec2 &operator-=(Vec2 rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    Vec2 &operator*=(Vec2 rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }

    Vec2 &operator/=(Vec2 rhs)
    {
        this->x /= rhs.x;
        this->y /= rhs.y;
        return *this;
    }
};

inline Vec2 operator+(Vec2 lhs, Vec2 rhs)
{
    lhs += rhs;
    return lhs;
}

inline Vec2 operator-(Vec2 lhs, Vec2 rhs)
{
    lhs -= rhs;
    return lhs;
}

inline Vec2 operator*(Vec2 lhs, Vec2 rhs)
{
    lhs *= rhs;
    return lhs;
}

inline Vec2 operator/(Vec2 lhs, Vec2 rhs)
{
    lhs /= rhs;
    return lhs;
}

inline bool operator==(Vec2 lhs, Vec2 rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(Vec2 lhs, Vec2 rhs)
{
    return !(lhs == rhs);
}

