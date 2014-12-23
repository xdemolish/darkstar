-----------------------------------
--  Area: Tavnazian Safehold
--   NPC: Morangeart
--  Type: ENM Quest Activator
-- @zone: 26
--  @pos -74.308 -24.782 -28.475
-- 
-- Auto-Script: Requires Verification (Verified by Brawndo)
-----------------------------------
package.loaded["scripts/zones/Tavnazian_Safehold/TextIDs"] = nil;
-----------------------------------

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)
end;

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)
    local enmTimer player:getVar("[ENM]MonarchLinn");
    local waitTime = enmTimer - getTimeOffset(TIME_OFFSET_DEC);

    -- player has the keyitem but has not triggered the ENM
    if(player:hasKeyItem(MONARCH_BEARD) == true) then
        player:startEvent(0x020B);
        
    -- player has triggered the ENM and had the keyitem removed, 5 days are up
    elseif((player:hasKeyItem(MONARCH_BEARD) == false) and waitTime <= 0) then
        player:startEvent(0x0209);
        
    -- player has triggered the ENM and had the keyitem removed, 5 day wait isn't over yet
    elseif((player:hasKeyItem(MONARCH_BEARD) == false) and waitTime > 0) then
        player:startEvent(0x020A,enmTimer);
    else
        player:startEvent(0x0208);
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
    if(csid == 0x0209) then
        player:addKeyItem(MONARCH_BEARD);
        player:messageSpecial(KEYITEM_OBTAINED, MONARCH_BEARD);
        player:setVar("[ENM]MonarchLinn", 0);
    end
end;