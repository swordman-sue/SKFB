LogicGUI = LogicGUI or {}

LogicGUI.TemplPreLookView = LogicGUI.TemplPreLookView or BaseClass(GUI.Template)

function LogicGUI.TemplPreLookView:__init(preLooktype)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initBaseUI(preLooktype)
	end
	self:InitTemplate(GUI.TemplateRegistry.HERO_RECRUIT_PRELOOK)
end

function LogicGUI.TemplPreLookView:__delete()
	if self._prLookScrollview then
		self._prLookScrollview:DeleteMe()
		self._prLookScrollview = nil
	end	
end

function LogicGUI.TemplPreLookView:initBaseUI(preLooktype)
	local heroList = {}
	if preLooktype == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then
		heroList = CPPGameLib.GetConfig("HeroLowRecruit",nil,nil,true)
		--标题
		Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_lowrecruitPreLookTitle"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, nil, Macros.Color.shadow_color)
	else
		heroList = CPPGameLib.GetConfig("HeroHighRecruit",nil,nil,true)
		--标题
		Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_highrecruitPreLookTitle"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, nil, Macros.Color.shadow_color)
	end	
	local heroMessageList = {}
	for hero_id,vip in pairs(heroList) do		
		local data = {}
		local config = CPPGameLib.GetConfig("Hero",hero_id)
		data.hero_id = hero_id
		data.init_quality = config.init_quality
		table.insert(heroMessageList,data)
	end
	--排序
	table.sort(heroMessageList,function (hero_1,hero_2)
		if hero_1.init_quality ~= hero_2.init_quality then
			return hero_1.init_quality > hero_2.init_quality
		else
			return hero_1.hero_id < hero_2.hero_id
		end
	end)
	--关闭按钮
	local buttonClose = self:GetWidget("Button_close")
	WidgetUnity.SetWidgetListener(buttonClose, nil, nil, function()
		self:DeleteMe()
	end)

	local params = {
		item_class = Modules.TemplRecruitPreLookCell,
		item_width = 100,
		item_height = 100,
		col = 5,
		row = 4 ,
		item_space_r = 9,
    	item_space_c = 15,
    	view_height = 427,
    	-- view_width = 560,
	}
	self._prLookScrollview = GUI.ScrollView.New(params)
	local scrollViewPanel = self:GetWidget("Panel_preLook")
	scrollViewPanel:AddChild(self._prLookScrollview:GetNode())
	PosTool.Center(self._prLookScrollview)
	self._prLookScrollview:SetDataList(heroMessageList)	
end


--------------------------------------------------------------------------------------------
--[[
	头像条目
]]
Modules = Modules or {}
Modules.TemplRecruitPreLookCell = Modules.TemplRecruitPreLookCell or BaseClass(GUI.ScrollItem)

function Modules.TemplRecruitPreLookCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplRecruitPreLookCell:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout( 100 , 99 )
	self.layout:SetTouchEnabled(false)
	self.layout:SetClippingEnable(false)
	self.layout:SetBackGroundColor(0, 0, 0)
	return self.layout
end

function Modules.TemplRecruitPreLookCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--头像
    self.icon = LogicGUI.IconLayer.New()
    self.icon:SetAnchorPoint(0.5,1)  
    self.layout:AddChild(self.icon:GetNode())
    PosTool.CenterTop(self.icon)  
    self.icon:SetData({id = data.hero_id})

    self.icon:ShowHeroName( true ,  0 , 0 )
end

