Deneb_Mart_MapScripts:: @ 8207D68
	.byte 0

Deneb_Mart_EventScript_Clerk:: @ 8207D69
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	goto_if_set FLAG_PETALBURG_MART_EXPANDED_ITEMS, Deneb_Mart_EventScript_ExpandedItems
	pokemart Deneb_Mart_Pokemart_Basic
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
Deneb_Mart_Pokemart_Basic: @ 8207D8C
	.2byte ITEM_POKE_BALL
	.2byte ITEM_ORAN_BERRY
	.2byte ITEM_PECHA_BERRY		
	.2byte ITEM_CHERI_BERRY		
	.2byte ITEM_STEEL_STONE		
	.2byte ITEM_BONDED_STONE		
	.2byte ITEM_TM35		
	.2byte ITEM_RARE_CANDY		
	.2byte ITEM_NONE
	release
	end

Deneb_Mart_EventScript_ExpandedItems:: @ 8207DA6
	pokemart Deneb_Mart_Pokemart_Expanded
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
Deneb_Mart_Pokemart_Expanded: @ 8207DB8
	.2byte ITEM_POKE_BALL
	.2byte ITEM_ORAN_BERRY
	.2byte ITEM_PECHA_BERRY		
	.2byte ITEM_CHERI_BERRY	
	.2byte ITEM_NONE
	release
	end

Deneb_Mart_Lukas::
	faceplayer
	msgbox Deneb_Mart_Lukas_Text, MSGBOX_NPC
	closemessage
	end
	
Deneb_Mart_Carly::
	faceplayer
	msgbox Deneb_Mart_Carly_Text, MSGBOX_NPC
	closemessage
	end
	
Deneb_Mart_Lukas_Text:
	.string "Don't you have something better to do\n"
	.string "than to bother me?$"

Deneb_Mart_Carly_Text:
	.string "Sorry about Nick. I think he is mad\n"
	.string "because I won our Pokémon Battle earlier.$"
