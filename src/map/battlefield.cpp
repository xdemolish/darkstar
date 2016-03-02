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

#include "battlefield.h"

#include "../common/timer.h"

#include "ai/ai_container.h"
#include "ai/states/death_state.h"

#include "enmity_container.h"

#include "entities/baseentity.h"
#include "entities/charentity.h"
#include "entities/mobentity.h"
#include "entities/npcentity.h"

#include "lua/luautils.h"

#include "packets/entity_animation.h"
#include "packets/entity_update.h"
#include "packets/message_basic.h"
#include "packets/position.h"

#include "status_effect_container.h"
#include "treasure_pool.h"

#include "utils/itemutils.h"
#include "utils/zoneutils.h"
#include "zone.h"

CBattlefield::CBattlefield(uint16 id, CZone* PZone, uint8 area, CCharEntity* PInitiator)
{
    m_ID = id;
    m_PZone = PZone;
    m_Area = area;
    m_Initiator.id = PInitiator->id;
    m_Initiator.name = PInitiator->name;

    InsertEntity(PInitiator);

    m_StartTime = server_clock::now();
}

CBattlefield::~CBattlefield()
{
    Cleanup();
}

uint16 CBattlefield::GetID()
{
    return m_ID;
}

CZone* CBattlefield::GetZone()
{
    return m_PZone;
}

uint16 CBattlefield::GetZoneID()
{
    return m_PZone->GetID();
}

string_t CBattlefield::GetName()
{
    return m_Name;
}

BattlefieldInitiator_t CBattlefield::GetInitiator()
{
    return m_Initiator;
}

uint8 CBattlefield::GetArea()
{
    return m_Area;
}

BattlefieldRecord_t CBattlefield::GetCurrentRecord()
{
    return m_CurrentRecord;
}

uint8 CBattlefield::GetStatus()
{
    return InProgress() ? BATTLEFIELD_STATUS_LOCKED : m_Status;
}

uint16 CBattlefield::GetRuleMask()
{
    return m_Rules;
}

time_point CBattlefield::GetStartTime()
{
    return m_StartTime;
}

duration CBattlefield::GetTimeInside()
{
    return m_Tick - m_StartTime;
}

duration CBattlefield::GetTimeLimit()
{
    return m_TimeLimit;
}

duration CBattlefield::GetAllDeadTime()
{
    return m_AllDeadTime;
}

duration CBattlefield::GetFinishTime()
{
    return m_FinishTime;
}


uint8 CBattlefield::GetMaxParticipants()
{
    return m_MaxParticipants;
}

uint8 CBattlefield::GetPlayerCount()
{
    return m_PlayerList.size();
}

uint8 CBattlefield::GetLevelCap()
{
    return m_LevelCap;
}

uint16 CBattlefield::GetLootID()
{
    return m_LootID;
}

void CBattlefield::SetName(int8* name)
{
    m_Name.clear();
    m_Name.insert(0, name);
}

void CBattlefield::SetInitiator(int8* name)
{
    m_Initiator.name.clear();
    m_Initiator.name.insert(0, name);
}

void CBattlefield::SetTimeLimit(duration time)
{
    m_TimeLimit = time;
}

void CBattlefield::SetAllDeadTime(duration time)
{
    m_AllDeadTime = time;
}

void CBattlefield::SetArea(uint8 area)
{
    m_Area = area;
}

void CBattlefield::SetCurrentRecord(int8* name, duration time)
{
    m_CurrentRecord.name = name;
    m_CurrentRecord.time = time;
}

void CBattlefield::SetStatus(uint8 status)
{
    m_Status = status;
}

void CBattlefield::SetRuleMask(uint16 rulemask)
{
    m_Rules = rulemask;
}

void CBattlefield::SetMaxParticipants(uint8 max)
{
    m_MaxParticipants = max;
}

void CBattlefield::SetLevelCap(uint8 cap)
{
    m_LevelCap = cap;
}

void CBattlefield::SetLootID(uint16 id)
{
    m_LootID = id;
}

void CBattlefield::ApplyLevelCap(CCharEntity* PChar)
{
    //adjust player's level to the appropriate cap and remove buffs

    //todo: find a better place to put this
    /*
    if (m_PlayerList.size() == 0)
    {
         ShowWarning("battlefield:GetPlayerMainLevel - No players in battlefield!\n");
         return;
    }
    uint8 cap = GetLevelCap();
    if (cap != 0)
    {	// Other missions lines and things like dragoon quest battle can be done similarly to CoP_Battle_cap.
        // Might be better to add a type flag to the sql to tell bcnm/isnm/which expantions mission than doing by bcnmID like this.
        if ((map_config.CoP_Battle_cap == 0) && (m_BcnmID == 768 || m_BcnmID == 800 || m_BcnmID == 832 || m_BcnmID == 960
            || m_BcnmID == 704 || m_BcnmID == 961 || m_BcnmID == 864 || m_BcnmID == 672 || m_BcnmID == 736 || m_BcnmID == 992 || m_BcnmID == 640))
        {
            cap = 99;
        }
        if (cap < 99 && cap > 1)
        {
            cap = cap + map_config.Battle_cap_tweak;
        }
        if (cap > 99)
        {
            cap = 99;
        }
        if (cap < 1)
        {
            cap = 1;
        }
        for (int i = 0; i < m_PlayerList.size(); i++)
        {
            m_PlayerList.at(i)->StatusEffectContainer->DelStatusEffectsByFlag(EFFECTFLAG_DISPELABLE);
            m_PlayerList.at(i)->StatusEffectContainer->DelStatusEffectsByFlag(EFFECTFLAG_ON_ZONE);
            m_PlayerList.at(i)->StatusEffectContainer->AddStatusEffect(new CStatusEffect(EFFECT_LEVEL_RESTRICTION, 0, cap, 0, 0), true);
        }
    }
    */
}

void CBattlefield::PushMessageToAllInBcnm(uint16 msg, uint16 param)
{
    // todo: handle this properly
    ForEachPlayer([msg, param](CCharEntity* PChar)
    {
        if (PChar->m_lastBcnmTimePrompt != param)
        {
            PChar->pushPacket(new CMessageBasicPacket(PChar, PChar, param, 0, msg));
            PChar->m_lastBcnmTimePrompt = param;
        }
    });
}

bool CBattlefield::AllPlayersDead()
{
    ForEachPlayer([](CCharEntity* PChar)
    {
        if (!PChar->PAI->IsCurrentState<CDeathState>())
            return false;
    });
    return true;
}

bool CBattlefield::AllEnemiesDefeated()
{
    for (auto mob : m_EnemyList)
    {
        if (mob.condition & CONDITION_WIN_REQUIREMENT && !mob.PMob->PAI->IsCurrentState<CDeathState>())
            return false;
    }
    return true;
}

bool CBattlefield::IsOccupied()
{
    return m_PlayerList.size() > 0;
}

bool CBattlefield::InsertEntity(CBaseEntity* PEntity, BCMOBCONDITIONS conditions)
{
    if (PEntity->objtype == TYPE_PC)
    {
        if (GetPlayerCount() < GetMaxParticipants())
            m_PlayerList.push_back(static_cast<CCharEntity*>(PEntity));
        else
            return false;
    }
    else if (PEntity->objtype == TYPE_NPC)
    {
        m_NpcList.push_back(static_cast<CNpcEntity*>(PEntity));
    }
    else if (PEntity->objtype == TYPE_MOB)
    {
        auto ally = PEntity->allegiance == ALLEGIANCE_PLAYER;

        // mobs
        if (!ally)
        {
            // only apply conditions to mobs spawning by default
            BattlefieldMob_t mob;
            mob.PMob = static_cast<CMobEntity*>(PEntity);
            mob.condition = conditions;
            m_EnemyList.push_back(mob);
        }
        // ally
        else
        {
            m_AllyList.push_back(static_cast<CMobEntity*>(PEntity));
        }
    }

    auto entity = dynamic_cast<CBattleEntity*>(PEntity);

    if (entity && !entity->StatusEffectContainer->GetStatusEffect(EFFECT_BATTLEFIELD))
        entity->StatusEffectContainer->AddStatusEffect(new CStatusEffect(EFFECT_BATTLEFIELD, EFFECT_BATTLEFIELD, this->GetID(),
            0, 0, this->GetArea()));

    PEntity->PBattlefield = this;
    return true;
}

bool CBattlefield::RemoveEntity(CBaseEntity* PEntity, uint8 leavecode)
{
    auto found = false;
    auto check = [PEntity, &found](auto entity) { if (PEntity == entity) { found = true; return found; } return false; };

    if (PEntity->objtype == TYPE_PC)
    {
        m_PlayerList.erase(std::remove_if(m_PlayerList.begin(), m_PlayerList.end(), check), m_PlayerList.end());

        luautils::OnBcnmLeave(static_cast<CCharEntity*>(PEntity), this, leavecode);
    }
    else if (PEntity->objtype == TYPE_NPC)
    {
        m_NpcList.erase(std::remove_if(m_NpcList.begin(), m_NpcList.end(), check), m_NpcList.end());
    }
    else if (PEntity->objtype == TYPE_MOB && PEntity->allegiance == ALLEGIANCE_PLAYER)
    {
        m_AllyList.erase(std::remove_if(m_AllyList.begin(), m_AllyList.end(), check), m_AllyList.end());
    }
    else
    {
        auto check = [PEntity, &found](auto entity) { if (entity.PMob == PEntity) { found = true; return found; } return false; };

        m_EnemyList.erase(std::remove_if(m_EnemyList.begin(), m_EnemyList.end(), check), m_EnemyList.end());
    }
    PEntity->PBattlefield = nullptr;
    return found;
}

void CBattlefield::Cleanup()
{
    // wipe enmity from all mobs in list if needed
    ForEachEnemy([](CMobEntity* PMob)
    {
        PMob->PAI->Despawn();
        PMob->status = STATUS_DISAPPEAR;
        PMob->PBattlefield = nullptr;
    });
    // wipe mob list
    m_EnemyList.clear();

    //make chest vanish (if any)
    ForEachNpc([](CNpcEntity* PNpc)
    {
        PNpc->loc.zone->PushPacket(PNpc, CHAR_INRANGE, new CEntityAnimationPacket(PNpc, CEntityAnimationPacket::Fade_Out));
        PNpc->PAI->Despawn();
    });
    //wipe npc list
    m_NpcList.clear();

    ForEachAlly([&](CMobEntity* PAlly)
    {
        PAlly->PAI->Despawn();
        GetZone()->DeletePET(PAlly);
        delete PAlly;
    });
    m_AllyList.clear();

    luautils::OnBcnmDestroy(this);

    ForEachPlayer([](CCharEntity* PChar)
    {
        PChar->PBattlefield = nullptr;
    });

    //todo: delete battlefield

}

bool CBattlefield::SpawnTreasureChest()
{
    //get ids from DB
    const int8* fmtQuery = "SELECT npcId \
						    FROM battlefield_treasure_chests \
							WHERE battlefieldId = %u AND battlefieldNumber = %u";

    int32 ret = Sql_Query(SqlHandle, fmtQuery, this->GetID(), this->GetArea());

    if (ret == SQL_ERROR || Sql_NumRows(SqlHandle) == 0)
    {
        ShowError("Battlefield::SpawnTreasureChest : SQL error - Cannot find any npc IDs for battlefieldId %i battlefieldNumber %i \n",
            this->GetID(), this->GetArea());
    }
    else
    {
        while (Sql_NextRow(SqlHandle) == SQL_SUCCESS)
        {
            uint32 npcid = Sql_GetUIntData(SqlHandle, 0);
            CBaseEntity* PNpc = (CBaseEntity*)zoneutils::GetEntity(npcid, TYPE_NPC);
            if (PNpc)
            {
                PNpc->Spawn();
                this->InsertEntity(PNpc);
            }
            else
            {
                ShowDebug(CL_CYAN"Battlefield::SpawnTreasureChest: <%s> is already spawned\n" CL_RESET, PNpc->GetName());
            }
        }
        return true;
    }
    return false;
}

void CBattlefield::OpenChest()
{
    auto LootList = itemutils::GetLootList(GetLootID());

    if (LootList)
    {
        for (auto i = 0; i < LootList->size(); ++i)
        {
            // todo: handle loot
        }
    }

    // start the event, they won
    ForEachPlayer([&](CCharEntity* PChar)
    {
        luautils::OnBcnmLeave(PChar, this, LEAVE_WIN);
    });
}

bool CBattlefield::LoseBcnm()
{
    // todo: handle losing
    ForEachPlayer([&](CCharEntity* PChar)
    {
        luautils::OnBcnmLeave(PChar, this, LEAVE_LOSE);
    });
    return true;
}

void CBattlefield::ClearPlayerEnmity(CCharEntity* PChar)
{
    ForEachEnemy([PChar](CMobEntity* PMob)
    {
        PMob->PEnmityContainer->Clear(PChar->id);
    });
}

bool CBattlefield::InProgress()
{
    ForEachEnemy([&](CMobEntity* PMob)
    {
        if (PMob->PEnmityContainer->GetEnmityList()->size())
        {
            if (m_Status == BATTLEFIELD_STATUS_OPEN)
                SetStatus(BATTLEFIELD_STATUS_LOCKED);

            return true;
        }
    });

    // mobs might have 0 enmity but we wont allow anymore players to enter
    return m_Status != BATTLEFIELD_STATUS_OPEN;
}

void CBattlefield::ForEachPlayer(std::function<void(CCharEntity*)> func)
{
    for (auto PChar : m_PlayerList)
    {
        func((CCharEntity*)PChar);
    }
}

void CBattlefield::ForEachEnemy(std::function<void(CMobEntity*)> func)
{
    for (auto mob : m_EnemyList)
    {
        func((CMobEntity*)mob.PMob);
    }
}

void CBattlefield::ForEachRequiredEnemy(std::function<void(CMobEntity*)> func)
{
    for (auto mob : m_EnemyList)
    {
        if (mob.condition & CONDITION_WIN_REQUIREMENT)
            func((CMobEntity*)mob.PMob);
    }
}

void CBattlefield::ForEachAdditionalEnemy(std::function<void(CMobEntity*)> func)
{
    for (auto mob : m_EnemyList)
    {
        if (mob.condition == CONDITION_NONE)
            func((CMobEntity*)mob.PMob);
    }
}

void CBattlefield::ForEachNpc(std::function<void(CNpcEntity*)> func)
{
    for (auto PNpc : m_NpcList)
    {
        func((CNpcEntity*)PNpc);
    }
}

void CBattlefield::ForEachAlly(std::function<void(CMobEntity*)> func)
{
    for (auto PAlly : m_AllyList)
    {
        func((CMobEntity*)PAlly);
    }
}