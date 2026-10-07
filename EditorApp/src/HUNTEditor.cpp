#include <HUNT/Engine.h>
#include <HUNTGame/Game.h>

int main() {
    HUNT::Engine engine;
    if (!engine.Initialise("HUNTEngine - Editor")) return 1;
    HUNTgame::DemoGame preview(54, 42, 70);
    return engine.Run(preview);
}
