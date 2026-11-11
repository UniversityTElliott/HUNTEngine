#pragma once

#include <string>
#include <HUNT/Scene/Scene.h>

class SceneSerializer
{
public:
    static bool SaveScene(HUNT::Scene& scene, const std::string& filepath);
};