/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileStartSyncPacket.h
 *  PURPOSE:     Projectile start synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CPacket.h"

class CProjectile;

class CProjectileStartSyncPacket final : public CPacket
{
public:
    CProjectileStartSyncPacket(CProjectile* projectile) { m_projectile = projectile; }

    ePacketID GetPacketID() const noexcept { return PACKET_ID_PROJECTILE_STARTSYNC; }
    unsigned long GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    CProjectile* m_projectile;
};
