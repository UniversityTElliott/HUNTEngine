
#pragma once
#include<vector>
#include <memory>
#include <HUNT/Objects/Components/HUNTBaseComponent.h>
#include <HUNT/Objects/Components/HUNTTransformComponent.h>
#include <HUNT/Math/mathFunctions.h>
#include <HUNT/Math/mat4.h>
namespace HUNT {
    class Renderer;

    class HUNTObject
    {
    public:
        HUNTObject() : objectID(nextObjectID++) {
            AddComponent<HUNTTransformComponent>();
            
        }
        ~HUNTObject() = default;

        void Init() {
            for (auto& component : objectComponents) {
                component->Init();
            }
        };
        void Update(float dt){
                        for (auto& component : objectComponents) {
                component->Update(dt);
            }
        };
        void FixedUpdate(){
            for (auto& component : objectComponents) {
                component->FixedUpdate();
            }
        };
        void Render(Renderer& renderer) {
            auto* transform = GetComponent<HUNTTransformComponent>();

            if (!transform) {
                return;
            }

            HUNT::mat4 worldMatrix = HUNT::calculateWorldMatrix(transform->position,transform->rotation,transform->scale);

            for (auto& component : objectComponents) {
                component->Render(renderer, worldMatrix);
            }
        }

        uint64_t GetID() const {
            return objectID;
        }


        template<class T> inline T* AddComponent()
        {
            std::unique_ptr<T> componentPointer = std::make_unique<T>();
            T* pointer = componentPointer.get();
            objectComponents.push_back(std::move(componentPointer));
            return pointer;
        }

        
        template<class T> T* GetComponent() noexcept{
            static_assert(std::is_base_of_v<HUNTBaseComponent, T>);
            for (const auto& component : objectComponents) {
                if (auto* match = dynamic_cast<T*>(component.get()))
                    return match;
            }
            return nullptr;
        }

        

    private:

        //https://stackoverflow.com/questions/14585385/best-practice-how-to-get-a-unique-identifier-for-the-object Not sure if good practice but works for now
        inline static uint64_t  nextObjectID = 1;
        uint64_t objectID;
        std::vector<std::unique_ptr<HUNTBaseComponent>> objectComponents;

    };

}
