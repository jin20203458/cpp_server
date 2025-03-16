#pragma once
#pragma comment(lib, "ws2_32.lib")

#include "GameWorld.h"

using namespace std;

int main()
{
    GameWorld gameWorld;
    gameWorld.run();
    return 0;
}
