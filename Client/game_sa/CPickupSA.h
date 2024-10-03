/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPickupSA.h
 *  PURPOSE:     Header file for pickup entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPickup.h>
#include "CObjectSA.h"

class CObjectSAInterface;
class CObjectSA;

#define FUNC_GIVEUSAPICKUP      0x4567e0
#define FUNC_CPickup_Remove     0x4556C0

class CPickupSAInterface
{
public:
    float               currentValue;            // For the revenue pickups
    CObjectSAInterface* object;
    std::uint32_t       monetaryValue;
    std::uint32_t       regenerationTime;
    std::int16_t        coorsX, coorsY, coorsZ;            // CCompressedVector
    std::uint16_t       moneyPerDay;
    std::int16_t        modelID;
    std::int16_t        referenceIndex;
    ePickupType         pickupType;
    ePickupState        pickupState;

    union
    {
        struct
        {
            // 'disabled' flag is replaced by pickupState
            std::uint8_t noAmmo : 1;
            std::uint8_t helpMessageDisplayed : 1;
            std::uint8_t isPickupNearby : 1;            // If the pickup is nearby it will get an object and it will get updated.
            std::uint8_t textIndex : 3;                 // What text label do we print out above it.
        };

        std::uint8_t flags;
    };
};
static_assert(sizeof(CPickupSAInterface) == 0x20, "Invalid size for CPickupSAInterface");

class CPickupSA : public CPickup
{
private:
    CPickupSAInterface* m_internalInterface;
    CObjectSA*          m_object;

public:
    CPickupSA(CPickupSAInterface* pickupInterface);
    CPickupSAInterface* GetInterface() noexcept { return m_internalInterface; }
    CObject*            GetObject() noexcept { return m_object; }

    void     SetPosition(CVector* vecPosition);
    CVector* GetPosition(CVector* vecPosition);

    ePickupType  GetType();
    void         SetType(ePickupType type);
    float        GetCurrentValue();
    void         SetCurrentValue(float fCurrentValue);
    void         SetRegenerationTime(DWORD dwTime);
    void         SetMoneyPerDay(WORD wMoneyPerDay);
    WORD         GetMoneyPerDay();
    WORD         GetModel();
    void         SetModel(WORD wModelIndex);            // do not export
    ePickupState GetState();
    void         SetState(ePickupState bState);
    BYTE         GetAmmo();
    void         SetAmmo(BYTE bAmmo);
    long         GetMonetaryValue();
    void         SetMonetaryValue(long lMonetaryValue);
    BYTE         IsNearby();
    void         GiveUsAPickUpObject(int ForcedObjectIndex = -1);
    void         GetRidOfObjects();
    void         Remove();
};
