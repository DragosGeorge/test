import dbg
import ui
import item
import localeInfo
import uiToolTip
import player
import grp
import cuberenewal
import app
import mouseModule
import chat
import constInfo
import wndMgr
import osfInfo
def GetMaxMultipler(infoDict):
	iMaxMultipler = 0

	while True:
		for i in xrange(1, 6):
			key = 'item%d' % i
			cubeKey = "count_elements_%d" % i 
			if key in infoDict and infoDict:
				if player.GetItemCountByVnum(infoDict[key][0], True, True) < infoDict[key][1] * (iMaxMultipler + 1):
					return iMaxMultipler
			else:
				continue
			iMaxMultipler += 1

	return iMaxMultipler

def GetItemGrade(itemVnum, infoDict):
	if infoDict['set_value'] <= 0:
		return ''

	item.SelectItem(itemVnum)
	itemName = item.GetItemName().strip()
	itemNameP = item.GetItemName().rfind('+')
	if itemNameP > 0 and len(itemName) > itemNameP + 1:
		level=itemName[itemNameP+1:]
		if level.isdigit():
			return '+%d' % int(level)

	return ''

def GetTotalRefinesNum(infoDict):
	requiredCount = []
	hasCount = []
	for i in xrange(1, 6):
		key = 'item%d' % i
		if key in infoDict:
			requiredCount.append(infoDict[key][1])
			hasCount.append(player.GetItemCountByVnum(infoDict[key][0], True, True))

	itemCount = []
	i = 0
	for count in requiredCount:
		itemCount.append(hasCount[i] / count)
		i += 1

	posChange = []
	i = 0
	for val in itemCount:
		if val >= min(itemCount):
			posChange.append(i)
		i += 1

	for pos in posChange:
		itemCount[pos] = min(itemCount)
	totalUpgrades = itemCount[0] if all(cnt == itemCount[0] for cnt in itemCount) else 0

	color1 = "|cff89B88D%d|h|r" % totalUpgrades#green
	color2 = "|cffFF0000%d|h|r" % totalUpgrades#red

	color = color1
	if totalUpgrades <= 0:
		color = color2

	return totalUpgrades

def GetTotalRefines(infoDict):
	requiredCount = []
	hasCount = []
	for i in range(1, 6):  # xrange a fost înlocuit cu range (pentru Python 3)
		key = 'item%d' % i
		if key in infoDict:
			requiredCount.append(infoDict[key][1])
			hasCount.append(player.GetItemCountByVnum(infoDict[key][0], True, True))

	itemCount = []
	for count, has in zip(requiredCount, hasCount):
		itemCount.append(has // count)  # "//" pentru divizare întreagă

	minCount = min(itemCount)  # Pre-calculăm valoarea minimă o singură dată

	posChange = [i for i, val in enumerate(itemCount) if val >= minCount]

	# Înlocuiește toate pozițiile relevante cu valoarea minimă
	for pos in posChange:
		itemCount[pos] = minCount

	# Verificăm dacă toate elementele sunt egale
	totalUpgrades = itemCount[0] if all(cnt == itemCount[0] for cnt in itemCount) else 0

	color1 = "|cff89B88D[%d]|h|r" % totalUpgrades  # green
	color2 = "|cffFF0000[%d]|h|r" % totalUpgrades  # red

	color = color1 if totalUpgrades > 0 else color2

	return '%s' % (color)


# def GetTotalRefines(infoDict):
# 	requiredCount = []
# 	hasCount = []
# 	for i in xrange(1, 6):
# 		key = 'item%d' % i
# 		if key in infoDict:
# 			requiredCount.append(infoDict[key][1])
# 			hasCount.append(player.GetItemCountByVnum(infoDict[key][0], True))
#
# 	itemCount = []
# 	i = 0
# 	for count in requiredCount:
# 		itemCount.append(hasCount[i] / count)
# 		i += 1
#
# 	posChange = []
# 	i = 0
# 	for val in itemCount:
# 		if val >= min(itemCount):
# 			posChange.append(i)
# 		i += 1
#
# 	for pos in posChange:
# 		itemCount[pos] = min(itemCount)
# 	totalUpgrades = itemCount[0] if all(cnt == itemCount[0] for cnt in itemCount) else 0
#
# 	color1 = "|cff89B88D[%d]|h|r" % totalUpgrades#green
# 	color2 = "|cffFF0000[%d]|h|r" % totalUpgrades#red
#
# 	color = color1
# 	if totalUpgrades <= 0:
# 		color = color2
#
# 	return ('%s' % (color))


class mainItem(ui.Window):

	def __init__(self, getParentEvent):
		ui.Window.__init__(self)
		self.SetParent(getParentEvent().GetBoard())
	
		self.refineCount = 0
		self.type = -1

		self.count_all = -1

		self.button = ui.Button()
		self.button.SetParent(self)
		self.button.SetEvent(self.Select)
		self.button.Show()

		self.image = ui.ImageBox()
		self.image.AddFlag("not_pick")
		self.image.SetParent(self)
		self.image.SetPosition(7,3)
		self.image.Show()

		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(40,2)
		self.text.Show()

		self.count_total = ui.TextLine()
		self.count_total.SetParent(self)
		self.count_total.SetPosition(16,2)
		self.count_total.Hide()

		self.getParentEvent = getParentEvent

		self.SetSize(280, 17)

	def SetRefineCount(self, num):
		self.refineCount += int(num)

		nameNew = ""
		if self.type == 2:
			if self.refineCount > 0:
				nameNew = "|cff89B88D[%d]|h|r" % (self.refineCount)
			# else:
				# nameNew = "|cffFF0000[%d]|h|r" % (0)

		self.text.SetText(nameNew + self.name)

	def SetName(self, name):
		self.name = name
		self.text.SetText(name)
		# dbg.LogBox('set name')

	def SetCountTotal(self, count):
		self.count_all = count

		if count <= 0:
			self.count_total.Hide()
			self.text.SetPosition(17,2)
			return

		if count * self.GetCountReward() >= 9900:
			self.count_total.SetText("[%d]" % (9900))
		else:
			self.count_total.SetText("[%d]" % (count * self.GetCountReward()) )
		self.count_total.SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
		self.count_total.Show()

		w, h = self.count_total.GetTextSize()
		self.text.SetPosition(w+20,2)

	def GetCount(self):
		return self.count_all

	def SetType(self,type):
		self.type = type

	def GetName(self):
		return self.name

	def GetType(self):
		return self.type

	def GetStepWidth(self):
		return 0

	def Select(self):
		self.getParentEvent().OnSelectItem(self)

class Category(mainItem):

	def __init__(self, getParentEvent):
		self.OpenFunc = False
		self.SubCategories = list()
		self.SubCategoriesByName = {}

		mainItem.__init__(self, getParentEvent)

		self.button.SetUpVisual("d:/ymir work/ui/game/cube/cube_menu_tab1.sub")
		self.button.SetOverVisual("d:/ymir work/ui/game/cube/cube_menu_tab1.sub")
		self.button.SetDownVisual("d:/ymir work/ui/game/cube/cube_menu_tab1.sub")

		self.SetType(1)

	def SetMainName(self, name = "None"):
		self.SetName(name)

	def AddSubCategory(self, name):
		subcat = SubCategory(self.getParentEvent)
		subcat.LoadInfo(name)
		subcat.Select()
		self.SubCategories.append(subcat)
		self.SubCategoriesByName[name] = subcat
		return self.SubCategories[len(self.SubCategories) - 1]

	def GetSubCategories(self):
		return self.SubCategories

	def SubCategoriesName(self, name):
		return name in self.SubCategoriesByName

	def GetSubCategoryByName(self, name):
		if name in self.SubCategoriesByName:
			return self.SubCategoriesByName[name]

		return None

	def Open(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab1_minus.sub")
		self.OpenFunc = True

	def Close(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab1_plus.sub")
		self.OpenFunc = False
		map(ui.Window.Hide, self.SubCategories)

	def IsOpen(self):
		return self.OpenFunc

	def Select(self):
		if self.OpenFunc:
			self.Close()
		else:
			self.Open()

		for x in self.GetSubCategories():
			x.Select()

		self.getParentEvent().OnRefresh()

class SubCategory(mainItem):

	def __init__(self, getParentEvent):

		self.OpenFunc = False
		self.SubCategoryObjects = []

		mainItem.__init__(self, getParentEvent)

		self.button.SetUpVisual("d:/ymir work/ui/game/cube/cube_menu_tab2.sub")
		self.button.SetOverVisual("d:/ymir work/ui/game/cube/cube_menu_tab2.sub")
		self.button.SetDownVisual("d:/ymir work/ui/game/cube/cube_menu_tab2.sub")

		self.image.SetPosition(7,4)
		self.text.SetPosition(25,2)
		self.text.SetPackedFontColor(0xffc2a046)

		self.SetType(2)
	
	def Refresh(self):
		self.refineCount = 0
		
		nameNew = ""

		if self.type == 2:
			for itemobj in self.SubCategoryObjects:
				self.refineCount += itemobj.infoDict['reward'][1] * GetMaxMultipler(itemobj.infoDict)

			if self.refineCount > 0:
				nameNew = "|cff89B88D[%d]|h|r" % (self.refineCount)

		self.text.SetText(nameNew + self.name)
	
	def LoadInfo(self, name):
		self.SetName(name)

	def AppendMainItem(self, name, infoDict):
		itemobj = SubCategoryMainItem(self.getParentEvent)
		itemobj.weak_parent = self
		itemobj.LoadInfo(name, infoDict)
		self.SubCategoryObjects.append(itemobj)

	def Open(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab2_minus.sub")
		self.OpenFunc = True

	def Close(self):
		self.image.LoadImage("d:/ymir work/ui/game/cube/cube_menu_tab2_plus.sub")
		self.OpenFunc = False
		map(ui.Window.Hide, self.SubCategoryObjects)

	def IsOpen(self):
		return self.OpenFunc

	def GetSubCategoriesObjects(self):
		return self.SubCategoryObjects

	def Select(self):
		if self.OpenFunc:
			self.Close()
		else:
			self.Open()

		self.getParentEvent().OnRefresh()

class SubCategoryMainItem(mainItem):
	def __init__(self, getParentEvent):
		mainItem.__init__(self, getParentEvent)

		self.button.SetUpVisual("d:/ymir work/ui/game/cube/cube_menu_tab3_default.sub")
		self.button.SetOverVisual("d:/ymir work/ui/game/cube/cube_menu_tab3_select.sub")
		self.button.SetDownVisual("d:/ymir work/ui/game/cube/cube_menu_tab3_select.sub")
		self.SetType(3)
		self.text.SetPosition(17,2)
		self.infoDict = {}
		
		self.iMaxMultipler = 0
		self.itemPos = -1
		self.weak_parent = None

	def LoadInfo(self, name, infoDict):
		self.SetName(name)
		self.infoDict = infoDict
		self.RefreshMultipler()

	def UnSelect(self):
		self.button.SetUp()
		self.button.Enable()

	def SetSelect(self):
		self.button.Down()
		self.button.Disable()

	def RefreshMultipler(self):
		# if self.infoDict['can_stack']:
		self.iMaxMultipler = GetMaxMultipler(self.infoDict)

		self.UpgradeChance(self.itemPos)
	
	def Refresh(self):
		self.RefreshMultipler()
		if self.weak_parent:
			self.weak_parent.Refresh()
	
	def UpgradeChance(self, itemPos):
		extraItemCount = 0
		refineCount = GetTotalRefines(self.infoDict)
		
		self.itemPos = itemPos
		if itemPos != -1:
			extraItemCount = player.GetItemCount(itemPos)
			# if self.infoDict['can_stack']:
			if self.iMaxMultipler > 0:
				self.SetName("|cff89B88D[%d]|h|r %s%s (%d%% + %d%%)" % (self.infoDict['reward'][1] * self.iMaxMultipler, self.infoDict['name'], GetItemGrade(self.infoDict['reward'][0], self.infoDict), self.infoDict['percent'], extraItemCount))
			else:
				self.SetName("|cffFF0000[%d]|h|r %s%s (%d%% + %d%%)" % (0, self.infoDict['name'], GetItemGrade(self.infoDict['reward'][0], self.infoDict), self.infoDict['percent'], extraItemCount))
			# else:
				# self.SetName("%s%s (%d%% + %d%%)" % (self.infoDict['name'], GetItemGrade(self.infoDict['reward'][0], self.infoDict), self.infoDict['percent'], extraItemCount))
				
		else:
			# if self.infoDict['can_stack']:
			if self.iMaxMultipler > 0:
				self.SetName("|cff89B88D[%d]|h|r %s%s (%d%%)" % (self.infoDict['reward'][1] * self.iMaxMultipler, self.infoDict['name'], GetItemGrade(self.infoDict['reward'][0], self.infoDict), self.infoDict['percent']))
			else:
				self.SetName("|cffFF0000[%d]|h|r %s%s (%d%%)" % (0, self.infoDict['name'], GetItemGrade(self.infoDict['reward'][0], self.infoDict), self.infoDict['percent']))
			# else:
				# self.SetName("%s%s (%d%%)" % (self.infoDict['name'], GetItemGrade(self.infoDict['reward'][0], self.infoDict), self.infoDict['percent']))
				
			
	def Select(self):
		if self.infoDict:
			self.getParentEvent().SelectObject(self, self.infoDict)

class CubeRenewalWindows(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.CategoryList = list()
		self.showingItemList = []
		self.NPCVNum = 0
		self.infoDict = {}
		self.handler = None
		self.MaxElements = 12
		self.toolTip = uiToolTip.ItemToolTip()
		self.LoadWindow()
		self.vnum_item_improve = 79605
		self.max_count_item_improve = 40
		self.slot_item_improve = -1
		self.multiplier = 1
		self.inventoryHandler = None
		self.specialInventoryHandler = None

	def __del__(self):
		ui.ScriptWindow.__init__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/cuberenewalwindow.py")
		except:
			import exception
			exception.Abort("CubeRenewalWindows.LoadWindow.LoadObject")

		try:
			self.board = self.GetChild("item_list_board")
			self.item_slot = self.GetChild("item_slot")
			self.cube_list_scroll_bar = self.GetChild("cube_list_scroll_bar")
			self.yang_text = self.GetChild("yang_text")
			if app.ENABLE_GEM_SYSTEM: #ENABLE_CUBE_RENEWAL_GEM
				self.gem_text = self.GetChild("gem_text")
			self.result_qty = self.GetChild("result_qty")
			self.result_qty.SetEscapeEvent(ui.__mem_func__(self.OnPressEscapeKey))
			self.result_qty.SetReturnEvent(ui.__mem_func__(self.OnPressQty))
			# self.result_qty.CanEdit(True)
			self.result_qty.SetText("1")

			self.button_ok = self.GetChild("button_ok")
			self.button_cancel = self.GetChild("button_cancel")

			self.qty_sub_button = self.GetChild("qty_sub_button")
			self.qty_add_button = self.GetChild("qty_add_button")

			self.imporve_slot = self.GetChild("imporve_slot")

			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.cube_elements = {}
			for i in xrange(1,6):
				self.cube_elements["count_elements_%d" % i] = self.GetChild("material_qty_text_%d"%i)

			self.slot_improve = ui.SlotWindow()
			self.slot_improve.SetParent(self.imporve_slot)
			self.slot_improve.SetSize(32,32)
			self.slot_improve.SetPosition(6,5)
			self.slot_improve.vnum = 0
			self.slot_improve.SAFE_SetButtonEvent("RIGHT", "EXIST", self.__OnSelectItemSlot)
			self.slot_improve.SetSelectEmptySlotEvent(ui.__mem_func__(self.__OnSelectEmptySlot))
			self.slot_improve.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnSelectItemSlot))
			self.slot_improve.SetOverInItemEvent(ui.__mem_func__(self.OverInItemChance))
			self.slot_improve.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.slot_improve.AppendSlot(0,0,0,32,32)
			self.slot_improve.Show()

			self.icons_items = ui.SlotWindow()
			self.icons_items.SetParent(self.item_slot)
			self.icons_items.SetPosition(25,13)
			self.icons_items.SetSize(32*9,32*3)
			self.icons_items.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.icons_items.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.icons_items.AppendSlot(0,0,0,32,32*3)

			self.qty_sub_button.SetEvent(self.QtySubButton)
			self.qty_add_button.SetEvent(self.QtyAddButton)

			self.cube_list_scroll_bar.SetScrollEvent(ui.__mem_func__(self.OnScroll))

			for i in xrange(5):
				self.icons_items.AppendSlot(i+1,46*i+62,0,32,32*3)

			self.icons_items.Show()

			self.button_ok.SetEvent(self.SendRefine)
			self.button_cancel.SetEvent(self.Close)

			self.SetCenterPosition()

		except:
			import exception
			exception.Abort("CubeRenewalWindows.LoadWindow.LoadElements")

	def RoundText(self):
		if not self.result_qty.GetText():
			return

		try:
			current_value = int(self.result_qty.GetText())
		except ValueError:
			self.result_qty.SetText("0")
			self.result_qty.KillFocus()
			return

		if current_value > 500:
			new_value = 500
		else:
			new_value = current_value

		if osfInfo.IS_BLEND_POTION(self.infoDict['reward'][0]) or osfInfo.IS_EXTENDED_BLEND_POTION(self.infoDict['reward'][0]):
			new_value = 1

		self.result_qty.SetText(str(new_value))
		self.result_qty.KillFocus()
	def OnPressQty(self):
		if not self.result_qty.GetText():
			return

		self.RoundText()
		if self.infoDict and self.infoDict['can_stack']:
			txt = int(self.result_qty.GetText())
			self.multiplier = txt / self.infoDict['reward'][1]
			if self.multiplier <= 0:
				self.multiplier = 1

			self.Refresh()

	def OverInItemChance(self):
		if self.slot_improve.vnum == 0:
			return

		if self.toolTip:
			self.toolTip.SetItemToolTip(self.slot_improve.vnum)
			self.toolTip.ShowToolTip()

	def OnScroll(self):
		scrollLineCount = len(self.showingItemList) - (self.MaxElements)
		startLine = int(scrollLineCount * self.cube_list_scroll_bar.GetPos())

		if startLine != self.startLine:
			self.startLine = startLine
			self.SetExtraInfo()

	def OnRunMouseWheel(self, pos):
		if self.cube_list_scroll_bar.IsShow():
			if pos > 0:
				self.cube_list_scroll_bar.OnUp()
			else:
				self.cube_list_scroll_bar.OnDown()


	def QtySubButton(self):
		if self.infoDict and self.infoDict['can_stack']:
			if osfInfo.IS_BLEND_POTION(self.infoDict['reward'][0]) or osfInfo.IS_EXTENDED_BLEND_POTION(self.infoDict['reward'][0]):
				if self.multiplier <= 1:
					self.multiplier = 1 // self.infoDict['reward'][1]
					new_value = 1
				else:
					self.multiplier -= 1
					new_value = self.infoDict['reward'][1] * self.multiplier
			else:
				if self.multiplier <= 1:
					self.multiplier = 500 // self.infoDict['reward'][1]
					new_value = 500
				else:
					self.multiplier -= 1
					new_value = self.infoDict['reward'][1] * self.multiplier

			self.result_qty.SetText(str(new_value))
			self.Refresh()

	def QtyAddButton(self):
		if self.infoDict and self.infoDict['can_stack']:
			new_value = self.infoDict['reward'][1] * (self.multiplier + 1)
			if osfInfo.IS_BLEND_POTION(self.infoDict['reward'][0]) or osfInfo.IS_EXTENDED_BLEND_POTION(self.infoDict['reward'][0]):
				if new_value >= 1:
					self.multiplier = 1 // self.infoDict['reward'][1]
					new_value = 1
				else:
					self.multiplier += 1
					new_value = self.infoDict['reward'][1] * self.multiplier
			else:
				if new_value >= 500:
					self.multiplier = 500 // self.infoDict['reward'][1]
					new_value = 500
				else:
					self.multiplier += 1
					new_value = self.infoDict['reward'][1] * self.multiplier

			self.result_qty.SetText(str(new_value))
			self.Refresh()

	def SendRefine(self):
		vnum = self.infoDict['reward'][0]

		reqItemsArg = list()
		for i in xrange(5):
			key = 'item%d' % (i+1)
			if key in self.infoDict:
				reqItemsArg.append(self.infoDict[key][0])

		cuberenewal.SendRefine(vnum, self.multiplier, self.slot_item_improve, len(reqItemsArg), *reqItemsArg)

	def GetBoard(self):
		return self.board

	def GetSelf(self):
		return self

	def Reset(self):
		if len(self.showingItemList) > 0:
			map(ui.Window.Hide, self.showingItemList)

		for group in self.CategoryList:
			group.Close()

			for categorias in group.GetSubCategories():
				categorias.Close()

		for i in xrange(0,6):
			self.icons_items.ClearSlot(i)
			if i > 0:
				self.cube_elements["count_elements_%d"%i].Hide()

		self.slot_improve.SetItemSlot(0,0,0)
		self.slot_improve.vnum = 0

		self.slot_item_improve = -1
		if self.handler:
			self.handler.UpgradeChance(self.slot_item_improve)

		self.multiplier = 1
		self.result_qty.SetText("")

		self.yang_text.Hide()
		if app.ENABLE_GEM_SYSTEM:
			self.gem_text.Hide()
		self.result_qty.Hide()

		self.CategoryList = []
		self.startLine = 0

		self.cube_list_scroll_bar.SetPos(0)

		# self.CategoryList = []
		# self.startLine = 0
		self.LoadCategories()
		# self.OnRefresh()

	def SetNpcVnum(self, vnum):
		self.NPCVNum = vnum

	def GetCategory(self, list, name_match):
		name_match = name_match.lower()
		for cat in list:
			fnd = cat.lower()
			if fnd == name_match:
				return fnd

		return NM

	def GetCategory(self, name):
		for cat in self.CategoryList:
			name_cat = cat.text.GetText()

			if name.find(name_cat) != -1:
				return cat

		return None

	def RetCleanName(self, name):
		text = ""

		for txt in name:
			if txt.isalpha() == False and txt != "_":
				continue

			text += txt

		return text

	def LoadCategories(self):
		# import dbg
		if len(self.showingItemList) > 0:
			map(ui.Window.Hide, self.showingItemList)

		for group in self.CategoryList:
			group.Close()

			for categorias in group.GetSubCategories():
				categorias.Close()

		for i in xrange(0,6):
			self.icons_items.ClearSlot(i)
			if i > 0:
				self.cube_elements["count_elements_%d"%i].Hide()

		self.yang_text.Hide()
		if app.ENABLE_GEM_SYSTEM:
			self.gem_text.Hide()
		self.result_qty.Hide()

		self.CategoryList = []
		self.startLine = 0

		self.cube_list_scroll_bar.SetPos(0)

		catByName = {}
		self.CategoryList = []
		categoryNames = {}
		categoryNames["ARMOR"] =  localeInfo.CUBE_CATEGORY_ARMOR
		categoryNames["WEAPON"] =  localeInfo.CUBE_CATEGORY_WEAPON
		categoryNames["ACCESSORY"] =  localeInfo.CUBE_CATEGORY_ACCESSORY
		categoryNames["BELT"] =  localeInfo.CUBE_CATEGORY_BELT
		categoryNames["EVENT"] =  localeInfo.CUBE_CATEGORY_EVENT
		categoryNames["ETC"] =  localeInfo.CUBE_CATEGORY_ETC
		categoryNames["JOB"] =  localeInfo.CUBE_CATEGORY_JOB
		categoryNames["SETADD_WEAPON"] =  localeInfo.CUBE_CATEGORY_SETADD_WEAPON
		categoryNames["SETADD_ARMOR"] =  localeInfo.CUBE_CATEGORY_SETADD_ARMOR
		categoryNames["SETADD_HELMET"] =  localeInfo.CUBE_CATEGORY_SETADD_HELMET
		categoryNames["PET"] =  localeInfo.CUBE_CATEGORY_PET
		categoryNames["SKILL_BOOK"] =  localeInfo.CUBE_CATEGOR_SKILL_BOOK
		categoryNames["ARMOR_GLOVE"] =  localeInfo.CUBE_CATEGORY_ARMOR_GLOVE

		tempCat = None
		for info in cuberenewal.GetInfo(self.NPCVNum):
			info['category'] = self.RetCleanName(info['category'])

			if info['category'] in categoryNames:
				info['category'] = categoryNames[info['category']]

			tempCat = self.GetCategory(info['category'])

			if tempCat == None:
				tempCat = Category(ui.__mem_func__(self.GetSelf))
				catByName[info['category']] = tempCat
				self.CategoryList.append(tempCat)

			tempCat.SetMainName(info['category'])
			set_string = ""
			# dbg.TraceError("str: %s, cat: %s" % (set_string, info['name']))

			if info['set_value']:
				set_string = "%s" % ([localeInfo.TOOLTIP_SET_ITEM_1, localeInfo.TOOLTIP_SET_ITEM_2, localeInfo.TOOLTIP_SET_ITEM_3, localeInfo.TOOLTIP_SET_ITEM_4, localeInfo.TOOLTIP_SET_ITEM_5][info['set_value']-1])
			info['name'] = '%s%s' % (set_string, info['name'])

			# dbg.TraceError("str: %s, cat: %s" % (set_string, info['name']))

			subCat = tempCat.GetSubCategoryByName(info['name'])
			if 'name' in info and not tempCat.SubCategoriesName(info['name']):
				subCat = tempCat.AddSubCategory(info['name'])
			tempCat.Close()
			tempCat.Show()
			if subCat:
				infoDict = {
					'reward' : info['reward'],
					'percent' : info['percent'],
					'gold' : info['gold'],
					'gem' : info['gem'],
					'set_value' : info['set_value'],
					'can_stack' : info['can_stack'],
					'name' : info['name'],
					'npc' : info['npcVnum'],
					'allow_copy' : info['allow_copy'],
				}
				for i in xrange(0, 5):
					if 'item%d' % i in info:
						infoDict['item%d' % (i+1) ] = info['item%d' % i ]
				refineCount = GetMaxMultipler(infoDict)
				subCat.AppendMainItem("%s%s (%d%%)" % (info['name'], GetItemGrade(info['reward'][0], info), info['percent']), infoDict)
				# if info['can_stack']:
				subCat.SetRefineCount(infoDict['reward'][1] * refineCount)
		self.OnRefresh()

	def Close(self):
		self.infoDict = {}
		self.Hide()
		cuberenewal.SendClosePacket()
		self.inventoryHandler.DelHighLightCube()
		self.specialInventoryHandler.DelHighLightCube()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def RefreshItems(self):
		if self.handler:
			self.handler.Refresh()

		if self.inventoryHandler:
			self.inventoryHandler.RefreshHighlights()

		if self.specialInventoryHandler:
			self.specialInventoryHandler.RefreshHighlights()

	def SelectObject(self, handler, infoDict):
		self.infoDict = infoDict
		for group in self.CategoryList:
			for categorias in group.GetSubCategories():
				for a in categorias.GetSubCategoriesObjects():
					a.UnSelect()
		handler.SetSelect()
		if self.inventoryHandler:
			self.inventoryHandler.HighlightSlot_Clear()
			self.inventoryHandler.RefreshHighlights()

		if self.specialInventoryHandler:
			self.specialInventoryHandler.HighlightSlot_Clear()
			self.specialInventoryHandler.RefreshHighlights()

		self.handler = handler

		self.result_qty.SetText(str(self.infoDict['reward'][1]))
		self.Refresh()
		# if self.handler:
			# self.handler.RefreshMultipler()

	def OnUpdate(self):
		# if self.infoDict:
		self.Refresh()
		if self.handler:
			if self.handler.infoDict and self.handler.infoDict['reward'][0] != 0 and self.slot_improve.vnum != 0:
				item.SelectItem(self.handler.infoDict['reward'][0])

				if item.ITEM_TYPE_BELT != item.GetItemType():
					self.slot_improve.SetItemSlot(0,0,0)
					self.slot_improve.vnum = 0
					self.slot_item_improve = -1

			self.handler.UpgradeChance(self.slot_item_improve)

	def Refresh(self):
		if self.infoDict and self.infoDict['can_stack'] == False:
			self.result_qty.SetText("1")
			self.multiplier = 1

		if self.infoDict and self.infoDict['can_stack'] == True:

			new_value = self.infoDict['reward'][1] * (self.multiplier + 1)

			if new_value > 500:
				self.multiplier = 500 // self.infoDict['reward'][1]
				new_value = 500

			self.result_qty.SetText(str(new_value))

		infoDict = self.infoDict

		if infoDict:
			self.result_qty.SetText(str(infoDict['reward'][1] * self.multiplier))
		else:
			self.result_qty.SetText('')
		if self.infoDict and self.infoDict['can_stack'] == True:
			if osfInfo.IS_BLEND_POTION(self.infoDict['reward'][0]) or osfInfo.IS_EXTENDED_BLEND_POTION(self.infoDict['reward'][0]):
				self.result_qty.SetText("1")
				self.multiplier = 1

		self.icons_items.ClearSlot(0)
		if infoDict:
			self.icons_items.SetItemSlot(0, infoDict['reward'][0], infoDict['reward'][1] * self.multiplier)

		for i in xrange(1, 6):
			self.icons_items.ClearSlot(i)
			key = 'item%d' % i
			if key in infoDict:
				self.icons_items.SetItemSlot(i, infoDict[key][0], infoDict[key][1])

		self.result_qty.KillFocus()
		# self.result_qty.CanEdit(False)

		self.result_qty.Show()

		for i in xrange(1, 6):
			key = 'item%d' % i
			cubeKey = "count_elements_%d" % i 
			if key in infoDict and infoDict:
				# if self.selectedItem.GetVnumMaterial(i) != 0:
				if player.GetItemCountByVnum(infoDict[key][0], True, True) >= infoDict[key][1] * self.multiplier:
					self.cube_elements[cubeKey].SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
				else:
					self.cube_elements[cubeKey].SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))

				self.cube_elements[cubeKey].SetText("%d/%d"%(player.GetItemCountByVnum(infoDict[key][0], True, True), infoDict[key][1] * self.multiplier))
				import chat
				if self.inventoryHandler:
					for slot in self.inventoryHandler.GetSlotListByVnum(infoDict[key][0], infoDict[key][1]):
						self.inventoryHandler.ActivateSlot(slot, wndMgr.HILIGHTSLOT_CUBE)
					self.inventoryHandler.RefreshHighlights()

				if self.specialInventoryHandler:
					for slot in self.specialInventoryHandler.GetSlotListByVnum(infoDict[key][0], infoDict[key][1]):
						self.specialInventoryHandler.ActivateSlot(slot, wndMgr.HILIGHTSLOT_CUBE)
					self.specialInventoryHandler.RefreshHighlights()

				self.cube_elements[cubeKey].Show()
			else:
				self.cube_elements[cubeKey].Hide()

			if key in infoDict and infoDict:
				self.icons_items.ClearSlot(i)
				self.icons_items.SetItemSlot(i, infoDict[key][0], infoDict[key][1] * self.multiplier)

		if infoDict:
			self.yang_text.SetText(localeInfo.NumberToMoneyString(infoDict['gold'] * self.multiplier))

			if 'gold' in infoDict and int(infoDict['gold']) * self.multiplier <= player.GetElk():
				self.yang_text.SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
			else:
				self.yang_text.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
		else:
			self.yang_text.SetText('')
		self.yang_text.Show()

		if app.ENABLE_GEM_SYSTEM: #ENABLE_CUBE_RENEWAL_GEM
			if infoDict:
				self.gem_text.SetText(localeInfo.NumberToMoneyStringEx(infoDict['gem'] * self.multiplier))

				if 'gem' in infoDict and int(infoDict['gem']) * self.multiplier <= player.GetGem():
					self.gem_text.SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
				else:
					self.gem_text.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
			else:
				self.gem_text.SetText('')
			self.gem_text.Show()

	def OnRefresh(self):
		self.showingItemList = []

		for group in self.CategoryList:
			self.showingItemList.append(group)

			if group.IsOpen():
				for categorias in group.GetSubCategories():
					self.showingItemList.append(categorias)

					if categorias.IsOpen():
						for a in categorias.GetSubCategoriesObjects():
							self.showingItemList.append(a)

		self.SetExtraInfo()

	def SetExtraInfo(self):
		if self.MaxElements >= len(self.showingItemList):
			self.cube_list_scroll_bar.Hide()
			self.startLine = 0
		else:
			if self.showingItemList:
				self.cube_list_scroll_bar.SetMiddleBarSize(float(self.MaxElements) / float(len(self.showingItemList)))
			self.cube_list_scroll_bar.Show()

		map(ui.Window.Hide, self.showingItemList)

		yPos = 11
		heightLimit = 240

		for item in self.showingItemList[self.startLine:]:
			XPos = 0
			if item.GetType() == 2:
				XPos += 15
			if item.GetType() == 3:
				XPos += 35
			item.SetPosition(6 + XPos, yPos)
			item.SetTop()
			item.Show()
			yPos += 20

			if yPos > heightLimit:
				break

	def BindInventoryClass(self, inventoryHandler):
		from _weakref import proxy
		self.inventoryHandler = proxy(inventoryHandler)

	def BindSpecialInventoryClass(self, specialInventoryHandler):
		from _weakref import proxy
		self.specialInventoryHandler = proxy(specialInventoryHandler)

	def GetAttrCopyNextItem(self, vnum):
		for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
			if player.GetItemSetValue(player.INVENTORY, i):
				continue

			if vnum == player.GetItemIndex(i):
				return i

		return -1

	def OverInItem(self, index):
		self.toolTip.ClearToolTip()

		infoDict = self.infoDict

		if infoDict:
			if index == 0:
				vnum = infoDict['reward'][0]
			else:
				vnum = infoDict['item%d'%index][0]
		else:
			return

		subtype_item_inv = None
		check_item = -1

		if infoDict['set_value'] or infoDict['allow_copy'] and index in (0, 1):
			for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
				iVnum = player.GetItemIndex(i)
				if player.GetItemSetValue(player.INVENTORY, i):
					continue

				if iVnum == vnum:
					check_item = i
					break

		metinSlot = []
		AttrList = []
		if app.ENABLE_APPLY_RANDOM:
			apply_random_list = []

		for i in xrange(player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT):
			item_vnum = player.GetItemIndex(i)

			if 'item%d' % index in infoDict and item_vnum == infoDict['item%d' % index][0] and check_item == -1:
				check_item = i

		# if app.ENABLE_REFINE_ELEMENT:
		# 	self.toolTip.FuncElementSpellItemDate(check_item)

		if check_item != -1:
			if infoDict['set_value']:
				for c in xrange(player.METIN_SOCKET_MAX_NUM):
					metinSlot.append(0)

				for b in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					AttrList.append((0, 0))

				if app.ENABLE_APPLY_RANDOM:
					for d in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM):
						apply_random_list.append((0, 0))
				if app.ENABLE_REFINE_ELEMENT:
					self.toolTip.ClearElementsSpellItemDate()

			else:
				for c in xrange(player.METIN_SOCKET_MAX_NUM):
					metinSlot.append(player.GetItemMetinSocket(check_item,c))

				for b in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					AttrList.append(player.GetItemAttribute(check_item,b))

				if app.ENABLE_APPLY_RANDOM:
					for d in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM):
						apply_random_list.append(player.GetItemApplyRandom(check_item, d))

				if app.ENABLE_REFINE_ELEMENT:
					self.toolTip.FuncElementSpellItemDate(check_item)

		else:
			for c in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(0)

			for b in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				AttrList.append((0,0))
			if app.ENABLE_APPLY_RANDOM:
				for d in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM):
					apply_random_list.append((0, 0))

			if app.ENABLE_REFINE_ELEMENT:
				self.toolTip.ClearElementsSpellItemDate()

		if infoDict['set_value'] or infoDict['allow_copy'] and index == 0:
			idx_inv = self.GetAttrCopyNextItem(infoDict['item%d' % 1][0])

			if idx_inv != -1:
				metinSlot = []
				AttrList = []
				apply_random_list = []

				for c in xrange(player.METIN_SOCKET_MAX_NUM):
					metinSlot.append(player.GetItemMetinSocket(idx_inv, c))

				for b in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					AttrList.append(player.GetItemAttribute(idx_inv, b))

				if app.ENABLE_APPLY_RANDOM:
					for d in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM):
						apply_random_list.append(player.GetItemApplyRandom(idx_inv, d))

				if app.ENABLE_REFINE_ELEMENT:
					self.toolTip.FuncElementSpellItemDate(idx_inv)


		# if app.ENABLE_REFINE_ELEMENT:
		# 	self.toolTip.FuncElementSpellItemDate(check_item)

		if index == 0:
			if osfInfo.IS_GLOVE_SUNGMA_BONUS(self.infoDict['reward'][0]):
				apply_random_list = []
				for d in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM):
					apply_random_list.append((0, 0))
			if osfInfo.IS_CRAFT_SUNGMA_BONUS(self.infoDict['reward'][0]) and infoDict['set_value'] == 0:
				apply_random_list = []
				for d in xrange(player.APPLY_RANDOM_SLOT_MAX_NUM):
					apply_random_list.append((0, 0))

		if app.ENABLE_SET_ITEM:
			cube_set_value = 0
			if index == 0:
				if infoDict['set_value'] != player.SET_ITEM_SET_VALUE_NONE:
					cube_set_value = infoDict['set_value']
					chat.AppendChat("test 4")

		# if app.ENABLE_REFINE_ELEMENT:
		# 	self.toolTip.FuncElementSpellItemDate(check_item)


		self.toolTip.AddItemData(int(vnum), metinSlot, AttrList, None, 0, 0, player.INVENTORY, -1, None, apply_random_list, set_value=cube_set_value)

		if index == 0:
			#if infoDict['reward'][0] == 360 and infoDict['set_value'] == 0:
			if osfInfo.IS_CRAFT_SUNGMA_BONUS(self.infoDict['reward'][0]) and infoDict['set_value'] == 0:
				self.toolTip.AppendSpace(5)
				self.toolTip.AppendTextLine(localeInfo.TOOLTIP_CRAFT_RANDOM_BONUS, grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))  # 4121 16748
				self.toolTip.AppendSpace(5)
				self.toolTip.AppendTextLine(localeInfo.TOOLTIP_CRAFT_3_RANDOM_BONUS, 0xff7AF6D4)
			if osfInfo.IS_GLOVE_SUNGMA_BONUS(self.infoDict['reward'][0]):
			#if infoDict['reward'][0] == 23050:
				self.toolTip.AppendSpace(5)
				self.toolTip.AppendTextLine(localeInfo.TOOLTIP_CRAFT_RANDOM_BONUS_VALUE, grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
				if infoDict['reward'][0] == 23000:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("3 - 7 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_STR, 0xff7AF6D4)
				if infoDict['reward'][0] == 23010:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("3 - 7 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_MOVE, 0xff7AF6D4)
				if infoDict['reward'][0] == 23020:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("3 - 7 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_HP, 0xff7AF6D4)
				if infoDict['reward'][0] == 23030:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("3 - 7 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_IMMUNE, 0xff7AF6D4)
				if infoDict['reward'][0] == 23050:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("6 - 9 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_STR, 0xff7AF6D4)
				if infoDict['reward'][0] == 23060:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("6 - 9 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_MOVE, 0xff7AF6D4)
				if infoDict['reward'][0] == 23070:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("6 - 9 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_HP, 0xff7AF6D4)
				if infoDict['reward'][0] == 23080:
					self.toolTip.AppendSpace(5)
					self.toolTip.AppendTextLine("6 - 9 " + localeInfo.TOOLTIP_CRAFT_SUNGMA_IMMUNE, 0xff7AF6D4)

	def OverOutItem(self):
		self.toolTip.Hide()

	def __OnSelectEmptySlot(self,selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()

		if not self.infoDict:
			return

		if self.infoDict['percent'] >= 100:
			return

		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			itemVNum = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			if itemVNum == self.vnum_item_improve and itemCount <= self.max_count_item_improve:
				self.slot_improve.SetItemSlot(selectedSlotPos,itemVNum,itemCount)
				self.slot_improve.vnum = itemVNum

				self.slot_item_improve = attachedSlotPos
				if self.handler:
					self.handler.UpgradeChance(self.slot_item_improve)

	def __OnSelectItemSlot(self,selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()

		if not self.infoDict:
			return

		if self.infoDict['percent'] >= 100:
			return

		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			itemVNum = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			if itemVNum == self.vnum_item_improve and itemCount <= self.max_count_item_improve:
				self.slot_improve.SetItemSlot(selectedSlotPos,itemVNum,itemCount)
				self.slot_improve.vnum = itemVNum
				
				self.slot_item_improve = attachedSlotPos
				if self.handler:
					self.handler.UpgradeChance(self.slot_item_improve)
		else:
			self.slot_improve.SetItemSlot(0,0,0)
			self.slot_improve.vnum = 0

			self.slot_item_improve = -1
			if self.handler:
				self.handler.UpgradeChance(self.slot_item_improve)
