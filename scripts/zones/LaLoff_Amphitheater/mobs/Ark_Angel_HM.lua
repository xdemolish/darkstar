-----------------------------------
-- Area: LaLoff Amphitheater
-- NPC:  Ark Angel HM
-----------------------------------

require("scripts/globals/status");
require("scripts/zones/LaLoff_Amphitheater/TextIDs");

function onMobInitialize(mob)
    mob:addMod(MOD_REGAIN, 50);
end;

-----------------------------------
-- onMobSpawn Action
-----------------------------------

function OnMobSpawn(mob)
end;

-----------------------------------
-- onMobEngaged
-----------------------------------

function onMobEngaged(mob,target)
    local mobid = mob:getID();

    for member = mobid, mobid+7 do
        if (GetMobAction(member) == 16) then 
            GetMobByID(member):updateEnmity(target);
        end
    end
   
    local hp = math.random(0,60)
    mob:setLocalVar("Mijin", hp);
end;

-----------------------------------
-- onMobFight Action
-----------------------------------
function onMobFight(mob,target)

	local battletime = mob:getBattleTime();
	local mstime = mob:getLocalVar("Mighty");
    local mghp = mob:getLocalVar("Mijin");
	
	if (battletime > mstime + 150) then
		mob:useMobAbility(432);
		mob:setLocalVar("Mighty", battletime);
	elseif (mob:getHPP() < mghp) then
		mob:useMobAbility(475);
		mob:setLocalVar("Mijin", 0);
	end
	
end;

-----------------------------------
-- onMobDeath Action
-----------------------------------

function onMobDeath(mob,killer)
end;
