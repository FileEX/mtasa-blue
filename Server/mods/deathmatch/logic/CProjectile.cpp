/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CProjectile.cpp
 *  PURPOSE:     Projectile entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectile.h"
#include "CProjectileManager.h"
#include "CLogger.h"
#include <packets/CElementRPCPacket.h>
#include <net/rpc_enums.h>

CProjectile::CProjectile(CProjectileManager* projectileManager, CElement* parent, CElement* creator, eWeaponType weaponType)
    : CPerPlayerEntity(parent),
      m_projectileManager(projectileManager),
      m_Creator(creator),
      m_TargetEntity(nullptr),
      m_TargetPosition(CVector()),
      m_OriginPosition(CVector()),
      m_Velocity(CVector()),
      m_ProjectileType(weaponType),
      m_Model(0),
      m_Counter(0),
      m_Force(0.0f)
{
    m_iType = CElement::PROJECTILE;
    SetTypeName("projectile");

    m_projectileManager->AddToList(this);
}

CProjectile::~CProjectile()
{
    Unlink();
}

void CProjectile::Unlink()
{
    m_projectileManager->RemoveFromList(this);
}

void CProjectile::SetPosition(const CVector& position)
{
    if (m_vecPosition == position)
        return;

    m_vecPosition = position;

    GenerateSyncTimeContext();

    // Tell all the players that know about us
    CBitStream BitStream;
    BitStream.pBitStream->Write(position.fX);
    BitStream.pBitStream->Write(position.fY);
    BitStream.pBitStream->Write(position.fZ);
    BitStream.pBitStream->Write(GetSyncTimeContext());
    BroadcastOnlyVisible(CElementRPCPacket(this, SET_ELEMENT_POSITION, *BitStream.pBitStream));
}

void CProjectile::SetVelocity(const CVector& velocity)
{
    if (m_Velocity == velocity)
        return;

    m_Velocity = velocity;

    // Tell all the players that know about us
    CBitStream BitStream;
    BitStream.pBitStream->Write(velocity.fX);
    BitStream.pBitStream->Write(velocity.fY);
    BitStream.pBitStream->Write(velocity.fZ);
    BroadcastOnlyVisible(CElementRPCPacket(this, SET_ELEMENT_VELOCITY, *BitStream.pBitStream));
}

void CProjectile::SetModel(std::uint16_t model)
{

}

void CProjectile::SetCounter(std::uint32_t counter)
{
    m_Counter = counter;

    // Tell all the players that know about us
    CBitStream BitStream;
    BroadcastOnlyVisible(CElementRPCPacket(this, SET_ELEMENT_VELOCITY, *BitStream.pBitStream));
}

