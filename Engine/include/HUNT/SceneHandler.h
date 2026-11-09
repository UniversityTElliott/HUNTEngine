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

            //If its the first scene made make it the 
            if (allScenes.empty()) {
                SetCurrentScene(scenePtr);

            }

            allScenes.push_back(std::move(scene));

            return scenePtr;
        }

        bool SetCurrentScene(Scene* scene);

        Scene* GetCurrentScene() const;

        const std::vector<std::unique_ptr<Scene>>&GetScenes() const;
        void Init(AssetHandler& asset);
        void Update(float deltaTime);
        void FixedUpdate(InputHandler& input);
        void Render(Renderer& renderer);

    private:
        std::vector<std::unique_ptr<Scene>> allScenes;

        Scene* currentScene = nullptr;
        
    };
}