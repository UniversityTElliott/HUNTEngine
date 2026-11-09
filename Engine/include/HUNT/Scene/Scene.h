#pragma once
#include "HUNT/Objects/HUNTObject.h"
#include "HUNT/AssetHandler.h"
#include "HUNT/Input/InputHandler.h"
#include <HUNT/Camera/Camera.h>

#include <string>
#include <memory>
#include <vector>

namespace HUNT
{
    class Scene
    {
    public:
        explicit Scene(const std::string& name)
            : sceneName(name)
        {
        }

        virtual ~Scene() = default;

        //All parts of the scene
        virtual void Init(AssetHandler& assets) {}
        virtual void Update(float deltaTime) {}
        virtual void FixedUpdate(InputHandler& input) {}
        virtual void Destroy() {}

        //for switching
        virtual void OnEnter() {}
        virtual void OnExit() {}

        const std::string& GetName()
        {
            return sceneName;
        }
        const std::vector<std::unique_ptr<HUNTObject>>& GetObjects()
        {
            return objects;
        }
        const bool& IsInitialised() //Ameircan c++
        {
            return isInitialised;
        }
        void setInitialised() {
            isInitialised = true;
        }

        Camera* GetActiveCamera()
        {
            if (!activeCamera) {
                return nullptr;
            }
            return activeCamera;
        }
        //Maybe use a name system like the assetHandler idk we will see this works for now
        Camera* AddCamera()
        {
            cameras.push_back(std::make_unique<Camera>());
            Camera* newCamera = cameras.back().get();

            //If no camera is set then make the next added camera the main one
            if (activeCamera == nullptr)
            {
                activeCamera = newCamera;
            }

            return newCamera;
        }

        void SetActiveCamera(Camera* camIn){
            
            //Check the camera is in the list of cameras
            //TODO:: if not make it? idk
            for (const auto& camera : cameras)
            {
                if (camera.get() == camIn)
                {
                    activeCamera = camIn;
                    return;
                }
            }

        }

    protected:
        std::vector<std::unique_ptr<HUNTObject>> objects;
        std::vector<std::unique_ptr<Camera>> cameras;
        Camera* activeCamera = nullptr;
        bool isInitialised = false;

    private:
        std::string sceneName;
    };
}