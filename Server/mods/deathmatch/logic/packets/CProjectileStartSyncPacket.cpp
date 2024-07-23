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

bool CProjectileStartSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (!m_projectile)
        return false;

    BitStream.Write(m_projectile->GetID());

    CVector position = m_projectile->GetPosition();
    BitStream.WriteVector(position.fX, position.fY, position.fZ);

    CVector rotation;
    m_projectile->GetRotation(rotation);
    BitStream.WriteVector(rotation.fX, rotation.fY, rotation.fZ);

    CVector velocity;
    m_projectile->GetVelocity(velocity);
    BitStream.WriteVector(velocity.fX, velocity.fY, velocity.fZ);

    return true;
}
