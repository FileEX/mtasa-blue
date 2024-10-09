/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/gamesa_defs.h
 *  PURPOSE:     Global game module header with defs
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CPedSAInterface;
class CEntitySAInterface;
class CPlayerPedSAInterface;
class CPedClothesDesc;
class CPedWeaponAudioEntitySAInterface;
class CVehicleSAInterface;
class CVector;

/////////////////////////////
/// CPed
/////////////////////////////
static auto FUNC_SetModelIndex = reinterpret_cast<void(__thiscall*)(CPedSAInterface* ped, int index)>(0x5E4880);
static auto FUNC_AttachPedToEntity = reinterpret_cast<void(__thiscall*)(CPedSAInterface* ped, CEntitySAInterface* entity, float offsetX, float offsetY, float offsetZ, int direction, float rotationLimit, std::uint32_t weaponType)>(0x5E7CB0);
static auto FUNC_SetIsStanding = reinterpret_cast<void(__thiscall*)(CPedSAInterface* ped, bool standing)>(0x4ABBE0);
static auto FUNC_SetCurrentWeapon = reinterpret_cast<void(__thiscall*)(CPedSAInterface* ped, std::uint8_t slot)>(0x5E61F0);
static auto FUNC_GiveWeapon = reinterpret_cast<std::uint32_t(__thiscall*)(CPedSAInterface* ped, std::uint32_t weaponID, int ammo, int unusedUnknown)>(0x5E6080);
static auto FUNC_GetBonePosition = reinterpret_cast<void(__thiscall*)(CPedSAInterface* ped, CVector* pos, std::uint32_t boneID, bool dynamic)>(0x5E4280);

/////////////////////////////
/// CPlayerPed
/////////////////////////////
static auto FUNC_MakeChangesForNewWeapon_Slot = reinterpret_cast<void(__thiscall*)(CPlayerPedSAInterface* playerPed, std::uint8_t weaponSlot)>(0x60D000);

/////////////////////////////
/// CPedClothesDesc
/////////////////////////////
static auto FUNC_CPedClothesDesc__SetTextureAndModel = reinterpret_cast<void(__fastcall*)(CPedClothesDesc* clothesDesc, int texture, const char* textureName, const char* modelName /*, eClothesTexturePart bodyPart*/)>(0x5A8080);

/////////////////////////////
/// CClothes
/////////////////////////////
static auto FUNC_CClothes__RebuildPlayer = reinterpret_cast<void(__cdecl*)(CPedSAInterface* ped, bool ignoreFatAndMuscle)>(0x5A82C0);

/////////////////////////////
/// CAEPedWeaponAudioEntity
/////////////////////////////
static auto FUNC_CAEPedWeaponAudioEntity__AddAudioEvent = reinterpret_cast<void(__thiscall*)(CPedWeaponAudioEntitySAInterface* pedAudioEntity, int eventID)>(0x4E69F0);

/////////////////////////////
/// CGameLogic
/////////////////////////////
static auto FUNC_RestorePlayerStuffDuringResurrection = reinterpret_cast<void(__cdecl*)(CEntitySAInterface* playerPed, float posX, float posY, float posZ, float startHeading)>(0x442060);

/////////////////////////////
/// CCarEnterExit
/////////////////////////////
static auto FUNC_QuitEnteringCar = reinterpret_cast<void(__cdecl*)(CPedSAInterface* ped, CVehicleSAInterface* vehicle, int doorID, bool wasBeingJacked)>(0x650130);
