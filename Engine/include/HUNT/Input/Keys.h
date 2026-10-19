#pragma once
#include <SDL3/SDL_scancode.h>
//UNSURE IF I SHOULD KEEP THIS! DOES IT MATTER ASK GAZ
//Feels shit but might be apprecired idk
namespace HUNT
{
    using Key = SDL_Scancode;

    namespace key {
        // Letters
        constexpr SDL_Scancode a = SDL_SCANCODE_A;
        constexpr SDL_Scancode b = SDL_SCANCODE_B;
        constexpr SDL_Scancode c = SDL_SCANCODE_C;
        constexpr SDL_Scancode d = SDL_SCANCODE_D;
        constexpr SDL_Scancode e = SDL_SCANCODE_E;
        constexpr SDL_Scancode f = SDL_SCANCODE_F;
        constexpr SDL_Scancode g = SDL_SCANCODE_G;
        constexpr SDL_Scancode h = SDL_SCANCODE_H;
        constexpr SDL_Scancode i = SDL_SCANCODE_I;
        constexpr SDL_Scancode j = SDL_SCANCODE_J;
        constexpr SDL_Scancode k = SDL_SCANCODE_K;
        constexpr SDL_Scancode l = SDL_SCANCODE_L;
        constexpr SDL_Scancode m = SDL_SCANCODE_M;
        constexpr SDL_Scancode n = SDL_SCANCODE_N;
        constexpr SDL_Scancode o = SDL_SCANCODE_O;
        constexpr SDL_Scancode p = SDL_SCANCODE_P;
        constexpr SDL_Scancode q = SDL_SCANCODE_Q;
        constexpr SDL_Scancode r = SDL_SCANCODE_R;
        constexpr SDL_Scancode s = SDL_SCANCODE_S;
        constexpr SDL_Scancode t = SDL_SCANCODE_T;
        constexpr SDL_Scancode u = SDL_SCANCODE_U;
        constexpr SDL_Scancode v = SDL_SCANCODE_V;
        constexpr SDL_Scancode w = SDL_SCANCODE_W;
        constexpr SDL_Scancode x = SDL_SCANCODE_X;
        constexpr SDL_Scancode y = SDL_SCANCODE_Y;
        constexpr SDL_Scancode z = SDL_SCANCODE_Z;

        // Common controls
        constexpr SDL_Scancode space = SDL_SCANCODE_SPACE;
        constexpr SDL_Scancode enter = SDL_SCANCODE_RETURN;
        constexpr SDL_Scancode escape = SDL_SCANCODE_ESCAPE;
        constexpr SDL_Scancode tab = SDL_SCANCODE_TAB;
        constexpr SDL_Scancode backspace = SDL_SCANCODE_BACKSPACE;
        constexpr SDL_Scancode delete_key = SDL_SCANCODE_DELETE;
        constexpr SDL_Scancode insert = SDL_SCANCODE_INSERT;

        // Modifiers
        constexpr SDL_Scancode left_shift = SDL_SCANCODE_LSHIFT;
        constexpr SDL_Scancode right_shift = SDL_SCANCODE_RSHIFT;
        constexpr SDL_Scancode left_ctrl = SDL_SCANCODE_LCTRL;
        constexpr SDL_Scancode right_ctrl = SDL_SCANCODE_RCTRL;
        constexpr SDL_Scancode left_alt = SDL_SCANCODE_LALT;
        constexpr SDL_Scancode right_alt = SDL_SCANCODE_RALT;
        constexpr SDL_Scancode caps_lock = SDL_SCANCODE_CAPSLOCK;

        // Arrow keys
        constexpr SDL_Scancode up = SDL_SCANCODE_UP;
        constexpr SDL_Scancode down = SDL_SCANCODE_DOWN;
        constexpr SDL_Scancode left = SDL_SCANCODE_LEFT;
        constexpr SDL_Scancode right = SDL_SCANCODE_RIGHT;

    }
    
   
}
namespace HUNT::mouse
{
    
}
namespace HUNT::controller
{
    
}