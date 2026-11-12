#include "HUNT/SceneHandler.h"
#include "HUNT/Graphics/GPU/ShaderBuffers.h"
#include "HUNT/Math/mathFunctions.h"
#include "HUNT/Graphics/Renderer.h"

namespace HUNT
{
    bool SceneHandler::SetCurrentScene(Scene* scene)
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

    Scene* SceneHandler::GetCurrentScene() const
    {
        return currentScene;
    }

    void SceneHandler::ProcessSceneChange() {
        
        if (currentScene && currentScene->IsSceneChangeRequested()) {
            for (const auto& scene : allScenes)
            {

                if (scene->GetName() == currentScene->GetSceneRequestName())
                {

                    SetCurrentScene(scene.get());
                    scene->resetSceneChangeRequested();
                    return;
                }
            }

            //Say no scene found 
            currentScene->resetSceneChangeRequested();
        }


    }

    //Read up as to why double const works but single doesnt?
    const std::vector<std::unique_ptr<Scene>>& SceneHandler::GetScenes() const
    {
        return allScenes;
    }

    void SceneHandler::Init(Renderer& rendererIn)
    {
        if (currentScene && !currentScene->IsInitialised()) {
            currentScene->setInitialised();
            currentScene->SetPhysicsWorld(std::make_unique<PhysicsWorld>());
            currentScene->GetPhysicsWorld()->Init();
            currentScene->SetSceneAssets(std::make_unique<AssetHandler>(&rendererIn));
            currentScene->Init();
        }
    }

    void SceneHandler::Update(InputHandler& input,float deltaTime)
    {
        if (currentScene) {
            currentScene->Update(input,deltaTime);
        }
    }
    void SceneHandler::FixedUpdate(InputHandler& input)
    {
        if (currentScene) {
            currentScene->FixedUpdate(input);
        }
    }

    void SceneHandler::Render(Renderer& renderer)
    {
        Camera* camera = currentScene->GetActiveCamera();

        if (camera)
        {
            HUNT::MatrixBufferType matrices = HUNT::makeGPUMatrix(HUNT::mat4(1.f), camera->getViewMatrix(), HUNT::perspectiveLH(HUNT::toRadians(90.f), 1080.f / 720.f, 0.1f, 200.f));


            // Set projection here
            renderer.SetCameraMatrix(matrices);
        }



        if (currentScene) {
            for (const auto& object : currentScene->GetObjects())
            {
                object->Render(renderer);
            }
        }
    }
}