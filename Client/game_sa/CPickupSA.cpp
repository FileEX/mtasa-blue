/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPickupSA.cpp
 *  PURPOSE:     Pickup entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CPickupSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

CPickupSA::CPickupSA(CPickupSAInterface* pickupInterface)
{
    m_internalInterface = pickupInterface;
    m_object = 0;
}

void CPickupSA::SetPosition(CVector* vecPosition)
{
    GetInterface()->isPickupNearby = false;

    CPickupSAInterface* iPickup = GetInterface();
    iPickup->coorsX = static_cast<std::int16_t>((vecPosition->fX * 8.0f));
    iPickup->coorsY = static_cast<std::int16_t>((vecPosition->fY * 8.0f));
    iPickup->coorsZ = static_cast<std::int16_t>((vecPosition->fZ * 8.0f));
}

CVector* CPickupSA::GetPosition(CVector* vecPosition)
{
    CPickupSAInterface* iPickup = GetInterface();
    vecPosition->fX = iPickup->coorsX / 8.0f;
    vecPosition->fY = iPickup->coorsY / 8.0f;
    vecPosition->fZ = iPickup->coorsZ / 8.0f;
    return vecPosition;
}

ePickupType CPickupSA::GetType()
{
    return GetInterface()->pickupType;
}

void CPickupSA::SetType(ePickupType type)
{
    GetInterface()->pickupType = type;
}

float CPickupSA::GetCurrentValue()
{
    return GetInterface()->currentValue;
}

void CPickupSA::SetCurrentValue(float fCurrentValue)
{
    GetInterface()->currentValue = fCurrentValue;
}

void CPickupSA::SetRegenerationTime(DWORD dwTime)
{
    GetInterface()->regenerationTime = dwTime;
}

void CPickupSA::SetMoneyPerDay(WORD wMoneyPerDay)
{
    GetInterface()->moneyPerDay = wMoneyPerDay;
}

WORD CPickupSA::GetMoneyPerDay()
{
    return GetInterface()->moneyPerDay;
}

WORD CPickupSA::GetModel()
{
    return GetInterface()->modelID;
}

void CPickupSA::SetModel(WORD wModelIndex)
{
    GetInterface()->modelID = wModelIndex;
}

ePickupState CPickupSA::GetState()
{
    return GetInterface()->pickupState;
}

void CPickupSA::SetState(ePickupState bState)
{
    GetInterface()->pickupState = bState;
}

BYTE CPickupSA::GetAmmo()
{
    return GetInterface()->noAmmo;
}

void CPickupSA::SetAmmo(BYTE bAmmo)
{
    GetInterface()->noAmmo = bAmmo;
}

long CPickupSA::GetMonetaryValue()
{
    return GetInterface()->monetaryValue;
}

void CPickupSA::SetMonetaryValue(long lMonetaryValue)
{
    GetInterface()->monetaryValue = lMonetaryValue;
}

BYTE CPickupSA::IsNearby()
{
    return GetInterface()->isPickupNearby;
}

void CPickupSA::GiveUsAPickUpObject(int ForcedObjectIndex)
{
    DWORD GiveUsAPickUpObject = FUNC_GIVEUSAPICKUP;
    DWORD dwObject = (DWORD) & (GetInterface()->object);
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    ForcedObjectIndex
        push    dwObject
        mov     ecx, dwThis
        call    GiveUsAPickUpObject
    }
    if (GetInterface()->object)
    {
        if (m_object)
        {
            ((CEntitySA*)m_object)->DoNotRemoveFromGame = true;
            delete m_object;
        }

        m_object = new CObjectSA(GetInterface()->object);
    }
}

void CPickupSA::GetRidOfObjects()
{
    if (GetInterface()->object)
        ((CWorldSA*)pGame->GetWorld())->Remove(GetInterface()->object, CPickup_Destructor);

    if (m_object)
    {
        ((CEntitySA*)m_object)->DoNotRemoveFromGame = true;
        delete m_object;
        m_object = nullptr;
    }
}

void CPickupSA::Remove()
{
    DWORD dwFunc = FUNC_CPickup_Remove;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // CPickup::Remove also destroys the owned object, so we need to delete our CObjectSA class
    if (m_object)
    {
        ((CEntitySA*)m_object)->DoNotRemoveFromGame = true;
        delete m_object;
        m_object = nullptr;
    }
}
