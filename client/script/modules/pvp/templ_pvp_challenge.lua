--
-- @brief PVP挑战
-- @author: yjg
-- @date: 2017年6月21日14:32:15
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPvpChallenge = LogicGUI.TemplPvpChallenge or BaseClass(GUI.Template)

function LogicGUI.TemplPvpChallenge:__init( )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_PVP_CHALLENGE)	

	self:BindGlobalEvent(Macros.Event.BattleModule.PVP_SEARCH_TARGET_RESP, function ( _, data )
		self:RandomHeroInfo()
	end)

	self:BindGlobalEvent(Macros.Event.BattleModule.PVP_READY_NOTIFY, function ( _, data )
		self:HeHeroInfo(data)
	end)

	self:BindGlobalEvent(Macros.Event.PVPModule.PVP_CANCEL, function ( _, data )
		self:DeleteMe()
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("pvp_cancel_") )
	end)
end

function LogicGUI.TemplPvpChallenge:__delete()
end

function LogicGUI.TemplPvpChallenge:__Getwidget()

	self.Panel_6 = self:GetWidget("Panel_6")
	--左边
	self.Panel_left = self:GetWidget("Panel_left")
	--左边英雄
	self.Panel_left_hero = self:GetWidget("Panel_left_hero")

	--右边
	self.Panel_right = self:GetWidget("Panel_right")
	--右边英雄
	self.Panel_right_hero = self:GetWidget("Panel_right_hero")
	--vs
	self.Panel_vs = self:GetWidget("Panel_vs")
end

function LogicGUI.TemplPvpChallenge:__ShowUI()
	self:MyHeroInfo()
	self:PlayActionTimeline("animation0", false, function()
		-- CallModuleFunc(ModuleType.BATTLE, "SearchTargetReq_PVP")
	end)

	self.sp_countdown_bg = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "sp_countdown_bg"))
	self.sp_countdown_bg:SetAnchorPoint( 0.5 , 0.5 )
	self.sp_countdown_bg:SetPosition( self.Panel_vs:GetPositionX() , self.Panel_vs:GetPositionY() )
	self.Panel_6:AddChild( self.sp_countdown_bg )
	self.sp_countdown_bg:SetVisible( false )

	local pvp_matching = Util:Label( CPPGameLib.GetString("pvp_matching") , Macros.TypefaceSize.normal , Macros.Color.golden )
	pvp_matching:SetAnchorPoint( 0.5 , 0.5 )
	pvp_matching:SetPosition( self.sp_countdown_bg:GetContentWidth()/2 , self.sp_countdown_bg:GetContentHeight()/2 + 18 )
	self.sp_countdown_bg:AddChild( pvp_matching )

	self.pvp_matching = Util:Label( "00:00" , Macros.TypefaceSize.normal , Macros.Color.golden )
	self.pvp_matching:SetAnchorPoint( 0.5 , 0.5 )
	self.pvp_matching:SetPosition( self.sp_countdown_bg:GetContentWidth()/2 , self.sp_countdown_bg:GetContentHeight()/2 - 18 )
	self.sp_countdown_bg:AddChild( self.pvp_matching )

	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
	self.btnSell  =  Util:Button(
									imgdata,
									CPPGameLib.Handler(self, self.OnBtnSell),
									CPPGameLib.GetString("pvp_cancel"),
									Macros.TypefaceSize.button,
									nil ,
									Macros.Color.button_yellow 
								)
	self.btnSell:SetPosition( self.Panel_6:GetContentWidth()/2 , 60 )
	self.Panel_6:AddChild(self.btnSell) 
end

--我的信息
function LogicGUI.TemplPvpChallenge:MyHeroInfo()
	local roleInfo = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	--左边
	local panel_left = Util:Sprite( Resource.PathTool.GetHalfHeadPath( roleInfo.head_icon ) )
	panel_left:SetAnchorPoint( 0.5 , 0 )
	panel_left:SetPosition( self.Panel_left_hero:GetContentWidth()/2 , 0 )
	self.Panel_left_hero:AddChild( panel_left )

	 --战斗力
    local Fighting = Util:Layout( 50 , 20 )
    Fighting:SetAnchorPoint( 0.5, 0.5 )
    Fighting:SetPosition( self.Panel_left_hero:GetContentWidth()/2 , 0 )
    self.Panel_left_hero:AddChild(Fighting)

    --战斗力图样
    local scale = 0.7

    local battle_powerSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "battle_value_big", true)
    local battle_power = Util:Sprite(battle_powerSp)
    battle_power:SetScaleFactor( scale )
    battle_power:SetContentSize( battle_power:GetContentWidth() * scale , battle_power:GetContentHeight() * scale )
    battle_power:SetAnchorPoint( 0 , 0.5)

    local power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "battle_num", true))
    power_font:SetText(tostring(roleInfo.battle_value))
    power_font:SetScaleFactor( scale )
    power_font:SetContentSize( power_font:GetNode():GetContentWidth() * scale , power_font:GetNode():GetContentHeight() * scale )
    power_font:SetAnchorPoint(0, 0.5)

    Fighting:SetContentSize( battle_power:GetContentWidth() + 5 + power_font:GetNode():GetContentWidth() , power_font:GetNode():GetContentHeight() )
    
    battle_power:SetPosition(0 , Fighting:GetContentHeight()/2)
    Fighting:AddChild(battle_power)

    Fighting:AddChild( power_font:GetNode() )
    PosTool.RightTo( battle_power , power_font , 5 )

    --名字
    local name = Util:Label( roleInfo.role_name )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( Fighting:GetContentWidth()/2 , 0 - Fighting:GetContentHeight() )
    Fighting:AddChild( name )
end

--随机信息
function LogicGUI.TemplPvpChallenge:RandomHeroInfo()
	--假装图标
	self.right = Util:Sprite( "" )
	self.right:SetAnchorPoint( 0.5 , 0 )
	self.right:SetFlipX(true)
	self.right:SetPosition( self.Panel_right_hero:GetContentWidth()/2 , 0 )
	self.Panel_right_hero:AddChild( self.right )

	-- 战斗力
    self.Fighting = Util:Layout( 50 , 20 )
    self.Fighting:SetAnchorPoint( 0.5, 0.5 )
    self.Fighting:SetPosition( self.Panel_right_hero:GetContentWidth()/2 , 0 )
    self.Panel_right_hero:AddChild(self.Fighting)


    --战斗力图样
    local scale = 0.7

    local battle_powerSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "battle_value_big", true)
    self.battle_power = Util:Sprite(battle_powerSp)
    self.battle_power:SetScaleFactor( scale )
    self.battle_power:SetContentSize( self.battle_power:GetContentWidth() * scale , self.battle_power:GetContentHeight() * scale )
    self.battle_power:SetAnchorPoint( 0 , 0.5)
    self.Fighting:AddChild(self.battle_power)
    self.battle_power:SetVisible(false)

    self.power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "battle_num", true))
    self.power_font:SetText(tostring(00000))
    self.power_font:SetScaleFactor( scale )
    self.power_font:SetContentSize( self.power_font:GetNode():GetContentWidth() * scale , self.power_font:GetNode():GetContentHeight() * scale )
    self.power_font:SetAnchorPoint(0, 0.5)
    self.Fighting:AddChild( self.power_font:GetNode() )
    self.power_font:SetVisible(false)

    --名字
    self.name = Util:Label( " " )
    self.name:SetAnchorPoint( 0.5 , 0.5 )
    self.name:SetPosition( self.Fighting:GetContentWidth()/2 , 0 - self.Fighting:GetContentHeight() )
    self.Fighting:AddChild( self.name )

    --区
    self.district = Util:Label( " " )
    self.district:SetAnchorPoint( 0.5 , 0.5 )
    self.district:SetPosition( self.Fighting:GetContentWidth()/2 , self.name:GetPositionY() - self.Fighting:GetContentHeight() )
    self.Fighting:AddChild( self.district )

    -- local m = 0
    -- local ms = 0
	CPPActionManager.DelayTo( self.Panel_right_hero , 0.01 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		local func = nil
		local function moveAnimate( ... )
			local icon_ , name_ = CallModuleFunc(ModuleType.PVP, "GetRandomInfo")
			self.right:SetImage(Resource.PathTool.GetHalfHeadPath( icon_ ))
			self.name:SetString( name_ )
			CPPActionManager.DelayTo( self.Panel_right_hero , 0.01 , func)
		end
		func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
		moveAnimate()
	end))

	local ms = 0
	local m = 0
	CPPActionManager.DelayTo( self.Panel_right_hero , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		local func = nil
		local function moveAnimate( ... )
			ms = ms + 1
			if ms == 60 then
				ms = 0 
				m = m + 1
			end
			self.pvp_matching:SetString( string.format("%02d", m) ..":" .. string.format("%02d", ms) )
			CPPActionManager.DelayTo( self.Panel_right_hero , 1 , func)
			self.sp_countdown_bg:SetVisible( true )
		end
		func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
		moveAnimate()
	end))
end

--他的信息
function LogicGUI.TemplPvpChallenge:HeHeroInfo( data )
	CPPActionManager.StopAllActions(self.Panel_right_hero)
	self.sp_countdown_bg:SetVisible( false )

	self.right:SetImage(Resource.PathTool.GetHalfHeadPath( data.head_icon ))

	self.name:SetString( data.name )

	self.power_font:SetText(tostring( data.fighting_power ))
    self.Fighting:SetContentSize( self.battle_power:GetContentWidth() + 5 + self.power_font:GetNode():GetContentWidth() , self.power_font:GetNode():GetContentHeight() )
    self.battle_power:SetPosition(0 , self.Fighting:GetContentHeight()/2)
	PosTool.RightTo( self.battle_power , self.power_font , 5 )

	self.power_font:SetVisible(true)
	self.battle_power:SetVisible(true)
    
	self.name:SetPosition( self.Fighting:GetContentWidth()/2 , 0 - self.Fighting:GetContentHeight() )
	self.district:SetPosition( self.Fighting:GetContentWidth()/2 , self.name:GetPositionY() - self.Fighting:GetContentHeight() )

	CallModuleFunc(ModuleType.PVP,"SaveRandomInfo" , data.name )
	
	local clipping_node_dialog = ClippingNodeEx.Node()
	clipping_node_dialog:SetAlphaTestValue(0)
	clipping_node_dialog:SetStencilMask(self.right)
	clipping_node_dialog:SetAnchorPoint( 0.5 , 0 )
	clipping_node_dialog:SetPosition( self.Panel_right_hero:GetContentWidth()/2 , 0 )
	self.Panel_right_hero:AddChild( clipping_node_dialog )

	local effect_id , effect = self:__CreateEffect( {res_name = "UI_pvpxuanzhong" } , clipping_node_dialog )
	effect:SetPosition( clipping_node_dialog:GetContentWidth()/2 , 261 )

	self:PlayActionTimeline("animation1", false, function()
	end)
end

--取消匹配
function LogicGUI.TemplPvpChallenge:OnBtnSell( data )
	CallModuleFunc(ModuleType.PVP,"PVPCancelSearchTargetReq" )
end
