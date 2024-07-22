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
    : CElement(parent),
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

    m_CreationTime = GetTickCount64_();
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
}

void CProjectile::SetRotation(const CVector& rotation)
{
    if (m_Rotation == rotation)
        return;

    m_Rotation = rotation;
}

void CProjectile::SetVelocity(const CVector& velocity)
{
    if (m_Velocity == velocity)
        return;

    m_Velocity = velocity;
}

void CProjectile::SetModel(std::uint16_t model)
{

}

void CProjectile::SetCounter(std::uint32_t counter)
{
    m_Counter = counter;

}

bool CProjectile::ReadSpecialData(const int iLine)
{
    int iTemp;
    if (GetCustomDataInt("dimension", iTemp, true))
        m_usDimension = static_cast<unsigned short>(iTemp);

    return true;
}

