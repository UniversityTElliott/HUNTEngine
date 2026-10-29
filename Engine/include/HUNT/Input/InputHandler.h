#pragma once

//https://wiki.libsdl.org/SDL3/BestKeyboardPractices
#include <HUNT/Input/Keys.h>
#include <array>
#include <memory>
namespace HUNT
{
    class InputHandler {
    public:

        InputHandler();
        ~InputHandler();
        //Same as renderer
        InputHandler(const InputHandler&) = delete;
        InputHandler& operator=(const InputHandler&) = delete;

        void update();
        bool isHeld(HUNT::Input inputIn) const;
        bool wasPressed(HUNT::Input inputIn) const;
        bool wasReleased(HUNT::Input inputIn) const;

        float getMouseX();
        float getMouseY();

        float getMouseXDifference();
        float getMouseYDifference();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;



    };
}