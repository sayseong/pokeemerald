PetalburgCity_PokemonCenter_1F_MapScripts::
	.byte 0
	
@ VAR_0x800B is the Nurse's object event id
PetalburgCity_PokemonCenter_1F_EventScript_Nurse:: @ 82079FC
	setvar VAR_0x800B, 1
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end	
