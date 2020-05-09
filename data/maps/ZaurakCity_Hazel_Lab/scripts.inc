ZaurakCity_Hazel_Lab_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ZaurakCity_Hazel_Lab_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ZaurakCity_Hazel_Lab_OnFrame
	.byte 0

ZaurakCity_Hazel_Lab_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	end

ZaurakCity_Hazel_Lab_OnFrame: @ 8228431
	map_script_2 VAR_UNUSED_0x404E, 0, ZaurakCity_Hazel_Lab_Meeting
	.2byte 0

ZaurakCity_Hazel_Lab_Meeting::
	applymovement 255, Common_Movement_WalkUp4
	waitmovement 0
	call Common_EventScript_SetupRival_Duo_Mugshot
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_1_Derek, MSGBOX_DEFAULT
	applymovement 4, Common_Movement_WalkInPlaceUp
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_2_Derek, MSGBOX_DEFAULT
	applymovement 2, Common_Movement_WalkInPlaceDown
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_3, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_Duo_Mugshot
	closemessage
	applymovement 2, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 2, ZaurakCity_Hazel_Lab_Walk_Left
	waitmovement 0
	applymovement 2, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 2, ZaurakCity_Hazel_Lab_Walk_Machine
	waitmovement 0
	playfanfare MUS_ME_ASA
	special HealPlayerParty
	waitfanfare
	applymovement 2, Zaurak_Hazel_Lab_Walk_Trainers
	waitmovement 0 	
	call Common_EventScript_SetupRival_Duo_Mugshot
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_4, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_Duo_Mugshot
	closemessage	
	applymovement 1, Zaurak_Hazel_Lab_Walk_Clair
	waitmovement 0
	applymovement 2, Common_Movement_FaceRight
	waitmovement 0
	load_field_pic 8, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_5, MSGBOX_DEFAULT
	applymovement 2, Common_Movement_FaceDown
	waitmovement 0
	destroy_field_pic 8, VAR_0x8009
	call Common_EventScript_SetupRival_Duo_Mugshot
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_6, MSGBOX_DEFAULT
	call ZaurakCity_EventScript_ReceivePokedex
	applymovement 1, Common_Movement_FaceDown
	waitmovement 0
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_7, MSGBOX_DEFAULT
	load_field_pic 8, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_8, MSGBOX_DEFAULT
	destroy_field_pic 8, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_9, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_Duo_Mugshot
	closemessage
	setvar VAR_UNUSED_0x404E, 1
	setflag FLAG_HIDE_ZAURAK_LAB_CLAIR_TOUR_SETUP
	end

ZaurakCity_Hazel_Lab_Meeting_Hazel::
	faceplayer
	call Common_EventScript_SetupPlayer_MugShot
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_9, MSGBOX_NPC
	call Common_EventScript_RemovePlayer_MugShot
	closemessage
	end

ZaurakCity_Hazel_Lab_Meeting_Clair::
	faceplayer
	call Common_EventScript_SetupPlayer_MugShot
	load_field_pic 8, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_8, MSGBOX_NPC
	call Common_EventScript_RemovePlayer_MugShot
	destroy_field_pic 8, VAR_0x8009
	closemessage
	end
	
ZaurakCity_Hazel_Lab_Meeting_Rival::
	faceplayer
	call Common_EventScript_SetupRival_MugShot
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_Rival, MSGBOX_NPC
	call Common_EventScript_RemoveRival_MugShot
	closemessage
	end

ZaurakCity_Heal_Station::
	msgbox ZaurakCity_Heal_Station_Text, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, ZaurakCity_No_Heal
	case YES, ZaurakCity_Heal
	end

ZaurakCity_Heal::
	special HealPlayerParty
	playfanfare MUS_ME_ASA
	waitfanfare
	closemessage
	end

ZaurakCity_No_Heal::
	closemessage
	end

ZaurakCity_EventScript_ReceivePokedex::
	playfanfare MUS_FANFA4
	message ZaurakCity_Text_ReceivedPokedex
	waitfanfare
	setflag FLAG_SYS_POKEDEX_GET
	special SetUnlockedPokedexFlags
	setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH
	setvar VAR_CABLE_CLUB_TUTORIAL_STATE, 1
	giveitem ITEM_POKE_BALL, 5
	return
	
ZaurakCity_Hazel_Lab_Walk_Left:
	walk_right
	step_end

ZaurakCity_Hazel_Lab_Walk_Machine:
	walk_right
	walk_right
	face_up
	step_end

Zaurak_Hazel_Lab_Walk_Trainers:
	walK_left
	walK_left
	walk_in_place_down
	delay_16
	delay_16
	walK_left
	walk_in_place_down
	delay_16
	delay_16
	step_end

Zaurak_Hazel_Lab_Walk_Clair:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	face_left
	step_end
	
ZaurakCity_Hazel_Lab_Meeting_Text_1_Derek:
	.string "Hello. You're {PLAYER} & Kate right?\p"
	.string "I'm professor Hazel. It's a pleasure\n"
	.string "to meet you \p"
	.string "I take it that your teacher has briefed\n"
	.string "you about my situation. Correct?$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_1_Kate:
	.string "Hello. You're {PLAYER} & Derek right?\p"
	.string "I'm professor Hazel. It's a pleasure\n"
	.string "to meet you \p"
	.string "I take it that your teacher has briefed\n"
	.string "you about my situation. Correct?$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_2_Derek:
	.string "Kate: Yeah, you are in need of two\n" 
	.string "trainers for your studies.$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_2_Kate:
	.string "Derek: Yeah, you are in need of two\n" 
	.string "trainers for your studies.$"

ZaurakCity_Hazel_Lab_Meeting_Text_3:
	.string "Hazel: That's right.\p"
	.string "The academy was kind enough to give \n"
	.string "me some candidates and you two had the\p"
	.string "qualifications I was looking for.\p"
	.string "May I have a look at your Pokémon?$"

ZaurakCity_Hazel_Lab_Meeting_Text_4:
	.string "It seems your Pokémon have already\n"
	.string "grown closer to you. \p"
	.string "I'd like to see what other effects\n"
	.string	"travelling with you will have on your.\p"
	.string "Pokémon. So I have a proposal.$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_5:
	.string "Professor Hazel.\n"
	.string "The Pokédex as you requested$"

ZaurakCity_Hazel_Lab_Meeting_Text_6:
	.string "Thank you, Clair.\p"
	.string "I want you to take these two Pokédexes.$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_7:
	.string "During your jouurney you will meet all\n"
	.string "sorts of Pokémon.\p"
	.string "Pokémon you may form bonds with.\p"
	.string "I do want you to have fun on your.\n"
	.string "journey. Do not hestitate to let me\p"
	.string "or Clair know about your progress.$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_8:
	.string "Clair: I'll be monitoring your progress.\n"
	.string "If you need anything, just ask.$"

ZaurakCity_Hazel_Lab_Meeting_Text_9:
	.string "I hope you'll be making great\n"
	.string "memories with your Pokémon.\p"
	.string "I'll be looking forward to see\n"
	.string "your progress.$"

ZaurakCity_Text_ReceivedPokedex: @ 81FAC32
	.string "{PLAYER} received the POKéDEX!$"	

ZaurakCity_Hazel_Lab_Meeting_Text_Rival:
	.string "We're really doing this.\p"
	.string "We're going to become Pokémon\n"
	.string "trainers!$"

ZaurakCity_Heal_Station_Text:
	.string "Would you like to heal your Pokémon?$"
