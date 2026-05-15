#include "entities.hpp"

#include <cstring>
#include <cstdint>
#include <array>
#include <list>
#include <Windows.h>

#include "utils.hpp"
#include "renderer.hpp"
#include "time.hpp"
#include "state.hpp"
#include "scenes.hpp"

// I probably could have done this in some 
// other better way but this works fine for now
#define RUN_TIMER(timer, timerMax, onTimer) do {                              \
    unsigned int nextTimerValue = timer - gameTime::getFrameTime();           \
                                                                              \
    /* Since timer is an uint we cannot check if it is/will     */            \
    /* be less than zero, so I am checking for overflow instead */            \
    if (nextTimerValue <= timer) {                                            \
        timer = nextTimerValue;                                               \
    } else {                                                                  \
        onTimer                                                               \
        timer = timerMax;                                                     \
    }                                                                         \
} while (0)

namespace label {
    static void render(const void *data)
    {
        const auto *self = reinterpret_cast<const Label *>(data); 

        if (!self->centered)
            renderer::draw(self->position, self->text);
        else
            renderer::drawCenteredX(self->position, self->text);
    }

    static void destroy(void *data)
    {
        auto *self = reinterpret_cast<Label *>(data); 
        delete self;
    }

    Entity create(Vec2 position, bool centered, const char *text)
    {
        void *data = new Label{
            position,
            centered,
            text,
        };

        return {
            nullptr, // initialize
            nullptr, // tick
            render,
            destroy,
            data,
        };
    }
}

namespace sceneSwitcher {
    static void tick(void *data)
    {
        auto *self = reinterpret_cast<SceneSwitcher *>(data); 

        if (GetAsyncKeyState(self->vkey) < 0)
            scenes::switchScene(self->nextSceneIndex);
    }

    static void destroy(void *data)
    {
        auto *self = reinterpret_cast<SceneSwitcher *>(data); 
        delete self;
    }

    Entity create(int vkey, size_t nextSceneIndex)
    {
        void *data = new SceneSwitcher{
            vkey,
            nextSceneIndex,
        };

        return {
            nullptr, // initialize
            tick,
            nullptr, // render
            destroy,
            data,
        };
    }
}

namespace player {
    static void tick(void *data)
    {
        auto *self = reinterpret_cast<Player *>(data); 

        Vec2 direction = {0, 0};

        if (GetAsyncKeyState('W') < 0) 
            direction.y -= 1;

        if (GetAsyncKeyState('S') < 0)
            direction.y += 1;

        if (GetAsyncKeyState('A') < 0)
            direction.x -= 1;

        if (GetAsyncKeyState('D') < 0)
            direction.x += 1;

        // Resetting the timer if the player has not moved
        if (direction.x == 0 && direction.y == 0) {
            self->movementTimer = 0;
            return;
        }

        RUN_TIMER(self->movementTimer, self->movementTimerMax, {
            int nextX = self->position.x + direction.x;
            int nextY = self->position.y + direction.y;

            // Clamping the position to the screen bounds
            if (nextX >= 0 && nextX < renderer::bufferSize.x)
                self->position.x = nextX;

            if (nextY >= 0 && nextY < renderer::bufferSize.y)
                self->position.y = nextY;
        });
    }

    static void render(const void *data)
    {
        const auto *self = reinterpret_cast<const Player *>(data); 
        renderer::draw(self->position, '}');
    }

    static void destroy(void *data)
    {
        auto *self = reinterpret_cast<Player *>(data); 
        delete self;
    }

    Entity create(Vec2 position)
    {
        void *data = new Player{
            position,
        };

        return {
            nullptr, // initialize
            tick,
            render,
            destroy,
            data,
        };
    }
}

namespace asteroidManager {
    // NOTE: For the purposes of time and keeping the scope of this 
    // framework manageable all of the collision detection and object 
    // instantiation is done as part of the asteroid manager's logic. 
    // Since this is the only place where I need these systems I decided 
    // that it would be better to just implement them here, instead of 
    // creating new systems. This is not scalable, but for the scope of 
    // this project it works good enough

    static void moveAsteroid(Asteroid *self)
    {
        RUN_TIMER(self->movementTimer, self->movementTimerMax, {
            self->position.x -= 1;
        });
    }

    static void renderAsteroid(const Asteroid *self)
    {
        renderer::draw(self->position, '@');
    }

    static void spawnAsteroid(AsteroidManager *self)
    {
        Asteroid *newAsteroid;

        Vec2 spawnPosition = { 
            renderer::bufferSize.x - 1, 
            getRandomValue(0, renderer::bufferSize.y - 1) 
        };

        if (self->inactiveAsteroids.empty()) {
            // There are no asteroids that we 
            // can reuse, so we spawn a new one
            newAsteroid = new Asteroid{spawnPosition};
        } else {
            // There is an inactive asteroid that we can reuse
            newAsteroid = self->inactiveAsteroids.front();
            self->inactiveAsteroids.pop_front();
            newAsteroid->position = spawnPosition;
        }

        self->activeAsteroids.push_back(newAsteroid);
    }

    static void initialize(void *data)
    {
        auto *self = reinterpret_cast<AsteroidManager *>(data); 

        auto *player = reinterpret_cast<player::Player *>(
            scenes::getObjectData(self->playerIndex)
        );

        self->playerPosition = &(player->position);
    }

    static void tick(void *data)
    {
        auto *self = reinterpret_cast<AsteroidManager *>(data); 

        RUN_TIMER(self->spawnTimer, self->spawnTimerMax, {
            spawnAsteroid(self);
        });

        if (self->activeAsteroids.empty())
            return;

        // We only need to check the oldest spawned asteroid 
        // because the speeds of all asteroids are equal
        Asteroid *oldestAsteroid = self->activeAsteroids.front();

        if (oldestAsteroid->position.x < 0)
        {
            self->inactiveAsteroids.push_back(oldestAsteroid);
            self->activeAsteroids.pop_front();
        }

        for (Asteroid *asteroid : self->activeAsteroids)
        {
            moveAsteroid(asteroid);
            if (asteroid->position == *(self->playerPosition))
                scenes::switchScene(2);
        }
    }

    static void render(const void *data)
    {
        const auto *self = reinterpret_cast<const AsteroidManager *>(data); 

        if (self->activeAsteroids.empty())
            return;

        for (const Asteroid *asteroid : self->activeAsteroids)
            renderAsteroid(asteroid);
    }

    static void destroy(void *data)
    {
        auto *self = reinterpret_cast<AsteroidManager *>(data); 

        while (!self->inactiveAsteroids.empty()) {
            Asteroid *asteroid = self->inactiveAsteroids.front();
            self->inactiveAsteroids.pop_front();
            delete asteroid;
        }

        while (!self->activeAsteroids.empty()) {
            Asteroid *asteroid = self->activeAsteroids.front();
            self->activeAsteroids.pop_front();
            delete asteroid;
        }

        delete self;
    }

    Entity create(size_t playerIndex)
    {
        void *data = new AsteroidManager{playerIndex};

        return {
            initialize,
            tick,
            render, 
            destroy,
            data,
        };
    }
}

namespace scoreCounter {
    static void tick(void *data)
    {
        auto *self = reinterpret_cast<ScoreCounter *>(data); 

        self->timeAccumulator += gameTime::getFrameTime();

        if (self->timeAccumulator < self->timeAccumulatorMax)
            return;

        self->score += self->timeAccumulator / self->timeAccumulatorMax;
        self->timeAccumulator %= self->timeAccumulatorMax;

        // Updating displayed text
        writeUInt32ToCString(self->managedString, self->score);
    }

    static void destroy(void *data)
    {
        auto *self = reinterpret_cast<ScoreCounter *>(data); 

        state::submitScore(self->score);

        delete[] self->managedString;
        delete self;
    }

    static void initialize(void *data)
    {
        auto *self = reinterpret_cast<ScoreCounter *>(data); 

        // +1 for null terminator
        constexpr size_t labelTextLength 
            = getUnsignedMaxDisplayLength<uint32_t>() + 1;

        self->managedString = new char[labelTextLength]{'0'};

        auto *managedLabel = reinterpret_cast<label::Label *>(
            scenes::getObjectData(self->managedLabelIndex)
        );

        managedLabel->text = self->managedString;
    }

    Entity create(size_t managedLabelIndex)
    {
        void *data = new ScoreCounter{managedLabelIndex};

        return {
            initialize, 
            tick,
            nullptr, // render
            destroy,
            data,
        };
    }
}

namespace leaderboard {
    static void destroy(void *data)
    {
        auto *self = reinterpret_cast<Leaderboard *>(data); 

        delete[] self->managedString;
        delete self;
    }

    static void initialize(void *data)
    {
        auto *self = reinterpret_cast<Leaderboard *>(data); 

        // +1 for newlines/null terminator
        constexpr size_t labelTextSize 
            = (getUnsignedMaxDisplayLength<uint32_t>() + 1) 
               * state::highscoresSize;

        self->managedString = new char[labelTextSize];

        char *ptrToText = self->managedString;
        for (size_t i = 0; i < state::highscoresSize; i++)
        {
            ptrToText = writeUInt32ToCString(
                ptrToText,
                state::highscoresPtr[i], 
                false // Do not write null terminator
            );

            *ptrToText = i != state::highscoresSize - 1 
                         ? '\n' 
                         : '\0';

            ptrToText++;
        }

        label::Label *managedLabel = reinterpret_cast<label::Label *>(
            scenes::getObjectData(self->managedLabelIndex)
        );

        managedLabel->text = self->managedString;
    }

    Entity create(size_t managedLabelIndex)
    {
        void *data = new Leaderboard{managedLabelIndex};

        return {
            initialize,
            nullptr, // tick
            nullptr, // render
            destroy,
            data,
        };
    }
}

