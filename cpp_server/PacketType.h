#pragma once
#include <cstdint>

enum class PacketType : uint8_t
{
    PlayerInit = 0x01,
    PlayerUpdate = 0x02,
    MonsterUpdate = 0x03,
    WorldUpdate = 0x04
};