#include "HUNT/SceneHandler.h"

namespace HUNT
{
    bool SceneHandler::SetActiveScene(Scene* scene)
    {
        if (!scene) {
            return false;

        }
            
        if (currentScene) {
            currentScene->OnExit();
        }

        currentScene = scene;
        currentScene->OnEnter();

        return true;
    }

    Scene* SceneHandler::GetActiveScene() const
    {
        return currentScene;
    }

    //Read up as to why double const works but single doesnt?
    const std::vector<std::unique_ptr<Scene>>& SceneHandler::GetScenes() const
    {
        return allScenes;
    }

 

    void SceneHandler::Update(float deltaTime)
    {
        if (currentScene) {
            currentScene->Update(deltaTime);
        }
    }
    void SceneHandler::FixedUpdate()
    {
        if (currentScene) {
            currentScene->FixedUpdate();
        }
    }

    void SceneHandler::Render()
    {
        if (currentScene)
            currentScene->Render();
    }
}