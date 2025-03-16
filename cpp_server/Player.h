#pragma once
#include "ClientSession.h"
#include <string>

class Player {
public:
    Player(SOCKET socket, const std::string& name) : session(socket), name(name), posX(0), posY(0) {}

    ClientSession& getSession() { return session; }
    void updatePosition(float x, float y) { posX = x; posY = y; }
    void sendPlayerData();
    bool receivePlayerData();

    void setName(const std::string& name) { this->name = name; }
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    const std::string& getName() const { return name; }

private:
    ClientSession session;
    std::string name;
    float posX, posY;
};

