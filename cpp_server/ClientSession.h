#pragma once
#include <winsock2.h>
#include "ByteStream.h"

class ClientSession {
public:
    ClientSession(SOCKET socket);
    ~ClientSession();

    bool receiveData();
    bool sendData();
    SOCKET getSocket() const;
    ByteStream& getReceiveStream();
    ByteStream& getSendStream();

private:
    SOCKET clientSocket;
    char receiveBuffer[1024];
    char sendBuffer[1024];
    ByteStream receiveStream;
    ByteStream sendStream;
};
