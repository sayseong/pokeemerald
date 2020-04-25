SlateportCity_MapScripts:: @ 81DCC61
	map_script MAP_SCRIPT_ON_TRANSITION, ZaurakCity_OnTransition
	.byte 0

ZaurakCity_OnTransition: @ 81F7365
	call Common_EventScript_SetupRivalGfxId
	call_if_set FLAG_HIDE_ZAURAK_LAB_CLAIR_TOUR_SETUP, ZaurakCity_Load_State_1
	compare VAR_SLATEPORT_CITY_STATE, 1
	call_if_eq EventScript_ZaurakCity_1
	end

ZaurakCity_Load_State_1::
	setvar VAR_SLATEPORT_CITY_STATE, 1
	return

EventScript_ZaurakCity_1::
	setobjectxyperm 1, 9, 30
	setobjectmovementtype 1, MOVEMENT_TYPE_FACE_UP
	clearflag FLAG_HIDE_ZAURAKCITY_RIVAL
	return
	
ZaurakCity_Conversation_Rival:: @ 82078F3
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq ZaurakCity_Conversation_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq ZaurakCity_Conversation_Kate
	end
	
ZaurakCity_Conversation_Derek::
	faceplayer
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Rival_Text_First_Battle, MSGBOX_YESNO		
	switch VAR_RESULT
	case NO, ZaurakCity_Rival_Waits_Derek
	case YES, ZaurakCity_Head_Into_Lab_Derek
	return
	
ZaurakCity_Conversation_Kate::	
	faceplayer
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Rival_Text_First_Battle, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, ZaurakCity_Rival_Waits_Kate
	case YES, ZaurakCity_Head_Into_Lab_Kate
	return

ZaurakCity_Head_Into_Lab_Derek::
	msgbox ZaurakCity_GoTo_Lab_Text, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	closemessage
	opendoor 9, 28
	applymovement 1, Common_Movement_WalkUp
	waitmovement 0
	removeobject 1
	setflag FLAG_HIDE_ZAURAKCITY_RIVAL
	end

ZaurakCity_Head_Into_Lab_Kate::
	msgbox ZaurakCity_GoTo_Lab_Text, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	closemessage
	opendoor 9, 28
	applymovement 1, Common_Movement_WalkUp
	waitmovement 0
	removeobject 1
	setflag FLAG_HIDE_ZAURAKCITY_RIVAL
	end

ZaurakCity_Rival_Waits_Derek::
	msgbox ZaurakCity_Rival_Waits_Text, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	closemessage
	return

ZaurakCity_Rival_Waits_Kate::
	msgbox ZaurakCity_Rival_Waits_Text, MSGBOX_DEFAULT	
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
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
