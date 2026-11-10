#include <HUNTGame/MainScene.h>

#include <HUNT/Engine.h>
#include <HUNT/AssetHandler.h>
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Objects/Components/HUNTMeshComponent.h>
#include <HUNT/Objects/Components/HUNTModelComponent.h>
#include <HUNT/Math/mathFunctions.h>


void MainScene::Init(HUNT::AssetHandler& assets)
{

    HUNT::Camera* camera = AddCamera();

    camera->setPosition(0.f, 0.f, -10.f);
    camera->setRotation(0.f, 0.f, 0.f);

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

   //Test
    assets.LoadTextureToAssetHandler("TestAssets/bunny.png", "bunny");
    assets.LoadModelToAssetHandler("TestAssets/Goron.obj", "Goron");

    HUNT::Material bunnyMat;
    bunnyMat.SetTexture(assets.GetTexture("bunny"));
    assets.LoadMeshToAssetHandler(data1, bunnyMat, "Cube");


    //THIS IS ALL A TEST



    auto triangle = std::make_unique<HUNT::HUNTObject>();
    auto* meshComp = triangle->AddComponent<HUNT::HUNTMeshComponent>();
    meshComp->setMesh(assets.GetMesh("Cube"));
    objects.push_back(std::move(triangle));

    auto model = std::make_unique<HUNT::HUNTObject>();
    auto* modelComp = model->AddComponent<HUNT::HUNTModelComponent>();
    modelComp->setModel(assets.GetModel("Goron"));
    objects.push_back(std::move(model));




}

void MainScene::Update(HUNT::InputHandler& input,float deltaTime)
{
    float speed = 0.8f;
    float rotSpeed = 1.5f;

    HUNT::Camera* defaultCamera = GetActiveCamera();


    if (input.isHeld(HUNT::key::w)) {
        defaultCamera->moveForward(speed);
    }
    if (input.isHeld(HUNT::key::s)) {
        defaultCamera->moveBackward(speed);
    }
    if (input.isHeld(HUNT::key::a)) {
        defaultCamera->moveLeft(speed);
    }
    if (input.isHeld(HUNT::key::d)) {
        defaultCamera->moveRight(speed);
    }
    if (input.isHeld(HUNT::key::q)) {
        defaultCamera->rotate(-rotSpeed, 0.f);
    }
    if (input.isHeld(HUNT::key::e)) {
        defaultCamera->rotate(rotSpeed, 0.f);
    }

    if (input.isHeld(HUNT::key::space)) {
        defaultCamera->moveUp(speed);
    }
    if (input.isHeld(HUNT::key::left_ctrl)) {
        defaultCamera->moveUp(-speed);
    }

    if (input.wasPressed(HUNT::key::u)) {
        requestSceneChange("Tutorial");
    }


    if (input.isHeld(HUNT::mouse::rightMouse))
    {
        constexpr float sensitivity = 0.15f;
        //  engine.lockMouse(true);
         // engine.resetMousePosition();
        defaultCamera->rotate(input.getMouseXDifference() * sensitivity, -input.getMouseYDifference() * sensitivity);

    }
    else {
        // engine.lockMouse(false);
    }
}

void MainScene::FixedUpdate(HUNT::InputHandler& input)
{
   

}



void MainScene::Destroy()
{
}
