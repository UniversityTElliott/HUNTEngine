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
        bool isHeld(HUNT::Key key) const;
        bool wasPressed(HUNT::Key key) const;
        bool wasReleased(HUNT::Key key) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;



    };
}