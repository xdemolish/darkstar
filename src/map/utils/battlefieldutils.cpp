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

#include "../entities/charentity.h"
#include "../entities/mobentity.h"
#include "../party.h"
#include "../treasure_pool.h"
#include "charutils.h"
#include "../alliance.h"
#include "zoneutils.h"
#include "itemutils.h"
#include "battlefieldutils.h"
#include "mobutils.h"
#include "../battlefield.h"
#include "../battlefield_handler.h"
#include "../packets/entity_update.h"
#include "../ai/ai_container.h"
#include "../status_effect_container.h"
#include "../status_effect.h"

namespace battlefieldutils {

    /**************************************************************
    Called by ALL BCNMs to check winning conditions every tick. This
    is usually when all the monsters are defeated but can be other things
    (e.g. mob below X% HP, successful Steal, etc)
    ***************************************************************/
    bool meetsWinningConditions(CBattlefield* battlefield, time_point tick) {

        //if (battlefield->won()) return true;

        //handle odd cases e.g. stop fight @ x% HP
        /* todo: handle in scripts
        //handle Maat fights
        if (battlefield->locked && (battlefield->m_RuleMask & RULES_MAAT))
        {
            // survive for 5 mins
            if (battlefield->getPlayerMainJob() == JOB_WHM && (tick - battlefield->fightTick) > 5min)
                return true;

            if (battlefield->isEnemyBelowHPP(10))
                return true;


            if (battlefield->getPlayerMainJob() == JOB_THF && battlefield->m_EnemyList.at(0)->m_ItemStolen) //thf can win by stealing from maat only if maat not previously defeated
            {
                const int8* fmtQuery = "SELECT value FROM char_vars WHERE charid = %u AND varname = '%s' LIMIT 1;";
                int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield->m_PlayerList.at(0)->id, "maatDefeated");
                if (ret != SQL_ERROR && Sql_NumRows(SqlHandle) == 0)
                    return true;
                else if (ret != SQL_ERROR && Sql_NumRows(SqlHandle) != 0 && Sql_NextRow(SqlHandle) == SQL_SUCCESS)
                {
                    int16 value = (int16)Sql_GetIntData(SqlHandle, 0);
                    if (value <= 0)
                        return true;
                }
            }
        }

        // savage
        if (battlefield->GetID() == 961 && battlefield->isEnemyBelowHPP(30)) {
            return true;
        }
        */

        //generic cases, kill all mobs
        if (battlefield->AllEnemiesDefeated())
        {
            return true;
        }
        return false;
    }

    /**************************************************************
    Called by ALL BCNMs to check losing conditions every tick. This
    will be when everyone is dead and the death timer is >3min (usually)
    or when everyone has left, etc.
    ****************************************************************/
    bool meetsLosingConditions(CBattlefield* battlefield, time_point tick) {
        /* todo: handle in scripts
        if (battlefield->lost()) return true;
        //check for expired duration e.g. >30min. Need the tick>start check as the start can be assigned
        //after the tick initially due to threading
        if (tick > battlefield->getStartTime() && (tick - battlefield->getStartTime()) > battlefield->getTimeLimit()) {
            ShowDebug("BCNM %i inst:%i - You have exceeded your time limit!\n", battlefield->GetID(),
                battlefield->GetArea(), tick, battlefield->getStartTime(), battlefield->getTimeLimit());
            return true;
        }

        battlefield->lastTick = tick;

        //check for all dead for 3min (or whatever the rule mask says)
        if (battlefield->getDeadTime() != time_point::min()) {
            if (battlefield->m_RuleMask & RULES_REMOVE_3MIN) {
                //	if(((tick - battlefield->getDeadTime())/1000) % 20 == 0){
                //		battlefield->pushMessageToAllInBcnm(200,180 - (tick - battlefield->getDeadTime())/1000);
                //	}
                if (tick - battlefield->getDeadTime() > 3min) {
                    ShowDebug("All players from the battlefield %i inst:%i have fallen for 3mins. Removing.\n",
                        battlefield->GetID(), battlefield->GetArea());
                    return true;
                }
            }
            else {
                ShowDebug("All players have fallen. Failed battlefield %i inst %i. No 3min mask. \n", battlefield->GetID(), battlefield->GetArea());
                return true;
            }
        }
        */
        return false;
    }

    /*************************************************************
    Returns the losing exit position for this BCNM.
    ****************************************************************/
    void getLosePosition(CBattlefield* battlefield, int(&pPosition)[4])
    {
        if (battlefield == nullptr)
            return;

        switch (battlefield->GetZoneID())
        {
            case 139: //Horlais Peak
                pPosition[0] = -503; pPosition[1] = 158; pPosition[2] = -212; pPosition[3] = 131;
                break;
        }
    }

    void getStartPosition(uint16 zoneid, int(&pPosition)[4])
    {

        switch (zoneid)
        {
            case 139: //Horlais Peak
                pPosition[0] = -503; pPosition[1] = 158; pPosition[2] = -212; pPosition[3] = 131;
                break;
            case 144: //Waug. Shrine
                pPosition[0] = -361; pPosition[1] = 100; pPosition[2] = -260; pPosition[3] = 131;
                break;
            case 146: //Balgas Dias
                pPosition[0] = 317; pPosition[1] = -126; pPosition[2] = 380; pPosition[3] = 131;
                break;
            case 165: //Throne Room
                pPosition[0] = 114; pPosition[1] = -8; pPosition[2] = 0; pPosition[3] = 131;
                break;
            case 206: //QuBia Arena
                pPosition[0] = -241; pPosition[1] = -26; pPosition[2] = 20; pPosition[3] = 131;
                break;
        }
    }

    /*************************************************************
    Returns the winning exit position for this BCNM.
    ****************************************************************/
    void getWinPosition(CBattlefield* battlefield, int(&pPosition)[4]) {
        if (battlefield == nullptr)
            return;

        switch (battlefield->GetZoneID()) {
            case 139: //Horlais Peak
                pPosition[0] = 445; pPosition[1] = -38; pPosition[2] = -19; pPosition[3] = 200;
                break;
        }
    }


    uint8 getMaxLootGroups(CBattlefield* battlefield) {
        const int8* fmtQuery = "SELECT MAX(lootGroupId) \
						FROM battlefield_loot \
						JOIN battlefield_info ON battlefield_info.LootDropId = battlefield_loot.LootDropId \
						WHERE battlefield_info.LootDropId = %u LIMIT 1";

        int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield->GetLootID());
        if (ret == SQL_ERROR || Sql_NumRows(SqlHandle) == 0 || Sql_NextRow(SqlHandle) != SQL_SUCCESS) {
            ShowError("battlefieldutils::getMaxLootGroups() : SQL error occured \n");
            return 0;
        }
        else {
            return (uint8)Sql_GetUIntData(SqlHandle, 0);
        }
    }

    uint16 getRollsPerGroup(CBattlefield* battlefield, uint8 groupID) {
        const int8* fmtQuery = "SELECT SUM(CASE \
			WHEN LootDropID = %u \
			AND lootGroupId = %u \
			THEN rolls  \
			ELSE 0 END) \
			FROM battlefield_loot;";

        int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield->GetLootID(), groupID);
        if (ret == SQL_ERROR || Sql_NumRows(SqlHandle) == 0 || Sql_NextRow(SqlHandle) != SQL_SUCCESS) {
            ShowError("battlefieldutils::getRollsPerGroup() : SQL error occured \n");
            return 0;
        }
        else {
            return (uint16)Sql_GetUIntData(SqlHandle, 0);
        }
    }

    /*************************************************************
    Get loot from the armoury crate
    ****************************************************************/

    void getChestItems(CBattlefield* battlefield) {
        int instzone = battlefield->GetZoneID();
        uint8 maxloot = 0;
        LootList_t* LootList = itemutils::GetLootList(battlefield->GetLootID());

        if (LootList == nullptr) {
            ShowError("battlefieldutils::getChestItems() : battlefieldId %u area %u chest opened with no valid loot list!", battlefield->GetID(), battlefield->GetArea());
            //no loot available for bcnm. End bcnm.
            // todo: battlefield->winBcnm();
            return;
        }
        else
        {
            for (uint8 sizeoflist = 0; sizeoflist < LootList->size(); ++sizeoflist) {
                if (LootList->at(sizeoflist).LootGroupId > maxloot) {
                    maxloot = LootList->at(sizeoflist).LootGroupId;
                }
            }
        }
        //getMaxLootGroups(battlefield);
        /* todo: handle rolls and finish on treasure
        if (maxloot != 0) {
            for (uint8 group = 0; group <= maxloot; ++group) {
                uint16 maxRolls = getRollsPerGroup(battlefield, group);
                uint16 groupRoll = dsprand::GetRandomNumber(maxRolls);
                uint16 itemRolls = 0;

                for (uint8 item = 0; item < LootList->size(); ++item)
                {
                    if (group == LootList->at(item).LootGroupId)
                    {
                        itemRolls += LootList->at(item).Rolls;
                        if (groupRoll <= itemRolls)
                        {
                            battlefield->m_PlayerList.at(0)->PTreasurePool->AddItem(LootList->at(item).ItemID, battlefield->m_NpcList.at(0));
                            break;
                        }
                    }
                }
            }
        }
        //user opened chest, complete bcnm
        if (instzone != 37 && instzone != 38) {
            battlefield->winBcnm();
        }
        else {
            battlefield->m_NpcList.clear();
        }
        */
    }
};
