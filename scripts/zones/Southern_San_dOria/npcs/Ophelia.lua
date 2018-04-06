-----------------------------------
--	Area: Southern San d'Oria
--	NPC: Ophelia
--  General Info NPC
-------------------------------------
package.loaded["scripts/zones/Southern_San_dOria/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/settings");
require("scripts/globals/quests");
require("scripts/zones/Southern_San_dOria/TextIDs");

----------------------------------- 
-- onTrade Action 
----------------------------------- 

function onTrade(player,npc,trade)
-- "Flyers for Regine" conditional script
FlyerForRegine = player:getQuestStatus(SANDORIA,FLYERS_FOR_REGINE);

	if (FlyerForRegine == 1) then
		count = trade:getItemCount();
		MagicFlyer = trade:hasItemQty(532,1);
		if (MagicFlyer == true and count == 1) then
			player:messageSpecial(FLYER_REFUSED);
		end
	end
end;

----------------------------------- 
-- onTrigger Action 
-----------------------------------
 
function onTrigger(player,npc)
    --[[
        param - 
                0x000 > 0x200 = REMOVE FROM LIST
                0x000 = DISPLAY ALL
                0x001 = REMOVE 'NOWHERE'
                0x002 = REMOVE SPIRE OF HOLLA
                0x004 = REMOVE SPIRE OF DEM
                0x008 = REMOVE SPIRE OF MEA
                0x010 = REMOVE SPIRE OF VAHZL
                0x020 = REMOVE MONARCH LINN
                0x040 = REMOVE SHROUDED MAW
                0x080 = REMOVE MINE SHAFT #2716
                0x100 = REMOVE BEARCLAW PINNACLE
                0x200 = REMOVE BONEYARD GULLY
                
        param2 - time to wait in unixtime
                
    ]]--
    local param = 0;
    local param2 = 0;

	player:startEvent(0x2F1,param,param2);
end; 

-----------------------------------
-- onEventUpdate
-----------------------------------

function onEventUpdate(player,csid,option)
--printf("CSID: %u",csid);
--printf("RESULT: %u",option);

    -- array for the progress checks, not used anywhere else - lets keep it local
    local vars = {"[ENM]Holla", "[ENM]Dem", "[ENM]Mea", "[ENM]Vahzl", "[ENM]MonarchLinn", "[ENM]ShroudedMaw", "[ENM]MineShaft", "[ENM]Bearclaw", "[ENM]BoneyardGully"}
    
    if(option > 0 and option < 255) then
        local enmTimer player:getVar(vars[option]);
        local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);
    
        -- waitTime > 0 means theres still wait time, display the date to wait until
        if(waitTime > 0) then
            player:updateEvent(0,enmTimer);
        end
        
    end

end;

-----------------------------------
-- onEventFinish
-----------------------------------

function onEventFinish(player,csid,option)
--printf("CSID: %u",csid);
--printf("RESULT: %u",option);
end;