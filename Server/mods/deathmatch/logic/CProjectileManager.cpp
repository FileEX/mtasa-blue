/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CProjectileManager.cpp
 *  PURPOSE:     Projectile entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileManager.h"
#include "CProjectile.h"
#include "Utils.h"

CProjectileManager::CProjectileManager()
{
    m_updateProjectilesListTimer.SetMaxIncrement(1500, true);
}

CProjectile* CProjectileManager::Create(CElement* parent, CElement* creator, eWeaponType weaponType)
{
    CProjectile* const projectile = new CProjectile(this, parent, creator, weaponType);

    if (projectile->GetID() == INVALID_ELEMENT_ID)
    {
        delete projectile;
        return nullptr;
    }

    return projectile;
}

void CProjectileManager::DestroyProjectiles(CPlayer* creator)
{
    for (auto it = IterBegin(); it != IterEnd();)
    {
        if ((*it)->GetCreator() == creator)
            delete *it;
    }
}

void CProjectileManager::DoPulse()
{
    if (m_updateProjectilesListTimer.Get() <= 1000)
        return;

    for (auto it = IterBegin(); it != IterEnd();)
    {
        if (GetTickCount64_() >= (*it)->m_CreationTime + (*it)->m_Counter)
            delete *it;
    }

    m_updateProjectilesListTimer.Reset();
}
