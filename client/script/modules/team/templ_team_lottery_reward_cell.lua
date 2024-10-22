GUI = GUI or {}
GUI.TemplTeamLotteryRewardCell = GUI.TemplTeamLotteryRewardCell or BaseClass(GUI.Template)

function GUI.TemplTeamLotteryRewardCell:__init(reward_type,coin_type)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TEAM_LOTTERY_REWARD_CELL)
	self:CreateMainUI(reward_type,coin_type)
end

function GUI.TemplTeamLotteryRewardCell:__AddToParent()

end

function GUI.TemplTeamLotteryRewardCell:CreateMainUI(reward_type,coin_type)
	if not reward_type then
		return
	end
	--奖励类型
	Util:WidgetLabel(self:GetWidget("Text_stage"),CPPGameLib.GetString("team_lottery_record_type"..reward_type),Macros.TypefaceSize.tab,Macros.Color.headline )
	--计算行数
	local rowNum = 0
	local type_num = 0
	self.__data_list = {}
	local reward_table
	if coin_type then
		local config_tag = CPPGameLib.GetConfig("NDLotteryReward",coin_type)
		if config_tag then
			reward_table = config_tag.item_list
		end
	else
		reward_table = CPPGameLib.GetConfig("TeamDungeonLotteryReward",nil,nil,true)
	end
	if reward_table then
		for i,reward_info in ipairs(reward_table) do
			if reward_info.type == reward_type then
				type_num = type_num + 1
				table.insert(self.__data_list,{item_id = reward_info.item_id,num = reward_info.num})
			end
		end
		rowNum = math.ceil(type_num/5)
		local scrollviewparams = {
			item_class = Modules.TeamDungeonLotteryRewardItem,
	    	item_width = 80,
	    	item_height	= 80,
	    	item_space_c = 44,
	    	item_space_r = 10,
	    	row	= rowNum,
	    	col	= 5,
		}

		--顶部控件
		local panel_top = self:GetWidget("Panel_top")
		--背景图
		local image_bg = self:GetWidget("Image_bg")
		--滚动容器
		local panel_scrollview = self:GetWidget("Panel_ScrollView")
		self.__reward_scrollview = GUI.ScrollView.New(scrollviewparams)					
		panel_scrollview:AddChild(self.__reward_scrollview:GetNode())			
		self.__reward_scrollview:SetDataList(self.__data_list)
		--根据高度适配
		local height = self.__reward_scrollview:GetViewHeight()
		panel_scrollview:SetContentSize(576,height)	
		PosTool.Center(self.__reward_scrollview)	
		--根节点设置大小
		self:GetNode():SetContentSize(626,height+82)
		--背景图设置大小
		image_bg:SetContentSize(603,height+20)
		PosTool.CenterTop(image_bg,0,-51)
		--设置位置
		PosTool.LeftTop(panel_scrollview,25,-61)
		PosTool.CenterTop(panel_top)	
	end
end

function GUI.TemplTeamLotteryRewardCell:__delete()
	if self.__reward_scrollview then
		self.__reward_scrollview:DeleteMe()
		self.__reward_scrollview = nil
	end
end

--------------------------------------------------------------------------------------------
--抽奖奖励条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.TeamDungeonLotteryRewardItem = Modules.TeamDungeonLotteryRewardItem or BaseClass(GUI.ScrollItem)

function Modules.TeamDungeonLotteryRewardItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TeamDungeonLotteryRewardItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,80)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TeamDungeonLotteryRewardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)	
	--头像
	if not self.icon then
	    self.icon = LogicGUI.IconLayer.New()
	    self.icon:SetAnchorPoint(0.5,0.5)  
	    self.layout:AddChild(self.icon:GetNode())
	    PosTool.Center(self.icon)
	end
    self.icon:SetData({id = self.__data.item_id,num = self.__data.num})
end