-----------------------------------
--  Area: Uleguerand Range
--  NPC:  Zebada
--  Type: ENM Quest Activator
--  @pos -308.112 -42.137 -570.096 5
-----------------------------------
package.loaded["scripts/zones/Uleguerand_Range/TextIDs"] = nil;
-----------------------------------

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)
    local enmTimer player:getVar("[ENM]Bearclaw");
    local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);

    -- trade Chamnaet Ice
    if(trade:hasItemQty(1780,1) and trade:getItemCount() == 1) then
    
    -- check if player already has keyitem or still has time left to wait
        if(player:hasKeyItem(ZEPHYR_FAN) == false and waitTime <= 0) then
            -- award keyitem
            player:startEvent(0x000d);
        end
    end
end;

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)
    local enmTimer = player:getVar("[ENM]Bearclaw");
    local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);
    
    -- player has the keyitem but has not triggered the ENM
    if(player:hasKeyItem(ZEPHYR_FAN) == true) then
        player:startEvent(0x000c);
        
    -- player has triggered the ENM and had the keyitem removed, 5 day wait isn't up
    elseif(player:hasKeyItem(ZEPHYR_FAN) == false and waitTime > 0) then
        player:startEvent(0x000F, enmTimer);
        
    -- player has triggered the ENM and had the keyitem removed, 5 days are up
    elseif(player:hasKeyItem(ZEPHYR_FAN) == false and waitTime <= 0) then
        player:startEvent(0x000e);
        
    else
        player:startEvent(0x000c);
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
end;