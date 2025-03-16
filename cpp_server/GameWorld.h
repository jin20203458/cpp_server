#pragma once
#include <unordered_map>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <iostream>
#include <chrono>
#include <print>
#include "Player.h"
#include "PacketType.h"

class GameWorld {
public:
    GameWorld();
    ~GameWorld();

    void addPlayer(SOCKET socket, Player* player);
    void removePlayer(SOCKET socket);
    void run();

private:
    void initialize(int portNum);
    void cleanup();
    void processPlayerInit(Player* player, ByteStream& stream);
    void processPlayerUpdate(Player* player, ByteStream& stream);
    void processMonsterUpdate(ByteStream& stream);
    void sendWorldData();
    void updateWorld();

    std::unordered_map<SOCKET, Player*> players;
    std::mutex worldMutex;
    fd_set readfds;
    SOCKET listenSock;
    std::thread updateThread;
    std::thread sendThread;
    ByteStream worldDataStream;
    bool running;
};
