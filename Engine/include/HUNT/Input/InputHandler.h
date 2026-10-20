#pragma once

//https://wiki.libsdl.org/SDL3/BestKeyboardPractices
#include <HUNT/Input/Keys.h>
#include <array>
namespace HUNT
{
    class InputHandler {
    public:

        void update();
        bool isHeld(HUNT::Key key) const;
        bool wasPressed(HUNT::Key key) const;
        bool wasReleased(HUNT::Key key) const;

    private:

        //arrays of every current and previous key and if it was pressed
        std::array<bool, SDL_SCANCODE_COUNT> current{};
        std::array<bool, SDL_SCANCODE_COUNT> previous{};


    };
}