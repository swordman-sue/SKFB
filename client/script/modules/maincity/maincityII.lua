--[[
%% @module：新版主界面(1、去掉上阵英雄。2、部分接口改为建筑。)
]]--

Modules = Modules or {}

Modules.MaincityViewII = Modules.MaincityViewII or BaseClass(Modules.MaincityView)

local MOVE_LAYERS_SPEED = 500

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--刷新角色等级
function Modules.MaincityViewII:HandleRoleLevelChange(old_var, new_var)
	Modules.MaincityView.HandleRoleLevelChange(self, old_var, new_var)

	--刷新建筑开锁
	self:HandleUpdateBuildingOpen()
end

--刷新建筑开锁
function Modules.MaincityViewII:HandleUpdateBuildingOpen()
	if self.__maincity_layer_building_info_list then
		--遍历信息表
		for _,icon_info in ipairs(self.__maincity_layer_building_info_list) do
			--系统信息
			local is_open = GlobalModulesMgr:IsOpen(icon_info.system_id)
			--锁
			icon_info.lock:SetVisible(not is_open)
			--开启等级
			icon_info.open_level:SetVisible(not is_open)
			--红点					
			if not is_open and icon_info.entry_red_dot then
				icon_info.entry_red_dot:SetVisible(false)
			end
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数（初始化相关）
--------------------------------------------------------------------------------------------------------------------------------
function Modules.MaincityViewII:__RegistAllEvents()
	Modules.MaincityView.__RegistAllEvents(self)

	--红点更新
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		if self.__maincity_layer_building_info_list then
			for _, icon_info in ipairs(self.__maincity_layer_building_info_list) do
				if not system_id or system_id == icon_info.system_id then
					local is_open = GlobalModulesMgr:IsOpen(icon_info.system_id)
					local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", icon_info.system_id)
					icon_info.entry_red_dot:SetVisible(is_open and red_num > 0)

					if system_id then
						break
					end
				end
			end
		end
	end)	
end

--初始化上阵英雄
function Modules.MaincityViewII:__InitLineupHeros()
end

--更新主界面效果
function Modules.MaincityViewII:__UpdateMaincityEffect()
end

--创建背景层
function Modules.MaincityViewII:__CreateLayers()
	--天空层
	self.__sky_layer_params = {image_path_list = {"bg_sp_aoyou_background"},image_size = COCOSize(1136, 640)}
	self.__sky_layer = Modules.MainCityLayerBG.New(self.__sky_layer_params,self)					
	self:AddChild(self.__sky_layer, Macros.Global.z_order_bottom)
	PosTool.Center(self.__sky_layer)	

	--触摸层
	self.__touch_layer_params = {image_size = COCOSize(1136, 640),bg_func = function (x,y)
		self.__begin_offsetx = x
		self.__begin_offsety = y
	end,touch_func = function (x,y)
		local offset_x = x - self.__begin_offsetx
		local offset_y = y - self.__begin_offsety
		self:__MovingBottomLayer(offset_x,offset_y)
		self.__begin_offsetx = x
		self.__begin_offsety = y
	end,ed_func = function (x,y)
	end}
	self.__touch_layer = Modules.MainCityLayerBG.New(self.__touch_layer_params,self)					
	self:AddChild(self.__touch_layer, Macros.Global.z_order_bottom)
	PosTool.Center(self.__touch_layer)	

	--主城层
	self.__maincity_layer_params = {image_path_list = {"bg_sp_aoyou_ground"},image_size = COCOSize(1136, 640)}
	self.__maincity_layer = Modules.MainCityLayerBG.New(self.__maincity_layer_params,self)					
	self:AddChild(self.__maincity_layer, Macros.Global.z_order_bottom)
	PosTool.Center(self.__maincity_layer)	

	--创建图标
	self.__maincity_layer_building_info_list = {{pos = {473,449.5},res_name = "building_team",system_id = Macros.Game.SystemID.LIMIT_TEAM},
	{pos = {175.5,417.5},res_name = "building_arena",system_id = Macros.Game.SystemID.ARENA},
	{pos = {881,380},res_name = "building_tower",system_id = Macros.Game.SystemID.TOWER},
	{pos = {645,273.5},res_name = "building_union",system_id = Macros.Game.SystemID.UNION},
	{pos = {253.5,207},res_name = "building_divination",system_id = Macros.Game.SystemID.DIVINATION},
	{pos = {896.5,171.5},res_name = "building_recruit",system_id = Macros.Game.SystemID.HERO_RECRUIT}}

	for _,icon_info in ipairs(self.__maincity_layer_building_info_list) do
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, icon_info.res_name)
		icon_info.building = Util:Button(sp,function ()
			--都要先检查是否已经开启
			if not GlobalModulesMgr:IsOpen(icon_info.system_id,true) then
				return
			end
			if icon_info.system_id == Macros.Game.SystemID.UNION then
				--军团
				local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
				if role_info.guild_id == 0 then
					CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "union" )
		    	else
					--系统模块跳转
					GlobalModulesMgr:OpenSystemModule(icon_info.system_id, nil, false)
		    	end
		    elseif icon_info.system_id == Macros.Game.SystemID.LIMIT_TEAM then
		    	--组队
		    	CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1)
		    else
		    	--系统模块跳转
				GlobalModulesMgr:OpenSystemModule(icon_info.system_id)
		    end
		end)
		self.__maincity_layer:GetNode():AddChild(icon_info.building,3)
		icon_info.building:SetPosition(icon_info.pos[1],icon_info.pos[2])
		--名字背景
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_aoyou_building_name")
		local name_bg = Util:Sprite9(sp)
		name_bg:SetContentWidth(150)
		icon_info.building:AddChild(name_bg)
		PosTool.CenterBottom(name_bg,0,25)
		--名字
		local system_config = CPPGameLib.GetConfig("SystemEntry",icon_info.system_id)
		local is_open = GlobalModulesMgr:IsOpen(icon_info.system_id)
		local txt = Util:RichText(
								string.format(Macros.Color.dungeon_chapter_name_rt_gradient, system_config.name),
								Macros.TypefaceSize.popup, nil, nil, nil, Macros.Color.white_stroke_hex, 4
							)
		txt:SetAnchorPoint(0.5,0.5)
		name_bg:AddChild(txt)
		PosTool.Center(txt)
		--锁
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon")
		icon_info.lock = Util:Sprite(sp)
		icon_info.lock:SetScaleFactor(0.5)
		icon_info.lock:SetAnchorPoint(1,0.5)
		name_bg:AddChild(icon_info.lock)
		PosTool.RightCenter(icon_info.lock)
		icon_info.lock:SetVisible(not is_open)
		--开启等级
		icon_info.open_level = Util:Label(CPPGameLib.GetString("dungeon_daily_open_level",system_config.level), Macros.TypefaceSize.slightly,Macros.Color.red,0,0,Macros.Color.red_stroke,2)
		icon_info.open_level:SetAnchorPoint(0.5,1)
		name_bg:AddChild(icon_info.open_level)
		PosTool.CenterBottom(icon_info.open_level,0,-2)
		icon_info.open_level:SetVisible(not is_open)
		--红点
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		icon_info.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		icon_info.entry_red_dot:SetAnchorPoint(1, 1)
		name_bg:AddChild(icon_info.entry_red_dot,1)
		PosTool.RightTop(icon_info.entry_red_dot)						
		local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", icon_info.system_id)
		icon_info.entry_red_dot:SetVisible(is_open and red_num > 0 or false)
	end	
	
	--模板层
	self.__test_layer_params = {image_size = COCOSize(1136, 640)}
	self.__test_layer = Modules.MainCityLayerBG.New(self.__test_layer_params,self)					
	self:AddChild(self.__test_layer, Macros.Global.z_order_bottom)
	PosTool.Center(self.__test_layer)	

	--若背景图可点击，则设置根节点可穿透
	self.__root_widget:SetTouchSwallow(false)	
end

--检测点击建筑
function Modules.MaincityViewII:__CheckClickBuildingLead()
	local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")   	
	if self.__maincity_layer_building_info_list then
		for _,building_info in ipairs(self.__maincity_layer_building_info_list) do
			--新手指引检测点击建筑
			if operate_id == Macros.Game.LeadOperateSpecialID.MAINCITY_CLICK_BUILDING and data_id == building_info.system_id then					
				if (building_info.pos[1]/Macros.Global.DesignSize.width) >= 0.75 then
					self:__MovingBottomLayer(-Macros.Global.DesignSize.width,nil,true)
				end
				CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAINCITY_CLICK_BUILDING, building_info.building, building_info.system_id)
			end
		end			
	end
end

--滚屏
function Modules.MaincityViewII:__MovingBottomLayer(offset_x,offset_y,is_sp)
	local cur_moving_dir = offset_x >= 0 and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT
	if (cur_moving_dir == self.__cur_moving_dir) and self.__on_moving_layers then
		return		
	end
	self.__cur_moving_dir = cur_moving_dir	
	
	--如果小于屏幕一半则为屏幕一半
	local actrual_offsetx = offset_x*12
	local cur_layer_posx = self.__test_layer:GetPositionX() + self:GetNode():GetContentWidth()/2 - self.__test_layer:GetWidth()
	if offset_x >= 0 then
		if cur_layer_posx >= 0 then
			return
		elseif (cur_layer_posx + actrual_offsetx) >= 0 then
			actrual_offsetx = -cur_layer_posx	
		end
	else	
		if cur_layer_posx <= (Macros.Global.DesignSize.width - self.__sky_layer:GetWidth()) then
			return
		elseif (cur_layer_posx + actrual_offsetx) <= (Macros.Global.DesignSize.width - self.__sky_layer:GetWidth()) then
			actrual_offsetx = Macros.Global.DesignSize.width - self.__sky_layer:GetWidth() - cur_layer_posx
		end
	end
	if math.abs(actrual_offsetx) > 0 then
		CPPActionManager.StopAllActions(self.__sky_layer:GetNode())
		CPPActionManager.StopAllActions(self.__maincity_layer:GetNode())
		CPPActionManager.StopAllActions(self.__test_layer:GetNode())		
		self.__on_moving_layers = true
		local cur_speed = is_sp and MOVE_LAYERS_SPEED*5 or MOVE_LAYERS_SPEED
		local speed_list = ConfigAdapter.Common.GetMonsterStormCastleSceneSpeed()
		CPPActionManager.MoveBy(self.__sky_layer:GetNode(),math.abs(actrual_offsetx)/cur_speed,speed_list[1]*actrual_offsetx,0)
		CPPActionManager.MoveBy(self.__maincity_layer:GetNode(),math.abs(actrual_offsetx)/cur_speed,speed_list[5]*actrual_offsetx,0,GlobalCallbackMgr:AddCallBackFunc(function()
			self.__on_moving_layers = false
		end))	
		CPPActionManager.MoveBy(self.__test_layer:GetNode(),math.abs(actrual_offsetx)/cur_speed,actrual_offsetx,0)
	end
end