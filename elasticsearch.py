import ime
import item
import player
import uiscriptlocale
import chat
import wndMgr
import skill

def clear_slot(window,page_size):
	for i in range(page_size):
		window.ClearSlot(i)

def set_item_slot(window, slot_number, item_id, item_count, search_text):
	if not item_id:
		return

	item_name = item.GetItemNameBySetValue(player.GetItemSetValue(slot_number))
	#item_name = item.GetItemName(item_id)

	if not search_text or search_text.lower() not in item_name.lower():
		window.DeactivateSlot(slot_number)
		return

	window.ActivateSlot(slot_number)
	window.SetSlotDiffuseColor(slot_number, wndMgr.COLOR_TYPE_ORANGE)


def unhighlight_matching_pages(page_buttons,current_page_index):
	for index,button in enumerate(page_buttons):
		button.DisableFlash()
		if index != current_page_index:
			button.SetUp()


def handle_tab_key(cls,item_id=None):
	item.SelectItem(item_id)
	cls.search_input.SetText(item.GetItemName())
	ime.SetCursorPosition(len(cls.search_input.GetText()) + 1)

	cls.search_input.SetFocus()
	cls.search_input.OnIMEUpdate()
	cls.unhighlight_matching_pages()
	cls.refresh_slot_page()


def highlight_matching_pages(get_func,page_count,page_size,page_buttons,current_page,search_text):
	if not search_text:
		return

	matched_pages = find_matching_pages(get_func,page_count,page_size,search_text)
	for index in matched_pages:
		if index != current_page:
			page_buttons[index].EnableFlash()
		else:
			page_buttons[index].DisableFlash()


def find_matching_pages(get_func,page_count,page_size,search_text):
	matched_page_buttons = set()

	for page in range(page_count):
		for i in range(page_size):
			item_id = get_func(page * page_size + i)
			if item_id:
				if player.IsMatchSlotText(item_id,search_text):
					matched_page_buttons.add(page)
	return matched_page_buttons


def generate_uiscript_window(formatting=tuple,x=0,y=0):
	return formatting(
		(
			dict(
				name = "SearchInputSlot",
				type = "image",
				x = x,
				y = y,
				image = "inventory/search_bar.png",
				children = (
					dict(
						name = "SearchInputValue",
						type = "editline",
						x = 4,
						y = 6,
						width = 130,
						height = 15,
						input_limit = 16,
						check_width = True,
					),
					dict(
						name = "SearchCancelButton",
						type = "button",
						x = 23,
						y = 1,
						horizontal_align = "right",
						default_image = "inventory/clear_text.png",
						over_image = "inventory/clear_text_d.png",
						down_image = "inventory/clear_text_d.png",
						tooltip_text = " ",
						tooltip_x = -45,
						tooltip_y = -5,
					),
				),
			),
		)
	)
