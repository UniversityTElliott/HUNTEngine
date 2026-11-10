

#include <HUNTGame/Game.h>
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Input/InputHandler.h>

#include <HUNT/SceneHandler.h>

#include <HUNTGame/MainScene.h>
#include <HUNTGame/TutorialScene.h>
#include <HUNT/AssetHandler.h>

#include <algorithm>
#include <cmath>
#include <SDL3/SDL_log.h> //FOR TESTING

namespace HUNTgame {

    DemoGame::DemoGame(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
        : red_(red), green_(green), blue_(blue) {
    }

    bool DemoGame::Start(HUNT::Engine& engine) {
        //Makes and sets main scene
        engine.GetSceneHandler().CreateScene<MainScene>();
        engine.GetSceneHandler().CreateScene<TutorialScene>();

        return true;
    }
  
 
    void DemoGame::FixedUpdate(HUNT::Engine& engine, float seconds) {

    }

    void DemoGame::HandleInput(HUNT::Engine& engine)
    {
      
    }

    void DemoGame::Render(HUNT::Engine& engine) {
        
    }

    void DemoGame::Stop(HUNT::Engine&) {}

}
//TODO NEXT
//IMGUI
//JOLT
//OTHER PROJECTS