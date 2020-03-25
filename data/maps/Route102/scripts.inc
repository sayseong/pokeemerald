Route102_MapScripts:: @ 81EC0E0
	map_script MAP_SCRIPT_ON_TRANSITION, Route02_OnTransition
	.byte 0

Route02_OnTransition::
	call_if_set FLAG_HIDE_ROUTE02_CLAIR, Route02_SetState
	end

Route02_SetState::
	setvar VAR_ROUTE102_STATE, 1
	return

Route02_Clair_Trigger_Left::
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_FaceLeft
	applymovement 255, Common_Movement_FaceRight
	waitmovement 0
	goto Route02_Clair_EventScript
	end

Route02_Clair_Trigger_Right::	
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_FaceRight
	applymovement 255, Common_Movement_FaceLeft
	waitmovement 0
	goto Route02_Clair_EventScript
	end

Route02_Clair_EventScript::
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route02_Male_Player
	compare VAR_RESULT, FEMALE
	call_if_eq Route02_Female_Player
	load_field_pic 8, 200, 83, VAR_0x8009
	switch VAR_STARTER_MON	
	case 0, Route02_Clair_Eventscript_Snivy
	case 1, Route02_Clair_Eventscript_Torchic
	case 2, Route02_Clair_Eventscript_Froakie
	end

Route02_Male_Player::
	load_field_pic 1, 40, 83, VAR_0x8008 
	return

Route02_Female_Player::
	load_field_pic 3, 40, 83, VAR_0x8008 
	return

Route02_Male_Player_Delete::
	destroy_field_pic 1, VAR_0x8008 
	return

Route02_Female_Player_Delete::
	destroy_field_pic 3, VAR_0x8008
	return

Route02_Clair_Eventscript_Snivy::
	msgbox Route02_Clair_Conversation_Snivy, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route02_Clair_Trainer_Battle
	compare VAR_RESULT, NO
	goto_if_eq Route02_Clair_Postpone_Battle
	end

Route02_Clair_Postpone_Battle::
	msgbox Route02_Clair_Declined_Battle, MSGBOX_DEFAULT
	destroy_field_pic 8, VAR_0x8009
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route02_Male_Player_Delete
	compare VAR_RESULT, FEMALE
	call_if_eq Route02_Female_Player_Delete
	closemessage
	applymovement 255, Route02_Player_Walk_Away
	waitmovement 0
	end

Route02_Clair_Eventscript_Torchic::
	msgbox Route02_Clair_Conversation_Torchic, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route02_Clair_Trainer_Battle
	compare VAR_RESULT, NO
	goto_if_eq Route02_Clair_Postpone_Battle
	end

Route02_Clair_Eventscript_Froakie::
	msgbox Route02_Clair_Conversation_Froakie, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route02_Clair_Trainer_Battle
	compare VAR_RESULT, NO
	goto_if_eq Route02_Clair_Postpone_Battle
	end

Route02_Clair_Trainer_Battle::
	trainerbattle_single TRAINER_CLAIR_ROUTE_02, Route02_ClairIntro, Route02_Clair_Defeated, Route02_Clair_PostBattle_EventScript, NO_MUSIC
	end
	
Route02_Clair_PostBattle_EventScript::	
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route02_Male_Player
	compare VAR_RESULT, FEMALE
	call_if_eq Route02_Female_Player
	load_field_pic 8, 200, 83, VAR_0x8009
	msgbox Route02_Clair_PostBattle, MSGBOX_DEFAULT
	giveitem ITEM_HERBAL_MEDICINE
	msgbox Route02_Clair_Medicine_Text, MSGBOX_DEFAULT
	destroy_field_pic 8, VAR_0x8009
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route02_Male_Player_Delete
	compare VAR_RESULT, FEMALE
	call_if_eq Route02_Female_Player_Delete
	closemessage
	applymovement 4, Route02_Clair_Exit
	waitmovement 0
	removeobject 4
	setflag FLAG_HIDE_ROUTE02_CLAIR
	setvar VAR_ROUTE102_STATE, 1
	releaseall
	end

Route02_Youngster_Trainer:: @ 81F2B3E
	trainerbattle_single TRAINER_CALVIN_1, Route102_Text_CalvinIntro, Route102_Text_CalvinDefeated	
	msgbox Route102_Text_CalvinPostBattle, MSGBOX_AUTOCLOSE
	end
	
Route02_Lass_Trainer:: @ 81F2B3E
	trainerbattle_single TRAINER_TIANA, Route102_Text_TianaIntro, Route102_Text_TianaDefeated
	msgbox Route102_Text_TianaPostBattle, MSGBOX_AUTOCLOSE
	end

Route02_Fisherman::
	faceplayer
	msgbox Route02_Fisherman_Text, MSGBOX_NPC
	end
	
Route02_Fisherman_Text:
	.string "I like fishing.$"

Route02_Clair_Exit:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

Route02_Player_Walk_Away:
	walk_down
	step_end

Route02_Clair_Conversation_Snivy:
	.string "Ah {PLAYER}. How are things between\n"
    .string "you and Snivy?\p"
	.string "It must have taken quite a liking to you\p"
	.string "If you don't mind I'd like to see how \n"
	.string "you perform in battle. Are you ready?$"

Route02_Clair_Conversation_Torchic:
	.string "Ah {PLAYER}. How are things between\n"
    .string "you and Torchic?\p"
	.string "It must have taken quite a liking to you\p"
	.string "If you don't mind I'd like to see how\n" 
	.string "you perform in battle. Are you ready?$"

Route02_Clair_Conversation_Froakie:
	.string "Ah {PLAYER}. How are things between\n"
    .string "you and Froakie?\p"
	.string "It must have taken quite a liking to you\p"
	.string "If you don't mind I'd like to see how \n"
	.string "you perform in battle. Are you ready?$"

Route02_Clair_Declined_Battle:
	.string "I'll wait here until you are ready.$"
	
Route02_ClairIntro:
	.string "Alright, let's battle!$"
	
Route02_Clair_Defeated:
	.string "Impressive!$"
	
Route02_Clair_PostBattle:
	.string "Not bad. Although I should have expected\n"
	.string "such from a trainer from Deneb Academy.\p"
	.string "I'd like you to have this.\n"
	.string "In case any of your Pokémon gets hurt.$"

Route02_Clair_Medicine_Text:
	.string "I made it myself.\p"
	.string "I am actually studying to become a\n"
	.string "Doctor.\p"
	.string "So if you and your Pokémon ever need it,\n"
	.string "I'll gladly help.$"
