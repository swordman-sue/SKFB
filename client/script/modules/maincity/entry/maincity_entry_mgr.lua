
--[[
%% @module: 主界面入口管理器
%% @author: swordman sue
%% @created: 2018-11-6
--]]

Modules = Modules or {}

Modules.MainCityEntryMgr = Modules.MainCityEntryMgr or BaseClass(EventAndTimerWrapper)

local get_entry_class

function Modules.MainCityEntryMgr.CreateInstance()
	if not GlobalMainCityEntryMgr then
		GlobalMainCityEntryMgr = Modules.MainCityEntryMgr.New()
	end
end

function Modules.MainCityEntryMgr.DelInstance()
	if GlobalMainCityEntryMgr then
		GlobalMainCityEntryMgr:DeleteMe()
		GlobalMainCityEntryMgr = nil
	end
end

function Modules.MainCityEntryMgr:__init()
	self.__entry_map = {}
	
	self.__entry_class_map = {
		--背包
		[Macros.Game.SystemID.PACK_EXTEND] = Modules.PackExtendEntry,
		[Macros.Game.SystemID.Pack] = Modules.PackExtendEntry,
		[Macros.Game.SystemID.EQUIP] = Modules.PackExtendEntry,
		[Macros.Game.SystemID.TREASURE] = Modules.PackExtendEntry,
		[Macros.Game.SystemID.HERO_BAG] = Modules.PackExtendEntry,
		[Macros.Game.SystemID.HERO_BAG] = Modules.PackExtendEntry,
		[Macros.Game.SystemID.DIVINATION_PACK] = Modules.PackExtendEntry,

		--主线副本
		[Macros.Game.SystemID.DUNGEON] = Modules.DungeonEntry,

		--军团
		[Macros.Game.SystemID.UNION] = Modules.UnionEntry,
		[Macros.Game.SystemID.LADDER] = Modules.LadderEntry,

		--组队
		[Macros.Game.SystemID.LIMIT_TEAM] = Modules.TeamEntry,
		[Macros.Game.SystemID.TEAM_INVITE] = Modules.TeamInviteEntry,

		--功能预告
		[Macros.Game.SystemID.SYSTEM_FORESHOW] = Modules.SystemForeshowEntry,

		--我要变强
		[Macros.Game.SystemID.STRONGER] = Modules.StrongerEntry,
		
		--首充
		--超值购
		[Macros.Game.SystemID.CUSTOM_PURCHASE] = Modules.ActivityEntry,
		--在线奖励
		[Macros.Game.SystemID.ONLINE_AWARD] = Modules.OnlineAwardEntry,
		[Macros.Game.SystemID.FIRST_RECHARGE] = Modules.FirstRechargeEntry,
		--次日登录
		[Macros.Game.SystemID.MORROW] = Modules.MorrowEntry,

		--Facebook分享
		[Macros.Game.SystemID.FACEBOOK_SHARE_REWARD] = Modules.FacebookShareRewardEntry,
		--AppStore五星好评
		[Macros.Game.SystemID.FIVESTAR_HIGHPRAISE] = Modules.FiveStarHighPraiseEntry,
	}

	--特效信息
	self.__entry_effect_map = {
		[Macros.Game.SystemID.CHARGE] = {res_name = "UI_chongzhitubiao",offset_x = -2,offset_y = -24},
		[Macros.Game.SystemID.DIRECT_BUY] = {res_name = "UI_shouchongtubiao",offset_x = -2,offset_y = -14},
		[Macros.Game.SystemID.LIMIT_ACTIVITY] = {res_name = "UI_shouchongtubiao",offset_x = 0,offset_y = -6},
		[Macros.Game.SystemID.ARENA_EXTRA] = {res_name = "UI_wangzherukou",offset_x = 0,offset_y = -4},
	}
end

function Modules.MainCityEntryMgr:__delete()
	--容错处理(清空所有入口，解绑相关事件与计时器，避免在主界面销毁后，还存在无效引用)
	self:ClearAllEntrys()
end

function Modules.MainCityEntryMgr:RegistAllEvents()
	--刷新入口红点
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		if system_id then
			local entry_id = CallModuleFunc(ModuleType.MAIN_CITY, "TranslateSystemIDToEntryID", system_id)
			self:UpdateEntryRedDot(entry_id)
		else
			for entry_id, entry in pairs(self.__entry_map) do
				self:UpdateEntryRedDot(entry_id, false)
			end
		end
	end)	
end

function Modules.MainCityEntryMgr:CreateEntry(region_layout_info, entry_info, region_panel)
	if not entry_info or not region_layout_info or not region_panel then
		LogTool.LogError("[Modules.MainCityEntryMgr:CreateModules] 找不到region_layout_info、entry_info、region_panel其中一个")	
		return
	end

	local entry = self:GetEntryById(entry_info.id) 
	if entry then
		entry:DeleteMe()
		LogTool.LogError("[Modules.MainCityEntryMgr:CreateEntry] Entry of id:%d has existed. so auto remove it", entry_info.id)
	end

	local entry_class = get_entry_class(self, entry_info.system_id) or Modules.BaseMainCityEntry
	entry = entry_class.New(region_layout_info, entry_info, region_panel)
	self.__entry_map[entry_info.id] = entry

	return entry
end

--删除某入口
function Modules.MainCityEntryMgr:DeleteEntry(entry_id)
	local entry = self:GetEntryById(entry_id)
	if not entry then
		return
	end	

	entry:DeleteMe()
	self.__entry_map[entry_id] = nil
end

--清空所有入口
function Modules.MainCityEntryMgr:ClearAllEntrys()
	if self.__entry_map then
		for _, entry in pairs(self.__entry_map) do
			entry:UnRegistAllEvents()
			entry:CancelAllTimers()
		end
		self.__entry_map = {}
	end
end

--获取指定区域的顶级、二级、扩展入口列表
function Modules.MainCityEntryMgr:GetEntrysByRegion(region_type, ignore_top_level_entrys)
	if not region_type then
		return
	end

	local entry_info, entry_list, region_layout_info
	if self.__entry_map then
		for _, entry in pairs(self.__entry_map) do
			entry_info = entry:GetEntryInfo()
			region_layout_info = entry:GetLayoutInfo()
			
			if entry_info and region_layout_info and 
				entry_info.region_type == region_type and 
				(not region_layout_info.is_top_level_region or not ignore_top_level_entry) then
				
				entry_list = entry_list or {}
				table.insert(entry_list, entry_info)
			end
		end
	end

	return entry_list
end

--获取指定区域的顶级、二级入口列表
function Modules.MainCityEntryMgr:GetEntrysExceptExtendRegion(region_type)
	if not region_type then
		return
	end

	local entry_info, entry_list, region_layout_info
	if self.__entry_map then
		for _, entry in pairs(self.__entry_map) do
			entry_info = entry:GetEntryInfo()
			region_layout_info = entry:GetLayoutInfo()
			
			if entry_info and region_layout_info and 
				entry_info.region_type == region_type and not region_layout_info.is_extend_region then
				
				entry_list = entry_list or {}
				table.insert(entry_list, entry_info)
			end
		end
	end

	return entry_list
end

--获取扩展区域的入口列表
function Modules.MainCityEntryMgr:GetEntrysOnlyExtendRegion(region_type, extend_id)
	if not region_type then
		return
	end

	local entry_info, entry_list, region_layout_info
	if self.__entry_map then
		for _, entry in pairs(self.__entry_map) do
			entry_info = entry:GetEntryInfo()
			region_layout_info = entry:GetLayoutInfo()
			
			if entry_info and region_layout_info and entry_info.region_type == region_type and 
				entry_info.extend_systemid == extend_id and region_layout_info.is_extend_region then
				
				entry_list = entry_list or {}
				table.insert(entry_list, entry_info)
			end
		end
	end

	return entry_list
end

--获取入口记录列表
function Modules.MainCityEntryMgr:GetRecordEntryList(region_layout_info)
	if not region_layout_info then
		return
	end

	return self.__entry_list_record and self.__entry_list_record[region_layout_info]
end

--获取入口控件
function Modules.MainCityEntryMgr:GetEntryById(entry_id)
	if not entry_id then
		return
	end

	return self.__entry_map and self.__entry_map[entry_id]
end

--获取入口控件
function Modules.MainCityEntryMgr:GetEntryBtnById(entry_id)
	local entry = self:GetEntryById(entry_id)
	if not entry then
		return
	end	

	return entry:GetEntryBtn()
end

--获取入口控件
function Modules.MainCityEntryMgr:GetEntryRedDotById(entry_id)
	local entry = self:GetEntryById(entry_id)
	if not entry then
		return
	end	

	return entry:GetEntryRedDot()
end

--获取入口特效信息
function Modules.MainCityEntryMgr:GetEffectInfo(system_id)
	local effect_info = self.__entry_effect_map[system_id]
	return effect_info
end

--更新单个模块的红点
function Modules.MainCityEntryMgr:UpdateEntryRedDot(entry_id, bubbling)
	--获取入口控件
	local entry = self:GetEntryById(entry_id)
	if entry then
		entry:UpdateEntryRedDot(bubbling)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_entry_class = function(obj_self, system_id)
	return obj_self.__entry_class_map[system_id]
end
