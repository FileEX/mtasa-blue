/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CObjectSA.cpp
 *  PURPOSE:     Object entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CObjectSA.h"
#include "CPoolsSA.h"
#include "CRopesSA.h"
#include "CWorldSA.h"
#include "CFireManagerSA.h"
#include "CColModelSA.h"

extern CGameSA* pGame;

std::unordered_map<std::uint32_t, CColModelSAInterface*> originalCollisions{};

static void CObject_PreRender(CObjectSAInterface* objectInterface)
{
    SClientEntity<CObjectSA>* objectEntity = pGame->GetPools()->GetObject((DWORD*)objectInterface);
    if (objectEntity && objectEntity->pEntity)
        objectEntity->pEntity->SetPreRenderRequired(true);
}

const std::uintptr_t RETURN_CCObject_PreRender = 0x59FD56;
static void __declspec(naked) HOOK_CCObject_PreRender()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        push    ecx
        call    CObject_PreRender
        pop     ecx
        sub     esp, 16
        push    esi
        mov     esi, ecx
        jmp     RETURN_CCObject_PreRender
    }
}

static void ReplaceCol(CEntitySAInterface* entity)
{
    if (!entity)
        return;

    if (entity->nType == 4)
    {
        auto object = pGame->GetPools()->GetObject((DWORD*)entity);
        if (object && object->pEntity)
        {
            CModelInfo* modelInfo = pGame->GetModelInfo(entity->m_nModelIndex);
            if (modelInfo && object->pEntity->GetCustomCol())
            {
                modelInfo->GetInterface()->pColModel = object->pEntity->GetCustomCol();
            }
        }
    }
}

static void RestoreCol(CEntitySAInterface* entity)
{
    if (!entity || entity->nType != 4)
        return;

    if (MapFind(originalCollisions, entity->m_nModelIndex))
    {
        CModelInfo* modelInfo = pGame->GetModelInfo(entity->m_nModelIndex);
        if (modelInfo)
        {
            modelInfo->GetInterface()->pColModel = MapGet(originalCollisions, entity->m_nModelIndex);
        }
    }
}

static std::uintptr_t         ProcessShift_BACK = 0x568908 + 7;
static void __declspec(naked) HOOK_CWrold_ProcessShift()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE

    _asm
    {
        push esi

        push esi
        call ReplaceCol
        add esp, 4

        mov eax, [esi]
        mov ecx, esi
        call dword ptr [eax+30h]

        push eax

        push esi
        call RestoreCol
        add esp, 4

        pop eax
        pop esi
        jmp ProcessShift_BACK
    }
}

static std::uintptr_t        ProcessCollision_BACK = 0x568829 + 5 + 3;
static void _declspec(naked) HOOK_CWorld_ProcessCollision()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE

    _asm
    {
        mov ecx, esi
        mov [esi+1Ch], eax

        push esi
        push edx

        push esi
        call ReplaceCol
        add esp, 4

        pop edx

        call dword ptr [edx+2Ch]

        push eax

        push esi
        call RestoreCol
        add esp, 4

        pop eax
        pop esi
        jmp ProcessCollision_BACK
    }
}

static constexpr std::uintptr_t ProcessCollision_BACK2 = 0x5687CF + 7;

static void _declspec(naked) HOOK_CWorld_ProcessCollision2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE

     _asm
    {
        mov edx, [esi]
        mov ecx, esi

        push esi
        push edx

        push esi
        call ReplaceCol
        add esp, 4

        pop edx
        call dword ptr [edx+2Ch]

        push esi
        call RestoreCol
        add esp, 4

        pop esi
        jmp ProcessCollision_BACK2
    }
}

static std::uintptr_t        ProcessCollision_BACK3 = 0x56876F + 7;
static void _declspec(naked) HOOK_CWorld_ProcessCollision3()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE

    _asm
    {
        mov edx, [esi]

        push esi
        push edx

        push esi
        call ReplaceCol
        add esp, 4

        pop edx
        mov ecx, esi
        call dword ptr [edx+2Ch]

        push esi
        call RestoreCol
        add esp, 4

        pop esi
        jmp ProcessCollision_BACK3
    }
}

void CObjectSA::StaticSetHooks()
{
    // Patch CObject::PreRender. We don't want the scaling code to execute
    // We'll scale the object entity matrix after onClientPedsProcessed event
    // 5E       - pop asi
    // 83 C4 10 - add esp, 0x10
    // C3       - ret
    std::uint8_t bytes[5] = {0x5E, 0x83, 0xC4, 0x10, 0xC3};
    MemCpy((void*)0x59FE0E, bytes, sizeof(bytes));
    HookInstall(0x59FD50, HOOK_CCObject_PreRender);

    HookInstall(0x568908, HOOK_CWrold_ProcessShift, 7);
    HookInstall(0x568829, HOOK_CWorld_ProcessCollision, 8);
    HookInstall(0x5687CF, HOOK_CWorld_ProcessCollision2, 7);
    HookInstall(0x56876F, HOOK_CWorld_ProcessCollision3, 7);
}

// GTA uses this to pass to CFileLoader::LoadObjectInstance the info it wants to load
struct CFileObjectInstance
{
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float rz;
    float rr;            // = 1
    DWORD modelId;
    DWORD areaNumber;
    long  flags;            // = -1
};

CObjectSA::CObjectSA(CObjectSAInterface* objectInterface)
{
    SetInterface(objectInterface);
    m_ucAlpha = 255;

    // Setup some flags
    BeingDeleted = false;
    DoNotRemoveFromGame = false;

    if (m_pInterface)
    {
        ResetScale();
        CheckForGangTag();
        m_pInterface->bStreamingDontDelete = true;
    }
}

CObjectSA::CObjectSA(DWORD dwModel, bool bBreakingDisabled)
{
    DWORD CObjectCreate = FUNC_CObject_Create;
    DWORD dwObjectPtr = 0;
    __asm
    {
        push    1
        push    dwModel
        call    CObjectCreate
        add     esp, 8
        mov     dwObjectPtr, eax
    }

    if (dwObjectPtr)
    {
        SetInterface((CEntitySAInterface*)dwObjectPtr);

        CWorldSA* world = (CWorldSA*)pGame->GetWorld();
        world->Add(m_pInterface, CObject_Constructor);

        // Setup some flags
        BeingDeleted = false;
        DoNotRemoveFromGame = false;
        MemPutFast<BYTE>(dwObjectPtr + 316, 6);
        if (bBreakingDisabled)
        {
            // Set our immunities
            // Sum of all flags checked @ CPhysical__CanPhysicalBeDamaged
            CObjectSAInterface* pObjectSAInterface = GetObjectInterface();
            pObjectSAInterface->bBulletProof = true;
            pObjectSAInterface->bFireProof = true;
            pObjectSAInterface->bCollisionProof = true;
            pObjectSAInterface->bMeeleProof = true;
            pObjectSAInterface->bExplosionProof = true;
        }
        m_pInterface->bStreamingDontDelete = true;
    }
    else
    {
        // The exception handler doesn't work for some reason, so do this
        SetInterface(nullptr);
    }

    m_ucAlpha = 255;

    if (m_pInterface)
    {
        ResetScale();
        CheckForGangTag();
    }
}

CObjectSA::~CObjectSA()
{
    if (!BeingDeleted && DoNotRemoveFromGame == false)
    {
        CEntitySAInterface* pInterface = GetInterface();
        if (pInterface)
        {
            pGame->GetRopes()->RemoveEntityRope(pInterface);

            if (!pInterface->IsPlaceableVTBL())
            {
                CWorldSA* world = (CWorldSA*)pGame->GetWorld();
                world->Remove(pInterface, CObject_Destructor);

                pInterface->Destructor(true);
            }
        }

        BeingDeleted = true;
        ((CPoolsSA*)pGame->GetPools())->RemoveObject((CObject*)(CObjectSA*)this);
    }
}

void CObjectSA::Explode()
{
    DWORD dwFunc = FUNC_CObject_Explode;
    DWORD dwThis = (DWORD)GetInterface();

    __asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CObjectSA::Break()
{
    DWORD dwFunc = 0x5A0D90;
    DWORD dwThis = (DWORD)GetInterface();

    float fHitVelocity = 1000.0f;            // has no direct influence, but should be high enough to trigger the break (effect)

    __asm
    {
        push    32h // most cases: between 30 and 37
        push    0 // colliding entity. To ignore it, we can set it to 0
        push    0B73710h // vecCollisionImpactVelocity
        push    0 // vecCollisionLastPos
        push    fHitVelocity
        mov     ecx, dwThis
        call    dwFunc
    }

    if (IsGlass())
    {
        float fX = 0.0f;
        float fY = 0.0f;
        float fZ = 0.0f;
        dwFunc = FUNC_CGlass_WindowRespondsToCollision;

        __asm
        {
            push 0
            push fZ
            push fY
            push fX
            push 0
            push 0
            push 0
            push fHitVelocity
            push dwThis
            call dwFunc
            add esp, 24h
        }
    }
}

void CObjectSA::SetHealth(float fHealth)
{
    static_cast<CObjectSAInterface*>(GetInterface())->fHealth = fHealth;
}

float CObjectSA::GetHealth()
{
    return static_cast<CObjectSAInterface*>(GetInterface())->fHealth;
}

void CObjectSA::SetModelIndex(unsigned long ulModel)
{
    // Delete any existing RwObject first
    GetInterface()->DeleteRwObject();

    // Jax: I'm not sure if using the virtual method is right (as ped and vehicle dont), but it works
    GetInterface()->SetModelIndex(ulModel);

    CheckForGangTag();
}

void CObjectSA::CheckForGangTag()
{
    switch (GetModelIndex())
    {
        case 1524:
        case 1525:
        case 1526:
        case 1527:
        case 1528:
        case 1529:
        case 1530:
        case 1531:
            m_bIsAGangTag = true;
            break;
        default:
            m_bIsAGangTag = false;
            break;
    }
}

bool CObjectSA::IsGlass()
{
    DWORD dwFunc = 0x46A760;
    DWORD dwThis = (DWORD)GetInterface();
    bool  bResult;

    __asm
    {
        push dwThis
        call dwFunc
        mov bResult, al
        add esp, 4
    }
    return bResult;
}

static inline float VecLength(const CVector& v)
{
    return std::sqrt(v.fX * v.fX + v.fY * v.fY + v.fZ * v.fZ);
}

void CObjectSA::SetScale(float fX, float fY, float fZ)
{
    m_vecScale = CVector(fX, fY, fZ);
    GetObjectInterface()->bUpdateScale = true;
    GetObjectInterface()->fScale = std::max(fX, std::max(fY, fZ));

    auto modelInfo = pGame->GetModelInfo(GetModelIndex());
    if (!modelInfo) return;

    auto* interf = modelInfo->GetInterface();
    CColModelSAInterface* newCollision = interf->pColModel;

    if (!MapFind(originalCollisions, GetModelIndex()))
    {
        if (!interf->pColModel || !interf->pColModel->m_data)
            return;

        MapSet(originalCollisions, GetModelIndex(), interf->pColModel);

        newCollision = new CColModelSAInterface();
        ((void(__thiscall*)(CColModelSAInterface*, int, int, int, int, int, bool))0x40F870)(newCollision, interf->pColModel->m_data->m_numSpheres, interf->pColModel->m_data->m_numBoxes, interf->pColModel->m_data->m_numSuspensionLines, interf->pColModel->m_data->m_numShadowVertices, interf->pColModel->m_data->m_numTriangles, interf->pColModel->m_data->m_usesDisks);

        interf->pColModel = newCollision;
        m_customCol = newCollision;
    }

    CColModelSAInterface* defaultCollision = MapGet(originalCollisions, GetModelIndex());

    newCollision->m_bounds.m_vecMax.fX = defaultCollision->m_bounds.m_vecMax.fX * fX;
    newCollision->m_bounds.m_vecMax.fY = defaultCollision->m_bounds.m_vecMax.fY * fY;
    newCollision->m_bounds.m_vecMax.fZ = defaultCollision->m_bounds.m_vecMax.fZ * fZ;

    newCollision->m_bounds.m_vecMin.fX = defaultCollision->m_bounds.m_vecMin.fX * fX;
    newCollision->m_bounds.m_vecMin.fY = defaultCollision->m_bounds.m_vecMin.fY * fY;
    newCollision->m_bounds.m_vecMin.fZ = defaultCollision->m_bounds.m_vecMin.fZ * fZ;

    bool isUniformScale = (fX == fY == fZ);

    CVector scaledMin = newCollision->m_bounds.m_vecMin;
    CVector scaledMax = newCollision->m_bounds.m_vecMax;

    CVector size = CVector(scaledMax.fX - scaledMin.fX, scaledMax.fY - scaledMin.fY, scaledMax.fZ - scaledMin.fZ);

    // --- Œrodek sfery = œrodek AABB ---
    newCollision->m_sphere.m_center = CVector((scaledMin.fX + scaledMax.fX) * 0.5f, (scaledMin.fY + scaledMax.fY) * 0.5f, (scaledMin.fZ + scaledMax.fZ) * 0.5f);

    // --- Promieñ sfery = po³owa najd³u¿szego wymiaru ---
    CVector diff;
    diff.fX = scaledMax.fX - scaledMin.fX;
    diff.fY = scaledMax.fY - scaledMin.fY;
    diff.fZ = scaledMax.fZ - scaledMin.fZ;
    newCollision->m_sphere.m_radius = 0.5f * VecLength(diff);

    CColDataSA* data = newCollision->m_data;
    CColDataSA* originalData = defaultCollision->m_data;
    if (data)
    {
        for (int i = 0; i < data->m_numBoxes; i++)
        {
            data->m_boxes[i].m_vecMax.fX = originalData->m_boxes[i].m_vecMax.fX * fX;
            data->m_boxes[i].m_vecMax.fY = originalData->m_boxes[i].m_vecMax.fY * fY;
            data->m_boxes[i].m_vecMax.fZ = originalData->m_boxes[i].m_vecMax.fZ * fZ;

            data->m_boxes[i].m_vecMin.fX = originalData->m_boxes[i].m_vecMin.fX * fX;
            data->m_boxes[i].m_vecMin.fY = originalData->m_boxes[i].m_vecMin.fY * fY;
            data->m_boxes[i].m_vecMin.fZ = originalData->m_boxes[i].m_vecMin.fZ * fZ;
        }
    }

    pGame->GetWorld()->Remove(m_pInterface, eDebugCaller::CObject_Destructor);
    pGame->GetWorld()->Add(m_pInterface, eDebugCaller::CObject_Constructor);
}

CVector* CObjectSA::GetScale()
{
    return &m_vecScale;
}

void CObjectSA::ResetScale()
{
    SetScale(1.0f, 1.0f, 1.0f);
}

bool CObjectSA::SetOnFire(bool onFire)
{
    CObjectSAInterface* objectInterface = GetObjectInterface();
    if (onFire == !!objectInterface->pFire)
        return false;

    auto* fireManager = static_cast<CFireManagerSA*>(pGame->GetFireManager());

    if (onFire)
    {
        CFire* fire = fireManager->StartFire(this, nullptr, static_cast<float>(DEFAULT_FIRE_PARTICLE_SIZE));
        if (!fire)
            return false;

        fire->SetTarget(this);
        fire->SetStrength(1.0f);
        fire->Ignite();
        fire->SetNumGenerationsAllowed(0);

        objectInterface->pFire = fire->GetInterface();
    }
    else
    {
        CFire* fire = fireManager->GetFire(objectInterface->pFire);
        if (!fire)
            return false;

        fire->Extinguish();
    }

    return true;
}
