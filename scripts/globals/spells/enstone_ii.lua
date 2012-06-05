-----------------------------------------
-- Spell: Enstone II
-----------------------------------------

require("scripts/globals/status");
require("scripts/globals/magic");

-----------------------------------------
-- OnSpellCast
-----------------------------------------

function onSpellCast(caster,target,spell)
	effect = EFFECT_ENSTONE_II;
	doEnspell(caster,target,spell,effect);
	return effect;
end;