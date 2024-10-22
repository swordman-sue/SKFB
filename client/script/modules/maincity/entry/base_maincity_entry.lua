
--[[
%% @module: 主界面入口基类
%% @author: swordman sue
%% @created: 2018-11-6
--]]

Modules = Modules or {}

Modules.BaseMainCityEntry = Modules.BaseMainCityEntry or BaseClass(UILayer)

function Modules.BaseMainCityEntry:__init(region_layout_info, entry_info, region_panel)
	self.__entry_effect_map = {}

	self.__region_layout_info = region_layout_info

	self.__entry_info = entry_info

	self.__system_id = entry_info.system_id

	self.__region_panel = region_panel

	self.__region_panel:AddChild(self.__node, nil, entry_info.id)

	--入口背景图
	local entry_bg
	if region_layout_info.entry_widget_bg and entry_info.system_id ~= Macros.Game.SystemID.HERO_ADVENT then
		entry_bg = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, region_layout_info.entry_widget_bg))
	else
		entry_bg = Util:Layout()
	end
	entry_bg:SetAnchorPoint(region_layout_info.entry_widget_anchor_x, region_layout_info.entry_widget_anchor_y)
	entry_bg:SetContentSize(region_layout_info.item_w, region_layout_info.item_h)
	self:AddChild(entry_bg, nil, entry_info.id)

	--入口按钮
	self.__entry_btn = Util:Button(Resource.PathTool.GetSystemEntryIconPath(entry_info.res_name), function ()
		self:__HandleEntryTouch()
	end)			
	self.__entry_btn:SetTag(entry_info.id)
	self.__entry_btn:SetAnchorPoint(0.5, 0)
	self.__entry_btn:SetScaleFactor(entry_info.icon_size or 1)
	entry_bg:AddChild(self.__entry_btn)			
	PosTool.CenterBottom(self.__entry_btn, region_layout_info.entry_btn_offset_x, region_layout_info.entry_btn_offset_y)

	--入口文字
	if entry_info.module_txt then
		--文字特殊处理
	    local module_txt = self:__GetSpecialEntryTxt()	
	    self.__entry_txt = Util:Label(module_txt, region_layout_info.entry_txt_size, Macros.Color.btn, 0, 0, region_layout_info.module_txt_stroke and region_layout_info.module_txt_stroke or Macros.Color.lilyBlack_stroke,2)				
		self.__entry_txt:SetAnchorPoint(0.5, 0)
		self.__entry_txt:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
		self.__entry_txt:SetTextVertiacalAlign(LabelEx.ALIGIN_MIDDLE)
		self.__entry_txt:SetLineSpacing(-3)
		entry_bg:AddChild(self.__entry_txt)
		PosTool.CenterBottom(self.__entry_txt, region_layout_info.entry_txt_offset_x, region_layout_info.entry_txt_offset_y)	
	end
					
	--红点图标
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__entry_red_dot:SetAnchorPoint(1, 1)
	entry_bg:AddChild(self.__entry_red_dot,1)
	PosTool.RightTop(self.__entry_red_dot,region_layout_info.entry_red_dot_offset_x,region_layout_info.entry_red_dot_offset_y)						
	self.__entry_red_dot:SetVisible(false)

	--特效提示
	local effect_info = GlobalMainCityEntryMgr:GetEffectInfo(self.__system_id)
	if effect_info then
		local effect
		self.__entry_effect_map[self.__system_id],effect = self:__CreateEffect({res_name = effect_info.res_name}, self.__entry_btn, 99)
		PosTool.Center(effect, effect_info.offset_x, effect_info.offset_y)
	end

	--限时活动相关提示
	for system_id, system_info in pairs(CPPGameLib.GetConfig("ActivityAdvanceNotice", nil, nil , true )) do
        if system_id == self.__entry_info.system_id then
        	--创建限时活动倒计时
        	self:CreateLimitActivityTimeTxt()
        	--背景
        	local show_txt,is_bg_visible = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonDesc", system_id)
        	self.__bg_time:SetVisible(is_bg_visible)
        	--倒计时
			self.__activity_txt:SetVisible(is_bg_visible)
			break
        end
    end

	--根据系统预告和开放等级设置入口按钮
	self:__ResetEntryByOpenLevel()    
end

function Modules.BaseMainCityEntry:__delete()
	self:DeleteEffectById()
end

function Modules.BaseMainCityEntry:RegistAllEvents()
	--等级变化
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function()
		self:__HandleLevelChange()
	end)

	--更新限时活动时间
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_TIME_RESP, function(_, system_id, show_txt, is_open)
		self:__UpdateLimmitActivity(system_id, show_txt, is_open)
	end)
end

--初始化入口位置
function Modules.BaseMainCityEntry:InitPosition(index, entry_list)
	local entry_widget_x = self.__region_layout_info.entry_widget_border_x + (self.__region_layout_info.index % self.__region_layout_info.col) * self.__region_layout_info.item_margin_x
	local entry_widget_y = self.__region_layout_info.entry_widget_border_y + math.floor(self.__region_layout_info.index / self.__region_layout_info.col) * self.__region_layout_info.item_margin_y

	--“更多”入口特殊处理
	if self.__entry_info.is_more then
		self:SetPosition(self.__region_layout_info.entry_widget_border_x, self.__region_layout_info.entry_widget_border_y + 198)
		self.__entry_btn:SetAnchorPoint(0.5, 0.5)
		PosTool.CenterBottom(self.__entry_btn, self.__region_layout_info.entry_btn_offset_x, self.__region_layout_info.entry_btn_offset_y + 37)
	
	--特殊布局
	elseif self.__region_layout_info.is_special_func then
		local org_pos = -(#entry_list - 1) * 90/2
		local org_dis = #entry_list == 1 and 0 or math.abs(org_pos/(#entry_list-1))*2
		PosTool.CenterBottom(self.__node, self.__region_layout_info.index*org_dis+org_pos, 0)
	
	--常规布局
	elseif self.__region_layout_info.pos_func then
		self.__region_layout_info.pos_func(self.__node, entry_widget_x, entry_widget_y)

	else
		self:SetPosition(entry_widget_x, entry_widget_y)
	end

	self.__node:SetZOrder(100 - index)
	self.__region_layout_info.index = self.__region_layout_info.index + 1
end

function Modules.BaseMainCityEntry:SetInitSuccess()
	self.__is_init_finished = true
end

function Modules.BaseMainCityEntry:IsInitSuccess()
	return self.__is_init_finished
end

function Modules.BaseMainCityEntry:GetEntryInfo()
	return self.__entry_info
end

function Modules.BaseMainCityEntry:GetLayoutInfo()
	return self.__region_layout_info
end

function Modules.BaseMainCityEntry:GetEntryBtn()
	return self.__entry_btn
end

function Modules.BaseMainCityEntry:GetEntryRedDot()
	return self.__entry_red_dot
end

--删除特效
function Modules.BaseMainCityEntry:DeleteEffectById(effect_id)
	if not effect_id then
		for _,del_id in pairs(self.__entry_effect_map) do
			self:__DelEffect(del_id)
		end
		self.__entry_effect_map = {}
	else
		--删除特效
		if self.__entry_effect_map[effect_id] then
			self:__DelEffect(self.__entry_effect_map[effect_id])
			self.__entry_effect_map[effect_id] = nil
		end
	end

	if self.__lead_player_attack_effectid then
		self:__DelEffect(self.__lead_player_attack_effectid)
		self.__lead_player_attack_effectid = nil
	end
end

function Modules.BaseMainCityEntry:IsEffectExist(res_name)
	return self.__entry_effect_map[res_name]
end

--创建限时活动倒计时
function Modules.BaseMainCityEntry:CreateLimitActivityTimeTxt()
	if not self.__bg_time then
		--时间底
		self.__bg_time = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY,"bg_time"))
		self:AddChild(self.__bg_time)
		PosTool.CenterBottom(self.__bg_time, self.__region_layout_info.entry_txt_offset_x, self.__region_layout_info.entry_txt_offset_y - 10)
	end
	if not self.__activity_txt then
		--倒计时
	   	self.__activity_txt = Util:RichText("", Macros.TypefaceSize.minimum,0,0,Macros.Color.green_hex)
		self.__activity_txt:SetAnchorPoint(0.5, 0)
		self:AddChild(self.__activity_txt)
		PosTool.CenterBottom(self.__activity_txt, self.__region_layout_info.entry_txt_offset_x, self.__region_layout_info.entry_txt_offset_y - 17)
	end
end

--更新入口红点
function Modules.BaseMainCityEntry:UpdateEntryRedDot(bubbling)
	if not self:IsInitSuccess() then
		return
	end

    local is_open, is_pre_open = GlobalModulesMgr:IsOpen(self.__system_id)   
    if not is_open and self.__system_id ~= nil then
    	return
    end

	--限时副本汇总
	if self.__system_id == Macros.Game.SystemID.LIMIT_DUNGEON then
		local lst = {
				[1] = Macros.Game.SystemID.WORLDBOSS,
				[2] = Macros.Game.SystemID.PVP,
				[3] = Macros.Game.SystemID.MONSTER_STORMCASTLE,
		}
		for i , system_id in ipairs( lst ) do
			local system = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "IsLimitDungeonOpen", system_id)
			self.__entry_red_dot:SetVisible( system )
			if system then
				break
			end
		end

	--我要变强
	elseif self.__system_id == Macros.Game.SystemID.STRONGER then
		local var = CallModuleFunc(ModuleType.STRONGER , "GetisFirstOpen")
		self.__entry_red_dot:SetVisible( var )

	--挑战入口
	elseif self.__system_id == Macros.Game.SystemID.CHALLENGE then
		local red_num = 0
		for sys_id, v in pairs(CPPGameLib.GetConfig("Challenge", nil, nil, true)) do
			red_num = red_num + CallModuleFunc(ModuleType.RED_MGR, "GetRed", sys_id, Macros.Game.SystemID.MAX)
			if red_num > 0 then
				break
			end
		end
		self.__entry_red_dot:SetVisible(red_num)

	--其他
	else
		local red_dot_num
		if self.__entry_info.is_more then
			red_dot_num = CallModuleFunc(ModuleType.MAIN_CITY, "GetEneryMoreRedDotNum")

		elseif self.__entry_info.is_extend then
			red_dot_num = CallModuleFunc(ModuleType.MAIN_CITY, "GetEneryExtendRedDotNum", self.__entry_info.is_extend)

		else
			red_dot_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", self.__system_id, Macros.Game.SystemID.MAX)
		end		
		self.__entry_red_dot:SetVisible(red_dot_num > 0)
	end

	--二级或扩展区域中的入口红点发生变化，需要冒泡刷新对应的一级入口红点
	if bubbling ~= false then
		--刷新二级区域对应的一级入口红点
		if self.__region_layout_info.is_second_region then
			GlobalMainCityEntryMgr:UpdateEntryRedDot(BOTTOM_LEFT_MORE_ENERY_ID)

		--刷新扩展区域对应的一级入口红点
		elseif self.__region_layout_info.is_extend_region then
			local entry_id = CallModuleFunc(ModuleType.MAIN_CITY, "TranslateSystemIDToEntryID", self.__entry_info.extend_systemid)
			GlobalMainCityEntryMgr:UpdateEntryRedDot(entry_id)
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--获取特殊入口文字
function Modules.BaseMainCityEntry:__GetSpecialEntryTxt()
	local module_txt = self.__entry_info.module_txt
    local limmit_activity_info = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonInfo",self.__entry_info.system_id)
   
	--不限定任何系统
	if self.__entry_info.system_id and limmit_activity_info then
		module_txt = limmit_activity_info.module_txt_list[limmit_activity_info.mark_index]
	end
	return module_txt		
end

--根据系统预告和开放等级设置入口按钮
function Modules.BaseMainCityEntry:__ResetEntryByOpenLevel()
	if self.__system_id then
	    local is_open, is_pre_open = GlobalModulesMgr:IsOpen(self.__system_id)

		--设置按钮状态	   	
	   	self.__entry_btn:SetBright(is_open)

	    --设置锁的显隐
	    if not is_open and not self.__sprite_entry_lock then
	    	self.__sprite_entry_lock = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))
	    	self.__entry_btn:AddChild(self.__sprite_entry_lock)
	    	PosTool.Center(self.__sprite_entry_lock)
	    end
	    if self.__sprite_entry_lock then
	    	self.__sprite_entry_lock:SetVisible(not is_open)
	    end
	end	
end

--处理入口点击
function Modules.BaseMainCityEntry:__HandleEntryTouch()
	--更多或者二级入口					
	CallModuleFunc(ModuleType.MAIN_CITY, "HandleEntryTouch", self.__entry_info, self.__node)
end

--等级变化相关处理
function Modules.BaseMainCityEntry:__HandleLevelChange()
	self:__ResetEntryByOpenLevel()
end

--更新限时活动图标
function Modules.BaseMainCityEntry:__UpdateLimmitActivity(system_id,show_txt,is_open)
	if not self:IsInitSuccess() then
		return
	end

	--系统入口Id转换为主界面入口Id
	if system_id == self.__system_id then
		if self.__bg_time then
			self.__bg_time:SetVisible(not is_open)
		end
		if self.__activity_txt then
			local lab = GUI.RichText.GetColorWord(show_txt, Macros.Color.green_hex)
			self.__activity_txt:SetText(lab)
			self.__activity_txt:SetVisible(not is_open)
		end
	end
end