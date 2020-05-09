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
	msgbox ZaurakCity_Fat_Man_Text, MSGBOX_SIGN	
	end
	
ZaurakCity_Rival_Text_First_Battle:
	.string "Ready to head inside {PLAYER}?$"
		
ZaurakCity_Fat_Man_Text:	
	.string "He is fast asleep.$"

ZaurakCity_Rival_Waits_Text:
	.string "Oh, Alright.\n "
	.string "I'll wait here until you are ready.$"

ZaurakCity_GoTo_Lab_Text:
	.string "Alright! Let's go!$"

ZaurakCity_Blockade_Text:
	.string "Beat it, kid.\n"
	.string "We are busy here!$"

ZaurakCity_Rival_Conversation:
	.string "Hi!$"
