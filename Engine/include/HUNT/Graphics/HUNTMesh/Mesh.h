
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

        uint32_t& getID() {
            return identifier;
        }


        void setID(const uint32_t& idIn)
        {
                identifier = idIn;
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
       uint32_t identifier;
       HUNT::Material meshMaterial; //change later idk if I want the mesh to own the material or the renderer

    };

}
