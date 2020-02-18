Route111_WinstrateFamilysHouse_MapScripts:: @ 822A48C
	.byte 0

Route111_EventScript_OldRodFisherman:: @ 81E95D5
	lock
	faceplayer
	goto_if_unset FLAG_RECEIVED_OLD_ROD, Route111_EventScript_GiveOldRod
	msgbox Route111_Text_Fisherman, MSGBOX_NPC
	end

Route111_EventScript_GiveOldRod:: @ 81E95FF
	msgbox Route111_Text_GiveOldRod, MSGBOX_DEFAULT
	giveitem ITEM_OLD_ROD
	setflag FLAG_RECEIVED_OLD_ROD
	msgbox Route111_Text_Fisherman, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_Have_Fun:: @ 81E9620
	msgbox Route111_Text_Fisherman, MSGBOX_DEFAULT
	release
	end

Route111_Text_GiveOldRod: @ 81E9E14
	.string "I'll give you one of my fishing RODS.$"
	
Route111_Text_Fisherman: 
	.string "Fishing is fun for everyone.$"
