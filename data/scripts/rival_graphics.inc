Common_EventScript_SetupRivalGfxId:: @ 8271ED7
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq EventScript_SetupRivalGfxIdFemale
	compare VAR_RESULT, FEMALE
	goto_if_eq EventScript_SetupRivalGfxIdMale
	end

Common_EventScript_SetupPlayer_MugShot::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Common_EventScript_SetupMale_Player_MugShot
	compare VAR_RESULT, FEMALE
	goto_if_eq Common_EventScript_SetupFemale_Player_MugShot
    end

Common_EventScript_SetupMale_Player_MugShot::
	load_field_pic 1, 40, 83, VAR_0x8008
    return

Common_EventScript_SetupFemale_Player_MugShot::
	load_field_pic 3, 40, 83, VAR_0x8008
    return

Common_EventScript_RemovePlayer_MugShot::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Common_EventScript_RemoveMale_Player_MugShot
	compare VAR_RESULT, FEMALE
	goto_if_eq Common_EventScript_RemoveFemale_Player_MugShot
    end

Common_EventScript_RemoveMale_Player_MugShot::
	destroy_field_pic 1, VAR_0x8008
    return

Common_EventScript_RemoveFemale_Player_MugShot::
	destroy_field_pic 3, VAR_0x8008
    return

Common_EventScript_SetupRival_MugShot::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Common_EventScript_SetupDerek_MugShot
	compare VAR_RESULT, FEMALE
	goto_if_eq Common_EventScript_SetupKate_MugShot
    end

Common_EventScript_SetupDerek_MugShot::
	load_field_pic 1, 40, 83, VAR_0x8008
	load_field_pic 4, 200, 83, VAR_0x8009
    return

Common_EventScript_SetupKate_MugShot::
	load_field_pic 3, 40, 83, VAR_0x8008
	load_field_pic 2, 200, 83, VAR_0x8009
    return

Common_EventScript_RemoveRival_MugShot::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Common_EventScript_RemoveDerek_MugShot
	compare VAR_RESULT, FEMALE
	goto_if_eq Common_EventScript_RemoveKate_MugShot
    end

Common_EventScript_RemoveDerek_MugShot::
	destroy_field_pic 1, VAR_0x8008
	destroy_field_pic 4, VAR_0x8009
    return

Common_EventScript_RemoveKate_MugShot::
	destroy_field_pic 3, VAR_0x8008
	destroy_field_pic 2, VAR_0x8009
    return

Common_EventScript_SetupRival_Duo_Mugshot::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Common_EventScript_SetupDerek_Duo_Mugshot
	compare VAR_RESULT, FEMALE
	goto_if_eq Common_EventScript_SetupKate_Duo_Mugshot
    end

Common_EventScript_SetupDerek_Duo_Mugshot::
	load_field_pic 7, 40, 83, VAR_0x8008
    return

Common_EventScript_SetupKate_Duo_Mugshot::
	load_field_pic 6, 40, 83, VAR_0x8008
    return

Common_EventScript_RemoveRival_Duo_Mugshot::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Common_EventScript_RemoveDerek_Duo_Mugshot
	compare VAR_RESULT, FEMALE
	goto_if_eq Common_EventScript_RemoveKate_Duo_Mugshot
    end

Common_EventScript_RemoveDerek_Duo_Mugshot::
	destroy_field_pic 7, VAR_0x8008
    return

Common_EventScript_RemoveKate_Duo_Mugshot::
	destroy_field_pic 6, VAR_0x8008
    return

EventScript_SetupRivalGfxIdFemale:: @ 8271EEF
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return

EventScript_SetupRivalGfxIdMale:: @ 8271EF5
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

Common_EventScript_SetupRivalOnBikeGfxId:: @ 8271EFB
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq EventScript_SetupRivalOnBikeGfxIdFemale
	compare VAR_RESULT, FEMALE
	goto_if_eq EventScript_SetupRivalOnBikeGfxIdMale
	end

EventScript_SetupRivalOnBikeGfxIdFemale:: @ 8271F13
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_MAY_MACH_BIKE
	return

EventScript_SetupRivalOnBikeGfxIdMale:: @ 8271F19
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_BRENDAN_MACH_BIKE
	return

@ Unused
Common_EventScript_SetupRivalGfxIdSameGender:: @ 8271F1F
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq EventScript_SetupRivalGfxIdMale2
	compare VAR_RESULT, FEMALE
	goto_if_eq EventScript_SetupRivalGfxIdFemale2
	end

EventScript_SetupRivalGfxIdMale2:: @ 8271F37
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

EventScript_SetupRivalGfxIdFemale2:: @ 8271F3D
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return
