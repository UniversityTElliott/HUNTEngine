#include <HUNTGame/TutorialScene.h>

#include <HUNT/Engine.h>
#include <HUNT/AssetHandler.h>
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Objects/Components/HUNTMeshComponent.h>
#include <HUNT/Objects/Components/HUNTModelComponent.h>
#include <HUNT/Math/mathFunctions.h>


void TutorialScene::Init(HUNT::AssetHandler& assets)
{

    HUNT::Camera* camera = AddCamera();

    camera->setPosition(0.f, 0.f, -10.f);
    camera->setRotation(0.f, 0.f, 0.f);

  
    assets.LoadTextureToAssetHandler("TestAssets/bunny.png", "bunny");
    assets.LoadModelToAssetHandler("TestAssets/Goron.obj", "Goron");

    auto model = std::make_unique<HUNT::HUNTObject>();
    auto* modelComp = model->AddComponent<HUNT::HUNTModelComponent>();
    modelComp->setModel(assets.GetModel("Goron"));
    objects.push_back(std::move(model));




}

void TutorialScene::Update(HUNT::InputHandler& input,float deltaTime)
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
        requestSceneChange("Main");
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

void TutorialScene::FixedUpdate(HUNT::InputHandler& input)
{
   
}



void TutorialScene::Destroy()
{
}
