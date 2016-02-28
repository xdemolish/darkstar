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

#include "../../common/showmsg.h"

#include "lua_battlefield.h"
#include "lua_baseentity.h"
#include "../battlefield.h"
#include "../utils/mobutils.h"
#include "../utils/zoneutils.h"
#include "../status_effect_container.h"


/************************************************************************
*																		*
*  Constructor															*
*																		*
************************************************************************/

CLuaBattlefield::CLuaBattlefield(lua_State *L)
{
    if (!lua_isnil(L, -1))
    {
        m_PLuaBattlefield = (CBattlefield*)(lua_touserdata(L, -1));
        lua_pop(L, 1);
    }
    else
    {
        m_PLuaBattlefield = nullptr;
    }
}

/************************************************************************
*																		*
*  Constructor															*
*																		*
************************************************************************/

CLuaBattlefield::CLuaBattlefield(CBattlefield* PBattlefield)
{
    m_PLuaBattlefield = PBattlefield;
}

/************************************************************************
*                                                                       *
*						Get methods								        *
*                                                                       *
************************************************************************/

inline int32 CLuaBattlefield::getBattlefieldNumber(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetArea());
    return 1;
}

inline int32 CLuaBattlefield::getTimeLimit(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    //todo: lua_pushinteger(L, std::chrono::duration_cast<std::chrono::seconds>(m_PLuaBattlefield->GetTimeLimit()).count());
    return 1;
}

inline int32 CLuaBattlefield::getBcnmID(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetID());
    return 1;
}

inline int32 CLuaBattlefield::getTimeInside(lua_State* L) {
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    uint32 duration = std::chrono::duration_cast<std::chrono::seconds>(m_PLuaBattlefield->GetTimeInside()).count();
    //lua_pushinteger(L, duration);
    return 1;
}

inline int32 CLuaBattlefield::getFastestTime(lua_State* L) {
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    //lua_pushinteger(L, m_PLuaBattlefield->GetCurrentRecord().time);
    return 1;
}

inline int32 CLuaBattlefield::getFastestPlayer(lua_State* L) {
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushstring(L, m_PLuaBattlefield->GetCurrentRecord().name.c_str());
    return 1;
}

inline int32 CLuaBattlefield::setAsFastest(lua_State* L) {
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, 0);
    return 1;
}

inline int32 CLuaBattlefield::getEntrance(lua_State* L) {
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetArea());
    return 1;
}

inline int32 CLuaBattlefield::setEntrance(lua_State* L) {
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(!lua_isnumber(L, 1) || lua_isnil(L, 1));
    m_PLuaBattlefield->SetArea(lua_tointeger(L, 1));
    return 0;
}

inline int32 CLuaBattlefield::insertAlly(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(!lua_isnumber(L, 1) || lua_isnil(L, 1));

    uint32 groupid = lua_tointeger(L, 1);

    CMobEntity* PAlly = mobutils::InstantiateAlly(groupid, m_PLuaBattlefield->GetZoneID());
    if (PAlly)
    {
        m_PLuaBattlefield->InsertEntity(PAlly, true);
        PAlly->PBattlefield = m_PLuaBattlefield;
        PAlly->StatusEffectContainer->AddStatusEffect(new CStatusEffect(EFFECT_BATTLEFIELD, EFFECT_BATTLEFIELD, m_PLuaBattlefield->GetID(), 0, 0), true);
        lua_getglobal(L, CLuaBaseEntity::className);
        lua_pushstring(L, "new");
        lua_gettable(L, -2);
        lua_insert(L, -2);
        lua_pushlightuserdata(L, (void*)PAlly);
        lua_pcall(L, 2, 1, 0);
    }
    else
    {
        lua_pushnil(L);
        ShowError(CL_RED "CLuaBattlefield::insertAlly - group ID %u not found!" CL_RESET, groupid);
    }
    return 1;
}

inline int32 CLuaBattlefield::getAllies(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    /* todo
    lua_createtable(L, m_PLuaBattlefield->m_AllyList.size(), 0);
    int8 newTable = lua_gettop(L);
    int i = 1;
    for (auto ally : m_PLuaBattlefield->m_AllyList)
    {
        lua_getglobal(L, CLuaBaseEntity::className);
        lua_pushstring(L, "new");
        lua_gettable(L, -2);
        lua_insert(L, -2);
        lua_pushlightuserdata(L, (void*)ally);
        lua_pcall(L, 2, 1, 0);

        lua_rawseti(L, -2, i++);
    }
    */
    return 1;
}

inline int32 CLuaBattlefield::lose(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    //m_PLuaBattlefield->lose();

    return 0;
}

inline int32 CLuaBattlefield::win(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    //m_PLuaBattlefield->win();

    return 0;
}

inline int32 CLuaBattlefield::loadMobs(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    //m_PLuaBattlefield->LoadMobs();
    return 0;
}

inline int32 CLuaBattlefield::loadNPCs(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    //m_PLuaBattlefield->LoadNPCs();
    return 0;
}

inline int32 CLuaBattlefield::insertEntity(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));

    auto targid = lua_tointeger(L, 1);
    auto filter = lua_tointeger(L, 2);
    bool ally = !lua_isnil(L, 3) ? lua_toboolean(L, 3) : false;
    BCMOBCONDITIONS conditions = static_cast<BCMOBCONDITIONS>(!lua_isnil(L, 4) ? lua_tointeger(L, 4) : 0);

    auto PEntity = ally ? mobutils::InstantiateAlly(targid, m_PLuaBattlefield->GetZoneID()) : m_PLuaBattlefield->GetZone()->GetEntity(targid, filter);

    if (PEntity)
    {
        m_PLuaBattlefield->InsertEntity(PEntity, ally, conditions);
        CLuaBaseEntity LuaBaseEntity(PEntity);
        Lunar<CLuaBaseEntity>::push(L, &LuaBaseEntity);
    }
    else
    {
        ShowError(CL_RED "CLuaBattlefield::insertEntity - targid ID %u not found!" CL_RESET, targid);
    }

    lua_pushnil(L);
    return 1;
}

/************************************************************************
*																		*
*  declare lua function													*
*																		*
************************************************************************/

const int8 CLuaBattlefield::className[] = "CBattlefield";
Lunar<CLuaBattlefield>::Register_t CLuaBattlefield::methods[] =
{
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getBattlefieldNumber),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getBcnmID),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getTimeLimit),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getTimeInside),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getFastestTime),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getFastestPlayer),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getEntrance),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setEntrance),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,insertAlly),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getAllies),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,lose),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,win),
    {nullptr,nullptr}
};