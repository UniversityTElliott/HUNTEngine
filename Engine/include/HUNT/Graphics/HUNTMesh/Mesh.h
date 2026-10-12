
#pragma once

#include <cstdint>

#include <HUNT/Graphics/HUNTMesh/MeshData.h>

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

    private:

       HUNT::MeshData meshData;
       //maybe add an index or soemthing?

    };

}
