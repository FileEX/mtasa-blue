/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CProjectileSync.h
 *  PURPOSE:     Header for projectile sync class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CPlayerManager.h"
#include "CProjectileManager.h"
#include "packets/CProjectileSyncPacket.h"

class CProjectileSync
{
public:
    CProjectileSync(CPlayerManager* playerManager, CProjectileManager* projectileManager);

    void DoPulse();
    bool ProcessPacket(CPacket& packet);

    void OverrideSyncer(CProjectile* projectile, CPlayer* player, bool persist = false);

    void SetSyncerAsPersistent(bool persist) noexcept { m_syncPersist = persist; }
    bool IsSyncerPersistent() const noexcept { return m_syncPersist; }

private:
    void UpdateAllSyncer();
    void UpdateSyncer(CProjectile* projectile);
    void FindSyncer(CProjectile* projectile);
    CPlayer* FindPlayerCloseToProjectile(CProjectile* projectile, float maxDist);

    void StartSync(CPlayer* player, CProjectile* projectile);
    void StopSync(CProjectile* projectile);

    void Packet_ProjectileSync(CProjectileSyncPacket& packet);

    CPlayerManager* m_playerManager;
    CProjectileManager* m_projectileManager;
    bool                m_syncPersist = false;

    CElapsedTime m_updateSyncerTimer;
};
