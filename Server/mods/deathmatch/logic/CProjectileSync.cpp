/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CProjectileSync.cpp
 *  PURPOSE:     Projectile entity synchronization class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileSync.h"
#include <packets/CProjectileStartSyncPacket.h>
#include <packets/CProjectileStopSyncPacket.h>
#include <CElementIDs.h>
#include "CGame.h"
#include "CColManager.h"

CProjectileSync::CProjectileSync(CPlayerManager* playerManager, CProjectileManager* projectileManager)
    : m_playerManager(playerManager), m_projectileManager(projectileManager)
{
}

void CProjectileSync::DoPulse()
{
    if (m_updateSyncerTimer.Get() > 500)
    {
        m_updateSyncerTimer.Reset();
        UpdateAllSyncer();
    }
}

bool CProjectileSync::ProcessPacket(CPacket& packet)
{
    if (packet.GetPacketID() == PACKET_ID_PROJECTILE_SYNC)
    {
        Packet_ProjectileSync(static_cast<CProjectileSyncPacket&>(packet));
        return true;
    }

    return false;
}

void CProjectileSync::OverrideSyncer(CProjectile* projectile, CPlayer* player, bool persistent)
{
    CPlayer* syncer = projectile->GetSyncer();
    if (syncer)
    {
        if (syncer == player)
        {
            if (!persistent)
                SetSyncerAsPersistent(false);

            return;
        }

        StopSync(projectile);
    }

    if (player && !projectile->IsBeingDeleted())
    {
        SetSyncerAsPersistent(persistent);
        StartSync(player, projectile);
    }
}

void CProjectileSync::UpdateAllSyncer()
{
    for (auto iter = m_projectileManager->IterBegin(); iter != m_projectileManager->IterEnd(); iter++)
    {
        UpdateSyncer(*iter);
    }
}

void CProjectileSync::UpdateSyncer(CProjectile* projectile)
{
    CPlayer* syncer = projectile->GetSyncer();
    if (!projectile->IsSyncable())
    {
        if (syncer)
            StopSync(projectile);

        return;
    }

    if (syncer)
    {
        if (IsSyncerPersistent() || (projectile->GetDimension() == syncer->GetDimension() && IsPointNearPoint3D(syncer->GetPosition(), projectile->GetPosition(), 500.0f)))
            return;

        StopSync(projectile);
    }

    if (projectile->IsBeingDeleted())
        return;

    FindSyncer(projectile);
}

void CProjectileSync::FindSyncer(CProjectile* projectile)
{
    assert(projectile->IsSyncable());

    CPlayer* player = FindPlayerCloseToProjectile(projectile, 500.0f);
    if (player)
        StartSync(player, projectile);
}

void CProjectileSync::StartSync(CPlayer* syncer, CProjectile* projectile)
{
    if (!projectile->IsSyncable())
        return;

    syncer->Send(CProjectileStartSyncPacket(projectile));

    projectile->SetSyncer(syncer);

    CLuaArguments Arguments;
    Arguments.PushElement(syncer);
    projectile->CallEvent("onElementStartSync", Arguments);
}

void CProjectileSync::StopSync(CProjectile* projectile)
{
    CPlayer* syncer = projectile->GetSyncer();
    syncer->Send(CProjectileStopSyncPacket(projectile->GetID()));

    projectile->SetSyncer(nullptr);

    SetSyncerAsPersistent(false);

    CLuaArguments Arguments;
    Arguments.PushElement(syncer);
    projectile->CallEvent("onElementStopSync", Arguments);
}

CPlayer* CProjectileSync::FindPlayerCloseToProjectile(CProjectile* projectile, float maxDist)
{
    CVector projectilePos = projectile->GetPosition();

    CPlayer* lastSyncer;
    CPlayer* syncer;

    for (auto iter = m_playerManager->IterBegin(); iter != m_playerManager->IterEnd(); iter++)
    {
        syncer = *iter;

        if (!syncer->IsJoined())
            continue;

        if (syncer->GetDimension() != projectile->GetDimension())
            continue;

        if (!IsPointNearPoint3D(projectilePos, syncer->GetPosition(), maxDist))
            continue;

        if (!lastSyncer || syncer->CountSyncingProjectiles() < lastSyncer->CountSyncingProjectiles())
            lastSyncer = syncer;
    }

    return lastSyncer;
}

void CProjectileSync::Packet_ProjectileSync(CProjectileSyncPacket& packet)
{
    CPlayer* player = packet.GetSourcePlayer();
    if (!player || !player->IsJoined())
        return;

    // Apply data for each projectile in packet
    for (auto& Data : packet.m_Syncs)
    {
        // Grab the projectile
        CElement* projectileElement = CElementIDs::GetElement(Data->ID);
        if (!projectileElement || !IS_PROJECTILE(projectileElement))
            continue;

        // Check if the time context matches.
        CProjectile* projectile = static_cast<CProjectile*>(projectileElement);
        if (projectile->GetSyncer() != player || !projectile->CanUpdateSync(Data->syncTimeContext))
            continue;

        // Apply data
        if (Data->flags & 0x1)
        {
            projectile->SetPosition(Data->position.data.vecPosition);
            g_pGame->GetColManager()->DoHitDetection(projectile->GetPosition(), projectile);
        }
        if (Data->flags & 0x2)
            projectile->SetRotation(Data->rotation.data.vecRotation);
        if (Data->flags & 0x3)
            projectile->SetVelocity(Data->velocity.data.vecVelocity);
    }

    m_playerManager->BroadcastOnlyJoined(packet, player);
}
