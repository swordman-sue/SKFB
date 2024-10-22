
Modules = Modules or {}

--战斗通用奖励模板(金币、经验)
Modules.TemplBattleCommonReward = Modules.TemplBattleCommonReward or BaseClass(GUI.Template)

--[[
@ params：
	parent_node
	add_gold
	add_exp
	add_type = {{id = 1,value = 2}}
	time

	callback
--]]
function Modules.TemplBattleCommonReward:__init(params)
	self.__layout_mode = GUI.Template.LayoutMode.CODE	
	self:InitTemplate(nil, params.parent_node)

	self.params = params
	self.tableLst = {}
	self.widget_list = {}
	self.effects = {}

	local t1 = {}
	if params.add_gold then
		table.insert( t1 , { id = Macros.Game.RolePropType.GOLD , value = params.add_gold  } )
	end
	if params.add_exp then
		table.insert( t1 , { id = Macros.Game.RolePropType.EXP , value = params.add_exp  } )
	end

	if params.add_type then
		for i,v in ipairs(params.add_type) do
			if v.id then
				table.insert( t1 , { id = v.id , value = v.value  } )
			elseif v.text then
				table.insert( t1 , { text = v.text , value = v.value  } )
			end
		end
	end
	self:__ShowUI(t1)

	params.lst = t1
	self:__DoAnimate(params)
end

function Modules.TemplBattleCommonReward:__delete()
	for i=1,999 do
		if self.effects[i] then
	        self.effects[i]:DeleteMe()
	        self.effects[i] = nil
		end
	end
end

function Modules.TemplBattleCommonReward:Update(elapse_time)
	if not self.__is_anim_started then
		return
	end
	self.__anim_elapse_time = self.__anim_elapse_time + elapse_time

	local factor = self.__anim_elapse_time / self.__anim_total_time
	if self.__anim_elapse_time >= self.__anim_total_time then
		self.__is_anim_started = false

		for i,v in ipairs(self.__params.lst) do
			if self.widget_list[i][3] then
				self.widget_list[i][3]:SetString(tostring(v.value))	
			end

			if v.id == Macros.Game.RolePropType.EXP then
				if v.value > 0 then
					self.__txt_lev:SetString(CPPGameLib.GetString("common_lev6", tonumber(self.__end_lev or 0 ) ))
					-- self.__bar_exp:SetProgressMax(self.__bar_end_max_exp)
					-- self.__bar_exp:SetProgress(self.__bar_end_exp)
				end  
			end
		end
		return
	end

	for i,v in ipairs(self.__params.lst) do
		if v.value > 0 then
			local new_exp = math.floor(v.value * factor)
			if self.widget_list[i][3] then
				self.widget_list[i][3]:SetString(tostring(new_exp))	
			end
		end
	end
end

function Modules.TemplBattleCommonReward:UpExpDate()
	if not self.__is_anim_started then
		return
	end

	if self.__anim_elapse_time >= self.__anim_total_time then
		for i,v in ipairs(self.__params.lst) do
			if v.id == Macros.Game.RolePropType.EXP then
				if v.value > 0 then
					self.__bar_exp:SetProgressMax(self.__bar_end_max_exp)
					self.__bar_exp:SetProgress(self.__bar_end_exp)
				end  
			end
		end
		return
	end
end


function Modules.TemplBattleCommonReward:__CreateWidgetFromLuaCode(params)
	self.layout = Util:Layout()
	self.layout:SetContentSize( 622.00 , 94)
	return self.layout
end

function Modules.TemplBattleCommonReward:__ShowUI( lst )
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local role_info = battle_info.pre_role_info

	--等级and经验条容器
	self.level_layer = Util:Layout(0,0)
	self.level_layer:SetAnchorPoint(0.5,0.5)
	self.level_layer:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2)
	self.layout:AddChild(self.level_layer)

	--等级
	self.__txt_lev = Util:Label(CPPGameLib.GetString("common_lev6", role_info.level) , Macros.TypefaceSize.popup )
	self.__txt_lev:SetAnchorPoint(0, 0)
	self.level_layer:AddChild(self.__txt_lev)

	--经验条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_preview")
	local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_interior_bg1")
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, 355, 20, 360, 24, role_info.exp, role_info.max_exp)
    self.__bar_exp:SetAnchorPoint(0, 0)
    self.level_layer:AddChild(self.__bar_exp:GetNode())

	if self.params.notShowExp then
		self.__txt_lev:SetVisible(false)
		self.__bar_exp:SetVisible(false)
	end

    --重设容器大小
	self.level_layer:SetContentSize( self.__txt_lev:GetContentWidth() + 10 + self.__bar_exp:GetNode():GetContentWidth() , 94.00 )

    --重设组件位置
	self.__txt_lev:SetPosition( 0 , 20 )
	self.__bar_exp:SetPosition( self.__txt_lev:GetContentWidth() + 10 , 20 )

	-- CPPActionManager.FadeOut(self.level_layer , 0 )
	self.level_layer:SetOpacity(0)

	self.layer_text = Util:Layout(0,0)
	self.layer_text:SetAnchorPoint(0.5,0.5)
	self.layer_text:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2)
	CPPActionManager.FadeOut(self.layer_text, 0 )
	self.layout:AddChild(self.layer_text)

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

	for i,v in ipairs(lst) do
		local t1 = {}
		if v.id then

			local battle_double = ""
			--竞技场双倍
			if battle_info.type == Macros.Game.FBType.ARENA then
				local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.ARENA )
				if num ~= 1 then
					battle_double = CPPGameLib.GetString("battle_double")
				end
			end

			local text = CPPGameLib.GetString("currency_get_"..v.id)
			t1 = {
					{zi = CPPGameLib.GetString("currency_get" , text  ) , ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
					{tp = Util:GetMoneyIconImgData(v.id) , dx = 0.85, jg = 5},
					{zi = v.value , jg = 2 , dx = Macros.TypefaceSize.popup },
					{zi = battle_double , ys = Macros.Color.green , dx = Macros.TypefaceSize.popup },
				}

		elseif v.text then
			t1 = {
					{zi = v.text , ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
					{zi = "" , ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
					{zi = v.value , jg = 2 , dx = Macros.TypefaceSize.popup },
				}
		end

		local lst = nil
		self.tableLst[i] , lst = Util:LabCenter( t1 )
		self.tableLst[i]:SetAnchorPoint(0.5 , 0.5)
		self.layer_text:AddChild(self.tableLst[i])

		if #lst == 4 then
			self.widget_list[i] = {}
			for j , vv in ipairs( lst ) do
				if j ~= 1 then
					table.insert( self.widget_list[i] , vv )
				end
			end
			table.insert( self.widget_list[i] ,1, lst[1] )
		else
			self.widget_list[i] = lst
		end
	end

	local jg = 50

	if #self.tableLst <= 2 then
		jg = 80
	end

	local width = 0
	for k,v in ipairs(self.tableLst) do
		width = width + v:GetContentWidth() + jg 
	end

	self.layer_text:SetContentSize( width , 94.00 )

	local interval = width / #self.tableLst

	local posx = 0
	for i=1 , #self.tableLst do
		self.tableLst[i]:SetPosition( (interval/2) + interval * (i - 1) , self.layer_text:GetContentHeight()/2 + 25 )
	end
end

function Modules.TemplBattleCommonReward:__DoAnimate(params)
	self.__anim_end_callback = params.callback

	local key_gold = nil
	local key_exp = nil

	local add_exp = 0

	for i,v in ipairs(params.lst) do
		if v.id == Macros.Game.RolePropType.GOLD then
			if v.value <= 0 then
				key_gold = true
			end
		elseif v.id == Macros.Game.RolePropType.EXP then
			if v.value <= 0 then
				key_exp = true
			else
				add_exp = v.value
			end
		end
	end

	if key_gold and key_exp then
		if self.__anim_end_callback then
			self.__anim_end_callback()
		end		
		return
	end

	self.__params = params
	self.__anim_elapse_time = 0
	self.__anim_total_time = params.time

	for i,v in ipairs( self.widget_list ) do
		if v[3] then
			v[3]:SetString("0")
		end
	end	

	CPPActionManager.FadeIn(self.layer_text, 0.1 )
	CPPActionManager.MoveTo(self.layer_text, 0.2 , self.layout:GetContentWidth()/2 + 7 , self.layout:GetContentHeight()/2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
	
		CPPActionManager.MoveTo(self.layer_text, 0.3 , self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2,  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			self.__is_anim_started = true	
			self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))
		end) )
	end) )

	--延时
	CPPActionManager.DelayTo(self.level_layer , 0.3 , GlobalCallbackMgr:AddCallBackFunc(function()
		--出现
		CPPActionManager.FadeIn(self.level_layer, 0.1)
		--移动
		CPPActionManager.MoveTo(self.level_layer, 0.2 , (self.layout:GetContentWidth()/2) + 7 , self.layout:GetContentHeight()/2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			--回弹
			CPPActionManager.MoveTo(self.level_layer, 0.3 , self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
				
				if add_exp> 0 then
					local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
					local role_info = battle_info.pre_role_info

					local speed = add_exp / (self.__anim_total_time - 0.1)
					self.__bar_exp:SetSpeed(speed)

					local delta_exp = role_info.exp + add_exp - role_info.max_exp

					if delta_exp < 0 then
						--未升级
						self.__end_lev = role_info.level
						self.__bar_end_exp = delta_exp + role_info.max_exp
						self.__bar_end_max_exp = role_info.max_exp
						self.__bar_exp:SetToProgress(self.__bar_end_exp)
					else
						--升级
						self.__end_lev = role_info.level + 1
						self.__bar_end_exp = delta_exp
						self.__bar_end_max_exp = ConfigAdapter.RoleLevel.GetMaxExp(self.__end_lev)
						self.__bar_exp:SetProgressEndCallback(function()
							self.__bar_exp:SetProgressEndCallback(nil)
							self.__bar_exp:SetProgressMax(self.__bar_end_max_exp)
							self.__bar_exp:SetSpeed(speed)
							self.__bar_exp:SetToProgress(self.__bar_end_exp)
						end)
						self.__bar_exp:SetToProgress(role_info.max_exp)
					end
				end
	
				if self.__anim_end_callback then
					self.__anim_end_callback()
				end

			end) )

		end) )

	end))
end
