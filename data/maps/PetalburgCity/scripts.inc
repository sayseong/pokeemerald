PetalburgCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PetalburgCity_OnTransition
	.byte 0

	@VAR_PETALBURG_CITY_STATE 
	@0: Right before recevieng first Pokemon	
	@1: Post-lesson and first Pokemon
	@2: Late game? Senior students return?

PetalburgCity_OnTransition::
	setflag FLAG_VISITED_PETALBURG_CITY
	call_if_set FLAG_HIDE_DENEB_NICK_AND_CARLY, Deneb_Academy_SetState
	call Common_EventScript_SetupRivalGfxId
	compare VAR_PETALBURG_CITY_STATE, 1
	call_if_eq EventScript_First_Lesson
	end

Deneb_Academy_SetState::
	setvar VAR_PETALBURG_CITY_STATE, 1
	return
	
EventScript_First_Lesson::
	setobjectxyperm 5, 29, 30
	setobjectxyperm 2, 30, 30
	setobjectmovementtype 2, MOVEMENT_TYPE_FACE_DOWN
	setobjectxyperm 1, 15, 24
	setobjectmovementtype 1, MOVEMENT_TYPE_FACE_DOWN
	return

Deneb_Academy_Zigzagoon:: @ 82161A3
	lock
	waitse
	playmoncry SPECIES_ZIGZAGOON, 0
	msgbox Deneb_Academy_Zigzagoon_Cry, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Deneb_Academy_Poochyena:: @ 82161A3
	lock
	waitse
	playmoncry SPECIES_POOCHYENA, 0
	msgbox Deneb_Academy_Poochyena_Cry, MSGBOX_DEFAULT
	waitmoncry
	release
	end
	
Mart_Clerk_EventScript::
    setflag FLAG_BADGE01_GET
    setflag FLAG_BADGE02_GET
    setflag FLAG_BADGE03_GET
    setflag FLAG_BADGE04_GET
    setflag FLAG_BADGE05_GET
    setflag FLAG_BADGE06_GET
    setflag FLAG_BADGE07_GET
    setflag FLAG_BADGE08_GET
    setflag FLAG_BADGE09_GET
    setflag FLAG_BADGE10_GET
	msgbox Deneb_MartClerk_message, MSGBOX_NPC
	closemessage
	end

EventScript_Lesson_Trigger1::
	msgbox DenebAcademy_FirstLesson_Hey
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, student_walk_down
	setobjectxyperm 3, 27, 31
	setobjectxyperm 4, 27, 32
	setobjectxyperm 5, 28, 32
	setobjectxyperm 6, 27, 32
	goto EventScript_GoToRoute1
	end

EventScript_Lesson_Trigger2::
	msgbox DenebAcademy_FirstLesson_Hey
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, student_walk_up
	setobjectxyperm 3, 27, 31
	setobjectxyperm 4, 27, 32
	setobjectxyperm 5, 28, 31
	setobjectxyperm 6, 27, 31
	goto EventScript_GoToRoute1
	end

EventScript_Lesson_Trigger3::
	msgbox DenebAcademy_FirstLesson_Hey
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, student_walk_up_up
	setobjectxyperm 3, 27, 31
	setobjectxyperm 4, 27, 32
	setobjectxyperm 5, 28, 31
	setobjectxyperm 6, 27, 31
	goto EventScript_GoToRoute1
	end

EventScript_GoToRoute1::
	applymovement OBJ_EVENT_ID_PLAYER, student_turns_left
	waitmovement 0
	applymovement 3, students_walk_right
	applymovement 4, students_walk_right
	applymovement 5, students_walk_right
	applymovement 6, rival_walk_right
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Teala_Lesson_Derek
	compare VAR_RESULT, FEMALE
	call_if_eq Teala_Lesson_Kate
	setvar VAR_ROUTE101_STATE, 0
	closemessage
	warp MAP_ROUTE101, 255, 10, 30
	waitstate
	releaseall
	end

Teala_EventScript::
	goto_if_set FLAG_HIDE_DENEB_NICK_AND_CARLY, Teala_EventScript_After_Tutorial
	goto Teala_EventScript_GoToRoute01
	end
	
	
Teala_EventScript_GoToRoute01::
	faceplayer	
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq PetalburgCity_Conversation_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq PetalburgCity_Conversation_Kate
	end

Teala_EventScript_After_Tutorial::
	faceplayer	
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq PetalburgCity_Conversation_Teala_Derek
	compare VAR_RESULT, FEMALE
	goto_if_eq PetalburgCity_Conversation_Teala_Kate
	end

PetalburgCity_Conversation_Derek::
	load_field_pic 5, 200, 83, VAR_0x8009
	load_field_pic 1, 40, 83, VAR_0x8008
	msgbox Beta_Teala_Text MSGBOX_NPC	
	destroy_field_pic 5, VAR_0x8009
	destroy_field_pic 1, VAR_0x8008
	return

PetalburgCity_Conversation_Kate::
	load_field_pic 5, 200, 83, VAR_0x8009
	load_field_pic 3, 40, 83, VAR_0x8008
	msgbox Beta_Teala_Text MSGBOX_NPC	
	destroy_field_pic 5, VAR_0x8009
	destroy_field_pic 3, VAR_0x8008
	return

PetalburgCity_Conversation_Teala_Derek::
	load_field_pic 5, 200, 83, VAR_0x8009
	load_field_pic 1, 40, 83, VAR_0x8008
	msgbox Beta_Teala_After_Tutorial MSGBOX_NPC	
	destroy_field_pic 5, VAR_0x8009
	destroy_field_pic 1, VAR_0x8008
	return

PetalburgCity_Conversation_Teala_Kate::
	load_field_pic 5, 200, 83, VAR_0x8009
	load_field_pic 3, 40, 83, VAR_0x8008
	msgbox Beta_Teala_After_Tutorial MSGBOX_NPC	
	destroy_field_pic 5, VAR_0x8009
	destroy_field_pic 3, VAR_0x8008
	return
		
Teala_Lesson_Derek::
	load_field_pic 5, 200, 83, VAR_0x8009
	load_field_pic 1, 40, 83, VAR_0x8008
	msgbox DenebAcademy_Teala_Lesson_Intro	
	destroy_field_pic 5, VAR_0x8009
	destroy_field_pic 1, VAR_0x8008
	return

Teala_Lesson_Kate::
	load_field_pic 5, 200, 83, VAR_0x8009
	load_field_pic 3, 40, 83, VAR_0x8008
	msgbox DenebAcademy_Teala_Lesson_Intro	
	destroy_field_pic 5, VAR_0x8009
	destroy_field_pic 3, VAR_0x8008
	return	
	
Deneb_Academy_Lukas:: @ 81DC3FD
	faceplayer
	msgbox Beta_Lukas_Message, MSGBOX_NPC
	applymovement 4, student_turns_left
	closemessage
	end

Deneb_Academy_Carly:: @ 81DC3FD
	faceplayer	
	msgbox Beta_Carly_Message, MSGBOX_NPC
	applymovement 3, turn_right
	closemessage
	end	

MovementScript_Left_2:
	walk_left
	walk_left
	step_end

student_turns_left: 
	face_left
	step_end

student_walk_down:
	walk_down
	face_left
	step_end	
		
student_walk_up:
	walk_up
	face_left
	step_end	
	
student_walk_up_up:
	walk_up
	walk_up
	face_left
	step_end

students_walk_right:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end
	
rival_walk_right:
	delay_16
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end
	
turn_right:
	face_right
	step_end
	
Beta_Starter_kit_message:
	.string "Take a starting kit for the beta.\p"

Deneb_MartClerk_message:
	.string "The Cafe is closed.\p"
	.string "Don't you have a class to go to as\n"
	.string "well?$"

Beta_Lukas_Message:
	.string "Can't you see I'm busy?$"
	
Beta_Carly_Message:
	.string "I'm battling right now.\n"
	.string "We can talk afterwards.$"
	
Beta_Teala_Text:
	.string "Please head towards Route 1.\n"
	.string "Next lesson will take place there.$"

Beta_Teala_After_Tutorial:
	.string "Please take care {PLAYER}\n"
	.string "I am rooting for you.$"

DenebAcademy_FirstLesson_Hey:
	.string "{PLAYER}!$"
	
DenebAcademy_Teala_Lesson_Intro:
	.string "Seems like we are all ready to go to\n"
	.string "Route 1.\p"
	.string "Please follow me, and don't run off$"

Deneb_Academy_Zigzagoon_Cry:
	.string "Zigzagoon: Bo-Wooh!$"

Deneb_Academy_Poochyena_Cry:
	.string "Poochyena: Ro-woowooh!$"
