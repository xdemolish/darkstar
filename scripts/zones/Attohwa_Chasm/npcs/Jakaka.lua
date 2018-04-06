-----------------------------------
--  Area: Attohwa Chasm
--  NPC:  Jakaka
--  Type: ENM
--  @pos -144.711 6.246 -250.309 7
-----------------------------------
package.loaded["scripts/zones/Attohwa_Chasm/TextIDs"] = nil;
-----------------------------------
require("scripts/zones/Attohwa_Chasm/TextIDs");
require("scripts/globals/keyitems");
require("scripts/globals/common");
require("scripts/globals/status");

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)

    local enmTimer player:getVar("[ENM]BoneyardGully");
    local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);
    
    if(trade:hasItemQty(1778,1) and trade:getItemCount() == 1) then
        if(player:hasKeyItem(MIASMA_FILTER) == false and waitTime <= 0) then
            player:startEvent(0x000c)
        end
    end
    
    
end;

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)
    
    local enmTimer player:getVar("[ENM]BoneyardGully");
    local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);

    -- player has the keyitem but hasn't triggered the ENM - gtfo
    if(player:hasKeyItem(MIASMA_FILTER) == true) then
        player:startEvent(0x000b);
        
    -- player has the keyitem and has triggered the ENM, display waiting time
    elseif(player:hasKeyItem(MIASMA_FILTER) == false and waitTime > 0) then
        player:startEvent(0x000e,enmTimer);
        
    -- player has no waiting time and already has the pouch in inventory, remind them what to do
    elseif(player:hasKeyItem(MIASMA_FILTER) == false and waitTime <= 0 and player:hasItem(1777) == true) then
        player:startEvent(0x000f);
        
    -- player doesn't have they keyitem, waiting time or pouch in inventory - display starting cs
    else
        player:startEvent(0x000d);
    end
end;

-----------------------------------
-- onEventUpdate
-----------------------------------

function onEventUpdate(player,csid,option)
	-- printf("CSID: %u",csid);
	-- printf("RESULT: %u",option);
end;

-----------------------------------
-- onEventFinish
-----------------------------------

function onEventFinish(player,csid,option)
	-- printf("CSID: %u",csid);
	-- printf("RESULT: %u",option);
    
    if(csid == 0x000d) then
    -- check if player has room in inventory before giving the Flaxen Pouch
        if(player:getFreeSlotsCount() > 0) then
            player:addItem(1777,1);
            player:messageSpecial(ITEM_OBTAINED, 1777);
        else
            -- player does not have enough inventory space free, tell them to free up some space
            player:messageSpecial(ITEM_CANNOT_BE_OBTAINED, 1777);
        end
        
    elseif(csid == 0x000c) then
        player:tradeComplete();
        player:addKeyItem(MIASMA_FILTER);
        player:messageSpecial(KEYITEM_OBTAINED, MIASMA_FILTER);
    end
end;