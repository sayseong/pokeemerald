Route11_Fishermans_Lodge_MapScripts:: @ 822A48C
	.byte 0

Route11_Fishermans_Lodge_EventScript_OldRodFisherman:: @ 81E95D5
	lock
	faceplayer
	goto_if_unset FLAG_RECEIVED_OLD_ROD, Route11_Fishermans_Lodge_EventScript_GiveOldRod
	msgbox Route11_Fishermans_Lodge_Text_Fisherman, MSGBOX_NPC
	end

Route11_Fishermans_Lodge_EventScript_GiveOldRod:: @ 81E95FF
	msgbox Route11_Fishermans_Lodge_Text_GiveOldRod, MSGBOX_DEFAULT
	giveitem ITEM_OLD_ROD
	setflag FLAG_RECEIVED_OLD_ROD
	msgbox Route11_Fishermans_Lodge_Text_Fisherman, MSGBOX_DEFAULT
	release
	end

Route11_Fishermans_Lodge_EventScript_Have_Fun:: @ 81E9620
	msgbox Route11_Fishermans_Lodge_Text_Fisherman, MSGBOX_DEFAULT
	release
	end

Route11_Fishermans_Lodge_Text_GiveOldRod: @ 81E9E14
	.string "I'll give you one of my fishing RODS.$"
	
Route11_Fishermans_Lodge_Text_Fisherman: 
	.string "Fishing is fun for everyone.$"
