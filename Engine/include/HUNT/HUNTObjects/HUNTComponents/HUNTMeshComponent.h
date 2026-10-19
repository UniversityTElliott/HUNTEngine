
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
         void Render(Renderer& rendererIn) {
             rendererIn.Render(mesh);
         }



         Mesh& getMesh()
         {
             return mesh;
         }

         void setMesh(const Mesh& data)
         {
             mesh = data;
         }

    private:

        HUNT::Mesh mesh;

    };

}
