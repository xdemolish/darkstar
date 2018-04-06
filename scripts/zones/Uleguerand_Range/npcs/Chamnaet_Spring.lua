-----------------------------------
--  Area: Uleguerand Range
--  NPC:  Chamnaet Spring
--  Type: Quest NPC
--  @pos -305 2 19 5
-----------------------------------
package.loaded["scripts/zones/Uleguerand_Range/TextIDs"] = nil;
-----------------------------------
require("scripts/zones/Uleguerand_Range/TextIDs");
require("scripts/globals/keyitems");
require("scripts/globals/common");
require("scripts/globals/status");

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)
    -- trade cotton pouch for chamnaet ice
    if(trade:hasItemQty(1779,1) and trade:getItemCount() == 1) then
        player:tradeComplete();
        player:addItem(1780,1);
        player:messageSpecial(ITEM_OBTAINED, 1780);
    end

end;

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)

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