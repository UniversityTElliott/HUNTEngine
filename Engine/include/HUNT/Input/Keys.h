#pragma once
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_mouse.h>
//UNSURE IF I SHOULD KEEP THIS! DOES IT MATTER ASK GAZ
//Feels shit but might be apprecired idk
namespace HUNT
{

    enum class InputType {
        Keyboard,
        Mouse,
        Controller
    };

    struct Input {
        InputType type;
        int idCode;
    };

    using Key = SDL_Scancode;

    namespace key {
        // Letters
// Letters
        constexpr Input a{ InputType::Keyboard, SDL_SCANCODE_A };
        constexpr Input b{ InputType::Keyboard, SDL_SCANCODE_B };
        constexpr Input c{ InputType::Keyboard, SDL_SCANCODE_C };
        constexpr Input d{ InputType::Keyboard, SDL_SCANCODE_D };
        constexpr Input e{ InputType::Keyboard, SDL_SCANCODE_E };
        constexpr Input f{ InputType::Keyboard, SDL_SCANCODE_F };
        constexpr Input g{ InputType::Keyboard, SDL_SCANCODE_G };
        constexpr Input h{ InputType::Keyboard, SDL_SCANCODE_H };
        constexpr Input i{ InputType::Keyboard, SDL_SCANCODE_I };
        constexpr Input j{ InputType::Keyboard, SDL_SCANCODE_J };
        constexpr Input k{ InputType::Keyboard, SDL_SCANCODE_K };
        constexpr Input l{ InputType::Keyboard, SDL_SCANCODE_L };
        constexpr Input m{ InputType::Keyboard, SDL_SCANCODE_M };
        constexpr Input n{ InputType::Keyboard, SDL_SCANCODE_N };
        constexpr Input o{ InputType::Keyboard, SDL_SCANCODE_O };
        constexpr Input p{ InputType::Keyboard, SDL_SCANCODE_P };
        constexpr Input q{ InputType::Keyboard, SDL_SCANCODE_Q };
        constexpr Input r{ InputType::Keyboard, SDL_SCANCODE_R };
        constexpr Input s{ InputType::Keyboard, SDL_SCANCODE_S };
        constexpr Input t{ InputType::Keyboard, SDL_SCANCODE_T };
        constexpr Input u{ InputType::Keyboard, SDL_SCANCODE_U };
        constexpr Input v{ InputType::Keyboard, SDL_SCANCODE_V };
        constexpr Input w{ InputType::Keyboard, SDL_SCANCODE_W };
        constexpr Input x{ InputType::Keyboard, SDL_SCANCODE_X };
        constexpr Input y{ InputType::Keyboard, SDL_SCANCODE_Y };
        constexpr Input z{ InputType::Keyboard, SDL_SCANCODE_Z };

        // Common controls
        constexpr Input space{ InputType::Keyboard, SDL_SCANCODE_SPACE };
        constexpr Input enter{ InputType::Keyboard, SDL_SCANCODE_RETURN };
        constexpr Input escape{ InputType::Keyboard, SDL_SCANCODE_ESCAPE };
        constexpr Input tab{ InputType::Keyboard, SDL_SCANCODE_TAB };
        constexpr Input backspace{ InputType::Keyboard, SDL_SCANCODE_BACKSPACE };
        constexpr Input delete_key{ InputType::Keyboard, SDL_SCANCODE_DELETE };
        constexpr Input insert{ InputType::Keyboard, SDL_SCANCODE_INSERT };

        // Modifiers
        constexpr Input left_shift{ InputType::Keyboard, SDL_SCANCODE_LSHIFT };
        constexpr Input right_shift{ InputType::Keyboard, SDL_SCANCODE_RSHIFT };
        constexpr Input left_ctrl{ InputType::Keyboard, SDL_SCANCODE_LCTRL };
        constexpr Input right_ctrl{ InputType::Keyboard, SDL_SCANCODE_RCTRL };
        constexpr Input left_alt{ InputType::Keyboard, SDL_SCANCODE_LALT };
        constexpr Input right_alt{ InputType::Keyboard, SDL_SCANCODE_RALT };
        constexpr Input caps_lock{ InputType::Keyboard, SDL_SCANCODE_CAPSLOCK };

        // Arrow keys
        constexpr Input up{ InputType::Keyboard, SDL_SCANCODE_UP };
        constexpr Input down{ InputType::Keyboard, SDL_SCANCODE_DOWN };
        constexpr Input left{ InputType::Keyboard, SDL_SCANCODE_LEFT };
        constexpr Input right{ InputType::Keyboard, SDL_SCANCODE_RIGHT };


    }
    
   
}
namespace HUNT::mouse{
    
 constexpr Input leftMouse{InputType::Mouse, SDL_BUTTON_LMASK};
 constexpr Input rightMouse{ InputType::Mouse, SDL_BUTTON_RMASK };
 constexpr Input middleMouse{ InputType::Mouse, SDL_BUTTON_MMASK };
}
namespace HUNT::controller
{
    
}
//Taken from CMP301 example proj for template
/*
    if (input->isMouseActive())
    {
        // mouse look is on
        deltax = input->getMouseX() - (winWidth / 2);
        deltay = input->getMouseY() - (winHeight / 2);
        turn(deltax, deltay);
        cursor.x = winWidth / 2;
        cursor.y = winHeight / 2;
        ClientToScreen(wnd, &cursor);
        SetCursorPos(cursor.x, cursor.y);
    }

    if (input->isRightMouseDown() && !input->isMouseActive())
    {
        // re-position cursor
        cursor.x = winWidth / 2;
        cursor.y = winHeight / 2;
        ClientToScreen(wnd, &cursor);
        SetCursorPos(cursor.x, cursor.y);
        input->setMouseX(winWidth / 2);
        input->setMouseY(winHeight / 2);

        // set mouse tracking as active and hide mouse cursor
        input->setMouseActive(true);
        ShowCursor(false);
    }
    else if (!input->isRightMouseDown() && input->isMouseActive())
    {
        // disable mouse tracking and show mouse cursor
        input->setMouseActive(false);
        ShowCursor(true);
    }



*/