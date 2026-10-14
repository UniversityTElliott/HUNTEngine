
#pragma once

namespace HUNT {

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

    private:
    
    
    };

}
