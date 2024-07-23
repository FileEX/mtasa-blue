/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileStopSyncPacket.h
 *  PURPOSE:     Projectile stop sync packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CProjectileStopSyncPacket final : public CPacket
{
public:
    CProjectileStopSyncPacket(ElementID ID) noexcept { m_ID = ID; }

    ePacketID     GetPacketID() const { return PACKET_ID_PROJECTILE_STOPSYNC; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const
    {
        BitStream.Write(m_ID);
        return true;
    };

private:
    ElementID m_ID;
};
