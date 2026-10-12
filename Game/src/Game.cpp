#include <HUNTGame/Game.h>
#include <HUNT/Graphics/Renderer.h>
#include <algorithm>
#include <cmath>

namespace HUNTgame {

    DemoGame::DemoGame(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
        : red_(red), green_(green), blue_(blue) {
    }

    bool DemoGame::Start(HUNT::Engine& engine) {
        auto& renderer = engine.GetRenderer();

        renderer.InitGPU();
        HUNT::MeshData data1;
        data1.vertices = std::vector<HUNT::Vertex>{
            HUNT::Vertex(HUNT::float3(-0.5f, 1.f, 0.f), HUNT::float4(1.f, 0.f, 0.f, 1.f)),
            HUNT::Vertex(HUNT::float3(-1.f, 0.5f, 0.f), HUNT::float4(0.f, 1.f, 0.f, 1.f)),
            HUNT::Vertex(HUNT::float3(0.f, 0.5f, 0.f), HUNT::float4(0.f, 0.f, 1.f, 1.f))
        };

        HUNT::MeshData data2;
        data2.vertices = std::vector<HUNT::Vertex>{
            HUNT::Vertex(HUNT::float3(0.5f, 0.f, 0.f), HUNT::float4(1.f, 0.f, 0.f, 1.f)),
            HUNT::Vertex(HUNT::float3(0.5f, -0.5f, 0.f), HUNT::float4(0.f, 1.f, 0.f, 1.f)),
            HUNT::Vertex(HUNT::float3(1.f, -0.5f, 0.f), HUNT::float4(0.f, 0.f, 1.f, 1.f))
        };

        triangle1.setMeshData(data1);
        triangle2.setMeshData(data2);
        previousTime_ = currentTime_ = 0.0f;
        return true;
    }

    void DemoGame::FixedUpdate(HUNT::Engine&, float seconds) {
        previousTime_ = currentTime_;
        currentTime_ += seconds;
    }

    void DemoGame::Render(HUNT::Engine& engine) {
        auto& renderer = engine.GetRenderer();
        
        renderer.BeginFrame();

        renderer.Render(triangle1);
        renderer.Render(triangle2);

        renderer.EndFrame();







        const float time = previousTime_ + (currentTime_ - previousTime_);
        const int glow = static_cast<int>(16.0f * (1.0f + std::sin(time)));
        

    }

    void DemoGame::Stop(HUNT::Engine&) {}

}
