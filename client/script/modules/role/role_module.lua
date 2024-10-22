
--[[
%% @module: 角色模块
%% @author: swordman sue
%% @created: 2016-07-28
%% @description: 角色模块
--]]

Modules = Modules or {}

Modules.RoleModule = Modules.RoleModule or BaseClass(BaseModule)

local update_property_max = function(role_info)
	role_info.max_exp = ConfigAdapter.RoleLevel.GetMaxExp(role_info.level)
	role_info.energy_max = ConfigAdapter.RoleLevel.GetMaxEnergy(role_info.level)
	role_info.stamina_max = ConfigAdapter.RoleLevel.GetMaxStamina(role_info.level)
	role_info.wanted_max = ConfigAdapter.RoleLevel.GetMaxWanted(role_info.level)	
end

function Modules.RoleModule:__init()
	self.__can_update = true

	--角色属性恢复注册表
	self.__prop_recovery_map = 
	{
		[Macros.Game.RolePropType.STAMINA] = {next_recover_time = 0},
		[Macros.Game.RolePropType.ENERGY] = {next_recover_time = 0},
		[Macros.Game.RolePropType.WANTED] = {next_recover_time = 0},
	}

	self.__battle_power_changed_effect_tool = BattlePowerChangedEffectTool.New()
end

function Modules.RoleModule:__delete()
	self.__role_info = nil

	if self.__prop_recovery_map then
		for _, prop_recovery_info in pairs(self.__prop_recovery_map) do
			if prop_recovery_info.timer then
				GlobalTimerQuest:CancelQuest(prop_recovery_info.timer)
			end
		end
		self.__prop_recovery_map = nil
	end

	if self.__battle_power_changed_effect_tool then
		self.__battle_power_changed_effect_tool:DeleteMe()
		self.__battle_power_changed_effect_tool = nil
	end
end

function Modules.RoleModule:Update(now_time, elapse_time)
	--更新相关记录(重试)
	self:__RetryUpdateProgressTag(now_time, elapse_time)
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--角色信息同步
function Modules.RoleModule:RoleInfoNotifyResp(protocol_data)
	self.__role_info = protocol_data
	update_property_max(self.__role_info)

	--相关记录信息
	self:__ReadProgressTagRecord()

	--请求属性恢复、获取属性恢复信息
	for prop_type, _ in pairs(self.__prop_recovery_map) do
		self:RoleProRecoverReq(prop_type)
	end

	--初始化新手指引进度记录
	CallModuleFunc(ModuleType.LEAD, "SetupProgressTagRecord")

	--上传创建角色
	CallModuleFunc(ModuleType.LOGIN, "ReportCreateRole")

	--初始化系统推送
	MsgPushHandle.SetPushInfo()

	--更新系统推送(紫装派送、登录获得橙英雄)
	MsgPushHandle.UpdateLocalPushFun(MsgPushHandle.PushKey.PURPLE_EQUIP_GIVE)
	MsgPushHandle.UpdateLocalPushFun(MsgPushHandle.PushKey.ORANGE_HERO_GIVE_OF_SEVEN_DAY_ACT)
end

--角色属性改变
function Modules.RoleModule:RolePropertyNotifyResp(protocol_data)
	if not self.__role_info then
		return
	end

	self:SetPropValue(protocol_data.type , protocol_data.value)
end

--角色升级
function Modules.RoleModule:RoleLevelUpgradeResp(protocol_data)
	local old_level = self.__role_info.level
	self.__role_info.level = protocol_data.level
	update_property_max(self.__role_info)

	--记录等级变化(用于升级界面打开后)
	self.__level_change_records = self.__level_change_records or {}
	table.insert(self.__level_change_records, {pre_level = old_level , now_level = self.__role_info.level})

	--派发升级事件
	self:FireNextFrame(Macros.Event.RoleModule.LEVEL_CHANGE, old_level, self.__role_info.level)

	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") then
		self:OpenView(BaseViewType.ROLEUPGRADE)

	--世界任务特殊处理
	elseif viewInfo and viewInfo.view_type == "PUSH_WORLDLEVEL" then
		--升级截断
	    self:BindGlobalEvent(Macros.Event.RoleModule.UP_LEVEL_TRUNCATION,function ( ... )
	    	self:OpenView(BaseViewType.ROLEUPGRADE)
	    end)
	else		
		if not CallModuleFunc(ModuleType.DUNGEON,"GetIsMainDungeonFight") then			
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "InsertDialog", function(close_callback)
				self:OpenView(BaseViewType.ROLEUPGRADE, close_callback)
			end, Macros.Game.SpDialogPriority.ROLE_UPGRADE, nil, true)
		end	
		--设置是否打副本导致升级
		CallModuleFunc(ModuleType.DUNGEON,"SetIsFightDungeonFBMakeLevelUp",CallModuleFunc(ModuleType.DUNGEON,"GetIsMainDungeonFight"))		
	end
	CallModuleFunc(ModuleType.DUNGEON,"SetIsMainDungeonFight",false)

	--上传角色升级到SDK
	CPPSdkTools.ReportRoleLevelUp()
end

--消息说明: 角色头像替换请求 消息Id: 20115
function Modules.RoleModule:HeadImageReplaceReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HEAD_IMAGE_REPLACE_REQ)
	data.head_image_id = info
	GlobalNetAdapter:OnSend(data)
end

--消息说明: 头像替换响应 消息Id: 20116
function Modules.RoleModule:HeadImageReplaceResp( info )
	self.__role_info.head_icon = info.head_image_id
	GlobalEventSystem:FireNextFrame(Macros.Event.RoleModule.HEAD_CHANGE,info)
end

--请求属性恢复、获取属性恢复信息
function Modules.RoleModule:RoleProRecoverReq(prop_type)
    local prop_recovery_info = self.__prop_recovery_map[prop_type]
    if not prop_recovery_info then
        return
    end

    --属性已满
	local prop_value = self:GetPropValue(prop_type)
	local max_prop_value = self:GetMaxPropValue(prop_type)
	if prop_value >= max_prop_value then
		return
	end

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ROLE_PRO_RECOVER_REQ)
	data.type = prop_type
	GlobalNetAdapter:OnSend(data) 
end

--请求属性恢复、获取属性恢复信息反馈
function Modules.RoleModule:RoleProRecoverResp(protocal_data)
	local prop_type = protocal_data.type
    local prop_recovery_info = self.__prop_recovery_map[prop_type]
    if not prop_recovery_info then
        return
    end

    local config_recovery = CPPGameLib.GetConfig("PropertyRecover", prop_type)
    if not config_recovery then
        return
    end

    --停止旧的计时器
	if prop_recovery_info.timer then
		GlobalTimerQuest:CancelQuest(prop_recovery_info.timer)
		prop_recovery_info.timer = nil
	end

    local prop_value = self:GetPropValue(prop_type)
    local max_prop_value = self:GetMaxPropValue(prop_type)
    if prop_value < max_prop_value then
	    local next_recover_time = protocal_data.recover_time + config_recovery.recover_interval_time	    
	    local recover_countdown_time = math.max(0, next_recover_time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"))
	    if recover_countdown_time > 0 then
		    prop_recovery_info.next_recover_time = next_recover_time

		    --更新倒计时
			GlobalEventSystem:Fire(Macros.Event.RoleModule.PROP_RECOVERY_COUNTDOWN, prop_type, recover_countdown_time)

		    --启动新的计时器
	        prop_recovery_info.timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
	            --倒计时
	            local recover_countdown_time = math.max(0, next_recover_time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"))
	            if recover_countdown_time <= 0 then
	                GlobalTimerQuest:CancelQuest(prop_recovery_info.timer)
	                prop_recovery_info.timer = nil

	                --请求属性恢复、获取属性恢复信息
	                self:RoleProRecoverReq(prop_type)
	            end

			    --更新倒计时
				GlobalEventSystem:Fire(Macros.Event.RoleModule.PROP_RECOVERY_COUNTDOWN, prop_type, recover_countdown_time)
	        end), 1, -1)
	    end    	
    end
end

--更新相关记录请求
function Modules.RoleModule:UpdateProgressTagReq(key, value)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUIDE_UPDATE_REQ)
	data.key = key
	data.value = value
	GlobalNetAdapter:OnSend(data)

	--设置重试信息
	self.__retry_update_prog_tag_map = self.__retry_update_prog_tag_map or {}
	local retry_info = self.__retry_update_prog_tag_map[key]
	if not retry_info then
		retry_info = {}
		self.__retry_update_prog_tag_map[key] = retry_info
	end
	retry_info.value = value
	retry_info.triggered = true
	retry_info.retry_time = Macros.Global.NowTime + Net.NetAdapter.NetLoadingTimeout
end

--更新相关记录反馈
function Modules.RoleModule:UpdateProgressTagResp(protocol_data)
	if not self.__retry_update_prog_tag_map then
		return
	end
	local retry_info = self.__retry_update_prog_tag_map[protocol_data.key]
	if retry_info and retry_info.value == protocol_data.value then
		retry_info.triggered = false
	end
end

--更新剧情记录请求
function Modules.RoleModule:UpdateStoryTagReq(key, value)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_STORY_TAG_UPDATE_REQ)
	data.key = key
	data.value = value
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 玩家留言请求 消息Id: 24101
function Modules.RoleModule:PlayerLevelMessageReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLAYER_LEVEL_MESSAGE_REQ)
	data.contack = info.contack
	data.text = info.text
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 玩家留言响应 消息Id: 24102
function Modules.RoleModule:PlayerLevelMessageResp( info )
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("server_importReply") )
end

--------------------------------------------------------------------------------------------------------------------------------
--接口
--------------------------------------------------------------------------------------------------------------------------------
--[[
	字段内容参考protocol_analyze_201
--]]
function Modules.RoleModule:GetRoleInfo()
	return self.__role_info
end

function Modules.RoleModule:GetRoleID()
	return self.__role_info and self.__role_info.role_id or 0
end

--根据类型获取属性值
function Modules.RoleModule:GetPropValue(prop_type)
	if not self.__role_info then
		return 0
	end

	local prop_name = Macros.Game.RolePropKey[prop_type]
	if prop_name then
		return self.__role_info[prop_name] or 0
	end
end

--根据类型获取属性上限值
function Modules.RoleModule:GetMaxPropValue(prop_type)
	if not self.__role_info then
		return 0
	end

	local prop_name = Macros.Game.RolePropKey[prop_type]
	if prop_name then
		local prop_name_max = prop_name.."_max"	
		return self.__role_info[prop_name_max] or 0
	end
end

--根据类型设置属性值
function Modules.RoleModule:SetPropValue(type , value)
	if type == Macros.Game.RolePropType.LEVEL then
		--战斗加速
		local is_open_by_vip = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.BATTLE_ACC_MAX) == Macros.Global.TRUE
		if value == ConfigAdapter.Common.GetBattleAccMaxOpenLev() and not is_open_by_vip then
			local acc_list = ConfigAdapter.Common.GetBattleACCList()
			Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.BattleACCIndex, tostring(#acc_list))
		end
		return
	end

	local prop_name = Macros.Game.RolePropKey[type]
	if not prop_name then
		LogTool.LogError("[Modules.RoleModule:RolePropertyNotifyResp] 找不到对应的属性类型(type:%d)，请到Macros.Game.RolePropKey中定义", type)
		return
	end
	if not self.__role_info[prop_name] then
		LogTool.LogError("[Modules.RoleModule:RolePropertyNotifyResp] Macros.Game.RolePropKey与Net.ProtocolNo.S2C_ROLE_INFO_NOTIFY协议中的属性字段不一致(type:%d)", protocol_data.type)
		return
	end

	local old_value = self.__role_info[prop_name]
	self.__role_info[prop_name] = value

	--钻石改变
	if type == Macros.Game.RolePropType.DIAMOND and old_value ~= value then
		CPPSdkTools.ReportDiamondChange(old_value, value)
	end

	--VIP等级改变
    if type == Macros.Game.RolePropType.VIP_LEVEL and old_value ~= value then
		CPPSdkTools.ReportVipLevelUp()
	end

	--战斗力
	if type == Macros.Game.RolePropType.BATTLE_VALUE and CallModuleFunc(ModuleType.SP_LOGIC_MGR, "CanShowBattleValueChangeEffect") then
		if old_value ~= value then
			--战力变化特效
			if not self.__battle_power_changed_effect_tool then
				self.__battle_power_changed_effect_tool = BattlePowerChangedEffectTool.New()
			end
			self.__battle_power_changed_effect_tool:HandleBattlePowerChanged( old_value , value )
		end	
	end

	--处理属性恢复相关
	local prop_recovery_info = self.__prop_recovery_map[type]
	if prop_recovery_info then
		local max_value = self:GetMaxPropValue(type)

		--属性从不满到满
		if old_value < max_value and value >= max_value then
			--停止计时器
			if prop_recovery_info.timer then
				GlobalTimerQuest:CancelQuest(prop_recovery_info.timer)
				prop_recovery_info.timer = nil
			end

		--属性从满到不满
		elseif old_value >= max_value and value < max_value then
            --请求属性恢复、获取属性恢复信息
            self:RoleProRecoverReq(type)
		end
	end

	self:FireNextFrame(Macros.Event.RoleModule.PROP_CHANGE, type, old_value, self.__role_info[prop_name])
end

function Modules.RoleModule:SetName(var)
	self.__role_info.role_name = var
	
	self:FireNextFrame(Macros.Event.RoleModule.ROLE_NAME_CHANGED)
end

function Modules.RoleModule:GetName()
	return self.__role_info.role_name
end

function Modules.RoleModule:GetWanted()
	return self.__role_info.wanted, CPPGameLib.GetConfig("RoleLevel", self:GetPropValue(Macros.Game.RolePropType.LEVEL)).wanted_max
end

function Modules.RoleModule:GetTotalFriendShipPoint()
	return self.__role_info.total_friendship_point
end

function Modules.RoleModule:GetNobility()
	return self.__role_info.nobility_level
end

function Modules.RoleModule:GetRecoveryInfo(prop_type)
    return self.__prop_recovery_map[prop_type]
end

--检测系统开放
function Modules.RoleModule:CheckSystemOpened()
	local config_sys_entrys = CPPGameLib.GetConfig("SystemEntry", nil, nil, true)
	if self.__level_change_records then
		for _, level_change_info in ipairs(self.__level_change_records) do
		    local newest_open_list
		    for id, entry_info in pairs(config_sys_entrys) do
		        if level_change_info.pre_level < entry_info.level and level_change_info.now_level >= entry_info.level then
		            newest_open_list = newest_open_list or {}
		            table.insert(newest_open_list, id)
		        end
		    end 

		    if newest_open_list then
		        self:FireNextFrame(Macros.Event.SystemModule.SYSTEM_OPENED, newest_open_list)
		    end
		end
		self.__level_change_records = nil
	end
end

--记录战力变化前后
function Modules.RoleModule:RecordBattlePowerChanged(id, is_changed)
	if self.__battle_power_changed_effect_tool then
		self.__battle_power_changed_effect_tool:RecordBattlePowerChanged(id, is_changed)
	end
end

--获得开服时间
function Modules.RoleModule:GetOpenClothing()
	if not self.__role_info then
		return
	end
	
	--开服时间 
	local  year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(self.__role_info.open_server_time)
	local sevenr_time = self.__role_info.open_server_time - second - ( minute * 60 ) - ( hour * 60 * 60 )
	--当前时间 
	local now = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	return math.ceil( ( now - sevenr_time ) / 60 / 60 / 24 ) , self.__role_info.open_server_time
end

--获得创号时间
function Modules.RoleModule:GetRoleCreateTime()
	if not self.__role_info then
		return
	end
	
	--开服时间 
	local  year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(self.__role_info.role_create_time)
	local sevenr_time = self.__role_info.role_create_time - second - ( minute * 60 ) - ( hour * 60 * 60 )
	--当前时间 
	local now = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local  nowyear, nowmonth, nowday, nowhour, nowminute, nowsecond, nowweek = TimeTool.GetDateFromTimeStamp(now)
	return math.ceil( ( now - sevenr_time ) / 60 / 60 / 24 ) , self.__role_info.role_create_time
end

--获得是否已经创号两天了，0点算重置
function Modules.RoleModule:GetIsHavePassSecondDay()
	if not self.__role_info then
		return
	end
	
	--开服时间 
	local  year, month, day, hour, minute, second = TimeTool.GetDateFromTimeStamp(self.__role_info.role_create_time)
	--当前时间 
	local now_servertime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--第二天时间
	local second_day_time = TimeTool.GetTimeStampFromDate(year, month, day+1, 0, 0, 0)
	return now_servertime >= second_day_time
end

--设置相关记录
function Modules.RoleModule:SetProgressTag(key, value)
	self.__progress_tag_record[key] = value
	self:UpdateProgressTagReq(key, value)
end

--获取相关记录
function Modules.RoleModule:GetProgressTag(key, def)
	def = def or 0
	return self.__progress_tag_record[key] or def
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--读取相关记录
function Modules.RoleModule:__ReadProgressTagRecord()
	self.__progress_tag_record = {}
	for _, info in ipairs(self.__role_info.guide_list) do
		self.__progress_tag_record[info.key] = info.value
	end
end

--更新相关记录(重试)
function Modules.RoleModule:__RetryUpdateProgressTag(now_time, elapse_time)
	if not self.__retry_update_prog_tag_map then
		return
	end

	for key, retry_info in pairs(self.__retry_update_prog_tag_map) do
		if retry_info.triggered and retry_info.retry_time <= now_time then
			self:UpdateProgressTagReq(key, retry_info.value)
		end
	end
end


