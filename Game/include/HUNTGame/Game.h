#pragma once

#include <HUNT/Engine.h>
#include <HUNT/Graphics/HUNTMesh/Mesh.h>
#include <HUNT/HUNTObjects/HUNTObject.h>
#include <HUNT/HUNTObjects/HUNTComponents/HUNTMeshComponent.h>

#include <cstdint>

namespace HUNTgame {

    //Extends from the Engine's Application class.
    class DemoGame final : public HUNT::Application {
    public:
        DemoGame(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept;

        bool Start(HUNT::Engine& engine) override;
        void FixedUpdate(HUNT::Engine& engine, float seconds) override;
        void HandleInput(HUNT::Engine& engine) override;
        void Render(HUNT::Engine& engine) override;
        void Stop(HUNT::Engine& engine) override;

    private:
       
        HUNT::Mesh triangle1;
        HUNT::Mesh triangle2;

        HUNT::HUNTObject triangle3;

        std::uint8_t red_;
        std::uint8_t green_;
        std::uint8_t blue_;
        float previousTime_ = 0.0f;
        float currentTime_ = 0.0f;
    };

}
