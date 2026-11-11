
#pragma once
#include <HUNT/Math/mat4.h>
namespace HUNT {
    class Renderer;

    class HUNTBaseComponent

    {
    public:
        HUNTBaseComponent() = default;
        ~HUNTBaseComponent() = default;

        virtual void Init() {

        };
        virtual void Update(float dt){

        };
        virtual void FixedUpdate(){

        };
        virtual void Render(Renderer& renderer, const HUNT::mat4& worldMatrix) {

        };

    private:
    
    
    };

}
