Route101_MapScripts:: @ 81EBCBA
	map_script MAP_SCRIPT_ON_TRANSITION, Route101_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route101_OnFrame
	.byte 0

Route101_OnTransition: @ 81EBCC5
	call Common_EventScript_SetupRivalGfxId
	end

Route101_OnFrame: @ 81EBCCB
	@map_script_2 VAR_ROUTE101_STATE, 0, Route101_EventScript_HideMapNamePopup
	map_script_2 VAR_ROUTE101_STATE, 0, Route_1_Lesson
	.2byte 0

Route101_EventScript_HideMapNamePopup:: @ 81E7E4B
	setflag FLAG_HIDE_MAP_NAME_POPUP
	return

Route_1_Lesson::	
	applymovement EVENT_OBJ_ID_PLAYER, Face_Teala
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Lesson_Message MSGBOX_DEFAULT	
	destroy_field_pic 5, VAR_0x8009
	msgbox Route_1_Skip_lesson MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route_1_After_Lesson
	compare VAR_RESULT, NO
	goto_if_eq Route_1_Begin_Lesson
	end

Route_1_Begin_Lesson::
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Lesson_Start, MSGBOX_DEFAULT
	special SavePlayerParty
	special PutZigzagoonInPlayerParty
	destroy_field_pic 5, VAR_0x8009
	applymovement 1, Teala_Into_Grass 	
	waitmovement 0
	special StartWallyTutorialBattle
	waitstate
	special LoadPlayerParty
	applymovement 1, Teala_Out_Of_Grass 	
	msgbox Route_1_Lesson_Finished, MSGBOX_DEFAULT
	goto Route_1_After_Lesson
	end

Route_1_After_Lesson::	
	applymovement 2, Common_Movement_FaceLeft
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Teala_Lesson_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Teala_Lesson_Kate
	applymovement 1, Teala_To_player
	applymovement 3, Teala_Leaves
	applymovement 4, Teala_Leaves
	msgbox Teala_Present_Task, MSGBOX_DEFAULT
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_ExclamationMark
	applymovement 2, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceUp
	applymovement 2, Common_Movement_FaceUp
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_About_To_Receive_Pokemon_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_About_To_Receive_Pokemon_Kate
	applymovement 2, Jumping_In_Excitement
	waitmovement 0
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceRight
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Player_Choose_Starter_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Player_Choose_Starter_Kate
	removeobject 3
	removeobject 4	
	setflag FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG
	special ChooseStarter
	waitstate
	setflag FLAG_SYS_POKEMON_GET
	switch VAR_STARTER_MON
	case 0, Route_1_Rival_Gets_Torchic
	case 1, Route_1_Rival_Gets_Froakie
	case 2, Route_1_Rival_Gets_Snivy
	end
	
Route_1_Obtained_Starter::	
	applymovement EVENT_OBJ_ID_PLAYER, Common_Movement_FaceUp
	applymovement 2, Common_Movement_FaceUp
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Teala_Objective_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Teala_Objective_Kate
	closemessage
	applymovement 1, Teala_Leaves
	waitmovement 0
	removeobject 1
	applymovement 255, Common_Movement_FaceRight
	applymovement 2, Common_Movement_FaceLeft
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Wait_Bridge_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Wait_Bridge_Kate
	closemessage
	applymovement 2, Rival_Runs_Bridge
	waitmovement 0
	removeobject 2
	setvar VAR_ROUTE101_STATE, 2
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	releaseall
	end
	
Route_1_Teala_Lesson_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Route_1_Rival_Text, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	return
	
Route_1_Teala_Lesson_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Route_1_Rival_Text, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	return

Route_1_About_To_Receive_Pokemon_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 3, 80, 83, VAR_0x800A
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Receiving_Pokemon_Derek, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 3, VAR_0x800A
	destroy_field_pic 5, VAR_0x8009
	return

Route_1_About_To_Receive_Pokemon_Kate::
	load_field_pic 1, 80, 83, VAR_0x800A
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Receiving_Pokemon_Kate, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x800A
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 5, VAR_0x8009
	return

Route_1_Player_Choose_Starter_Derek::
	load_field_pic 1, 40, 83, VAR_0x8009
	load_field_pic 4, 200, 83, VAR_0x8008
	msgbox Rival_Gives_First_Choice, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8009
	destroy_field_pic 4, VAR_0x8008
	return
	
Route_1_Player_Choose_Starter_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Rival_Gives_First_Choice, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	return

Route_1_Rival_Gets_Snivy::
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Derek_Gets_Snivy
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Kate_Gets_Snivy
	return
	
Route_1_Rival_Gets_Torchic::
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Derek_Gets_Torchic
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Kate_Gets_Torchic
	return

Route_1_Rival_Gets_Froakie::
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route_1_Derek_Gets_Froakie
	compare VAR_RESULT, FEMALE
	call_if_eq Route_1_Kate_Gets_Froakie
	return

Route_1_Derek_Gets_Snivy::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Rival_Picks_Snivy_Text MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	goto Route_1_Obtained_Starter
	return
	
Route_1_Kate_Gets_Snivy::	
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Rival_Picks_Snivy_Text, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	goto Route_1_Obtained_Starter
	return

Route_1_Derek_Gets_Torchic::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Rival_Picks_Torchic_Text MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	goto Route_1_Obtained_Starter
	return
	
Route_1_Kate_Gets_Torchic::	
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Rival_Picks_Torchic_Text, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	goto Route_1_Obtained_Starter
	return
	
Route_1_Derek_Gets_Froakie::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Rival_Picks_Froakie_Text MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	goto Route_1_Obtained_Starter
	return
	
Route_1_Kate_Gets_Froakie::	
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Rival_Picks_Froakie_Text, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	goto Route_1_Obtained_Starter
	return	

Route_1_Teala_Objective_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Objective_Text, MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 5, VAR_0x8009
	return

Route_1_Teala_Objective_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Objective_Text, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 5, VAR_0x8009
	return

Route_1_Wait_Bridge_Derek::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
	msgbox Rival_Goes_Bridge MSGBOX_DEFAULT
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
	return
	
Route_1_Wait_Bridge_Kate::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
	msgbox Rival_Goes_Bridge, MSGBOX_DEFAULT
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
	return
	
Teala_Into_Grass:
	walk_up
	walk_up
	step_end
	
Teala_Out_Of_Grass:
	walk_down
	walk_down
	step_end

Rival_Looks_Left:
	face_left
	step_end

Teala_To_player:
	walk_down
	step_end

Face_Teala:
	face_up
	step_end
	
Player_Face_Teala:
	face_down
	step_end

Jumping_In_Excitement:
	jump_in_place_up
	face_left
	step_end

Teala_Leaves:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Rival_Runs_Bridge:
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end
	
Route_1_Lesson_Message:
	.string "Today's lesson will be about catching\n"
	.string "your first Pokémon.\p"
	.string "If you don't know how to catch one,\n"
	.string "please pay attention!$"

Route_1_Skip_lesson:
	.string "Skip Teala's lesson?$"

Route_1_Lesson_Start:
	.string "Alright. Observe, students!$"

Route_1_Lesson_Finished:
	.string "And that is how you catch a\n"
	.string "Pokémon.$"

Route_1_Rival_Text:
	.string "I can't wait to catch my first Pokémon.\p"
	.string "{PLAYER} Do you have any idea which kind\n"
	.string "of Pokémon you want to get first?$"

Teala_Present_Task:
	.string "Actually. I wanted to talk to you both.$"

Route_1_Receiving_Pokemon_Derek:
	.string "I talked to my good friend:\n"
	.string "Professor Hazel from Zaurak City.\p"
	.string "She is looking for two trainers to\n"
	.string "help her with one of her studies.\p"
	.string "I was thinking you'd be ideal for\n"
	.string "the study.\p"
	.string "Kate: No way. We are going to be\n"
	.string "helping a professor?.\p"
	.string "Teala: Yes. She even gave me some\n"
	.string "rare Pokémon for you to choose as\p"
	.string "your first partner.$"
	
Route_1_Receiving_Pokemon_Kate:
	.string "I talked to my good friend:\n"
	.string "Professor Hazel from Zaurak City.\p"
	.string "She is looking for two trainers to\n"
	.string "help her with one of her studies.\p"
	.string "I was thinking you'd be ideal for\n"
	.string "the study.\p"
	.string "Derek: No way. We are going to be\n"
	.string "helping a professor?.\p"
	.string "Teala: Yes. She even gave me some\n"
	.string "rare Pokémon for you to choose as\p"
	.string "your first partner.$"	
	
Rival_Gives_First_Choice:
	.string "You should pick a Pokémon first.\n"
	.string "I can wait.$"
	
Rival_Picks_Snivy_Text:
	.string "Then I will take Snivy.$"

Rival_Picks_Torchic_Text:
	.string "Then I will take Torchic.$"

Rival_Picks_Froakie_Text:
	.string "Then I will take Froakie.$"
	
Route_1_Objective_Text:
	.string "Zaurak City is to the north.\p"
	.string "Just across the bridge. Meet with\n"
	.string "Hazel and she'll brief you on the\p"
	.string "details of her study. Good luck\n"
	.string "To you both. I'm rooting for you.$"

Rival_Goes_Bridge:
	.string "I'll be waiting for you at Zaurak Bridge.\n"
	.string "This is so exciting! Our first Pokémon!\p"
	.string "I can't wait to go on this adventure!$"
