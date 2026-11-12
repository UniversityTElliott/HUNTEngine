#include <HUNT/Scene/Scene.h>
class AssetHandler;

class MainScene : public HUNT::Scene
{
public:
    MainScene() : HUNT::Scene("Main")
    {
    }

    void Init() override;
    void Update(HUNT::InputHandler& input,float deltaTime) override;
    void FixedUpdate(HUNT::InputHandler& input) override;
    void Destroy() override;

private:

};