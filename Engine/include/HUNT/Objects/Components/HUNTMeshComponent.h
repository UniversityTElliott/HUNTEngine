
#pragma once
#include "HUNTBaseComponent.h"
#include <cstdint>

#include <HUNT/Graphics/HUNTMesh/MeshData.h>

#include <HUNT/Graphics/Renderer.h>

namespace HUNT {

    class HUNTMeshComponent : public HUNTBaseComponent
    {
    public:
        HUNTMeshComponent() = default;
   
         ~HUNTMeshComponent() = default;
         void Init() final{

        };
         void Update(float dt) final {

        };
         void FixedUpdate() final {

        };
         void Render(Renderer& rendererIn, const HUNT::mat4& worldMatrix) final{
             rendererIn.Render(*mesh,worldMatrix);
         }



         Mesh& getMesh()
         {
             return *mesh;
         }

         void setMesh(Mesh* data)
         {
             mesh = data;
         }

    private:

        HUNT::Mesh* mesh = nullptr;

    };

}
