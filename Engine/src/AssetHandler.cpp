#include "HUNT/AssetHandler.h"
#include <HUNT/Graphics/HUNTMesh/Material.h>
#include <HUNT/Graphics/HUNTMesh/Texture.h>
#include <HUNT/Graphics/HUNTMesh/Mesh.h>
#include <HUNT/Graphics/HUNTMesh/Model.h>
#include <HUNT/Graphics/Renderer.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace HUNT
{
    struct AssetHandler::Impl
    {
        Renderer* renderer;

        uint32_t uniqueMeshIDs = 0;

        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::unique_ptr<Material>> materials;
        std::vector<std::unique_ptr<Model>> models; //make the model class




        //MAKE THIS A PAIR?

        std::unordered_map<std::string, uint32_t> textureLookup;
        std::unordered_map<std::string, uint32_t> texturePathLookup;
        std::unordered_map<std::string, uint32_t> meshLookup;
        std::unordered_map<std::string, uint32_t> materialLookup;
        std::unordered_map<std::string, uint32_t> modelLookup;


        explicit Impl(Renderer* renderer)
            : renderer(renderer)
        {
        }
    };

    AssetHandler::AssetHandler(Renderer* renderer)
        : impl_(std::make_unique<Impl>(renderer))
    {
    }



    AssetHandler::~AssetHandler() = default;





    void AssetHandler::LoadTextureToAssetHandler(const char* path, std::string name)
    {

        // Check if this logical asset name already exis
        auto nameCheck = impl_->textureLookup.find(name);

        if (nameCheck != impl_->textureLookup.end())
        {
            return;
        }

        //Check if this file has already been loaded
        uint32_t textureID;
        auto pathCheck = impl_->texturePathLookup.find(path);
        if (pathCheck != impl_->texturePathLookup.end())
        {
            // Reuse the existing texture
            textureID = pathCheck->second;
        }
        else
        {
            // Load the texture through the Renderer
            textureID = impl_->renderer->LoadTexture(path);

            // Check for load failure
            if (textureID == -1)
            {
                return;
            }

            // Store the file path lookup
            impl_->texturePathLookup.emplace(path, textureID);
        }

        // Store ID
        impl_->textureLookup.emplace(std::move(name), textureID);
    }
    void AssetHandler::LoadModelToAssetHandler(const char* path, std::string name)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

        if (!scene) {
            return;
        }

        std::vector<HUNT::Mesh>tempModelMesh;
        std::string modelPath = "TestAssets/";
        //FOR EACH MESH THAT EXISTS
        for (int i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh* mesh = scene->mMeshes[i];
            HUNT::MeshData modelMeshData;
            //FILL VERTEX DATA
            for (int i = 0; i < mesh->mNumVertices; ++i)
            {
                const aiVector3D& position = mesh->mVertices[i];

                HUNT::Vertex vertex{};

                vertex.position = HUNT::float3{ position.x, position.y, position.z };

                if (mesh->mTextureCoords[0])
                {
                    const aiVector3D& uv = mesh->mTextureCoords[0][i];

                    vertex.UV = HUNT::float2{ uv.x, uv.y };
                }

                modelMeshData.vertices.push_back(vertex);
            }

            printf("%zu\n", modelMeshData.vertices.size());
            //FILL INDEX DATA AND FLIP IT
            //TODO:: REMOVE AS THERE IS A WAY WIOTH ASSIMP TO FLIP
            for (int j = 0; j < mesh->mNumFaces; ++j)
            {
                const aiFace& face = mesh->mFaces[j];

                for (int k = 0; k < face.mNumIndices; k += 3)
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
            if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
            {
                std::string fullPath = modelPath + texturePath.C_Str();

                LoadTextureToAssetHandler(fullPath.c_str(), name);
                HUNT::Texture* texture = GetTexture(name);
                material.SetTexture(texture);
            }


            HUNT::Mesh meshToLoad(modelMeshData);
            meshToLoad.setID(impl_->uniqueMeshIDs++);
            meshToLoad.setMaterial(material);
            tempModelMesh.push_back(meshToLoad);

        }

        //THIS IS SAME AS OTHER FUNCTIONS
        auto modelID = impl_->modelLookup.find(name);
        if (modelID != impl_->modelLookup.end())
        {
            return;
        }

        uint32_t index = impl_->models.size();
        HUNT::Model modelToStore(tempModelMesh);
        impl_->models.push_back(std::make_unique<HUNT::Model>(std::move(modelToStore)));
        impl_->modelLookup.emplace(std::move(name), index);

    }


    void AssetHandler::LoadMeshToAssetHandler(HUNT::MeshData meshDataIn, HUNT::Material materialIn, std::string name)
    {
        auto meshID = impl_->meshLookup.find(name);

        if (meshID != impl_->meshLookup.end())
        {
            return;
        }

        HUNT::Mesh newMesh;
        newMesh.setID(impl_->uniqueMeshIDs++);
        newMesh.setMaterial(materialIn);
        newMesh.setMeshData(meshDataIn);
        
        //As models have meshes and they can exist by themself they need shared unique IDS but I want to keep the modelMeshes away from this vector therefore this solution
        uint32_t index = impl_->meshes.size();

        impl_->meshes.push_back(std::make_unique<HUNT::Mesh>(std::move(newMesh)));
        impl_->meshLookup.emplace(std::move(name), index);
    }


    Texture* AssetHandler::GetTexture(const std::string& name)
    {
        //Check for the ID
        auto texID = impl_->textureLookup.find(name);
        //Fail if it doesnt exist
        if (texID == impl_->textureLookup.end())
        {
            return nullptr;
        }
        //Search it in the vector (oh so contiguous)
        return impl_->renderer->GetLoadedTexture(texID->second);
    }

    Model* AssetHandler::GetModel(const std::string& name)
    {
        //Check for the ID
        auto modelID = impl_->modelLookup.find(name);
        //Fail if it doesnt exist
        if (modelID == impl_->modelLookup.end())
        {
            return nullptr;
        }
        //Search it in the vector (oh so contiguous)
        return impl_->models.at(modelID->second).get();
    }

    Mesh* AssetHandler::GetMesh(const std::string& name)
    {
        //Check for the ID
        auto meshID = impl_->meshLookup.find(name);
        //Fail if it doesnt exist
        if (meshID == impl_->meshLookup.end())
        {
            return nullptr;
        }
        //Search it in the vector (oh so contiguous)
        return impl_->meshes.at(meshID->second).get();
    }


}
