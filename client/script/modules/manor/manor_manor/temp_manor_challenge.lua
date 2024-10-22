
--[[
%% @module: 领地挑战
%% @author: yjg
%% @created: 2016年10月8日20:29:00
--]]

Modules = Modules or {}

Modules.TempManorChallenge = Modules.TempManorChallenge or BaseClass(GUI.Template)

function Modules.TempManorChallenge:__init(id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.id = id
		self:__Getwidget()
		self:__ShowUI()
		-- self.index = index
	end
	self:InitTemplate(GUI.TemplateRegistry.MANOR_CHALLENGE)	
end


function Modules.TempManorChallenge:__delete()
	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end
	if self.modes then
		self.modes:DeleteMe()
		self.modes = nil
	end
end

function Modules.TempManorChallenge:__Getwidget()

	--居中
	self.Image_BG = self:GetWidget("Image_BG") 
	--标题
	self.Image_light = self:GetWidget("Image_light") 
	--背景
	self.Image_setting = self:GetWidget("Image_setting") 
	--内容框
	self.Image_controlPanel = self:GetWidget("Image_controlPanel") 
	--说明
	self.Image_award = self:GetWidget("Image_award") 
	--条目
	self.Image_1 = self:GetWidget("Image_1") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--icon
	self.Image_iconBg = self:GetWidget("Image_iconBg") 
	--战斗力
	self.Text_power = self:GetWidget("Text_power") 
	--关闭
	self.Button_close = Util:WidgetButton(
											self:GetWidget("Button_close") , 
											nil , 
											CPPGameLib.Handler(self,self.OnButtonClose)
											)
	--挑战
	self.Button_challenge = Util:WidgetButton(
												self:GetWidget("Button_challenge") , 
												CPPGameLib.GetString("tower_challenge") , 
												CPPGameLib.Handler(self,self.OnButtonChallenge),
												nil,nil, Macros.Color.button_yellow )
end

function Modules.TempManorChallenge:__ShowUI()
	--领地表
	local landCityCnf = CPPGameLib.GetConfig("LandCity", self.id)
	--关卡表
	local gameLevelCnf = CPPGameLib.GetConfig("GameLevel", landCityCnf.map_id)
	--获得怪物的信息
	local monsterCnf = CPPGameLib.GetConfig("Monster", gameLevelCnf.icon_id )

	--标题
	local light = Util:Label( landCityCnf.name , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

-----------
	self.Image_setting:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR_BG, landCityCnf.build_bg ) )
	
	--怪物地光
	local bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
	bg_shadow:SetPosition( 100 , 45 )
	self.Image_setting:AddChild(bg_shadow)

	--怪物模型
	self.modes = LogicGUI.Model.New({
										info_id = gameLevelCnf.icon_id , 
										parent = bg_shadow ,
										model_type = LogicGUI.ModelType.MONSTER 
									})
	self.modes:SetPosition( bg_shadow:GetContentWidth()/2 , bg_shadow:GetContentHeight()/2 )

	--名字
	local name = Util:Name(monsterCnf.name , Macros.TypefaceSize.normal , monsterCnf.quality)
	name:SetAnchorPoint(0.5,0.5)
	name:SetPosition(bg_shadow:GetContentWidth()/2 , 0 )
	bg_shadow:AddChild(name)

-----------
	--遗迹说明
	local manor_referral = Util:Label( CPPGameLib.GetString("manor_referral") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	manor_referral:SetAnchorPoint(0,0.5)
	manor_referral:SetPosition( 10 , self.Image_award:GetContentHeight()/2  )
	self.Image_award:AddChild(manor_referral)

	--描述
	local desc = Util:Label( landCityCnf.desc , Macros.TypefaceSize.normal , Macros.Color.content , self.Image_controlPanel:GetContentWidth() - 20 )
	desc:SetAnchorPoint(0,1)
	desc:SetPosition( 10 , self.Image_award:GetPositionY() - (self.Image_award:GetContentHeight()/2) - 10 )
	self.Image_controlPanel:AddChild(desc)	

-----------
	--胜利奖励
	local manor_winAward = Util:Label( CPPGameLib.GetString("manor_winAward") , Macros.TypefaceSize.normal , Macros.Color.headline )
	manor_winAward:SetAnchorPoint(0,0.5)
	manor_winAward:SetPosition( 0 , self.Text_name:GetContentHeight()/2  )
	self.Text_name:AddChild(manor_winAward)

	local value = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.BATTLE_VALUE )
	local color = Macros.Color.red_color
	if value > gameLevelCnf.recommended_power then
		color = Macros.Color.front_color
	end

	--战斗力
	local manor_recommendFighting = Util:Label( 	
													CPPGameLib.GetString("manor_recommendFighting" , gameLevelCnf.recommended_power or 0 ) , 
													Macros.TypefaceSize.normal , 
													color  
												)
	manor_recommendFighting:SetAnchorPoint( 1 , 0.5 )
	manor_recommendFighting:SetPosition( self.Text_power:GetContentWidth() , self.Text_power:GetContentHeight()/2 )
	self.Text_power:AddChild( manor_recommendFighting )

-----------
	local reward = ConfigAdapter.GameLevel.GetRewardObjList(landCityCnf.map_id)

    local params =  
    {
        item_class = Modules.IconItem ,
        item_width = (self.Image_iconBg:GetContentWidth()-10) /4 ,
        item_height = self.Image_iconBg:GetContentHeight() ,
        row = 1,
        col = 4,
        view_width = self.Image_iconBg:GetContentWidth() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_iconBg:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

	self.lst_list:SetDataList(reward)
end

function Modules.TempManorChallenge:OnButtonClose( )
	self:DeleteMe()
end

function Modules.TempManorChallenge:OnButtonChallenge( )
	local checkBattle = CallModuleFunc(ModuleType.MANOR, "CheckBattleCondition")
	if checkBattle then
		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.MANOR_DARE, self.id , 0)
	end
end


Modules = Modules or {}

Modules.TempManorChallengeItem = Modules.TempManorChallengeItem or BaseClass(GUI.ScrollItem)

function Modules.TempManorChallengeItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TempManorChallengeItem:__delete()
end

function Modules.TempManorChallengeItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(80,80)
    self.layout:SetAnchorPoint(0.5,0.5)
    return self.layout
end

function Modules.TempManorChallengeItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

	local icon = LogicGUI.IconLayer.New(true)
	icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight()/2)
	icon:SetScaleFactor(0.9)
	self.layout:AddChild(icon:GetNode())

	local iconData = { id = data.id , num = data.num }
	icon:SetData(iconData)

end

function Modules.TempManorChallengeItem:__AddToParent()

end
