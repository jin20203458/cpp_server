#include "ClientSession.h"

ClientSession::ClientSession(SOCKET socket) : clientSocket(socket) {}

ClientSession::~ClientSession() {
    closesocket(clientSocket);
}

bool ClientSession::receiveData()
{
    int bytesRead = recv(clientSocket, receiveBuffer, sizeof(int), 0);
    if (bytesRead <= 0)
    {
        return false;
    }
    else if (bytesRead == sizeof(int))
    {
        int dataSize = *reinterpret_cast<int*>(receiveBuffer);
        bytesRead = recv(clientSocket, receiveBuffer, dataSize, 0);
        if (bytesRead == dataSize)
        {
            receiveStream.SetBuffer(std::vector<uint8_t>(receiveBuffer, receiveBuffer + dataSize));
            return true;
        }
    }
    return false;
}

bool ClientSession::sendData() 
{
    const std::vector<uint8_t>& buffer = sendStream.GetBuffer();
    int bytesSent = send(clientSocket, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0);
    return bytesSent == buffer.size();
}

SOCKET ClientSession::getSocket() const { return clientSocket;}

ByteStream& ClientSession::getReceiveStream() { return receiveStream;}

ByteStream& ClientSession::getSendStream() { return sendStream;}
