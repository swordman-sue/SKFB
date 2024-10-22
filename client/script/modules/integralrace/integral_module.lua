Modules = Modules or {}

Modules.IntegralModule = Modules.IntegralModule or BaseClass(BaseModule)


function Modules.IntegralModule:__init()
	
end

function Modules.IntegralModule:__delete()
	
end

-- 消息说明: 积分赛数据请求 消息Id: 23201
function Modules.IntegralModule:ScoreMatchDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCORE_MATCH_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明: 积分赛数据响应 消息Id: 23202
-- "cur_activity_idx__H",      --当前活动索引
-- "battle_times__H",          --已挑战次数
-- "refresh_times__H",         --已刷新次数
-- "buy_battle_times__H",      --已购买挑战的次数
-- "camp__C",                  --阵营
-- "win_times__H",             --获胜次数
-- "battle_target_list__T__target_id@I##head_img@I##title_id@I##target_name@s##server_id@H##battle_value@I##is_dead@C",--挑战目标列表
-- "task_reward_record__T__task_id@H",--任务奖励记录列表(已领取)
-- is_dead 状态(0=还没挑战,1=战胜,2=战败)
-- 参数说明: 目标ID, 头像ID, 当前使用的称号(0=表示木有), 目标所在服务器, 目标战力,  是否已经死亡
function Modules.IntegralModule:ScoreMatchDataResp(protocol_data)
	CPPGameLib.PrintTable("skfb",protocol_data,"ScoreMatchDataResp ScoreMatchDataResp 1111 ")
	self.__score_match_data = CPPGameLib.CopyTblShallow(protocol_data)
	if not self.__is_take_taskreward then
		self:FireNextFrame(Macros.Event.IntegralModule.SCORE_MATCHDATA_RESP,self.__score_match_data,self.__is_refresh_target,self.__is_buy_battle_times)
	end
	self.__is_refresh_target = false
	self.__is_buy_battle_times = false
	self.__activity_open_list = {}
end

-- 消息说明: 选择阵营 消息Id: 23203
-- "camp__C",                  --选择的阵营
function Modules.IntegralModule:ScoreMatchSelectCampReq(camp)
	print("skfb","ScoreMatchSelectCampReqScoreMatchSelectCampReq ",camp)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCORE_MATCH_SELECT_CAMP_REQ)
	protocol.camp = camp
	GlobalNetAdapter:OnSend(protocol)
end

-- "camp__C",                  --选择的阵营
-- 消息说明:选择阵营响应 消息Id: 23204
function Modules.IntegralModule:ScoreMatchSelectCampResp(protocol_data)

end

-- 消息说明: 刷新目标 消息Id: 23205
function Modules.IntegralModule:ScoreMatchRefreshTargetReq()
	self.__is_refresh_target = true
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCORE_MATCH_REFRESH_TARGET_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:刷新目标响应 消息Id: 23206
function Modules.IntegralModule:ScoreMatchRefreshTargetResp(protocol_data)
	self.__is_refresh_target = false
end

-- 消息说明: 购买挑战次数请求 消息Id: 23207
function Modules.IntegralModule:ScoreMatchBuyBattleTimesReq()
	print("skfb","ScoreMatchBuyBattleTimesReqScoreMatchBuyBattleTimesReq ")
	self.__is_buy_battle_times = true
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCORE_MATCH_BUY_BATTLE_TIMES_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:购买挑战次数响应 消息Id: 23208
function Modules.IntegralModule:ScoreMatchBuyBattleTimesResp(protocol_data)
	print("skfb","ScoreMatchBuyBattleTimesRespScoreMatchBuyBattleTimesRespScoreMatchBuyBattleTimesResp ")
	self.__is_buy_battle_times = false
end

-- "task_list__T__task_id@H",--任务列表
-- 消息说明: 领取挑战任务奖励 消息Id: 23209
function Modules.IntegralModule:ScoreMatchTakeTaskRewardReq(task_list)
	self.__is_take_taskreward = true
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCORE_MATCH_TAKE_TASK_REWARD_REQ)
	protocol.task_list = task_list
	GlobalNetAdapter:OnSend(protocol)
end

-- "task_list__T__task_id@H",--任务列表
-- 消息说明:领取挑战任务奖励响应 消息Id: 23210
function Modules.IntegralModule:ScoreMatchTakeTaskRewardResp(protocol_data)
	CPPGameLib.PrintTable("skfb",protocol_data,"ScoreMatchTakeTaskRewardRespScoreMatchTakeTaskRewardRespScoreMatchTakeTaskRewardResp ")
	self.__is_take_taskreward = false
	local t1 = {}
	for _,task_info in ipairs(protocol_data.task_list) do
		table.insert(self.__score_match_data.task_reward_record,{task_id = task_info.task_id})		
		local cnf = CPPGameLib.GetConfig("ScoreMatchBattleTask",task_info.task_id,false)	
    	table.insert(t1,{id = cnf.item_id_list[1] , num = cnf.item_num_list[1]})
	end	
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    self:FireNextFrame(Macros.Event.IntegralModule.SCORE_MATCH_TAKE_TASKREWARD_RESP)
end

--整理商店购买数据数据
function Modules.IntegralModule:ClearUpShopData(buy_record_list)
	if not buy_record_list then
		return
	end
	CPPGameLib.PrintTable("skfb",buy_record_list,"buy_record_list ")
	self.__shop_buy_record_list = {}
	--购买记录
	for _,record_info in ipairs(buy_record_list) do
		local data = CPPGameLib.CopyTblShallow(record_info)
		self.__shop_buy_record_list[record_info.goods_id] = data
	end
	--商品信息
	self.__shop_message_list = {}
	for goods_id,config_info in ipairs(CPPGameLib.GetConfig("ScoreMatchShop",nil,nil,true)) do
		local data = CPPGameLib.CopyTblShallow(config_info)
		data.__shop_type = Macros.Game.SHOP.SHOP_SCORE_MATCH
		data.goods_id = goods_id
		data.haveBuyNum = self.__shop_buy_record_list[goods_id] and self.__shop_buy_record_list[goods_id].buy_num or 0
		data.__is_sell_out = data.buy_num_max ~= 0 and data.haveBuyNum >= data.buy_num_max or false
		table.insert(self.__shop_message_list,data)
	end
	--排序
	table.sort(self.__shop_message_list,function (data1,data2)
		--售完
		if data1.__is_sell_out or data2.__is_sell_out then
			--是否售完
			if data1.__is_sell_out and data2.__is_sell_out then
				return data1.sort_id < data2.sort_id
			elseif data1.__is_sell_out and not data2.__is_sell_out then
				return false
			elseif not data1.__is_sell_out and data2.__is_sell_out then
				return true
			else
			end	
		else
			return data1.sort_id < data2.sort_id
		end
	end)
	--打开荣耀商店
	CallModuleFunc(ModuleType.INTEGRAL_RACE,"OpenView",BaseViewType.INTEGRAL_GLORY_SHOP)
end

--整理商店购买数据数据
function Modules.IntegralModule:GetShopDataList()
	return self.__shop_message_list or {}
end

--获取当前阵营类型
function Modules.IntegralModule:GetCamp()
	return self.__score_match_data and self.__score_match_data.camp or 0
end

--获取可获得荣耀
function Modules.IntegralModule:GetIntergralRaceHonor(battle_value)
	local honor = 0
	if not battle_value then
		return honor 
	end	
	honor = math.floor(math.pow(battle_value,ConfigAdapter.Common.GetIntegralRaceHonorNum()/10000)+50)
	return honor 
end

--设置商店需要的数据
function Modules.IntegralModule:SaveNeedData(index,buy_num)
	self.__shop_index = index
	self.__shop_buy_num = buy_num
end

--获取商店需要的数据
function Modules.IntegralModule:GetNeedData()
	return self.__shop_index or 0,self.__shop_buy_num or 0
end

--整理任务数据
function Modules.IntegralModule:GetClearUpTaskData()
	self.__score_match_task_list = {}
	if not self.__score_match_data then
		return self.__score_match_task_list
	end
	local task_reward_record = {}
	for _,reward_info in ipairs(self.__score_match_data.task_reward_record) do
		task_reward_record[reward_info.task_id] = reward_info.task_id
	end
	local reward_list = CPPGameLib.GetConfig("ScoreMatchBattleTask",nil,nil,true)
	for id,reward_info in ipairs(reward_list) do
		local data = CPPGameLib.CopyTblShallow(reward_info)
		data.id = id
		data.have_battle_times = self.__score_match_data.battle_times
		data.is_already_get = task_reward_record[id] and true
		table.insert(self.__score_match_task_list,data)
	end
	table.sort(self.__score_match_task_list,function (data1,data2)
		if data1.is_already_get or data2.is_already_get then
			if data1.is_already_get and data2.is_already_get then
				return data1.id < data2.id
			elseif data1.is_already_get and not data2.is_already_get then
				return false
			elseif not data1.is_already_get and data2.is_already_get then
				return true
			else
			end
		else
			return data1.id < data2.id
		end
	end)
	return self.__score_match_task_list
end

--获取当前赛事是否开启
function Modules.IntegralModule:GetIsCurActivityOpen()
	if not self.__score_match_data then
		return
	end
	self.__activity_open_list = self.__activity_open_list or {}
	--当前服务器时间
	local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local scorematch_config = CPPGameLib.GetConfig("ScoreMatchActivityTime",self.__score_match_data.cur_activity_idx)	
	local GetTime = function (date_key,time_key)
		--获取当前赛季开始和结束时间
		local year = tonumber(string.sub(tostring(scorematch_config[date_key]),1,4))	
		local month = tonumber(string.sub(tostring(scorematch_config[date_key]),5,6))	
		local day = tonumber(string.sub(tostring(scorematch_config[date_key]),7,8))
		local hour = tonumber(string.sub(scorematch_config[time_key],1,2))				
		local min = tonumber(string.sub(scorematch_config[time_key],string.len(scorematch_config[time_key]) == 4 and 3 or 2,string.len(scorematch_config[time_key])))
		local time_key = TimeTool.GetTimeStampFromDate(year, month, day, hour, min, 0)
		return time_key
	end
	if not self.__activity_open_list[1] or not self.__activity_open_list[2] then
		self.__activity_open_list = {}
		table.insert(self.__activity_open_list,GetTime("start_date","start_time"))
		table.insert(self.__activity_open_list,GetTime("over_date","over_time"))
	end
	--未开始
	if cur_server_time < self.__activity_open_list[1] then
		return
	end
	--已结束
	if cur_server_time > self.__activity_open_list[2] then
		return
	end
	--查看当前服务器是否开启这活动
    local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")   
    for open_server_id,_ in ipairs(CPPGameLib.GetConfig("ScoreMatchTargetServer",nil,nil,true)) do
        if open_server_id == role_info.server_id then
        	return true,self.__activity_open_list[1],self.__activity_open_list[2]
        end
    end	
end

function Modules.IntegralModule:SetIntegralRaceBattleMessage(enemy_name,enemy_head)
	self.__integralrace_enemy_name = enemy_name
	self.__integralrace_enemy_head = enemy_head
end

function Modules.IntegralModule:GetIntegralRaceBattleMessage(enemy_name,enemy_head)
	return self.__integralrace_enemy_name,self.__integralrace_enemy_head
end

function Modules.IntegralModule:SetISWin( bool )
	self.isWin = bool
end
function Modules.IntegralModule:GetISWin( bool )
	return self.isWin
end