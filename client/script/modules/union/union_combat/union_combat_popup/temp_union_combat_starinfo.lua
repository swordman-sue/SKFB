--
-- @brief 工会战（星球）
-- @author: yjg
-- @date: 2016年10月14日10:39:24
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatStarInfo = LogicGUI.TempUnionCombatStarInfo or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatStarInfo:__init( id )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID}	
    self:InitTemplate(nil)  
	self.id = id
	self:__Getwidget()
	self:__Server() 
end

function LogicGUI.TempUnionCombatStarInfo:__delete( )
end

function LogicGUI.TempUnionCombatStarInfo:__Server( )
	local info = {}
	info.star_id = self.id
	CallModuleFunc(ModuleType.UNION , "GuildWarStarDetailReq" , info )

	self.isNew = false
	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_STAR_INFO , function (_, data)
		if self.isNew == false then
			self.isNew = true
			self:__ShowUI() 
		end
	end)

	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_OCCUPY_AWARD , function (_, data)
	    --开采设置
	    self:SetExploit( self.exploit )
	end)

end

function LogicGUI.TempUnionCombatStarInfo:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function( ... )
    	self:DeleteMe()
    end)
    return self.layout
end

function LogicGUI.TempUnionCombatStarInfo:__Getwidget( )
	--背景
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_info")
	self.bg = Util:Sprite( sp )
	self.bg:SetAnchorPoint( 0.5 , 0.5 )
	self.layout:AddChild( self.bg )
	PosTool.Center( self.bg )

	--过滤退出
	local touchEnd = Util:Layout( 700 , 400 , function( ... )
		-- body
	end)
	touchEnd:SetAnchorPoint( 0.5 , 0.5 )
	touchEnd:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight()/2 )
	self.bg:AddChild( touchEnd )
	
	local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , self.id )
	--星球
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_STAR, starCnf.sp[1])
	self.star = Util:Sprite( sp )
	self.star:SetAnchorPoint( 0.5 , 0.5 )
	self.star:SetPosition( self.bg:GetContentWidth()/2 - 180 , self.bg:GetContentHeight()/2 + 20 )
	self.star:SetScaleFactor( 0.8 )
	self.bg:AddChild( self.star )

	--名字
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_name_frame_" .. starCnf.star_type )
	local sp_name_frame = Util:Sprite( sp )
	sp_name_frame:SetAnchorPoint( 0.5 , 0.5 )
	sp_name_frame:SetPosition( self.bg:GetContentWidth()/2 + 120  , self.bg:GetContentHeight()/2 + 155 )
	self.bg:AddChild( sp_name_frame )
	sp_name_frame:SetScaleFactor( 2 )

	local name = Util:Name( starCnf.name , Macros.TypefaceSize.tab , starCnf.star_type )
	name:SetAnchorPoint( 0.5 , 0.5 )
	name:SetPosition( sp_name_frame:GetPositionX() , sp_name_frame:GetPositionY() )
	self.bg:AddChild( name )

	--当前占领军团
	local union_combat_current = Util:Label( CPPGameLib.GetString("union_combat_current") )
	union_combat_current:SetAnchorPoint( 0 , 0.5 )
	union_combat_current:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight()/2 + 90 )
	self.bg:AddChild( union_combat_current )

	--暂无
	self.union_combat_pitera = Util:Label( "" )
	self.bg:AddChild( self.union_combat_pitera )
	PosTool.RightTo( union_combat_current , self.union_combat_pitera )

	--星球特产
	local union_combat_specialty = Util:Label( CPPGameLib.GetString("union_combat_specialty") )
	union_combat_specialty:SetAnchorPoint( 0 , 0.5 )
	union_combat_specialty:SetPosition( union_combat_current:GetPositionX() , union_combat_current:GetPositionY() - 30 )
	self.bg:AddChild( union_combat_specialty )

	--特产背景
	self.specialty_bg = Util:Layout( 400 , 50 )
	self.specialty_bg:SetAnchorPoint( 0 , 0.5 )
	self.specialty_bg:SetPosition( self.bg:GetContentWidth()/2 + 20 , self.bg:GetContentHeight()/2 + 5 )
	self.bg:AddChild( self.specialty_bg )
    
    if starCnf.local_product_id_list or starCnf.daily_reward_id_list then

    	local lst = starCnf.local_product_id_list or starCnf.daily_reward_id_list
    	local num = starCnf.local_product_num_list or starCnf.daily_reward_num_list

    	self.specialty_bg:SetContentSize(#lst * 100 , 90 )
	    local interval = self.specialty_bg:GetContentWidth() / #lst
	    for i , v in ipairs( lst ) do
	    	local icon = LogicGUI.IconLayer.New(true)
	    	icon:SetData( { id = v , num = num[i] } )
	    	icon:SetPosition( (interval/2) + interval * ( i-1 ) , self.specialty_bg:GetContentHeight()/2 )
	   		self.specialty_bg:AddChild( icon:GetNode() )
	   		icon:SetScaleFactor( 0.8 )
	    end
	end

    --开采
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big")
    self.exploit = Util:Button( sp , function( ... )
    	self:OnExploit()
    end , CPPGameLib.GetString("union_combat_exploit") , Macros.TypefaceSize.normal , nil , Macros.Color.blue_stroke )
    self.exploit:SetAnchorPoint( 0.5 , 0.5 )
    self.exploit:SetPosition( sp_name_frame:GetPositionX() , self.bg:GetContentHeight()/2 - 60 )
    self.bg:AddChild( self.exploit )
    --开采设置
    self:SetExploit( self.exploit )

    --撤离
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big")
    self.evacuate = Util:Button( sp , function( ... )
    	self:OnEvacuate()
    end , CPPGameLib.GetString("union_combat_evacuate") , Macros.TypefaceSize.normal , nil , Macros.Color.blue_stroke )
    self.evacuate:SetAnchorPoint( 0.5 , 0.5 )
    self.evacuate:SetPosition( self.exploit:GetPositionX() + 180 , self.bg:GetContentHeight()/2 - 60 )
    self.bg:AddChild( self.evacuate )
    self.evacuate:SetScaleFactor( 0.8 )
    self.evacuate:SetVisible( false )

    --查看
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_check")
   	self.btn_check = Util:Button( sp , function( ... )
   		-- LogicGUI.TempUnionCombatStarInfo.New()
   		self:OnBtnCheck()
   	end )
   	self.btn_check:SetAnchorPoint( 0.5 , 0.5 )
   	self.btn_check:SetPosition( 400 , 120 )
   	self.btn_check:SetVisible(false)
   	self.bg:AddChild( self.btn_check )

end

function LogicGUI.TempUnionCombatStarInfo:__ShowUI( )
	local type_ = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarLevel" , self.id )
	if type_ == "low" then
		self:__SetLow()
	elseif type_ == "tall" then
		self:__SetTall()
	end
end

--设置低级
function LogicGUI.TempUnionCombatStarInfo:__SetLow( )
	--vs
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_black")
	local sp_vs = Util:Sprite( sp )
	sp_vs:SetAnchorPoint( 0.5 , 0.5 )
	sp_vs:SetPosition( self.star:GetPositionX() , 180 )
	sp_vs:SetVisible( false )
	self.bg:AddChild( sp_vs )

	--进入战场
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_blue")
   	local btn_blue = Util:Button( sp , function( ... )

   		local checkBattle = CallModuleFunc(ModuleType.UNION , "CheckBattleCondition" , Macros.Game.FBType.UNION_LOW )
   		if checkBattle then
	   		--请求战斗(工会战低级星球)
	   		CallModuleFunc(ModuleType.BATTLE,"BattleStartReq",Macros.Game.FBType.UNION_LOW,self.id)
   		else
   			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("heroShop_noResidue") )
   		end

   	end , CPPGameLib.GetString("union_combat_enterInto") , Macros.TypefaceSize.button , Macros.Color.btn , Macros.Color.blue_stroke )
   	btn_blue:SetAnchorPoint( 0.5 , 0.5 )
   	btn_blue:SetPosition( self.star:GetPositionX() , self.btn_check:GetPositionY() )
   	btn_blue:SetVisible( false )
   	self.bg:AddChild( btn_blue )

   	--攻打倒计时
   	local time = string.format( Macros.Color.green_rt , "00:00:00" )
   	self.union_combat_Countdown = Util:RichText( CPPGameLib.GetString("union_combat_Countdown" , time ) , Macros.TypefaceSize.slightly )
   	self.union_combat_Countdown:SetAnchorPoint( 0.5 , 0.5 )
   	self.union_combat_Countdown:SetPosition( sp_vs:GetContentWidth()/2 , sp_vs:GetContentHeight()/2 + 13 )
	sp_vs:AddChild( self.union_combat_Countdown )

	--今日剩余次数
	local num = string.format( Macros.Color.green_rt , CallModuleFunc(ModuleType.UNION , "GetUnionCombatLowTime" ) )
	local divination_remainTime = Util:RichText( CPPGameLib.GetString("divination_remainTime" ) .. num , Macros.TypefaceSize.slightly )
	divination_remainTime:SetAnchorPoint( 0.5 , 0.5 )
	divination_remainTime:SetPosition( sp_vs:GetContentWidth()/2 , sp_vs:GetContentHeight()/2 - 13 )
	sp_vs:AddChild( divination_remainTime )

	local type_ , endTime = CallModuleFunc( ModuleType.UNION , "UnionCombatIntegralStarTime" )
	--开始中
	if type_ == "begin" then
		sp_vs:SetVisible( true )
		btn_blue:SetVisible( true )
		self.btn_check:SetVisible( true )
		--倒计时
		self:Countdown( endTime )
	--领奖中
	elseif type_ == "award" then
		sp_vs:SetVisible( false )
		btn_blue:SetVisible( false )
		self.btn_check:SetVisible( false )
	else
		sp_vs:SetVisible( false )
		btn_blue:SetVisible( false )
		self.btn_check:SetVisible( false )
	end
	
	local starDetail = CallModuleFunc( ModuleType.UNION , "GetUnionCombatStarDetail")
	if starDetail.belong_guild_name ~= "" then
		self.union_combat_pitera:SetString( starDetail.belong_guild_name )
	else
		self.union_combat_pitera:SetString( CPPGameLib.GetString("union_combat_pitera") )
	end

end


--设置高级
function LogicGUI.TempUnionCombatStarInfo:__SetTall( )
	--ui设置
	local type_ = CallModuleFunc( ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )
	local duiZhanKey = false
	local xuanZhanKey = false
	local kaiCaiKey = false
	for i , v in ipairs( type_ ) do
		if v == "duiZhan" then
			duiZhanKey = true
		elseif v == "xuanZhan" then
			xuanZhanKey = true
		elseif v == "kaiCai" then
			kaiCaiKey = true
		end
	end

	--攻击
	if duiZhanKey then
		self:SetCombat()
	--宣战
	elseif xuanZhanKey then
		self.btn_check:SetVisible( true )
		self:SetDeclareWar( )
	-- 布防
	elseif kaiCaiKey then
		local occupyInfo = CallModuleFunc( ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.id )
		if occupyInfo then
			self.btn_check:SetVisible( true )
			self:SetDeclareWar( )
		end
	end

	local starDetail = CallModuleFunc( ModuleType.UNION , "GetUnionCombatStarDetail")
	if starDetail.belong_guild_name ~= "" then
		self.union_combat_pitera:SetString( starDetail.belong_guild_name )
	else
		self.union_combat_pitera:SetString( CPPGameLib.GetString("union_combat_pitera") )
	end
end

----------------------------------------------------------------------------------------------------------------------------
--宣战
----------------------------------------------------------------------------------------------------------------------------
--宣战
function LogicGUI.TempUnionCombatStarInfo:SetDeclareWar()
	local info = CallModuleFunc( ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.id )
	
	local lst = {}
	--宣战
	lst.text = CPPGameLib.GetString("union_combat_declareWar") 
	lst.func = CPPGameLib.Handler(self,self.OnDeclareWar)

	if info then
		--布防
		lst.text = CPPGameLib.GetString("union_combat_arming") 
		lst.func = CPPGameLib.Handler(self,self.OnArming)

		local occupy = CallModuleFunc( ModuleType.UNION , "GetUnionCombatOccupy" )
		if #occupy > CallModuleFunc( ModuleType.UNION , "GetUnionCombatOccupyMax" ) then
			--撤离
			local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DECLARE_WAR )
			if type_ ~= 0 then
			    self.evacuate:SetVisible( false )
			end
		end
	end

	--进入战场
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_blue")
   	local declareWar = Util:Button( sp , function( ... )
   		lst.func()
   		self:DeleteMe()
   	end , lst.text , Macros.TypefaceSize.button , Macros.Color.btn , Macros.Color.blue_stroke )
   	declareWar:SetAnchorPoint( 0.5 , 0.5 )
   	declareWar:SetPosition( self.star:GetPositionX() , self.btn_check:GetPositionY() )
   	self.bg:AddChild( declareWar )

	if myDeclareWar then
		Util:GreyButton( declareWar )
	end
end

--选中宣战
function LogicGUI.TempUnionCombatStarInfo:OnDeclareWar( )
	--宣战信息
	local allLst = CallModuleFunc( ModuleType.UNION , "GetUnionCombatMyDeclareWarLst" )
	local allLstSum = 0
	for i , v in pairs( allLst ) do
		allLstSum = allLstSum + 1
	end

   	local myDeclareWar = CallModuleFunc( ModuleType.UNION , "GetUnionCombatMyDeclareWarLst" , self.id )

   	local occupy = CallModuleFunc( ModuleType.UNION , "GetUnionCombatOccupy" )
   	local index = 0
   	for i , v in ipairs( occupy ) do
   		local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , v.id )
   		if starCnf.star_type >= 4 then
   			index = index + 1
   		end
   	end

	--公会权限
	if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DECLARE_WAR ) == 0 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_permission") )
		return 
	--已经宣战过
	elseif myDeclareWar then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_declareWarTips_1") )
		return 
	--名额上限
	elseif allLstSum >= CallModuleFunc( ModuleType.UNION , "GetUnionCombatMyDeclareWarMax" ) then 
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_declareWarMax") )
		return 
	--占领上限
	elseif CallModuleFunc( ModuleType.UNION , "GetUnionCombatOccupyMax" ) <= index then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_occupyMax") )
		return 
	end

	local info = {} 
	info.star_id = self.id
	CallModuleFunc( ModuleType.UNION , "GuildWarDeclareWarReq" , info )
end

----------------------------------------------------------------------------------------------------------------------------
--布防
----------------------------------------------------------------------------------------------------------------------------
--选中布防
function LogicGUI.TempUnionCombatStarInfo:OnArming( )
	print("xy" , "===选中布防")
	local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DECLARE_WAR )
	if type_ == 0 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_adjustment") )
	else
		CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_COMBAT_BATTLEGROUND)
	end
end

----------------------------------------------------------------------------------------------------------------------------
--攻击
----------------------------------------------------------------------------------------------------------------------------
--设置攻击我
function LogicGUI.TempUnionCombatStarInfo:SetCombat( )
   	local lst = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatStarInfo" , self.id )
   	local starInfo = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail")
	--没人打
	if starInfo.is_over == 0 then
		if starInfo.attack_guild_id ~= 0 then
			self:SetCombatExist()
		else
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_peace") )
		end
	else
		self:SetCombatNot()
	end
end

--有战斗
function LogicGUI.TempUnionCombatStarInfo:SetCombatExist( )
	--vs
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_vs")
	local sp_vs = Util:Sprite( sp )
	sp_vs:SetAnchorPoint( 0.5 , 0.5 )
	sp_vs:SetPosition( self.star:GetPositionX() , 210 )
	self.bg:AddChild( sp_vs )

	--进入战场
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_blue")
   	local btn_blue = Util:Button( sp , function( ... )
   		print("xy","进入战场")
		CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_COMBAT_BATTLEGROUND)
		self:DeleteMe()
   	end , CPPGameLib.GetString("union_combat_enterInto") , Macros.TypefaceSize.button , Macros.Color.btn , Macros.Color.blue_stroke )
   	btn_blue:SetAnchorPoint( 0.5 , 0.5 )
   	btn_blue:SetPosition( self.star:GetPositionX() , self.btn_check:GetPositionY() )
   	self.bg:AddChild( btn_blue )

   	local lst = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatStarInfo" , self.id )
   	local lst_pos = {
						[1] = { color = Macros.Color.ladderVs_1 , x = sp_vs:GetContentWidth()/2 - 70 },
						[2] = { color = Macros.Color.ladderVs_2 , x = sp_vs:GetContentWidth()/2 + 70 }
					}
   	for i , v in ipairs( lst ) do
   		local name = Util:Label( v.union_name , Macros.TypefaceSize.slightly ,lst_pos[i].color )
   		name:SetAnchorPoint( 0.5 , 0.5 )
   		name:SetPosition( lst_pos[i].x , sp_vs:GetContentHeight()/2 + 30 )
   		sp_vs:AddChild( name )

   		if v.isPerson == true then
	   		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon"..v.union_icon)
	   		local icon = Util:Sprite( sp )
	   		icon:SetAnchorPoint( 0.5 , 0.5 )
	   		icon:SetPosition( name:GetPositionX() , sp_vs:GetContentHeight()/2 - 13 )
	   		sp_vs:AddChild( icon )
	   		icon:SetScaleFactor( 0.5 )
	   	else
	   		name:SetPosition( lst_pos[i].x , sp_vs:GetContentHeight()/2 )
	   	end
   	end
end

--没战斗
function LogicGUI.TempUnionCombatStarInfo:SetCombatNot( )
	--进入战场
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_blue")
   	local btn_blue = Util:Button( sp , function( ... )
   		print("xy","进入战场")
		CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_COMBAT_BATTLEGROUND)
		self:DeleteMe()
   	end , CPPGameLib.GetString("union_combat_enterInto") , Macros.TypefaceSize.button , Macros.Color.btn , Macros.Color.blue_stroke )
   	btn_blue:SetAnchorPoint( 0.5 , 0.5 )
   	btn_blue:SetPosition( self.star:GetPositionX() , self.btn_check:GetPositionY() )
   	self.bg:AddChild( btn_blue )
end

----------------------------------------------------------------------------------------------------------------------------
--撤离
----------------------------------------------------------------------------------------------------------------------------
--设置撤离
function LogicGUI.TempUnionCombatStarInfo:OnEvacuate( )
	print("xy" , "===选中撤离" , self.id )
	LogicGUI.TempUnionCombatEvacuate.New( self.id )
end

----------------------------------------------------------------------------------------------------------------------------
--
----------------------------------------------------------------------------------------------------------------------------
--设置开采
function LogicGUI.TempUnionCombatStarInfo:SetExploit( btn )
	local type_ = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarLevel" , self.id )
	
	local occupyInfo = CallModuleFunc( ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.id )
	if not occupyInfo then
		btn:SetVisible( false )
		return
	end

	if type_ == "low" then
		local type_ , endTime = CallModuleFunc( ModuleType.UNION , "UnionCombatIntegralStarTime" )

		--领奖中
		if type_ == "award" then
			btn:SetVisible( true )
			local info = CallModuleFunc( ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.id )
			if not info or info.is_have_reward == 0 then
				Util:GreyButton( btn )
			end
		else
			btn:SetVisible( false )
		end

	elseif type_ == "tall" then
		local type_ = CallModuleFunc( ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )

		if occupyInfo and occupyInfo.is_have_reward == 0 then
			btn:SetText( CPPGameLib.GetString( "union_combat_texploit" ) )
			Util:GreyButton( btn )
			return 
		end

		for i , v in ipairs( type_ ) do
			if v == "duiZhan" then
				btn:SetVisible( false )
			elseif v == "xuanZhan" then
				local info = CallModuleFunc( ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.id )
				if info then
					btn:SetVisible( true )
				else
					btn:SetVisible( false )
				end
			end
		end
	end
end

--开采
function LogicGUI.TempUnionCombatStarInfo:OnExploit( )
	local occupyInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.id )
	if occupyInfo.is_have_reward == 0 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_notexploit") )
		return
	end

	-- 1.低级星球领取 2.高级星球领取 3.一键领取
	local type_ = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarLevel" , self.id )
	local take_type = 0
	if type_ == "low" then
		take_type = 1
	elseif type_ == "tall" then
		take_type = 2
	end

	local info = {}
	info.take_type = take_type
	info.take_star_list = {}
	table.insert( info.take_star_list , { star_id = self.id } )
	CallModuleFunc(ModuleType.UNION , "GuildWarTakeStarRewardReq" , info )
end

--查看
function LogicGUI.TempUnionCombatStarInfo:OnBtnCheck( )
	local type_ = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarLevel" , self.id )
	local take_type = 0
	if type_ == "low" then
		take_type = 1
	elseif type_ == "tall" then
		take_type = 0
	end
	--0是宣战 1是积分
	LogicGUI.TempUnionCombatDeclarewar.New( self.id , take_type )
end

--倒计时
function LogicGUI.TempUnionCombatStarInfo:Countdown( endTime )
	local func = nil
	local function moveAnimate( ... )
		local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		if endTime - server_time >= 0 then
			local time = string.format( Macros.Color.green_rt , TimeTool.TransTimeStamp( "HH:MM:SS" , endTime - server_time ) )
			self.union_combat_Countdown:SetText( CPPGameLib.GetString("union_combat_Countdown" , time ) )
			CPPActionManager.DelayTo( self.union_combat_Countdown , 1 , func)
		else
			self.union_combat_Countdown:SetText( "" )
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()
end