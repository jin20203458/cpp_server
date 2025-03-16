#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include "PacketType.h"

class ByteStream {
public:
	ByteStream() : readPos(0) { buffer.resize(sizeof(int), 0); } // Header reserve
    
    void HeaderInit(); // Call at the end
    void WritePacketType(PacketType type);
    void WriteInt(int value);
    void WriteFloat(float value);
    void WriteString(const std::string& value);

    PacketType ReadPacketType();
    int ReadInt();
    float ReadFloat();
    std::string ReadString();

    const std::vector<uint8_t>& GetBuffer() const { return buffer; }
    size_t GetSize() const { return buffer.size(); }
    void Clear() { buffer.resize(sizeof(int), 0); readPos = 0; }

    void SetBuffer(const std::vector<uint8_t>& data)
    {
        buffer = data;
        readPos = 0;
    }
    const char* GetCharBuffer() const { return reinterpret_cast<const char*>(buffer.data()); }

private:
    std::vector<uint8_t> buffer;
    size_t readPos;

    template<typename T>
    T ConvertEndian(T value);
};
