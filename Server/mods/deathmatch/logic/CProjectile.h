/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CProjectile.h
 *  PURPOSE:     Projectile entity class header
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CElement.h"
#include "CProjectileManager.h"

class CProjectile : public CElement
{
public:
    CProjectile(class CProjectileManager* projectileManager, CElement* parent, CElement* creator, eWeaponType weaponType);
    ~CProjectile();

    void Unlink();

    void SetPosition(const CVector& position);
    void SetRotation(const CVector& rotation);

    void GetVelocity(CVector& velocity) const noexcept { velocity = m_Velocity; }
    void SetVelocity(const CVector& velocity);

    void          SetModel(std::uint16_t model);
    std::uint16_t GetModel() const noexcept { return m_Model; }

    void          SetCounter(std::uint32_t counter);
    std::uint32_t GetCounter() const noexcept { return m_Counter; }

    CElement*     GetCreator() const noexcept { return m_Creator; }
    eWeaponType   GetWeaponType() const noexcept { return m_ProjectileType; }

    void          SetTargetEntity(CElement* target) noexcept { m_TargetEntity = target; }
    CElement*     GetTargetEntity() const noexcept { return m_TargetEntity; }

    void          SetTarget(const CVector& target) noexcept { m_TargetPosition = target; }
    void          GetTarget(CVector& target) const noexcept { target = m_TargetPosition; }

    void          SetOrigin(const CVector& origin) noexcept { m_OriginPosition = origin; }
    void          GetOrigin(CVector& origin) const noexcept { origin = m_OriginPosition; }

    void          SetForce(float force) noexcept { m_Force = force; }
    float         GetForce() const noexcept { return m_Force; }

    bool     IsSyncable() const noexcept { return m_syncable; }
    void     SetSyncable(bool syncable) noexcept { m_syncable = syncable; }

    void     SetSyncer(CPlayer* player) noexcept { m_syncer = player; }
    CPlayer* GetSyncer() { return m_syncer; }

protected:
    bool ReadSpecialData(const int iLine) override;

public:
    std::uint64_t m_CreationTime;

private:
    CProjectileManager* m_projectileManager;
    CPlayer*            m_syncer;
    bool                m_syncable;

    CElement* m_Creator;
    CElement* m_TargetEntity;
    CVector  m_TargetPosition;
    CVector  m_OriginPosition;
    CVector  m_Velocity;
    CVector   m_Rotation;

    eWeaponType   m_ProjectileType;
    std::uint16_t m_Model;
    std::uint32_t m_Counter;
    float         m_Force;
};
