
Modules = Modules or {}

--副本扫荡累计结果条目
Modules.TemplDungeonCleanResultTotalItem = Modules.TemplDungeonCleanResultTotalItem or BaseClass(GUI.Template)

function Modules.TemplDungeonCleanResultTotalItem:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_CLEAN_RESULT_TOTAL_ITEM)	

	self.__txt_get_gold = Util:WidgetLabel(self:GetWidget("Text_GetGold"), "", Macros.TypefaceSize.largish, Macros.Color.content)
	self.__txt_get_exp = Util:WidgetLabel(self:GetWidget("Text_GetExp"), "", Macros.TypefaceSize.largish, Macros.Color.content)
	self.__txt_no_drop = Util:WidgetLabel(self:GetWidget("Text_NoDrop"), CPPGameLib.GetString("dungeon_clean_not_drop"), Macros.TypefaceSize.largish, Macros.Color.content)
	self.__list_drop = self:GetWidget("ListView_DropList")
	self.__list_drop:SetTouchSwallow(false)

	self:SetData(data)
end

function Modules.TemplDungeonCleanResultTotalItem:__delete()
end

function Modules.TemplDungeonCleanResultTotalItem:SetData(data)
	GUI.Template.SetData(self, data)

	local effect_id,effect = self:__CreateEffect({id = 100051},self:GetWidget("Image_30"),-1)
	PosTool.Center(effect,0,20)

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)	
	--升级
	if self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON then
		local gold_num,exp_num
		local reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(Macros.Game.FBType.MAIN_DUNGEON, self.__data.gamelevel_id,self.__data.is_level_up and (role_level - 1) or role_level)			
		gold_num = reward_currency_map[Macros.Game.RolePropType.GOLD] * self.__data.clean_times
		exp_num = reward_currency_map[Macros.Game.RolePropType.EXP] * self.__data.clean_times
		self.__txt_get_gold:SetText(gold_num or 0)
		self.__txt_get_exp:SetText(exp_num or 0)	
	end
	self.__txt_get_gold:SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	self.__txt_get_exp:SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	self:GetWidget("Image_10"):SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	self:GetWidget("Image_10_0"):SetVisible(self.__data.fb_type ~= Macros.Game.FBType.DAILY_DUNGEON)
	
	local drop_item_map = {}
	for _, drop_data in ipairs(self.__data.drop_data_list) do
		for _, drop_item in ipairs(drop_data) do
			drop_item_map[drop_item.item_id] = drop_item_map[drop_item.item_id] or 0
			drop_item_map[drop_item.item_id] = drop_item_map[drop_item.item_id] + drop_item.item_num
		end
	end

	local has_drop = CPPGameLib.GetTableLength(drop_item_map) > 0
	self.__list_drop:SetVisible(has_drop)
	self.__txt_no_drop:SetVisible(not has_drop)

	if has_drop then
		self.__list_drop:RemoveAllItems()
		for _id, _num in pairs(drop_item_map) do
			icon = LogicGUI.IconLayer.New(false, false)
			icon:SetData({id = _id, num = _num})
			self.__list_drop:PushBackCustomItem(icon:GetNode())	
			
			local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.MAIN_DUNGEON )
			if num ~= 1 then
				local double_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "double" , true )
				local double = Util:Sprite( double_sp )
				double:SetAnchorPoint( 0 , 1 )
				double:SetPosition( 0 , icon:GetContentHeight() )
				icon:AddChild( double )
			end

		end
	end
end

function Modules.TemplDungeonCleanResultTotalItem:__AddToParent(parent_node, fill_parent)
end