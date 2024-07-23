/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CProjectileSync.cpp
 *  PURPOSE:     Projectile synchronization handler
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileSync.h"

#define PROJECTILE_SYNC_RATE ( 1000 )

CProjectileSync::CProjectileSync(CClientProjectileManager* projectileManager)
    : m_projectileManager(projectileManager), m_lastSyncTime(0)
{
}

bool CProjectileSync::ProcessPacket(std::uint8_t packetID, NetBitStreamInterface& bitStream)
{
    switch (packetID)
    {
        case PACKET_ID_PROJECTILE_STARTSYNC:
        {
            Packet_ProjectileStartSync(bitStream);
            return true;
        }

        case PACKET_ID_PROJECTILE_STOPSYNC:
        {
            Packet_ProjectileStopSync(bitStream);
            return true;
        }

        case PACKET_ID_PROJECTILE_SYNC:
        {
            Packet_ProjectileSync(bitStream);
            return true;
        }
    }

    return false;
}

void CProjectileSync::DoPulse()
{
    // Has it been long enough since our last state's sync?
    unsigned long ulCurrentTime = CClientTime::GetTime();
    if (ulCurrentTime >= m_lastSyncTime + PROJECTILE_SYNC_RATE)
    {
        Sync();
        m_lastSyncTime = ulCurrentTime;
    }
}

void CProjectileSync::AddProjectile(CClientProjectile* projectile)
{
    m_List.push_front(projectile);
}

void CProjectileSync::RemoveProjectile(CClientProjectile* projectile)
{
    if (!m_List.empty())
        m_List.remove(projectile);
}

bool CProjectileSync::Exists(CClientProjectile* projectile)
{
    return m_List.Contains(projectile);
}

void CProjectileSync::Packet_ProjectileStartSync(NetBitStreamInterface& bitStream)
{
    // Read out the ID
    ElementID ID;
    if (!bitStream.Read(ID))
        return;

    CClientProjectile* projectile = m_projectileManager->Get(ID);
    if (!projectile)
        return;

    SPositionSync position;
    SRotationRadiansSync rotation;
    SVelocitySync        velocity;

    if (!bitStream.Read(&position) || !bitStream.Read(&rotation) || !bitStream.Read(&velocity))
        return;

    projectile->SetPosition(position.data.vecPosition);
    projectile->SetRotationRadians(rotation.data.vecRotation);
    projectile->SetVelocity(velocity.data.vecVelocity);

    AddProjectile(projectile);
}

void CProjectileSync::Packet_ProjectileStopSync(NetBitStreamInterface& bitStream)
{
    ElementID ID;
    if (!bitStream.Read(ID))
        return;

    CClientProjectile* projectile = m_projectileManager->Get(ID);
    if (!projectile)
        return;

    RemoveProjectile(projectile);
}

void CProjectileSync::Packet_ProjectileSync(NetBitStreamInterface& bitStream)
{
    while (bitStream.GetNumberOfUnreadBits() > 32)
    {
        ElementID ID;
        if (!bitStream.Read(ID))
            return;

        std::uint8_t syncTimeContext;
        if (!bitStream.Read(syncTimeContext))
            return;

        std::uint8_t flags;
        if (!bitStream.Read(flags))
            return;

        CVector vecPos, vecRot, vecVel;

        SPositionSync position;
        if (flags & 0x1 && !bitStream.Read(&position))
            return;

        SRotationRadiansSync rotation;
        if (flags & 0x2 && !bitStream.Read(&rotation))
            return;

        SVelocitySync velocity;
        if (flags & 0x3 && !bitStream.Read(&velocity))
            return;

        vecPos = position.data.vecPosition;
        vecRot = rotation.data.vecRotation;
        vecVel = velocity.data.vecVelocity;

        CClientProjectile* projectile = m_projectileManager->Get(ID);
        if (!projectile || !projectile->CanUpdateSync(syncTimeContext))
            return;

        if (flags & 0x1)
            projectile->SetPosition(vecPos);
        if (flags & 0x2)
            projectile->SetRotationRadians(vecRot);
        if (flags & 0x3)
            projectile->SetVelocity(vecVel);
    }
}

void CProjectileSync::Sync()
{
    if (m_List.size() <= 0)
        return;

    NetBitStreamInterface* bitStream = g_pNet->AllocateNetBitStream();
    if (!bitStream)
        return;

    for (auto iter = IterBegin(); iter != IterEnd(); ++iter)
    {
        WriteProjectileInformation(bitStream, *iter);
    }

    // Send and destroy the packet
    g_pNet->SendPacket(PACKET_ID_PROJECTILE_SYNC, bitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED);
    g_pNet->DeallocateNetBitStream(bitStream);
}

void CProjectileSync::WriteProjectileInformation(NetBitStreamInterface* bitStream, CClientProjectile* projectile)
{
    CVector vecPos, vecRot, vecVel;
    projectile->GetPosition(vecPos);
    projectile->GetRotationRadians(vecRot);
    projectile->GetVelocity(vecVel);

    std::uint8_t flags = 0;

    if (vecPos != projectile->m_LastSyncedData->position)
        flags |= 0x1;
    if (vecRot != projectile->m_LastSyncedData->rotation)
        flags |= 0x2;
    if (vecVel != projectile->m_LastSyncedData->velocity)
        flags |= 0x3;

    if (flags == 0)
        return;

    bitStream->Write(projectile->GetID());
    bitStream->Write(projectile->GetSyncTimeContext());
    bitStream->Write(flags);

    if (flags & 0x1)
    {
        SPositionSync pos;
        pos.data.vecPosition = vecPos;
        bitStream->Write(&pos);
    }

    if (flags & 0x2)
    {
        SRotationRadiansSync rot;
        rot.data.vecRotation = vecRot;
        bitStream->Write(&rot);
    }

    if (flags & 0x3)
    {
        SVelocitySync vel;
        vel.data.vecVelocity = vecVel;
        bitStream->Write(&vel);
    }
}
