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
	applymovement OBJ_EVENT_ID_PLAYER, Face_Teala
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
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	call Common_EventScript_SetupRival_MugShot
	msgbox Route_1_Rival_Text, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_MugShot
	applymovement 1, Teala_To_player
	applymovement 3, Teala_Leaves
	applymovement 4, Teala_Leaves
	msgbox Teala_Present_Task, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	applymovement 2, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	applymovement 2, Common_Movement_FaceUp
	waitmovement 0
	call Common_EventScript_SetupRival_Duo_Mugshot
	load_field_pic 5, 200, 83, VAR_0x8009
	msgbox Route_1_Receiving_Pokemon_Derek, MSGBOX_DEFAULT
	call Common_EventScript_RemoveDerek_Duo_Mugshot
	destroy_field_pic 5, VAR_0x8009
	applymovement 2, Jumping_In_Excitement
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	call Common_EventScript_SetupRival_MugShot
	msgbox Rival_Gives_First_Choice, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_MugShot
	removeobject 3
	removeobject 4	
	setflag FLAG_HIDE_DENEB_NICK_AND_CARLY
	special ChooseStarter
	waitstate
	setflag FLAG_SYS_POKEMON_GET
	switch VAR_STARTER_MON
	case 0, Route_1_Rival_Gets_Torchic
	case 1, Route_1_Rival_Gets_Froakie
	case 2, Route_1_Rival_Gets_Snivy
	end
	
Route_1_Obtained_Starter::	
	call Common_EventScript_SetupRival_Duo_Mugshot
	load_field_pic 5, 200, 83, VAR_0x8009
	switch VAR_STARTER_MON
	case 0, Route_1_Rival_Picks_Torchic
	case 1, Route_1_Rival_Picks_Froakie
	case 2, Route_1_Rival_Picks_Snivy
	end
	
Route_1_Received_Starter::	
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	applymovement 2, Common_Movement_FaceUp
	waitmovement 0
	msgbox Route_1_Objective_Text, MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_Duo_Mugshot
	destroy_field_pic 5, VAR_0x8009
	closemessage
	applymovement 1, Teala_Leaves
	waitmovement 0
	removeobject 1
	applymovement 255, Common_Movement_FaceRight
	applymovement 2, Common_Movement_FaceLeft
	call Common_EventScript_SetupRival_MugShot
	msgbox Rival_Goes_Bridge MSGBOX_DEFAULT
	call Common_EventScript_RemoveRival_MugShot
	closemessage
	applymovement 2, Rival_Runs_Bridge
	waitmovement 0
	removeobject 2
	setvar VAR_ROUTE101_STATE, 2
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	releaseall
	end

Route_1_Rival_Picks_Snivy::
	msgbox Rival_Picks_Snivy_Text MSGBOX_DEFAULT
	goto Route_1_Received_Starter
	return
	
Route_1_Rival_Picks_Torchic::	
	msgbox Rival_Picks_Torchic_Text MSGBOX_DEFAULT
	goto Route_1_Received_Starter
	return
	
Route_1_Rival_Picks_Froakie::	
	msgbox Rival_Picks_Froakie_Text MSGBOX_DEFAULT
	goto Route_1_Received_Starter
	return

Route_1_Rival_Gets_Snivy::
	playfanfare MUS_FANFA4
	msgbox You_Get_Froakie, MSGBOX_DEFAULT
	fadeinbgm 1
	waitfanfare	
	msgbox Route_1_Nickname_Starter_Froakie, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route_1_Give_Nickname_To_Starter
	compare VAR_RESULT, NO
	goto_if_eq Route_1_Obtained_Starter
	end
	
Route_1_Rival_Gets_Torchic::
	playfanfare MUS_FANFA4
	msgbox You_Get_Snivy, MSGBOX_DEFAULT
	fadeinbgm 1
	waitfanfare	
	msgbox Route_1_Nickname_Starter_Snivy, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route_1_Give_Nickname_To_Starter
	compare VAR_RESULT, NO
	goto_if_eq Route_1_Obtained_Starter
	end

Route_1_Rival_Gets_Froakie::
	playfanfare MUS_FANFA4
	msgbox You_Get_Torchic, MSGBOX_DEFAULT
	fadeinbgm 1
	waitfanfare	
	msgbox Route_1_Nickname_Starter_Torchic, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route_1_Give_Nickname_To_Starter
	compare VAR_RESULT, NO
	goto_if_eq Route_1_Obtained_Starter
	end

Route_1_Give_Nickname_To_Starter:: @ 81F9E07
	setvar VAR_0x8004, 0
	call Common_EventScript_NameReceivedPartyMon
	goto Route_1_Obtained_Starter
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
	.string "{PLAYER}, do you have any idea which kind\n"
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

You_Get_Snivy:
	.string "You received Snivy!$"

You_Get_Torchic:
	.string "You received Torchic!$"

You_Get_Froakie:
	.string "You received Froakie!$"

Route_1_Nickname_Starter_Snivy:
	.string "Give Snivy a nickname?$"

Route_1_Nickname_Starter_Torchic:
	.string "Give Torchic a nickname?$"
	
Route_1_Nickname_Starter_Froakie:
	.string "Give Froakie a nickname?$"	
	
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
