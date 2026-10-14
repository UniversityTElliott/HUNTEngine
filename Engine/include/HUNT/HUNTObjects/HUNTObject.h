
#pragma once
#include<vector>
#include <memory>
#include <HUNT/HUNTObjects/HUNTComponents/HUNTBaseComponent.h>
namespace HUNT {

    class HUNTObject
    {
    public:
        HUNTObject() = default;
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
        std::vector<std::unique_ptr<HUNTBaseComponent>> objectComponents;

    };

}
