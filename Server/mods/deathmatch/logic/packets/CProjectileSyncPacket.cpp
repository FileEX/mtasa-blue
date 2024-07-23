/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CProjectileSyncPacket.cpp
 *  PURPOSE:     Projectile sync packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileSyncPacket.h"

CProjectileSyncPacket::~CProjectileSyncPacket()
{
    for (auto iter = IterBegin(); iter != IterEnd(); ++iter)
    {
        delete *iter;
    }
    m_Syncs.clear();
}

bool CProjectileSyncPacket::Read(NetBitStreamInterface& BitStream)
{
    while (BitStream.GetNumberOfUnreadBits() > 32)
    {
        SyncData Data;

        // Read out the ID
        if (!BitStream.Read(Data.ID))
            return false;

        // Read the sync time context
        if (!BitStream.Read(Data.syncTimeContext))
            return false;

        std::uint8_t flags = 0;
        if (!BitStream.Read(flags))
            return false;
        Data.flags = flags;

        // Read out the position
        if (flags & 0x1)
        {
            SPositionSync position;
            if (!BitStream.Read(&position))
                return false;

            Data.position = position.data.vecPosition;
        }

        // Read out the rotation
        if (flags & 0x2)
        {
            SRotationRadiansSync rotation;
            if (!BitStream.Read(&rotation))
                return false;

            Data.rotation = rotation.data.vecRotation;
        }

        // Read out the velocity
        if (flags & 0x3)
        {
            SVelocitySync velocity;
            if (!BitStream.Read(&velocity))
                return false;

            Data.velocity = velocity.data.vecVelocity;
        }

        // Add it to list
        m_Syncs.push_back(&Data);
    }

    return m_Syncs.size() > 0;
}

bool CProjectileSyncPacket::Write(NetBitStreamInterface& BitStream)
{
    for (auto iter = IterBegin(); iter != IterEnd(); ++iter)
    {
        SyncData* Data = *iter;

        // Write the ID
        BitStream.Write(Data->ID);

        // Write the sync time context
        BitStream.Write(Data->syncTimeContext);

        // Write flags
        BitStream.Write(Data->flags);

        // Write position
        if (Data->flags & 0x1)
        {
            SPositionSync position;
            position.data.vecPosition = Data->position;

            BitStream.Write(&position);
        }

        // Write rotaion
        if (Data->flags & 0x2)
        {
            SRotationRadiansSync rotation;
            rotation.data.vecRotation = Data->rotation;

            BitStream.Write(&rotation);
        }

        // Write velocity
        if (Data->flags & 0x3)
        {
            SVelocitySync velocity;
            velocity.data.vecVelocity = Data->velocity;

            BitStream.Write(&velocity);
        }
    }

    return true;
}
