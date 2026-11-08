#pragma once
#include "HUNT/Objects/HUNTObject.h"
#include <string>
#include <memory>
#include <vector>

namespace HUNT
{
    class SceneObject;

    class Scene
    {
    public:
        explicit Scene(const std::string& name)
            : m_Name(name)
        {
        }

        virtual ~Scene() = default;

        //All parts of the scene
        virtual void Init() {}
        virtual void Update(float deltaTime) {}
        virtual void FixedUpdate() {}
        virtual void Render() {}
        virtual void Destroy() {}

        //for switching
        virtual void OnEnter() {}
        virtual void OnExit() {}

        const std::string& GetName() const
        {
            return m_Name;
        }
    protected:
        std::vector<std::unique_ptr<HUNTObject>> objects;
    private:
        std::string m_Name;
    };
}