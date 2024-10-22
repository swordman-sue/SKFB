Modules = Modules or {}
Modules.PlunderedTreasuresModule = Modules.PlunderedTreasuresModule or BaseClass(BaseModule)

Modules.PlunderedTreasuresModule.TargetType = {
	PLAYER = 1,
	MONSTER = 2,
}

function Modules.PlunderedTreasuresModule:__init()
	self:__RegistProtocolForOpenView(BaseViewType.PLUNDEREDTREASURES,Net.ProtocolNo.C2S_PLUNDEREDTREASURES_DATA_REQ, false)
end

function Modules.PlunderedTreasuresModule:__delete()
end

--夺宝数据请求
function Modules.PlunderedTreasuresModule:plunderedTreasuresDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLUNDEREDTREASURES_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.PlunderedTreasuresModule:plunderedTreasuresDataResp(data)
	self.avoidWarOverTime = data.over_time
	self.__daily_doubao_times = data.daily_doubao_times
	
	--进度奖励
	self.__pregross_reward_record = {}
	for _,pregross_info in ipairs(data.pregross_reward_record) do
		self.__pregross_reward_record[pregross_info.pregross_id] = pregross_info.pregross_id
	end

	self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHAVOIDWARTIME)

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.PLUNDEREDTREASURES)
end

--免战结束时间
function Modules.PlunderedTreasuresModule:GetAvoidWarOverTime()
	return self.avoidWarOverTime
end

--挑战对手列表申请请求
function Modules.PlunderedTreasuresModule:plunderedTreasuresTargetListReq(treasure_info_id,treasure_item_id,is_open)
	self.__is_open_plunderedtreasures_chooseenemy_view = is_open or false
	self.treasure_info_id = treasure_info_id
	self.treasure_item_id = treasure_item_id
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLUNDEREDTREASURES_TARGET_LIST_REQ)
	protocol.treasure_info_id = treasure_info_id
	protocol.treasure_item_id = treasure_item_id
	GlobalNetAdapter:OnSend(protocol)
end

--挑战对手列表申请响应
function Modules.PlunderedTreasuresModule:plunderedTreasuresTargetListResp(data)	
	for k,v in pairs(data.player_list) do
		if v.hero_list.GetData then
			v.hero_list = v.hero_list:GetData()
		end
	end
	self.__save_target_data = CPPGameLib.CopyTbl(data)	
	if self.__is_open_plunderedtreasures_chooseenemy_view then 
		self:OpenView(BaseViewType.PLUNDEREDTREASURESCHOOSE_ENEMYVIEW,data)
	else
		self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASH_TARGET_LIST,data)		
	end	
end

function Modules.PlunderedTreasuresModule:DeleteChooseEnemyView()
	CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"CloseView",BaseViewType.PLUNDEREDTREASURESCHOOSE_ENEMYVIEW)
end

function Modules.PlunderedTreasuresModule:GetCurtreasureInfoId()
	return self.treasure_info_id,self.treasure_item_id
end

--夺宝五次数据请求
function Modules.PlunderedTreasuresModule:plunderedTreasuresFiveTimesReq(info)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLUNDEREDTREASURES_FIVETIMES_REQ)
	protocol.treasure_info_id = info and info.treasure_id or self.treasure_info_id
	protocol.treasure_item_id = info and info.treasure_piece_id or self.treasure_item_id
	protocol.duobao_times = info and 1 or 5
	GlobalNetAdapter:OnSend(protocol)
end

function Modules.PlunderedTreasuresModule:plunderedTreasuresFiveTimesResp(data)
	data.treasure_item_id = self.treasure_item_id
	self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_FIVETIMES,data)			
end

--领取夺宝进度奖励请求
function Modules.PlunderedTreasuresModule:TakeDuoBaoPregressRewardReq(pregross_id_list)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_DUOBAO_PREGRESS_REWARD_REQ)
	protocol.pregross_id_list = pregross_id_list
	GlobalNetAdapter:OnSend(protocol)
end

--领取夺宝进度奖励响应
function Modules.PlunderedTreasuresModule:TakeDuoBaoPregressRewardResp(protocol)
	local t1 = {}
	for _,pregross_info in ipairs(protocol.pregross_id_list) do
		self.__pregross_reward_record[pregross_info.id] = pregross_info.id
		local progress_info = CPPGameLib.GetConfig("DoubaoPregrossReward",pregross_info.id)
		table.insert( t1 , { id = progress_info.reward_item_id_list[1] , num = progress_info.reward_item_num_list[1] })
	end			
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	
	self:FireNextFrame(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_PROGRESS_REWARD)		

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.PLUNDEREDTREASURES)
end

--计算进度奖励的红点
function Modules.PlunderedTreasuresModule:CountProgressRewardRedDot()
	local red_dot_num = 0
	if self.__pregross_reward_record and self.__daily_doubao_times then
		for progress_id,progress_info in ipairs(CPPGameLib.GetConfig("DoubaoPregrossReward",nil,nil,true)) do
			--没有领取并且夺宝次数多于可领取次数
			if not self.__pregross_reward_record[progress_id] and self.__daily_doubao_times >= progress_info.times then
				red_dot_num = red_dot_num + 1
			end
		end
	end
	return red_dot_num
end

--获取进度列表
function Modules.PlunderedTreasuresModule:GetProgressRewardList()
	local progressreward_list = {}
	local could_get
	if self.__pregross_reward_record and self.__daily_doubao_times then
		for progress_id,progress_info in ipairs(CPPGameLib.GetConfig("DoubaoPregrossReward",nil,nil,true)) do
			local cy_progress_info = CPPGameLib.CopyTblShallow(progress_info)	
			cy_progress_info.progress_id = progress_id
			cy_progress_info.cur_progress = self.__daily_doubao_times
			--没有领取并且夺宝次数多于可领取次数
			if not self.__pregross_reward_record[progress_id] and self.__daily_doubao_times >= progress_info.times then
				cy_progress_info.reward_status = 2
				could_get = true
			elseif self.__pregross_reward_record[progress_id] then
				--已领取
				cy_progress_info.reward_status = 0
			elseif not self.__pregross_reward_record[progress_id] and self.__daily_doubao_times < progress_info.times then
				--未满足
				cy_progress_info.reward_status = 1
			end
			table.insert(progressreward_list,cy_progress_info)
		end
		--排序
		table.sort(progressreward_list,function (sort1,sort2)
			if sort1.reward_status ~= sort2.reward_status then
				return sort1.reward_status > sort2.reward_status
			else
				return sort1.times < sort2.times
			end
		end)
	end
	return progressreward_list,could_get
end

--保存设置选中的要打的沙包
function Modules.PlunderedTreasuresModule:SetOnPlaye( data )
	if not self.onPlaye then
		self.onPlaye = {}
	end
	if not data then
		self.onPlaye = nil
	else
		self.onPlaye = data
	end
end

--获得选中的要打的玩家
function Modules.PlunderedTreasuresModule:GetOnPlaye()
	return self.onPlaye
end

--是否打开界面标记
function Modules.PlunderedTreasuresModule:SetIsOpenEnemyViewTag(view_tag)
	self.__is_open_view_tag = view_tag
end

--是否打开界面标记
function Modules.PlunderedTreasuresModule:GetIsSelectedTag()
	return self.__is_selected_tag
end

--是否打开界面标记
function Modules.PlunderedTreasuresModule:SetIsSelectedTag(selected_tag)
	self.__is_selected_tag = selected_tag
end

--挑战对手列表申请响应
function Modules.PlunderedTreasuresModule:OpenEnemyView()
	if self.__save_target_data and self.__is_open_view_tag then 
		self:OpenView(BaseViewType.PLUNDEREDTREASURESCHOOSE_ENEMYVIEW,self.__save_target_data)
	end
	self.__is_open_view_tag = false
end

--是否在抢夺
function Modules.PlunderedTreasuresModule:GetIsOnPlunder()
	return self.__is_on_plundering
end

--设置是否在抢夺
function Modules.PlunderedTreasuresModule:SetIsOnPlunder(is_plunder)
	self.__is_on_plundering = is_plunder
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.PlunderedTreasuresModule:CheckBattleCondition( type_ )
	return true
end