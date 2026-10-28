
#pragma once
#include "HUNTBaseComponent.h"
#include <cstdint>

#include <HUNT/Graphics/HUNTMesh/MeshData.h>

#include <HUNT/Graphics/Renderer.h>

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
            for (auto& model : modelMeshes) {
                rendererIn.Render(model);
            }
            
        }


        void loadModel(const char* filePath)
        {
            Assimp::Importer importer;
            // | aiProcess_FlipUVs
            const aiScene* scene = importer.ReadFile(filePath,aiProcess_Triangulate |aiProcess_GenSmoothNormals |aiProcess_JoinIdenticalVertices);

            if (!scene) {
                return;
            }

            for (int i = 0; i < scene->mNumMeshes; ++i) {
                const aiMesh* mesh = scene->mMeshes[i];
                HUNT::MeshData modelMeshData;
                for ( int i = 0; i < mesh->mNumVertices; ++i)
                {
                    const aiVector3D& position = mesh->mVertices[i];

                    HUNT::Vertex vertex{};

                    vertex.position = HUNT::float3{position.x, position.y, position.z};

                    if (mesh->mTextureCoords[0])
                    {
                        const aiVector3D& uv = mesh->mTextureCoords[0][i];

                        vertex.UV = HUNT::float2{ uv.x, uv.y };
                    }

                    modelMeshData.vertices.push_back(vertex);
                }

                printf("%zu\n", modelMeshData.vertices.size());
                // Copy indices
                for ( int j = 0; j < mesh->mNumFaces; ++j)
                {
                    const aiFace& face = mesh->mFaces[j];

                    for (int j = 0; j < face.mNumIndices; ++j)
                    {
                        modelMeshData.indices.push_back(static_cast<uint32_t>(face.mIndices[j]));
                    }
                }
                HUNT::Mesh meshToLoad(modelMeshData);
                meshToLoad.setID(i+1);
                modelMeshes.push_back(meshToLoad);
               
            }
        }

    private:
        
        std::vector<HUNT::Mesh> modelMeshes;

    };

}
