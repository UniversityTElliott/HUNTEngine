
#pragma once

#include <cstdint>
#include <string>

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


        MeshData& GetMeshData()
        {
            return meshData;
        }

        void SetMeshData(const MeshData& data)
        {
            meshData = data;
        }

        uint32_t GetID() {
            return identifier;
        }


        void SetID(uint32_t idIn)
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

        void SetName(const std::string& nameIn)
        {
            meshName = nameIn;
        }
        const std::string& GetName() const
        {
            return meshName;
        }


    private:

       HUNT::MeshData meshData;
       std::string meshName;
       uint32_t identifier = 0;
       HUNT::Material meshMaterial; //change later idk if I want the mesh to own the material or the renderer

    };

}
