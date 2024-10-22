--[[
%% @module: 世界等级任务
%% @author: yjg
%% @created: 2018年5月24日10:10:12
--]]

Modules = Modules or {}

Modules.PushWorldLevelModule = Modules.PushWorldLevelModule or BaseClass(BaseModule)

function Modules.PushWorldLevelModule:__init()
	self.is_already_take_final_reward = 0
	self.task_event_list = {}
end

function Modules.PushWorldLevelModule:__delete()
end

-- 消息说明:世界等级任务数据请求 消息Id: 26901
function Modules.PushWorldLevelModule:WorldLevelTaskDataReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:世界等级任务数据请求 消息Id: 26901")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_WORLD_LEVEL_TASK_DATA_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:世界等级任务数据响应 消息Id: 26902
-- "is_already_take_final_reward__C",  --是否已领取最终奖励
-- "task_event_list__T__task_id@H##context@I##is_already_take@C", -- 任务事件列表(任务ID，上下文， 是否已领取[1=是,0=否])
function Modules.PushWorldLevelModule:WorldLevelTaskDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:世界等级任务数据响应 消息Id: 26902")
	self.is_already_take_final_reward = info.is_already_take_final_reward
	self.task_event_list = info.task_event_list
	GlobalEventSystem:FireNextFrame(Macros.Event.PushWorldLevelModule.WORLDLEVEL_DATA)
end

-- 消息说明:领取世界等级任务奖励请求 消息Id: 26903
-- "task_id__H",   --要领取的任务ID
function Modules.PushWorldLevelModule:TakeWorldLevelTaksRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取世界等级任务奖励请求 消息Id: 26903")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_WORLD_LEVEL_TASK_REWARD_REQ)
	data.task_id = info.task_id
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:领取世界等级任务奖励响应 消息Id: 26904
-- "task_id__H",   --要领取的任务ID
function Modules.PushWorldLevelModule:TakeWorldLevelTaksRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取世界等级任务奖励响应 消息Id: 26904")
	local itemLst = self:GetWorldLevelTaskEventCnf( info.task_id )
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , itemLst.lst , function( ... )
    	GlobalEventSystem:FireNextFrame(Macros.Event.RoleModule.UP_LEVEL_TRUNCATION)
    end )
end

-- 消息说明:领取世界等级任务最终奖励请求 消息Id: 26905
function Modules.PushWorldLevelModule:TakeWorldLevelTaskFinalRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取世界等级任务最终奖励请求 消息Id: 26905")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_WORLD_LEVEL_TASK_FINAL_REWARD_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:领取世界等级任务最终奖励响应 消息Id: 26906
function Modules.PushWorldLevelModule:TakeWorldLevelTaskFinalRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:领取世界等级任务最终奖励响应 消息Id: 26906")
	local itemLst = self:GetTodayAwardLst()
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , itemLst , function( ... )
    	GlobalEventSystem:FireNextFrame(Macros.Event.RoleModule.UP_LEVEL_TRUNCATION)
    end )
    self:WorldLevelTaskDataReq()
end

--获得今日大奖领取状态
function Modules.PushWorldLevelModule:GetTodayAwardType()
	if self.is_already_take_final_reward and self.is_already_take_final_reward ~= 0 then
		return true
	else
		return false
	end
end

--获得任务进度
function Modules.PushWorldLevelModule:GetTaskSchedule()
	local maxlst , lst = self:GetWorldLevelTaskEventCnf()
	return #lst , #maxlst
end

--获得世界等级
function Modules.PushWorldLevelModule:GetWorldLevel()
	--开服天数
	local openClothing = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
	--世界任务等级表
	local cnf = CPPGameLib.GetConfig( "WorldTaskLevel" , openClothing )

	local worldLevel = 0
	if not cnf then
		local t1 = CPPGameLib.GetConfig( "WorldTaskLevel" , openClothing , nil , true )
		worldLevel = t1[#t1].world_level
	else
		worldLevel = cnf.world_level
	end

	return worldLevel
end

--获得我的等级
function Modules.PushWorldLevelModule:GetWorldLevelMyLevel()
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	return level
end

--获得任务ID列表
function Modules.PushWorldLevelModule:GetTaskIdList()
	local t1 = {}
	for i , v in ipairs( self.task_event_list ) do
		table.insert( t1 , { level = v.task_level , id = v.task_id , content = v.context , getType = v.is_already_take } )  
	end
	return t1
end


--获得任务表
function Modules.PushWorldLevelModule:GetWorldLevelTaskEventCnf( task_id )
	local lst = self:GetTaskIdList()
	
	local t1 = {}
	local t2 = {}
	local t3 = {}
    local t7 = {}

    local t8 = {}
    for i , v in ipairs( lst ) do
    	local eventCnf = CPPGameLib.GetConfig( "WorldLevelTaskEvent" , v.id )
    	if eventCnf then
    		table.insert( t8 , v )
    	end
    end
    lst = t8

	for i , v in ipairs( lst ) do
		-- -- 任务等级表
		-- local worldLevelTaskCnf = CPPGameLib.GetConfig( "WorldLevelTaskIdList" , v.level )
		-- 世界等级任务事件
		local eventCnf = CPPGameLib.GetConfig( "WorldLevelTaskEvent" , v.id )

		local t5 = {}
		--经验奖励
		local expLst = self:GetTodayAwardEXP( v.level )
		if expLst and expLst.num ~= 0 then
			table.insert( t5 , expLst )
		end
		--道具奖励
		for j , vv in ipairs( eventCnf.reward_item_id_list ) do
			local itemNum = self:GetTodayAwardItemNum( v.level , eventCnf.reward_item_num_list[j] )
			table.insert( t5 , { id = vv , num = itemNum } )
		end
		--领取状态
		local gt = "yilingqu"
		if v.getType ~= 1 then --未领取
			if v.content >= eventCnf.x then
				gt = "kelingqu"
			else
				gt = "weiwancheng"
			end
		end

		local t4 = { 
					--id  		--描述					
					id = v.id , describe = eventCnf.desc , 
					--参数1 				--参数2
					parameterX = eventCnf.x , parameterY = eventCnf.y , 
					--内容  			--跳转目录
					content = v.content , approach = eventCnf.system_id ,
					--列表 	   --领取状态
					lst = t5 , getType = gt, level = v.level
		}
		if gt == "kelingqu" then
			table.insert( t1 , t4 )
			table.insert( t7 , t4 )
		elseif gt == "yilingqu" then
			table.insert( t2 , t4 )
			table.insert( t7 , t4 )
		elseif gt == "weiwancheng" then
			table.insert( t3 , t4 )
		end
	end

    table.sort( t1, function( a , b )
 		return a.level < b.level
    end )

    table.sort( t2, function( a , b )
 		return a.level < b.level
    end )

    table.sort( t3, function( a , b )
 		return a.level < b.level
    end )

    -- 3.可领取的任务置顶
    for i , v in ipairs(t3) do
    	table.insert( t1 , v )
    end

    -- 4.已领取的任务置底
    for i , v in ipairs(t2) do
    	table.insert( t1 , v )
    end

    --单独读取
    local t6 = t1
    if task_id then
    	for i , v in ipairs( t1 ) do
    		if v.id == task_id then
    			t6 = v
    			break
    		end
    	end
    end

    return t6 , t7 
end

--获得今日大奖
function Modules.PushWorldLevelModule:GetTodayAwardLst()
	--上线时等级
	local roleLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.DAILY_INIT_LEVEL)

	--开服天数
	local openClothing = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")

	--世界任务等级表
	local cnf = CPPGameLib.GetConfig( "WorldTaskLevel" , openClothing )

	if not cnf then
		return {}
	end
	--寻找奖励区间
	local section = #cnf.reward_level_section
	for i , v in ipairs( cnf.reward_level_section ) do
		if v >= roleLevel then
			section = i
			break
		end
	end
	--寻找大奖列表
	local reward_list = {}
	for i , v in ipairs( cnf.reward_id_list[ section ] ) do
		table.insert( reward_list , { id = v , num = cnf.reward_num_list[ section ][i] } )
	end
	return reward_list
end

--获得今日奖励经验
-- iii.完成任务后，可领取一定经验及部分道具奖励
-- 1.领取经验 = math.min( math.max( 世界等级 - 任务等级 , 0 ) , 系数 ) / 系数 * 任务经验
function Modules.PushWorldLevelModule:GetTodayAwardEXP( taskLevel )
	local factor = ConfigAdapter.Common.GetWorldLevelFactor()
	local cnf = CPPGameLib.GetConfig( "WorldLevelTaskIdList" , taskLevel )
	local exp = cnf and cnf.exp or 0
	local HuoBicnf = CPPGameLib.GetConfig( "HuoBi" , Macros.Game.RolePropType.EXP )
	exp = math.floor( math.min( math.max( self:GetWorldLevel() - taskLevel , 0 ) , factor ) / factor * exp )
	local t1 = { id = HuoBicnf.big_icon , num = exp , type = Macros.Game.RolePropType.EXP }
	return t1
end

--获得今日奖励道具数量
-- iii.完成任务后，可领取一定经验及部分道具奖励
-- 2.道具奖励数量 = math.floor( ( 1 + math.min( math.max( 任务等级 - 世界等级 + 系数 , 0 ) , 系数 ) / 系数 ) * 任务道具数量 )
function Modules.PushWorldLevelModule:GetTodayAwardItemNum( taskLevel , num )
	local factor = ConfigAdapter.Common.GetWorldLevelFactor()
	local item_num = math.floor( ( 1 + math.min( math.max( taskLevel - self:GetWorldLevel() + factor , 0 ) , factor ) / factor ) * num )
	return item_num
end

--红点需要提示的数量
-- function Modules.PushWorldLevelModule:GetNoticeNum()
-- 	local num , max = self:GetTaskSchedule()
-- 	if max > num then
-- 		return max - num
-- 	elseif max == num then
-- 		if self:GetTodayAwardType() == false then
-- 			return 1
-- 		else
-- 			return 0
-- 		end
-- 	end
-- end