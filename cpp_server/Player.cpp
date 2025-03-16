#include "Player.h"

bool Player::receivePlayerData()
{
    return session.receiveData(); 
}

void Player::sendPlayerData() 
{
    ByteStream& stream = session.getSendStream();
    stream.Clear();
    stream.WriteFloat(posX);
    stream.WriteFloat(posY);
    stream.HeaderInit();
    session.sendData();
}
