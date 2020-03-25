MauvilleCity_MapScripts:: @ 81DF385
	map_script MAP_SCRIPT_ON_TRANSITION, MauvilleCity_OnTransition
	.byte 0

MauvilleCity_OnTransition: @ 81DF38B
	setflag FLAG_VISITED_MAUVILLE_CITY
	goto_if_set FLAG_ENIF_CITY_PIDGEY_FED, Enif_City_Show_Pidgey
	end

Enif_City_Show_Pidgey::
	clearflag FLAG_HIDE_ENIF_CITY_PIDGEY
	return

Mauville_Rest_Point::
	special HealPlayerParty
	msgbox ResPoint_Talk, MSGBOX_NPC
	closemessage
	end

Enif_City_Pidgey_Eventscript:: 
	goto_if_set FLAG_ENIF_CITY_PIDGEY_FED, Enif_City_Pidgey_Fed
	goto Enif_City_Pidgey_Feed
	end

Enif_City_Pidgey_Feed::
	lockall
	faceplayer
	playmoncry SPECIES_PIDGEY, 0
	msgbox Enif_City_Will_Player_Feed_Pidgey, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Enif_City_Did_Not_Feed
	call Enif_City_Pidgey_Has_Berries
	compare VAR_TEMP_1, 0
	goto_if_eq Enif_City_Player_Has_No_Oran_Berries
	goto Enif_City_Player_Has_Oran_Berries
	end

Enif_City_Player_Has_Oran_Berries::
	bufferitemname 0, ITEM_ORAN_BERRY
	msgbox Enif_City_You_Fed_Pidgey, MSGBOX_DEFAULT
	removeitem ITEM_ORAN_BERRY
	closemessage
	releaseall
	fadescreen FADE_TO_BLACK
	removeobject 2
	setflag FLAG_ENIF_CITY_PIDGEY_FED
	fadescreen FADE_FROM_BLACK
	end
	

Enif_City_Did_Not_Feed::
	msgbox Enif_City_Pidgey_Looks_sad, MSGBOX_NPC
	closemessage
	releaseall
	end

Enif_City_Player_Has_No_Oran_Berries::
	msgbox Enif_City_You_Dont_Have_Berries_Text, MSGBOX_NPC
	closemessage
	releaseall
	end

Enif_City_Pidgey_Has_Berries:: @ 8270A72
	setvar VAR_TEMP_1, 0
	checkitem ITEM_ORAN_BERRY, 1
	compare VAR_RESULT, TRUE
	call_if_eq Enif_City_Player_Has_Oran_Berry
	return

Enif_City_Player_Has_Oran_Berry:: @ 8270AB8
	addvar VAR_TEMP_1, 1
	return


Enif_City_Pidgey_Fed::
	lock
	waitse
	playmoncry SPECIES_PIDGEY, 0
	msgbox Enif_City_Pidgey_Fed_Text, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Enif_City_Recieve_Pidgey
	compare VAR_RESULT, NO
	goto_if_eq Enif_City_Wait_For_Pidgey
	waitmoncry
	release
	end

Enif_City_Wait_For_Pidgey::
	msgbox Enif_City_Pidgey_Will_Wait_Text, MSGBOX_DEFAULT
	releaseall
	end

Enif_City_Recieve_Pidgey::
	setvar VAR_TEMP_1, SPECIES_PIDGEY
	givemon SPECIES_PIDGEY, 14, ITEM_ORAN_BERRY
	compare VAR_RESULT, 0
	goto_if_eq Enif_City_Pidgey_Receieved_ToParty
	compare VAR_RESULT, 1
	goto_if_eq Enif_City_Pidgey_Receieved_ToPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

Enif_City_Pidgey_Receieved_ToParty:: @ 8211A6E
	call Enif_city_Pidgey_Recieved_Fanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Enif_City_Finish_Recieving_Pidgey
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	closemessage
	goto Enif_City_Finish_Recieving_Pidgey
	end

Enif_City_Pidgey_Receieved_ToPC:: @ 8211A96
	call Enif_city_Pidgey_Recieved_Fanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Enif_city_TransferPidgeyToPC
	call Common_EventScript_NameReceivedBoxMon
	goto Enif_city_TransferPidgeyToPC
	end

Enif_city_TransferPidgeyToPC:: @ 8211AB9
	call Common_EventScript_TransferredToPC
	goto Enif_City_Finish_Recieving_Pidgey
	closemessage
	end

Enif_city_Pidgey_Recieved_Fanfare:: @ 8211AC4
	bufferspeciesname 1, SPECIES_PIDGEY
	removeobject 2
	playfanfare MUS_FANFA4
	message EnifCity_Pidgey_Joins
	waitmessage
	waitfanfare
	bufferspeciesname 0, SPECIES_PIDGEY
	return
	
Enif_City_Finish_Recieving_Pidgey::	
	setflag FLAG_HIDE_ENIF_CITY_PIDGEY
	end
	
ResPoint_Talk:
	.string "Wimp$"

Enif_City_Pidgey_Looks_sad:
	.string "Pidgey looks disappointed$"

Enif_City_Pidgey_Fed_Text:
	.string "Pidgey: Chirp!\p"
	.string "Pidgey appears to be carrying something.\p"
	.string "It's a Poké Ball!\n"
	.string "Pidgey seems like it wants to follow you\p"
	.string "Will you take Pidgey with you?$"

EnifCity_Pidgey_Joins:
	.string "Pidgey joined your Team!$"

Enif_City_Will_Player_Feed_Pidgey:
	.string "Pidgey: Chirp!\p"
	.string "Pidgey looks hungry.\n"
	.string "Will you feed it an Oran Berry?$"

Enif_City_You_Dont_Have_Berries_Text:
	.string "You do not have any Oran Berries$"

Enif_City_You_Fed_Pidgey:
	.string "You fed the Pidgey an Oran Berry\p"
	.string "The Pidgey Flew off.$"

Enif_City_Pidgey_Will_Wait_Text:
	.string "Pidgey stares at you.$"
