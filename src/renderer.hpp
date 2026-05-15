// This module contains all functionality related to displaying things in the 
// terminal.
//
// The game uses an internal char[] buffer which is flushed in the console 
// with a single call at the end of every frame. 
//
// The renderer supports multiline strings and can center them horizontally. 
// It also automatically discards any draw calls that are outside of the 
// bounds of the char buffer.
//
// The render buffer is 80 cols by 24 rows by default, which can be changed

#pragma once

#include "utils.hpp"

namespace renderer {
    inline constexpr Vec2 bufferSize{80, 24};

    inline constexpr Vec2 bufferSizeHalf{ 
        bufferSize.x / 2, 
        bufferSize.y / 2 
    };

    void clear();

    void draw(Vec2 cell, const char *text);
    void draw(Vec2 cell, const char c);
    void drawCenteredX(Vec2 cell, const char *text);

    void flush();

    void setCursor(bool on);
}

