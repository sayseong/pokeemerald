MeteorFalls_1F_1R_MapScripts:: @ 822BD2A
	map_script MAP_SCRIPT_ON_LOAD, Alcor_Tunnel_OnTransition
	.byte 0

Alcor_Tunnel_OnTransition::
	call_if_set FLAG_BADGE01_GET, Alcor_Tunnel_Cleared
	end

Alcor_Tunnel_Cleared::
	setmetatile 23, 23, METATILE_Cave_FlatFloor, 0
	setmetatile 24, 23, METATILE_Cave_FlatFloor, 0
	setmetatile 26, 23, METATILE_Cave_FlatFloor, 0
	setmetatile 27, 23, METATILE_Cave_FlatFloor, 0
	setmetatile 23, 22, METATILE_Cave_FlatFloor, 0
	setmetatile 24, 22, METATILE_Cave_FlatFloor, 0
	setmetatile 25, 22, METATILE_Cave_FlatFloor, 0
	setmetatile 26, 22, METATILE_Cave_FlatFloor, 0
	setmetatile 27, 22, METATILE_Cave_FlatFloor, 0
	setmetatile 23, 21, METATILE_Cave_FlatFloor, 0
	setmetatile 24, 21, METATILE_Cave_FlatFloor, 0
	setmetatile 25, 21, METATILE_Cave_FlatFloor, 0
	setmetatile 26, 21, METATILE_Cave_FlatFloor, 0
	setmetatile 27, 21, METATILE_Cave_FlatFloor, 0
	setmetatile 25, 20, METATILE_Cave_FlatFloor, 0
	setmetatile 26, 20, METATILE_Cave_FlatFloor, 0
	return

Alcor_Tunnel_Worker:: @ 82078F3
	msgbox Alcor_Tunnel_Worker_Text, MSGBOX_NPC
	end

Alcor_Tunnel_Worker_Text: @ 8207969
	.string "A landslide occured earlier.\p"
	.string "We are working as fast as we can to\n"
	.string "clean it up.$"
