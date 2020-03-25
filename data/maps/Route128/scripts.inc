Route128_MapScripts:: @ 81F6B38
	map_script MAP_SCRIPT_ON_TRANSITION, Route128_OnTransition
	.byte 0

Route128_OnTransition: @ 81F6B43
	end

Lake_Vega_Gentleman_EventScript::
	lockall
	msgbox Lake_Vega_Gentleman_Text, MSGBOX_NPC
	end

Lake_Vega_Gentleman_Text:
	.string "Some locations are special. \p"
	.string "In those areas, two wild Pokémon will\n"
	.string "appear instead of one.$"
