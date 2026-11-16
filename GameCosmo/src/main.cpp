#include "game.h"

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

int main()
{
    Game CosmoGame(SCREEN_WIDTH, SCREEN_HEIGHT);
    CosmoGame.Run();

    return 0;
}