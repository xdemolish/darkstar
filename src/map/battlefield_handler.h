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

#ifndef _CBATTLEFIELDHANDLER_H
#define _CBATTLEFIELDHANDLER_H

#include "../common/cbasetypes.h"
#include "../common/mmo.h"

#include <vector>
#include <memory>

class CBattlefield;
class CCharEntity;
class CMobEntity;
class CZone;

class CBattlefieldHandler
{
public:

    CBattlefieldHandler(CZone* PZone);
    void	HandleBattlefields(time_point tick);							    // called every tick to handle win/lose conditions, locking the bcnm, etc
    void    LoadBattlefield(CCharEntity* PChar, uint16 battlefield);
    CBattlefield* GetBattlefield(CCharEntity* PChar);                           // returns the battlefield a player is in

private:
    CZone*                     m_PZone;
    uint8                      m_MaxBattlefields; // usually 3 except dynamis, einherjar, besieged, ...
    std::vector<std::unique_ptr<CBattlefield>> m_Battlefields;    // area
};

#endif