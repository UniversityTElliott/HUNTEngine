#pragma once

#include "HUNT/Scene/Scene.h"

#include <memory>
#include <string>
#include <vector>

namespace HUNT
{
    class SceneHandler
    {
    public:
        //Template function to allow for different types of Scene to all be stored in the handler
        template<typename T, typename... Args>
        T* CreateScene(Args&&... args)
        {
            static_assert(std::is_base_of_v<Scene, T>,"T must inherit from HUNT::Scene");

            auto scene = std::make_unique<T>(std::forward<Args>(args)...);

            T* scenePtr = scene.get();

            allScenes.push_back(std::move(scene));

            return scenePtr;
        }

        bool SetActiveScene(Scene* scene);

        Scene* GetActiveScene() const;

        const std::vector<std::unique_ptr<Scene>>&GetScenes() const;

        void Update(float deltaTime);
        void FixedUpdate();
        void Render();

    private:
        std::vector<std::unique_ptr<Scene>> allScenes;

        Scene* currentScene = nullptr;
    };
}