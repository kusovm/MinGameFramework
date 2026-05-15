#include "time.hpp"

#include <Windows.h>

namespace gameTime {
    unsigned int lastRecordedTime = 0;
    unsigned int frameTime = 0;

    void updateFrameTime()
    {
        unsigned int currentTime = timeGetTime();

        frameTime = currentTime - lastRecordedTime;
        lastRecordedTime = currentTime;
    }

    unsigned int getFrameTime()
    {
        return frameTime;
    }
}
