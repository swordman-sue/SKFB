
--[[
%% @module：英雄阵位条目
%% @author: swordman sue
%% @created: 2016-08-23
--]]

Modules = Modules or {}

Modules.HeroLineupItem = Modules.HeroLineupItem or BaseClass(GUI.ScrollItem)

function Modules.HeroLineupItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self:InitTemplate()
end

function Modules.HeroLineupItem:__delete()
end

function Modules.HeroLineupItem:__CreateWidgetFromLuaCode()
	local layout = Util:Layout(88, 76)
	return layout
end

function Modules.HeroLineupItem:SetData(data)
	self:__Clear()

	GUI.ScrollItem.SetData(self, data)

	--阵位索引
	local lineup_index = self.__data.index	
	--阵位配置
	local pos_info = ConfigAdapter.Hero.GetLineupPosInfo(lineup_index)
	--英雄信息
	local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", lineup_index)

	--角色信息
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

	--阵位图标数据
	local icon_data
	if hero_info then
		icon_data = {id = hero_info.info_id, obj_info = hero_info}

		--新手指引特殊操作检测(阵容切换英雄)
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.LINEUP_CHANGE_HERO, self:GetNode(), hero_info.info_id)					
	end
	self.__data.icon_data = icon_data

	--计算红点数量
	local red_dot_num = 0
	--是否开启
	local is_open_lev = pos_info and pos_info.need_level <= role_info.level
	--是否显示图标数据
	local is_show_data = (is_open_lev and icon_data) and true or false
	if is_show_data then
		--英雄图标
		if not self.__icon_layer then
			self.__icon_layer = LogicGUI.IconLayer.New(false, false)
			self:AddChild(self.__icon_layer)
			PosTool.Center(self.__icon_layer)
		end
		self.__icon_layer:SetData(icon_data)
		--觉醒星数
		self.__icon_layer:ShowAwakenStar()
		--判断是否有红点
		self:JudgeHaveRedDot()
	else
		self.__sp_bg = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW, "icon_blank_frame"))
		self:AddChild(self.__sp_bg)			
		PosTool.Center(self.__sp_bg)

		if not is_open_lev then			
			--开启等级提示
			local sp_lock = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))		
			self.__sp_bg:AddChild(sp_lock)
			PosTool.Center(sp_lock)

		    local lab_open_lev = Util:Label(nil, nil, Macros.Color.btn, 50,0,Macros.Color.button_yellow)
		    lab_open_lev:SetAnchorPoint(0.5, 0.5)
		    lab_open_lev:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
		    self.__sp_bg:AddChild(lab_open_lev)
		    PosTool.Center(lab_open_lev)			
			lab_open_lev:SetString(string.format(CPPGameLib.GetString("common_open_lev"), pos_info.need_level))
		else
			--上阵提示
			local sp_add = SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_num_add_big"))
			self.__sp_bg:AddChild(sp_add)
			PosTool.Center(sp_add)
			ActionManager.GetInstance():CycleBlinkEffect(sp_add, 0.5)
			--红点
			local hero_info_list = CallModuleFunc(ModuleType.HERO,"GetLineupSelectList",nil,Macros.Game.HeroLineup.HERO_LINEUP,lineup_index,true)  
			if #hero_info_list > 0 then
				red_dot_num = red_dot_num + 1
				self:__AddRedDot()
			end
		end		
	end	
	if self.__icon_layer then
		self.__icon_layer:SetVisible(is_show_data)	
	end
end

function Modules.HeroLineupItem:SetSelected(var)
	if var then
		if not self.__sp_selected then
			self.__sp_selected = Util:Sprite9(
				Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected"), {10, 10})
			self.__sp_selected:SetContentSize(86, 86)
			self:AddChild(self.__sp_selected)
			PosTool.Center(self.__sp_selected)
		end
	end
	if self.__sp_selected then
		self.__sp_selected:SetVisible(var)
	end
end

function Modules.HeroLineupItem:JudgeHaveRedDot()
	--计算红点数量
	local red_dot_num = 0
	--英雄信息
	local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", self.__data.index)
	if hero_info then	
		local isCouldUpgrade = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldUpgrade",hero_info.uid) 
		if isCouldUpgrade then
			red_dot_num = red_dot_num + 1
			self:__AddRedDot()
		else
			local isCouldBreak = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldBreak",hero_info.uid) 
			if isCouldBreak then
				red_dot_num = red_dot_num + 1
				self:__AddRedDot()
			else
				local equip_reddot_num = CallModuleFunc(ModuleType.HERO,"GetHeroEquipRedDotNumByLineUpIndex",self.__data.index) 
				if equip_reddot_num > 0 then
					red_dot_num = red_dot_num + 1
					self:__AddRedDot()
				else
					local divination_reddot_table = CallModuleFunc(ModuleType.HERO,"GetHeroDivinationRedDotNumByLineUpIndex",self.__data.index) 
					for i,divination_reddot_num in ipairs(divination_reddot_table) do
						red_dot_num = red_dot_num + divination_reddot_num
					end
					if red_dot_num > 0 then						
						self:__AddRedDot()
					else
						self:__HideRedDot()
					end
				end
			end			
		end		
	end
end

--新手指引特殊操作检测(英雄上阵按钮)
function Modules.HeroLineupItem:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_LINEUP_BTN, self:GetNode())
end

--添加红点
function Modules.HeroLineupItem:__AddRedDot()
	self.__data.has_red_dot = true

	--红点
	if not self.entry_red_dot then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.entry_red_dot:SetAnchorPoint(1, 1)
		self:GetNode():AddChild(self.entry_red_dot,2)
		PosTool.RightTop(self.entry_red_dot)
	end
	self.entry_red_dot:SetVisible(true)
end

--隐藏红点
function Modules.HeroLineupItem:__HideRedDot()
	if self.entry_red_dot then
		self.entry_red_dot:SetVisible(false)
	end	
end

function Modules.HeroLineupItem:__Clear()
	if self.__sp_bg then
		self.__sp_bg:RemoveSelf(true)
		self.__sp_bg = nil
	end
end
