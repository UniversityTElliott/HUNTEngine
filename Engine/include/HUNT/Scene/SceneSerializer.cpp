#include "SceneSerializer.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "HUNT/Objects/HUNTObject.h"
#include "HUNT/Objects/Components/HUNTTransformComponent.h"

///https://json.nlohmann.me/api/basic_json/ Had to fish thru this

using json = nlohmann::json;

bool SceneSerializer::SaveScene(HUNT::Scene& scene, const std::string& filepath)
{
    json sceneData;

    sceneData["sceneName"] = scene.GetName();
    sceneData["objects"] = json::array();

    for (const auto& objectPtr : scene.GetObjects())
    {
        if (!objectPtr)
            continue;

        auto* object = objectPtr.get();

        json objectData;

        objectData["id"] = object->GetID();

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

        sceneData["objects"].push_back(objectData);
    }

    std::filesystem::path scenePath(filepath);

    //
    std::error_code error;
    std::filesystem::create_directories(scenePath.parent_path(), error);

    if (error)
        return false;

    std::ofstream file(scenePath);

    if (!file.is_open())
        return false;

    file << sceneData.dump(4);

    return file.good();
}
