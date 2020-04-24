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
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq ZaurakCity_Hazel_Lab_Meeting_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq ZaurakCity_Hazel_Lab_Meeting_Kate
	end

ZaurakCity_Hazel_Lab_Meeting_Derek::
	load_field_pic 7, 40, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_1_Derek, MSGBOX_DEFAULT
	applymovement 4, Common_Movement_WalkInPlaceUp
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_2_Derek, MSGBOX_DEFAULT
	applymovement 2, Common_Movement_WalkInPlaceDown
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_3, MSGBOX_DEFAULT
	destroy_field_pic 7, VAR_0x8008
	closemessage
	applymovement 2, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 2, ZaurakCity_Hazel_Lab_Walk_Left
	waitmovement 0
	applymovement 2, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 2, ZaurakCity_Hazel_Lab_Walk_Machine
	waitmovement 0
	applymovement 2, Zaurak_Hazel_Lab_Walk_Trainers
	waitmovement 0 	
	load_field_pic 7, 40, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_4, MSGBOX_DEFAULT
	destroy_field_pic 7, VAR_0x8008
	closemessage	
	applymovement 1, Zaurak_Hazel_Lab_Walk_Clair
	waitmovement 0
	applymovement 2, Common_Movement_FaceRight
	waitmovement 0
	load_field_pic 8, 200, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_5, MSGBOX_DEFAULT
	applymovement 2, Common_Movement_FaceDown
	waitmovement 0
	destroy_field_pic 8, VAR_0x8008
	load_field_pic 7, 40, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_6, MSGBOX_DEFAULT
	call ZaurakCity_EventScript_ReceivePokedex
	applymovement 1, Common_Movement_FaceDown
	waitmovement 0
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_7, MSGBOX_DEFAULT
	load_field_pic 8, 200, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_8, MSGBOX_DEFAULT
	destroy_field_pic 8, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_9, MSGBOX_DEFAULT
	destroy_field_pic 7, VAR_0x8009
	closemessage
	setvar VAR_UNUSED_0x404E, 1
	setflag FLAG_HIDE_ZAURAK_LAB_CLAIR_TOUR_SETUP
	end

ZaurakCity_Hazel_Lab_Meeting_Kate::
	load_field_pic 6, 40, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_1_Kate, MSGBOX_DEFAULT
	applymovement 4, Common_Movement_WalkInPlaceUp
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_2_Kate, MSGBOX_DEFAULT
	applymovement 2, Common_Movement_WalkInPlaceDown
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_3, MSGBOX_DEFAULT
	destroy_field_pic 6, VAR_0x8008
	closemessage
	applymovement 2, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 2, ZaurakCity_Hazel_Lab_Walk_Left
	waitmovement 0
	applymovement 2, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 2, ZaurakCity_Hazel_Lab_Walk_Machine
	waitmovement 0
	applymovement 2, Zaurak_Hazel_Lab_Walk_Trainers
	waitmovement 0 	
	load_field_pic 6, 40, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_4, MSGBOX_DEFAULT
	destroy_field_pic 6, VAR_0x8008
	closemessage	
	applymovement 1, Zaurak_Hazel_Lab_Walk_Clair
	waitmovement 0
	applymovement 2, Common_Movement_FaceRight
	waitmovement 0
	load_field_pic 8, 200, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_5, MSGBOX_DEFAULT
	applymovement 2, Common_Movement_FaceDown
	waitmovement 0
	destroy_field_pic 8, VAR_0x8008
	load_field_pic 6, 40, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_6, MSGBOX_DEFAULT
	call ZaurakCity_EventScript_ReceivePokedex
	applymovement 1, Common_Movement_FaceDown
	waitmovement 0
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_7, MSGBOX_DEFAULT
	load_field_pic 8, 200, 83, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_8, MSGBOX_DEFAULT
	destroy_field_pic 8, VAR_0x8008
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_9, MSGBOX_DEFAULT
	destroy_field_pic 6, VAR_0x8009
	closemessage
	setvar VAR_UNUSED_0x404E, 1
	setflag FLAG_HIDE_ZAURAK_LAB_CLAIR_TOUR_SETUP
	end

ZaurakCity_Hazel_Lab_Meeting_Hazel::
	faceplayer
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_9, MSGBOX_DEFAULT
	closemessage
	end

ZaurakCity_Hazel_Lab_Meeting_Clair::
	faceplayer
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq ZaurakCity_Hazel_Lab_Meeting_Clair_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq ZaurakCity_Hazel_Lab_Meeting_Clair_Kate
	end

ZaurakCity_Hazel_Lab_Meeting_Clair_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 8, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_8, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 8, VAR_0x8009
	closemessage
	end
	
ZaurakCity_Hazel_Lab_Meeting_Clair_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 8, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_8, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 8, VAR_0x8009
	closemessage
	end

ZaurakCity_Hazel_Lab_Meeting_Rival::
	faceplayer
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq ZaurakCity_Hazel_Lab_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq ZaurakCity_Hazel_Lab_Kate
	closemessage
	end

ZaurakCity_Hazel_Lab_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_Rival, MSGBOX_DEFAULT
	closemessage
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	end

ZaurakCity_Hazel_Lab_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_Rival, MSGBOX_DEFAULT
	closemessage
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	end

ZaurakCity_EventScript_ReceivePokedex::
	playfanfare MUS_FANFA4
	message ZaurakCity_Text_ReceivedPokedex
	waitfanfare
	setflag FLAG_SYS_POKEDEX_GET
	special sub_81AFDA0  @ sets flags that are never read
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
	delay_16
	delay_16
	delay_16
	delay_16
	step_end

Zaurak_Hazel_Lab_Walk_Trainers:
	walK_left
	walK_left
	face_down
	delay_16
	delay_16
	walK_left
	face_down
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
	.string	"travelling with you. So I have a proposal.$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_5:
	.string "Professor Hazel.\n"
	.string "The Pokédex as you requested$"

ZaurakCity_Hazel_Lab_Meeting_Text_6:
	.string "Thank you, Clair.\p"
	.string "I want you to take these two Pokédexes.$"
	
ZaurakCity_Hazel_Lab_Meeting_Text_7:
	.string "During your jouurney you will meet many\n"
	.string "all sorts of Pokémon.\p"
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
