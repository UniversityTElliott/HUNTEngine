
#pragma once

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
        virtual void Render(Renderer& renderer) {

        };

    private:
    
    
    };

}
