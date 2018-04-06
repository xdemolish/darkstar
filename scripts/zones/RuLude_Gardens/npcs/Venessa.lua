-----------------------------------
--	Area: Ru'Lude Gardens
--	NPC:  Venessa
--	Standard Info NPC
-----------------------------------
package.loaded["scripts/zones/RuLude_Gardens/TextIDs"] = nil;
-----------------------------------
require("scripts/zones/RuLude_Gardens/TextIDs");
require("scripts/globals/settings");
require("scripts/globals/common");
require("scripts/globals/keyitems");
require("scripts/globals/missions");
require("scripts/globals/quests");
require("scripts/globals/status");

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)
end; 

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)

    --[[
        param  - 0x00 ~ 0x1F = REMOVE FROM LIST
                
                 0x00 = DISPLAY ALL
                 0x01 = REMOVE 'NOWHERE'
                 0x02 = REMOVE SPIRE OF HOLLA
                 0x04 = REMOVE SPIRE OF DEM
                 0x08 = REMOVE SPIRE OF MEA
                 0x10 = REMOVE SPIRE OF VAHZL
                 0x1F ~ 0x00 = ADD TO LIST
                
                 0x1F = DISPLAY NONE
                 0x1E = ADD 'NOWHERE'
                 0x1D = ADD SPIRE OF HOLLA
                 0x1B = ADD SPIRE OF DEM
                 0x17 = ADD SPIRE OF MEA
                 0x0F = ADD SPIRE OF VAHZL
        
        param2 - unknown -- STATIC
        param3 - unknown
        param4 - 0 for keyitem cs, 1+ if already got keyitem
        param5 - client side conversion handled from vana year/month/day to earth year/month/day/hour/minute
        param6 - if > 0, time to wait until is displayed
        param7 - unknown
        param8 - unknown
        
    ]]--
    
    local param  = 0x10;
    local param2 = 15;  
    local param3 = 50624;
    local param4 = 1;
    
    -- if player has completed Desires of Emptiness, display Spire of Vahzl in locations list
    if(player:hasCompletedMission(COP, DESIRES_OF_EMPTINESS)) then
        param = 0x00;
    end

	player:startEvent(0x2750,param,param2,param3,param4);
end;

-----------------------------------
-- onEventUpdate
-----------------------------------

function onEventUpdate(player,csid,option)
--printf("CSID: %u",csid);
--printf("RESULT: %u",option);

    -- array for the keyitems
    censer = {CENSER_OF_ABANDONMENT, CENSER_OF_ANTIPATHY, CENSER_OF_ANIUMUS, CENSER_OF_ACRIMONY}
    
    -- create an array for the progress checks, not used anywhere else - lets keep it local
    local vars = {"[ENM]Holla", "[ENM]Dem", "[ENM]Mea", "[ENM]Vahzl"}

    -- player has the keyitem already ENM and/or has triggered the wait time
    if(option > 0 and option < 255) then
        local enmTimer player:getVar(vars[option]);
        local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);
         
        -- player has already triggered the ENM, display the wait time
        if(waitTime > 0) then
            player:updateEvent(0,0,0,0,enmTimer,1);
            
        -- player already has keyitem but has not triggered the ENM, dont display the wait time
        elseif (waitTime <= 0 and player:hasKeyItem(censer[option])) then
            player:updateEvent(0,0,0,0,0,0);
            -- waitTime < 0 means player has waited 5 days, clear the var
            player:setVar(vars[option], 0);
        end
        
    end
    
end;

-----------------------------------
-- onEventFinish
-----------------------------------

function onEventFinish(player,csid,option)
--printf("CSID: %u",csid);
--printf("RESULT: %u",option);

    if(csid == 0x2750) then
        if(option > 0 and option < 255) then
            player:addKeyItem(censer[option]);
            player:messageSpecial(KEYITEM_OBTAINED, censer[option]);
        end
    end

end;