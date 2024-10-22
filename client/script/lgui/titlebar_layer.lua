
--[[
%% @module: 标题控件
%% @author: swordman sue
%% @created: 2016-08-11
注：需手动释放(DeleteMe)
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TitleBarLayer = LogicGUI.TitleBarLayer or BaseClass(UILayer)

--[[
@ params：
	必须：
	title 					[string]标题名
	close_func				[function]关闭回调
	show_bg 				[bool]是否显示标题栏背景
	lucency_bg 				[bool]是否显示透明的背景
	show_diamond			[bool]是否显示钻石
	show_gold				[bool]是否显示金币
	show_stamina			[bool]是否显示体力
	show_energy				[bool]是否显示精力
	show_power 				[bool]是否显示战力
	show_arena_prestige		[bool]是否显示竞技场声望
	show_level				[bool]是否显示等级
	show_score_match_honor		[bool]是否显示积分赛积分
	res_beg_offset_x 		[number]资源栏起始X偏移
	res_unit_bg_imageinfo	[userdata]资源栏每个单元的背景信息({atlas_type, img_name})
	res_unit_title_offset_x	[number]资源栏每个单元的数量文本X偏移
--]]

function LogicGUI.TitleBarLayer:__init(params, parent_node)
	self:SetAnchorPoint(0.5, 1)

	if parent_node then
		self:SetContentSize(parent_node:GetContentWidth(), 54)
	else
		self:SetContentSize(Macros.Global.DesignSize.width, 54)		
	end

	if params.show_bg ~= false then
		self.__show_bg = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_up_part2"), {10, 10})	
		self.__show_bg:SetAnchorPoint(0,0.5)
		self.__show_bg:SetContentSize(self.__show_bg:GetContentWidth() * 2 , self:GetHeight())
		self.__show_bg:SetPosition(0,self:GetHeight()/2)
		self:AddChild(self.__show_bg)
	end

	if params.lucency_bg == true then
		local bg = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_up_part2"), {10, 10})	
		bg:SetAnchorPoint(0,0.5)
		bg:SetContentSize(bg:GetContentWidth() * 2 , self:GetHeight())
		bg:SetPosition(0,self:GetHeight()/2)
		self:AddChild(bg)
	end

	--关闭按钮
	if params.close_func then
		self.__close_func = params.close_func
		self:__CreateCloseBtn()
	end

	--标题
	if params.title then
		self:__CrateTitle(params.title)
	end

	--组件
    self:SetCurrency( params )

	--等级、经验条
	if params.show_level then
		self:__CreateLevel()
	end	

	self:__RegistAllEvents()
end

function LogicGUI.TitleBarLayer:__delete()
	if self.faceEffects then
		for _, effect in pairs(self.faceEffects) do
			effect:DeleteMe()
		end
		self.faceEffects = nil
	end
	self:CurrencyEffects()
end

function LogicGUI.TitleBarLayer:CurrencyEffects()
	if self.effectsDiamno then
		GlobalEffectMgr:DelEffect(self.effectsDiamno)
		self.effectsDiamno = nil
	end

	if self.effectsGold then
		GlobalEffectMgr:DelEffect(self.effectsGold)
		self.effectsGold = nil
	end

	if self.effectsStaminaId then
		GlobalEffectMgr:DelEffect(self.effectsStaminaId)
		self.effectsStaminaId = nil
	end

	if self.effectsEnergyId then
		GlobalEffectMgr:DelEffect(self.effectsEnergyId)
		self.effectsEnergyId = nil
	end
end

function LogicGUI.TitleBarLayer:SetTitle(title)
	if self.__txt_title then
		self.__txt_title:SetString(title)
	else
		self:__CrateTitle(title)
	end
end

function LogicGUI.TitleBarLayer:SetCloseFunc(close_func)
	self.__close_func = close_func
end

function LogicGUI.TitleBarLayer:GetCloseFunc()
	return self.__close_func
end

function LogicGUI.TitleBarLayer:GetCloseBtn()
	return self.__btn_close
end

--设置组件
function LogicGUI.TitleBarLayer:SetCurrency( params )

	--资源：金币、钻石、体力、精力....
	self.__rt_offset = params.res_beg_offset_x or -90
	self.__rt_step = -160

	self:CurrencyEffects()
	if self.titleLayer then
		self.titleLayer:RemoveSelf(true)
		self.titleLayer = nil
	end

	--存放控件的容器
	self.titleLayer = Util:Layout( self:GetNode():GetContentWidth() , self:GetNode():GetContentHeight() )
	self.titleLayer:SetAnchorPoint( 0.5 , 1 )
	self.titleLayer:SetPosition( self:GetNode():GetContentWidth()/2 , self:GetNode():GetContentHeight() )
	self:AddChild( self.titleLayer )

	local create_prop_type_map = {}
	if params.show_diamond then create_prop_type_map[Macros.Game.RolePropType.DIAMOND] = true end
	if params.show_gold then create_prop_type_map[Macros.Game.RolePropType.GOLD] = true end
	if params.show_stamina then create_prop_type_map[Macros.Game.RolePropType.STAMINA] = true end
	if params.show_energy then create_prop_type_map[Macros.Game.RolePropType.ENERGY] = true end
	if params.show_power then create_prop_type_map[Macros.Game.RolePropType.BATTLE_VALUE] = true end
	if params.show_arena_prestige then create_prop_type_map[Macros.Game.RolePropType.ARENA_PRESTIGE] = true end
	--魂晶
	if params.show_hero_soul then create_prop_type_map[Macros.Game.RolePropType.HERO_SOUL] = true end
	--神魂
	if params.show_godsoul then create_prop_type_map[Macros.Game.RolePropType.GODSOUL] = true end
	--公会贡献
	if params.show_contribution then create_prop_type_map[Macros.Game.RolePropType.GUILD_CONTRIBUTION] = true end
	--无尽石
	if params.show_endless then create_prop_type_map[Macros.Game.RolePropType.ENDLESS_STORE] = true end
	--宝石
	if params.show_gem then create_prop_type_map[Macros.Game.RolePropType.GEM] = true end
	--命运硬币
	if params.show_destiny_coin then create_prop_type_map[Macros.Game.RolePropType.DESTINY_COIN] = true end
	--星魂
	if params.show_star_soul then create_prop_type_map[Macros.Game.RolePropType.CARD_CURRENCY] = true end
	--积分赛积分
	if params.show_score_match_honor then create_prop_type_map[Macros.Game.RolePropType.SCORE_MATCH_HONOR] = true end
	--资源战积分
	if params.show_resource then create_prop_type_map[Macros.Game.RolePropType.RESOURCE_WAR] = true end

	local all_prop_type_list = 
	{
		Macros.Game.RolePropType.DIAMOND, Macros.Game.RolePropType.GOLD,
		Macros.Game.RolePropType.ARENA_PRESTIGE, Macros.Game.RolePropType.ENERGY, 
		Macros.Game.RolePropType.STAMINA, Macros.Game.RolePropType.BATTLE_VALUE,
		Macros.Game.RolePropType.HERO_SOUL,Macros.Game.RolePropType.GODSOUL,
		Macros.Game.RolePropType.GUILD_CONTRIBUTION,Macros.Game.RolePropType.ENDLESS_STORE,
		Macros.Game.RolePropType.GEM , Macros.Game.RolePropType.DESTINY_COIN ,
		Macros.Game.RolePropType.CARD_CURRENCY,Macros.Game.RolePropType.SCORE_MATCH_HONOR,
		Macros.Game.RolePropType.RESOURCE_WAR,
	}
	for _, prop_type in ipairs(all_prop_type_list) do
		local prop_name = Macros.Game.RolePropKey[prop_type]
		local widget_name = "__txt_prop_"..prop_name
		if self[widget_name] then
			self[widget_name] = nil
		end
		if create_prop_type_map[prop_type] then
			self:__CreatePropWidget(prop_type, params)
		end
	end
end



--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function LogicGUI.TitleBarLayer:__CreateCloseBtn()
	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_scene_return")
	self.__btn_close = Util:Button(imgdata, function()
		if self.__close_func then
			self.__close_func()
		end
	end)	
	self.__btn_close:SetAnchorPoint( 0, 1 )
	self.__btn_close:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 3)
	self.__btn_close:SetPosition( 0 , self:GetHeight() )
	self:AddChild(self.__btn_close)

end

function LogicGUI.TitleBarLayer:__CrateTitle(title)
	self.__txt_title = Util:Name( title , Macros.TypefaceSize.headline)
	self.__txt_title:SetAnchorPoint(0, 0.5)
	self.__txt_title:SetPosition( self.__btn_close:GetPositionX() + self.__btn_close:GetContentWidth() ,  self:GetHeight()/2 )
	self:AddChild(self.__txt_title)
end

function LogicGUI.TitleBarLayer:__CreatePropWidget(prop_type, params)
	local prop_name = Macros.Game.RolePropKey[prop_type]
	local prop_name_max = prop_name.."_max"
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

	local imgdata
	if params.res_unit_bg_imageinfo then
		imgdata = Resource.LoadTool.GetImageData(params.res_unit_bg_imageinfo[1], params.res_unit_bg_imageinfo[2])
	else
		imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_resource")
	end
    local sp_bg = Util:Sprite9(imgdata)
    sp_bg:SetAnchorPoint(1, 0.5)
    sp_bg:SetContentSize(150, sp_bg:GetContentHeight() )
    self.titleLayer:AddChild(sp_bg)    
    PosTool.RightCenter(sp_bg, self.__rt_offset + 50 )
    self.__rt_offset = self.__rt_offset + self.__rt_step

    local sp_icon
    
    if prop_type == Macros.Game.RolePropType.DIAMOND then 		--钻石
		self.effectsDiamno , sp_icon = GlobalEffectMgr:CreateUIEffect( {res_name = "UI_zhuchengzhuanshi", loop = true}, sp_bg )
		sp_icon:SetAnchorPoint(0, 0.5)
		PosTool.LeftCenter( sp_icon  , 10 )
   	elseif prop_type == Macros.Game.RolePropType.GOLD then		--金币

		self.effectsGold , sp_icon = GlobalEffectMgr:CreateUIEffect( {res_name = "UI_jinbi", loop = true}, sp_bg )
		sp_icon:SetAnchorPoint(0, 0.5)
		PosTool.LeftCenter( sp_icon  , 10 )

   	elseif prop_type == Macros.Game.RolePropType.STAMINA then	--体力
		self.effectsStaminaId , sp_icon = GlobalEffectMgr:CreateUIEffect( {res_name = "UI_tili", loop = true}, sp_bg )
		sp_icon:SetAnchorPoint(0, 0.5)
		PosTool.LeftCenter( sp_icon  , 10 )
   	elseif prop_type == Macros.Game.RolePropType.ENERGY then	--精力
		self.effectsEnergyId , sp_icon = GlobalEffectMgr:CreateUIEffect( {res_name = "UI_jingli", loop = true}, sp_bg )
		sp_icon:SetAnchorPoint(0, 0.5)
		PosTool.LeftCenter( sp_icon  , 10 )
   	else
		if prop_type == Macros.Game.RolePropType.BATTLE_VALUE then
			imgdata = Resource.PathTool.GetUIResPath( "commonmoneyicon/type_" .. Macros.Game.RolePropType.BATTLE_VALUE , true )
		else
			imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..prop_type)
		end
		sp_icon = Util:Sprite(imgdata)
		sp_icon:SetAnchorPoint(0, 0.5)
		sp_bg:AddChild(sp_icon)
		PosTool.LeftCenter(sp_icon)
   	end

	local widget_name = "__txt_prop_"..prop_name
	if role_info[prop_name_max] then
		self[widget_name] = Util:Label(role_info[prop_name].."/"..role_info[prop_name_max], Macros.TypefaceSize.largish, Macros.Color.btn)
	else
		self[widget_name] = Util:Label(tostring( (role_info[prop_name]) ), Macros.TypefaceSize.largish, Macros.Color.btn)
	end
	self[widget_name]:SetAnchorPoint(1, 0.5)
	sp_bg:AddChild(self[widget_name])
	PosTool.RightCenter(self[widget_name], params.res_unit_title_offset_x or -16 , -2)

	local shopCnf = {}
	for i,v in pairs( CPPGameLib.GetConfig("ItemShop" ,nil ,nil ,true) ) do
		shopCnf[v.item_id] = {}
		shopCnf[v.item_id].goods_id = i
		shopCnf[v.item_id].lst = v
	end
	local wallet = CPPGameLib.GetConfig("HuoBi", prop_type , false)
	if not wallet then
		return
	end
	if ( Macros.Game.RolePropType.BATTLE_VALUE ~= prop_type and shopCnf[wallet.item_id] ) or prop_type == Macros.Game.RolePropType.DIAMOND then
		local btn_btn_bag_add = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bag_add"))
		btn_btn_bag_add:SetAnchorPoint(0.5 , 0.5)
		btn_btn_bag_add:SetPosition( sp_icon:GetContentWidth() + 10 , -10 )
		sp_icon:AddChild(btn_btn_bag_add , 999 )
		
		--触摸层
		local layou = Util:Layout(sp_bg:GetContentWidth(), sp_bg:GetContentHeight(), function ( ... )
			--判断是否在匹配中
			if params.is_matching then
				if CallModuleFunc(ModuleType.TEAM,"GetIsOnMatching") then
					--提示
	                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_matching_tips"))
					return
				end
			end
			--判断是否在抢夺中
			if params.is_plundering then
				if CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"GetIsOnPlunder") then
					--提示
	                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("quick_plunderedtreasures_on_animationing_tips"))
					return
				end
			end
			if prop_type ~= Macros.Game.RolePropType.DIAMOND then
				local viewInfo = GlobalViewMgr:GetCurViewInfo()
				if viewInfo and ( viewInfo.view_type == "SHOP_FAILARMY" or viewInfo.view_type == "SHOP") then
				else
					LogicGUI.TemplConsumeBuy.New( prop_type )
				end
			else
				if ConfigAdapter.Common.GetDiamondSkip() == 1 then
					CallModuleFunc(ModuleType.RECHARGE, "OpenView", BaseViewType.RECHARGE )
				end
			end
		end)
		layou:SetAnchorPoint(0.5,0.5)
		layou:SetTouchSwallow(true)
		sp_bg:AddChild(layou)
		PosTool.Center(layou)
	end
end

function LogicGUI.TitleBarLayer:__CreateLevel()
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

	local layout = Util:Layout(220, 28)
    layout:SetAnchorPoint(1, 0.5)
    self:AddChild(layout)    
    PosTool.RightCenter(layout, self.__rt_offset)
	self.__rt_offset = self.__rt_offset + self.__rt_step    

    self.__txt_level = Util:Label(role_info.level..CPPGameLib.GetString("common_level_suffix") )
    self.__txt_level:SetAnchorPoint(0, 0.5)
    layout:AddChild(self.__txt_level)
    PosTool.LeftCenter(self.__txt_level, 0, -1)

	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_preview")
	local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_main_bg")
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, 220, 20, 224, 24, role_info.exp, role_info.max_exp)
    self.__bar_exp:SetAnchorPoint( 0 , 0.5)
    self.__bar_exp:SetPosition( self.__txt_level:GetPositionX() + self.__txt_level:GetContentWidth() + 5 , self.__txt_level:GetPositionY() )
    layout:AddChild(self.__bar_exp:GetNode())
end

function LogicGUI.TitleBarLayer:__RegistAllEvents()
	--变化飘字
	local function Change( node , slab , blab )
		slab = tonumber(slab) or 0
		blab = tonumber(blab) or 0
		local text = ""
		local color = ""

		if slab > blab then
			text = "-" .. math.abs( slab - blab )
			color = Macros.Color.red_hex
		else
			text = "+" .. math.abs( slab - blab )
			color = Macros.Color.green_hex
		end

		--飘增加的经验
		local lab = TypefaceEffects:LittleFloat( text , color , 1 , 0 , 0 , nil , 1)
		lab:SetPosition( node:GetContentWidth(), node:GetContentHeight()/2 + lab:GetContentHeight() )
		node:AddChild(lab)
	end

	self.faceEffects = {}

	--属性变化
	self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE, function(_, type, old_var, new_var)
		if type == Macros.Game.RolePropType.EXP then
			--刷新经验
			if self.__bar_exp then
				self.__bar_exp:SetProgress(new_var)
			end
		else
			--刷新属性
			local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")		
			local prop_name = Macros.Game.RolePropKey[type]
			local widget_name = "__txt_prop_"..prop_name
			if prop_name and self[widget_name] then
				local prop_name_max = prop_name.."_max"	
				if role_info[prop_name_max] then
					if not self.faceEffects then
						self.faceEffects = {}
					end
					if not self.faceEffects[type] then
						self.faceEffects[type] = TypefaceEffects.New()
					end
					self.faceEffects[type]:Roll( self[widget_name] , new_var , "/"..role_info[prop_name_max] ,function ( ... )
						if old_var ~= new_var then
							Change( self[widget_name]:GetParent() , old_var , new_var)
						end
					end)
				else
					if not self.faceEffects[type] then
						self.faceEffects[type] = TypefaceEffects.New()
					end
					self.faceEffects[type]:Roll( self[widget_name] , new_var , nil , function ( ... )
						if old_var ~= new_var then
							Change( self[widget_name]:GetParent() , old_var , new_var)
						end
					end)

				end
			end
		end
	end)	

	--等级变化
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function(_, old_var, new_var)
		if self.__txt_level then
			self.__txt_level:SetString(tostring(new_var) .. CPPGameLib.GetString("common_level_suffix") )
		end

		--刷新属性
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
		local widget_name, prop_name_max
		for _, prop_name in pairs(Macros.Game.RolePropKey) do
			widget_name = "__txt_prop_"..prop_name
			if self[widget_name] then
				prop_name_max = prop_name.."_max"
				if role_info[prop_name_max] then
					self[widget_name]:SetString(role_info[prop_name].."/"..role_info[prop_name_max])
				end				
			end
		end
	end)		
end

function LogicGUI.TitleBarLayer:SetTitleBgVisible(is_visible)
	if self.__show_bg then
		self.__show_bg:SetVisible(is_visible)
	end
end