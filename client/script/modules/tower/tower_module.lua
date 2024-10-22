--
-- @brief 爬塔模块 
-- @author: yjg
-- @date: 2016年9月26日09:53:18
--

Modules = Modules or {}

Modules.TowerModule = Modules.TowerModule or BaseClass(BaseModule)

function Modules.TowerModule:__init()

	self.oneKeyIndex = 1

	self.towerData = {}
	self.towerLst = {}
	self.record = {}

	self.upDate = false
end

function Modules.TowerModule:__delete()
end

-- 爬塔数据请求
function Modules.TowerModule:FbTowerDataReq(info)
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FB_TOWER_DATA_REQ)) 
end

-- 爬塔数据响应
function Modules.TowerModule:FbTowerDataResp(info)
	self.towerData = info

	if self.oneKeyCleanInfo and self.oneKeyCleanInfo.type == "oneKeyMarch" then

	else
		if self.oneKeyCleanInfo and self.oneKeyCleanInfo.type == "common" then
		elseif self.oneKeyCleanInfo and self.oneKeyCleanInfo.type == "oneKeyMarch" then
		elseif self.oneKeyCleanInfo and self.oneKeyCleanInfo.type == "oneKey" then
		else
			GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_UP)
		end
	end
	-- else
	-- 	self.upDate = false
	-- end
end

--请求排行榜列表
function Modules.TowerModule:TowerRankReq( type )
	-- --CPPGameLib.PrintTable(type,"请求排行榜列表")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RANK_LIST_REQ)  
	data.type = type
	data.sub_type = 0
	GlobalNetAdapter:OnSend(data)
end

--请求排行榜列表响应
function Modules.TowerModule:TowerRankResp( info )
	-- -- --CPPGameLib.PrintTable(info,"请求排行榜列表响应")
	self.towerLst = info
	GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_RANK)
end

-- 重置关卡
function Modules.TowerModule:TowerResetReq(info)
	--CPPGameLib.PrintTable("xy",info,"重置关卡")
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOWER_RESET_REQ)) 
end

-- 重置关卡响应
function Modules.TowerModule:TowerResetResp(info)
	--CPPGameLib.PrintTable("xy",info,"重置关卡响应")
	self.towerData.is_need_reset = 0
	self.oneKeyIndex = 1
	GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_RESET)
end

-- 选择属性
function Modules.TowerModule:TowerSelectProReq(info)
	CPPGameLib.PrintTable("xy" , info,"选择属性")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOWER_SELECT_PRO_REQ)
	data.star = info.star
	data.pro_index = info.pro_index
	GlobalNetAdapter:OnSend(data) 
	-- self.upDate = true
end

-- 选择属性响应
function Modules.TowerModule:TowerSelectProResp(info)
	CPPGameLib.PrintTable("xy" , self.oneKeyCleanInfo,"选择属性响应")

	local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	-- CPPGameLib.PrintTable("xy",time,"扫荡响应-------------")

	if self.oneKeyCleanInfo and ( self.oneKeyCleanInfo and self.oneKeyCleanInfo.type == "oneKeyMarch") then

		if self.towerData.keep_three_star_passed_record > self.oneKeyIndex then
			self.oneKeyIndex = (self:GetCurMapLevel()/3) + 1
			--print("xy" , "==============self.oneKeyIndex1" , self.oneKeyIndex )

			self.oneKeyCleanInfo = info
			self.oneKeyCleanInfo.type = "oneKeyMarch"

			self:TowerOneKeyAllCleanReq()
		else
			--print("xy" , "==============self.oneKeyIndex2" , self.oneKeyIndex )

			self.oneKeyCleanInfo = info
			self.oneKeyCleanInfo.type = "oneKey"
			GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_SWEEP)
			
			-- self:SetIsOneKeyClean( nil )
		end

	elseif not self.oneKeyCleanInfo or self.oneKeyCleanInfo.type ~= "oneKeyMarch" then

		local t1 = CPPGameLib.GetConfig("TowerRandPro", info.star )
		local type_ , info_ = Util:Nature( t1.pro_type_list[info.pro_index + 1] , t1.pro_value_list[info.pro_index + 1] , true)
		GlobalTipMsg.GetInstance():Show( type_ .. info_ )

		GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_BUFF)

		self:SetIsOneKeyClean( nil )

	end

end

--购买发送
function Modules.TowerModule:ShopBuyRequest( info )
	-- --CPPGameLib.PrintTable(info,"购买发送")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_REQ)  

	data.type =  info.type
	data.param1 =  info.param1
	data.param2 =  info.param2
	--发送请求消息
	GlobalNetAdapter:OnSend(data) 
end

--购买响应
function Modules.TowerModule:TowerBuyResponse( info )
	-- --CPPGameLib.PrintTable(info,"购买响应")

	local data = {}
	data.goods_id = info.param1
	data.buy_num = info.param2
	self:BuyRecord(data)
end

--请求购买记录
function Modules.TowerModule:TowerBuyNotesRequest( type )
	CPPGameLib.PrintTable("xy" , info,"请求购买记录")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ)
	data.type = Macros.Game.SHOP.TOWER 
	--发送请求消息
	GlobalNetAdapter:OnSend(data)
end

--购买记录响应
function Modules.TowerModule:TowerBuyNotesResponse( info )
	CPPGameLib.PrintTable("xy" , info,"购买记录响应")
	for i,v in ipairs(info.buy_record_list) do
		self:BuyRecord(v , i , #info.buy_record_list )
	end
end

--购买无双密藏
function Modules.TowerModule:TowerBuyStarGoosReq( info )
	-- --CPPGameLib.PrintTable(info,"购买无双密藏")
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOWER_BUY_STAR_GOODS_REQ))
end

--购买无双密藏响应
function Modules.TowerModule:TowerBuyStarGoosResp( info )
	-- --CPPGameLib.PrintTable(info,"购买无双密藏响应")
	local type_ , info = self:GetStarGoods()
	local t1 = {}
	table.insert(t1,{id = info.award_id , num = info.award_num})
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	self.towerData.star_goods_index = 0
	self.towerData.is_buy_star_goods = 1

	GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_UP)
end

--请求扫荡
function Modules.TowerModule:TowerOneKeyCleanReq( info )
	-- --CPPGameLib.PrintTable("xy",info,"请求扫荡")
	-- if info.is_to_history_record == 1 then
	-- 	self:SetOneKeyType(true )
	-- end
	if not self.oneKeyCleanInfo then
		self.oneKeyCleanInfo = {}
	end
	self.oneKeyCleanInfo.type = "common"

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOWER_ONE_KEY_CLEAN_REQ)
	data.is_to_history_record = 0
	GlobalNetAdapter:OnSend( data )

	-- self.upDate = true
end

--扫荡响应
function Modules.TowerModule:TowerOneKeyCleanResp( info )
	-- --CPPGameLib.PrintTable("xy",info,"扫荡响应")
	-- GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_SWEEP)

	--扫荡
	if info.is_to_history_record == 0 then
		self.oneKeyCleanInfo = info
		self.oneKeyCleanInfo.type = "common"
		GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_SWEEP)
	--全部
	elseif info.is_to_history_record == 1 then
		self:TowerOneKeyAllCleanResp( info )
	end

	-- if info.is_to_history_record ~= 0 then
	-- 	if self.towerData and self.towerData.keep_three_star_passed_record then

	-- 		local lst = self:GetRandProList(  )
	-- 		local info = {}
	-- 		info.star = 9
	-- 		info.pro_index = lst[ #lst ].pro_index - 1
	-- 		self:TowerSelectProReq( info )

	-- 		if self.towerData.keep_three_star_passed_record > self.oneKeyIndex then

	-- 			self.oneKeyIndex = (self:GetCurMapLevel()/3) + 1

	-- 			self.oneKeyCleanInfo = info
	-- 			self.oneKeyCleanInfo.type = "oneKeyMarch"

	-- 			local info = {}
	-- 			info.is_to_history_record = 1
	-- 			self:TowerOneKeyCleanReq( info )

	-- 		else
	-- 			self.oneKeyCleanInfo = info
	-- 			self.oneKeyCleanInfo.type = "oneKey"
	-- 			GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_SWEEP)
	-- 		end
	-- 	end
	-- else
	-- 	self.oneKeyCleanInfo = info
	-- 	self.oneKeyCleanInfo.type = "common"
	-- 	GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_SWEEP)
	-- end
end

--请求全部扫荡
function Modules.TowerModule:TowerOneKeyAllCleanReq( info )
	-- --CPPGameLib.PrintTable("xy",info,"请求全部扫荡")
	--print("xy" , "请求全部扫荡=====")

	local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	CPPGameLib.PrintTable("xy",time,"请求扫荡-------------")

	if not self.oneKeyCleanInfo then
		self.oneKeyCleanInfo = {}
	end
	self.oneKeyCleanInfo.type = "oneKeyMarch"

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOWER_ONE_KEY_CLEAN_REQ)
	data.is_to_history_record = 1
	GlobalNetAdapter:OnSend( data )
end


--请求全部扫荡
function Modules.TowerModule:TowerOneKeyAllCleanResp( info )
	--print("xy" , "请求全部扫荡响应=====")
	self.oneKeyCleanInfo = info
	self.oneKeyCleanInfo.type = "oneKeyMarch"

	--选择属性
	local lst = self:GetRandProList(  )
	if lst and #lst ~= 0 then
		local info = {}
		info.star = 9
		info.pro_index = lst[ #lst ].pro_index - 1
		self:TowerSelectProReq( info )
	end
end

--获得当前关卡记录
function Modules.TowerModule:GetLevelRecord()
	return self.towerData.map_level_record
end

--获得当前所有星数
function Modules.TowerModule:GetTotalStar( ... )
	local cur_total_star = 0
	for i,v in ipairs(self.towerData.map_star_list) do
		cur_total_star = cur_total_star + v.star
	end
	return cur_total_star
end

--获得当前关卡等级  
function Modules.TowerModule:GetCurMapLevel()
	return self.towerData.cur_map_level
end

--获得可打关卡等级
function Modules.TowerModule:GetBattleMapLevel()
	return self.towerData.cur_map_level + 1
end

--获得当前选择属性列表 
function Modules.TowerModule:GetRandProList()
	local lst = {}
	local key_ = {3,6,9}
	for i,v in ipairs(self.towerData.rand_pro_list) do
		local t1 = CPPGameLib.GetConfig("TowerRandPro", key_[i] )
		lst[ i ] = {}
		lst[ i ].pro_type = t1.pro_type_list[v.pro_index + 1]
		lst[ i ].pro_value = t1.pro_value_list[v.pro_index + 1]
		lst[ i ].pro_name = t1.property_name[v.pro_index + 1]
		lst[ i ].pro_index = v.pro_index + 1
	end
	local num = #self.towerData.map_star_list % 3
	if num ~= 0 then
		lst = {}
	end
	return lst
end

--获得当前持有的buff
function Modules.TowerModule:GetHoldBuff()
	return self.towerData.select_pro_list
end

--获得重置的次数
function Modules.TowerModule:GetResetTime()
	--今天已经重置的次数
	local time = self.towerData.today_reset_times

	--按VIP的我今天有多少次重置次数
	local VIPTime = CallModuleFunc(ModuleType.VIP , "GetPrivilege" , Macros.Game.VIPType.KRUUNI_RESET)

	local lst = {}
	--消费金额
	local cnf = ConfigAdapter.Common.GetTowerConsume()
	--如果今天重置的次数 大于等于 我可以重置的次数
	if time >= VIPTime then
		table.insert( lst , { type = "no" , value = 0 } )
	-- 如果今天重置的次数 小于 我可以重置的次数
	else
		--下一次重置的次数
		local nexNum = time + 1 
		if cnf[nexNum] then
			if cnf[nexNum] == 0 then
				table.insert( lst , {type = "free" , value = 0 } )
			else
				table.insert( lst , {type = "charge" , value = cnf[nexNum] } )
			end
		else
			table.insert( lst , {type = "charge" , value = cnf[#cnf] } )
		end
	end
	
	return lst , VIPTime - time
end


--本地保存购买记录
function Modules.TowerModule:BuyRecord( data , index , max )
	if self.record[data.goods_id] then
		self.record[data.goods_id] = self.record[data.goods_id] + data.buy_num
	else
		self.record[data.goods_id] = data.buy_num
	end

	self:ShopSettle()

	if index and index == max then
		GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_RECORD)
	else
		GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_RECORD)
	end
end

--获得购买的记录
function Modules.TowerModule:GetBuyRecord( id )
	return self.record[id]
end 

--今日重置次数
function Modules.TowerModule:GetTowerResetTimes()
	return CallModuleFunc(ModuleType.VIP , "GetPrivilege" , Macros.Game.VIPType.KRUUNI_RESET) - self.towerData.today_reset_times
end

--获得可用星数
function Modules.TowerModule:GetCurStar( ... )
	return self.towerData.cur_star
end

--获得扫荡是否开启
function Modules.TowerModule:GetSweepOp(section)
	local level = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").level
	local vipType = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.KRUUNU_SWEEP)
	if level >= ConfigAdapter.Common.GetSweepLv() or vipType ~= 0 then
		return true
	else
		return false
	end
end

--获得是否能扫荡
function Modules.TowerModule:GetSweep()
	local level = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").level
	local vipType = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.KRUUNU_SWEEP)

	if vipType == 0 and level < ConfigAdapter.Common.GetSweepLv() then
		return false
	else
		local section = 0
		for i=1 , self:GetCurMapLevel() do
			if i %3 == 0 then
				section = section + 1
			end
		end
		if self.towerData.keep_three_star_passed_record > section then
			return true
		else
			return false
		end
	end
end

--获得是否能扫荡
function Modules.TowerModule:GetTollgateStar(id)
	return self.towerData.map_star_list[id]
end

--获得连续通关的章节的记录
function Modules.TowerModule:GetPassedRecord(id)
	return self.towerData.keep_three_star_passed_record
end

---------------------------------------------------------------------------------------------------
--排行榜
---------------------------------------------------------------------------------------------------
--获得当前排行榜
function Modules.TowerModule:GetRankLst()
	return self.towerLst
end

--历史最佳星数记录
function Modules.TowerModule:GetSumStar()
	return self.towerData.star_max_record
end

---------------------------------------------------------------------------------------------------
--精英挑战
---------------------------------------------------------------------------------------------------

-- 购买挑战次数请求
function Modules.TowerModule:TowerBuyTimesReq(info)
	-- --CPPGameLib.PrintTable(info,"购买挑战次数请求")
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOWER_BUY_TIMES_REQ)) 
end

-- 购买挑战次数响应
function Modules.TowerModule:TowerBuyTimesResp(info)
	-- --CPPGameLib.PrintTable(info,"购买挑战次数响应")

	-- 已经购买的精英关卡挑战次数
	self.towerData.buy_elite_map_battle_times = self.towerData.buy_elite_map_battle_times + 1 

	self:TowerEliteTime()
end

--获得精英关卡等级
function Modules.TowerModule:GetEliteLevel()
	return self.towerData.elite_map_level
end

--获得当前持有的精英挑战次数  
function Modules.TowerModule:GetEliteTime()
	local num = self.towerData.buy_elite_map_battle_times + ConfigAdapter.Common.GetTowerEliteItem() - self.towerData.elite_map_battle_times
	return num
end

--获得当前已经购买的精英挑战次数  
function Modules.TowerModule:GetAlreadyBuyTime()
	local num = self.towerData.buy_elite_map_battle_times
	return num
end

--获得当前购买次数需要消耗 
function Modules.TowerModule:GetBuyConsume()

	local buyTime = self:GetAlreadyBuyTime() 

	local eliteLst = ConfigAdapter.Common.GetTowerEliteConsume()

	local consume = 0
	if buyTime ~= 0 and not eliteLst[buyTime+1] then
		consume = eliteLst[#eliteLst]
	elseif eliteLst[buyTime+1] then
		consume = eliteLst[buyTime+1]
	end

	return consume
end

--获精英列表
function Modules.TowerModule:GetEliteLst( )
	--读取整张精英表
	local lst = {}
	-- local index = 0
	for i,v in ipairs(CPPGameLib.GetConfig("TowerEliteMap", nil, nil , true )) do
		--关卡
		local cnf = CPPGameLib.GetConfig("GameLevel",v.map_id , false )
		--开启的等级
		local normal_map_level = v.normal_map_level
		--前置
		local preposition = nil
		if CPPGameLib.GetConfig("TowerEliteMap", i-1 , false) then
			preposition = i-1
		end

		if i <= (self:GetEliteLevel() + 4)  then

			local monsterCnf = CPPGameLib.GetConfig("Monster", cnf.icon_id)
			local herocnf = CPPGameLib.GetConfig("Hero", monsterCnf.hero_id)

			table.insert( lst , { 
									level = i , 
									map_id = v.map_id , 
									icon_id = cnf.icon_id , 
									half_head = herocnf.half_head,
									name = v.name ,
									normal_map_level  = normal_map_level , 
									preposition = preposition ,
									challenge_bg = cnf.challenge_bg,
									boss_chat = v.boss_chat,
								} )
		end
	end

	return lst
end

--刷新精英挑战次数
function Modules.TowerModule:TowerEliteTime( ... )
	GlobalEventSystem:FireNextFrame(Macros.Event.TowerModule.TOWER_ELITE_TIME)
end

---------------------------------------------------------------------------------------------------
--怪物
---------------------------------------------------------------------------------------------------

--刷新怪物列表
function Modules.TowerModule:TowerMonsterStar( is )
	local t1 = {}

	local num = 1
	if is then
		num = 0
	end

	--取出当前的章节
	local chapter = CPPGameLib.GetConfig("TowerGameLevel", self:GetCurMapLevel() + num , false )

	if not chapter then
		chapter = CPPGameLib.GetConfig("TowerGameLevel", self:GetCurMapLevel() , false )
	end

	if not chapter then
		return t1
	end
	
	chapter = chapter.chapter
	for k,v in pairs(CPPGameLib.GetConfig("TowerGameLevel", nil, nil , true )) do
		if v.chapter == chapter then
			table.insert(t1,v)
		end
	end

	table.sort( t1, function ( a , b )
		return a.game_level < b.game_level
	end )

	return t1
end

---------------------------------------------------------------------------------------------------
--奖励预览
---------------------------------------------------------------------------------------------------

--获得奖励预览
function Modules.TowerModule:GetRewardPreview( num )

	num = num or 0

	local lst = self:TowerMonsterStar()
	--章节号
	local chapter = lst[1].chapter + num
	local chapterCnf = CPPGameLib.GetConfig("TowerChapter",chapter)
	local t1 = {}
	local lst = {}
	local index = 0
	for i,v in ipairs(chapterCnf.star_award_id_list) do
		for j,vv in ipairs(v) do
			index = index + 1
			table.insert( t1 , {id = vv , num = chapterCnf.star_award_num_list[i][j] , star = chapterCnf.star_list[i] })
			if j == #v then
				table.insert(lst,t1)
				t1 = {}
				index = 0
			end
		end
	end
	if #t1 ~= 0 then
		table.insert(lst,t1)
	end
	return lst
end


--获得扫荡的关卡
function Modules.TowerModule:GetOneKeyCleanInfo()
	local ended = self.oneKeyCleanInfo.ended
	local begin = self.oneKeyCleanInfo.begin

	--CPPGameLib.PrintTable("xy" , ended , "ended------------") 
	--CPPGameLib.PrintTable("xy" , begin , "begin------------") 

	local t1 = {}
	for i = begin , ended do
		table.insert( t1 , i )
	end

	local t2 = {}
	for i=2,#t1 do
		local map = CPPGameLib.GetConfig("TowerGameLevel", t1[i])
		table.insert( t2 , { lv = t1[i] , map = map.map_list[3] } )
	end

	return t2
end

--获得是否是扫荡出来
function Modules.TowerModule:GetIsOneKeyClean()
	if self.oneKeyCleanInfo then
		return self.oneKeyCleanInfo.type
	else
		return nil
	end
end

--设置是否是扫荡出来
function Modules.TowerModule:SetIsOneKeyClean( bool )
	if not self.oneKeyCleanInfo then
		self.oneKeyCleanInfo = {}
	end
	self.oneKeyCleanInfo.type = bool
end

--一键扫荡开始标识
function Modules.TowerModule:SetOneKeyType( bool )
	self.oneKeyType = bool
end

--设置是否是扫荡出来
function Modules.TowerModule:GetOneKeyType( bool )
	return self.oneKeyType
end

--一键扫荡开始标识
function Modules.TowerModule:SetOneKeyNum( )
	self.oneKeyNum = (self:GetCurMapLevel()/3)
end

--一键扫荡开始标识
function Modules.TowerModule:GetOneKeyNum( bool )
	return self.oneKeyNum or 0
end

--获得当前章节持有星数
function Modules.TowerModule:GetNowHoldStar( ... )
	local chapter = math.ceil(#self.towerData.map_star_list / 3)
	local star = 0
	for i,v in ipairs(self.towerData.map_star_list) do
		if math.ceil(i/3) == chapter then
			star = star + v.star
		end
	end
	return star
end

--获得当前可以买的道具数量（红点
function Modules.TowerModule:GetNowBuyNum( ... )
	local lst = {}
	local index = 0
	for i,v in pairs( CPPGameLib.GetConfig("TowerShop", nil, nil , true ) ) do
		if not lst[v.type] then
			lst[v.type] = {}
		end

		local possess = 0
		if v.cost_type_list then
			for i , vv in ipairs( v.cost_type_list ) do
				local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , vv )
				if item_num > v.cost_value_list[i] then
					possess = possess + 1
				end
			end
		end

		if v.cost_item_id_list then
			for i , vv in ipairs( v.cost_item_id_list ) do
				local item_num = CallModuleFunc( ModuleType.Pack,"GetNumByItemID" , vv )
				if item_num > v.cost_item_num_list[i] then
					possess = possess + 1
				end
			end
		end

		local type_ = true 
		if v.buy_num_max ~= 0 and v.type == 4 then
			if self:GetBuyRecord(i) then
				if self:GetBuyRecord(i) >= v.buy_num_max then
					type_ = false
				end
			end
		end

		if possess ~= 0 and v.need_star <= ( self:GetSumStar() or 0 ) and type_ == true and v.type == 4  then
			index = index + 1
		end

	end

	return index
end

---------------------------------------------------------------------------------------------------
--商店整合
---------------------------------------------------------------------------------------------------

function Modules.TowerModule:ShopSettle()
	--四个页签
    local lst = {}
    lst[Macros.Game.TowerShop.SHANGPING] = {}
    lst[Macros.Game.TowerShop.ZIZHUANG] = {}
    lst[Macros.Game.TowerShop.CHENGZHUANG] = {}
    lst[Macros.Game.TowerShop.JIANGLI] = {}
    lst[Macros.Game.TowerShop.HONGZHUANG] = {}

    --开启的道具
    for k,v in pairs( CPPGameLib.GetConfig("TowerShop", nil, nil , true ) ) do
		table.insert( lst[v.type] , { 
										id = k , 
										type = v.type,
										object_id = v.object_id,
										object_num = v.object_num,
										need_level = v.need_level,
										need_star = v.need_star,
										buy_num_max = v.buy_num_max,
										cost_type_list = v.cost_type_list,
										cost_value_list = v.cost_value_list,

										cost_item_id_list = v.cost_item_id_list,
										cost_item_num_list = v.cost_item_num_list,
										discount = v.discount
										} )
    end

    --五个页签的所有内容
    self.towerShopLst = {}
    for i,v in ipairs(lst) do
    	table.sort( v, function ( a , b )
    		return a.id < b.id
    	end )

        self.towerShopLst[i] = v
    end

    --历史最高星数
    local nowStar = CallModuleFunc(ModuleType.TOWER , "GetSumStar")

    --处理奖励的列表
    local t1 = {}
    local t2 = {}
    for i , v in pairs( self.towerShopLst[Macros.Game.TowerShop.JIANGLI] ) do
    	--购买记录
    	local record = self:GetBuyRecord( v.id )
    	--满了
    	if record and record >= v.buy_num_max then
    		table.insert( t1 , v )
    	else
    		table.insert( t2 , v )
    	end
    end

    --买完了的
    table.sort( t1, function ( a , b )
    	return a.id < b.id
    end )
    --还没买完的
    table.sort( t2, function ( a , b )
    	return a.id < b.id
    end )

    for i,v in ipairs(t1) do
    	table.insert( t2 , v )
    end
    self.towerShopLst[Macros.Game.TowerShop.JIANGLI] = t2
end

function Modules.TowerModule:GetShopLst( id )
	return self.towerShopLst[id]
end

function Modules.TowerModule:GetAllShopLst(  )
	return self.towerShopLst
end

function Modules.TowerModule:GetOnShopPos( id )
	--页签
	local yeqian = 1
	for k , v in pairs( self.towerShopLst ) do
		for kk , vv in pairs( v ) do
			if vv.object_id == id then
				yeqian = k
				break
			end
		end
	end

	--页面
	local item = 0
	local lst = self:GetShopLst( yeqian )
	local cnf = Util:SegmentationTab( lst , 6 )
	for i , v in ipairs( cnf ) do
		for k , vv in pairs( v ) do
			if vv.object_id == id then
				item = i
			end
		end
	end

	return yeqian , item
end

--是否第一次进界面
function Modules.TowerModule:SetIsOneOpen( var )
	self.isOneOpen = var
end

--是否第一次进界面
function Modules.TowerModule:GetIsOneOpen( var )
	return self.isOneOpen
end







----------------------------------------------------------------------------------------------------------------------------
--战斗
----------------------------------------------------------------------------------------------------------------------------

--保存战斗出来的数据
function Modules.TowerModule:SetbattleData( info )
	self.battleData = info
end

function Modules.TowerModule:GetbattleData( info )
	return self.battleData
end








----------------------------------------------------------------------------------------------------------------------------
--失败
----------------------------------------------------------------------------------------------------------------------------

--是否需要重置
function Modules.TowerModule:IsNeedReset()
	local type_ = false
	--如果需要重置
	-- --print("xy" , "self.towerData.is_need_reset-------------" , self.towerData.is_need_reset )
	if self.towerData.is_need_reset ~= 0 then
		type_ = true
	end
	return type_ 
end

--有没有商品给我
function Modules.TowerModule:GetStarGoods()
	--是否需要重置
	local reset = self:IsNeedReset()
		--如果需要重置 			而且还没购买		
	if reset == true and (self.towerData.is_buy_star_goods == 0) then

		local star = self:GetTotalStar()
		--遍历表
		for k,v in pairs(CPPGameLib.GetConfig("TowerStarGoods", nil, nil , true )) do
			if v.star_min < star and v.star_max > star then
				t1 = v
				break
			elseif v.star_min == star or v.star_max == star then
				t1 = v
				break
			end
		end
	
		local info = {}
		--道具的id
		info.award_id = t1.award_id_list[self.towerData.star_goods_index + 1 ]
		--道具的数量
		info.award_num = t1.award_num_list[self.towerData.star_goods_index + 1 ]
		--道具的价格
		info.award_price = t1.award_price_list[self.towerData.star_goods_index + 1 ]
		--道具的原价
		info.origina = t1.original_price[self.towerData.star_goods_index + 1 ]
		
		--CPPGameLib.PrintTable("xy" , info,"物品列表")

		return true , info
	else
		return false
	end
end

--是否显示试炼密藏
function Modules.TowerModule:SetShowAwardType( var )
	self.showAwardType = var
end

function Modules.TowerModule:GetShowAwardType( var )
	return self.showAwardType 
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.TowerModule:CheckBattleCondition( type_ )
	--爬塔精英
	if type_ == Macros.Game.FBType.TOWER_ELITE then
		local num = CallModuleFunc(ModuleType.TOWER , "GetEliteTime")
		if num > 0 then
			return true
		end
	--爬塔普通
	elseif type_ == Macros.Game.FBType.TOWER_COMMON then
		return true
	end
end