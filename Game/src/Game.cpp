#include <HUNTGame/Game.h>
#include <HUNT/Graphics/Renderer.h>
#include <algorithm>
#include <cmath>

namespace HUNTgame {

    DemoGame::DemoGame(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
        : red_(red), green_(green), blue_(blue) {
    }

    bool DemoGame::Start(HUNT::Engine& engine) {
        engine.GetRenderer().InitGPU();
        engine.GetRenderer().CreatePipeline();
        previousTime_ = currentTime_ = 0.0f;
        return true;
    }

    void DemoGame::FixedUpdate(HUNT::Engine&, float seconds) {
        previousTime_ = currentTime_;
        currentTime_ += seconds;
    }

    void DemoGame::Render(HUNT::Engine& engine) {
       
        
        const float time = previousTime_ + (currentTime_ - previousTime_);
        const int glow = static_cast<int>(16.0f * (1.0f + std::sin(time)));
        engine.GetRenderer().UpdateGPU();
        
       // engine.GetRenderer().Clear(red_, green_, static_cast<std::uint8_t>(std::min(255, int(blue_) + glow)));
       // engine.GetRenderer().drawRectangle();
    }

    void DemoGame::Stop(HUNT::Engine&) {}

}
