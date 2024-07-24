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
    while (BitStream.GetNumberOfUnreadBits() > 5)
    {
        SyncData* Data = new SyncData;

        // Read out the ID
        if (!BitStream.Read(Data->ID))
            return false;

        // Read the sync time context
        if (!BitStream.Read(Data->syncTimeContext))
            return false;

        std::uint8_t flags = 0;
        if (!BitStream.Read(flags))
            return false;
        Data->flags = flags;

        // Read out the position
        if (flags & 0x1)
        {
            SPositionSync position;
            if (!BitStream.Read(&position))
                return false;

            Data->position = position;
        }

        // Read out the rotation
        if (flags & 0x2)
        {
            SRotationRadiansSync rotation;
            if (!BitStream.Read(&rotation))
                return false;

            Data->rotation = rotation;
        }

        // Read out the velocity
        if (flags & 0x3)
        {
            SVelocitySync velocity;
            if (!BitStream.Read(&velocity))
                return false;

            Data->velocity = velocity;
        }

        // Add it to list
        m_Syncs.push_back(Data);
    }

    return m_Syncs.size() > 0;
}

bool CProjectileSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    bool sent = false;

    for (auto iter = m_Syncs.begin(); iter != m_Syncs.end(); ++iter)
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
            SPositionSync position = Data->position;
            BitStream.Write(&position);
        }

        // Write rotaion
        if (Data->flags & 0x2)
        {
            SRotationRadiansSync rotation = Data->rotation;
            BitStream.Write(&rotation);
        }

        // Write velocity
        if (Data->flags & 0x3)
        {
            SVelocitySync velocity = Data->velocity;
            BitStream.Write(&velocity);
        }

        sent = true;
    }

    return sent;
}
