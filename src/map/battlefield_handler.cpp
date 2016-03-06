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
    auto check = [](auto& battlefield) {return battlefield->CanCleanup();};
    for (auto& PBattlefield : m_Battlefields)
    {
        luautils::OnBattlefieldTick(PBattlefield.get());
    };

    m_Battlefields.erase(std::remove_if(m_Battlefields.begin(), m_Battlefields.end(), check), m_Battlefields.end());
}

CBattlefield* CBattlefieldHandler::LoadBattlefield(CCharEntity* PChar, uint16 battlefield)
{
    if (m_Battlefields.size() < m_MaxBattlefields)
    {
        auto area = 1;

        // todo: this is horrible, find another way to set the area number
        std::vector<uint8> areas;
        for (auto& PBattlefield : m_Battlefields)
            areas.push_back(PBattlefield->GetArea());

        std::sort(areas.begin(), areas.end());
        area = areas[areas.size() - 1] + 1;

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
            auto PEffect = PChar->StatusEffectContainer->GetStatusEffect(EFFECT_BATTLEFIELD);
            PEffect->SetSubPower(area);
            std::unique_ptr<CBattlefield> PBattlefield = std::make_unique<CBattlefield>(battlefield, m_PZone, area, PChar);

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
    return nullptr;
}

CBattlefield* CBattlefieldHandler::GetBattlefield(CBaseEntity* PEntity)
{
    for (auto& PBattlefield : m_Battlefields)
    {
        if (PBattlefield == PEntity->PBattlefield || PBattlefield->GetEntity(PEntity))
            return PBattlefield.get();
    }
    return nullptr;
}

CBattlefield* CBattlefieldHandler::RegisterBattlefield(CCharEntity* PChar, uint16 battlefield, uint8 area)
{
    bool exists = false;

    // attempt to add to an existing battlefield
    auto PBattlefield = GetBattlefield(PChar);

    // assume relogging, remove entity from battlefield
    if (RemoveFromBattlefield(PChar, PBattlefield))
        return nullptr;

    if (PBattlefield->GetID() == battlefield && PBattlefield->GetArea() == area)
    {
        if (!PBattlefield->InProgress() && (exists = PBattlefield->IsOccupied()))
        {
            PBattlefield->InsertEntity(PChar);
            return PBattlefield;
        }
    }

    return exists ? nullptr : LoadBattlefield(PChar, battlefield);
}

bool CBattlefieldHandler::RemoveFromBattlefield(CBaseEntity* PEntity, CBattlefield* PBattlefield, uint8 leavecode)
{
    // would only be true for pets and players
    if (!PBattlefield)
    {
        PBattlefield = GetBattlefield(PEntity);
    }

    // idek why this has a return type
    DSP_DEBUG_BREAK_IF(PBattlefield == nullptr);

    return PBattlefield->RemoveEntity(PEntity, leavecode);
}