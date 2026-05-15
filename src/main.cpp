#include "utils.hpp"
#include "time.hpp"
#include "state.hpp"
#include "localization.hpp"
#include "renderer.hpp"
#include "scenes.hpp"
#include "entities.hpp"

int main()
{
    localization::loadResourceFile();

    state::loadScores();
    scenes::initialize();

    renderer::setCursor(false); 
    renderer::clear(); 

    bool exit = false;
    while (!exit)
    {
        if (GetAsyncKeyState(VK_ESCAPE) < 0)
            exit = true;

        renderer::clear(); 
        scenes::doFrame();
        renderer::flush(); 

        gameTime::updateFrameTime();
    }

    renderer::setCursor(true); 

    state::saveScores();
    scenes::cleanup();

    localization::cleanup();

    return 0;
}

