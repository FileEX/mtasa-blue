/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/packets/CProjectileSyncPacket.h
 *  PURPOSE:     Header for projectile sync packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CPacket.h"
#include "net/SyncStructures.h"

class CProjectileSyncPacket final : public CPacket
{
public:
    struct SyncData
    {
        ElementID ID;
        std::uint8_t         flags;
        std::uint8_t         syncTimeContext;
        SPositionSync position;
        SRotationRadiansSync rotation;
        SVelocitySync velocity;
    };

public:
    CProjectileSyncPacket(){};
    ~CProjectileSyncPacket();

    ePacketID GetPacketID() const { return PACKET_ID_PROJECTILE_SYNC; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::vector<SyncData*>::const_iterator IterBegin() { return m_Syncs.begin(); };
    std::vector<SyncData*>::const_iterator IterEnd() { return m_Syncs.end(); };

    std::vector<SyncData*> m_Syncs;
};
