-----------------------------------	
-- Shadow Of Death	
-- Scythe weapon skill	
-- Skill Level: 70	
-- Delivers a dark elemental attack. Damage varies with TP.	
-- Aligned with the Snow Gorget & Aqua Gorget.	
-- Aligned with the Snow Belt & Aqua Belt.	
-- Element: Dark	
-- Modifiers: STR:30% ; INT:30%	
-- 100%TP    200%TP    300%TP	
-- 1.00      2.50      3.00	
-----------------------------------	
	
require("scripts/globals/status");	
require("scripts/globals/settings");	
require("scripts/globals/weaponskills");	
-----------------------------------	
	
function onUseWeaponSkill(player, target, wsID)	
	
	local params = {};
	params.numHits = 1;
	params.ftp100 = 1; params.ftp200 = 2.5; params.ftp300 = 3;
	params.str_wsc = 0.3; params.dex_wsc = 0.0; params.vit_wsc = 0.0; params.agi_wsc = 0.0; params.int_wsc = 0.3; params.mnd_wsc = 0.0; params.chr_wsc = 0.0;
	params.crit100 = 0.0; params.crit200 = 0.0; params.crit300 = 0.0;
	params.canCrit = false;
	params.acc100 = 0.0; params.acc200= 0.0; params.acc300= 0.0;
	params.atkmulti = 1;
	local damage, criticalHit, tpHits, extraHits = doPhysicalWeaponskill(player, target, params);
	
	return tpHits, extraHits, criticalHit, damage;
	
end	
