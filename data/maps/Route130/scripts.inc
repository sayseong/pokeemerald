Route130_MapScripts:: @ 81F735F
	map_script MAP_SCRIPT_ON_TRANSITION, Route130_OnTransition
	.byte 0

Route130_OnTransition: @ 81F7365
	call Common_EventScript_SetupRivalGfxId
	call_if_set FLAG_DEFEATED_RIVAL_ROUTE103, Zaurak_Bridge_SetState
	end

Zaurak_Bridge_SetState::
	setvar VAR_ROUTE130_STATE, 1
	return

Zaurak_Bridge_Conversation:: @ 82078F3
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Zaurak_Bridge_Conversation_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq Zaurak_Bridge_Conversation_Kate
	end
	
Zaurak_Bridge_Conversation_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Zaurak_Bridge_Rival_Text_First_Battle, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	switch VAR_STARTER_MON
	case 0, Zaurak_Bridge_StartKateBattleSnivy
	case 1, Zaurak_Bridge_StartKateBattleTorchic
	case 2, Zaurak_Bridge_StartKateBattleFroakie
	return
	
Zaurak_Bridge_Conversation_Kate::	
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Zaurak_Bridge_Rival_Text_First_Battle, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	switch VAR_STARTER_MON,
	case 0, Zaurak_Bridge_StartDerekBattleSnivy
	case 1, Zaurak_Bridge_StartDerekBattleTorchic
	case 2, Zaurak_Bridge_StartDerekBattleFroakie
	return

Zaurak_Bridge_StartDerekBattleSnivy::
	trainerbattle_no_intro TRAINER_DEREK_ZAURAK_BRIDGE_TORCHIC, Zaurak_Bridge_Rival_Defeated
	goto Zaurak_Bridge_After_Battle_Kate
	end

Zaurak_Bridge_StartDerekBattleTorchic::
	trainerbattle_no_intro TRAINER_DEREK_ZAURAK_BRIDGE_FROAKIE, Zaurak_Bridge_Rival_Defeated
	goto Zaurak_Bridge_After_Battle_Kate
	end

Zaurak_Bridge_StartDerekBattleFroakie::
	trainerbattle_no_intro TRAINER_DEREK_ZAURAK_BRIDGE_SNIVY, Zaurak_Bridge_Rival_Defeated
	goto Zaurak_Bridge_After_Battle_Kate
	end

Zaurak_Bridge_StartKateBattleSnivy::
	trainerbattle_no_intro TRAINER_KATE_ZAURAK_BRIDGE_TORCHIC, Zaurak_Bridge_Rival_Defeated
	goto Zaurak_Bridge_After_Battle_Derek
	end
	
Zaurak_Bridge_StartKateBattleTorchic::
	trainerbattle_no_intro TRAINER_KATE_ZAURAK_BRIDGE_FROAKIE, Zaurak_Bridge_Rival_Defeated
	goto Zaurak_Bridge_After_Battle_Derek
	end

Zaurak_Bridge_StartKateBattleFroakie::
	trainerbattle_no_intro TRAINER_KATE_ZAURAK_BRIDGE_SNIVY, Zaurak_Bridge_Rival_Defeated
	goto Zaurak_Bridge_After_Battle_Derek
	end

Zaurak_Bridge_After_Battle_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Zaurak_Bridge_Derek_Text_First_Battle_End, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	closemessage
	applymovement 1, Rival_Walk_Away
	waitmovement 0
	removeobject 1
	setvar VAR_ROUTE130_STATE, 1
	setflag FLAG_DEFEATED_RIVAL_ROUTE103
	end
	
Zaurak_Bridge_After_Battle_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Zaurak_Bridge_Derek_Text_First_Battle_End, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	closemessage
	applymovement 1, Rival_Walk_Away
	waitmovement 0
	removeobject 1
	setvar VAR_ROUTE130_STATE, 1
	setflag FLAG_DEFEATED_RIVAL_ROUTE103
	end

Trigger_EventScript_Rival_1::
	applymovement 1, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 1, Trigger_Rival_Walk_1
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	goto Zaurak_Bridge_Conversation
	end

Trigger_EventScript_Rival_2::
	applymovement 1, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 1, Trigger_Rival_Walk_2
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceLeft
	waitmovement 0
	goto Zaurak_Bridge_Conversation
	end

Trigger_EventScript_Rival_3::
	applymovement 1, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 1, Common_Movement_FaceLeft
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	goto Zaurak_Bridge_Conversation
	end

Trigger_EventScript_Rival_4::
	applymovement 1, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 1, Common_Movement_FaceRight
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceLeft
	waitmovement 0
	goto Zaurak_Bridge_Conversation
	end

Trigger_Rival_Walk_1:
	walk_left
	step_end

Trigger_Rival_Walk_2:
	walk_right
	step_end

Rival_Walk_Away:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end
	
Zaurak_Bridge_Rival_Text_First_Battle:
	.string "Hey {PLAYER}. Ready to head to\n"
	.string "Zaurak City?\p"
	.string "Before we go to see Professor\n"
	.string "Hazel. How about a friendly\p"
	.string "Battle?$"

Zaurak_Bridge_Rival_Defeated:
	.string "That was awesome. You're really\n"
	.string "good at this, {PLAYER}$"
	
Zaurak_Bridge_Derek_Text_First_Battle_End:
	.string "This is so cool. We are actual\n"
	.string "Pokémon Trainers now.\p"
	.string "Let's go meet the professor.\n"
	.string "I'll see you on the other side.$"
	
@special ChooseStarter To choose starter

