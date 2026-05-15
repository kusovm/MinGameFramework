// This module contains all scene management functionality.
//
// The active scene is a global std::list (this was one of the requirements 
// for the assignment) variable that contains Entity objects. It is created 
// from the hardcoded std::list objects returned by functions in "factories".
//
// When "switchScene" gets called, the game waits for the beginning of the 
// next frame and then destroyes the current scene, along with all contained 
// objects. The variable is then populated from one of the scene templates in 
// one of the functions in "factories", depending on which index was passed in 
// "switchScene".
//
// If "switchScene" was called multiple times during a single frame the last 
// call takes precedence.

#pragma once

#include <list>

#include "entities.hpp"

using Scene = std::list<Entity>;

namespace scenes {
    void initialize();
    void doFrame();
    void cleanup();
    void switchScene(size_t index);

    void *getObjectData(size_t itemIndex);
}
