#pragma once


#include <Engine/include/HUNT/Input/Keys.h>
#include <array>

class Input {
public:
    bool isHeld(HUNT::Key key) const;
    bool wasPressed(HUNT::Key key) const;
    bool wasReleased(HUNT::Key key) const;

private:
    
    //Temp 256 find a good way to set this
    std::array<bool, 256> current{};
    std::array<bool, 256> previous{};
    
    
};