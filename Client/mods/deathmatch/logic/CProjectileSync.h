/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CProjectileSync.h
 *  PURPOSE:     Header for projectile sync class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CClientCommon.h>
#include "CClientProjectile.h"

class CProjectileSync
{
public:
    CProjectileSync(CClientProjectileManager* projectileManager);

    bool ProcessPacket(std::uint8_t packetID, NetBitStreamInterface& bitStream);
    void DoPulse();

    void AddProjectile(CClientProjectile* projectile);
    void RemoveProjectile(CClientProjectile* projectile);

    std::list<CClientProjectile*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<CClientProjectile*>::const_iterator IterEnd() { return m_List.end(); }

    bool Exists(CClientProjectile* projectile);

private:
    void Packet_ProjectileStartSync(NetBitStreamInterface& bitStream);
    void Packet_ProjectileStopSync(NetBitStreamInterface& bitStream);
    void Packet_ProjectileSync(NetBitStreamInterface& bitStrem);

    void Sync();
    void WriteProjectileInformation(NetBitStreamInterface* bitStream, CClientProjectile* projectile);

    CClientProjectileManager* m_projectileManager;
    CMappedList<CClientProjectile*> m_List;
    std::uint64_t                   m_lastSyncTime;
};
