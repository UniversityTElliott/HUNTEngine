#include <HUNT/Scene/Scene.h>
class AssetHandler;

class TutorialScene : public HUNT::Scene
{
public:
    TutorialScene() : HUNT::Scene("Tutorial")
    {
    }

    void Init() override;
    void Update(HUNT::InputHandler& input,float deltaTime) override;
    void FixedUpdate(HUNT::InputHandler& input) override;
    void Destroy() override;

private:

};