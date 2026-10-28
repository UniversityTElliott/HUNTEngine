

#include <HUNTGame/Game.h>
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Input/InputHandler.h>
#include <HUNT/HUNTMath/mathFunctions.h>
#include <algorithm>
#include <cmath>
#include <SDL3/SDL_log.h> //FOR TESTING
namespace HUNTgame {

    DemoGame::DemoGame(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
        : red_(red), green_(green), blue_(blue) {
    }

    bool DemoGame::Start(HUNT::Engine& engine) {
        auto& renderer = engine.GetRenderer();

        renderer.InitGPU();

        renderer.LoadTexture("TestAssets/bunny.png");



        HUNT::MeshData data1;
        //CUBE!
        data1.vertices = std::vector<HUNT::Vertex>{
            // Back face (-Z)
            HUNT::Vertex(HUNT::float3(-0.5f,  0.5f, -0.5f), HUNT::float4(1.f, 0.f, 0.f, 1.f), HUNT::float2(0.f, 0.f)),
            HUNT::Vertex(HUNT::float3(-0.5f, -0.5f, -0.5f), HUNT::float4(0.f, 1.f, 0.f, 1.f), HUNT::float2(0.f, 1.f)),
            HUNT::Vertex(HUNT::float3(0.5f, -0.5f, -0.5f), HUNT::float4(0.f, 0.f, 1.f, 1.f), HUNT::float2(1.f, 1.f)),
            HUNT::Vertex(HUNT::float3(0.5f,  0.5f, -0.5f), HUNT::float4(1.f, 0.f, 1.f, 1.f), HUNT::float2(1.f, 0.f)),

            // Front face (+Z)
            HUNT::Vertex(HUNT::float3(-0.5f,  0.5f,  0.5f), HUNT::float4(1.f, 1.f, 0.f, 1.f), HUNT::float2(0.f, 0.f)),
            HUNT::Vertex(HUNT::float3(-0.5f, -0.5f,  0.5f), HUNT::float4(0.f, 1.f, 1.f, 1.f), HUNT::float2(0.f, 1.f)),
            HUNT::Vertex(HUNT::float3(0.5f, -0.5f,  0.5f), HUNT::float4(1.f, 0.5f, 0.f, 1.f), HUNT::float2(1.f, 1.f)),
            HUNT::Vertex(HUNT::float3(0.5f,  0.5f,  0.5f), HUNT::float4(1.f, 1.f, 1.f, 1.f), HUNT::float2(1.f, 0.f))
        };

        // Counter-clockwise order like CMP203
        data1.indices = std::vector<uint32_t>{
            0, 1, 2, 0, 2, 3,
            4, 6, 5, 4, 7, 6,
            4, 5, 1, 4, 1, 0,
            3, 2, 6, 3, 6, 7,
            4, 0, 3, 4, 3, 7,
            1, 5, 6, 1, 6, 2
        };

    
        //ADD SOMETHING THAT LETS YOU LOAD ALL THE TEXTURES BEFOREHAND AND JUST GRAB THEM BY A NAME??? maybe

         

       HUNT::Material bunnyMat;
      // bunnyMat.SetTexture(renderer.GetLoadedTexture("TestAssets/bunny.png"));

       //Set it all for the example
       // triangle1.setMeshData(data1);
//triangle1.setID(0);
      //  triangle1.SetMaterial(bunnyMat);


       //auto* meshComp =  triangle3.AddComponent<HUNT::HUNTMeshComponent>();

      // meshComp->setMesh(triangle1);

       //THIS IS ALL A TEST
       auto* modelComp = modelTest.AddComponent<HUNT::HUNTModelComponent>();
       modelComp->loadModel("TestAssets/vegeta.obj", renderer); //CHANGE THIS
      
       myCamera.setPosition(0.f, 0.f, -10.f);
       myCamera.setRotation(0.f, 0.f, 0.f);

        previousTime_ = currentTime_ = 0.0f;
        return true;
    }
  
    //Make a handleInput function
    //cycle anything that has a handeinput component and call it
    void DemoGame::FixedUpdate(HUNT::Engine& engine, float seconds) {
        previousTime_ = currentTime_;
        currentTime_ += seconds;
        float speed = 0.2f;
        float rotSpeed = 1.5f;
        
        auto& input = engine.GetInput();

        input.update();

        //Position
      
        if (input.isHeld(HUNT::key::w)) {
            myCamera.moveForward(speed);
        }
        if (input.isHeld(HUNT::key::s)) {
            myCamera.moveBackward(speed);
        }
        if (input.isHeld(HUNT::key::a)) {
            myCamera.moveLeft(speed);
        }
        if (input.isHeld(HUNT::key::d)) {
            myCamera.moveRight(speed);
        }
        if (input.isHeld(HUNT::key::q)) {
            myCamera.rotate(-rotSpeed,0.f);
        }
        if (input.isHeld(HUNT::key::e)) {
            myCamera.rotate(rotSpeed, 0.f);
        }

        if (input.isHeld(HUNT::key::space)) {
            myCamera.moveUp(speed);
        }
        if (input.isHeld(HUNT::key::left_ctrl)) {
            myCamera.moveUp(-speed);
        }

        //Rotate

    }

    void DemoGame::HandleInput(HUNT::Engine& engine)
    {
      
    }

    void DemoGame::Render(HUNT::Engine& engine) {
        auto& renderer = engine.GetRenderer();
        
        HUNT::MatrixBufferType matrices = HUNT::makeGPUMatrix(HUNT::mat4(1.f), myCamera.getViewMatrix(), HUNT::perspectiveLH(HUNT::toRadians(90.f), 1080.f / 720.f, 0.1f, 200.f));

        renderer.SetCameraMatrix(matrices);

        renderer.BeginFrame();
        
     //   triangle3.GetComponent<HUNT::HUNTMeshComponent>()->Render(renderer);
        modelTest.GetComponent<HUNT::HUNTModelComponent>()->Render(renderer);




        renderer.EndFrame();







        const float time = previousTime_ + (currentTime_ - previousTime_);
        const int glow = static_cast<int>(16.0f * (1.0f + std::sin(time)));
        

    }

    void DemoGame::Stop(HUNT::Engine&) {}

}
