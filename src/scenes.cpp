#include "scenes.hpp"

#include <Windows.h>
#include <cassert>
#include <list>

#include "state.hpp"
#include "localization.hpp"
#include "entities.hpp"
#include "renderer.hpp"

namespace scenes {
    void replaceCurrentScene(size_t index);
    void destroyCurrentScene();

    namespace factories {
        Scene createTitleScene();
        Scene createGameScene();
        Scene createGameOverScene();
    }

    Scene currentScene;

    // These are used to check if we need to 
    // switch a scene at the start of the frame
    size_t currentSceneIndex = 0;
    size_t nextSceneIndex = 0;

    void initialize()
    {
        // I am making this and cleanup() separate functions to be able to 
        // change the initialization/cleanup routines only in this file 
        // without having to change other files

        replaceCurrentScene(0);
    }

    void doFrame()
    {
        // Switch scenes if was requested
        if (currentSceneIndex != nextSceneIndex)
        {
            destroyCurrentScene();
            replaceCurrentScene(nextSceneIndex);

            currentSceneIndex = nextSceneIndex;
        }
        
        // Tick
        for (Entity &entity : currentScene)
            if (entity.tick != nullptr)
                entity.tick(entity.data);

        // Render
        for (const Entity &entity : currentScene)
            if (entity.render != nullptr)
                entity.render(entity.data);
    }

    void cleanup()
    {
        // See initialize()

        destroyCurrentScene();
    }

    void switchScene(size_t index)
    {
        nextSceneIndex = index;
    }

    inline void replaceCurrentScene(size_t index)
    {
        static constexpr Scene (*sceneFactories[])() = {
            factories::createTitleScene,
            factories::createGameScene,
            factories::createGameOverScene,
        };

        assert(index < sizeof(sceneFactories) / sizeof(sceneFactories[0]) 
               && "Scene index is too big");

        currentScene = sceneFactories[index]();

        for (Entity &entity : currentScene)
            if (entity.initialize != nullptr)
                entity.initialize(entity.data);
    }

    inline void destroyCurrentScene()
    {
        for (
            auto it = currentScene.rbegin(); 
            it != currentScene.rend(); 
            it++
        ) {
            Entity &entity = *it;

            assert(entity.data == nullptr || entity.destroy != nullptr 
                   && "Entity has an undefined destructor");

            entity.destroy(entity.data);
        }
    }

    // NOTE: Accessing scene objects via indicies is a horrible idea. 
    // I only did this because the size of the game is small enough 
    // to make an ID system more of a burden than a useful thing
    void *getObjectData(size_t itemIndex)
    {
        Scene::const_iterator it = std::next(currentScene.begin(), itemIndex);
        return it->data;
    }

    namespace factories {
        Scene createTitleScene()
        {
            return {
                label::create(
                    { 
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y - 1 
                    }, 
                    true, // Centered
                    localization::getString("Title_Welcome")
                ),

                label::create(
                    { 
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y + 1 
                    }, 
                    true, // Centered
                    localization::getString("Title_Prompt")
                ),

                sceneSwitcher::create(VK_SPACE, 1 /* scene index */)
            };
        }

        Scene createGameScene()
        {
            return {
                player::create({10, renderer::bufferSizeHalf.y}),
                asteroidManager::create(0 /* player index */),

                label::create(
                    {renderer::bufferSizeHalf.x, 2},
                    true, // Centered
                    nullptr
                ),

                scoreCounter::create(2 /* label index */),
            };
        }

        Scene createGameOverScene()
        {
            int leaderboardHalfHeight 
                = static_cast<int>(state::highscoresSize) / 2;

            return {
                label::create(
                    { 
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y - 3 - leaderboardHalfHeight
                    }, 
                    true, // Centered
                    localization::getString("GO_GameOver")
                ),

                label::create(
                    { 
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y - 1 - leaderboardHalfHeight
                    }, 
                    true, // Centered
                    localization::getString("GO_TopScores")
                ),

                label::create(
                    {
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y - leaderboardHalfHeight
                    },
                    true, // Centered
                    nullptr
                ),

                leaderboard::create(2 /* label index */),

                label::create(
                    { 
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y + 1 + leaderboardHalfHeight
                    }, 
                    true, // Centered
                    localization::getString("GO_PromptRestart")
                ),

                label::create(
                    { 
                        renderer::bufferSizeHalf.x, 
                        renderer::bufferSizeHalf.y + 2 + leaderboardHalfHeight
                    }, 
                    true, // Centered
                    localization::getString("GO_PromptMenu")
                ),

                sceneSwitcher::create(VK_SPACE, 1 /* scene index */),
                sceneSwitcher::create(VK_TAB, 0 /* scene index */),
            };
        }
    }
}

