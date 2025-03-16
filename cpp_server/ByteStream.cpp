#include "ByteStream.h"

template<typename T>
T ByteStream::ConvertEndian(T value) 
{
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
    std::reverse(ptr, ptr + sizeof(T));
    return value;
}

void ByteStream :: HeaderInit()  // Call last before send
{
    int dataSize = buffer.size() - sizeof(int);
    std::memcpy(&buffer[0], &dataSize, sizeof(int));
}

void ByteStream::WritePacketType(PacketType type)
{
    buffer.push_back(static_cast<uint8_t>(type));
}

void ByteStream::WriteInt(int value)
{
    int netValue = ConvertEndian(value);
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&netValue);
    buffer.insert(buffer.end(), ptr, ptr + sizeof(int));
}

void ByteStream::WriteFloat(float value) 
{
    float netValue = ConvertEndian(value);
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&netValue);
    buffer.insert(buffer.end(), ptr, ptr + sizeof(float));
}

void ByteStream::WriteString(const std::string& value) 
{
    int length = value.size();
    WriteInt(length);
    buffer.insert(buffer.end(), value.begin(), value.end());
}

PacketType ByteStream::ReadPacketType() 
{
    PacketType type = static_cast<PacketType>(buffer[readPos]);
    readPos += sizeof(uint8_t);
    return type;
}

int ByteStream::ReadInt() 
{
    if (readPos + sizeof(int) > buffer.size()) throw std::runtime_error("Buffer overread");

    int value;
    std::memcpy(&value, &buffer[readPos], sizeof(int));
    readPos += sizeof(int);
    return ConvertEndian(value);
}

float ByteStream::ReadFloat()
{
    if (readPos + sizeof(float) > buffer.size()) throw std::runtime_error("Buffer overread");

    float value;
    std::memcpy(&value, &buffer[readPos], sizeof(float));
    readPos += sizeof(float);
    return ConvertEndian(value);
}

std::string ByteStream::ReadString()
{
    int length = ReadInt();
    if (readPos + length > buffer.size()) throw std::runtime_error("Buffer overread");

    std::string value(buffer.begin() + readPos, buffer.begin() + readPos + length);
    readPos += length;
    return value;
}
