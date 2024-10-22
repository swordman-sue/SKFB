--
-- @brief 七天活动模块
-- @author: yjg
-- @date: 2017年1月17日10:53:55
--

Modules = Modules or {}

Modules.SevenDaysModule = Modules.SevenDaysModule or BaseClass(BaseModule)

function Modules.SevenDaysModule:__init()
	self.allBox = {}
	self.allRed = {}
	self.allShopRecord = {}

	self:SevenDayActivityCnf()
end

function Modules.SevenDaysModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------------------------------
--处理新一天的通知
function Modules.SevenDaysModule:HandleNewDayNotify()
	self.allinfo = nil
end

--------------------------------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 请求7天目标列表请求 消息Id: 23001
function Modules.SevenDaysModule:SevenDayTargetListReq( info, force_req )	
	if not self.allinfo or force_req then
		local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SEVEN_DAY_TARGET_LIST_REQ)
		GlobalNetAdapter:OnSend(protocol) 
	else
		self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY )

		--统计红点
		CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SEVEN_DEYS)
	end
end

-- 消息说明: 请求7天目标列表响应 消息Id: 23002
function Modules.SevenDaysModule:SevenDayTargetListResp( info )
	local is_first_time = not self.allinfo

	self.allinfo = {}
	for i,v in ipairs(info.target_list) do
		self:UPData( v )
	end
	for i,v in ipairs(info.reward_record_list) do
		self.allBox[v.completion_num] = true
	end

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY )

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SEVEN_DEYS, is_first_time)
end

-- 消息说明: 7天目标数据通知 消息Id: 23005
function Modules.SevenDaysModule:SevenDayTargetDataNotice( info )
	self:UPData( info )

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SEVEN_DEYS)
end

-- 消息说明: 领取7天目标奖励请求 消息Id: 23003
function Modules.SevenDaysModule:SevenDayGetRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SEVEN_DAY_GET_REWARD_REQ)
	data.target_id = info.target_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取7天目标奖励响应 消息Id: 23004
function Modules.SevenDaysModule:SevenDayGetRewardResp( info )
	print("skfb", "23004!!!!!!!!!!!!!!!!!!!!!!!!!!")
	local data = {}
	data.id = info.target_id
	data.is_get_reward = 1
	self:UPData( data )

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY_GET , info.target_id)
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_GIFT_BAG)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SEVEN_DEYS)

	--显示奖励
	local cnf = CPPGameLib.GetConfig("SevenDayTargetData", info.target_id)
	if not cnf then return end
	local t1 = {}
    for i,v in ipairs(cnf.reward_item_id_list) do
        table.insert(t1,{id = v , num = cnf.reward_item_num_list[i]})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

--购买发送
function Modules.SevenDaysModule:ShopBuyRequest( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_REQ)  
	data.type =  Macros.Game.SHOP.ACTIVITY_DISCOUNT
	data.param1 =  info.param1
	data.param2 =  1
	data.param3 =  0
	--发送请求消息
	GlobalNetAdapter:OnSend(data) 

	self.itemId_ = info.itemId_
	self.shopId = info.param1
	self.itemId = info.itemId
end

--购买响应
function Modules.SevenDaysModule:HandleBuyResponse( info )
	local data = {}
	data.id = self.itemId_
	self:UPData( data )

	if self.allShopRecord[self.shopId] then
		self.allShopRecord[self.shopId] = self.allShopRecord[self.shopId] + 1
	else
		self.allShopRecord[self.shopId] = 1
	end

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY_CHOOSE_BUY )

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SEVEN_DEYS)

	--显示奖励
	local cnf = self:SevenDayShopData( info.param1 )
    local t1 = {}
    for i,v in ipairs(cnf) do
        table.insert(t1,{id = v.item_id , num = v.item_num})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

--购买记录
function Modules.SevenDaysModule:ShopBuyRecordRequest( info )
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",Macros.Game.SHOP.ACTIVITY_DISCOUNT)
end

--记录响应
function Modules.SevenDaysModule:ShopBuyRecordResponse( info )
	for i,v in ipairs(info.buy_record_list) do
		self:ShopRecord(v)
	end
	--统计红点
	CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SEVEN_DEYS)
end

-- 消息说明: 领取完成度奖励请求 消息Id: 23006
function Modules.SevenDaysModule:SevenDayCompletionRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SEVEN_DAY_COMPLETION_REWARD_REQ)  
	data.completion_num = info.completion_num
	--发送请求消息
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取完成度奖励响应 消息Id: 23007
function Modules.SevenDaysModule:SevenDayCompletionRewardResp( info )
	self.allBox[info.completion_num] = true
	
	local cnf = CPPGameLib.GetConfig("SevenDayTargetCompletionReward", info.completion_num)
	if not cnf then end
	local t1 = {}
	for i,v in ipairs(cnf.reward_item_id_list) do
	    table.insert(t1,{id = v , num = cnf.reward_item_num_list[i] })
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_GIFT_BAG)
end

--------------------------------------------------------------------------------------------------------------------------------------------------------
--接口
--------------------------------------------------------------------------------------------------------------------------------------------------------
--本地购买记录
function Modules.SevenDaysModule:ShopRecord( info )
	self.allShopRecord[info.goods_id] = info.buy_num
end

--获得本地购买记录
function Modules.SevenDaysModule:GetShopRecord( id )
	if self.allShopRecord[id] then
		return self.allShopRecord[id]
	else
		return 0
	end
end

-- 更新数据
function Modules.SevenDaysModule:UPData( info )
	if self.allinfo then
		self.allinfo[info.id] = info
	end
end

--获得七天活动表
function Modules.SevenDaysModule:SevenDayActivityCnf( )
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( CPPGameLib.GetConfig( "SevenDayActivity" , nil , nil , true ) ) do
		if v.type == Macros.Game.SEVEN_TYPE.SEVEN then
			if not self.sevenLst then
				self.sevenLst = {}
			end
			table.insert( self.sevenLst , v )
		else
			if not self.eighiLst then
				self.eighiLst = {}
			end
			table.insert( self.eighiLst , v )
		end
	end
end

--获得七天
function Modules.SevenDaysModule:GetSevenDayCnf()
	return self.sevenLst 
end

--获得八天
function Modules.SevenDaysModule:GetEighiDayCnf()
	return self.eighiLst 
end

--获得七天活动表
function Modules.SevenDaysModule:SevenDayTargetDataCnf( target_list )
	local t1 = {}
	for i,v in ipairs(target_list) do
		table.insert( t1 , {id = v} )
	end

	local t2 = {}
	for i , v in ipairs(t1) do
		local value = 0
		local is_get_reward = 0
		
		local sevenDayCnf = CPPGameLib.GetConfig("SevenDayTargetData", v.id)
		if not sevenDayCnf then
			return
		end
		local sevenDayCnf_x = sevenDayCnf.x
		if self.allinfo and self.allinfo[v.id] then
			if self.allinfo[v.id].is_get_reward == 0 then
				value = self.allinfo[v.id].value
			else
				value = sevenDayCnf_x
			end
			is_get_reward = self.allinfo[v.id].is_get_reward
		end

		v.value = value
		v.is_get_reward = is_get_reward
		v.logic_type = sevenDayCnf.logic_type
		v.is_asc = sevenDayCnf.is_asc

		if sevenDayCnf.y ~= 0 then
			if sevenDayCnf.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.TREASURE_GATHER or 
				sevenDayCnf.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.EQUIP_LIFT then 
				v.text = string.format( sevenDayCnf.name , sevenDayCnf_x , value , sevenDayCnf_x )
			else
				v.text = string.format( sevenDayCnf.name , sevenDayCnf_x , sevenDayCnf.y , value , sevenDayCnf_x )
			end
		else
			if sevenDayCnf.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.CHARGE then
				local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
				sevenDayCnf_x = sevenDayCnf.x * addUpAddition
				value = value * addUpAddition
			end
			v.text = string.format( sevenDayCnf.name , sevenDayCnf_x , value , sevenDayCnf_x )
		end

		v.value = value
		
		if v.value >= sevenDayCnf_x then
			v.is = true
		else
			v.is = false
		end

		if sevenDayCnf.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.DEBARK or
		 sevenDayCnf.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.ATHLETICS then
			if value ~= 0 and value <= sevenDayCnf_x then
				v.is = true
				v.text = string.format( sevenDayCnf.name , sevenDayCnf_x , 1 , 1 )
			else
				v.is = false
				v.text = string.format( sevenDayCnf.name , sevenDayCnf_x , 0 , 1 )
			end
		end

		if t2[sevenDayCnf.show_type] then
			table.insert( t2[sevenDayCnf.show_type] , v )
		else
			t2[sevenDayCnf.show_type] = {}
			table.insert( t2[sevenDayCnf.show_type] , v )
		end

		v.show_type = sevenDayCnf.show_type
		v.x = sevenDayCnf_x
		v.y = sevenDayCnf.y
	end

	for i = 1 , 99  do
		if t2[i] then
			table.sort( t2[i] , function( a , b )
				if a.is_get_reward == 0 and b.is_get_reward == 0 then
					if a.is == true and b.is ~= true then
						return a.is
					elseif a.is == true and b.is == true then
						return a.id < b.id
					elseif a.is ~= true and b.is ~= true then
						return a.id < b.id
					end
				elseif a.is_get_reward == 0 and b.is_get_reward ~= 0 then
					return a.is_get_reward 
				elseif a.is_get_reward ~= 0 and b.is_get_reward ~= 0 then
					return a.id < b.id 
				end
			end )
		end
	end

	local t3 = {}
	for i,v in pairs( t2 ) do
		table.insert(t3 , v)
	end

	table.sort( t3, function ( a , b )
		return a[1].show_type < b[1].show_type
	end )

	return t3
end

--获得商店数据
function Modules.SevenDaysModule:SevenDayShopData( data )
	local t1 = {}
	local cnf = CPPGameLib.GetConfig("SevenDayShop", data )
	if not cnf then
		return
	end
	table.insert(t1 , cnf)
	return t1
end

--获得进度条的信息
function Modules.SevenDaysModule:GetProgressData( )
	--进度条最大
	local max = ConfigAdapter.Common.GetSevenActivityMAX()
	local now = 0
	if self.allinfo then
		for i,v in ipairs( CPPGameLib.GetConfig("SevenDayTargetData" , nil , nil , true ) ) do
			if v.is_completion_target == 1 then
				if self.allinfo[i] and self.allinfo[i].is_get_reward == 1 then
					now = now + 1
				end
			end
		end
	end

	--奖励箱子
	local t1 = {}
	for i,v in pairs( CPPGameLib.GetConfig("SevenDayTargetCompletionReward" , nil , nil, true ) ) do
		table.insert( t1 , v )
	end
	table.sort( t1, function ( a , b )
		return a.target_num < b.target_num
	end )

	return max , now , t1
end

--获得进度条的信息
function Modules.SevenDaysModule:GetBoxType( num )
	return self.allBox[num]
end

--计算红点数量
function Modules.SevenDaysModule:CountRedDotNum()
	local lst = {}
	if self.__cur_type == Macros.Game.SEVEN_TYPE.SEVEN then
		lst = self:GetSevenDayCnf()
	elseif self.__cur_type == Macros.Game.SEVEN_TYPE.EIGHT then
		lst = self:GetEighiDayCnf()
	end

    local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")

    local t1 = {}
    for i,v in ipairs(lst) do
    	if v.day <= time then
    		table.insert(t1,v)
    	end
    end

	local t2 = {}
	self.allRed = {}
	for i,v in ipairs( t1 ) do
		--每一天的数据
		local cnf = self:SevenDayTargetDataCnf(v.target_list)
		local t3 = {}
		for __,vv in ipairs(cnf) do
			--每一页的数据
			local t4 = {}
			for ___,vvv in ipairs(vv) do
				--每一行的数据
				local type_ = "true"
				if vvv.show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.DISCOUNT then
					local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , vvv.x )
					local time = CallModuleFunc(ModuleType.SEVENDAYS, "GetShopRecord" , lst[1].goods_id )
					if lst[1].buy_num_max - time == 0 then
						type_ = "false"
					else
						type_ = "true"
					end

					if lst[1].buy_num_max == 0 then
						type_ = "true"
					end
				elseif vvv.show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.ROD then

					local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , vvv.x )
					local time = self:GetShopRecord( lst[1].goods_id ) 
					
					if time >= lst[1].buy_num_max then
						type_ = "false"
					else
						type_ = "true"
					end
				elseif vvv.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.ATHLETICS then
				    if vvv.value ~= 0 and vvv.value <= vvv.x and vvv.is_get_reward == 0 then
				    	type_ = "true"
				    else
				    	type_ = "false"
				    end 
				else
					if vvv.is_get_reward ~= 0 then
						type_ = "false"
					else
						type_ = "true"
					end

					if vvv.value < vvv.x then
						type_ = "false"
					end
				end

				table.insert( t4 , type_ )
			end
			table.insert(t3 , t4)
		end

		table.insert(self.allRed , t3)
	end
end

--按照天数获取红点
function Modules.SevenDaysModule:GetRedDotNumByDay( sky )
	local num = 0
	if self.allRed[sky] then
		for i,v in ipairs(self.allRed[sky]) do
			for _,vv in ipairs(v) do
				if vv == "true" then
					num = num + 1
					if not Modules.Red.BaseRed.IS_SHOW_NUM then
						break
					end
				end
			end
			if num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
	end
	return num
end

--按照天数获取红点详细
function Modules.SevenDaysModule:GetRedDotInfoByDay( sky )
	return self.allRed[sky]
end

--获取奖励预览
function Modules.SevenDaysModule:GetAwardPreview( day )

	local cnf = CPPGameLib.CopyTbl( CPPGameLib.GetConfig( "SevenDayActivity" , nil , nil , true) )     
	local lst = cnf[day]

	local t1 = {}

	local t2 = {}
	for i,v in ipairs(lst.login_preview) do
		table.insert( t2 , v )
	end
	table.insert( lst , { lst = t2 , name = CPPGameLib.GetString("activity_previewType1") } )
	
	t2 = {}
	for i,v in ipairs(lst.task_preview) do
		table.insert( t2 , v )
	end
	table.insert( lst , { lst = t2 , name = CPPGameLib.GetString("activity_previewType2") } )

	t2 = {}
	table.insert( t2 , lst.sale_previw )
	table.insert( lst , { lst = t2 , name = CPPGameLib.GetString("activity_previewType3") } )

	return lst
end

--设置当前类型
function Modules.SevenDaysModule:SetCurType(var)
	self.__cur_type = var
end

--获取当前类型(七天活动/七天活动II)
function Modules.SevenDaysModule:GetCurType()
	return self.__cur_type
end