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

#include "utils/zoneutils.h"
#include "zone.h"

CBattlefield::CBattlefield(uint16 id, CZone* PZone, uint8 area, CCharEntity* PInitiator)
{
    m_ID = id;
    m_PZone = PZone;
    m_Area = area;
    m_Initiator = PInitiator->name.c_str();
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

const int8* CBattlefield::GetName()
{
    return m_Name.c_str();
}

const int8* CBattlefield::GetInitiator()
{
    return m_Initiator.c_str();
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

uint32 CBattlefield::GetStartTime()
{
    return m_StartTime;
}

uint32 CBattlefield::GetTimeInside()
{
    return uint32(m_Tick - m_StartTime);
}

uint32 CBattlefield::GetTimeLimit()
{
    return m_TimeLimit;
}

uint32 CBattlefield::GetAllDeadTime()
{
    return m_AllDeadTime;
}

uint32 CBattlefield::GetFinishTime()
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

void CBattlefield::SetTimeLimit(uint32 time)
{
    m_TimeLimit = time;
}

void CBattlefield::SetAllDeadTime(uint32 time)
{
    m_AllDeadTime = time;
}

void CBattlefield::SetArea(uint8 area)
{
    m_Area = area;
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
    if (m_PlayerList.size() == 0) {
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
    for (auto condition : m_EnemyList)
    {
        if (condition.required && !condition.PMob->PAI->IsCurrentState<CDeathState>())
            return false;
    }
    return true;
}

bool CBattlefield::IsOccupied()
{
    return m_PlayerList.size() > 0;
}

bool CBattlefield::InsertEntity(CBaseEntity* PEntity, bool ally, bool requiredkill)
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
        // mobs
        if (!ally)
        {
            // only apply conditions to mobs spawning by default
            BattlefieldMob_t mob;
            mob.PMob = static_cast<CMobEntity*>(PEntity);
            mob.required = requiredkill;
            m_EnemyList.push_back(mob);
        }
        // ally
        else
        {
            m_AllyList.push_back(static_cast<CMobEntity*>(PEntity));
        }
    }
    return true;
}

void CBattlefield::Cleanup()
{
    // wipe enmity from all mobs in list if needed
    ForEachEnemy([](CMobEntity* PMob)
    {
        PMob->PAI->Despawn();
        PMob->status = STATUS_DISAPPEAR;
        PMob->PBCNM = nullptr;
    });
    // wipe mob list
    m_EnemyList.clear();

    //make chest vanish (if any)
    ForEachNpc([](CNpcEntity* PNpc)
    {
        PNpc->loc.zone->PushPacket(PNpc, CHAR_INRANGE, new CEntityAnimationPacket(PNpc, CEntityAnimationPacket::Fade_Out));
        PNpc->animation = ANIMATION_DEATH;
        PNpc->status = STATUS_MOB;
        PNpc->loc.zone->PushPacket(PNpc, CHAR_INRANGE, new CEntityUpdatePacket(PNpc, ENTITY_UPDATE, UPDATE_COMBAT));
    });
    //wipe npc list
    m_NpcList.clear();

    for (auto PAlly : m_AllyList)
    {
        zoneutils::GetZone(GetZoneID())->DeletePET(PAlly);
        delete PAlly;
    }
    m_AllyList.clear();

    luautils::OnBcnmDestroy(this);

    ForEachPlayer([](CCharEntity* PChar)
    {
        PChar->PBCNM = nullptr;
    });

    //todo: delete battlefield

}

bool CBattlefield::SpawnTreasureChest()
{
    //todo: battlefieldutils::spawnTreasureForBcnm(this);
    return true;
}

void CBattlefield::OpenChestinBcnm()
{
    //todo: handle chestsB
}

bool CBattlefield::LoseBcnm()
{
// todo: handle losing
    for (int i = 0; i < m_PlayerList.size(); i++)
        luautils::OnBcnmLeave(m_PlayerList.at(i), this, LEAVE_LOSE);

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
    ForEachEnemy([](CMobEntity* PMob)
    {
        if (PMob->PEnmityContainer->GetEnmityList()->size())
            return true;
    });

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
        if(mob.required)
            func((CMobEntity*)mob.PMob);
    }
}

void CBattlefield::ForEachAdditionalEnemy(std::function<void(CMobEntity*)> func)
{
    for (auto mob : m_EnemyList)
    {
        if (mob.required)
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