ZaurakCity_Hazel_Lab_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, ZaurakCity_Hazel_Lab_OnWarp
	.byte 0

ZaurakCity_Hazel_Lab_OnWarp:
	applymovement 255, Common_Movement_WalkUp4
	waitmovement 0
	goto ZaurakCity_Hazel_Lab_Meeting
	end

ZaurakCity_Hazel_Lab_Meeting::
	msgbox ZaurakCity_Hazel_Lab_Meeting_Text_1, MSGBOX_DEFAULT
	end

ZaurakCity_Hazel_Lab_Meeting_Text_1:
	.string "Test string$"
