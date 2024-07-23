/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/packets/CProjectileStartSyncPacket.cpp
 *  PURPOSE:     Projectile start synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileStartSyncPacket.h"
#include "CProjectile.h"
#include "net/SyncStructures.h"

bool CProjectileStartSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (!m_projectile)
        return false;

    BitStream.Write(m_projectile->GetID());

    SPositionSync position;
    SRotationRadiansSync rotation;
    SVelocitySync        velocity;

    position.data.vecPosition = m_projectile->GetPosition();
    m_projectile->GetRotation(rotation.data.vecRotation);
    m_projectile->GetVelocity(velocity.data.vecVelocity);

    BitStream.Write(&position);
    BitStream.Write(&rotation);
    BitStream.Write(&velocity);

    return true;
}
