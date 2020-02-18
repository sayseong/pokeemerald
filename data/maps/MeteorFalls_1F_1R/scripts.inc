MeteorFalls_1F_1R_MapScripts:: @ 822BD2A
	.byte 0

Alcor_Tunnel_Worker:: @ 82078F3
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Alcor_Tunnel_Worker_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Alcor_Tunnel_Worker_Kate
	end

Alcor_Tunnel_Worker_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	msgbox Alcor_Tunnel_Worker_Text, MSGBOX_NPC
	destroy_field_pic 1, VAR_0x8008
	return

Alcor_Tunnel_Worker_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	msgbox Alcor_Tunnel_Worker_Text, MSGBOX_NPC
	destroy_field_pic 3, VAR_0x8008
	return
	
Alcor_Tunnel_Worker_Text: @ 8207969
	.string "A landslide occured earlier.\p"
	.string "We are working as fast as we can to\n"
	.string "clean it up.$"
