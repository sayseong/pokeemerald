Route104_MapScripts:: @ 81ECC32
	map_script MAP_SCRIPT_ON_LOAD, Route104_OnTransition
	.byte 0

Route104_OnTransition: @ 81ECC4E
	call_if_set FLAG_HIDE_ROUTE104_POKEMON, Route104_Load_state
	end

Route104_Load_state::
	setvar VAR_ROUTE104_STATE, 1
	return

Route104_Trigger_Left::
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, Route104_Camera_Left
	waitmovement 0
	special RemoveCameraObject
	call Route104_Poochyena_Cornered
	end

Route104_Trigger_Right::
	applymovement 255, Route104_Growlithes_Left
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, Route104_Camera_Right
	waitmovement 0
	special RemoveCameraObject
	call Route104_Poochyena_Cornered
	end

Route104_Poochyena_Cornered::
	applymovement 3, Common_Movement_WalkInPlaceRight
	playmoncry SPECIES_POOCHYENA, 1
	msgbox Route104_Poochyena_Cries_For_Help, MSGBOX_DEFAULT
	closemessage
	applymovement 4, Route104_Growlithes_Left
	waitmovement 0
	msgbox Route104_Poochyena_About_To_Attack, MSGBOX_DEFAULT
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, Route104_Camera
	waitmovement 0
	special RemoveCameraObject
	applymovement 255, Route104_Player_Rescues
	waitmovement 0
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_FaceRight
	waitmovement 0
	msgbox Route104_Poochyena_Attack_Player, MSGBOX_DEFAULT
	applymovement 4, Route104_Growlithe_Attacks
	waitmovement 0
	playmoncry SPECIES_POOCHYENA, 0
	setwildbattle SPECIES_POOCHYENA, 18, ITEM_NONE
	waitse
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	removeobject 4
	special RemoveCameraObject
	applymovement 255, Route104_To_Poochyena
	waitmovement 0
	playmoncry SPECIES_POOCHYENA, 1
	msgbox Route104_Poochyena_Rescue_Msgbox, MSGBOX_DEFAULT
	applymovement 3, Common_Movement_ExclamationMark
	waitmovement 0
	playmoncry SPECIES_POOCHYENA, 0
	givemon SPECIES_POOCHYENA, 15, ITEM_NONE
	compare VAR_RESULT, 0
	goto_if_eq Route104_Poocghena_Receieved_ToParty
	compare VAR_RESULT, 1
	goto_if_eq Route104_Poochyena_Receieved_ToPC
	msgbox Route104_Poochyena_Saved, MSGBOX_DEFAULT
	end

Route104_Poocghena_Receieved_ToParty:: @ 8211A6E
	call Route104_Poochyena_Recieved_Fanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route104_Finish_Recieving_Poochyena
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	closemessage
	goto Route104_Finish_Recieving_Poochyena
	end

Route104_Poochyena_Receieved_ToPC:: @ 8211A96
	call Route104_Poochyena_Recieved_Fanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route104_TransferPoochyenaToPC
	call Common_EventScript_NameReceivedBoxMon
	goto Route104_TransferPoochyenaToPC
	end

Route104_TransferPoochyenaToPC:: @ 8211AB9
	call Common_EventScript_TransferredToPC
	goto Route104_Finish_Recieving_Poochyena
	closemessage
	end

Route104_Poochyena_Recieved_Fanfare:: @ 8211AC4
	bufferspeciesname 1, SPECIES_POOCHYENA
	removeobject 3
	playfanfare MUS_FANFA4
	message Route104_Poochyena_Joins
	waitmessage
	waitfanfare
	bufferspeciesname 0, SPECIES_POOCHYENA
	return
	
Route104_Finish_Recieving_Poochyena::	
	closemessage
	setflag FLAG_HIDE_ROUTE104_POKEMON
	setvar VAR_ROUTE104_STATE, 1
	end
	

Route104_Growlithes_Left:
	walk_left
	step_end

Route104_Camera_Left:
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	step_end

Route104_Camera:
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	step_end
	
Route104_Camera_Right:
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route104_Player_Rescues:
	walk_up
	walk_up
	walk_up
	step_end

Route104_Growlithe_Attacks:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	face_down
	step_end
	
Route104_Growlithes_Leave1:
	walK_fast_right	
	walK_fast_right
	walK_fast_down
	walK_fast_down
	walK_fast_right
	walK_fast_down
	walK_fast_down
	walK_fast_down
	step_end
	
Route104_Growlithes_Leave2:
	walK_fast_right
	walK_fast_right
	walK_fast_right
	walK_fast_down
	walK_fast_down
	walK_fast_right
	walK_fast_down
	walK_fast_down
	walK_fast_down
	step_end
	
Route104_To_Poochyena:
	walk_up
	walk_left
	walk_left
	walk_left
	step_end

Route104_Poochyena_Cries_For_Help:
	.string "Poochyena: Ro-woowooh!\p"
	.string "Poochyena looks injured and scared.$"

Route104_Poochyena_About_To_Attack:
	.string "The other Poochyena is about to attack!$"

Route104_Poochyena_Attack_Player:
	.string "Poochyena spotted you!\n"
	.string "The wild Poochyena attacked!$"

Route104_Poochyena_Rescue_Msgbox:
	.string "Poochyena looks weak and injured.\n"
	.string "You applied the H. Medicine!$"

Route104_Poochyena_Saved:
	.string "Poochyena seems healthy again.\n"
	.string "It appears to be grateful.$"
	
Route104_Poochyena_Joins:	
	.string "Poochyena joined your team!$"
