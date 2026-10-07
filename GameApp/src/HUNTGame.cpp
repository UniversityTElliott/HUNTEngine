#include <HUNT/Engine.h>
#include <HUNTGame/Game.h>

int main() {
    HUNT::Engine engine;
    if (!engine.Initialise("HUNTEngine")) return 1;
    HUNTgame::DemoGame game(24, 48, 85);
    return engine.Run(game);
}
