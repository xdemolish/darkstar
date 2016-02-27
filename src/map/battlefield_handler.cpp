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

#include "utils/charutils.h"
#include "utils/zoneutils.h"



CBattlefieldHandler::CBattlefieldHandler(CZone* PZone)
{
    m_PZone = PZone;
    m_MaxBattlefields = luautils::OnBattlefieldHandlerInitialise(PZone);
}

void CBattlefieldHandler::HandleBattlefields(time_point tick)
{
    for (auto PBattlefield : m_Battlefields)
    {
        luautils::OnBattlefieldTick(PBattlefield);
    }
}