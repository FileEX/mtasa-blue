/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CProjectileManager.h
 *  PURPOSE:     Projectile entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CProjectile.h"
#include "Utils.h"

class CElement;

class CProjectileManager
{
    friend class CProjectile;

public:
    CProjectileManager();
    ~CProjectileManager() { DeleteAll(); }

    CProjectile* Create(CElement* parent, CElement* creator, eWeaponType weaponType);
    void         DestroyProjectiles(CPlayer* creator);
    void         DoPulse();

    void DeleteAll() { DeletePointersAndClearList(m_projectilesList); }

    std::uint32_t Count() { return m_projectilesList.size(); }
    bool          Exists(CProjectile* projectile) { return ListContains(m_projectilesList, projectile); }

    std::list<CProjectile*>::const_iterator IterBegin() { return m_projectilesList.begin(); }
    std::list<CProjectile*>::const_iterator IterEnd() { return m_projectilesList.end(); }

private:
    void AddToList(CProjectile* projectile) { m_projectilesList.push_back(projectile); }
    void RemoveFromList(CProjectile* projectile) { m_projectilesList.remove(projectile); }

    CElapsedTime            m_updateProjectilesListTimer;
    std::list<CProjectile*> m_projectilesList;
};
