Route102_MapScripts:: @ 81EC0E0
	.byte 0

Open_Alcor_Tunnel::
	setflag FLAG_BADGE01_GET
	msgbox Temporary_Route_2_Clair, MSGBOX_NPC
	end

Temporary_Route_2_Clair:
	.string "This should open Alcor Tunnel.$"
