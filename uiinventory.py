import ui
import player
import mouseModule
import net
import app
import snd
import item
import player
import chat
import grp
import uiScriptLocale
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
import uiPrivateShopBuilder # ���λ��� ������ ItemMove ����
import localeInfo
import constInfo
import osfInfo
import ime
import wndMgr
import weakref
import uispecialinventory
if app.ENABLE_SEARCH_INVENTORY_INPUT:
	import elasticsearch

if app.BL_67_ATTR:
	import uiattr67Add

if app.__ENABLE_NEW_OFFLINESHOP__:
	import offlineshop
	import uiofflineshop

if app.ENABLE_CHEQUE_SYSTEM:
	import uiToolTip
	import uiPickETC

if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import acce

if app.ENABLE_GROWTH_PET_SYSTEM:
	import uiPetInfo
if app.ITEM_CHECKINOUT_UPDATE:
	import exchange

ITEM_MALL_BUTTON_ENABLE = True
ITEM_FLAG_APPLICABLE = 1 << 14

def ReprGetItemIndex(window):
	return lambda pos : player.GetItemIndex(window, pos)

class CostumeWindow(ui.ScriptWindow):
	def __init__(self, wndInventory):
		import exception

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleBtnList = []
			self.visibleSlotList = []

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		self.RefreshCostumeSlot()

		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		return x - 138, y

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		self.SetPosition(bx, by)
		self.SetSize(self.GetWidth(), self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CostumeWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			wndEquip = self.GetChild("CostumeSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.visibleBtnList.append(self.GetChild("BodyToolTipButton")) ## 0
				self.visibleBtnList.append(self.GetChild("HairToolTipButton")) ## 1
				self.visibleBtnList.append(self.GetChild("AcceToolTipButton")) ## 2
				self.visibleBtnList.append(self.GetChild("WeaponToolTipButton")) ## 3
				self.visibleBtnList.append(self.GetChild("AuraToolTipButton"))  ## 4

				self.visibleSlotList.append(self.GetChild("VisibleBodySlotImg")) ## 0
				self.visibleSlotList.append(self.GetChild("VisibleHairSlotImg")) ## 1
				self.visibleSlotList.append(self.GetChild("VisibleAcceSlotImg")) ## 2
				self.visibleSlotList.append(self.GetChild("VisibleWeaponSlotImg")) ## 3
				self.visibleSlotList.append(self.GetChild("VisibleAuraSlotImg"))  ## 4

			# if app.ENABLE_CUBE_RENEWAL:
			# 	self.listHighlightedCubeSlot = []

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndEquip = wndEquip

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleBtnList[0].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 1, 0)
			self.visibleBtnList[0].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 1, 1)

			self.visibleBtnList[1].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 2, 0)
			self.visibleBtnList[1].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 2, 1)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.visibleBtnList[2].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 3, 0)
				self.visibleBtnList[2].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 3, 1)

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.visibleBtnList[3].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 4, 0)
				self.visibleBtnList[3].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 4, 1)

			self.visibleBtnList[4].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 5, 0)
			self.visibleBtnList[4].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 5, 1)

			for slot in xrange(5):
				self.visibleSlotList[slot].Hide()

	def RefreshCostumeSlot(self):
		getItemVNum = player.GetItemIndex

		for i in xrange(item.COSTUME_SLOT_COUNT):
			slotNumber = item.COSTUME_SLOT_START + i
			self.wndEquip.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.EQUIPMENT, slotNumber) == 0:
					self.wndEquip.SetSlotCoverImage(slotNumber, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndEquip.EnableSlotCoverImage(slotNumber, False)

		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			self.wndEquip.SetItemSlot(item.COSTUME_SLOT_WEAPON, getItemVNum(item.COSTUME_SLOT_WEAPON), 0)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.EQUIPMENT, item.COSTUME_SLOT_WEAPON) == 0:
					self.wndEquip.SetSlotCoverImage(item.COSTUME_SLOT_WEAPON, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndEquip.EnableSlotCoverImage(item.COSTUME_SLOT_WEAPON, False)

		self.wndEquip.RefreshSlot()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			body = osfInfo.HIDDEN_BODY_COSTUME
			self.SetHideEvent(0, body)

			hair = osfInfo.HIDDEN_HAIR_COSTUME
			self.SetHideEvent(1, hair)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				acce = osfInfo.HIDDEN_ACCE_COSTUME
				self.SetHideEvent(2, acce)

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				weapon = osfInfo.HIDDEN_WEAPON_COSTUME
				self.SetHideEvent(3, weapon)

			aura = osfInfo.HIDDEN_AURA_COSTUME
			self.SetHideEvent(4, aura)

		def SetHideEvent(self, slot, hide = False):
			if hide:
				self.visibleBtnList[slot].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleBtnList[slot].SetUpVisual("d:/ymir work/ui/game/costume/eye_closed_01.tga")
				self.visibleBtnList[slot].SetOverVisual("d:/ymir work/ui/game/costume/eye_closed_02.tga")
				self.visibleBtnList[slot].SetDownVisual("d:/ymir work/ui/game/costume/eye_closed_02.tga")
				self.visibleSlotList[slot].Show()
			else:
				self.visibleBtnList[slot].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleBtnList[slot].SetUpVisual("d:/ymir work/ui/game/costume/eye_normal_01.tga")
				self.visibleBtnList[slot].SetOverVisual("d:/ymir work/ui/game/costume/eye_normal_02.tga")
				self.visibleBtnList[slot].SetDownVisual("d:/ymir work/ui/game/costume/eye_normal_02.tga")
				self.visibleSlotList[slot].Hide()

		def VisibleCostume(self, part, hidden):
			net.SendChatPacket("/hide_costume %d %d" % (part, hidden))

		def OnPressEscapeKey(self):
			self.Close()

class BeltInventoryWindow(ui.ScriptWindow):
	def __init__(self, wndInventory):
		import exception

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		self.wndBeltInventoryLayer = None
		self.wndBeltInventorySlot = None
		self.expandBtn = None
		if not app.ENABLE_BELT_RENEWAL:
			self.minBtn = None

		self.UseItemBelt = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, openBeltSlot = False):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)

		if openBeltSlot:
			self.OpenInventory()
		else:
			self.CloseInventory()

		self.SetTop()

	def Close(self):
		self.Hide()

	def IsOpeningInventory(self):
		return self.wndBeltInventoryLayer.IsShow()

	def OpenInventory(self):
		self.wndBeltInventoryLayer.Show()
		self.expandBtn.Hide()

		if localeInfo.IsARABIC() == 0:
			self.AdjustPositionAndSize()
		else:
			self.AdjustPositionAndSizeAE()

	def CloseInventory(self):
		self.wndBeltInventoryLayer.Hide()
		self.expandBtn.Show()

		if localeInfo.IsARABIC() == 0:
			self.AdjustPositionAndSize()
		else:
			self.AdjustPositionAndSizeAE()

	## ���� �κ��丮 ��ġ�� �������� BASE ��ġ�� ���, ����.. ���� �ϵ��ڵ��ϱ� ���� ������ ����� ����..
	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		return x - 148, y + 50

	if localeInfo.IsARABIC():
		def AdjustPositionAndSizeAE(self):
			bx, by = self.GetBasePosition()

			if self.IsOpeningInventory():
				self.SetPosition(bx, by);
				self.wndBeltInventoryLayer.SetPosition(self.ORIGINAL_WIDTH-5, 0)
			else:
				self.SetPosition(bx + 140, by);
				self.wndBeltInventoryLayer.SetPosition(self.ORIGINAL_WIDTH - 10, 0)
				self.expandBtn.SetPosition(self.ORIGINAL_WIDTH + 1, 15);

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		if self.IsOpeningInventory():
			if app.ENABLE_BELT_RENEWAL:
				self.SetPosition(bx - 63, by)
			else:
				self.SetPosition(bx, by)
			self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())

		else:
			self.SetPosition(bx + 138, by);
			self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/BeltInventoryWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndBeltInventorySlot = self.GetChild("BeltInventorySlot")
			self.wndBeltInventoryLayer = self.GetChild("BeltInventoryLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.UseItemBelt = self.GetChild("UseBeltItemsButton")
			if not app.ENABLE_BELT_RENEWAL:
				self.minBtn = self.GetChild("MinimizeBtn")

			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.UseItemBelt.SetEvent(ui.__mem_func__(self.ActivateAll))
			if not app.ENABLE_BELT_RENEWAL:
				self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))
			else:
				self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.CloseInventory))

			if localeInfo.IsARABIC():
				if not app.ENABLE_BELT_RENEWAL:
					self.minBtn.SetPosition(self.minBtn.GetWidth() + 3, 15)

			for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
				slotNumber = item.BELT_INVENTORY_SLOT_START + i
				wndBeltInventorySlot.SetCoverButton(slotNumber,
					"d:/ymir work/ui/game/belt_inventory/no_img.tga",\
					"d:/ymir work/ui/game/belt_inventory/no_img.tga",\
					"d:/ymir work/ui/game/belt_inventory/no_img.tga",\
					"d:/ymir work/ui/game/belt_inventory/slot_disabled.tga", False, False)

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndBeltInventorySlot.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndBeltInventorySlot.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndBeltInventorySlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndBeltInventorySlot.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		## MT-436 slot bug fix
		#wndBeltInventorySlot.SetSlotType(player.SLOT_TYPE_BELT_INVENTORY)
		self.wndBeltInventorySlot = wndBeltInventorySlot

	def RefreshSlot(self):
		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount

		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			if app.ENABLE_BELT_RENEWAL:
				itemCount = getItemCount(slotNumber)
				if itemCount <= 1:
					itemCount = 0

				self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), itemCount)
			else:
				self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), player.GetItemCount(slotNumber))

			self.wndBeltInventorySlot.SetAlwaysRenderCoverButton(slotNumber, True)

			avail = "0"

			if player.IsAvailableBeltInventoryCell(slotNumber):
				self.wndBeltInventorySlot.EnableCoverButton(slotNumber)
			else:
				self.wndBeltInventorySlot.DisableCoverButton(slotNumber)

		self.wndBeltInventorySlot.RefreshSlot()

	def OnTop(self):
		if self.wndInventory:
			self.wndInventory.SetClickBeltInventory(True)
			self.wndInventory.SetTop()

	def ActivateAll(self):
		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			net.SendItemUsePacket(slotNumber)

class InventoryWindow(ui.ScriptWindow):
	USE_TYPE_TUPLE = (
		"USE_CLEAN_SOCKET",
		"USE_CHANGE_ATTRIBUTE",
		"USE_CHANGE_ATTRIBUTE2",
		"USE_ADD_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE2",
		"USE_ADD_ACCESSORY_SOCKET",
		"USE_PUT_INTO_ACCESSORY_SOCKET",
		"USE_PUT_INTO_BELT_SOCKET",
		"USE_PUT_INTO_RING_SOCKET",
		"USE_CHANGE_COSTUME_ATTR",
		"USE_RESET_COSTUME_ATTR",
		"USE_PUT_INTO_AURA_SOCKET"
		"USE_ELEMENT_UPGRADE",
		"USE_ELEMENT_DOWNGRADE",
		"USE_ELEMENT_CHANGE"
	)
	if app.ENABLE_COSTUME_EXTENDED_RECHARGE:
		USE_TYPE_LIST = list(USE_TYPE_TUPLE)
		USE_TYPE_LIST.append("USE_TIME_CHARGE_PER")
		USE_TYPE_LIST.append("USE_TIME_CHARGE_FIX")
		USE_TYPE_TUPLE = tuple(USE_TYPE_LIST)

	questionDialog = None
	tooltipItem = None
	wndCostume = None
	wndBelt = None
	dlgPickMoney = None
	if app.ENABLE_CHEQUE_SYSTEM:
		dlgPickETC = None

	sellingSlotNumber = -1
	isLoaded = 0
	isOpenedCostumeWindowWhenClosingInventory = 0 # �κ��丮 ���� �� �ڽ����� �����־����� ����-_-; ���̹� ����
	isOpenedBeltWindowWhenClosingInventory = 0 # �κ��丮 ���� �� ��Ʈ �κ��丮�� �����־����� ����-_-; ���̹� ����

	interface = None

	if app.ENABLE_GROWTH_PET_SYSTEM:
		petHatchingWindow = None
		petFeedWindow = None
		petNameChangeWindow = None

		if app.ENABLE_PET_ATTR_DETERMINE:
			petChangeAttrWindow = None

	def __init__(self):
		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.petHatchingWindow = None
			self.petFeedWindow = None
			self.petNameChangeWindow = None

			if app.ENABLE_PET_ATTR_DETERMINE:
				self.petChangeAttrWindow = None
				self.petAttrChangeWindow = None

		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyBar = None
			self.wndGem = None

		ui.ScriptWindow.__init__(self)

		self.isOpenedBeltWindowWhenClosingInventory = 0 # �κ��丮 ���� �� ��Ʈ �κ��丮�� �����־����� ����-_-; ���̹� ����

		self.__LoadWindow()

		self.IsClickBeltInventory = False

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyBar = None
			self.wndGem = None

	def SetClickBeltInventory(self, isclick):
		self.IsClickBeltInventory = isclick

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

		self.RefreshItemSlot()
		self.RefreshStatus()

		# �κ��丮�� ���� �� �ڽ����� �����־��ٸ� �κ��丮�� �� �� �ڽ����� ���� ������ ��.
		if self.isOpenedCostumeWindowWhenClosingInventory and self.wndCostume:
			self.wndCostume.Show()

		# �κ��丮�� ���� �� ��Ʈ �κ��丮�� �����־��ٸ� ���� ������ ��.
		if self.wndBelt:
			self.wndBelt.Show(self.isOpenedBeltWindowWhenClosingInventory)

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyBar:
				self.wndExpandedMoneyBar.Show()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()

			# if ITEM_MALL_BUTTON_ENABLE:
			# 	pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "InventoryWindow.py")
			# else:
			pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindow.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
		#	wndEquip = self.GetChild("EquipmentSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.wndMoney = self.GetChild("Money")
			self.wndMoneySlot = self.GetChild("Money_Slot")
		#	self.mallButton = self.GetChild2("MallButton")
		#	self.DSSButton = self.GetChild2("DSSButton")
		#	self.costumeButton = self.GetChild2("CostumeButton")
			if app.ENABLE_SORT_INVENTORY:
				self.sortInventoryButton = self.GetChild2("SortInventoryButton")

			if app.ENABLE_CHEQUE_SYSTEM:
				self.wndCheque = self.GetChild("Cheque")
				self.wndChequeSlot = self.GetChild("Cheque_Slot")

				if app.ENABLE_GEM_SYSTEM:
					self.wndMoneyIcon = self.GetChild("Money_Icon")
					self.wndChequeIcon = self.GetChild("Cheque_Icon")
					self.wndMoneyIcon.Hide()
					self.wndMoneySlot.Hide()
					self.wndChequeIcon.Hide()
					self.wndChequeSlot.Hide()

					## ���� ����
					height = self.GetHeight()
					width = self.GetWidth()
					self.SetSize(width, height - 22)
					self.GetChild("board").SetSize(width, height - 22)
				else:
					self.wndMoneyIcon = self.GetChild("Money_Icon")
					self.wndChequeIcon = self.GetChild("Cheque_Icon")

					self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 0)
					self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 1)

					self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 0)
					self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 1)

					self.toolTip = uiToolTip.ToolTip()
					self.toolTip.ClearToolTip()

			self.inventoryTab = []
			self.inventoryTab.append(self.GetChild("Inventory_Tab_01"))
			self.inventoryTab.append(self.GetChild("Inventory_Tab_02"))
			if app.ENABLE_EXTEND_INVEN_SYSTEM:
				self.inventoryTab.append(self.GetChild("Inventory_Tab_03"))
				self.inventoryTab.append(self.GetChild("Inventory_Tab_04"))

			#self.equipmentTab = []
			#self.equipmentTab.append(self.GetChild("Equipment_Tab_01"))
			#self.equipmentTab.append(self.GetChild("Equipment_Tab_02"))

			# Belt Inventory Window
			self.wndBelt = None

			if app.ENABLE_SEARCH_INVENTORY_INPUT:
				self.search_input = self.GetChild('SearchInputValue')
				self.search_input.OnKeyDown = ui.__mem_func__(self.handle_search_key_down_event)
				self.search_input.OnIMEUpdate = ui.__mem_func__(self.handle_search_key_update_event)
				self.search_input.SetEscapeEvent(self.OnPressEscapeKey)
				self.search_input.SetTabEvent(lambda: None)
				self.clear_search_input()

				self.search_cancel_button = self.GetChild('SearchCancelButton')
				self.search_cancel_button.SAFE_SetEvent(self.handle_cancel_search_event)

			self.dlgQuestion = uiCommon.QuestionDialog2()
			self.dlgQuestion.Close()

			if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
				self.wndBelt = BeltInventoryWindow(self)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.listHighlightedAcceSlot = []

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				self.listHighlightedChangeLookSlot = []

			if app.ENABLE_CUBE_RENEWAL:
				self.listHighlightedCubeSlot = []

			if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
				self.listHighlightedSlot = []

			if app.ENABLE_GROWTH_PET_SYSTEM:
				self.PetItemQuestionDlg = uiCommon.QuestionDialog()
				self.PetItemQuestionDlg.Close()

			if app.ENABLE_GROWTH_PET_SYSTEM and app.ENABLE_GROWTH_PET_SKILL_DEL:
				self.PetSkillDelDlg = uiCommon.QuestionDialog2()
				self.PetSkillDelDlg.Close()
				self.PetSkillAllDelBookIndex = -1

		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## Equipment
	#	wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
	#	wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
	#	wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
	#	wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
	#	wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
	#	wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		## PickETCDialog
		if app.ENABLE_CHEQUE_SYSTEM:
			dlgPickETC = uiPickETC.PickETCDialog()
			dlgPickETC.LoadDialog()
			dlgPickETC.Hide()
			self.dlgPickETC = dlgPickETC

		## RefineDialog
		self.refineDialog = uiRefine.RefineDialog()
		self.refineDialog.Hide()

		## AttachMetinDialog
		self.attachMetinDialog = uiAttachMetin.AttachMetinDialog()
		self.attachMetinDialog.Hide()

		## MoneySlot
		if app.ENABLE_CHEQUE_SYSTEM:
			#self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 0)
			#self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 1)
			self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindowSell))
			self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindowBuy))
		else:
			self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindowSell))

		if app.ENABLE_EXTEND_INVEN_SYSTEM:
			for inven_index in range(player.INVENTORY_PAGE_COUNT): ## 0, 1, 2, 3
				self.inventoryTab[inven_index].SetEvent(lambda arg=inven_index: self.SetInventoryPage(arg))
		else:
			self.inventoryTab[0].SetEvent(lambda arg = 0: self.SetInventoryPage(arg))
			self.inventoryTab[1].SetEvent(lambda arg = 1: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		# self.equipmentTab[0].SetEvent(lambda arg = 0: self.SetEquipmentPage(arg))
		# self.equipmentTab[1].SetEvent(lambda arg = 1: self.SetEquipmentPage(arg))
		# self.equipmentTab[0].Down()
		# self.equipmentTab[0].Hide()
		# self.equipmentTab[1].Hide()

		self.wndItem = wndItem
	#	self.wndEquip = wndEquip
		self.dlgPickMoney = dlgPickMoney

		# MallButton
		#if self.mallButton:
		#	self.mallButton.SetEvent(ui.__mem_func__(self.ClickMallButton))

		#if self.DSSButton:
		#	self.DSSButton.SetEvent(ui.__mem_func__(self.ClickDSSButton))

		# Costume Button
		#if self.costumeButton:
		#	self.costumeButton.SetEvent(ui.__mem_func__(self.ClickCostumeButton))

	#	self.wndCostume = None

		#####

		if app.ENABLE_SORT_INVENTORY:
			if self.sortInventoryButton:
				self.sortInventoryButton.SetEvent(ui.__mem_func__(self.SortInventoryButton))

		## Refresh
		self.SetInventoryPage(0)
		#self.SetEquipmentPage(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

	if app.ENABLE_SEARCH_INVENTORY_INPUT:
		def handle_cancel_search_event(self):
			self.clear_search_input()
			self.unhighlight_matching_pages()
			self.refresh_slot_page()
			return True

		def handle_search_key_down_event(self,key):
			self.unhighlight_matching_pages()
			self.RefreshBagSlotWindow()
			return True


		def handle_search_key_update_event(self):
			ui.EditLine.OnIMEUpdate(self.search_input)

			if not self.get_search_text():
				self.handle_cancel_search_event()

		def clear_search_input(self):
			self.search_input.SetText('')
			self.search_input.SetOverlayText(uiScriptLocale.PRIVATESHOPSEARCH_SEARCH)

		def kill_focus_search_input(self):
			if self.search_input.IsFocus():
				self.search_input.KillFocus()

		def get_search_text(self):
			return self.search_input.GetText()

		def refresh_slot_page(self):
			self.RefreshBagSlotWindow()

		def highlight_matching_pages(self):
			elasticsearch.highlight_matching_pages(lambda x: player.GetItemIndex(player.INVENTORY, x), player.INVENTORY_PAGE_COUNT, player.INVENTORY_PAGE_SIZE, self.inventoryTab, self.inventoryPageIndex, self.get_search_text())

		def unhighlight_matching_pages(self):
			elasticsearch.unhighlight_matching_pages(self.inventoryTab, self.inventoryPageIndex)

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		if app.ENABLE_CHEQUE_SYSTEM:
			self.dlgPickETC.Destroy()
			self.dlgPickETC = 0

		self.refineDialog.Destroy()
		self.refineDialog = 0

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		#self.wndEquip = 0
		self.dlgPickMoney = 0
		self.wndMoney = 0
		self.wndMoneySlot = 0
		self.questionDialog = None
		#self.mallButton = None
		#self.DSSButton = None
		#self.costumeButton = None
		self.interface = None

		if app.ENABLE_CHEQUE_SYSTEM:
			self.wndCheque = 0
			self.wndChequeSlot = 0
			self.dlgPickETC = 0

	#	if self.wndCostume:
	#		self.wndCostume.Destroy()
	#		self.wndCostume = 0

		if self.wndBelt:
			self.wndBelt.Destroy()
			self.wndBelt = None

	#	self.inventoryTab = []
	#	self.equipmentTab = []

		if app.ENABLE_GROWTH_PET_SYSTEM:
			if self.petHatchingWindow:
				self.petHatchingWindow = None

			if self.petFeedWindow:
				self.petFeedWindow = None

			if self.petNameChangeWindow:
				self.petNameChangeWindow = None

			if app.ENABLE_PET_ATTR_DETERMINE:
				if self.petAttrChangeWindow:
					self.petAttrChangeWindow = None

		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyBar = None

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.wndBelt:
			self.isOpenedBeltWindowWhenClosingInventory = self.wndBelt.IsOpeningInventory() # �κ��丮 â�� ���� �� ��Ʈ �κ��丮�� ���� �־��°�?
			print "Is Opening Belt Inven?? ", self.isOpenedBeltWindowWhenClosingInventory
			self.wndBelt.Close()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		if app.ENABLE_CHEQUE_SYSTEM:
			if self.dlgPickETC:
				self.dlgPickETC.Close()

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyBar:
				self.wndExpandedMoneyBar.Close()

		wndMgr.Hide(self.hWnd)

	def Close(self):
		self.Hide()
		if app.ENABLE_SEARCH_INVENTORY_INPUT:
			self.kill_focus_search_input()

	if app.ENABLE_GEM_SYSTEM:
		def SetExpandedMoneyBar(self, wndBar):
			self.wndExpandedMoneyBar = wndBar
			if self.wndExpandedMoneyBar:
				self.wndMoneySlot = self.wndExpandedMoneyBar.GetMoneySlot()
				self.wndMoney = self.wndExpandedMoneyBar.GetMoney()
				if app.ENABLE_CHEQUE_SYSTEM:
					## �� ����
					self.wndMoneyIcon = self.wndExpandedMoneyBar.GetMoneyIcon()
					if self.wndMoneyIcon:
						self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 0)
						self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 0)
					if self.wndMoneySlot:
						#self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 0)
						self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindowSell))
					## 2��ȭ�� ����
					self.wndChequeIcon = self.wndExpandedMoneyBar.GetChequeIcon()
					if self.wndChequeIcon:
						self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 1)
						self.wndChequeIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 1)
					self.wndChequeSlot = self.wndExpandedMoneyBar.GetChequeSlot()
					if self.wndChequeSlot:
						#self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog), 1)
						self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindowBuy))
					self.wndCheque = self.wndExpandedMoneyBar.GetCheque()
					## ���� ����
					self.wndGemIcon = self.wndExpandedMoneyBar.GetGemIcon()
					if self.wndGemIcon:
						self.wndGemIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 2)
						self.wndGemIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 2)
					self.wndGem = self.wndExpandedMoneyBar.GetGem()
					self.toolTip = uiToolTip.ToolTip()
					self.toolTip.ClearToolTip()
				else:
					if self.wndMoneySlot:
						self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindowSell))

	if app.ENABLE_EXTEND_INVEN_SYSTEM:
		def SetInventoryPage(self, page):
			self.inventoryPageIndex = page

			for index in range(len(self.inventoryTab)):
				if index == page:
					continue
				self.inventoryTab[index].SetUp()

			self.RefreshBagSlotWindow()
	else:
		def SetInventoryPage(self, page):
			self.inventoryPageIndex = page
			self.inventoryTab[1-page].SetUp()
			self.RefreshBagSlotWindow()

#	def SetEquipmentPage(self, page):
#		self.equipmentPageIndex = page
#		self.equipmentTab[1-page].SetUp()
#		self.RefreshEquipSlotWindow()

	def ClickMallButton(self):
		if self.interface.dlgPassword.IsShow():
			self.interface.dlgPassword.CloseDialog()
		else:
			net.SendChatPacket("/click_mall")

	# DSSButton
	def ClickDSSButton(self):
		self.interface.ToggleDragonSoulWindow()

	def ClickCostumeButton(self):
		if self.wndCostume:
			if self.wndCostume.IsShow():
				self.wndCostume.Hide()
			else:
				self.wndCostume.Show()
		else:
			self.wndCostume = CostumeWindow(self)
			self.wndCostume.Show()

	# def ShowCostumeInventory(self):
	# 	if self.wndCostume:
	# 		if not self.wndCostume.IsShow():
	# 			self.wndCostume.Show()
	# 	else:
	# 		self.wndCostume = CostumeWindow(self)
	# 		self.wndCostume.Show()

	if app.ENABLE_SORT_INVENTORY:
		def SortInventoryButton(self):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_DO_YOU_SORT)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnSortInventoryItems))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		def OnSortInventoryItems(self):
			if None == self.questionDialog:
				return

			net.SendChatPacket("/sort_inventory")
			snd.PlaySound("sound/ui/pickup_item_in_inventory.wav")

			self.OnCloseQuestionDialog()

	if app.ENABLE_CHEQUE_SYSTEM:
		def OpenPickMoneyDialog(self, focus_idx = 0):
			if mouseModule.mouseController.isAttached():

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():
					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")
					elif player.ITEM_CHEQUE == mouseModule.mouseController.GetAttachedItemIndex() and app.ENABLE_CHEQUE_SYSTEM:
						net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

				mouseModule.mouseController.DeattachObject()
			else:
				curMoney = player.GetElk()
				curCheque = player.GetCheque()

				if curMoney <= 0:
					return

				self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
				self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
				self.dlgPickMoney.Open(curMoney, curCheque)
				self.dlgPickMoney.SetMax(7) # �κ��丮 990000 ���� ���� ����
				self.dlgPickMoney.SetFocus(focus_idx)
	else:
		def OpenPickMoneyDialog(self):
			if mouseModule.mouseController.isAttached():

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():
					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

				mouseModule.mouseController.DeattachObject()
			else:
				curMoney = player.GetElk()

				if curMoney <= 0:
					return

				self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
				self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
				self.dlgPickMoney.Open(curMoney)
				self.dlgPickMoney.SetMax(7)

	def OpenWonExchangeWindow(self):
		self.interface.ToggleWonExchangeWindow()

	def OpenWonExchangeWindowSell(self):
		self.interface.ToggleWonExchangeWindowSell()

	def OpenWonExchangeWindowBuy(self):
		self.interface.ToggleWonExchangeWindowBuy()

	if app.ENABLE_CHEQUE_SYSTEM:
		def OnPickMoney(self, money, cheque):
			mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money, cheque)
	else:
		def OnPickMoney(self, money):
			mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		if app.ENABLE_CHEQUE_SYSTEM:
			itemSlotIndex = self.dlgPickETC.itemGlobalSlotIndex
		else:
			itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex

		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, itemSlotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or (player.IsBeltInventorySlot(local)):
			return local

		if app.ENABLE_SPECIAL_INVENTORY_SYSTEM:
			if player.IsSkillBookInventorySlot(local) or player.IsUpgradeItemsInventorySlot(local) or player.IsStoneInventorySlot(local) or player.IsGiftBoxInventorySlot(local) or player.IsAttributeInventorySlot(local):
				return local

		return self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE + local



	def __SpecialInventoryLocalSlotPosToGlobalSlotPosCube(self, typeIndex, pageIndex, local,window_type=player.INVENTORY):
	# 	if self.inventoryType == 0:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.SKILL_BOOK_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 1:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.UPGRADE_ITEMS_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 2:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.STONE_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 3:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.GIFT_BOX_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 4:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.ATTRIBUTE_INVENTORY_SLOT_START
		if player.INVENTORY == window_type:
			start = item.SKILL_BOOK_INVENTORY_SLOT_START
			slotCountPerType = (player.SPECIAL_INVENTORY_PAGE_SIZE * player.SPECIAL_INVENTORY_PAGE_COUNT)
			slotStartForType = start + slotCountPerType * (typeIndex - 1)
			slotPageStart = slotStartForType + (player.SPECIAL_INVENTORY_PAGE_SIZE * pageIndex)

			return slotPageStart + local
		else:
			return local

	# def __InventoryLocalSlotPosToGlobalSlotPosCube(self, local):
	# 	if self.inventoryType == 0:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.SKILL_BOOK_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 1:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.UPGRADE_ITEMS_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 2:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.STONE_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 3:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.GIFT_BOX_INVENTORY_SLOT_START
	# 	elif self.inventoryType == 4:
	# 		return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.ATTRIBUTE_INVENTORY_SLOT_START

	def GetSlotListByVnum(self, vnum, count=1):
		itemSlots = list()

		counts = 0
	#	for special_index in range(player.INVENTORY_PAGE_SIZE):
	#	for page in xrange(0, 4):
		for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
			#if app.ENABLE_SPECIAL_INVENTORY_SYSTEM:
			#	slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPosCube(special_index)
			#else:
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			if player.GetItemIndex(slotNumber) == vnum:
				if app.ENABLE_CUBE_RENEWAL and app.ENABLE_SET_ITEM:
					if player.GetItemSetValue(player.INVENTORY, slotNumber):
						continue

				tempSlotNum = slotNumber
				# if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
				# 	tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				itemSlots.append(tempSlotNum)

				counts += player.GetItemCount(slotNumber)

				if counts >= count:
					return itemSlots

		return itemSlots

	def RefreshBagSlotWindow(self):
		if not self.wndItem:
			return

		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setItemVNum = self.wndItem.SetItemSlot

		if app.ENABLE_SEARCH_INVENTORY_INPUT:
			elasticsearch.clear_slot(self.wndItem, player.INVENTORY_PAGE_SIZE)
			#self.highlight_matching_pages()

		## �������� �ϱ����� ���̶���Ʈ ��� ����.
		for i in xrange(self.wndItem.GetSlotCount()):
			self.wndItem.DeactivateSlot(i)

		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				onTopWindow = self.interface.GetOnTopWindow()

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(slotNumber)
			# itemCount == 0�̸� ������ ����.
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)
			if app.ENABLE_SEARCH_INVENTORY_INPUT:
				elasticsearch.set_item_slot(self.wndItem, i, itemVnum, itemCount, self.get_search_text())
				#self.highlight_matching_pages()

			## �ڵ����� (HP: #72723 ~ #72726, SP: #72727 ~ #72730) Ư��ó�� - �������ε��� ���Կ� Ȱ��ȭ/��Ȱ��ȭ ǥ�ø� ���� �۾��� - [hyo]
			if constInfo.IS_AUTO_POTION(itemVnum):
				# metinSocket - [0] : Ȱ��ȭ ����, [1] : ����� ��, [2] : �ִ� �뷮
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]	
				tempSlotNum = slotNumber
				if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
					tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				isActivated = 0 != metinSocket[0]

				if isActivated:
					self.wndItem.ActivateSlot(tempSlotNum)
				else:
					self.wndItem.DeactivateSlot(tempSlotNum)

			if app.WJ_ENABLE_TRADABLE_ICON:
				if itemVnum and self.interface and onTopWindow:
					if self.interface.MarkUnusableInvenSlotOnTopWnd(onTopWindow,slotNumber):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				else:
					self.wndItem.SetUsableSlotOnTopWnd(i)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				for j in xrange(acce.WINDOW_MAX_MATERIALS):
					(isHere, iCell) = acce.GetAttachedItem(j)
					if isHere:
						if iCell == slotNumber:
							self.__AddHighlightSlotAcce(slotNumber)
					else:
						self.__DelHighlightSlotAcce(slotNumber)

			if app.ENABLE_SOUL_SYSTEM:
				if item.IsSoulItem(itemVnum):
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					tempSlotNum = slotNumber
					if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
						tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

					if 0 != metinSocket[1]:
						self.wndItem.ActivateSlot(tempSlotNum)
					else:
						self.wndItem.DeactivateSlot(tempSlotNum)

			## [MT-662] �Ϲ��� ��ȯǥ�� ��ũ
			# if item.ITEM_TYPE_PET == item.GetItemType() and item.PET_PAY == item.GetItemSubType():
			# 	metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
			# 	tempSlotNum = slotNumber
			# 	if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
			# 		tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)
			# 	if 0 != metinSocket[1]:
			# 		self.wndItem.ActivateSlot(tempSlotNum)
			# 	else:
			# 		self.wndItem.DeactivateSlot(tempSlotNum)

			if app.ENABLE_GROWTH_PET_SYSTEM:
				if constInfo.IS_PET_ITEM(itemVnum):
					self.__ActivePetHighlightSlot(slotNumber)
			# 		self.__SetCollTimePetItemSlot(slotNumber, itemVnum)

			if app.ENABLE_EXTENDED_BLEND_AFFECT:
				if osfInfo.IS_BLEND_POTION(itemVnum) or osfInfo.IS_EXTENDED_BLEND_POTION(itemVnum):
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					tempSlotNum = slotNumber
					if tempSlotNum >= player.INVENTORY_PAGE_SIZE:
						tempSlotNum -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

					isActivated = 0 != metinSocket[3]
					if isActivated:
						self.wndItem.ActivateSlot(tempSlotNum)
						if osfInfo.IS_EXTENDED_BLEND_POTION(itemVnum):
							self.wndItem.SetSlotDiffuseColor(tempSlotNum, wndMgr.COLOR_TYPE_PURPLE)
						else:
							self.wndItem.SetSlotDiffuseColor(tempSlotNum, wndMgr.COLOR_TYPE_DARK_BLUE)
					else:
						self.wndItem.DeactivateSlot(tempSlotNum)

				# if osfInfo.IS_EXTENDED_BLEND_POTION(itemVnum):
				# 	self.wndItem.SetSlotCoverImage(i, "d:/ymir work/ui/acce/ingame_convert_mark_02.tga")
				# elif osfInfo.IS_BLEND_POTION(itemVnum):
				# 	self.wndItem.SetSlotCoverImage(i, "icon/item/trade.tga")
				# else:
				# 	if app.ENABLE_CHANGE_LOOK_SYSTEM:
				# 		if not player.GetChangeLookVnum(player.INVENTORY, slotNumber):
				# 			self.wndItem.EnableSlotCoverImage(i, False)
				# 	else:
				# 		self.wndItem.EnableSlotCoverImage(i, False)

		self.__HighlightSlot_Refresh()

		self.wndItem.RefreshSlot()
		if app.QUICK_SELL_SYSTEM:
			self.RefreshIconSlot()

		self.DelHighLightCube()

		#self.SetStoneIcon()

		if self.wndBelt:
			self.wndBelt.RefreshSlot()

	# def SetStoneIcon(self):
	# 	for j in xrange(player.INVENTORY_PAGE_SIZE * 5):
	# 		slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(j)
	# 		itemVnum = player.GetItemIndex(slotNumber)
	# 		if 28400 <= itemVnum <= 28447:
	# 			self.wndItem.SetSlotCoverImage(j, "icon/item/trade.tga")
	# 		else:
	# 			self.wndItem.EnableSlotCoverImage(j, False)

	if app.QUICK_SELL_SYSTEM:
		# def RefreshQuickSell(self):
		# 	for k in xrange(player.INVENTORY_PAGE_SIZE * 5):
		# 		slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(k)
		# 		itemVnum = player.GetItemIndex(slotNumber)
		# 		if 28400 <= itemVnum <= 28447:
		# 			self.wndItem.SetSlotCoverImage(k, "icon/item/trade.tga")
		# 		else:
		# 			self.wndItem.EnableSlotCoverImage(k, False)
		# 		if slotNumber in constInfo.QUICK_SELL_ITEMS:
		# 			self.wndItem.SetSlotCoverImage(k, "inventory/selected_icon.tga")
		# 		else:
		# 			self.wndItem.EnableSlotCoverImage(k, False)
		def RefreshIconSlot(self):
			for k in xrange(player.INVENTORY_PAGE_SIZE * 5):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(k)
				itemVnum = player.GetItemIndex(slotNumber)
				if slotNumber in constInfo.QUICK_SELL_ITEMS:
					if 28400 <= itemVnum <= 28447:
						self.wndItem.SetSlotCoverImage(k, "inventory/slot_cover_sell.tga")
					else:
						self.wndItem.SetSlotCoverImage(k, "inventory/selected_icon.tga")
				elif 28400 <= itemVnum <= 28447:
					self.wndItem.SetSlotCoverImage(k, "icon/item/slot_cover.tga")
				elif not player.GetChangeLookVnum(player.INVENTORY, slotNumber) == 0 and app.ENABLE_CHANGE_LOOK_SYSTEM:
					self.wndItem.SetSlotCoverImage(k, "icon/item/ingame_convert_Mark.tga")
				elif osfInfo.IS_EXTENDED_BLEND_POTION(itemVnum):
					self.wndItem.SetSlotCoverImage(k, "d:/ymir work/ui/acce/ingame_convert_mark_02.tga")
				elif osfInfo.IS_BLEND_POTION(itemVnum):
					self.wndItem.SetSlotCoverImage(k, "icon/item/trade.tga")
				else:
					self.wndItem.EnableSlotCoverImage(k, False)

	# def RefreshEquipSlotWindow(self):
	# 	getItemVNum = player.GetItemIndex
	# 	getItemCount = player.GetItemCount
	# 	setItemVNum = self.wndEquip.SetItemSlot
	#
	# 	for i in xrange(player.EQUIPMENT_PAGE_COUNT):
	# 		slotNumber = player.EQUIPMENT_SLOT_START + i
	# 		itemCount = getItemCount(slotNumber)
	# 		if itemCount <= 1:
	# 			itemCount = 0
	# 		setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
	#
	# 		if app.ENABLE_CHANGE_LOOK_SYSTEM:
	# 			if not player.GetChangeLookVnum(player.EQUIPMENT, slotNumber) == 0:
	# 				self.wndEquip.SetSlotCoverImage(slotNumber, "icon/item/ingame_convert_Mark.tga")
	# 			else:
	# 				self.wndEquip.EnableSlotCoverImage(slotNumber, False)
	#
	# 	if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
	# 		for i in xrange(player.NEW_EQUIPMENT_SLOT_COUNT):
	# 			slotNumber = player.NEW_EQUIPMENT_SLOT_START + i
	# 			itemCount = getItemCount(slotNumber)
	# 			if itemCount <= 1:
	# 				itemCount = 0
	# 			setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
	#
	# 	self.wndEquip.RefreshSlot()
	#
	# 	if self.wndCostume:
	# 		self.wndCostume.RefreshCostumeSlot()

	def RefreshItemSlot(self):
		self.RefreshBagSlotWindow()
#		self.RefreshEquipSlotWindow()

	def RefreshStatus(self):
		money = player.GetElk()
		self.wndMoney.SetText(localeInfo.NumberToMoneyString(money))

		if app.ENABLE_CHEQUE_SYSTEM:
			cheque = player.GetCheque()
			self.wndCheque.SetText(str(cheque))

		if app.ENABLE_GEM_SYSTEM:
			if self.wndGem:
				gem = player.GetGem()
				self.wndGem.SetText(localeInfo.NumberToMoneyString(gem))

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, self.sellingSlotNumber):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				## ��ȥ���� �ȸ��� �ϴ� ��� �߰��ϸ鼭 ���� type �߰�
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")

		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		#net.SendItemUseToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		## �Ǽ����� â�� ����������
		## ������ �̵� ����.
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if player.GetAcceRefineWindowOpen() == 1:
				return

		if app.ENABLE_AURA_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				return

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(
						player.SlotTypeToInvenType(attachedSlotType), attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
					return

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:

				if attachedSlotPos >= player.DRAGON_SOUL_EQUIPMENT_SLOT_START and attachedSlotPos < player.DRAGON_SOUL_EQUIPMENT_SLOT_END:
					mouseModule.mouseController.DeattachObject()
					return

				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedItemCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.interface.SetUseItemMode(False)

			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mouseModule.mouseController.RunCallBack("INVENTORY")

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)

			elif app.ENABLE_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos,
									   attachedCount)

			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")
				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif app.ENABLE_AURA_SYSTEM and player.SLOT_TYPE_AURA == attachedSlotType:
				net.SendAuraRefineCheckOut(attachedSlotPos, player.GetAuraRefineWindowType())

			if app.ENABLE_GUILDSTORAGE_SYSTEM and player.SLOT_TYPE_GUILDBANK == attachedSlotType:
				net.SendGuildstorageCheckoutPacket(attachedSlotPos, player.INVENTORY, selectedSlotPos)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if player.SLOT_TYPE_CHANGE_LOOK == attachedSlotType:
					pass

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if player.IsActingEmotion():
			return

		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()

			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)

			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)

				if itemCount > 1:
					if app.ENABLE_CHEQUE_SYSTEM:
						self.dlgPickETC.SetTitleName(localeInfo.PICK_ITEM_TITLE)
						self.dlgPickETC.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
						self.dlgPickETC.Open(itemCount)
						self.dlgPickETC.itemGlobalSlotIndex = itemSlotIndex
					else:
						self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
						self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
						self.dlgPickMoney.Open(itemCount)
						self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
				#else:
				#	selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				#	mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum)

			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)

				if True == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)
			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)

				if app.ENABLE_GROWTH_PET_SYSTEM:
					if self.__CanAttachGrowthPetItem(selectedItemVNum, itemSlotIndex):
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum,itemCount)
				else:
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.interface.SetUseItemMode(True)
				else:
					self.interface.SetUseItemMode(False)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if app.ENABLE_AURA_SYSTEM and player.IsAuraRefineWindowOpen():
			return
		if srcItemSlotPos == dstItemSlotPos:
			return

		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and (
					uiofflineshop.IsSaleSlot(player.INVENTORY, srcItemSlotPos) or uiofflineshop.IsSaleSlot(
					player.INVENTORY, dstItemSlotPos)):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		## �Ǽ����� â�� ����������
		## ������ �̵� ����.
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if player.GetAcceRefineWindowOpen() == 1:
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				return

		## [MT-462]
		player.GetItemIndex(srcItemSlotPos)
		if player.GetItemIndex(srcItemSlotPos) == player.GetItemIndex(dstItemSlotPos) \
			and (player.GetItemFlags(srcItemSlotPos) & item.ITEM_FLAG_STACKABLE) \
			and player.IsEquipmentSlot(dstItemSlotPos) == False:
			self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
			return

		if app.ENABLE_SOUL_BIND_SYSTEM:
			# cyh itemseal 2013 11 08
			if item.IsSealScroll(srcItemVID):
				if player.CanSealItem(srcItemVID, player.INVENTORY, dstItemSlotPos):
					self.__OpenQuestionDialog(srcItemSlotPos, dstItemSlotPos)

		if item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.interface.SetUseItemMode(False)

		elif item.IsMetin(srcItemVID) and not item.IsMetin(player.GetItemIndex(dstItemSlotPos)):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif app.ENABLE_REFINE_ELEMENT and item.IsElement(srcItemVID):
			self.ElementItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif app.ENABLE_67TH_BONUS and (srcItemVID == item.ADD_RARE_ATTRIBUTE_VNUM or srcItemVID == item.CHANGE_RARE_ATTRIBUTE_VNUM):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		else:
			if app.ENABLE_GROWTH_PET_SYSTEM:
				if self.__IsPetItem(srcItemVID):
					if self.__SendUsePetItemToItemPacket(srcItemVID, srcItemSlotPos, dstItemSlotPos):
						return

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVID):
					if dstItemSlotPos > player.EQUIPMENT_SLOT_START - 1:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_EQUIP_ITEM)
						return

					if player.CanChangeLookClearItem(srcItemVID, player.INVENTORY, dstItemSlotPos):
						self.__OpenQuestionDialog(srcItemSlotPos, dstItemSlotPos)
						return

			if app.ENABLE_SET_ITEM:
				if item.IsSetClearScroll(srcItemVID):
					if player.CanSetItemClear(srcItemVID, player.INVENTORY, dstItemSlotPos):
						self.__OpenSetClearItemQuestionDialog(srcItemSlotPos, dstItemSlotPos)
						return

			## �̵���Ų ���� ���� ������ ��� �������� ����ؼ� ���� ��Ų�� - [levites]
			if player.IsEquipmentSlot(dstItemSlotPos):

				## ��� �ִ� �������� ����϶���
				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)
			else:
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				# net.SendItemMovePacket(srcItemSlotPos, dstItemSlotPos, 0)

	if app.ENABLE_SET_ITEM:
		def __OpenSetClearItemQuestionDialog(self, srcItemPos, dstItemPos):
			if self.interface.IsShowDlgQuestionWindow():
				self.interface.CloseDlgQuestionWindow()

			getItemVNum = player.GetItemIndex
			self.srcItemPos = srcItemPos
			self.dstItemPos = dstItemPos

			# self.srcItemWindow = srcItemWindow
			# self.dstItemWindow = dstItemWindow

			self.dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.__SetClearItemAccept))
			self.dlgQuestion.SetCancelEvent(ui.__mem_func__(self.__SetClearItemCancel))

			self.dlgQuestion.SetText1(localeInfo.SET_ITEM_CLEAR_CONFIRM_QUESTION)
			self.dlgQuestion.SetText2(localeInfo.SET_ITEM_CLEAR_CONFIRM_QUESTION_2)
			self.dlgQuestion.Open()

			#constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		def __SetClearItemAccept(self):
			self.__Accept()

		def __SetClearItemCancel(self):
			self.__Cancel()

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def __OpenPetBagQuestionDialog(self, srcItemSlotPos, dstItemSlotPos):
			if self.interface.IsShowDlgQuestionWindow():
				self.interface.CloseDlgQuestionWindow()

		#def __OpenPetItemQuestionDialog(self, srcItemWindow, srcItemPos, dstItemWindow, dstItemPos):
		def __OpenPetItemQuestionDialog(self, srcItemPos, dstItemPos):
			if self.interface.IsShowDlgQuestionWindow():
				self.interface.CloseDlgQuestionWindow()

			getItemVNum=player.GetItemIndex
			self.srcItemPos = srcItemPos
			self.dstItemPos = dstItemPos

			#self.srcItemWindow = srcItemWindow
			#self.dstItemWindow = dstItemWindow

			#srcItemVnum = getItemVNum(srcItemWindow, srcItemPos)
			#dstItemVnum = getItemVNum(dstItemWindow, dstItemPos)

			srcItemVnum = getItemVNum(srcItemPos)
			dstItemVnum = getItemVNum(dstItemPos)

			item.SelectItem( srcItemVnum )
			src_item_name = item.GetItemName( srcItemVnum )
			srcItemType		= item.GetItemType()
			srcItemSubType	= item.GetItemSubType()

			item.SelectItem( dstItemVnum )
			#dst_item_name = item.GetItemName( getItemVNum(dstItemWindow, dstItemPos) )
			dst_item_name = item.GetItemName(getItemVNum(dstItemPos))

			self.PetItemQuestionDlg.SetAcceptEvent(ui.__mem_func__(self.__PetItemAccept))
			self.PetItemQuestionDlg.SetCancelEvent(ui.__mem_func__(self.__PetItemCancel))

			if item.ITEM_TYPE_PET == srcItemType:
				if item.PET_FEEDSTUFF == srcItemSubType:
					self.PetItemQuestionDlg.SetText(localeInfo.INVENTORY_REALLY_USE_PET_FEEDSTUFF_ITEM % (src_item_name, dst_item_name))
					self.PetItemQuestionDlg.Open()

				elif item.PET_BAG == srcItemSubType:
					self.PetItemQuestionDlg.SetText(localeInfo.INVENTORY_REALLY_USE_PET_BAG_ITEM)
					self.PetItemQuestionDlg.Open()

		def __PetItemAccept(self):
			self.PetItemQuestionDlg.Close()

			self.__SendUseItemToItemPacket(self.srcItemPos, self.dstItemPos)
			self.srcItemPos = (0, 0)
			self.dstItemPos = (0, 0)
			self.srcItemWindow = None
			self.dstItemWindow = None

		def __PetItemCancel(self):
			self.srcItemPos = (0, 0)
			self.dstItemPos = (0, 0)
			self.PetItemQuestionDlg.Close()
			self.srcItemWindow = None
			self.dstItemWindow = None


	def __OpenQuestionDialog(self, srcItemPos, dstItemPos):
		if self.interface.IsShowDlgQuestionWindow():
			self.interface.CloseDlgQuestionWindow()

		getItemVNum = player.GetItemIndex(dstItemPos)

		self.srcItemPos = srcItemPos
		self.dstItemPos = dstItemPos

		self.dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.__Accept))
		self.dlgQuestion.SetCancelEvent(ui.__mem_func__(self.__Cancel))

		self.dlgQuestion.SetText1("%s" % item.GetItemNameByVnum(getItemVNum))
		self.dlgQuestion.SetText2(localeInfo.INVENTORY_REALLY_USE_ITEM)

		self.dlgQuestion.Open()

	def __Accept(self):
		self.dlgQuestion.Close()
		self.__SendUseItemToItemPacket(self.srcItemPos, self.dstItemPos)
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)

	def __Cancel(self):
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)
		self.dlgQuestion.Close()

	def __SellItem(self, itemSlotPos):
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber = itemSlotPos
			itemIndex = player.GetItemIndex(itemSlotPos)
			itemCount = player.GetItemCount(itemSlotPos)

			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)
			## ��Ƽ �÷��� �˻� ������ �߰�
			## 20140220
			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice / 5
			else:
				itemPrice = itemPrice * itemCount / 5

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()
			if app.ENABLE_SET_ITEM:
				itemName = item.GetItemNameBySetValue(player.GetItemSetValue(itemSlotPos))

			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	if app.ENABLE_REFINE_ELEMENT:
		def ElementItem(self, srcItemSlotPos, dstItemSlotPos):
			itemElement = player.GetItemIndex(srcItemSlotPos)

			if player.ELEMENT_UPGRADE_CANT_ADD == player.GetElements(itemElement, dstItemSlotPos) or \
					player.ELEMENT_DOWNGRADE_CANT_ADD == player.GetElements(itemElement, dstItemSlotPos) or \
					player.ELEMENT_CANT == player.GetElements(itemElement, dstItemSlotPos) or \
					player.ELEMENT_CHANGE_CANT_ADD == player.GetElements(itemElement, dstItemSlotPos):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_ELEMENT_CANT_DO_THIS)
				return

			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

	def RefineItem(self, scrollSlotPos, targetSlotPos):
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		if app.ENABLE_AUTO_REFINE:
			osfInfo.AUTO_REFINE_TYPE = 1
			osfInfo.AUTO_REFINE_DATA["ITEM"][0] = scrollSlotPos
			osfInfo.AUTO_REFINE_DATA["ITEM"][1] = targetSlotPos

		###########################################################
		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		#net.SendItemUseToItemPacket(scrollSlotPos, targetSlotPos)
		return
		###########################################################

		###########################################################
		#net.SendRequestRefineInfoPacket(targetSlotPos)
		#return
		###########################################################

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if scrollIndex == 71109:
			if player.CanDetach(scrollIndex, targetSlotPos):
				self.__OpenQuestionDialog(scrollSlotPos, targetSlotPos)
			return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if not player.CanDetach(scrollIndex, targetSlotPos):
				item.SelectItem(scrollIndex)
				if acce.IsCleanScroll(scrollIndex):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_FAILURE_CLEAN)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)

				return
		else:
			if not player.CanDetach(scrollIndex, targetSlotPos):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
				return

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			item.SelectItem(targetIndex)
			if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
				item.SelectItem(scrollIndex)
				if acce.IsCleanScroll(scrollIndex):
					self.questionDialog.SetText(localeInfo.ACCE_DO_YOU_CLEAN)

		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()
		if app.ENABLE_SET_ITEM:
			itemName = item.GetItemNameBySetValue(player.GetItemSetValue(metinSlotPos))

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH % (itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		overInvenSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(False)

		getItemVNum = player.GetItemIndex
		itemVnum = getItemVNum(overInvenSlotPos)
		if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
			self.DelHighlightSlot(overInvenSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedItemType:

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overInvenSlotPos):
					self.wndItem.SetUsableItem(True)
					self.ShowToolTip(overInvenSlotPos)
					return

		self.ShowToolTip(overInvenSlotPos)

	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		# �ٸ� �����ۿ� ����� �� �ִ� �������ΰ�?

		if item.IsRefineScroll(srcItemVNum):
			return True
		elif app.ENABLE_SOUL_BIND_SYSTEM and item.IsSealScroll(srcItemVNum):
			return True
		elif app.ENABLE_67TH_BONUS and (srcItemVNum == item.ADD_RARE_ATTRIBUTE_VNUM or srcItemVNum == item.CHANGE_RARE_ATTRIBUTE_VNUM):
			return True
		elif item.IsMetin(srcItemVNum):
			return True
		elif item.IsDetachScroll(srcItemVNum):
			return True
		elif item.IsKey(srcItemVNum):
			return True
		elif app.ENABLE_REFINE_ELEMENT and item.IsElement(srcItemVNum):
			return True
		elif item.IsItemUsedForDragonSoul(srcItemVNum):
			return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		else:
			if app.ENABLE_GROWTH_PET_SYSTEM:
				if self.__IsUsablePetItem(srcItemVNum):
					return True

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVNum):
					return True

			if app.ENABLE_SET_ITEM:
				if item.IsSetClearScroll(srcItemVNum):
					return True

			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return True

		return False

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		# ��� �����ۿ� ����� �� �ִ°�?
		if srcSlotPos == dstSlotPos:
			return False

		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return True
		elif app.ENABLE_SOUL_BIND_SYSTEM and item.IsSealScroll(srcItemVNum):
			if player.CanSealItem(srcItemVNum, player.INVENTORY, dstSlotPos):
				return True
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos):
				return True
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return True
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return True
		elif app.ENABLE_REFINE_ELEMENT and item.IsElement(srcItemVNum):
			if app.ENABLE_SOUL_BIND_SYSTEM and (
					player.GetItemSealDate(player.INVENTORY, dstSlotPos) != item.E_SEAL_DATE_DEFAULT_TIMESTAMP):
				return False
			if player.ELEMENT_UPGRADE_ADD == player.GetElements(srcItemVNum, dstSlotPos) or \
					player.ELEMENT_DOWNGRADE_ADD == player.GetElements(srcItemVNum, dstSlotPos) or \
					player.ELEMENT_CHANGE_ADD == player.GetElements(srcItemVNum, dstSlotPos):
				return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		elif app.ENABLE_67TH_BONUS and srcItemVNum == item.ADD_RARE_ATTRIBUTE_VNUM:
			if self.__CanAddRareItemAttr(dstSlotPos):
				return True
		elif app.ENABLE_67TH_BONUS and srcItemVNum == item.CHANGE_RARE_ATTRIBUTE_VNUM:
			if self.__CanChangeRareItemAttrList(dstSlotPos):
				return True
		else:
			if app.ENABLE_GROWTH_PET_SYSTEM:
				if self.__CanUseSrcPetItemToDstPetItem(srcItemVNum, player.INVENTORY, dstSlotPos):
				#if self.__CanUseSrcPetItemToDstPetItem(srcItemVNum, srcSlotWindow, srcSlotPos, dstSlotWindow,dstSlotPos):
					return True

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if player.CanChangeLookClearItem(srcItemVNum, player.INVENTORY, dstSlotPos):
					return True

			if app.ENABLE_SET_ITEM:
				if item.IsSetClearScroll(srcItemVNum):
					if player.CanSetItemClear(srcItemVNum, player.INVENTORY, dstSlotPos):
						return True

			useType = item.GetUseType(srcItemVNum)

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return True
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return True
			elif app.BL_67_ATTR and "USE_CHANGE_ATTRIBUTE2" == useType:
				if self.__CanChangeItemAttrList2(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr2(dstSlotPos):
					return True
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return True
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return True;

			elif "USE_PUT_INTO_AURA_SOCKET" == useType:
				if app.ENABLE_EXTEND_INVEN_SYSTEM:
					if item.ITEM_TYPE_COSTUME == player.GetItemTypeBySlot(dstSlotPos) and item.COSTUME_TYPE_AURA == player.GetItemSubTypeBySlot(dstSlotPos):
						if player.GetItemMetinSocket(dstSlotPos, player.ITEM_SOCKET_AURA_BOOST) == 0:
							return True
				else:
					dstItemVnum = player.GetItemIndex(dstSlotPos)
					item.SelectItem(dstItemVnum)
					if item.ITEM_TYPE_COSTUME == item.GetItemType() and item.COSTUME_TYPE_AURA == item.GetItemSubType():
						if player.GetItemMetinSocket(dstSlotPos, player.ITEM_SOCKET_AURA_BOOST) == 0:
							return True

			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)

				item.SelectItem(dstItemVNum)

				if item.ITEM_TYPE_BELT == item.GetItemType():
					return True
			elif "USE_CHANGE_COSTUME_ATTR" == useType:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return True
			elif "USE_RESET_COSTUME_ATTR" == useType:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return True
			elif "USE_TIME_CHARGE_PER" == useType:
				if self.__CanExtendTime(dstSlotPos):
					return True
			elif "USE_TIME_CHARGE_FIX" == useType:
				if self.__CanExtendTime(dstSlotPos):
					return True
			else:
				pass

		return False

	if app.ENABLE_67TH_BONUS:
		def __CanAddRareItemAttr(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
				return False

			attrCount = 0
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					attrCount += 1

			if attrCount < player.ATTRIBUTE_SLOT_MAX_NUM and attrCount >= (player.ATTRIBUTE_SLOT_MAX_NUM - 2):
				return True

			return False

		def __CanChangeRareItemAttrList(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
				return False

			attrCount = 0
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					attrCount += 1

			if attrCount < player.ATTRIBUTE_SLOT_MAX_NUM and attrCount >= (player.ATTRIBUTE_SLOT_MAX_NUM - 2):
				return True

			return False

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		# if item.ITEM_TYPE_WEAPON != item.GetItemType():
		if item.ITEM_TYPE_WEAPON != item.GetItemType() and item.ITEM_TYPE_ARMOR != item.GetItemType():
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return True

		return False

	if app.BL_67_ATTR:
		def __CanChangeItemAttrList2(self, dstSlotPos):
			return uiattr67Add.Attr67AddWindow.CantAttachToAttrSlot(dstSlotPos, False)

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				attrCount += 1

		if attrCount > 0:
			return True

		return False

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return False

		if curCount >= maxCount:
			return False

		return True

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		#if item.GetItemType() != item.ITEM_TYPE_ARMOR:
		if not item.GetItemType() in (item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_BELT):
			return False

		#if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
		if not item.GetItemType() == item.ITEM_TYPE_BELT:
			if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
				return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return False

		return True

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			# 2020.02.13.Owsap - Fix Attribute Count
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				attrCount += 1

		if attrCount < 4:
			return True

		return False

	# 2020.02.13.Owsap - Fix Attribute Count
	def __CanAddItemAttr2(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				attrCount += 1

		if attrCount == 4:
			return True

		return False

	def __CanChangeCostumeAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:
			return False

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
				return False

		if app.ENABLE_MOUNT_COSTUME_SYSTEM:
			if item.GetItemSubType() == item.COSTUME_TYPE_MOUNT:
				return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			type, value = player.GetItemAttribute(dstSlotPos, i)
			if type != 0:
				return True

		return False

	def __CanResetCostumeAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_COSTUME:
			return False

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
				return False

		if app.ENABLE_MOUNT_COSTUME_SYSTEM:
			if item.GetItemSubType() == item.COSTUME_TYPE_MOUNT:
				return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			type, value = player.GetItemAttribute(dstSlotPos, i)
			if type != 0:
				return True

		return False

	def __CanExtendTime(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if app.ENABLE_COSTUME_EXTENDED_RECHARGE:
			if item.GetItemType() != item.ITEM_TYPE_COSTUME and item.GetItemType() != item.ITEM_TYPE_DS:
				return False

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
					return False

			metinSlot = [player.GetItemMetinSocket(dstSlotPos, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
			leftSec = max(0, metinSlot[0] - app.GetGlobalTimeStamp())

			if leftSec >= item.MIN_INFINITE_DURATION:
				return False
		else:
			if item.GetItemType() != item.ITEM_TYPE_DS:
				return False

		return True

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)
			if app.__ENABLE_NEW_OFFLINESHOP__:
				if uiofflineshop.IsBuildingShop():
					self.__AddTooltipSaleMode(slotIndex)

	if app.__ENABLE_NEW_OFFLINESHOP__:
		def __AddTooltipSaleMode(self, slot):
			if player.IsEquipmentSlot(slot):
				return

			itemIndex = player.GetItemIndex(slot)
			if itemIndex !=0:
				item.SelectItem(itemIndex)
				if item.IsAntiFlag(item.ANTIFLAG_MYSHOP) or item.IsAntiFlag(item.ANTIFLAG_GIVE):
					return

				self.tooltipItem.AddRightClickForSale()

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()

		if self.wndBelt:
			if self.IsClickBeltInventory == False:
				self.wndBelt.SetTop()
			else:
				self.IsClickBeltInventory = False

		if self.wndCostume:
			self.wndCostume.SetTop()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def UseItemSlot(self, slotIndex):

		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		## 2021.02.01.Owsap - Prevent unequipping an item while acting emotion.
		if player.IsActingEmotion():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		if app.ENABLE_SKILLBOOK_COMB_SYSTEM:
			if self.interface and self.interface.wndSkillBookCombination and self.interface.wndSkillBookCombination.IsShow():
				self.interface.wndSkillBookCombination.AddItemFromInventory(slotIndex)
				return

		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop():
				if not item.IsAntiFlag(item.ANTIFLAG_GIVE) and not item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
					offlineshop.ShopBuilding_AddItem(player.INVENTORY, slotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		if app.QUICK_SELL_SYSTEM:
			if app.IsPressed(app.DIK_LALT):
				if slotIndex in constInfo.QUICK_SELL_ITEMS:
					self.interface.RemoveSellSlot(slotIndex)
					self.RefreshBagSlotWindow()
					return
				else:
					self.interface.AppendSellSlot(slotIndex)
					self.RefreshBagSlotWindow()
					return

		if self.wndDragonSoulRefine.IsShow():
			self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), 1)
			return

		if app.ITEM_CHECKINOUT_UPDATE:
			if self.wndSafeBox.IsShow() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendSafeboxCheckinPacket(slotIndex)
				return
			if exchange.isTrading() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendExchangeItemAddPacket(player.INVENTORY, slotIndex, -1)
				return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if self.isShowAcceWindow():
				if app.ENABLE_SOUL_BIND_SYSTEM:
					if player.GetItemSealDate(player.INVENTORY, slotIndex) == -1 or player.GetItemSealDate(player.INVENTORY, slotIndex) > 0:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_SEALITEM)
						return

				acce.Add(player.INVENTORY, slotIndex, 255)
				return

		self.__UseItem(slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)

		if app.ENABLE_AURA_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				self.__UseItemAura(slotIndex)
				return
		if app.__ENABLE_NEW_OFFLINESHOP__:
			if uiofflineshop.IsBuildingShop() and uiofflineshop.IsSaleSlot(player.INVENTORY, slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINESHOP_CANT_SELECT_ITEM_DURING_BUILING)
				return

		if app.ENABLE_GROWTH_PET_SYSTEM and app.ENABLE_GROWTH_PET_SKILL_DEL:
			## When using other items while the training pet skill deletion window is open, the window is closed.
			if self.PetSkillDelDlg and self.PetSkillDelDlg.IsShow():
				self.__PetSkillDeleteQuestionDialogCancel()

		if app.__BL_CHEST_DROP_INFO__:
			if app.IsPressed(app.DIK_LCONTROL):
				isMain = not app.IsPressed(app.DIK_LSHIFT)
				if item.HasDropInfo(ItemVNum, isMain) and self.interface:
					self.interface.OpenChestDropWindow(ItemVNum, isMain)
				return

			if app.IsPressed(app.DIK_LCONTROL):
				isMain = not app.IsPressed(app.DIK_LCONTROL)
				if item.HasDropInfo(ItemVNum, isMain) and self.interface:
					self.interface.OpenChestDropWindow(ItemVNum, isMain)
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				return

		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface.AttachInvenItemToOtherWindowSlot(slotIndex):
				return

		if app.ENABLE_MOUNT_COSTUME_SYSTEM:
			if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_MOUNT:
				self.__SendUseItemPacket(slotIndex)
				return

		if app.ENABLE_GROWTH_PET_SYSTEM:
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if item.ITEM_TYPE_PET == itemType and item.PET_PAY != itemSubType:
				self.__UseItemPet(slotIndex)
				return

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog()
			if app.ENABLE_12ZI:
				self.questionDialog.SetText(self.GetConfirmQuestion(ItemVNum))
				(width, height) = self.questionDialog.GetTextSize()
				self.questionDialog.SetWidth(width + 16)
			else:
				self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM2)
			# self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM2)

			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		else:
			self.__SendUseItemPacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	if app.ENABLE_12ZI:
		def GetConfirmQuestion(self, vnum):
			item.SelectItem(vnum)
			if vnum == 72327 or vnum == 72329:
				return localeInfo.CHARGE_BEAD_QUESTION % (item.GetValue(0))
			elif vnum == 72328:
				return localeInfo.UNLIMIT_ENTER_CZ_QUESTION % (item.GetValue(0))
			else:
				return localeInfo.INVENTORY_REALLY_USE_ITEM2

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		# ���λ��� ���� �ִ� ���� ������ ��� ����
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if self.interface.IsShowDlgQuestionWindow():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DONT_USE_ITEM_WHEN_SHOW_CONFIRM)
			return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	if app.ENABLE_GROWTH_PET_SYSTEM:
		##Pet UseItemPet
		def __UseItemPet(self, slotIndex):
			itemSubType = item.GetItemSubType()
			if item.PET_EGG == itemSubType:
				self.petHatchingWindow.HatchingWindowOpen(player.INVENTORY, slotIndex)

			elif item.PET_UPBRINGING == itemSubType:
				if player.CanUsePetCoolTimeCheck():
					if self.__CanUseGrowthPet(slotIndex):
						self.__SendUseItemPacket(slotIndex)

			elif item.PET_BAG == itemSubType:
				if self.__CanUsePetBagItem(slotIndex):
					if self.questionDialog:
						self.questionDialog.Close()

					self.questionDialog = uiCommon.QuestionDialog()
					self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_PET_BAG_TAKE_OUT)
					self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
					self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
					self.questionDialog.slotIndex = slotIndex
					self.questionDialog.Open()

			if app.ENABLE_GROWTH_PET_SKILL_DEL:
				if item.PET_SKILL_ALL_DEL_BOOK == itemSubType:
					## Pet must be active.
					pet_id = player.GetActivePetItemId()
					if 0 == pet_id:
						return
					(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2,
					 pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)
					## Pets must have learned skills.
					if 0 == pet_skill1 and 0 == pet_skill2 and 0 == pet_skill3:
						# Pop up window
						popup = uiCommon.PopupDialog()
						popup.SetText(localeInfo.PET_EMPTY_SKILL_SLOT_USE_ITEM)
						popup.SetAcceptEvent(self.__OnClosePopupDialog)
						popup.Open()
						self.pop = popup
						return

					##
					self.PetSkillAllDelBookIndex = slotIndex
					self.SetCantMouseEventSlot(self.PetSkillAllDelBookIndex)

					self.PetSkillDelDlg.SetAcceptEvent(ui.__mem_func__(self.__PetSkillDeleteQuestionDialogAccept))
					self.PetSkillDelDlg.SetCancelEvent(ui.__mem_func__(self.__PetSkillDeleteQuestionDialogCancel))

					self.PetSkillDelDlg.SetText1(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG1)
					self.PetSkillDelDlg.SetText2(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG2)
					(w, h) = self.PetSkillDelDlg.GetTextSize1()
					self.PetSkillDelDlg.SetWidth(w + 100)
					self.PetSkillDelDlg.Open()

	if app.ENABLE_GROWTH_PET_SYSTEM and app.ENABLE_GROWTH_PET_SKILL_DEL:
		def __PetSkillDeleteQuestionDialogAccept(self):
			net.SendPetDeleteAllSkill(self.PetSkillAllDelBookIndex)
			self.__PetSkillDeleteQuestionDialogCancel()

		def __PetSkillDeleteQuestionDialogCancel(self):
			self.PetSkillDelDlg.Close()
			self.SetCanMouseEventSlot(self.PetSkillAllDelBookIndex)
			self.PetSkillAllDelBookIndex = -1

		def SetCanMouseEventSlot(self, inven_slot_index):
			if inven_slot_index >= player.INVENTORY_PAGE_SIZE:
				page = self.GetInventoryPageIndex()  # range 0,1,2,3
				inven_slot_index -= (page * player.INVENTORY_PAGE_SIZE)
			self.wndItem.SetCanMouseEventSlot(inven_slot_index)

		def SetCantMouseEventSlot(self, inven_slot_index):
			if inven_slot_index >= player.INVENTORY_PAGE_SIZE:
				page = self.GetInventoryPageIndex()  # range 0,1,2,3
				inven_slot_index -= (page * player.INVENTORY_PAGE_SIZE)
			self.wndItem.SetCantMouseEventSlot(inven_slot_index)

	if app.ENABLE_AURA_SYSTEM:
		def __UseItemAuraQuestionDialog_OnAccept(self):
			self.questionDialog.Close()
			net.SendAuraRefineCheckIn(
				*(self.questionDialog.srcItem + self.questionDialog.dstItem + (player.GetAuraRefineWindowType(),)))
			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAuraQuestionDialog_Close(self):
			self.questionDialog.Close()

			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAura(self, slotIndex):
			AuraSlot = player.FineMoveAuraItemSlot()
			UsingAuraSlot = player.FindActivatedAuraSlot(player.INVENTORY, slotIndex)
			AuraVnum = player.GetItemIndex(slotIndex)
			item.SelectItem(AuraVnum)
			if player.GetAuraCurrentItemSlotCount() >= player.AURA_SLOT_MAX <= UsingAuraSlot:
				return

			if player.IsEquipmentSlot(slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EQUIPITEM)
				return

			# if app.ENABLE_SEALBIND_SYSTEM and player.IsSealedItemBySlot(player.INVENTORY, slotIndex):
			# 	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_SEALITEM)
			# 	return

			if player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
				isAbsorbItem = False
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
						if player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
							if UsingAuraSlot == player.AURA_SLOT_MAX:
								if AuraSlot != player.AURA_SLOT_MAIN:
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot,
														  player.GetAuraRefineWindowType())

						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return

					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return

				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					if item.GetItemSubType() in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						isAbsorbItem = True
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return

				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
					return

				if isAbsorbItem:
					if UsingAuraSlot == player.AURA_SLOT_MAX:
						if AuraSlot != player.AURA_SLOT_SUB:
							if player.FindUsingAuraSlot(player.AURA_SLOT_SUB) == player.NPOS():
								AuraSlot = player.AURA_SLOT_SUB
							else:
								return

						self.questionDialog = uiCommon.QuestionDialog()
						self.questionDialog.SetText(localeInfo.AURA_NOTICE_DEL_ABSORDITEM)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_Close))
						self.questionDialog.srcItem = (player.INVENTORY, slotIndex)
						self.questionDialog.dstItem = (player.AURA_REFINE, AuraSlot)
						self.questionDialog.Open()

			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex,
																			 player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curExp >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot,
														  player.GetAuraRefineWindowType())

							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return

						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if item.GetItemType() == item.ITEM_TYPE_RESOURCE:
								if item.GetItemSubType() == item.RESOURCE_AURA:
									if UsingAuraSlot == player.AURA_SLOT_MAX:
										if AuraSlot != player.AURA_SLOT_SUB:
											return

										net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE,
																  AuraSlot, player.GetAuraRefineWindowType())
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
									return

							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
								return

						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex,
																			 player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curLevel != player.GetAuraRefineInfo(curLevel,
																		player.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(
										curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return

								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot,
														  player.GetAuraRefineWindowType())

							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return

						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						socketLevelValue = player.GetItemMetinSocket(*(Cell + (player.ITEM_SOCKET_AURA_CURRENT_LEVEL,)))
						curLevel = (socketLevelValue / 100000) - 1000
						curExp = socketLevelValue % 100000;
						if curLevel >= player.AURA_MAX_LEVEL:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
							return

						if curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if AuraVnum != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if player.GetItemCount(slotIndex) < player.GetAuraRefineInfo(curLevel,
																					 player.AURA_REFINE_INFO_MATERIAL_COUNT):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
							return

						if UsingAuraSlot == player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAX:
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot,
													  player.GetAuraRefineWindowType())

	def __SendUseItemPacket(self, slotPos):
		# ���λ��� ���� �ִ� ���� ������ ��� ����
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if self.interface.IsShowDlgQuestionWindow():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DONT_USE_ITEM_WHEN_SHOW_CONFIRM)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		# ���λ��� ���� �ִ� ���� ������ ��� ����
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if self.interface.IsShowDlgQuestionWindow():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DONT_USE_ITEM_WHEN_SHOW_CONFIRM)
			return

		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	if app.ITEM_CHECKINOUT_UPDATE:
		def SetSafeboxWindow(self, wndSafeBox):
			self.wndSafeBox = wndSafeBox

	def SetDragonSoulRefineWindow(self, DragonSoulRefine):
		from _weakref import proxy
		self.wndDragonSoulRefine = proxy(DragonSoulRefine)

	def OnMoveWindow(self, x, y):
		#if not app.ENABLE_BELT_RENEWAL:
		if self.wndBelt:
			# print "Belt Global Pos : ", self.wndBelt.GetGlobalPosition()
			if localeInfo.IsARABIC():
				self.wndBelt.AdjustPositionAndSizeAE()
			else:
				self.wndBelt.AdjustPositionAndSize()

	# �Ǽ����� ���� �׵θ� �߰�, ����.
	## HilightSlot Change
	def DeactivateSlot(self, slotindex, type):
		if type == wndMgr.HILIGHTSLOT_MAX:
			return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_ACCE:
				self.__DelHighlightSlotAcce(slotindex)

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_CHANGE_LOOK:
				self.__DelHighlightSlotChangeLook(slotindex)

		if app.ENABLE_CUBE_RENEWAL:
			if type == wndMgr.HILIGHTSLOT_CUBE:
				self.__DelHighlightSlotCube(slotindex)

	def UpdateSellPrice(self):
		self.interface.UpdateMobileSellPrice()


	## HilightSlot Change
	def ActivateSlot(self, slotindex, type):
		if type == wndMgr.HILIGHTSLOT_MAX:
			return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_ACCE:
				self.__AddHighlightSlotAcce(slotindex)

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if type == wndMgr.HILIGHTSLOT_CHANGE_LOOK:
				self.__AddHighlightSlotChangeLook(slotindex)

		if app.ENABLE_CUBE_RENEWAL:
			if type == wndMgr.HILIGHTSLOT_CUBE:
				self.__AddHighlightSlotCube(slotindex)

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		## �Ǽ����� �� ���̶���Ʈ list �߰�.
		def __AddHighlightSlotAcce(self, slotIndex):
			if not slotIndex in self.listHighlightedAcceSlot:
				self.listHighlightedAcceSlot.append(slotIndex)

		## �Ǽ����� �� ���̶���Ʈ list ����.
		def __DelHighlightSlotAcce(self, slotIndex):
			if slotIndex in self.listHighlightedAcceSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
					self.wndItem.SetCanMouseEventSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
					self.wndItem.SetCanMouseEventSlot(slotIndex)

				self.listHighlightedAcceSlot.remove(slotIndex)

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def __AddHighlightSlotChangeLook(self, slotIndex):
			if not slotIndex in self.listHighlightedChangeLookSlot:
				self.listHighlightedChangeLookSlot.append(slotIndex)

		def __DelHighlightSlotChangeLook(self, slotIndex):
			if slotIndex in self.listHighlightedChangeLookSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
					self.wndItem.SetCanMouseEventSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
					self.wndItem.SetCanMouseEventSlot(slotIndex)
				self.listHighlightedChangeLookSlot.remove(slotIndex)

	if app.ENABLE_CUBE_RENEWAL:
		def __AddHighlightSlotCube(self, slotIndex):
			if not slotIndex in self.listHighlightedCubeSlot:
				self.listHighlightedCubeSlot.append(slotIndex)

		def __DelHighlightSlotCube(self, slotIndex):
			if slotIndex in self.listHighlightedCubeSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)

				self.listHighlightedCubeSlot.remove(slotIndex)

	## ���̶���Ʈ ��������.
	def __HighlightSlot_Refresh(self):
		## �Ǽ�����.
		for i in xrange(self.wndItem.GetSlotCount()):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if slotNumber in self.listHighlightedAcceSlot:
					self.wndItem.ActivateSlot(i)
					## �Ǽ����� ���� �׵θ� �� ������� �����Ѵ�.
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_GREEN)
					self.wndItem.SetCantMouseEventSlot(i)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if slotNumber in self.listHighlightedChangeLookSlot:
					self.wndItem.ActivateSlot(i)
					self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_RED)
					self.wndItem.SetCantMouseEventSlot(i)

			# if app.ENABLE_CUBE_RENEWAL:
			# 	if slotNumber in self.listHighlightedCubeSlot:
			# 		if not player.GetItemSetValue(player.INVENTORY, slotNumber):
			# 			self.wndItem.ActivateSlot(i)
			# 			self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_GREEN)
			# 		else:
			# 			self.wndItem.DeactivateSlot(i)
			# 			self.listHighlightedCubeSlot.remove(i)
			#
			# 			self.HighlightSlot(i)

			# if app.ENABLE_CUBE_RENEWAL:
			# 	if slotNumber in self.listHighlightedCubeSlot:
			# 		#if not player.GetItemSetValue(player.INVENTORY, slotNumber):
			# 			self.wndItem.ActivateSlot(i)
			# 			self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_GREEN)
			# 		#else:
			# 		#	self.wndItem.DeactivateSlot(i)
			# 		#	self.listHighlightedCubeSlot.remove(slotNumber)
			#
			# 		#	self.HighlightSlot(slotNumber)

			if app.ENABLE_CUBE_RENEWAL:
				if slotNumber in self.listHighlightedCubeSlot:
					if not player.GetItemSetValue(player.INVENTORY, slotNumber):
						self.wndItem.ActivateSlot(i)
						self.wndItem.SetSlotDiffuseColor(i, wndMgr.COLOR_TYPE_GREEN)
					else:
						self.wndItem.DeactivateSlot(i)
						self.listHighlightedCubeSlot.remove(slotNumber)

						self.HighlightSlot(slotNumber)

			## HilightSlot Change
			if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.ActivateSlot(i)

	def RefreshHighlights(self):
		self.__HighlightSlot_Refresh()

	def HighlightSlot_Clear(self):
		self.__HighlightSlot_Clear()

	## �Ǽ����� �� ���̶���Ʈ list Ŭ����.
	def __HighlightSlot_Clear(self):
		## �Ǽ�����
		for i in xrange(self.wndItem.GetSlotCount()):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if slotNumber in self.listHighlightedAcceSlot:
					self.wndItem.DeactivateSlot(i)
					self.wndItem.SetCanMouseEventSlot(i)
					self.listHighlightedAcceSlot.remove(slotNumber)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if slotNumber in self.listHighlightedChangeLookSlot:
					self.wndItem.DeactivateSlot(i)
					self.wndItem.SetCanMouseEventSlot(i)
					self.listHighlightedChangeLookSlot.remove(slotNumber)

			# if app.ENABLE_CUBE_RENEWAL:
			# 	if slotNumber in self.listHighlightedCubeSlot:
			# 		self.wndItem.DeactivateSlot(i)
			# 		self.listHighlightedCubeSlot.remove(slotNumber)

			if app.ENABLE_CUBE_RENEWAL:
				if slotNumber in self.listHighlightedCubeSlot:
					self.wndItem.DeactivateSlot(i)
					self.listHighlightedCubeSlot.remove(slotNumber)

			## HilightSlot Change
			if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.DeactivateSlot(i)
					self.listHighlightedSlot.remove(slotNumber)

		# if app.ENABLE_CUBE_RENEWAL:
		# 	self.listHighlightedCubeSlot = []

	if app.WJ_ENABLE_PICKUP_ITEM_EFFECT:
		# ���� highlight ����
		## �߰�
		def HighlightSlot(self, slot):
			#slot���� ���� ����ó��.
			if slot > player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT:
				return

			if not slot in self.listHighlightedSlot:
				self.listHighlightedSlot.append(slot)

		## ����
		def DelHighlightSlot(self, inventorylocalslot):
			if inventorylocalslot in self.listHighlightedSlot:
				if inventorylocalslot >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(inventorylocalslot - (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(inventorylocalslot)

				self.listHighlightedSlot.remove(inventorylocalslot)
		# ���� highlight ���� ��

	def DelHighLightCube(self):
		if app.ENABLE_CUBE_RENEWAL:
			slotsToRemove = list(self.listHighlightedCubeSlot)
			for i in xrange(player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if slotNumber in slotsToRemove:
					self.wndItem.DeactivateSlot(slotNumber)
					if slotNumber in self.listHighlightedCubeSlot:
						self.listHighlightedCubeSlot.remove(slotNumber)
					player.RefreshInventory()

	def IsDlgQuestionShow(self):
		if self.dlgQuestion.IsShow():
			return True
		else:
			return False

	def ExternQuestionDialog_Close(self):
		if self.attachMetinDialog.IsShow():
			self.attachMetinDialog.Close()
		self.dlgQuestion.Close()
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)

	def CancelDlgQuestion(self):
		self.__Cancel()

	def SetUseItemMode(self, bUse):
		self.wndItem.SetUseMode(bUse)

	if app.ENABLE_GROWTH_PET_SYSTEM:
		## Highlight the active pet
		def __ActivePetHighlightSlot(self, slotNumber):
			active_id = player.GetActivePetItemId()

			if active_id == 0:
				return

			if active_id == player.GetItemMetinSocket(player.INVENTORY, slotNumber, 2):  ## 0 ~ 89

				if slotNumber >= player.INVENTORY_PAGE_SIZE:
					slotNumber -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				self.wndItem.ActivateSlot(slotNumber)  ## 0 ~ 44
				self.wndItem.SetSlotDiffuseColor(slotNumber, wndMgr.COLOR_TYPE_GRAY)
				#self.wndItem.SetCantMouseEventSlot(slotNumber)

		## Cool time display according to remaining time (slotNumber is 0 ~ 89)
		def __SetCollTimePetItemSlot(self, slotNumber, itemVnum):
			item.SelectItem(itemVnum)
			itemSubType = item.GetItemSubType()

			if itemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
				return

			if itemSubType == item.PET_BAG:
				id = player.GetItemMetinSocket(player.INVENTORY, slotNumber, 2)
				if id == 0:
					return

			(limitType, limitValue) = item.GetLimit(0)

			# The limit value of the nurturing pet is in socket 1.
			if itemSubType == item.PET_UPBRINGING:
				limitValue = player.GetItemMetinSocket(player.INVENTORY, slotNumber, 1)

			if limitType in [item.LIMIT_REAL_TIME, item.LIMIT_REAL_TIME_START_FIRST_USE]:
				sock_time = player.GetItemMetinSocket(player.INVENTORY, slotNumber, 0)

				remain_time = max(0, sock_time - app.GetGlobalTimeStamp())

				if slotNumber >= player.INVENTORY_PAGE_SIZE:
					slotNumber -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				## SetSlotCoolTimeInverse slotNumber is 0 ~ 44
				self.wndItem.SetSlotCoolTimeInverse(slotNumber, limitValue, limitValue - remain_time)

		# print "item Limit TYPE: ", limitType
		# print "item Limit VALUE: ", limitValue
		# print "Time remaining : ", remain_time
		# print "Elapsed time : ", limitValue - remain_time

	def GetInventoryPageIndex(self):
		## 0 or 1
		return self.inventoryPageIndex

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def __IsPetItem(self, srcItemVID):
			item.SelectItem(srcItemVID)

			if item.GetItemType() == item.ITEM_TYPE_PET:
				return True

			return False

		#def __SendUsePetItemToItemPacket(self, srcItemVID, srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos):
		def __SendUsePetItemToItemPacket(self, srcItemVID, srcItemSlotPos,dstItemSlotPos):
			if self.__CanUseSrcPetItemToDstPetItem(srcItemVID, srcItemSlotPos, dstItemSlotPos):
				#srcItemVnum = player.GetItemIndex(srcItemSlotWindow, srcItemSlotPos)
				srcItemVnum = player.GetItemIndex(srcItemSlotPos)
				item.SelectItem(srcItemVnum)
				srcItemType = item.GetItemType()
				srcItemSubType = item.GetItemSubType()

				if item.ITEM_TYPE_PET == srcItemType:
					if srcItemSubType in [item.PET_FEEDSTUFF, item.PET_BAG]:
					#if item.PET_BAG == srcItemSubType:
						# self.__OpenPetItemQuestionDialog(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						self.__OpenPetItemQuestionDialog(srcItemSlotPos, dstItemSlotPos)
					elif item.PET_NAME_CHANGE == srcItemSubType:
						# self.__UseItemPetNameChange(srcItemSlotWindow, srcItemSlotPos, dstItemSlotWindow, dstItemSlotPos)
						self.__UseItemPetNameChange(srcItemSlotPos, dstItemSlotPos)
				return True

			return False

		#def __UseItemPetNameChange(self, srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos):
		def __UseItemPetNameChange(self, srcSlotPos, dstSlotPos):
			if self.petNameChangeWindow:
				# self.petNameChangeWindow.NameChangeWindowOpen(srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos)
				self.petNameChangeWindow.NameChangeWindowOpen(srcSlotPos, dstSlotPos)

		def __IsUsablePetItem(self, srcItemVNum):
			item.SelectItem(srcItemVNum)
			srcItemType = item.GetItemType()
			srcItemSubType = item.GetItemSubType()

			if srcItemType != item.ITEM_TYPE_PET:
				return False

			if srcItemSubType not in [item.PET_FEEDSTUFF, item.PET_BAG, item.PET_NAME_CHANGE]:
				return False

			return True

		#if self.__CanUseSrcPetItemToDstPetItem(srcItemVNum, player.INVENTORY, dstSlotPos):
		#def __CanUseSrcPetItemToDstPetItem(self, srcItemVNum, srcSlotWindow, srcSlotPos, dstSlotWindow, dstSlotPos):
		#def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		def __CanUseSrcPetItemToDstPetItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
			item.SelectItem(srcItemVNum)
			srcItemType = item.GetItemType()
			srcItemSubType = item.GetItemSubType()

			if srcItemType != item.ITEM_TYPE_PET:
				return False

			if srcItemSubType == item.PET_FEEDSTUFF:
				#detIndex = player.GetItemIndex(dstSlotWindow, dstSlotPos)
				detIndex = player.GetItemIndex(dstSlotPos)
				item.SelectItem(detIndex)

				dstItemType = item.GetItemType()
				dstItemSubType = item.GetItemSubType()

				if dstItemType != item.ITEM_TYPE_PET:
					return False

				if dstItemSubType not in [item.PET_UPBRINGING]:
					return False

				if dstItemSubType == item.PET_BAG:
					#incaseTime = player.GetItemMetinSocket(dstSlotWindow, dstSlotPos, 1)
					incaseTime = player.GetItemMetinSocket(dstSlotPos, 1)
					if incaseTime == 0:
						return False

			elif srcItemSubType == item.PET_BAG:
				detIndex = player.GetItemIndex(dstSlotPos)
				item.SelectItem(detIndex)

				dstItemType = item.GetItemType()
				dstItemSubType = item.GetItemSubType()

				if dstItemType != item.ITEM_TYPE_PET:
					return False

				if dstItemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
					return False

				lifeTime = player.GetItemMetinSocket(dstSlotPos, 0)


				if dstItemSubType == item.PET_UPBRINGING:
					if lifeTime < app.GetGlobalTimeStamp():
						return False

					srcIncase = player.GetItemMetinSocket(srcSlotPos, 1)

					if srcIncase != 0:
						return False

				if dstItemSubType == item.PET_BAG:
					## When using a bag item in a bag, the period of the dest bag must pass.
					if lifeTime > app.GetGlobalTimeStamp():
						return False

					## The bag you intend to use must be empty.
					srcIncase = player.GetItemMetinSocket(srcSlotPos, 1)
					if srcIncase != 0:
						return False

					## The subject bag must not be empty.
					destIncase = player.GetItemMetinSocket(dstSlotPos, 1)
					if destIncase == 0:
						return False

			elif srcItemSubType == item.PET_NAME_CHANGE:
				detIndex = player.GetItemIndex(dstSlotPos)
				item.SelectItem(detIndex)

				dstItemType = item.GetItemType()
				dstItemSubType = item.GetItemSubType()

				if dstItemType != item.ITEM_TYPE_PET:
					return False

				if dstItemSubType not in [item.PET_UPBRINGING]:
					return False

			else:
				return False

			return True

		def __CanUseGrowthPet(self, slotIndex):
			if not player.GetItemMetinSocket(player.INVENTORY, slotIndex, 2):
				return False

			(limitType, limitValue) = item.GetLimit(0)
			remain_time = 999
			if item.LIMIT_REAL_TIME == limitType:
				sock_time = player.GetItemMetinSocket(player.INVENTORY, slotIndex, 0)
				if app.GetGlobalTimeStamp() > sock_time:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_SUMMON_BECAUSE_LIFE_TIME_END)
					return False

			return True

		def __CanUsePetBagItem(self, slotIndex):
			if not player.GetItemMetinSocket(player.INVENTORY, slotIndex, 2):
				return False

			(limitType, limitValue) = item.GetLimit(0)
			remain_time = 999
			if item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
				sock_time = player.GetItemMetinSocket(player.INVENTORY, slotIndex, 0)
				use_cnt = player.GetItemMetinSocket(player.INVENTORY, slotIndex, 1)

				if use_cnt:
					if app.GetGlobalTimeStamp() > sock_time:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_USE_BAG)
						return False;

			return True

		def __CanAttachGrowthPetItem(self, itemVNum, itemSlotIndex):
			activePetId = player.GetActivePetItemId()
			if activePetId == 0:
				return True

			item.SelectItem(itemVNum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()

			if item.ITEM_TYPE_PET == itemType and itemSubType == item.PET_UPBRINGING:
				petId = player.GetItemMetinSocket(itemSlotIndex, 2)
				if petId == activePetId:
					return False

			return True

		def SetPetHatchingWindow(self, window):
			self.petHatchingWindow = window

		def SetPetNameChangeWindow(self, window):
			self.petNameChangeWindow = window

		def SetPetFeedWindow(self, window):
			self.petFeedWindow = window

		def ItemMoveFeedWindow(self, slotWindow, slotIndex):
			if not self.petFeedWindow:
				return

			self.petFeedWindow.ItemMoveFeedWindow(slotWindow, slotIndex)

		if app.ENABLE_PET_ATTR_DETERMINE:
			def SetPetAttrChangeWindow(self, window):
				self.petAttrChangeWindow = window

			def ItemMovePetAttrChangeWindow(self, slotWindow, slotIndex):
				self.petAttrChangeWindow.AttachToPetAttrChangeSlot(slotWindow, slotIndex)

	if app.ENABLE_CHEQUE_SYSTEM:
		def OverInToolTip(self, arg):
			arglen = len(str(arg))
			pos_x, pos_y = wndMgr.GetMousePosition()

			self.toolTip.ClearToolTip()
			self.toolTip.SetThinBoardSize(11 * arglen)
			self.toolTip.SetToolTipPosition(pos_x + 5, pos_y - 5)
			self.toolTip.AppendTextLine(arg, 0xffffff00)
			self.toolTip.Show()

		def OverOutToolTip(self):
			self.toolTip.Hide()

		def EventProgress(self, event_type, idx):
			if "mouse_over_in" == str(event_type):
				if idx == 0:
					self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_YANG)
				elif idx == 1:
					self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_WON)
				elif app.ENABLE_GEM_SYSTEM and idx == 2 :
					self.OverInToolTip(localeInfo.GEM_SYSTEM_NAME)
				else:
					return
			elif "mouse_over_out" == str(event_type):
				self.OverOutToolTip()
			else:
				return

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def SetAcceWindow(self, wndAcceCombine, wndAcceAbsorption):
			self.wndAcceCombine = wndAcceCombine
			self.wndAcceAbsorption = wndAcceAbsorption

		def isShowAcceWindow(self):
			if self.wndAcceCombine:
				if self.wndAcceCombine.IsShow():
					return 1

			if self.wndAcceAbsorption:
				if self.wndAcceAbsorption.IsShow():
					return 1

			return 0

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			if self.wndCostume:
				self.wndCostume.RefreshVisibleCostume()
			else:
				self.wndCostume = CostumeWindow(self)
				self.wndCostume.RefreshVisibleCostume()
