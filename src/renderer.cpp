#include "renderer.hpp"

#include <cstring>
#include <cstdio>

namespace renderer {
    bool isInBounds(Vec2 cell);

    const HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);

    // Adding + 1 to bufferSize.x to store null terminators
    char renderBuffer[bufferSize.y][bufferSize.x + 1]; 

    void clear()
    {
        std::memset(renderBuffer, ' ', sizeof(renderBuffer));

        for (int y = 0; y < bufferSize.y - 1; y++)
            renderBuffer[y][bufferSize.x] = '\n';

        renderBuffer[bufferSize.y - 1][bufferSize.x] = '\0';
    }

    void draw(Vec2 cell, const char *text)
    {
        int originX = cell.x;

        const char *cptr = text;
        while (*cptr != '\0') {
            char c = *cptr;

            if (c != '\n') {
                if (isInBounds(cell))
                    renderBuffer[cell.y][cell.x] = c;

                cell.x++;
            } else {
                cell.y++;
                cell.x = originX;
            }

            cptr++;
        }
    }

    void draw(Vec2 cell, const char c)
    {
        if (!isInBounds(cell))
            return;

        renderBuffer[cell.y][cell.x] = c;
    }

    void drawCenteredX(Vec2 cell, const char *text)
    {
        int originX = cell.x;

        const char *cptr = text;
        while (*cptr != '\0') {
            int lineLength = 0;

            while (*cptr != '\n' && *cptr != '\0')
            {
                lineLength++;
                cptr++;
            }

            cptr -= lineLength;
            cell.x = originX - lineLength / 2;

            while (*cptr != '\n' && *cptr != '\0')
            {
                if (isInBounds(cell))
                    renderBuffer[cell.y][cell.x] = *cptr;

                cell.x++;
                cptr++;
            }

            if (*cptr == '\n')
            {
                cptr++;
                cell.y++;
            }
        }
    }

    void flush()
    {
        SetConsoleCursorPosition(screen, {0, 0});
        std::puts(renderBuffer[0]);
    }

    void setCursor(bool on)
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(screen, &cursorInfo);
        cursorInfo.bVisible = on;
        SetConsoleCursorInfo(screen, &cursorInfo);
    }

    inline bool isInBounds(Vec2 cell)
    {
        return cell.x >= 0 
               && cell.x < bufferSize.x 
               && cell.y >= 0 
               && cell.y < bufferSize.y;
    }
}
