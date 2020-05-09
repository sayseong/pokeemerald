Deneb_PlayersHouse_MapScripts::
	.byte 0

Deneb_PlayersHouse_Rest::
	msgbox Deneb_PlayersHouse_FellAsleep, MSGBOX_DEFAULT
	fadescreenswapbuffers FADE_TO_BLACK
	playfanfare MUS_ME_ASA
	special HealPlayerParty
	waitfanfare
	closemessage
	applymovement 255, Common_Movement_FaceDown
	fadescreenswapbuffers FADE_FROM_BLACK
	setvar VAR_TEMP_0, 1
	end
	
Deneb_PlayersHouse_FellAsleep:
	.string "You decided to take a rest.$"
