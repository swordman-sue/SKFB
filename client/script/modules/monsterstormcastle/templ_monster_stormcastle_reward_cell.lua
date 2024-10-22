Modules = Modules or {}

--怪物攻城BOSS奖励
Modules.TemplMonsterStormCastleRewardCell = Modules.TemplMonsterStormCastleRewardCell or BaseClass(GUI.ScrollItem)

function Modules.TemplMonsterStormCastleRewardCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_MONSTER_STROMCASTLE_REWARD_CELL)
end

function Modules.TemplMonsterStormCastleRewardCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)
	--攻城信息
	local attack_city_info = CPPGameLib.GetConfig("AttackCityMonster",self.__data.chapter)
	--关卡信息
	local gamelevel_info = CPPGameLib.GetConfig("GameLevel",attack_city_info.map_id_list[1])
	--怪物信息
	local monster_config = CPPGameLib.GetConfig("Monster",gamelevel_info.icon_id)
	--奖励条件
	Util:WidgetLabel(self:GetWidget("Text_monsterName"),CPPGameLib.GetString("monster_stormcastle_boss_reward_condition",CPPGameLib.GetString("monster_stormcastle_add_boss_name",monster_config.name)), Macros.TypefaceSize.popup, Macros.Color.keypoint)
	--滚动容器
	local panel_rewardscrollview = self:GetWidget("Panel_rewardScrollView")
	local params = {
		item_class = Modules.TemplMonsterStormCastleRewardListCell,
	    item_width = 80,
	    item_height = 80,
	    row	= 1,
	    col	= 3,
	    item_space_c = 8,
	    view_width = 266,
	    view_height = 80, 
	}
	self.__boss_reward_list = GUI.ScrollView.New(params)
	panel_rewardscrollview:AddChild(self.__boss_reward_list:GetNode())
	PosTool.Center(self.__boss_reward_list)
	--奖励列表
	self.__reward_list = {}
	for i,cur_item_id in ipairs(self.__data.item_id_list) do
		local data = {id = cur_item_id,num = self.__data.item_num_list[i]}
		table.insert(self.__reward_list,data)
	end
	self.__boss_reward_list:SetDataList(self.__reward_list)
	--图片状态
	self.__image_haveget = self:GetWidget("Image_haveGet")
	self.__image_haveget:SetVisible(self.__data.__have_reward)
	--按钮状态
	self.__button_get = self:GetWidget("Button_get")
	Util:WidgetButton(self.__button_get, CPPGameLib.GetString("dungeon_receive"),function ()
		self.__button_get:SetEnabled(false)
		--请求领取奖励
		CallModuleFunc(ModuleType.MONSTER_STORMCASTLE,"AttackCityTakeBossRewardReq",self.__data.chapter)
	end, Macros.TypefaceSize.button, self.__data.__could_reward and Macros.Color.btn or Macros.Color.forbidden, self.__data.__could_reward and Macros.Color.button_red or Macros.Color.forbidden_stroke)
	self.__button_get:SetVisible(not self.__data.__have_reward)
	self.__button_get:SetEnabled(self.__data.__could_reward)
end

function Modules.TemplMonsterStormCastleRewardCell:ResetData()
	self.__data.__have_reward = true
	self.__image_haveget:SetVisible(self.__data.__have_reward)
	self.__button_get:SetVisible(not self.__data.__have_reward)
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , self.__reward_list )	
end

--------------------------------------------------------------------------------------------
--奖励列表条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.TemplMonsterStormCastleRewardListCell = Modules.TemplMonsterStormCastleRewardListCell or BaseClass(GUI.ScrollItem)

function Modules.TemplMonsterStormCastleRewardListCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplMonsterStormCastleRewardListCell:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,80)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TemplMonsterStormCastleRewardListCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	
	--头像
	if not self.icon then
	    self.icon = LogicGUI.IconLayer.New()
	    self.icon:SetAnchorPoint(0.5,0.5)  
	    self.layout:AddChild(self.icon:GetNode())
	    PosTool.Center(self.icon)
	end
    self.icon:SetData({id = self.__data.id,num = self.__data.num})
end