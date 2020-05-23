SlateportCity_MapScripts:: @ 81DCC61
	map_script MAP_SCRIPT_ON_TRANSITION, ZaurakCity_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ZaurakCity_OnFrame
	.byte 0

ZaurakCity_OnTransition: @ 81F7365
	call Common_EventScript_SetupRivalGfxId
	call_if_set FLAG_HIDE_ZAURAK_LAB_CLAIR_TOUR_SETUP, ZaurakCity_Load_State_1
	call_if_set FLAG_HIDE_ZAURAKCITY_TOUR_FINISH, ZaurakCity_Load_State_2
	compare VAR_SLATEPORT_CITY_STATE, 1
	call_if_eq EventScript_ZaurakCity_1
	end

ZaurakCity_Load_State_1::
	setvar VAR_SLATEPORT_CITY_STATE, 1
	return

ZaurakCity_Load_State_2::
	setvar VAR_SLATEPORT_CITY_STATE, 2
	return
	
ZaurakCity_OnFrame:	
	map_script_2 VAR_SLATEPORT_CITY_STATE, 1, ZaurakCity_Rival_Meeting
	.2byte 0

EventScript_ZaurakCity_1::
	setobjectxyperm 1, 9, 30
	setobjectmovementtype 1, MOVEMENT_TYPE_FACE_UP
	clearflag FLAG_HIDE_ZAURAKCITY_RIVAL
	return

ZaurakCity_Rival_Meeting::
	msgbox ZaurakCity_Rival_Conversation, MSGBOX_DEFAULT
	setvar VAR_SLATEPORT_CITY_STATE, 2
	setflag FLAG_HIDE_ZAURAKCITY_RIVAL
	setflag FLAG_HIDE_ZAURAKCITY_TOUR_FINISH
	closemessage
	end
	
ZaurakCity_Conversation_Rival:: @ 82078F3
	call Common_EventScript_SetupRival_MugShot
	msgbox ZaurakCity_Rival_Text_First_Battle, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, ZaurakCity_Rival_Waits
	case YES, ZaurakCity_Head_Into_Lab
	end

ZaurakCity_Seraph_Blockade::
	call Common_EventScript_SetupPlayer_MugShot
	applymovement 11, Common_Movement_FaceDown
	applymovement 12, Common_Movement_FaceDown
	load_field_pic 9, 200, 83, VAR_0x8009
	load_field_pic 10, 160, 83, VAR_0x800A
	msgbox ZaurakCity_Blockade_Text, MSGBOX_DEFAULT	
	call Common_EventScript_RemovePlayer_MugShot	
	destroy_field_pic 9, VAR_0x8009
	destroy_field_pic 10, VAR_0x800A
	closemessage
	applymovement 11, Common_Movement_FaceRight
	applymovement 12, Common_Movement_FaceLeft
	end
	
ZaurakCity_Head_Into_Lab::
	msgbox ZaurakCity_GoTo_Lab_Text, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_MugShot
	closemessage
	opendoor 9, 28
	applymovement 1, Common_Movement_WalkUp
	waitmovement 0
	removeobject 1
	setflag FLAG_HIDE_ZAURAKCITY_RIVAL
	end

ZaurakCity_Rival_Waits::
	msgbox ZaurakCity_Rival_Waits_Text, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_MugShot
	closemessage
	return
	
ZaurakCity_Fat_Man::
	msgbox ZaurakCity_Fat_Man_Text, MSGBOX_DEFAULT
	closemessage
	end

ZaurakCity_MarillDoll::
	msgbox ZaurakCity_MarillDoll_Text, MSGBOX_DEFAULT
	closemessage
	end

ZaurakCity_Old_Lady_Lake::
	msgbox ZaurakCity_Old_Lady_Lake_Text, MSGBOX_NPC
	end

ZaurakCity_Little_Girl::
	msgbox ZaurakCity_Little_Girl_Text, MSGBOX_NPC
	end

ZaurakCity_Balding_Man::
	msgbox ZaurakCity_Balding_Man_Text, MSGBOX_NPC
	end

ZaurakCity_Gentleman::
	msgbox ZaurakCity_Gentleman_Text, MSGBOX_NPC
	end

ZaurakCity_Gameboy_Kid::
	msgbox ZaurakCity_Gameboy_Kid_Text, MSGBOX_NPC
	end

ZaurakCity_Old_Man::
	msgbox ZaurakCity_Old_Man_Text, MSGBOX_NPC
	end

ZaurakCity_Old_Woman::
	msgbox ZaurakCity_Old_Woman_Text, MSGBOX_NPC
	end
	
ZaurakCity_Subway_Sign::
	msgbox ZaurakCity_Subway_Sign_Text, MSGBOX_SIGN
	end

ZaurakCity_Hotel_Sign::
	msgbox ZaurakCity_Hotel_Sign_Text, MSGBOX_SIGN
	end
	
ZaurakCity_Sign::
	msgbox ZaurakCity_Sign_Text, MSGBOX_SIGN
	end
	
ZaurakCity_Lake_Sign::
	msgbox ZaurakCity_Lake_Sign_Text, MSGBOX_SIGN
	end
	
ZaurakCity_Rival_Text_First_Battle:
	.string "Ready to head inside {PLAYER}?$"
		
ZaurakCity_Fat_Man_Text:	
	.string "He is fast asleep.$"

ZaurakCity_MarillDoll_Text:
	.string "It's a Marill Poké-doll!$"

ZaurakCity_Little_Girl_Text:
	.string "Marill is my favorite Pokémon!$"

ZaurakCity_Old_Lady_Lake_Text:
	.string "This is my favourite spot in the city.$"

ZaurakCity_Balding_Man_Text:
	.string "My daughter loves Marill.\p"
	.string "If only I could find one to give\n"
	.string "her for her birthday.$"

ZaurakCity_Gentleman_Text:
	.string "Have you checked out this store?\p"
	.string "You can find some neat stuff useful\n"
	.string "for new trainers.$"
	
ZaurakCity_Rival_Waits_Text:
	.string "Oh, Alright.\n "
	.string "I'll wait here until you are ready.$"

ZaurakCity_GoTo_Lab_Text:
	.string "Alright! Let's go!$"

ZaurakCity_Blockade_Text:
	.string "Beat it, kid.\n"
	.string "We are busy here!$"

ZaurakCity_Subway_Sign_Text:
	.string "Zaurak City Subway Station$"

ZaurakCity_Hotel_Sign_Text:
	.string "Zaurak City Hotel$"

ZaurakCity_Sign_Text:
	.string "Zaurak City\n"
	.string "The City of Tomorrow$"
	
ZaurakCity_Lake_Sign_Text:
	.string "Zaurak Park$"

ZaurakCity_Rival_Conversation:
	.string "Hi!$"
	
ZaurakCity_Gameboy_Kid_Text:
	.string "Have you tried Pokémon Rose?\n"
	.string "It's amazing!$"

ZaurakCity_Old_Man_Text:
	.string "The fountain is so soothing.$"

ZaurakCity_Old_Woman_Text:
	.string "My husband and I come here\n"
	.string "everyday to watch the Fountain.$"
