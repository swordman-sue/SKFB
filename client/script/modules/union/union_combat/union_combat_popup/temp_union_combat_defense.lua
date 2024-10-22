-- @brief 防御塔 
-- @author: yjg
-- @date: 2018年10月22日17:38:07
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatDefense = LogicGUI.TempUnionCombatDefense or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatDefense:__init( data )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID,color = {0,0,0,100}}
    self:InitTemplate(nil)  
    self.data = data
	self:__Getwidget( )
	self:__ShowUI( data )   
	self:__Server() 
end

function LogicGUI.TempUnionCombatDefense:__delete( )
end

function LogicGUI.TempUnionCombatDefense:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function( ... )
   		self:DeleteMe()
    end)
    return self.layout
end

function LogicGUI.TempUnionCombatDefense:__Server( )
	local starInfo = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
	local occupyInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , starInfo.star_id )

	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionMemberLst")
	if occupyInfo and #lst <= 0 then
		CallModuleFunc(ModuleType.UNION , "GuildMemberListReq")
	else
		self:__SetShowUI()
	end

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_MEMBER,function (_, data)
        self:__SetShowUI()
    end)

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_DEFENSE,function (_, roleId)
        self:__SetShowUI( roleId )
    end)

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_BUY_TIME,function (_, data)
        self:SetCombatTime()
    end)    

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_TOWER,function (_, data)
        self:__SetShowUI()
    end)
    
end

function LogicGUI.TempUnionCombatDefense:__Getwidget()
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_frame_bg") 
	self.sp_frame_bg = Util:Sprite( sp , function ( ... )
		-- body
	end)
	self.sp_frame_bg:SetAnchorPoint( 0.5 , 0.5 )
	self.sp_frame_bg:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild( self.sp_frame_bg )

	--内容
	self.content = Util:Layout( self.sp_frame_bg:GetContentWidth() - 80 , self.sp_frame_bg:GetContentHeight() - 90 )
	self.content:SetAnchorPoint( 0.5 , 0 )
	self.content:SetPosition( self.sp_frame_bg:GetContentWidth()/2 , 45 )
	self.sp_frame_bg:AddChild( self.content )
    self.content:SetBackGroundColor(0, 0, 0)
    self.content:SetBackGroundColorType(1)
    self.content:SetBackGroundColorOpacity(200)

	local common_continue = Util:Label(CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.slightly, Macros.Color.lilyWhite)
	common_continue:SetAnchorPoint(0.5 , 1 )
	common_continue:SetPosition( self.sp_frame_bg:GetContentWidth()/2 , -20 )
	self.sp_frame_bg:AddChild( common_continue )
end

function LogicGUI.TempUnionCombatDefense:__ShowUI( data )
	local headline = Util:Name( CPPGameLib.GetString("union_combat_architecture_"..data. tower_type ) , Macros.TypefaceSize.popup , 2 )
	headline:SetAnchorPoint( 0.5 , 0.5 )
	headline:SetPosition( self.sp_frame_bg:GetContentWidth()/2 , self.sp_frame_bg:GetContentHeight() - 22 )
	self.sp_frame_bg:AddChild( headline )

	--图标列表
	local params =  
	{
	    item_class = Modules.TempUnionCombatDefenseItem,
	    item_width = self.content:GetContentWidth(), --349
	    item_height = self.content:GetContentHeight()/4, --86.25	
	    row = 4,
	    col = 1,
	    -- item_space_r = 5 ,
	    view_width = self.content:GetContentWidth(),
	    view_height = self.content:GetContentHeight() - 10 ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.content:AddChild(self.lst_list:GetNode())
	PosTool.Center( self.lst_list )

	self:SetCombatTime()
end

--设置进攻次数
function LogicGUI.TempUnionCombatDefense:SetCombatTime()

	if self.combatTime then
		self.combatTime:RemoveSelf( true )
		self.combatTime = nil
	end

	local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
	local occupyInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , starDetail.star_id )
	if occupyInfo then
		return 
	end

	--今日剩余进攻次数
	local highTime = CallModuleFunc(ModuleType.UNION , "GetUnionCombatHighTime")

	--剩余进攻次数
	local union_combat_combatTime = Util:Label( CPPGameLib.GetString("union_combat_combatTime" , highTime ) )

	--按钮
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_buy" )
	local btn = Util:Button( sp , function( ... )
		local buyTime = CallModuleFunc(ModuleType.UNION , "GetUnionBuyTime")
		if buyTime ~= 0 then 
			print("xy","购买次数")
			LogicGUI.TempUnionCombatBuy.New()
		else
			print("xy" , "没有次数")		
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_limit_buy_num"))	
		end
	end )

	self.combatTime = Util:Layout( union_combat_combatTime:GetContentWidth() + btn:GetContentWidth() + 5 , 10 )
	self.combatTime:SetAnchorPoint( 0.5 , 0.5 )
	self.combatTime:SetPosition( self.sp_frame_bg:GetContentWidth()/2 , 27 )
	self.sp_frame_bg:AddChild( self.combatTime )

    union_combat_combatTime:SetAnchorPoint( 0 , 0.5 )
    union_combat_combatTime:SetPosition( 0 , self.combatTime:GetContentHeight()/2 )
    self.combatTime:AddChild( union_combat_combatTime )

    btn:SetAnchorPoint( 0 , 0.5 )
    btn:SetPosition( union_combat_combatTime:GetContentWidth() + 5 , self.combatTime:GetContentHeight()/2 )
    self.combatTime:AddChild( btn )
end

function LogicGUI.TempUnionCombatDefense:__SetShowUI( roleId )
	if not roleId then
		local lst = CallModuleFunc(ModuleType.UNION , "ToIdGetGuardLst" , self.data.tower_id )
		self.lst_list:SetDataList( lst )
	else
		local roleInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerRoleInfo", roleId )
		if not roleInfo then
			local lst = CallModuleFunc(ModuleType.UNION , "ToIdGetGuardLst" , self.data.tower_id )
			self.lst_list:SetDataList( lst )
		else
			self.lst_list:ItemIter( function( index , item )
				if item then
					item:SetBtn()
				end
			end )
		end
	end
end


Modules = Modules or {}
Modules.TempUnionCombatDefenseItem = Modules.TempUnionCombatDefenseItem or BaseClass(GUI.ScrollItem)

function Modules.TempUnionCombatDefenseItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TempUnionCombatDefenseItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(349,86.25)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.TempUnionCombatDefenseItem:__delete()
    
end

function Modules.TempUnionCombatDefenseItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TempUnionCombatDefenseItem:__ShowUI( data )

    --头像
	local img_head = LogicGUI.RoleIcon.New(false)
	img_head:SetTouchCallback(function()
		-- LogicGUI.TemplCheckRoleInfo.New( data )
    end)
 	img_head:SetData(data.head_icon)
	self.layout:AddChild(img_head:GetNode())
	PosTool.Center(img_head , -120 ) 
	img_head:SetScaleFactor( 0.9 )

	--名字
	local name = Util:Label( data.name , Macros.TypefaceSize.normal )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( self.layout:GetContentWidth()/2 - 70 , self.layout:GetContentHeight()/2 + 20 )
	self.layout:AddChild( name )

	--战力
	local dungeon_power = Util:Label( CPPGameLib.GetString("dungeon_power" , data.battle_value ) , Macros.TypefaceSize.slightly )
	dungeon_power:SetAnchorPoint( 0 , 0.5 )
	dungeon_power:SetPosition( self.layout:GetContentWidth()/2 - 70 , self.layout:GetContentHeight()/2 - 20 )
	self.layout:AddChild( dungeon_power )	

	--按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_little")
	self.examine = Util:Button(sp , function( ... )
		-- body
	end , "" , Macros.TypefaceSize.minimum , Macros.Color.lilyWhite )
	self.examine:SetAnchorPoint( 1 , 0.5 )
	self.examine:SetPosition( self.layout:GetContentWidth() - 10 , self.layout:GetContentHeight()/2 + 15 )
	self.layout:AddChild(self.examine) 
	self.examine:SetScaleFactor( 1.2 )

	--次数
	local beattack_times = data.beattack_times
	if data.beattack_times <= 0 then
		beattack_times = 0
	end
	self.union_combat_time = Util:Label( CPPGameLib.GetString("union_combat_time" , beattack_times ) , Macros.TypefaceSize.minimum )
	self.union_combat_time:SetAnchorPoint( 0.5 , 0.5 )
	self.union_combat_time:SetPosition( self.examine:GetPositionX() - ( (self.examine:GetContentWidth()*1.2) / 2 ) , self.layout:GetContentHeight()/2 - 20 )
	self.layout:AddChild(self.union_combat_time) 

	self:SetBtn()
end


function Modules.TempUnionCombatDefenseItem:SetBtn()

	local occupyInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.__data.star_id )
	if occupyInfo then
		local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DECLARE_WAR )
		--设置防御
		self:SetGuard( self.__data )
	else
		--非人
		if self.__data.isPerson == false then
			-- 进攻
			self:SetAttack()
		else
			local garrisonInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerRoleInfo", self.__data.role_id )
			CPPGameLib.PrintTable("xy" , garrisonInfo , "garrisonInfo") 
			local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , garrisonInfo.tower_id )
			if (towerCnf.role_defense_times - garrisonInfo.be_attack_times) > 0 then
				-- 进攻
				self:SetAttack()
			else
				--败退
				self:SetRepulse()
			end
		end
	end
end

--设置防御
function Modules.TempUnionCombatDefenseItem:SetGuard( data )
	--调遣信息
	local garrisonInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerRoleInfo", data.role_id )
	local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DECLARE_WAR )
	if type_ == 0 then
		self.examine:SetVisible( false )
		return
	end

	--如果有调遣信息
	if garrisonInfo then
		--如果是驻守在这个塔里
		if garrisonInfo.tower_id == data.tower_id then
			--撤下
			self:SetRetreat( garrisonInfo )
		else
			--调动
			self:SetTransfer( garrisonInfo )
		end
	else
		--驻守
		self:SetGarrison()
	end
end

--撤下
function Modules.TempUnionCombatDefenseItem:SetRetreat( garrisonInfo )	
	local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , garrisonInfo.tower_id )
	self.union_combat_time:SetString( CPPGameLib.GetString("union_combat_guard_" .. towerCnf.tower_type ) )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_little")
	self.examine:SetNormalImage( sp )

	self.examine:SetText( CPPGameLib.GetString("union_combat_remove") )
	WidgetUnity.SetWidgetListener( self.examine , nil, nil, function( ... )
		print("xy" , "撤下")
		if not self:JudgeAdjustType() then
			return
		end

		local star = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail")
		local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , self.__data.tower_id )

		local info = {}
		info.star_id = star.star_id
		info.tower_type = towerCnf.tower_type
		info.role_id = self.__data.role_id
		info.is_to_battle = 0
		CallModuleFunc(ModuleType.UNION , "GuildWarOrganizeDefenseReq" , info )

	end, nil, 0.5*1000)
end

--调动
function Modules.TempUnionCombatDefenseItem:SetTransfer( garrisonInfo )
	local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , garrisonInfo.tower_id )
	self.union_combat_time:SetString( CPPGameLib.GetString("union_combat_guard_" .. towerCnf.tower_type ) )
	
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_little")
	self.examine:SetNormalImage( sp )

	self.examine:SetText( CPPGameLib.GetString("union_combat_transfer") )
	WidgetUnity.SetWidgetListener( self.examine , nil, nil, function( ... )
		print("xy" , "调动")
		if not self:JudgeAdjustType() then
			return
		end

		local defenseInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerDefenseInfo" , self.__data.tower_id )
		local nowNum = #defenseInfo.defender_list.__list
		if nowNum + 1 > towerCnf.defender_num_max then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_defenseNum" , towerCnf.defender_num_max ) )
			return
		end
		
		local star = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail")
		local info = {}
		info.star_id = star.star_id
		info.tower_type = towerCnf.tower_type
		info.role_id = self.__data.role_id
		info.is_to_battle = 0
		CallModuleFunc(ModuleType.UNION , "GuildWarOrganizeDefenseReq" , info )

		local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , self.__data.tower_id )
		local info = {}
		info.star_id = star.star_id
		info.tower_type = towerCnf.tower_type
		info.role_id = self.__data.role_id
		info.is_to_battle = 1
		CallModuleFunc(ModuleType.UNION , "GuildWarOrganizeDefenseReq" , info )

	end, nil, 0.5*1000)
end

--驻守
function Modules.TempUnionCombatDefenseItem:SetGarrison()
	local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , self.__data.tower_id )
	self.union_combat_time:SetString("")

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_littlegreen")
	self.examine:SetNormalImage( sp )

	self.examine:SetText( CPPGameLib.GetString("union_combat_garrison") )
	WidgetUnity.SetWidgetListener( self.examine , nil, nil, function( ... )
		print("xy" , "驻守")
		if not self:JudgeAdjustType() then
			return
		end

		local defenseInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerDefenseInfo" , self.__data.tower_id )
		local nowNum = #defenseInfo.defender_list.__list
		if nowNum + 1 > towerCnf.defender_num_max then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_defenseNum" , towerCnf.defender_num_max ) )
			return
		end

		local star = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail")

		local info = {}
		info.star_id = star.star_id
		info.tower_type = towerCnf.tower_type
		info.role_id = self.__data.role_id
		info.is_to_battle = 1
		CallModuleFunc(ModuleType.UNION , "GuildWarOrganizeDefenseReq" , info )
	end, nil, 0.5*1000)
end

--判断调动时间
function Modules.TempUnionCombatDefenseItem:JudgeAdjustType()
	local timeCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )
	for i , v in ipairs( timeCnf ) do
		if v == "duiZhan" then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_incombat") )
			return false
		end
	end

	local roleInfo , index = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerRoleInfo" , self.__data.role_id , Modules.UnionModule.BASE )
	if roleInfo then
		local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , roleInfo.tower_id )
		CPPGameLib.PrintTable("xy" , towerCnf  , "towerCnf")
		if towerCnf.tower_type ==  Modules.UnionModule.BASE then
			if index <= 1 then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_baseguard" ) )
				return false
			end
		end
	end

	return true
end

--进攻
function Modules.TempUnionCombatDefenseItem:SetAttack()
	self.examine:SetText( CPPGameLib.GetString("union_combat_attack") )
	WidgetUnity.SetWidgetListener( self.examine , nil, nil, function( ... )

		local highTime = CallModuleFunc(ModuleType.UNION , "GetUnionCombatHighTime")
		if highTime <= 0 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("heroShop_noResidue"))		
			return
		end

		local checkBattle = CallModuleFunc(ModuleType.UNION , "CheckBattleCondition" , Macros.Game.FBType.UNION_TALL , self.__data )
		if checkBattle then
			local attackInfo = {}
			attackInfo.name = self.__data.name
			attackInfo.server = self.__data.server
			CallModuleFunc(ModuleType.UNION , "SetAttackObject" , attackInfo )

			local star = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail")
			--不是人
			if self.__data.isPerson == false then
				CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.UNION_TALL , star.star_id , self.__data.tower_id , 0 )
			else
				CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.UNION_TALL , star.star_id , self.__data.tower_id , self.__data.role_id )
			end
		
		--如果是基地 而且还不能打
		elseif isBase == true and isAttack ~= true  then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("union_combat_Nebulus"))		
		end

	end, nil, 0.5*1000)
end

--败退
function Modules.TempUnionCombatDefenseItem:SetRepulse()
	self.examine:SetText( CPPGameLib.GetString("union_combat_repulse") )
	WidgetUnity.SetWidgetListener( self.examine , nil, nil, function( ... )
		print("xy" , "进攻")
	end, nil, 0.5*1000)
	Util:GreyButton( self.examine )
end

