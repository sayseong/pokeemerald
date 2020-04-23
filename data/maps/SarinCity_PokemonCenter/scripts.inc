SarinCity_PokemonCenter_MapScripts::
	.byte 0

SarinCity_PokemonCente_EventScript_Nurse:: @ 8214D76
	setvar VAR_0x800B, 1
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end
