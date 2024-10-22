
Modules = Modules or {}

--副本扫荡单次结果条目
Modules.TemplDungeonCleanResultItem = Modules.TemplDungeonCleanResultItem or BaseClass(GUI.Template)

function Modules.TemplDungeonCleanResultItem:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_CLEAN_RESULT_ITEM)	

	self.__txt_times = Util:WidgetLabel(self:GetWidget("Text_Times"), "", Macros.TypefaceSize.largish, Macros.Color.headline )
	self.__txt_get_gold = Util:WidgetLabel(self:GetWidget("Text_GetGold"), "", Macros.TypefaceSize.largish, Macros.Color.content )
	self.__txt_get_exp = Util:WidgetLabel(self:GetWidget("Text_GetExp"), "", Macros.TypefaceSize.largish, Macros.Color.content )
	self.__txt_no_drop = Util:WidgetLabel(self:GetWidget("Text_NoDrop"), CPPGameLib.GetString("dungeon_clean_not_drop"), Macros.TypefaceSize.largish, Macros.Color.content )
	self.__list_drop = self:GetWidget("ListView_DropList")
	self.__list_drop:SetTouchSwallow(false)

	self:SetData(data)
end

function Modules.TemplDungeonCleanResultItem:__delete()
end

function Modules.TemplDungeonCleanResultItem:SetData(data)
	GUI.Template.SetData(self, data)

	self.__txt_times:SetText(CPPGameLib.GetString("dungeon_clean_times_index", self.__data.clean_times_index))

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)	

	if self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON then
		local reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(Macros.Game.FBType.MAIN_DUNGEON, self.__data.gamelevel_id,self.__data.is_level_up and (role_level - 1) or role_level)	
		self.__txt_get_gold:SetText(reward_currency_map[Macros.Game.RolePropType.GOLD])
		self.__txt_get_exp:SetText(reward_currency_map[Macros.Game.RolePropType.EXP])
	end
	self.__txt_get_gold:SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	self.__txt_get_exp:SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	self:GetWidget("Image_7"):SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	self:GetWidget("Image_7_0"):SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)

	local drop_item_list = self.__data.drop_item_list
	local has_drop = #drop_item_list > 0
	self.__list_drop:SetVisible(has_drop)
	self.__txt_no_drop:SetVisible(not has_drop)
	if has_drop then
		self.__list_drop:RemoveAllItems()
		local icon
		for _, drop_item in ipairs(drop_item_list) do
			icon = LogicGUI.IconLayer.New(false, false)
			icon:SetData({id = drop_item.item_id, num = drop_item.item_num})
			self.__list_drop:PushBackCustomItem(icon:GetNode())
		end
	end
end

function Modules.TemplDungeonCleanResultItem:__AddToParent(parent_node, fill_parent)
end