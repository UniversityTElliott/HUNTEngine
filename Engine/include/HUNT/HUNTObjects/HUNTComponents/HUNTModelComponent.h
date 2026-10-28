
#pragma once
#include "HUNTBaseComponent.h"
#include <cstdint>
#include <filesystem>
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


        void loadModel(const char* filePath, Renderer& rendererIn)
        {

            //I FUCKING HATE THIS IMPLEMENTATION IM SO TIRED

            Assimp::Importer importer;
            
            const aiScene* scene = importer.ReadFile(filePath,aiProcess_Triangulate |aiProcess_GenSmoothNormals |aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

            if (!scene) {
                return;
            }
            std::string modelPath = "TestAssets/";
            //FOR EACH MESH THAT EXISTS
            for (int i = 0; i < scene->mNumMeshes; ++i) {
                const aiMesh* mesh = scene->mMeshes[i];
                HUNT::MeshData modelMeshData;
                //FILL VERTEX DATA
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
                //FILL INDEX DATA AND FLIP IT
                for ( int j = 0; j < mesh->mNumFaces; ++j)
                {
                    const aiFace& face = mesh->mFaces[j];

                    for ( int k = 0; k < face.mNumIndices; k += 3)
                    {
                        modelMeshData.indices.push_back(face.mIndices[k]);
                        modelMeshData.indices.push_back(face.mIndices[k + 2]);
                        modelMeshData.indices.push_back(face.mIndices[k + 1]);
                    }
                }
                HUNT::Material material;
                aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
                aiString texturePath;
                //load and add texture
                if (aiMat->GetTexture(aiTextureType_DIFFUSE,0,&texturePath) == AI_SUCCESS)
                {
                    std::string fullPath =modelPath + texturePath.C_Str();

                    rendererIn.LoadTexture(fullPath.c_str());
                    HUNT::Texture* texture = rendererIn.GetLoadedTexture(fullPath.c_str());
                    material.SetTexture(texture);
                }


                HUNT::Mesh meshToLoad(modelMeshData);
                meshToLoad.setID(i);
                meshToLoad.SetMaterial(material);
                modelMeshes.push_back(meshToLoad);
               

                //TODO:: Proper asset manager that means this doesnt need renderer can just use "LoadTexture"
                //THAT SHOULD ALSO AUTO ASSIGN IDS CUZ ITS DUMB MANUALLY SETTING THEM
                //ADD DEFAULT MATERAL AND TEXTURE ALSO !!!!
                //Renderer doesnt load in the right order add Z buffer things
            }
        }

    private:
        
        std::vector<HUNT::Mesh> modelMeshes;

    };

}
