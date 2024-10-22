
--系统推送逻辑处理
MsgPushHandle = MsgPushHandle or {}

--本地推送Key
MsgPushHandle.PushKey = MsgPushHandle.PushKey or
{
	PURPLE_EQUIP_GIVE = 3,
	ORANGE_HERO_GIVE_OF_SEVEN_DAY_ACT = 4,
	HIGH_RECURIT_TIMES_RECOVERY = 8,
	MANOR_PATROL_FINISHED = 10,
}

--动态推送表
MsgPushHandle.DynamicPush = {}
for _, v in pairs(MsgPushHandle.PushKey) do
	MsgPushHandle.DynamicPush[v] = true	
end

function MsgPushHandle.GetChannelTypeTag()
	return 	tostring(CPPSdkTools.GetCPChannelType())
end

function MsgPushHandle.GetChannelTypeServerTag()
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	local server_id = role_info and role_info.server_id or 0
	return string.format("%d_%d", CPPSdkTools.GetCPChannelType(), server_id)
end

function MsgPushHandle.GetUserAlias()
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	local role_id = role_info and role_info.role_id or 0
	return string.format("%s_%d", MsgPushHandle.GetChannelTypeServerTag(), role_id)
end

--推送启动
function MsgPushHandle.PushMsgStart()
	-- if CPPSystemTools.IsWin32() then
	-- 	return
	-- end
	if true then return end

	MsgPushHandle.TagNeedUpdate = true
	MsgPushHandle.AliasNeedUpdate = true
	MsgPushHandle.LocalPushNeedUpdate = true

	local start_callback = function(info)
		local data, pos, err = CPPGameLib.JSON.decode(info)
		if err then
			LogTool.LogInfo("[MsgPushHandle.PushMsgStart] Json Error result: [%s] json error :[%s]", info, err)
		else
			if data.Status == 1 then
				LogTool.LogInfo("[MsgPushHandle.PushMsgStart] Start Msg Push Success")
			else 
				LogTool.LogInfo("[MsgPushHandle.PushMsgStart] Start Msg Push Fail %s", tostring(data.Content))
			end
		end
	end
	local start_callback_fun = GlobalCallbackMgr:AddCallBackFunc(start_callback)
	CPPMsgPush:StartMsgPush(start_callback_fun)
end

--设置推送用户tag
function MsgPushHandle.PushMsgSetTag()
	-- if not CPPSystemTools.IsAndroid() then
	-- 	--TODO：处理IOS的推送
	-- 	return
	-- end
	if true then return end

	local add_tag_callback = function(info)
		local data, pos, err = CPPGameLib.JSON.decode(info)
		if err then
			LogTool.LogInfo("[MsgPushHandle.PushMsgSetTag] Json Error result: [%s] json error :[%s]", info, err)
		else
			if data.Status == 1 then
				LogTool.LogInfo("[MsgPushHandle.PushMsgSetTag] Add Tag Success")
				MsgPushHandle.TagNeedUpdate = false
			else 
				LogTool.LogInfo("[MsgPushHandle.PushMsgSetTag] Add Tag Fail %s", tostring(data.Content))
				MsgPushHandle.TagNeedUpdate = true
			end
		end
	end
	local add_tag_callback_fun = GlobalCallbackMgr:AddCallBackFunc(add_tag_callback)
	local add_tag =  string.format("%s;%s", MsgPushHandle.GetChannelTypeServerTag(), MsgPushHandle.GetChannelTypeTag())
	CPPMsgPush:SetTag(add_tag, add_tag_callback_fun)

	MsgPushHandle.TagNeedUpdate = false	
end

--设置推送用户别名
function MsgPushHandle.PushMsgSetAlias()
	-- if not CPPSystemTools.IsAndroid() then
	-- 	--TODO：处理IOS的推送
	-- 	return
	-- end
	if true then return end

	local set_alias_callback = function(info)
		local data, pos, err = CPPGameLib.JSON.decode(info)
		if err then
			LogTool.LogInfo("[MsgPushHandle.PushMsgSetAlias] SetAlias Json Error result: [%s] json error :[%s]", info, err)
		else
			if data.Status == 1 then
				LogTool.LogInfo("[MsgPushHandle.PushMsgSetAlias] Set Alias Success")
				MsgPushHandle.AliasNeedUpdate = false
			else 
				LogTool.LogInfo("[MsgPushHandle.PushMsgSetAlias] Set Alias Fail %s", tostring(data.Content))
				MsgPushHandle.AliasNeedUpdate = true
			end
		end
	end
	local set_alias_callback_fun = GlobalCallbackMgr:AddCallBackFunc(set_alias_callback)
	CPPMsgPush:SetAlias(MsgPushHandle.GetUserAlias(), set_alias_callback_fun)

	MsgPushHandle.AliasNeedUpdate = false	
end

--设置标签、别名、本地推送
function MsgPushHandle.SetPushInfo()
	-- if not CPPSystemTools.IsAndroid() then
	-- 	--TODO：处理IOS的推送
	-- 	return
	-- end
	if true then return end

	if MsgPushHandle.TagNeedUpdate then
		MsgPushHandle.PushMsgSetTag()
	end
	
	if MsgPushHandle.AliasNeedUpdate then
		MsgPushHandle.PushMsgSetAlias()
	end

	if MsgPushHandle.LocalPushNeedUpdate then
		MsgPushHandle.UpdateAllLocalPushFun()
	end
end

--设置所有本地推送
function MsgPushHandle.UpdateAllLocalPushFun()
	-- if not CPPSystemTools.IsAndroid() then
	-- 	--TODO：处理IOS的推送
	-- 	return
	-- end
	if true then return end

	local config_sys_pushs = CPPGameLib.GetConfig("SysPush", nil, nil, true)
	if not config_sys_pushs or #config_sys_pushs <= 0 then
		return
	end

	-- CPPMsgPush:CancelTimerTask("ALL")

	for key, _ in ipairs(config_sys_pushs) do
		if not MsgPushHandle.DynamicPush[key] then
			MsgPushHandle.UpdateLocalPushFun(key)
		end
	end

	MsgPushHandle.LocalPushNeedUpdate = false
end

--紫装派送
local function UpdateLocalPushPurpleEquipGive(config_push)
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	local current_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local push_server_time = TimeTool.CalcTimeStampFrmBasicTimeStamp(role_info.open_server_time, config_push.push_time[1], config_push.push_time[2])
	local pre_time = current_server_time - push_server_time
	if pre_time < 0 then
		CPPMsgPush:SetTimerTask(
			tostring(config_push.key), "", config_push.content or "", tostring(config_push.push_type), tostring(pre_time), "")			
	end
end

--7天活动之登录获得橙英雄
local function UpdateLocalPushOrangeHeroGiveOfSevenDaysAct(config_push)
	UpdateLocalPushPurpleEquipGive(config_push)
end

--高级召唤的免费次数恢复
local function UpdateLocalPushHighRecuritTimesRecovery(config_push)
	local recurit_info = CallModuleFunc(ModuleType.HERO, "GetHeroRecruitInfo")
	local current_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local push_server_time = recurit_info.advanced_summon_time + ConfigAdapter.Common.GetHighRecruitFreeTimeCD()
	local pre_time = current_server_time - push_server_time
	if pre_time < 0 then		
		CPPMsgPush:SetTimerTask(
			tostring(config_push.key), "", config_push.content or "", tostring(config_push.push_type), tostring(pre_time), "")			
	end
end

--领地的巡逻时间完成
local function UpdateLocalPushManorPatrolFinished(config_push)
	local push_server_time = CallModuleFunc(ModuleType.MANOR, "GetFirstPatrolFinishedTime")
	if not push_server_time then
		return
	end
	local current_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local pre_time = current_server_time - push_server_time
	if pre_time < 0 then		
		CPPMsgPush:SetTimerTask(
			tostring(config_push.key), "", config_push.content or "", tostring(config_push.push_type), tostring(pre_time), "")			
	end
end

--每日推送、每周推送
local function UpdateLocalPushCommon(config_push)
	local push_time = ""
	if config_push.push_time then
		for k, time in ipairs(config_push.push_time) do
			push_time = string.format("%s%d%s", push_time, time, k == #config_push.push_time and "" or ';')
		end
	end

	local week_time = ""
	if config_push.week_time then
		for k, week in ipairs(config_push.week_time) do
			week_time = string.format("%s%d%s", week_time, week, k == #config_push.week_time and "" or ';')
		end
	end

	CPPMsgPush:SetTimerTask(
		tostring(config_push.key), push_time, config_push.content or "", tostring(config_push.push_type), tostring(config_push.pre_time or 0), week_time)			
end

--设置本地推送
function MsgPushHandle.UpdateLocalPushFun(key)
	if not CPPSystemTools.IsAndroid() then
		--TODO：处理IOS的推送
		return
	end

	local config_push = CPPGameLib.GetConfig("SysPush", key)
	if config_push and config_push.toggle ~= Macros.Global.FALSE then
		-- MsgPushHandle.RemoveLocalPushFun(key)

		CPPGameLib.Switch(key)
		{
			--紫装派送
			[MsgPushHandle.PushKey.PURPLE_EQUIP_GIVE] = function()
				UpdateLocalPushPurpleEquipGive(config_push)
			end,

			--7天活动之登录获得橙英雄
			[MsgPushHandle.PushKey.ORANGE_HERO_GIVE_OF_SEVEN_DAY_ACT] = function()
				UpdateLocalPushOrangeHeroGiveOfSevenDaysAct(config_push)
			end,

			--高级召唤的免费次数恢复
			[MsgPushHandle.PushKey.HIGH_RECURIT_TIMES_RECOVERY] = function()
				UpdateLocalPushHighRecuritTimesRecovery(config_push)
			end,

			--领地的巡逻时间完成
			[MsgPushHandle.PushKey.MANOR_PATROL_FINISHED] = function()
				UpdateLocalPushManorPatrolFinished(config_push)
			end,

			--每日推送、每周推送
			[Default] = function()
				UpdateLocalPushCommon(config_push)
			end,
		}
	end
end

--移除本地推送
function MsgPushHandle.RemoveLocalPushFun(key)
	CPPMsgPush:CancelTimerTask(tostring(key))
end
