import uiScriptLocale
import app

ROOT_PATH = "d:/ymir work/ui/public/"

TEMPORARY_X = +13
BUTTON_TEMPORARY_X = 5
PVP_X = -10

LINE_LABEL_X = 30
LINE_DATA_X = 90
LINE_BEGIN = 40
LINE_STEP = 25
SMALL_BUTTON_WIDTH = 45
MIDDLE_BUTTON_WIDTH = 65

window = {
	"name" : "GameOptionDialog",
	"style" : ["movable", "float",],

	"x" : 0,
	"y" : 0,

	"width" : 300,
	"height" : 25*11+8,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 300,
			"height" : 25*11+8,

			"children" :
			[
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ["attach",],

					"x" : 8,
					"y" : 8,

					"width" : 284,
					"color" : "gray",

					"children" :
					[
						{ "name":"titlename", "type":"text", "x":0, "y":3, 
						"text" : uiScriptLocale.GAMEOPTION_TITLE, 
						"horizontal_align":"center", "text_horizontal_align":"center" },
					],
				},

				{
					"name" : "name_color",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 40+2,

					"text" : uiScriptLocale.OPTION_NAME_COLOR,
				},
				{
					"name" : "name_color_normal",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
					"y" : 40,

					"text" : uiScriptLocale.OPTION_NAME_COLOR_NORMAL,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				{
					"name" : "name_color_empire",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
					"y" : 40,

					"text" : uiScriptLocale.OPTION_NAME_COLOR_EMPIRE,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},

				## Ÿ��â
				{
					"name" : "target_board",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 65+2,

					"text" : uiScriptLocale.OPTION_TARGET_BOARD,
				},
				{
					"name" : "target_board_no_view",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
					"y" : 65,

					"text" : uiScriptLocale.OPTION_TARGET_BOARD_NO_VIEW,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},
				{
					"name" : "target_board_view",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
					"y" : 65,

					"text" : uiScriptLocale.OPTION_TARGET_BOARD_VIEW,

					"default_image" : ROOT_PATH + "Middle_Button_01.sub",
					"over_image" : ROOT_PATH + "Middle_Button_02.sub",
					"down_image" : ROOT_PATH + "Middle_Button_03.sub",
				},

				## PvP Mode
				{
					"name" : "pvp_mode",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 90+2,

					"text" : uiScriptLocale.OPTION_PVPMODE,
				},
				{
					"name" : "pvp_peace",
					"type" : "radio_button",

					"x" : LINE_DATA_X+SMALL_BUTTON_WIDTH*0,
					"y" : 90,

					"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,
					"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_PEACE_TOOLTIP,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "pvp_revenge",
					"type" : "radio_button",

					"x" : LINE_DATA_X+SMALL_BUTTON_WIDTH*1,
					"y" : 90,

					"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,
					"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_REVENGE_TOOLTIP,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "pvp_guild",
					"type" : "radio_button",

					"x" : LINE_DATA_X+SMALL_BUTTON_WIDTH*2,
					"y" : 90,

					"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,
					"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_GUILD_TOOLTIP,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},
				{
					"name" : "pvp_free",
					"type" : "radio_button",

					"x" : LINE_DATA_X+SMALL_BUTTON_WIDTH*3,
					"y" : 90,

					"text" : uiScriptLocale.OPTION_PVPMODE_FREE,
					"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_FREE_TOOLTIP,

					"default_image" : ROOT_PATH + "small_Button_01.sub",
					"over_image" : ROOT_PATH + "small_Button_02.sub",
					"down_image" : ROOT_PATH + "small_Button_03.sub",
				},

				## Block
				{
					"name" : "block",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 115+2,

					"text" : uiScriptLocale.OPTION_BLOCK,
				},
				{
					"name" : "block_exchange_button",
					"type" : "toggle_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
					"y" : 115,

					"text" : uiScriptLocale.OPTION_BLOCK_EXCHANGE,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "block_party_button",
					"type" : "toggle_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
					"y" : 115,

					"text" : uiScriptLocale.OPTION_BLOCK_PARTY,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "block_guild_button",
					"type" : "toggle_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*2,
					"y" : 115,

					"text" : uiScriptLocale.OPTION_BLOCK_GUILD,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "block_whisper_button",
					"type" : "toggle_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
					"y" : 140,

					"text" : uiScriptLocale.OPTION_BLOCK_WHISPER,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "block_friend_button",
					"type" : "toggle_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
					"y" : 140,

					"text" : uiScriptLocale.OPTION_BLOCK_FRIEND,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "block_party_request_button",
					"type" : "toggle_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*2,
					"y" : 140,

					"text" : uiScriptLocale.OPTION_BLOCK_PARTY_REQUEST,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},

				## Chat
				{
					"name" : "chat",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 165+2,

					"text" : uiScriptLocale.OPTION_VIEW_CHAT,
				},
				{
					"name" : "view_chat_on_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X,
					"y" : 165,

					"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "view_chat_off_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH,
					"y" : 165,

					"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},

				## Always Show Name
				{
					"name" : "always_show_name",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 190+2,

					"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME,
				},
				{
					"name" : "always_show_name_on_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X,
					"y" : 190,

					"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_ON,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "always_show_name_off_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH,
					"y" : 190,

					"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_OFF,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},

				## Effect On/Off
				{
					"name" : "effect_on_off",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 215+2,

					"text" : uiScriptLocale.OPTION_EFFECT,
				},
				{
					"name" : "show_damage_on_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X,
					"y" : 215,

					"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "show_damage_off_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH,
					"y" : 215,

					"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},

				## �ǸŹ���
				{
					"name" : "salestext_on_off",
					"type" : "text",

					"x" : LINE_LABEL_X,
					"y" : 240+2,

					"text" : uiScriptLocale.OPTION_SALESTEXT,
				},
				{
					"name" : "salestext_on_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X,
					"y" : 240,

					"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_ON,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
				{
					"name" : "salestext_off_button",
					"type" : "radio_button",

					"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH,
					"y" : 240,

					"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_OFF,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",
				},
			],
		},
	],
}

CUR_LINE_Y = LINE_BEGIN + LINE_STEP * 8

if app.WJ_SHOW_MOB_INFO:
	CUR_LINE_Y += LINE_STEP
	window["height"] = window["height"] + 25
	window["children"][0]["height"] = window["children"][0]["height"] + 25
	window["children"][0]["children"] = window["children"][0]["children"] + [
		## Show Mob Info List
		{
			"name" : "show_mob_info",
			"type" : "text",

			"multi_line" : 1,

			"x" : LINE_LABEL_X,
			"y" : CUR_LINE_Y+2,

			"text" : uiScriptLocale.OPTION_MOB_INFO,
		},
		{
			"name" : "show_mob_level_button",
			"type" : "toggle_button",

			"x" : LINE_DATA_X,
			"y" : CUR_LINE_Y,

			"text" : uiScriptLocale.OPTION_MOB_INFO_LEVEL,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
		{
			"name" : "show_mob_AI_flag_button",
			"type" : "toggle_button",

			"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH,
			"y" : CUR_LINE_Y,

			"text" : uiScriptLocale.OPTION_MOB_INFO_AGGR,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	]

if app.ENABLE_AUTO_PICKUP:
	CUR_LINE_Y += LINE_STEP
	window["height"] = window["height"] + 25
	window["children"][0]["height"] = window["children"][0]["height"] + 25
	window["children"][0]["children"] = window["children"][0]["children"] + [
		{
			"name" : "auto_pick_on_off",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : CUR_LINE_Y + 2,

			"text" : "Pickup",
		},
		{
			"name" : "auto_pick_on",
			"type" : "radio_button",

			"x" : LINE_DATA_X,
			"y" : CUR_LINE_Y,

			"text" : uiScriptLocale.OPTION_FOG_ON,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
		{
			"name" : "auto_pick_off",
			"type" : "radio_button",

			"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH,
			"y" : CUR_LINE_Y,

			"text" : uiScriptLocale.OPTION_FOG_OFF,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	]

if app.ENABLE_KEYCHANGE_SYSTEM:
	CUR_LINE_Y += LINE_STEP
	window["height"] = window["height"] + 25
	window["children"][0]["height"] = window["children"][0]["height"] + 25
	window["children"][0]["children"] = window["children"][0]["children"] + [
		## Ű����
		{
			"name" : "key_setting_show",
			"type" : "text",

			"x" : LINE_LABEL_X,
			"y" : CUR_LINE_Y + 2,

			"text" : uiScriptLocale.OPTION_KEY_SETTING,
		},
		{
			"name" : "key_setting_show_button",
			"type" : "button",

			"x" : LINE_DATA_X,
			"y" : CUR_LINE_Y,

			"text" : uiScriptLocale.OPTION_SETTING,

			"default_image" : ROOT_PATH + "middle_button_01.sub",
			"over_image" : ROOT_PATH + "middle_button_02.sub",
			"down_image" : ROOT_PATH + "middle_button_03.sub",
		},
	]

if app.ENABLE_LEFT_SEAT:
	CUR_LINE_Y += LINE_STEP
	window["height"] = window["height"] + LINE_STEP
	window["children"][0]["height"] = window["children"][0]["height"] + LINE_STEP
	window["children"][0]["children"] = [
		{ "name" : "left_seat_time_bar_text", "type" : "text", "x" : LINE_LABEL_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "text" : uiScriptLocale.LEFT_SEAT,},
		{ "name" : "left_seat_time_list_button", "type" : "button", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "text" : uiScriptLocale.LEFT_SEAT_10_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_one.sub", },
		{
			"name" : "left_seat_time_list_window", "type" : "window", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y+16, "width" : 130, "height" : 80,
			"children":
			(
				{ "name" : "left_seat_time_10_min", "type" : "button", "x" : 0, "y" : 0, "text" : uiScriptLocale.LEFT_SEAT_10_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_top.sub", },
				{ "name" : "left_seat_time_30_min", "type" : "button", "x" : 0, "y" : 16, "text" : uiScriptLocale.LEFT_SEAT_30_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_time_90_min", "type" : "button", "x" : 0, "y" : 32, "text" : uiScriptLocale.LEFT_SEAT_90_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", },
			),
		},
		{ "name" : "left_seat_list_time_arrow_button", "type" : "button", "x" : LINE_DATA_X + 130, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub", "over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub", "down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub", },
		{ "name" : "left_seat_list_time_mouse_over_image", "type" : "expanded_image", "style" : ("not_pick",), "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "image" : "d:/ymir work/ui/game/party_match/button_over.sub", },
	] + window["children"][0]["children"]

	CUR_LINE_Y += LINE_STEP
	window["height"] = window["height"] + (LINE_STEP * 3)
	window["children"][0]["height"] = window["children"][0]["height"] + (LINE_STEP * 3)
	window["children"][0]["children"] = [
		{ "name" : "left_seat_logout_bar_text", "type" : "text", "x" : LINE_LABEL_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "text" : uiScriptLocale.LEFT_SEAT_LOGOUT,},
		{ "name" : "left_seat_logout_list_button", "type" : "button", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "text" : uiScriptLocale.LEFT_SEAT_180_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_one.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_one.sub", },
		{
			"name" : "left_seat_logout_list_window", "type" : "window", "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y+16, "width" : 130, "height" : 80,
			"children":
			(
				{ "name" : "left_seat_logout_30_min", "type" : "button", "x" : 0, "y" : 0, "text" : uiScriptLocale.LEFT_SEAT_30_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_top.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_top.sub", },
				{ "name" : "left_seat_logout_60_min", "type" : "button", "x" : 0, "y" : 16, "text" : uiScriptLocale.LEFT_SEAT_60_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_logout_120_min", "type" : "button", "x" : 0, "y" : 32, "text" : uiScriptLocale.LEFT_SEAT_120_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_logout_180_min", "type" : "button", "x" : 0, "y" : 48, "text" : uiScriptLocale.LEFT_SEAT_180_MIN, "default_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_middle.sub", },
				{ "name" : "left_seat_logout_off", "type" : "button", "x" : 0, "y" : 64, "text" : uiScriptLocale.LEFT_SEAT_OFF, "default_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "over_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", "down_image" : "d:/ymir work/ui/game/party_match/button_bottom.sub", },
			),
		},
		{ "name" : "left_seat_logout_list_arrow_button", "type" : "button", "x" : LINE_DATA_X + 130, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub", "over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub", "down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub", },
		{ "name" : "left_seat_logout_list_mouse_over_image", "type" : "expanded_image", "style" : ("not_pick",), "x" : LINE_DATA_X, "y" : LINE_BEGIN + LINE_STEP * CUR_LINE_Y, "image" : "d:/ymir work/ui/game/party_match/button_over.sub", },
	] + window["children"][0]["children"]

