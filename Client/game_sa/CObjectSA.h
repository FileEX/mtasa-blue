/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectSA.h
 *  PURPOSE:     Header file for object entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CObject.h>
#include "CPhysicalSA.h"
#include "CColPointSA.h"

class CFireSAInterface;

#define FUNC_CObject_Create             0x5A1F60
#define FUNC_CObject_Explode            0x5A1340
#define FUNC_CGlass_WindowRespondsToCollision 0x71BC40

class CDummyObjectSAInterface : public CEntitySAInterface
{
    // Empty
};

class CObjectInfo
{
public:
    float         fMass;
    float         fTurnMass;
    float         fAirResistance;
    float         fElasticity;
    float         fBuoyancy;
    float         fUprootLimit;
    float         fColDamageMultiplier;
    std::uint8_t  ucColDamageEffect;
    std::uint8_t  ucSpecialColResponseCase;
    std::uint8_t  ucCameraAvoidObject;
    std::uint8_t  ucCausesExplosion;
    std::uint8_t  ucFxType;
    std::uint8_t  pad1[3];
    CVector       vecFxOffset;
    void*         pFxSystem;            // from CParticleData::GetDataFromName FxSystemBP
    float         fSmashMultiplier;
    CVector       vecBreakVelocity;
    float         fBreakVelocityRand;
    std::uint32_t uiGunBreakMode;
    std::uint32_t uiSparksOnImpact;
};
static_assert(sizeof(CObjectInfo) == 0x50, "Invalid size for CObjectInfo");

class CObjectSAInterface : public CPhysicalSAInterface
{
public:
    void*         pObjectList;
    eObjectType   objectType;
    std::uint8_t  bonusValue;
    std::uint16_t costValue;

    union
    {
        struct
        {
            std::uint32_t isPickup : 1;
            std::uint32_t unknown_0x2 : 1;            // Using in 0x475C6B, probably something related to collisions
            std::uint32_t pickupPropertyForSale : 1;
            std::uint32_t pickupInShopOutOfStock : 1;
            std::uint32_t hasBrokenGlass : 1;
            std::uint32_t glassBrokenAllTogether : 1;
            std::uint32_t isExploded : 1;
            std::uint32_t changesVehColor : 1;

            std::uint32_t isLampPost : 1;
            std::uint32_t isTargetable : 1;
            std::uint32_t isBroken : 1;
            std::uint32_t trainCrossEnabled : 1;            // Train crossing will be opened if flag is set (distance < 120.0f)
            std::uint32_t isPhotographed : 1;
            std::uint32_t isLiftable : 1;
            std::uint32_t isDoorMoving : 1;
            std::uint32_t isDoorOpen : 1;

            std::uint32_t hasNoModel : 1;
            std::uint32_t isScaled : 1;
            std::uint32_t canBeAttachedToMagnet : 1;
            std::uint32_t isDamaged : 1;
            std::uint32_t unknown_scripts : 2;            // something for scripts
            std::uint32_t fadingIn : 1;                   // works only for objects with type MISSION_OBJECT
            std::uint32_t affectedByColBrightness : 1;

            std::uint32_t enableDisabledAttractors : 1;
            std::uint32_t doNotRender : 1;
            std::uint32_t fadingIn2 : 1;
            std::uint32_t unknown_b4_3 : 1;
            std::uint32_t unknown_b4_4 : 1;
            std::uint32_t unknown_b4_5 : 1;
            std::uint32_t unknown_b4_6 : 1;
            std::uint32_t unknown_b4_7 : 1;
        };

        std::uint32_t flags;
    };

    eObjectColDamageEffect         colDamageEffect;
    eObjectSpecialColResponseCases specialColResponseCase;
    std::uint8_t                   field_146;
    std::int8_t                    garageDoorGarageIndex;
    std::uint8_t                   lastWeaponDamage;
    CColLighting                   colBrightness;
    std::uint16_t                  carPartModelIndex;
    std::uint8_t                   carColors[4];            // used for detached car parts
    std::uint32_t                  removalTime;
    float                          health;
    float                          doorStartAngle;            // used for door objects
    float                          scale;
    CObjectInfo*                   objectInfo;
    CFireSAInterface*              fire;
    std::int16_t                   scriptTriggerIndex;
    std::int16_t                   remapTxd;                     // used for detached car parts
    RwTexture*                     remapTexture;                 // used for detached car parts
    CDummyObjectSAInterface*       linkedObjectDummy;            // used for dynamic objects like garage doors, train crossings etc.
    std::uint32_t                  timeToRemoveParticles;
    float                          particlesIntensity;
};
static_assert(sizeof(CObjectSAInterface) == 0x17C, "Invalid size for CObjectSAInterface");

class CCutsceneObjectSAInterface : public CObjectSAInterface
{
    union
    {
        RwFrame*      attachToFrame;
        std::uint32_t attachToBone;            //  this one if attachToObject != 0
    };

    CObjectSAInterface* attachToObject;
    CVector             worldPosition;
    CVector             force;
};
static_assert(sizeof(CCutsceneObjectSAInterface) == 0x19C, "Invalid size for CCutsceneObjectSAInterface");

class CHandObjectSAInterface : public CObjectSAInterface
{
    CPedSAInterface* ped;
    std::uint32_t    boneIndex;
    RwTexture*       texture;
    bool             updateMarticesArray;
    std::uint8_t     field_189[3];
};
static_assert(sizeof(CHandObjectSAInterface) == 0x18C, "Invalid size for CHandObjectSAInterface");

class CObjectSA : public virtual CObject, public virtual CPhysicalSA
{
private:
    unsigned char m_ucAlpha;
    bool          m_bIsAGangTag;
    CVector       m_vecScale;
    bool          m_preRenderRequired = false;

public:
    static void StaticSetHooks();

    CObjectSA(CObjectSAInterface* objectInterface);
    CObjectSA(DWORD dwModel, bool bBreakingDisabled);
    ~CObjectSA();

    CObjectSAInterface* GetObjectInterface() { return (CObjectSAInterface*)GetInterface(); }

    void  Explode();
    void  Break();
    void  SetHealth(float fHealth);
    float GetHealth();
    void  SetModelIndex(unsigned long ulModel);

    void          SetPreRenderRequired(bool required) { m_preRenderRequired = required; }
    bool          GetPreRenderRequired() { return m_preRenderRequired; }
    void          SetAlpha(unsigned char ucAlpha) { m_ucAlpha = ucAlpha; }
    unsigned char GetAlpha() { return m_ucAlpha; }

    bool IsAGangTag() const { return m_bIsAGangTag; }
    bool IsGlass();

    void     SetScale(float fX, float fY, float fZ);
    CVector* GetScale();
    void     ResetScale();

private:
    void CheckForGangTag();
};
