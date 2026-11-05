
#pragma once

#include <cstdint>
#include <vector>
#include <HUNT/Graphics/HUNTMesh/Mesh.h>


namespace HUNT {

    class Model
    {
    public:
        Model() = default;
        explicit Model(std::vector<Mesh> meshesIn)
            : modelMeshes(std::move(meshesIn))
        {
        }
        ~Model() = default;
        std::vector<HUNT::Mesh>& getModelMeshes()
        {
            return modelMeshes;
        }


    private:

        
        std::vector<HUNT::Mesh> modelMeshes;

    };

}
