// This module contains all game entities.
//
// I decided not to use OOP for this game and instead use functional/procedural 
// approach, because from my experience OOP is a lot more difficult to debug 
// (especially polymorphic functions), often obscures order of execution and 
// has higher overhead because of indirection (specifically the 'object * -> 
// vptr -> vtable -> function' chain for every non-static virtual method).
//
// The way "objects" work in this game is as follows:
//
//      1) Every "object" is an Entity struct, which has 4 pointers to basic 
//         lifecycle functions and an additional void pointer, which points to 
//         the heap-allocated struct with data used in the aformentioned 
//         functions
//
//      2) Every frame "tick" and "render" get called with a pointer to "data" 
//         as an argument. This void * is then reinterpret_cast to the actual 
//         struct type. This is can be dangerous if someone passes a struct 
//         with some wrong type to a function that expects a different type, 
//         but from what I could find on Google there really is no better 
//         solution to this problem. I tried to somewhat mitigate this by 
//         using namespaces, since this is mostly a human error, but hopefully 
//         the scale of the game is small enough for this to work out fine.
//
//      3) "initialize" gets called after all of the objects in the scene have 
//         been created, and "destroy" gets called just before the scene gets 
//         deallocated for all entities with data != nullptr in the reverse 
//         order of their construction
//
//      4) Instead of constructors I use factory functions that return Entity 
//         object (not pointer)

#pragma once

#include <cstdint>
#include <deque>
#include <Windows.h>

#include "utils.hpp"

struct Entity
{
    void (*initialize)(void *);
    void (*tick)(void *);
    void (*render)(const void *);
    void (*destroy)(void *);

    void *data; 
};

namespace label {
    struct Label
    {
        Vec2 position = {0, 0};
        bool centered = false;
        const char *text;
    };

    Entity create(Vec2 position, bool centered, const char *text);
}

namespace sceneSwitcher {
    struct SceneSwitcher
    {
        int vkey;
        size_t nextSceneIndex;
    };

    Entity create(int vkey, size_t nextSceneIndex);
}

namespace player {
    struct Player
    {
        Vec2 position;

        // In milliseconds
        unsigned int movementTimer = 0u;
        const unsigned int movementTimerMax = 25u;
    };

    Entity create(Vec2 position);
}

namespace asteroidManager {
    struct Asteroid
    {
        Vec2 position;

        // In milliseconds
        unsigned int movementTimer = 0u;
        const unsigned int movementTimerMax = 10u;
    };

    struct AsteroidManager
    {
        const size_t playerIndex;
        const Vec2 *playerPosition = nullptr;

        std::deque<Asteroid *> inactiveAsteroids;
        std::deque<Asteroid *> activeAsteroids;

        // In milliseconds
        unsigned int spawnTimer = 0u;
        const unsigned int spawnTimerMax = 100u; 
    };

    Entity create(size_t playerIndex);
}

namespace scoreCounter {
    struct ScoreCounter
    {
        const size_t managedLabelIndex;
        char *managedString = nullptr;

        // Using a uint32 here because I am storing this value in a file
        uint32_t score = 0; 
        unsigned int timeAccumulator = 0u;
        unsigned int timeAccumulatorMax = 50u;
    };

    Entity create(size_t managedLabelIndex);
}

namespace leaderboard {
    struct Leaderboard
    {
        const size_t managedLabelIndex;
        char *managedString = nullptr;
    };

    Entity create(size_t managedLabelIndex);
}

