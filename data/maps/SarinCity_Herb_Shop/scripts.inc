SarinCity_Herb_Shop_MapScripts::
	.byte 0

SarinCity_HerbShop_EventScript_Clerk:: @ 81FE4D7
	lock
	faceplayer
	message SarinCity_HerbShop_Text_WelcomeToHerbShop
	waitmessage
	pokemart LavaridgeTown_HerbShop_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SarinCity_HerbShop_Pokemart: @ 81FE4F0
	.2byte ITEM_ENERGY_POWDER
	.2byte ITEM_ENERGY_ROOT
	.2byte ITEM_HEAL_POWDER
	.2byte ITEM_REVIVAL_HERB
	.2byte ITEM_NONE
	release
	end

SarinCity_BerryShop_EventScript_Clerk:: @ 81FE4D7
	lock
	faceplayer
	message SarinCity_BerryShop_Text_WelcomeToHerbShop
	waitmessage
	pokemart SarinCity_BerryShop_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SarinCity_BerryShop_Pokemart: @ 81FE4F0
	.2byte ITEM_ORAN_BERRY
	.2byte ITEM_LUM_BERRY
	.2byte ITEM_SITRUS_BERRY
	.2byte ITEM_PERSIM_BERRY
	.2byte ITEM_NONE
	release
	end
	
SarinCity_HerbShop_Text_WelcomeToHerbShop: @ 81FE53E
	.string "Welcome to the Herb Shop, home of\n"
	.string "effective and inexpensive medicine!$"

SarinCity_BerryShop_Text_WelcomeToHerbShop: @ 81FE53E
	.string "Welcome to the Berry Shop, home of\n"
	.string "a wide selection of tasty berries!$"
