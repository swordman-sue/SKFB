
require "modules/red/basered"

--[[
%% @module: 红点对象管理器
%% @author: swordman sue
%% @created: 2018-12-6
%% @description: 负责管理红点对象，提供申请统计某系统的红点、获取红点信息等接口
--]]

Modules = Modules or {}

Modules.RedMgr = Modules.RedMgr or BaseClass(BaseModule)

--用于统计红点的自定义系统ID
Modules.RedMgr.SystemID = Modules.RedMgr.SystemID or
{
	--占卜成就
	DIVINATION_ACHIEVEMENT = "DIVINATION_ACHIEVEMENT",
	--占卜收藏书
	DIVINATION_BOOK = "DIVINATION_BOOK",

	--精灵强化
	SPIRIT_ENHANCE = "SPIRIT_ENHANCE",
	--精灵培养
	SPIRIT_TRAIN = "SPIRIT_TRAIN",
	--精灵升华
	SPIRIT_SUBLIMATE = "SPIRIT_SUBLIMATE",
}

function Modules.RedMgr:__init()
	self.__can_update = true

	--客户端红点对象列表
	self.__client_red_obj_map = {}

	--客户端红点对象注册表
	self.__client_red_obj_registry = 
	{
		--装备背包
		[Macros.Game.SystemID.EQUIP] = { class_name = "EquipBAG" , lua_file_path = "modules/red/red_equipbag" } ,

		--英雄阵容
		[Macros.Game.SystemID.HERO_LINEUP] = { class_name = "HeroLineup" , lua_file_path = "modules/red/red_hero_lineup"},
		--英雄背包
		[Macros.Game.SystemID.HERO_BAG] = { class_name = "HeroBag" , lua_file_path = "modules/red/red_herobag" } ,
	
		--主线副本
		[Macros.Game.SystemID.DUNGEON] = { class_name = "Dungeon" , lua_file_path = "modules/red/red_dungeon"},
		--日常副本
		[Macros.Game.SystemID.DUNGEON_DAILY] = { class_name = "Dungeon" , lua_file_path = "modules/red/red_dungeon"},
		--英雄降临
		[Macros.Game.SystemID.HERO_ADVENT] = { class_name = "Dungeon" , lua_file_path = "modules/red/red_dungeon"},

		--精灵
		[Macros.Game.SystemID.SPIRIT] = { class_name = "Spirit" , lua_file_path = "modules/red/red_spirit"},
		--世界之心
		[Macros.Game.SystemID.WORLD_HEART] = { class_name = "WorldHeart" , lua_file_path = "modules/red/red_worldheart"},

		--商城
		[Macros.Game.SystemID.SHOP] = { class_name = "Shop" , lua_file_path = "modules/red/red_shop" } ,
		--英雄商店
		[Macros.Game.SystemID.HERO_SHOP] = { class_name = "HeroShop" , lua_file_path = "modules/red/red_heroshop" } ,
		--爬塔商店
		[Macros.Game.SystemID.TOWER_SHOP] = { class_name = "TowerShop" , lua_file_path = "modules/red/red_tower_shop" } ,

		--回收
		[Macros.Game.SystemID.RECYCLE] = { class_name = "Recycle" , lua_file_path = "modules/red/red_recycle" } ,

		--阵营招募
		[Macros.Game.SystemID.CAMP_RECRUIT] = { class_name = "LimitFestival" , lua_file_path = "modules/red/red_limitfestival"},

		--圣器争夺
		[Macros.Game.SystemID.PLUNDEREDTREASURES] = { class_name = "Plunderedtreasures" , lua_file_path = "modules/red/red_plunderedtreasures"},

		--叛军
		[Macros.Game.SystemID.REBEL] = { class_name = "Rebel" , lua_file_path = "modules/red/red_rebel" } ,

		--军团
		[Macros.Game.SystemID.UNION] = { class_name = "Union" , lua_file_path = "modules/red/red_union" } ,

		--占卜
		[Macros.Game.SystemID.DIVINATION] = { class_name = "Divination" , lua_file_path = "modules/red/red_divination" } ,

		--领地
		[Macros.Game.SystemID.MANOR] = { class_name = "Manor" , lua_file_path = "modules/red/red_manor" } ,

		--首充
		[Macros.Game.SystemID.FIRST_RECHARGE] = { class_name = "FirstCharge" , lua_file_path = "modules/red/red_first_charge" } ,

		--活动
		[Macros.Game.SystemID.ACTIVITY] = { class_name = "Activity" , lua_file_path = "modules/red/red_activity" } ,
		--圣诞活动
		[Macros.Game.SystemID.CHRISTMAS_ACTIVITY] = { class_name = "LimitFestival" , lua_file_path = "modules/red/red_limitfestival"},
		--春节活动
		[Macros.Game.SystemID.SPRING_FESTIVAL] = { class_name = "LimitFestival" , lua_file_path = "modules/red/red_limitfestival"},
		--端午活动
		[Macros.Game.SystemID.DRAGONBOAT_FESTIVAL] = { class_name = "LimitFestival" , lua_file_path = "modules/red/red_limitfestival"},
		--七天活动
		[Macros.Game.SystemID.SEVEN_DEYS] = { class_name = "SevenDays" , lua_file_path = "modules/red/red_seven_days" } ,
		--七天登录
		[Macros.Game.SystemID.PUSH_SEVEN] = { class_name = "PushSeven" , lua_file_path = "modules/red/red_pushseven" } ,
		--女王
		[Macros.Game.SystemID.QUEEN] = { class_name = "Queen" , lua_file_path = "modules/red/red_queen" } ,		
		--充值回馈
		[Macros.Game.SystemID.FEEDBACK] = { class_name = "Feedback" , lua_file_path = "modules/red/red_feedback" } ,
		--在线奖励
		[Macros.Game.SystemID.ONLINE_AWARD] = { class_name = "Onlineaward" , lua_file_path = "modules/red/red_onlineaward" } ,
		--领奖中心
		[Macros.Game.SystemID.ACCEPT_PRIZE_CENTER] = { class_name = "AcceptPrizeCenter" , lua_file_path = "modules/red/red_accept_prize_center"},
	}

	--服务端红点信息表
	self.__server_red_num_map = {}

	--组合系统表(此类系统的红点提示由多个子系统组合而定)
	self.__combined_system_map = 
	{
		--日常任务与成就
		[Macros.Game.SystemID.DALIYTASK_AND_ACHIEVEMENT] = {Macros.Game.SystemID.ACHIEVEMENT, Macros.Game.SystemID.DALIYTASK},
	}
end

function Modules.RedMgr:__delete()
	if self.__client_red_obj_map then
		for k, v in pairs(self.__client_red_obj_map) do
			v:DeleteMe()
		end
		self.__client_red_obj_map = nil
	end
end

function Modules.RedMgr:Update(now_time, elapse_time)
	--副本扫荡中，过滤
	if CallModuleFunc(ModuleType.DUNGEON, "IsOnCleaning") then
		return
	end

	--礼包开启中，过滤
	if CallModuleFunc(ModuleType.Pack, "IsUsingGift") then
		return
	end

	--更新所有红点对象	
	for _, red_object in pairs(self.__client_red_obj_map) do
		red_object:Update(now_time, elapse_time)
	end	
end

--申请统计某系统的红点
function Modules.RedMgr:StatisticsRed(system_id, immediately, sub_system_id, ...)
	system_id = system_id or 0
	sub_system_id = sub_system_id or 0
	local params_list = {...}

	--检测系统是否开启
    local is_open, is_pre_open = GlobalModulesMgr:IsOpen(system_id)
    if not is_open then
    	return
    end

	--初始化红点对象
	self:__InitClientRedObj(system_id, function()
		local client_red_obj = self.__client_red_obj_map[system_id]
		if client_red_obj then
			client_red_obj:Statistics(immediately, sub_system_id, unpack(params_list))
		end
	end)
end

--申请统计所有系统的红点
function Modules.RedMgr:StatisticsAllRed()
	for system_id, _ in pairs(self.__client_red_obj_registry) do
		self:StatisticsRed(system_id, nil, Macros.Game.SystemID.MAX)
	end
end

--获取红点
function Modules.RedMgr:GetRed(system_id, sub_system_id)
	system_id = system_id or 0
	sub_system_id = sub_system_id or 0
	if 0 == system_id and 0 == sub_system_id then
		return 0
	end

	--组合统计
	local combined_sub_system_list = self.__combined_system_map[system_id]
	if combined_sub_system_list then
		local red_dot_num = 0
		for _, combined_sub_system_id in ipairs(combined_sub_system_list) do
			red_dot_num = red_dot_num + self:GetRed(combined_sub_system_id)
			if red_dot_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
		return red_dot_num
	end

	--获取客户端统计的红点数
	local client_red_num = self.__client_red_obj_map[system_id] and self.__client_red_obj_map[system_id]:GetRed(sub_system_id) or nil
	--获取服务端统计的红点数
	local server_red_num = self.__server_red_num_map[sub_system_id] or self.__server_red_num_map[system_id] or nil

	--在主界面时，优先使用服务端统计的红点
	if GlobalViewMgr:IsOnlyCityViewOpen() then
		return server_red_num or client_red_num or 0
	end

	--不在主界面时，优先使用客户端统计的红点
	return client_red_num or server_red_num or 0
end

--获取服务器统计的红点
function Modules.RedMgr:GetServerRed(system_id)
	return self.__server_red_num_map[system_id]
end

--------------------------------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 请求红点列表 消息Id: 23501
function Modules.RedMgr:RedPointListReq()
	--避免重复请求
	if self.__is_requesting_red_point_list then
		return
	end
	self.__is_requesting_red_point_list = true

    GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RED_POINT_LIST_REQ))
end

-- 消息说明: 请求红点列表响应 消息Id: 23502
function Modules.RedMgr:RedPointListResp(protocol_data)
	self.__is_requesting_red_point_list = nil

	--记录服务端统计的红点数
	self.__server_red_num_map = {}
	for i, v in ipairs(protocol_data.red_point_list) do
		self.__server_red_num_map[v.system_id] = 1
	end

	self:FireNextFrame(Macros.Event.RedModule.RED_UPDATE)
end

-- 消息说明: 红点通知 消息Id: 23503
function Modules.RedMgr:RedPointNotify(protocol_data)
	self.__server_red_num_map[protocol_data.system_id] = 1
end

--------------------------------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------------------------------
--处理角色等级变化
function Modules.RedMgr:HandleRoleLevelChange(old_level, new_level)
	--在主界面时，才请求服务端红点数据
	if GlobalViewMgr:IsOnlyCityViewOpen() then
		self:RedPointListReq()
	end
end

--处理系统开启
function Modules.RedMgr:HandleSystemOpened(newest_open_list)
	--申请统计新开启系统的红点
	if newest_open_list then
		for _, system_id in ipairs(newest_open_list) do
			self:StatisticsRed(system_id, nil, Macros.Game.SystemID.MAX)
		end
	end
end

--处理返回主界面
function Modules.RedMgr:HandleMaincityVisibleEvent()
	self:RedPointListReq()

	--统计红点-摇钱树
	self:StatisticsRed(Macros.Game.SystemID.ACTIVITY, nil, Macros.Game.SystemID.YCS)
end

--处理新一天的通知
function Modules.RedMgr:HandleNewDayNotify()
	self:RedPointListReq()
end

--处理与服务端断开连接
function Modules.RedMgr:HandleServerDisconnected()
	self.__is_requesting_red_point_list = nil
end

--处理界面关闭
function Modules.RedMgr:HandleViewCloseEvent(_, view_type)
    --阵营招募
    if view_type == BaseViewType.CAMP_RECRUIT then
        self:StatisticsRed(Macros.Game.SystemID.CAMP_RECRUIT)

    --圣诞活动
    elseif view_type == BaseViewType.CHRISTMAS then
    	self:StatisticsRed(Macros.Game.SystemID.CHRISTMAS_ACTIVITY)

   	--端午活动
    elseif view_type == BaseViewType.DRAGONBOAT_FESTIVAL then
    	self:StatisticsRed(Macros.Game.SystemID.DRAGONBOAT_FESTIVAL)

    --春节活动
    elseif view_type == BaseViewType.SPRING_FESTIVAL then
    	self:StatisticsRed(Macros.Game.SystemID.SPRING_FESTIVAL)

    --领地
    elseif view_type == BaseViewType.MANOR then
        self:StatisticsRed(Macros.Game.SystemID.MANOR)

	--回收
    elseif view_type == BaseViewType.RECYCLE then
        self:StatisticsRed(Macros.Game.SystemID.RECYCLE)

    --商城
    elseif view_type == BaseViewType.SHOP then
		self:StatisticsRed(Macros.Game.SystemID.SHOP)

  	--爬塔商店
    elseif view_type == BaseViewType.TOWERSHOP then
        self:StatisticsRed(Macros.Game.SystemID.TOWER_SHOP)

    --圣器背包
    elseif view_type == BaseViewType.TREASURE then
        self:StatisticsRed(Macros.Game.SystemID.TREASURE)

    --世界之心
    elseif view_type == BaseViewType.WORLD_HEART then
	    self:StatisticsRed(Macros.Game.SystemID.WORLD_HEART)

	--邮件
	elseif view_type == BaseViewType.EMAIL then
		--TODO：需要服务端的红点请求协议添加系统ID参数，当传入此参数时，只统计该系统的红点数
		self:RedPointListReq()

	--七日登录
	elseif view_type == BaseViewType.PUSH_SEVEN then
		--TODO：需要服务端的红点请求协议添加系统ID参数，当传入此参数时，只统计该系统的红点数
		self:RedPointListReq()
    end
end

---------------------------------------------------------------------------------------------
--处理更新英雄碎片
function Modules.RedMgr:HandleUpdateHeroDebris()
    --统计红点-英雄碎片
    self:StatisticsRed(Macros.Game.SystemID.HERO_BAG)
end
--处理更新装备碎片
function Modules.RedMgr:HandleUpdateEquipDebris()
    --统计红点--装备碎片
    self:StatisticsRed(Macros.Game.SystemID.EQUIP)
end

---------------------------------------------------------------------------------------------
--统计红点-阵容
local function StatisticsHeroLineupRed(self)
    self:StatisticsRed(Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.HERO_LINEUP)
    self:StatisticsRed(Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.REINFORCEMENT)
end
--处理增加英雄
function Modules.RedMgr:HandleAddHeroEvent()
    --统计红点-回收
    self:StatisticsRed(Macros.Game.SystemID.RECYCLE, nil, Macros.Game.SystemID.HERO_BAG)
end
--处理更新英雄
function Modules.RedMgr:HandleUpdateHeroEvent()
end
--处理移除英雄
function Modules.RedMgr:HandleRemoveHeroEvent()
    --统计红点-回收
    self:StatisticsRed(Macros.Game.SystemID.RECYCLE, nil, Macros.Game.SystemID.HERO_BAG)
end
--处理改变英雄
function Modules.RedMgr:HandleChangeHeroEvent()
	--统计红点-阵容
	StatisticsHeroLineupRed(self)
end
--处理英雄/援军的上阵/下阵
function Modules.RedMgr:HandleHeroOnOrOffFightEvent()
	--统计红点-阵容
	StatisticsHeroLineupRed(self)
end
--处理英雄阵容变化
function Modules.RedMgr:HandleHeroLineupUpdateEvent()
	--统计红点
	self:StatisticsRed(Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.DIVINATION_DRESS)	
end

---------------------------------------------------------------------------------------------
--处理增加装备
function Modules.RedMgr:HandleAddEquipEvent(data)
    --统计红点-回收
    self:StatisticsRed(Macros.Game.SystemID.RECYCLE, nil, Macros.Game.SystemID.EQUIP)
end
--处理更新装备
function Modules.RedMgr:HandleUpdateEquipEvent(data)
end
--处理移除装备
function Modules.RedMgr:HandleRemoveEquipEvent(data)
    --统计红点-回收
    self:StatisticsRed(Macros.Game.SystemID.RECYCLE, nil, Macros.Game.SystemID.EQUIP)
end
--处理改变装备
function Modules.RedMgr:HandleEquipChangeEvent(data)
    --统计红点-装备穿戴
    self:StatisticsRed(Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.EQUIP_DRESS)
end

---------------------------------------------------------------------------------------------
--处理改变圣器
function Modules.RedMgr:HandleChangeTreasure()
    --统计红点-圣器穿戴
    self:StatisticsRed(Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.EQUIP_DRESS)    
end

---------------------------------------------------------------------------------------------
--处理改变卡牌
function Modules.RedMgr:HandleChangeCard()
	--统计红点-占卜收藏书
	self:StatisticsRed(Macros.Game.SystemID.DIVINATION , nil , 2)
	--统计红点-卡牌穿戴
    self:StatisticsRed(Macros.Game.SystemID.HERO_LINEUP, nil, Macros.Game.SystemID.DIVINATION_DRESS)
end

---------------------------------------------------------------------------------------------
--处理主线副本数据反馈
function Modules.RedMgr:HandleMainDungeonDataResp()
	--统计红点-英雄降临
	self:StatisticsRed(Macros.Game.SystemID.HERO_ADVENT)	
end
--处理领取主线副本章节奖励
function Modules.RedMgr:HandleTakeMainDungeonChapterReward()
	--统计红点-英雄降临
	self:StatisticsRed(Macros.Game.SystemID.HERO_ADVENT)
end
--处理日常副本数据反馈
function Modules.RedMgr:HandleDailyDungeonDataResp()
	--统计红点-日常副本
	self:StatisticsRed(Macros.Game.SystemID.DUNGEON_DAILY)
end

--------------------------------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------------------------------
--初始化客户端红点对象
function Modules.RedMgr:__InitClientRedObj(system_id, finished_callback)
	--已初始化
	if self.__client_red_obj_map[system_id] then
		if finished_callback then
			finished_callback()
		end
		return 
	end

	--检测注册信息
	local red_init_info = self.__client_red_obj_registry[system_id]
	if not red_init_info then
		return
	end

	--加载脚本文件
	GlobalRequireToolBatch:LoadFiles(
	{
        file_list = {red_init_info.lua_file_path},
		callback = function() 		
			local class = Modules.Red[red_init_info.class_name]
			if class then
				local object = class.New()
				if object then
					object:SetSystemID(system_id)
					self.__client_red_obj_map[system_id] = object
			
					--初始化完成回调		
					if finished_callback then
						finished_callback()
					end			
				else
					LogTool.LogError("[Modules.RedMgr:__InitClientRedObj] 创建红点对象失败 class_name:%s", red_init_info.class_name)
				end
			else
				LogTool.LogError("[Modules.RedMgr:__InitClientRedObj] 找不到对应的红点对象类 class_name:%s", red_init_info.class_name)
			end
		end
	})
end