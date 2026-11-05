
#pragma once

#include <cstdint>

#include <HUNT/Graphics/HUNTMesh/MeshData.h>
#include <HUNT/Graphics/HUNTMesh/Material.h>

namespace HUNT {

    class Mesh
    {
    public:
        Mesh() = default;
        explicit Mesh(const MeshData& data)
            : meshData(data)
        {
        }
        ~Mesh() = default;


        MeshData& getMeshData()
        {
            return meshData;
        }

        void setMeshData(const MeshData& data)
        {
            meshData = data;
        }

        void SetMaterial(HUNT::Material material)
        {
            meshMaterial = material;
        }
        HUNT::Material GetMaterial() 
        {
            return meshMaterial;
        }


    private:

       HUNT::MeshData meshData;
       HUNT::Material meshMaterial; //change later idk if I want the mesh to own the material or the renderer

    };

}
