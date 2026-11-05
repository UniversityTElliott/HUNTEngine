
#pragma once
#include "HUNTBaseComponent.h"
#include <cstdint>
#include <filesystem>
#include <HUNT/Graphics/HUNTMesh/Model.h>

#include <HUNT/Graphics/Renderer.h>
#include <HUNT/AssetManager.h>

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace HUNT {

    class HUNTModelComponent : public HUNTBaseComponent
    {
    public:
        HUNTModelComponent() = default;

        ~HUNTModelComponent() = default;
        void Init() final {

        };
        void Update(float dt) final {

        };
        void FixedUpdate() final {

        };
        void Render(Renderer& rendererIn) {
            for (auto& model : model->getModelMeshes()) {
                rendererIn.Render(model);
            }
            
        }

        void setModel(Model* data)
        {
            model = data;
        }
        

    private:
        
        HUNT::Model* model;

    };

}
