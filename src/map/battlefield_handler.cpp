/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

  This file is part of DarkStar-server source code.

===========================================================================
*/

#include <string.h>
#include <algorithm>

#include "ai/states/death_state.h"

#include "alliance.h"

#include "battlefield.h"
#include "battlefield_handler.h"

#include "entities/charentity.h"
#include "entities/mobentity.h"

#include "lua/luautils.h"

#include "packets/char_recast.h"
#include "packets/char_skills.h"
#include "packets/message_basic.h"

#include "recast_container.h"
#include "status_effect.h"
#include "status_effect_container.h"

#include "utils/charutils.h"
#include "utils/zoneutils.h"

#include "zone.h"


CBattlefieldHandler::CBattlefieldHandler(CZone* PZone)
{
    m_PZone = PZone;
    m_MaxBattlefields = luautils::OnBattlefieldHandlerInitialise(PZone);
}

void CBattlefieldHandler::HandleBattlefields(time_point tick)
{
    auto check = [](auto& battlefield) {return battlefield->GetStatus() == BATTLEFIELD_STATUS_LOST || battlefield->GetStatus() == BATTLEFIELD_STATUS_WON;};
    for (auto& PBattlefield : m_Battlefields)
    {
        luautils::OnBattlefieldTick(PBattlefield.get());
    };

    m_Battlefields.erase(std::remove_if(m_Battlefields.begin(), m_Battlefields.end(), check), m_Battlefields.end());
}

CBattlefield* CBattlefieldHandler::LoadBattlefield(CCharEntity* PChar, uint16 battlefield)
{
    for (auto i = 0; i < m_MaxBattlefields; ++i)
    {
        auto area = i + 1;
        if (m_Battlefields[i]->GetArea() != area)
        {
            const int8* fmtQuery = "SELECT name, battlefieldId, fastestName, fastestTime, timeLimit, levelCap, lootDropId, rules, partySize, zoneId \
						    FROM battlefield_info \
							WHERE battlefieldId = %u";

            int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield);

            if (ret == SQL_ERROR ||
                Sql_NumRows(SqlHandle) == 0 ||
                Sql_NextRow(SqlHandle) != SQL_SUCCESS)
            {
                ShowError("Cannot load battlefield : %u \n", battlefield);
                return nullptr;
            }
            else
            {
                std::unique_ptr<CBattlefield> PBattlefield(new CBattlefield(battlefield, m_PZone, area, PChar));

                PBattlefield->SetName(Sql_GetData(SqlHandle, 0));
                PBattlefield->SetCurrentRecord(Sql_GetData(SqlHandle, 2), std::chrono::seconds(Sql_GetUIntData(SqlHandle, 3)));
                PBattlefield->SetTimeLimit(std::chrono::seconds(Sql_GetUIntData(SqlHandle, 4)));
                PBattlefield->SetLevelCap(Sql_GetUIntData(SqlHandle, 5));
                PBattlefield->SetLootID(Sql_GetUIntData(SqlHandle, 6));
                PBattlefield->SetMaxParticipants(Sql_GetUIntData(SqlHandle, 8));
                PBattlefield->SetRuleMask((uint16)Sql_GetUIntData(SqlHandle, 7));

                m_Battlefields.push_back(std::move(PBattlefield));
                return PBattlefield.get();
            }
        }
    }
    return nullptr;
}

CBattlefield* CBattlefieldHandler::GetBattlefield(CCharEntity* PChar)
{
    for (auto& PBattlefield : m_Battlefields)
    {
        PBattlefield->ForEachPlayer([PChar, &PBattlefield](CCharEntity* PPlayer)
        {
            if (PPlayer == PChar)
            {
                return PBattlefield.get();
            }
        });
    }
    return nullptr;
}

CBattlefield* CBattlefieldHandler::EnterBattlefield(CCharEntity* PChar, uint16 battlefield, uint8 area)
{
    bool exists = false;
    // attempt to add to an existing battlefield

    auto PBattlefield = GetBattlefield(PChar);

    if (PBattlefield->GetID() == battlefield && PBattlefield->GetArea() == area)
    {
        if (RemoveFromBattlefield(PChar, PBattlefield))
            return nullptr;

        if (!PBattlefield->InProgress() && (exists = PBattlefield->IsOccupied()))
        {
            PBattlefield->InsertEntity(PChar);
            return PBattlefield;
        }
    }

    return exists ? nullptr : LoadBattlefield(PChar, battlefield);
}

bool CBattlefieldHandler::RemoveFromBattlefield(CCharEntity* PChar, CBattlefield* PBattlefield, uint8 leavecode)
{
    if (!PBattlefield)
    {
        PBattlefield = GetBattlefield(PChar);
    }

    // just in case
    DSP_DEBUG_BREAK_IF(PBattlefield == nullptr);

    return PBattlefield->RemoveEntity(PChar, leavecode);
}