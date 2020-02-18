SlateportCity_MapScripts:: @ 81DCC61
	map_script MAP_SCRIPT_ON_TRANSITION, ZaurakCity_OnTransition
	.byte 0

ZaurakCity_OnTransition: @ 81F7365
	call Common_EventScript_SetupRivalGfxId
	end

ZaurakCity_Conversation_Rival:: @ 82078F3
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq ZaurakCity_Conversation_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq ZaurakCity_Conversation_Kate
	end
	
ZaurakCity_Conversation_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Rival_Text_First_Battle, MSGBOX_NPC	
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	return
	
ZaurakCity_Conversation_Kate::	
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox ZaurakCity_Rival_Text_First_Battle,MSGBOX_NPC
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	return
	
ZaurakCity_Fat_Man::
		msgbox ZaurakCity_Fat_Man_Text, MSGBOX_SIGN	
		end
	
ZaurakCity_Rival_Text_First_Battle:
	.string "Ready to head inside {PLAYER}?$"
		
ZaurakCity_Fat_Man_Text:	
	.string "He is fast asleep.$"
