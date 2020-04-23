MeteorFalls_B1F_1R_MapScripts:: @ 822CE25
	map_script MAP_SCRIPT_ON_LOAD, AlcorTunnel_OnTransition
	.byte 0

AlcorTunnel_OnTransition::
	call_if_set FLAG_HIDE_ALCOR_TUNNEL_GRUNTS, AlcorTunnel_SetState
	end

AlcorTunnel_SetState::
	setvar VAR_METEOR_FALLS_STATE, 1
	call AlcorTunnel_ExitOpened
	return
	
AlcorTunnel_ExitOpened::
	setmetatile 8, 12, METATILE_Cave_FlatFloor, 0
	setmetatile 9, 12, METATILE_Cave_FlatFloor, 0
	setmetatile 8, 13, METATILE_Cave_FlatFloor, 0
	setmetatile 9, 13, METATILE_Cave_FlatFloor, 0
	return
	
AlcorTunnel_Hiker_Trainer1:: @ 81F2B3E
	trainerbattle_single TRAINER_MARCEL, AlcorTunnel_Text_MarcelIntro, AlcorTunnel_Text_MarcelDefeat	
	msgbox AlcorTunnel_Text_MarcelPostBattle, MSGBOX_AUTOCLOSE
	end
	
AlcorTunnel_Hiker_Trainer2:: @ 81F2B3E
	trainerbattle_single TRAINER_ALBERTO, AlcorTunnel_Text_AlbertoIntro, AlcorTunnel_Text_AlbertoDefeat	
	msgbox AlcorTunnel_Text_AlbertoPostBattle, MSGBOX_AUTOCLOSE
	end

AlcorTunnel_Hiker_Trainer3:: @ 81F2B3E
	trainerbattle_single TRAINER_ED, AlcorTunnel_Text_EdIntro, AlcorTunnel_Text_EdDefeat	
	msgbox AlcorTunnel_Text_EdPostBattle, MSGBOX_AUTOCLOSE
	end

AlcorTunnel_Trigger_Top::
	lockall
	applymovement 255, AlcorTunnel_Player_Walk_Into_Position1
	waitmovement 0
	goto AlcorTunnel_First_Seraph_Encounter_EventScript
	end

AlcorTunnel_Trigger_Bottom::
	lockall
	applymovement 255, AlcorTunnel_Player_Walk_Into_Position2
	waitmovement 0
	goto AlcorTunnel_First_Seraph_Encounter_EventScript
	end

AlcorTunnel_First_Seraph_Encounter_EventScript::
	playbgm MUS_AQA_0, 0
	special SpawnCameraObject
	applymovement EVENT_OBJ_ID_CAMERA, AlcorTunnel_Camera_PanUp
	waitmovement 0
	special RemoveCameraObject
	waitmovement 0
	load_field_pic 10, 160, 83, VAR_0x8008
	load_field_pic 9, 200, 83, VAR_0x8009
	msgbox Alcor_Tunnel_Seraph_Grunts, MSGBOX_DEFAULT
	destroy_field_pic 10, VAR_0x8008
	destroy_field_pic 9, VAR_0x8009
	applymovement 7, Common_Movement_WalkInPlaceDown
	waitmovement 0
	msgbox AlcorTunnel_Lone_Boy_Text, MSGBOX_DEFAULT
	closemessage
	applymovement 255, AlcorTunnel_Player_WalksUp
	waitmovement 0
	applymovement 8, Common_Movement_ExclamationMark
	applymovement 9,Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 8, Common_Movement_FaceDown
	applymovement 9,Common_Movement_FaceDown
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player
	load_field_pic 10, 160, 83, VAR_0x8008
	load_field_pic 9, 200, 83, VAR_0x8009
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	compare VAR_RESULT, 2
	goto_if_lt AlcorTunnel_Grunts_Single_Battle
	goto AlcorTunnel_Grunts_Double_Battle
	end

AlcorTunnel_Grunts_Double_Battle::
	msgbox Alcor_Tunnel_Seraph_Grunts_Startled, MSGBOX_DEFAULT
	msgbox AlcorTunnel_BlackSeraphs_Intro, MSGBOX_DEFAULT
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player_Delete
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player_Delete
	destroy_field_pic 10, VAR_0x8008
	destroy_field_pic 9, VAR_0x8009
	special SpawnCameraObject
	applymovement EVENT_OBJ_ID_CAMERA, AlcorTunnel_Camera_WalkDown
	waitmovement 0
	special RemoveCameraObject
	trainerbattle_no_intro TRAINER_GRUNT_BLACK_SERAPH_ALCOR_TUNNEL_DOUBLE, AlcorTunnel_BlackSeraphs_Defeated
	call AlcorTunnel_BlackSeraphs_PostBattle_EventScript
	end
	
AlcorTunnel_Grunts_Single_Battle::
	msgbox Alcor_Tunnel_Seraph_Grunts_Startled, MSGBOX_DEFAULT
	applymovement 9, AlcorTunnel_Male_Grunt_To_Singles
	special SpawnCameraObject
	applymovement EVENT_OBJ_ID_CAMERA, AlcorTunnel_Camera_PanDown
	waitmovement 0
	special RemoveCameraObject
	applymovement 255, Common_Movement_FaceRight
	waitmovement 0
	msgbox AlcorTunnel_BlackSeraphs_Intro, MSGBOX_DEFAULT
	closemessage
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player_Delete
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player_Delete
	destroy_field_pic 10, VAR_0x8008
	destroy_field_pic 9, VAR_0x8009
	trainerbattle_no_intro TRAINER_GRUNT_BLACK_SERAPH_ALCOR_TUNNEL_SINGLE1, AlcorTunnel_BlackSeraph1_Defeated
	call AlcorTunnel_Grunt_Single_Battle_2
	end	

AlcorTunnel_Grunt_Single_Battle_2::
	playbgm MUS_AQA_0, 0
	applymovement 9, Common_Movement_WalkInPlaceLeft
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player
	load_field_pic 10, 160, 83, VAR_0x8008
	load_field_pic 9, 200, 83, VAR_0x8009
	msgbox AlcorTunnel_BlackSeraph_Defeated2, MSGBOX_DEFAULT
	applymovement 8, Common_Movement_WalkInPlaceDown
	waitmovement 0
	applymovement 255, Common_Movement_FaceUp
	msgbox AlcorTunnel_BlackSeraphs_Intro2, MSGBOX_DEFAULT
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player_Delete
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player_Delete
	destroy_field_pic 10, VAR_0x8008
	destroy_field_pic 9, VAR_0x8009
	trainerbattle_no_intro TRAINER_GRUNT_BLACK_SERAPH_ALCOR_TUNNEL_SINGLE2, AlcorTunnel_BlackSeraph2_Defeated
	call AlcorTunnel_BlackSeraphs_PostBattle_EventScript1
	end

AlcorTunnel_BlackSeraphs_PostBattle_EventScript1::
	applymovement 9, AlcorTunnel_SingleGrunt_ToEscapePosition
	waitmovement 0
	goto AlcorTunnel_BlackSeraphs_PostBattle_EventScript
	end

AlcorTunnel_BlackSeraphs_PostBattle_EventScript::
	playbgm MUS_AQA_0, 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player
	load_field_pic 10, 160, 83, VAR_0x8008
	load_field_pic 9, 200, 83, VAR_0x8009
	applymovement 8, Common_Movement_WalkInPlaceDown
	waitmovement 0
	msgbox AlcorTunnel_BlackSeraphs_Text_Defeated, MSGBOX_DEFAULT
	applymovement 9, Common_Movement_WalkInPlaceDown
	waitmovement 0
	msgbox AlcorTunnel_BlackSeraphs_Text_Defeated2, MSGBOX_DEFAULT	
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq AlcorTunnel_Male_Player_Delete
	compare VAR_RESULT, FEMALE
	call_if_eq AlcorTunnel_Female_Player_Delete
	destroy_field_pic 10, VAR_0x8008
	destroy_field_pic 9, VAR_0x8009
	closemessage
	applymovement 8, AlcorTunnel_Grunt_Runaway1
	applymovement 9, AlcorTunnel_Grunt_Runaway2
	waitmovement 0
	removeobject 8
	removeobject 9
	fadedefaultbgm
	applymovement 7, AlcorTunnel_BoyWalks_To_You
	applymovement 255, AlcorTunnel_Player_Checks_Boy
	waitmovement 0
	msgbox AlcorTunnel_Saved_Boy_And_Growlithe, MSGBOX_DEFAULT
	applymovement 7, Common_Movement_FaceDown
	applymovement 10, Common_Movement_FaceDown
	waitmovement 0
	msgbox AlcorTunnel_Boy_Gives_Charcoal, MSGBOX_DEFAULT
	giveitem ITEM_CHARCOAL
	setflag FLAG_HIDE_ALCOR_TUNNEL_GRUNTS
	setvar VAR_METEOR_FALLS_STATE, 1 
	call AlcorTunnel_ExitOpened	
	releaseall
	end

AlcorTunnel_Player_Checks_Boy:
	walk_up
	walk_right
	face_up
	step_end
	

AlcorTunnel_SingleGrunt_ToEscapePosition:
	face_down
	lock_facing_direction
	walk_up
	unlock_facing_direction
	step_end

AlcorTunnel_Boy_Eventscript::
	faceplayer
	lockall
	msgbox AlcorTunnel_Boy, MSGBOX_NPC
	releaseall
	end
	
AlcorTunnel_Growlithe_Eventscript::
	faceplayer
	lockall
	playmoncry SPECIES_GROWLITHE, 0
	msgbox AlcorTunnel_Growlithe, MSGBOX_NPC
	releaseall
	end


AlcorTunnel_BoyWalks_To_You:
	walk_down
	walk_right
	walk_down
	face_left
	step_end

AlcorTunnel_Grunt_Runaway1:
	walk_fast_left
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end
	
AlcorTunnel_Grunt_Runaway2:
	walk_fast_left
	walk_fast_left
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

AlcorTunnel_Male_Player::
	load_field_pic 1, 40, 83, VAR_0x800A
	return

AlcorTunnel_Female_Player::
	load_field_pic 3, 40, 83, VAR_0x800A
	return

AlcorTunnel_Male_Player_Delete::
	destroy_field_pic 1, VAR_0x800A
	return

AlcorTunnel_Female_Player_Delete::
	destroy_field_pic 3, VAR_0x800A
	return

AlcorTunnel_Camera_PanUp: @ 8239858
	walk_slow_up
	walk_slow_up
	step_end

AlcorTunnel_Camera_PanDown:
	walk_down
	walk_down
	step_end

AlcorTunnel_Camera_WalkDown:
	walk_down
	step_end

AlcorTunnel_Male_Grunt_To_Singles:
	walk_down
	face_left
	step_end

AlcorTunnel_Player_WalksUp:
	walk_up
	step_end

AlcorTunnel_Player_Walk_Into_Position1:
	walk_down
	walk_right
	walk_right
	face_up
	step_end
	
AlcorTunnel_Player_Walk_Into_Position2:
	walk_right
	walk_right
	face_up
	step_end	

Alcor_Tunnel_Seraph_Grunts:
	.string "???: There's no use in fighting.\p"
	.string "You are too weak a trainer for a\n"
	.string "Pokémon like this.\p"
	.string "???: Don't Worry, the less you fight,\n"
	.string "makes this a whole lot easier.$"

AlcorTunnel_Lone_Boy_Text:
	.string "Boy: No! Growlithe!\n"
	.string "Give him back!$"
	
Alcor_Tunnel_Seraph_Grunts_Ridicule:
	.string "???: Fufufuh\p"
	.string "And just what are you going to\n"
	.string "do to stop us?\p"
	
Alcor_Tunnel_Seraph_Grunts_Startled:
	.string "???: Well isn't that just adorable.\p"
	.string "This little brat is going to try \n"
	.string "and stop us.$"

AlcorTunnel_BlackSeraphs_Defeated:
	.string "???: Tch, you got Lucky!\p"
	.string "???: Damn you, kid!$"

AlcorTunnel_BlackSeraphs_Intro:
	.string "???: You're no match for us kid!$"
	
AlcorTunnel_BlackSeraph_Defeated2:
	.string "Damn it!$"

AlcorTunnel_BlackSeraphs_Intro2:
	.string "???: You won't stop us!$"

AlcorTunnel_BlackSeraph1_Defeated:
	.string "???: Damn you, kid!$"
	
AlcorTunnel_BlackSeraph2_Defeated:
	.string "???: Tch, you got Lucky!$"

AlcorTunnel_BlackSeraphs_Text_Defeated:
	.string "Tch! Screw this!\p"
	.string "This Pokémon isn't even that good\n"
	.string "Why did we waste our time with this?$"
	
AlcorTunnel_BlackSeraphs_Text_Defeated2:
	.string "Whatever!\p"
	.string "You'd have been destroyed by our\n"
	.string "Leader.\p"
	.string "So I wouldn't go messin with us.$"	

AlcorTunnel_Saved_Boy_And_Growlithe:
	.string "Growlithe! You're safe!$"

AlcorTunnel_Boy_Gives_Charcoal:
	.string "Thank you so much Trainer.\p"
	.string "I have to be more careful next time\p"
	.string "I'd like to give you something for \n"
	.string "helping us.\p"
	.string "It's not much but it's the least I can\n"
	.string "do after you saved Growlithe.$"

AlcorTunnel_Growlithe:
	.string "Groroh!$"

AlcorTunnel_Boy:
	.string "I really can't thank you enough\n"
	.string "for your help.$"
