// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Game.h"

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Game game;

    if (game.Init()) {
        game.Run();
    }

    game.Shutdown();
    return 0;
}
