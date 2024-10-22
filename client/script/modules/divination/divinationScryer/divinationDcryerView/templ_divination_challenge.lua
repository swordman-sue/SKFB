--[[
%% @module: 卡牌挑战界面
%% @author: yjg
%% @created: 2017年11月28日22:02:30
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationNpc = LogicGUI.TemplDivinationNpc or BaseClass(GUI.Template)

LogicGUI.TemplDivinationNpc.TIME = 0

function LogicGUI.TemplDivinationNpc:__init( )
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_NPC) 
    self:__Getwidget()
    self:__ShowUI()
end

function LogicGUI.TemplDivinationNpc:__delete()
end

function LogicGUI.TemplDivinationNpc:__AddToParent()
end

function LogicGUI.TemplDivinationNpc:__Getwidget()
    --npc
    self.Panel_npc = self:GetWidget("Panel_npc")
    --桌子
    self.Image_desk = self:GetWidget("Image_desk")
end

function LogicGUI.TemplDivinationNpc:__ShowUI()
    --npc移动
    self:NpcMove()
end

------------------------------------------------------------------------------------------------------------------------
-- npc
------------------------------------------------------------------------------------------------------------------------

--npc移动
function LogicGUI.TemplDivinationNpc:NpcMove()
    local var = CallModuleFunc(ModuleType.DIVINATION, "GetShowMoveOpen" )
    if var then
        self.Panel_npc:SetPosition( 0 , self.Panel_npc:GetPositionY() )  
    else
        CPPActionManager.MoveTo( self.Panel_npc , LogicGUI.TemplDivinationNpc.TIME, 0 , self.Panel_npc:GetPositionY() ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
            CallModuleFunc(ModuleType.DIVINATION, "SetShowMoveOpen" , true )
        end) )
    end
end

--npc设置
function LogicGUI.TemplDivinationNpc:SetNpc( cardType )
	if not self.npc then
	    self.npc = Util:Sprite( "" )
	    self.npc:SetAnchorPoint( 0.5 , 0 )
	    self.Image_desk:AddChild( self.npc , -1 )
	    self.npc:SetScaleFactor( 0.8 )
	end
	local info = nil
	local index = 1

	--npc
    if cardType == Macros.Game.CARD_TYPE.LOW then
    	info = CallModuleFunc(ModuleType.DIVINATION, "GetNowCollectionInfo" )

        local mapIdx = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )
        local map = CallModuleFunc(ModuleType.DIVINATION, "GetCardGroupMap" , mapIdx )
        index = map.card_group_id 

    elseif cardType == Macros.Game.CARD_TYPE.TALL then
		info = CallModuleFunc(ModuleType.DIVINATION, "GetNowCollectionInfo" ,  #CPPGameLib.GetConfig("CollectionInfo", nil , nil , true ) )
	
		index = #CPPGameLib.GetConfig("CollectionInfo", nil , nil , true )
	end
	self.npc:SetImage( Resource.PathTool.GetHalfHeadPath( info.icon ) )
    self.npc:SetPosition( self.Image_desk:GetContentWidth()/2 + info.deviation , self.Image_desk:GetContentHeight()/2 )

   	if self.ferrule then
		GlobalEffectMgr:DelEffect(self.ferrule)
		self.ferrule = nil
	end
    --转卡
    self.ferrule , con = GlobalEffectMgr:CreateUIEffect( {     
                                                        res_name = "UI_zhanbufanpai" .. index , 
                                                        type = Effect.EffectType.SKELETON , 
                                                        act_index = RenderObj.ActionIndex.IDLE3 ,
                                                    } , self.Image_desk , 10 )
    con:SetPosition( 285 , 240 )

    --挑战
    self:SetChallengeNpc( cardType )

    --喊话
    self:SetNpcShout( cardType )
end

--npc挑战
function LogicGUI.TemplDivinationNpc:SetChallengeNpc( cardType )
    if cardType == Macros.Game.CARD_TYPE.LOW then
		local index = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )
		self.Panel_npc:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_npc, nil, nil, function ( ... )
			local cnf = CallModuleFunc(ModuleType.DIVINATION, "GetCardGroupMap" , index )
			if cnf.is_the_last ~= 1 then
				self:ShowChallenge()
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("manor_nowMaxLevel"))
			end
		end)
    elseif cardType == Macros.Game.CARD_TYPE.TALL then
		self.Panel_npc:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_npc, nil, nil, function ( ... )

		end)
	end
end

--npc喊话
function LogicGUI.TemplDivinationNpc:SetNpcShout( cardType )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION, "sp_divination_bubble")
	if not self.bubble then
		self.bubble = Util:Sprite( sp )
		self.bubble:SetAnchorPoint( 0.5 , 0 )
		self.Image_desk:AddChild( self.bubble)
		self.bubble:SetScaleFactor( 1.2 )
		ActionManager.GetInstance():UpDownMoveAnimation(self.bubble , 1 , 15)
		self.bubble:SetVisible(false)

		self.text = Util:RichText( CPPGameLib.GetString("divination_combat_tips") , Macros.TypefaceSize.normal , 180 )
		self.text:SetAnchorPoint( 0.5 , 1 )
		self.text:SetPosition( self.bubble:GetContentWidth()/2 + 8 , self.bubble:GetContentHeight() - 8 )
		self.bubble:AddChild( self.text)
	end

	local info = nil
	if cardType == Macros.Game.CARD_TYPE.LOW then
		self.bubble:SetVisible(true)
		--战力
	    local index = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )
	    local cnf = CallModuleFunc(ModuleType.DIVINATION, "GetCardGroupMap" , index )
	    local config_gamelevel = CPPGameLib.GetConfig("GameLevel", cnf.map_id )
	    info = CallModuleFunc(ModuleType.DIVINATION, "GetNowCollectionInfo" )
	    --喊话
		local role_battle_value = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.BATTLE_VALUE)
		local text = ""
		if config_gamelevel and config_gamelevel.recommended_power > role_battle_value then
			if info.speak then
				local index = MathTool.GetRandom( 1 , #info.speak )
				text = info.speak[index]
			end
		else
			text = info.speak_challenge
		end
		self.text:SetText( text or " " )
		if not text then
			local index = MathTool.GetRandom( 1 , #info.speak )
			text = info.speak[index]
			self.text:SetText( text )
		end
	elseif cardType == Macros.Game.CARD_TYPE.TALL then
		info = CallModuleFunc(ModuleType.DIVINATION, "GetNowCollectionInfo" ,  #CPPGameLib.GetConfig("CollectionInfo", nil , nil , true ) )
		local index = MathTool.GetRandom( 1 , #info.speak )
		local text = info.speak[index]
		self.text:SetText( text or " " )
	end

	local x , y = 0 , 0
	if info.speak_pos then
		x = info.speak_pos[1]
		y = info.speak_pos[2]
	end
	
	self.bubble:SetPosition( self.Image_desk:GetContentWidth()/2 + x , self.Image_desk:GetContentHeight()/2 + y  )
end


--显示挑战
function LogicGUI.TemplDivinationNpc:ShowChallenge( )
	print("xy" , "ShowChallenge----------------")
	LogicGUI.TemplDivinationChallenge.New()
end


--[[
%% @module: 卡牌挑战界面
%% @author: yjg
%% @created: 2017年11月28日22:02:30
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationChallenge = LogicGUI.TemplDivinationChallenge or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationChallenge:__init( )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__Getwidget()
	    self:__ShowUI()
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_CHALLENGE) 
end

function LogicGUI.TemplDivinationChallenge:__delete()
end

function LogicGUI.TemplDivinationChallenge:__Getwidget()
	--背景
	self.Image_challenge = self:GetWidget("Image_challenge") 
	--下一次
	self.Button_nex = Util:WidgetButton( 
											self:GetWidget("Button_nex") , 
											CPPGameLib.GetString("divination_nexEnd") , 
											CPPGameLib.Handler(self,self.OnButtonClose) ,
											nil,nil, Macros.Color.button_yellow
										) 
	--确定
	self.Button_confirm = Util:WidgetButton( 
												self:GetWidget("Button_confirm") , 
												CPPGameLib.GetString("dungeon_daily_challenge") ,
												CPPGameLib.Handler(self,self.OnButtonConfirm) , 
												nil,nil, Macros.Color.button_yellow
											) 
	--名字
	self.Image_name = self:GetWidget("Image_name") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TemplDivinationChallenge:__ShowUI()
	--当前关卡
	local info = CallModuleFunc(ModuleType.DIVINATION, "GetNowCollectionInfo" )
	CPPGameLib.PrintTable("xy" , info , "请求物品列表")  

	--卡组属性
	local divination_cardPro = Util:Label( CPPGameLib.GetString("divination_cardPro") , Macros.TypefaceSize.normal , Macros.Color.Special )
	divination_cardPro:SetAnchorPoint( 0 , 0.5 )
	divination_cardPro:SetPosition( 385 , 400 )
	self.Image_challenge:AddChild( divination_cardPro )

    local MapIdx = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )
    local cnfMap =  CallModuleFunc(ModuleType.DIVINATION, "GetCardGroupMap" , MapIdx + 1 )
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetCollectionInfo" , cnfMap.card_group_id )

    self.panel = Util:Sprite( Resource.PathTool.GetHalfHeadPath( info.icon ) )
    self.panel:SetAnchorPoint( 0.5 , 0 )
    self.panel:SetPosition( self.Image_name:GetContentWidth()/2 , self.Image_name:GetContentHeight()/2 + 20 )
    self.Image_name:AddChild( self.panel , -1 )
    self.panel:SetScaleFactor( 0.8 )

	local mapIdx = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )

    local sp = Resource.PathTool.GetUIResPath("divinationbig/sp_divination_bottom"..mapIdx)
    local bubble = Util:Sprite( sp )
    bubble:SetAnchorPoint( 0.5 , 0 )
    bubble:SetPosition( self.Image_name:GetContentWidth()/2 , self.Image_name:GetContentHeight()/2 - 50 )
    self.Image_name:AddChild( bubble , -2 )
   
	--当前关卡信息
	local info = CallModuleFunc(ModuleType.DIVINATION, "GetCardGroupMap" , mapIdx )
	
	local describe = Util:RichText( info.describe , Macros.TypefaceSize.normal , 450 )
	describe:SetAnchorPoint( 0 , 1 )
	describe:SetPosition( 385 , 385 )
	self.Image_challenge:AddChild( describe )

	--战斗奖励
	local common_reward = Util:Label( CPPGameLib.GetString("battle_result_common_reward_title2") , Macros.TypefaceSize.normal , Macros.Color.Special )
	common_reward:SetAnchorPoint( 0 , 0.5 )
	common_reward:SetPosition( 385 , 310 )
	self.Image_challenge:AddChild( common_reward )

	--奖励图标
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", info.map_id)	
	for i , v in ipairs( config_gamelevel.reward_item_id_list ) do
	    --头像
	    local icon = LogicGUI.IconLayer.New(true)
	    icon:SetData({ id = v , num = config_gamelevel.reward_item_num_min_list[i] })
	    icon:SetAnchorPoint( 0 , 1 )
	    icon:SetPosition( 385 + ( (i-1) * 85 ) , 290 )
	    icon:SetScaleFactor( 0.9 )
	    self.Image_challenge:AddChild(icon:GetNode())
	end

	--名字
	local name = Util:Label( info.name , Macros.TypefaceSize.normal , Macros.Color.Special )
	name:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_name:AddChild( name )
	PosTool.Center( name )
end

--关闭
function LogicGUI.TemplDivinationChallenge:OnButtonClose()
	self:DeleteMe()
end

--确定
function LogicGUI.TemplDivinationChallenge:OnButtonConfirm()
	local checkBattle , map_id = CallModuleFunc(ModuleType.DIVINATION, "CheckBattleCondition" )
	if checkBattle then
		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.DIVINATION , map_id )
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("manor_nowMaxLevel"))
	end
end