
--[[
%% @module: 限时活动汇总(春节、端午节、圣诞节....)
--]]

Modules = Modules or {}

Modules.LimitActivityModule = Modules.LimitActivityModule or BaseClass(BaseModule)

function Modules.LimitActivityModule:__init()
	self.__clear_up_data = {}
	self.__is_show_gashapon_cost_diamonds_tips = true
end

function Modules.LimitActivityModule:__delete()
	
end

--请求圣诞节数据
function Modules.LimitActivityModule:ChristmasDataReq()
	self.__is_req = true
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHRISTMAS_DATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

--请求圣诞节数据响应
function Modules.LimitActivityModule:ChristmasDataResp(protocol_data)	
	if self.__is_req then
		self.__christmas_data = protocol_data
		self:FireNextFrame(Macros.Event.LimitActivityModule.CHRISTMAS_DATA_RESP,self.__christmas_data)
	end
	self.__is_req = false
end

--进入战斗请求
function Modules.LimitActivityModule:ChristmasBattleEnterReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHRISTMAS_BATTLE_ENTER_REQ)
	GlobalNetAdapter:OnSend(data)
end

--进入战斗响应
function Modules.LimitActivityModule:ChristmasBattleEnterResp()
end

--离开战斗请求
function Modules.LimitActivityModule:ChristmasBattleLeaveReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHRISTMAS_BATTLE_LEAVE_REQ)
	GlobalNetAdapter:OnSend(data)
end

--离开战斗响应
function Modules.LimitActivityModule:ChristmasBattleLeaveResp(protocol_data)
	
end

--攻击Boss请求
function Modules.LimitActivityModule:ChristmasAttackBossReq(req_info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHRISTMAS_ATTACK_BOSS_REQ)
	data.boss_type = req_info.boss_type
	data.snowball_id = req_info.snowball_id
	GlobalNetAdapter:OnSend(data)
end

--攻击Boss响应
function Modules.LimitActivityModule:ChristmasAttackBossResp(protocol_data)
	--self:FireNextFrame(Macros.Event.LimitActivityModule.CHRISTMAS_ATTACK_BOSS_RESP,protocol_data)
end

--攻击Boss通知
function Modules.LimitActivityModule:ChristmasAttackBossNotify(protocol_data)
	self:FireNextFrame(Macros.Event.LimitActivityModule.CHRISTMAS_ATTACK_BOSS_RESP,protocol_data)
end

--领取奖励请求
function Modules.LimitActivityModule:ChristmasTakeBossRewardReq(boss_type)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHRISTMAS_TAKE_BOSS_REWARD_REQ)
	data.boss_type = boss_type
	GlobalNetAdapter:OnSend(data)
end

--领取奖励响应
function Modules.LimitActivityModule:ChristmasTakeBossRewardResp(protocol_data)
	if protocol_data.boss_type == 1 then
		self.__christmas_data.already_take_lower_boss_reward_level = self.__christmas_data.already_take_lower_boss_reward_level + 1
	else
		self.__christmas_data.already_take_high_boss_reward_level = self.__christmas_data.already_take_high_boss_reward_level + 1
	end
	self:FireNextFrame(Macros.Event.LimitActivityModule.GET_BOSS_REWARD_RESP)
end

--Boss死亡通知
function Modules.LimitActivityModule:ChristmasBossDeathNotify(protocol_data)
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	if protocol_data.killer_id == role_info.role_id then
		--配置
		local config_bosslevel = CPPGameLib.GetConfig("ChristmasBossLevel",protocol_data.boss_type)
		--获取奖励
		local getReward = {}
		local boss_level = protocol_data.boss_type == 1 and self.__christmas_data.lower_boss_level or self.__christmas_data.high_boss_level
	    for i, reward_id in ipairs(config_bosslevel.boss_level_list[boss_level].kill_reward_id_list) do
	        table.insert(getReward,{id = reward_id, num = config_bosslevel.boss_level_list[boss_level].kill_reward_num_list[i]})
	    end
	    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )
	end	    
	if protocol_data.boss_type == 1 then
		self.__christmas_data.lower_before_boss_hp = self.__christmas_data.lower_boss_hp
		self.__christmas_data.lower_boss_level = protocol_data.boss_level
		self.__christmas_data.lower_boss_hp = protocol_data.boss_hp
		self.__christmas_data.lower_boss_death_time = protocol_data.boss_death_time
	else
		self.__christmas_data.high_before_boss_hp = self.__christmas_data.high_boss_hp
		self.__christmas_data.high_boss_level = protocol_data.boss_level
		self.__christmas_data.high_boss_hp = protocol_data.boss_hp
		self.__christmas_data.high_boss_death_time = protocol_data.boss_death_time
	end
	self:FireNextFrame(Macros.Event.LimitActivityModule.CHRISTMAS_BOSSDEATH_NOTIFY,protocol_data.boss_type)
end

--抽奖请求
function Modules.LimitActivityModule:NDLotteryReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ND_LOTTERY_REQ)
	data.times = info.times
	data.is_cost_item = info.is_cost_item
	GlobalNetAdapter:OnSend(data)
end

--抽奖响应
function Modules.LimitActivityModule:NDLotteryResp(protocol_data)
	local getReward = {}
    for i, rData in ipairs(protocol_data.item_list) do
        table.insert(getReward,{id = rData.item_id, num = rData.item_num})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward,nil,nil,nil,true )
 	self:FireNextFrame(Macros.Event.LimitActivityModule.ND_LOTTERY_RESP)
end

--抽奖记录请求
function Modules.LimitActivityModule:NDLotteryRecordReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ND_LOTTERY_RECORD_REQ)
	GlobalNetAdapter:OnSend(data)
end

--抽奖记录响应
function Modules.LimitActivityModule:NDLotteryRecordResp(protocol_data)
	--打开扭蛋机
	self:FireNextFrame(Macros.Event.LimitActivityModule.ND_LOTTERYRECORD_RESP,protocol_data.lottery_record_list)
end

--阵营招募数据请求
function Modules.LimitActivityModule:CampRecruitDataReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CAMP_RECRUIT_DATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- "cur_data_id__H",   --当前数据id
-- "today_already_recruit_times__H",   --今日已招募次数
-- "camp_score_list__T__camp@C##score@I",  --积分列表(阵营,积分)
-- "score_task_reward_record_list__T__camp@C##record_list@D|RecordList|", --积分任务奖励记录列表
-- RecordList说明:
-- reward_idx@H: 奖励位置下标(从0开始)
--阵营招募数据响应
function Modules.LimitActivityModule:CampRecruitDataResp(protocol_data)
	self.__camp_recruit_data = protocol_data	
	if self.__camp_recruit_data and self.__camp_recruit_data.score_task_reward_record_list then
		for i,score_info in ipairs(self.__camp_recruit_data.score_task_reward_record_list) do
			score_info.record_list = score_info.record_list:GetData()
		end
	end
	self:FireNextFrame(Macros.Event.LimitActivityModule.CAMP_RECRUIT_DATA_RESP,self.__camp_recruit_data)
end

-- "recruit_type__H",  --招募类型(1=普通,2=一键)
-- "camp__H",  --阵营
--阵营招募请求
function Modules.LimitActivityModule:CampRecruitReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CAMP_RECRUIT_REQ)
	data.recruit_type = info.recruit_type
	data.camp = info.camp
	GlobalNetAdapter:OnSend(data)
end

-- "recruit_type__H",  --招募类型(1=普通,2=一键)
-- "camp__H",  --阵营
-- "hero_list__T__hero_id@H##hero_num@I",  --招募到的英雄列表(英雄Id,英雄数量)
--阵营招募响应
function Modules.LimitActivityModule:CampRecruitResp(protocol_data)
	if self.__camp_recruit_data then
		local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",self.__camp_recruit_data.cur_data_id)
		local add_score = (protocol_data.recruit_type == 1 and config_camp_recruit.recruit_data_list[protocol_data.camp].normal_recruit_score) or config_camp_recruit.recruit_data_list[protocol_data.camp].one_key_recruit_score
		self.__camp_recruit_data.camp_score_list[protocol_data.camp+1].score = self.__camp_recruit_data.camp_score_list[protocol_data.camp+1].score + add_score
		if protocol_data.recruit_type == 1 then
			self.__camp_recruit_data.today_already_recruit_times = self.__camp_recruit_data.today_already_recruit_times + 1
		end
	end
	self:FireNextFrame(Macros.Event.LimitActivityModule.CAMP_RECRUIT_RESP,protocol_data)
end

-- "camp__H",  --阵营
-- "reward_idx__H",    --奖励位置索引(从0开始)
--阵营招募积分任务奖励请求
function Modules.LimitActivityModule:CampRecruitScoreTaskRewardReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CAMP_RECRUIT_SCORE_TASK_REWARD_REQ)
	data.camp = info.camp
	data.reward_idx = info.reward_idx
	GlobalNetAdapter:OnSend(data)
end

-- "camp__H",  --阵营
-- "reward_idx__H",    --奖励位置索引(从0开始)
--阵营招募积分任务奖励响应
function Modules.LimitActivityModule:CampRecruitScoreTaskRewardResp(protocol_data)
	if self.__camp_recruit_data then
		for i,recruit_info in ipairs(self.__camp_recruit_data.score_task_reward_record_list) do
			if recruit_info.camp == protocol_data.camp then
				table.insert(recruit_info.record_list,{reward_idx = protocol_data.reward_idx})
				break
			end
		end
	end
	self:FireNextFrame(Macros.Event.LimitActivityModule.CAMP_RECRUIT_SCORE_TASK_REWARD_RESP,protocol_data)
end

-- "camp__H",  --阵营
--阵营招募记录请求
function Modules.LimitActivityModule:CampRecruitRecordReq(cur_camp)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CAMP_RECRUIT_RECORD_REQ)
	data.camp = cur_camp
	GlobalNetAdapter:OnSend(data)
end

-- "camp__H",	--阵营
-- "hero_list__T__role_name@s##hero_id@I##hero_num@I", 	--招募到的英雄列表(英雄Id,英雄数量)
--阵营招募记录响应
function Modules.LimitActivityModule:CampRecruitRecordResp(protocol_data)	
	self:FireNextFrame(Macros.Event.LimitActivityModule.CAMP_RECRUIT_RECORD_RESP,protocol_data)
end

--抽奖记录响应
function Modules.LimitActivityModule:GetNDLotteryCoinNum()
	local coin_num,coin_id
	local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.GASHAPONMACHINE )
    if info.show == "open" then
    	local config_num = CPPGameLib.GetConfig("NDLotteryCost",info.subtype)
    	if config_num and config_num.cost_item_id then 
    		coin_num = ObjIDTool.GetOwnNum(config_num.cost_item_id)
    		coin_id = config_num.cost_item_id
    	end       
    end
    return coin_num or 0,coin_id,info.subtype
end

--获取商店数据
function Modules.LimitActivityModule:GetShopDataByShopType(shop_type)
	--没有则请求数据
	if not self.__clear_up_data[shop_type] then
		--请求数据
		CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",shop_type)
		return
	end
	self:SortShopData(shop_type)
end

--整理商店数据
function Modules.LimitActivityModule:ClearUpShopDataByShopType(shop_type,buy_record_list,is_buy)
	local integral
	if not is_buy then
		self.__clear_up_data[shop_type] = {}
		local shop_buy_notes_data = {}
		--遍历记录
		for _,shopData in ipairs(buy_record_list) do
			shop_buy_notes_data[shopData.goods_id] = shopData.buy_num
		end
		local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		--整理数据
		for _,shop_info in pairs(CPPGameLib.GetConfig(shop_type == Macros.Game.SHOP.CHRISTMAS_INTEGRAL and "ChristmasScoreExchangeShop" or "ChristmasItemExchangeShop",nil,nil,true)) do
			local data = CPPGameLib.CopyTblShallow(shop_info)
			data.is_buy_full = false
			data.is_satisfy_level = role_level >= shop_info.need_role_level
			data.is_satisfy_score = true
			data.buy_record_num = 0
			if shop_buy_notes_data[shop_info.goods_id] then
				data.buy_record_num = shop_buy_notes_data[shop_info.goods_id]
				if shop_info.buy_num_max ~= 0 then
					data.is_buy_full = data.buy_record_num >= shop_info.buy_num_max
				end
			end
			table.insert(self.__clear_up_data[shop_type],data)
		end
	else
		for _,shop_info in ipairs(self.__clear_up_data[shop_type]) do
			if shop_info.goods_id == buy_record_list.param1 then
				--消耗积分
				if shop_info.cost_score then
					integral = shop_info.cost_score * buy_record_list.param2
				end
				--获取奖励
				local getReward = {}
			    table.insert(getReward,{id = shop_info.item_id, num = shop_info.item_num * buy_record_list.param2})
			    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )
			    --记录
				shop_info.buy_record_num = shop_info.buy_record_num + buy_record_list.param2
				if shop_info.buy_num_max ~= 0 then
					shop_info.is_buy_full = shop_info.buy_record_num >= shop_info.buy_num_max
				end
				break
			end
		end
		self:FireNextFrame(Macros.Event.LimitActivityModule.CHRISTMAS_SHOP_BUY_NOTIFY)
	end
	self:SortShopData(shop_type,integral)
end

--清空商店数据
function Modules.LimitActivityModule:SortShopData(shop_type,integral)
	if shop_type == Macros.Game.SHOP.CHRISTMAS_INTEGRAL then
		for i,shop_info in ipairs(self.__clear_up_data[shop_type]) do
			shop_info.is_satisfy_score = self.__christmas_data.total_score >= shop_info.need_history_score
		end
	end
	--排序
	table.sort(self.__clear_up_data[shop_type],function (info1,info2)
		if info1.is_satisfy_level and info2.is_satisfy_level then
			--限购完的放后面
			if info1.is_buy_full and info2.is_buy_full then
				return info1.goods_id < info2.goods_id			
			elseif info1.is_buy_full and not info2.is_buy_full then
				return false
			elseif not info1.is_buy_full and info2.is_buy_full then
				return true
			else				
				if info1.is_satisfy_score and info2.is_satisfy_score then
					return info1.goods_id < info2.goods_id
				elseif info1.is_satisfy_score and not info2.is_satisfy_score then
					return true
				elseif not info1.is_satisfy_score and info2.is_satisfy_score then
					return false
				else
					return info1.goods_id < info2.goods_id
				end	
			end
		elseif not info1.is_satisfy_level and info2.is_satisfy_level then
			return false
		elseif info1.is_satisfy_level and not info2.is_satisfy_level then
			return true
		elseif not info1.is_satisfy_level and not info2.is_satisfy_level then
			return info1.goods_id < info2.goods_id
		end
	end)
	self:FireNextFrame(Macros.Event.LimitActivityModule.GET_SHOPDATA_BY_SHOPTYPE,self.__clear_up_data[shop_type],integral)
end

--清空商店数据
function Modules.LimitActivityModule:ClearShopData()
	self.__clear_up_data = {}
end

--获取雪球积分
function Modules.LimitActivityModule:GetSnowBallIntegral()
	return self.__christmas_data and self.__christmas_data.cur_score or 0
end

--获取阵营招募数据
function Modules.LimitActivityModule:GetCampRecruitData()
	return self.__camp_recruit_data
end

--判断活动是否结束
function Modules.LimitActivityModule:GetActivityOpen(system_id)
	if not system_id then return end
	local cnf = CPPGameLib.GetConfig("Activity",system_id)
	local activityCnf = CallModuleFunc(ModuleType.ACTIVITY,"ToTypeSettleActivityType",cnf.sort_list[1].time_type,cnf.sort_list[1])

	local is_open = false
	if activityCnf.judge == "open" then
		is_open = true	
	end
	local text = TimeTool.GetTimeFormat(activityCnf.over, 21)
	return is_open,text
end

--获取活动持续时间
function Modules.LimitActivityModule:GetActivityOpenSp(system_id)
	if not system_id then return end
	local activityCnf = CallModuleFunc(ModuleType.ACTIVITY,"ToIdGetActivityTime",system_id)
	local op_txt = TimeTool.GetTimeFormat(activityCnf.star, 9) or ""
	local ov_txt = TimeTool.GetTimeFormat(activityCnf.over, 9) or ""
	return CPPGameLib.GetString("gashaponmachine_opentime",op_txt)," ~ "..ov_txt 
end

--判断活动是否结束
function Modules.LimitActivityModule:GetActivityOverTime(system_id)
	if not system_id then return end
	local info = CallModuleFunc(ModuleType.ACTIVITY,"ToIdGetActivityTime",system_id)
	return info.over
end

--显示扭蛋消耗钻石提示
function Modules.LimitActivityModule:SetIsShowGashaponCostDiamondsTips(is_hide)
	self.__is_show_gashapon_cost_diamonds_tips = not is_hide and true or false
end


--显示扭蛋消耗钻石提示
function Modules.LimitActivityModule:GetIsShowGashaponCostDiamondsTips()
	return self.__is_show_gashapon_cost_diamonds_tips
end