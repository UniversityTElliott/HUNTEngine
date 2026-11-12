#include "SceneSerializer.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "HUNT/Objects/HUNTObject.h"
#include "HUNT/Objects/Components/HUNTTransformComponent.h"
#include "HUNT/Objects/Components/HUNTMeshComponent.h"

///https://json.nlohmann.me/api/basic_json/ Had to fish thru this
//https://www.youtube.com/watch?v=Sa8bdVogGIo was useful too

using json = nlohmann::json;

bool SceneSerializer::SaveScene(HUNT::Scene& scene, const std::string& filepath)
{
    json sceneData;

    sceneData["sceneName"] = scene.GetName();
    sceneData["objects"] = json::array();

    for (const auto& objectPtr : scene.GetObjects())
    {
        if (!objectPtr) {
            continue;
        }

        auto* object = objectPtr.get();

        json objectData;

        objectData["id"] = object->GetID();
        //TRANSFORM
        auto* transform = object->GetComponent<HUNT::HUNTTransformComponent>();
        if (transform)
        {
            objectData["transform"]["position"] =
            {
                transform->position.x,
                transform->position.y,
                transform->position.z
            };

            objectData["transform"]["rotation"] =
            {
                transform->rotation.w,
                transform->rotation.x,
                transform->rotation.y,
                transform->rotation.z
            };

            objectData["transform"]["scale"] =
            {
                transform->scale.x,
                transform->scale.y,
                transform->scale.z
            };
        }
        //MESH
        auto* meshComponent =object->GetComponent<HUNT::HUNTMeshComponent>();
        if (meshComponent)
        {
            objectData["mesh"]["name"] =meshComponent->GetAssetName();
        }

        sceneData["objects"].push_back(objectData);
    }

    std::filesystem::path scenePath(filepath);

    //
    std::error_code error;
    std::filesystem::create_directories(scenePath.parent_path(), error);

    if (error) {
        return false;
    }

    std::ofstream file(scenePath);

    if (!file.is_open()) {
        return false;
    }

    file << sceneData.dump(4);

    return file.good();
}

bool SceneSerializer::LoadScene(HUNT::Scene& scene, const std::string& filepath)
{
    std::ifstream file(filepath);

    if (!file.is_open()) {
        return false;
    }
    json sceneData;

    try
    {
        file >> sceneData;
    }
    catch (const json::parse_error&)
    {
        return false;
    }

    if (!sceneData.contains("objects") ||
        !sceneData["objects"].is_array())
    {
        return false;
    }

    //CLEAR EXISTING OBJECTS
    scene.ClearObjects();

    for (const auto& objectData : sceneData["objects"])
    {
        auto* object = scene.CreateObject();

        //ID
        if (objectData.contains("id"))
        {
            object->SetID(objectData["id"].get<uint64_t>());
        }

        //TRANSFORM
        if (objectData.contains("transform"))
        {
            auto* transform =object->GetComponent<HUNT::HUNTTransformComponent>();

            const auto& transformData = objectData["transform"];

            if (transformData.contains("position"))
            {
                transform->position = HUNT::float3(
                    transformData["position"][0].get<float>(),
                    transformData["position"][1].get<float>(),
                    transformData["position"][2].get<float>()
                );
            }

            if (transformData.contains("rotation"))
            {
                transform->rotation = HUNT::quat(
                    transformData["rotation"][0].get<float>(),
                    transformData["rotation"][1].get<float>(),
                    transformData["rotation"][2].get<float>(),
                    transformData["rotation"][3].get<float>()
                );
            }

            if (transformData.contains("scale"))
            {
                transform->scale = HUNT::float3(
                    transformData["scale"][0].get<float>(),
                    transformData["scale"][1].get<float>(),
                    transformData["scale"][2].get<float>()
                );
            }
        }

        //MESH
        if (objectData.contains("mesh") &&objectData["mesh"].contains("name"))
        {
            std::string meshName =objectData["mesh"]["name"].get<std::string>();

            HUNT::Mesh* mesh = scene.GetSceneAssets()->GetMesh(meshName);

            if (mesh)
            {
                auto* meshComponent =object->AddComponent<HUNT::HUNTMeshComponent>();

                meshComponent->SetMesh(mesh);
            }
            else
            {
                std::cerr << "Failed to load mesh: "<< meshName << '\n';
            }
        }
    }

    return true;
}

