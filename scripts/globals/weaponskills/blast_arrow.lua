-----------------------------------	
-- Blast Arrow	
-- Archery weapon skill	
-- Skill level: 200	
-- Delivers a melee-distance ranged attack. params.accuracy varies with TP.	
-- Aligned with the Snow Gorget & Light Gorget.	
-- Aligned with the Snow Belt & Light Belt.	
-- Element: None	
-- Modifiers: STR:16% ; AGI:25%	
-- 100%TP    200%TP    300%TP	
-- 2.00      2.00      2.00	
-----------------------------------	
	
require("scripts/globals/status");	
require("scripts/globals/settings");	
require("scripts/globals/weaponskills");	
-----------------------------------	
	
function onUseWeaponSkill(player, target, wsID)	
	
	local params = {};
	params.numHits = 1;
	params.ftp100 = 2; params.ftp200 = 2; params.ftp300 = 2;
	params.str_wsc = 0.16; params.dex_wsc = 0.0; params.vit_wsc = 0.0; params.agi_wsc = 0.25; params.int_wsc = 0.0; params.mnd_wsc = 0.0; params.chr_wsc = 0.0;
	params.crit100 = 0.0; params.crit200 = 0.0; params.crit300 = 0.0;
	params.canCrit = false;
	params.acc100 = 0.8; params.acc200= 0.9; params.acc300= 1;
	params.atkmulti = 1;
	local damage, tpHits, extraHits = doRangedWeaponskill(player, target, params);
   local crticalHit = false;
	
	return tpHits, extraHits, criticalHit, damage;
	
end	
