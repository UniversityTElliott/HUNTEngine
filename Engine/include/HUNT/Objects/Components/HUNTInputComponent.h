
#pragma once
#include "HUNTBaseComponent.h"

namespace HUNT {

    class HUNTInputComponent : public HUNTBaseComponent
    {
    public:
        HUNTInputComponent() = default;

        ~HUNTInputComponent() = default;
        void Init() final {

        };
        void Update(float dt) final {

        };
        void FixedUpdate() final {

        };
        void Render(Renderer& rendererIn) {
            rendererIn.Render(meshData);
        }
        void handleInput() {

        };



    private:

    };

}
