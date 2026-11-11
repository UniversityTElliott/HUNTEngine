
#pragma once
#include "HUNTBaseComponent.h"
#include <cstdint>
#include <filesystem>
#include <HUNT/Graphics/HUNTMesh/Model.h>

#include <HUNT/Graphics/Renderer.h>
#include <HUNT/AssetHandler.h>



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
        void Render(Renderer& rendererIn, const HUNT::mat4& worldMatrix) final {
            if (model == nullptr) {
                return;
            }

            for (auto& mesh : model->getModelMeshes()) {
                rendererIn.Render(mesh, worldMatrix);
            }
        }

        void setModel(Model* data)
        {
            model = data;
        }
        

    private:
        
        HUNT::Model* model = nullptr;

    };

}
