ZaurakCity_Mart_MapScripts::
	.byte 0

ZaurakCity_Mart_EventScript_Clerk:: @ 82110E6
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart ZaurakCity_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
ZaurakCity_Mart_Pokemart: @ 8211100
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_NONE
	release
	end

ZaurakCity_Mart_EventScript_Man:: @ 821111C
	msgbox ZaurakCity_Man_Text, MSGBOX_NPC
	end

ZaurakCity_Man_Text:
	.string "This store just opened recently.\p"
	.string "It's selection is small, but it\n"
	.string "covers the essentials.$"
