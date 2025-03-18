#include "GameWorld.h"

GameWorld::GameWorld() : running(true)
{
    initialize(5000);
}

void GameWorld::run()
{
    updateThread = std::thread(&GameWorld::updateWorld, this);
    sendThread = std::thread(&GameWorld::sendWorldData, this);
    updateThread.join();
    sendThread.join();
}

void GameWorld::initialize(int portNum)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cerr << "WSAStartup failed!\n";
        exit(-1);
    }

    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "Server socket creation failed!\n";
        exit(-1);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNum);

    if (bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!\n";
        exit(-1);
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed!\n";
        exit(-1);
    }

    FD_ZERO(&readfds);
    FD_SET(listenSock, &readfds);
}

void GameWorld::cleanup()
{
    for (auto& pair : players)  delete pair.second;

    closesocket(listenSock);
    WSACleanup();
}

void GameWorld::addPlayer(SOCKET socket, Player* player)
{
    std::lock_guard<std::mutex> lock(worldMutex);
    players[socket] = player;
}

void GameWorld::removePlayer(SOCKET socket)
{
    std::lock_guard<std::mutex> lock(worldMutex);
    auto it = players.find(socket);
    if (it != players.end())
    {
        delete it->second;
        players.erase(it);
    }
}

void GameWorld::updateWorld()
{
    SOCKADDR_IN caddr;
    int addrlen = sizeof(caddr);
    SOCKET client_sock;

    while (running)
    {
        fd_set copyfds = readfds;
        int fd_num = select(0, &copyfds, NULL, NULL, NULL);
        if (fd_num == SOCKET_ERROR)
        {
            std::cerr << "select error\n";
            break;
        }

        if (FD_ISSET(listenSock, &copyfds))
        {
            client_sock = accept(listenSock, (SOCKADDR*)&caddr, &addrlen);
            if (client_sock == INVALID_SOCKET)std::cerr << "accept failed!\n";
            else
            {
                FD_SET(client_sock, &readfds);
                addPlayer(client_sock, new Player(client_sock, "UninitPlayer"));
                std::print("new client connected, current number of players: {}\n", players.size());
            }
        }

        for (int i = 0; i < readfds.fd_count; i++)
        {
            SOCKET sock = readfds.fd_array[i];
            if (FD_ISSET(sock, &copyfds) && sock != listenSock)
            {
                auto it = players.find(sock);
                if (it != players.end())
                {
                    Player* player = it->second;
                    if (!player->receivePlayerData())
                    {     
                        std::print("disconneted player :{}\n",player->getName());
                        closesocket(sock);
                        FD_CLR(sock, &readfds);
                        removePlayer(sock);
                    }
                    else
                    {
                        ByteStream& stream = player->getSession().getReceiveStream();
                        PacketType dataType = stream.ReadPacketType();

                        if      (dataType == PacketType::PlayerInit)    processPlayerInit(player, stream);
                        else if (dataType == PacketType::PlayerUpdate)  processPlayerUpdate(player, stream);
                        else if (dataType == PacketType::MonsterUpdate) processMonsterUpdate(stream);
                        else    std::cerr << "Invalid packet type\n";
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250)); // send�� 2-3��ӵ��� ����
        std::this_thread::sleep_for(std::chrono::milliseconds(8)); // send�� 2-3��ӵ��� ����
    }
}

void GameWorld::sendWorldData()
{
    while (running)
    {
		if (players.empty()) continue;

        {
        std::lock_guard<std::mutex> lock(worldMutex);
        worldDataStream.Clear();
        worldDataStream.WritePacketType(PacketType::WorldUpdate);
		worldDataStream.WriteInt(players.size());

        for (auto& pair : players)  // ���� ������ ����ȭ
        {
            Player* player = pair.second;
            worldDataStream.WriteString(player->getName());
            worldDataStream.WriteFloat(player->getPosX());
            worldDataStream.WriteFloat(player->getPosY());
                //  std::print("Player {} at ({}, {})\n",
                // player->getName(), player->getPosX(), player->getPosY());
         //  std::print("Player {} at ({}, {})\n", player->getName(), player->getPosX(), player->getPosY());
        }
        worldDataStream.HeaderInit();
        }
       
        for (auto& pair : players)  // ����ȭ�� ���嵥���� ��ε�ĳ��Ʈ
        {
            Player* player = pair.second;
            player->getSession().getSendStream().SetBuffer(worldDataStream.GetBuffer());
            player->getSession().sendData();
        }
        std::print("data size {}\n",worldDataStream.GetSize());
		worldDataStream.HeaderInit();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 60 FPS
    }
}

void GameWorld::processPlayerInit(Player* player, ByteStream& stream)
{
    std::string name = stream.ReadString();
    player->setName(name);
    float x = stream.ReadFloat();
    float y = stream.ReadFloat();
    player->updatePosition(x, y);
	std::print("Player {} initialized at ({}, {})\n", name, x, y);
}

void GameWorld::processPlayerUpdate(Player* player, ByteStream& stream)
{
    float x = stream.ReadFloat();
    float y = stream.ReadFloat();
    player->updatePosition(x, y);
	std::print("Player {} updated position to ({}, {})\n", player->getName(), x, y);
}

void GameWorld::processMonsterUpdate(ByteStream& stream)
{
    // ���� �����͸� ó���ϴ� ������ ���⿡ �߰��մϴ�.
}

GameWorld::~GameWorld()
{
    running = false;
    updateThread.join();
    sendThread.join();
    cleanup();
}
