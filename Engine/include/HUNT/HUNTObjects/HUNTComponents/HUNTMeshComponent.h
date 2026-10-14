
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
         //explicit HUNTMeshComponent(const MeshData& data)
         //    : meshData(data)
         //{
         //}
         ~HUNTMeshComponent() = default;
         void Init() final{

        };
         void Update(float dt) final {

        };
         void FixedUpdate() final {

        };
         void Render(Renderer& rendererIn) {
             rendererIn.Render(meshData);
         }



         MeshData& getMeshData()
         {
             return meshData;
         }

         void setMeshData(const MeshData& data)
         {
             meshData = data;
         }

    private:

        HUNT::MeshData meshData;
        //maybe add an index or soemthing?

    };

}
