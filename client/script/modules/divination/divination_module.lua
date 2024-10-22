--[[
%% @module: 占卜
%% @author: yjg
%% @created: 2017年11月14日11:36:19
--]]

Modules = Modules or {}

Modules.DivinationModule = Modules.DivinationModule or BaseClass(BaseModule)

function Modules.DivinationModule:__init()
	self.divinationLst = {}
	self.dogFood = {}
end

function Modules.DivinationModule:__delete()
	if self.lowRenewTime then
		GlobalTimerQuest:CancelQuest( self.lowRenewTime )
		self.lowRenewTime = nil
	end
	if self.highRenewTime then
		GlobalTimerQuest:CancelQuest( self.highRenewTime )
		self.highRenewTime = nil
	end
end

-- 消息说明: 卡牌列表请求 消息Id: 23103
function Modules.DivinationModule:CardListReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CARD_LIST_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 卡牌列表响应 消息Id: 23104
-- uid : CARD唯一ID
-- info_id : 信息ID
-- level : CARD等级
-- exp : CARD经验
-- break_level: 突破等级
-- hero_id:英雄id
-- 消息说明: 卡牌列表响应 消息Id: 23104
function Modules.DivinationModule:CardListResp( info )
	--初始化卡牌列表
	for i , v in ipairs( info.card_list ) do
		v.flag = 1
		self:UPDataNoticeData( v , true )
	end

	--初始化新增记录表
    local lst = CPPGameLib.SplitEx( Cookies.Get( Cookies.LevelType.ROLE , Macros.Setting.NewCard) , "<GH>")
    for k , v in pairs( lst ) do
        self:AddToNewRecordMap( tonumber(v) , true )
    end

	self:FireNextFrame(Macros.Event.DivinationModule.DIVINATION)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.DIVINATION , nil , Modules.RedMgr.SystemID.DIVINATION_BOOK)
end

-- 消息说明: 卡牌数据通知 消息Id: 23121
-- "uid__I",       --卡牌唯一ID
-- "info_id__I",   --卡牌信息ID
-- "level__H",     --卡牌等级
-- "exp_I",        --卡牌经验
-- "break_level__H",--卡牌等级
function Modules.DivinationModule:CardDataNotice( info )
	self:UPDataNoticeData( info )
end

-- 消息说明: 命运商店购买 消息Id: 
function Modules.DivinationModule:DestinyBuyRequest( info )
	local data = {}
	data.type = Macros.Game.SHOP.DESTINY
	data.param1 =  info.param1 or 0
	data.param2 =  info.param2 or 0
	data.param3 =  info.param3 or 0
	CallModuleFunc(ModuleType.SHOP, "ShopBuyRequest" , data )
end

-- 消息说明: 命运商店响应 消息Id: 
function Modules.DivinationModule:DestinyBuyResponse( info )	
	local lst = CPPGameLib.GetConfig("DestinyShop", info.param1 , false )
	local t1 = {}
    table.insert(t1,{id = lst.object_id , num = lst.object_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_DESTINY)
end

-- 消息说明: 收藏值排行榜 消息Id: 
function Modules.DivinationModule:DivinationRankListRequest( info )
	local info = {}
	info.type = Macros.Game.RankType.CARD
	CallModuleFunc(ModuleType.RANK,"RankListRequest",info)
end

-- 消息说明: 收藏值排行榜响应 消息Id: 
function Modules.DivinationModule:DivinationRankListResponse( info )
	self.RankInfo = info
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_RANK)
end

-- 消息说明: 玩家占卜数据请求 消息Id: 23101
function Modules.DivinationModule:PlayerZhanbuDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLAYER_ZHANBU_DATA_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 玩家占卜数据响应 消息Id: 23102
-- "history_collection_value__I",  --历史收藏值
-- "low_card_group_map_idx__H",    --低级卡组关卡索引(当前真正的等级)
-- "cur_low_card_group_id__H", --当前低级卡组ID(历史卡库id)
-- "low_card_group_last_recover_time__I",  --低级卡组最近恢复时间
-- "cur_low_card_group_refresh_times__H",  --当前低级卡组拥有的刷新次数
-- "today_low_card_group_open_card_times__H",  --今日低级卡组翻卡次数
-- "high_card_group_last_refresh_time__I", --高级卡组最近刷新时间
-- "low_card_group_list__T__card_id@I##is_already_open@C", -- 低级卡组列表
-- "high_card_group_list__T__card_id@I##is_already_open@C", -- 高级卡组列表
function Modules.DivinationModule:PlayerZhanbuDataResp( info )
	if not self.zhanbuData then
		self.zhanbuData = {}
	end
	self.zhanbuData = info

	self:Countdown()
	self:HighCountdown()

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_DEAL)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.DIVINATION , nil , Modules.RedMgr.SystemID.DIVINATION_ACHIEVEMENT)
end

-- 消息说明: 卡牌刷新请求 消息Id: 23111
function Modules.DivinationModule:CardRefreshReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CARD_REFRESH_REQ)
	data.refresh_type = info.refresh_type
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 卡牌刷新响应 消息Id: 23112
function Modules.DivinationModule:CardRefreshResp( info )	
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_SHUFFLE , info )
	--刷新
	self:PlayerZhanbuDataReq()
end

-- 消息说明: 低级卡组刷新次数更新请求 消息Id: 23115
function Modules.DivinationModule:LowCardGroupRefreshTimesUpdateReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LOW_CARD_GROUP_REFRESH_TIMES_UPDATE_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 低级卡组刷新次数更新响应 消息Id: 23116
function Modules.DivinationModule:LowCardGroupRefreshTimesUpdateResp( info )
	self.zhanbuData.low_card_group_last_recover_time = info.low_card_group_last_recover_time
	self.zhanbuData.cur_low_card_group_refresh_times = info.cur_low_card_group_refresh_times

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_LOW_RENEW)
	self:Countdown()
end

-- 消息说明: 收藏组激活请求 消息Id: 23117
function Modules.DivinationModule:CellectionGroupActivateReq( info )
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY )  

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CELLECTION_GROUP_ACTIVATE_REQ)
	data.book_id = info.book_id
	data.group_id = info.group_id
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 收藏组激活响应 消息Id: 23118
function Modules.DivinationModule:CellectionGroupActivateResp( info )
	self.zhanbuData.history_collection_value = info.history_collection_calue
	self:UpdateBookList( info.book_id , info.group_id , 1 )

	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_DIVINATION_COLLECT ,false , info.group_id , 0 , function ( ... )
    	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , true )

        local layout = Util:Layout( 100 , 100 )
        layout:SetAnchorPoint( 0.5 , 0.5 )

		local cnf = self:GetCollectionGroupCnf( info.group_id )
        --收藏值
        local divination_collectValue = Util:Name( CPPGameLib.GetString("divination_collectValue") .. "+" .. cnf.activate_collection_value , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
        divination_collectValue:SetAnchorPoint( 0.5 , 0.5 )
        layout:AddChild(divination_collectValue)
        PosTool.Center( divination_collectValue )

        --属性
        local lst = self:ToGroupIdGetProLst( nil , info.group_id , 1 )
		for i , v in ipairs( lst ) do
			local type_ , value_ = Util:Nature( v.type , v.value , true , true)
			local text = Util:Name( type_ .. " +" .. value_ , nil , Macros.Game.QualityType.GREEN )
	        text:SetAnchorPoint( 0.5 , 0.5 )
	        text:SetPosition( layout:GetContentWidth()/2 , divination_collectValue:GetPositionY() - 30 - ( ( i-1 ) * 30 )  )
	        layout:AddChild(text)
		end
        GlobalTipMsg.GetInstance():ShowLayer( layout , false , true , true  )
	end )

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.COLLECT_UP )
end

-- 消息说明: 收藏组升星请求 消息Id: 23119
function Modules.DivinationModule:CellectionGroupStarUpReq( info )
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY ) 

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CELLECTION_GROUP_STAR_UP_REQ)
	data.book_id = info.book_id
	data.group_id = info.group_id
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 收藏组升星响应 消息Id: 23120
function Modules.DivinationModule:CellectionGroupStarUpResp( info )
	self.zhanbuData.history_collection_value = info.history_collection_calue

	local num = self:ToBookAndIdGetStar( info.book_id , info.group_id )
	self:UpdateBookList( info.book_id , info.group_id , num + 1 )

	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_DIVINATION_COLLECT ,false , info.group_id , num + 1 , function ( ... )
    	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , true )

        local layout = Util:Layout( 100 , 100 )
        layout:SetAnchorPoint( 0.5 , 0.5 )

		local cnf = self:GetCollectionGroupCnf( info.group_id )
        --收藏值
        local divination_collectValue = Util:Name( 
        											CPPGameLib.GetString("divination_collectValue") .. "+" .. cnf.activate_collection_value + ( cnf.upgrade_collection_value * num  ) , 
        											Macros.TypefaceSize.popup , 
        											Macros.Game.QualityType.GREEN )
        divination_collectValue:SetAnchorPoint( 0.5 , 0.5 )
        layout:AddChild(divination_collectValue)
        PosTool.Center( divination_collectValue )

        --属性
        local lst = self:ToGroupIdGetProLst( nil , info.group_id , 1 )
		for i , v in ipairs( lst ) do
			local type_ , value_ = Util:Nature( v.type , v.value , true , true)
			local text = Util:Name( type_ .. " +" .. value_ , nil , Macros.Game.QualityType.GREEN )
	        text:SetAnchorPoint( 0.5 , 0.5 )
	        text:SetPosition( layout:GetContentWidth()/2 , divination_collectValue:GetPositionY() - 30 - ( ( i-1 ) * 30 )  )
	        layout:AddChild(text)
		end
        GlobalTipMsg.GetInstance():ShowLayer( layout , false , true , true  )
	end )

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.COLLECT_UP )
end

-- 消息说明: 收藏组重置(取卡)请求 消息Id: 23122
function Modules.DivinationModule:CellectionGroupRestReq( info )
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , nil )

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CELLECTION_GROUP_RESET_REQ)
	data.book_id = info.book_id
	data.group_id = info.group_id
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 收藏组重置(取卡)响应 消息Id: 23123
function Modules.DivinationModule:CellectionGroupRestResp( info )
	self:UpdateBookList( info.book_id , info.group_id , 0 )
	local book = self:GetCollectionBookCnf( info.book_id )

	local group = nil
	for i , v in ipairs(book.collection_group_list) do
		if v == info.group_id then
			group = v
		end
	end
	local cnf = self:GetCollectionGroupCnf( group )

	local t1 = {}
    for i,v in ipairs( cnf.cost_card_list ) do
        table.insert(t1 , { id = v , num = info.star  })
    end

    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 ,function ( ... )
    	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , true )

        local layout = Util:Layout( 100 , 100 )
        layout:SetAnchorPoint( 0.5 , 0.5 )

        --收藏值
        local divination_collectValue = Util:Name( CPPGameLib.GetString("divination_collectValue") , Macros.TypefaceSize.popup , Macros.Game.QualityType.RED )
        divination_collectValue:SetAnchorPoint( 0.5 , 0.5 )
        layout:AddChild(divination_collectValue)
        PosTool.Center( divination_collectValue )

        --属性
        local lst = self:ToGroupIdGetProLst( nil , info.group_id , info.star )
		for i , v in ipairs( lst ) do
			local type_ , value_ = Util:Nature( v.type , v.value , true , true)
			local text = Util:Name( type_ .. " -" .. value_ , nil , Macros.Game.QualityType.RED )
	        text:SetAnchorPoint( 0.5 , 0.5 )
	        text:SetPosition( layout:GetContentWidth()/2 , divination_collectValue:GetPositionY() - 30 - ( ( i-1 ) * 30 )  )
	        layout:AddChild(text)
		end
        GlobalTipMsg.GetInstance():ShowLayer( layout , false , true , true  )
    end)

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.COLLECT_UP )
end

-- 消息说明: 收藏书列表请求 消息Id: 23124
function Modules.DivinationModule:CollectionBookListReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_COLLECTION_BOOK_LIST_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 收藏书列表响应 消息Id: 23125
function Modules.DivinationModule:CollectionBookListResp( info )
	for i , v in ipairs( info.book_list ) do
		for ii , vv in ipairs( v.group_list:GetData() ) do
			self:UpdateBookList( v.book_id , vv.group_id , vv.star , true )
		end
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.COLLECT_BOOK_UP)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.DIVINATION , nil , Modules.RedMgr.SystemID.DIVINATION_BOOK)	
end

-- 消息说明: 装备卡牌请求 消息Id: 23126
function Modules.DivinationModule:CollectionEquipCardReq( info )
	CallModuleFunc(ModuleType.HERO,"SaveHeroPropertyBeforeChange")
	CallModuleFunc(ModuleType.HERO,"JudgeIsActivatedByLineupIndex",info.formation_pos or 1)	

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_COLLECTION_EQUIP_CARD_REQ)
	data.formation_pos = info.formation_pos
	data.card_slot = info.card_slot
	data.card_uid = info.card_uid
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 装备卡牌响应 消息Id: 23127
function Modules.DivinationModule:CollectionEquipCardResp( info )
	--阵容飘字
	CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)
    CallModuleFunc(ModuleType.HERO,"SetMessageTypeAndTranslateInfo",Macros.Game.WIND_WORDS_LINEUP_TYPE.DIVINATION_MESSAGE,info)
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_DRESS)
end

-- 消息说明: 占卜成就激活请求 消息Id: 23130
function Modules.DivinationModule:ZhanBuAchievementAcivateReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ZHAN_BU_ACHIEVEMENT_ACIVATE_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 占卜成就激活响应 消息Id: 23131
function Modules.DivinationModule:ZhanBuAchievementAcivateResp( info )
	self.zhanbuData.achievement_level = info.cur_achievement_level
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.COLLECT_ACHIEVEMENT)

	--统计红点
	CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.DIVINATION , nil , Modules.RedMgr.SystemID.DIVINATION_ACHIEVEMENT)
end

-- 消息说明: 星魂商店数据请求 消息Id: 20915
function Modules.DivinationModule:StarSoulShopDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_STAR_SOUL_SHOP_DATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 星魂商店数据响应 消息Id: 20916
function Modules.DivinationModule:StarSoulShopDataResp( info )
	if not self.starSoulShop then
		self.starSoulShop = {}
	end
	self.starSoulShop = info
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_STAR_SHOP)
end

-- 消息说明: 星魂商店购买 消息Id: 
function Modules.DivinationModule:DestinyStarBuyRequest( info )
	local data = {}
	data.type = Macros.Game.SHOP.STAR_SOUL
	data.param1 =  info.param1 or 0
	data.param2 =  info.param2 or 0
	data.param3 =  info.param3 or 0
	CallModuleFunc(ModuleType.SHOP, "ShopBuyRequest" , data )
end

-- 消息说明: 星魂商店响应 消息Id: 
function Modules.DivinationModule:DestinyStarBuyResponse( info )	
	local lst = CPPGameLib.GetConfig("StarSoulShop", info.param1 , false )
	local t1 = {}
    table.insert(t1,{id = lst.item_id , num = lst.item_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_DESTINY)
end

-- 消息说明: 星魂刷新 消息Id: 
function Modules.DivinationModule:DestinyStarShopRefreshReq( info )
	local data = {}
	data.shop_type = Macros.Game.SHOP.STAR_SOUL
	data.refresh_type = info.refresh_type

	CallModuleFunc(ModuleType.HERO_SHOP , "ShopRefreshReq" ,data )
end

-- 消息说明: 兑换商店 消息Id: 
function Modules.DivinationModule:DestinyExchangeShopResp( info )
	local cnf = CPPGameLib.GetConfig( "HideShop" , info.param1 )
	local t1 = {}
    table.insert(t1,{id = cnf.item_id , num = cnf.item_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_CONVERT)
end

--星魂商店的列表
function Modules.DivinationModule:GetStarSoulShopLst( info )
	self:TidyStarSoulShopIdGetFate()
	--商品列表
	local t1 = {}
	for i , v in ipairs( self.starSoulShop.goods_list ) do
		local info = CPPGameLib.GetConfig("StarSoulShop", v.goods_id )
		table.insert( t1 , { cnf = info } )
		t1[#t1].buy_num = v.buy_num
	end
	return t1
end

--星魂最近一次的刷新时间
function Modules.DivinationModule:GetStarSoulShopTime( info )
	return self.starSoulShop.last_auto_refresh_time
end

--星魂刷新了几次
function Modules.DivinationModule:GetStarSoulShopUpTimes( info )
	return self.starSoulShop.refresh_times
end

--整理阵位上的卡牌宿命
function Modules.DivinationModule:TidyStarSoulShopIdGetFate( )
	self.starSoulShopFate = {}
	local lineupList = CallModuleFunc(ModuleType.HERO, "GetLineupList")
	for i1 , t1 in ipairs( lineupList ) do
		--该阵位上的英雄的宿命情况
		local reinforcement_list = {}
		if t1.uid ~= 0 then 
			reinforcement_list = CallModuleFunc(ModuleType.HERO, "GetHeroReinforcementInfoList", t1.uid)
		end
		--该英雄对应的宿命列表
		local t4 = {}
		for i2 , t2 in ipairs( reinforcement_list ) do
			if t2.is_card == true then
				for i3 , t3 in ipairs( t2.recommend_card_list ) do
					local heroInfo = CallModuleFunc(ModuleType.HERO, "GetHeroInfo" , t1.uid )
					local cnf = self:ToInfoIdGetDivinationInfo( t3 )
					if not t4[ cnf.type ] then
						t4[ cnf.type ] = {}
					end
					t4[ cnf.type ] = { card_Id = 0 , quality = 0 , card_Type = cnf.type , hero_Id = cnf.hero_id }
				end
				break
			end
		end

		--穿戴着的列表
		local dress = CallModuleFunc(ModuleType.HERO, "GetDivinationListByLineupIndex" , i1 )

		for i5 , t5 in ipairs( dress ) do


			if t5 ~= 0 then
				local cnf = self:GetDivination( t5 )
			end
		end
		table.insert( self.starSoulShopFate , t4 )
	end
end

--根据卡牌id获得能不能达成宿命，是不是比我穿着的强 比哪个强
function Modules.DivinationModule:ToStarSoulShopIdGetFateType( info_id )
	local cnf = self:ToInfoIdGetDivinationInfo( info_id )
	for i , v in ipairs( self.starSoulShopFate ) do
		if v[ cnf.type ] and  cnf.quality > v[cnf.type].quality  then
			return true
		end
	end
end

function Modules.DivinationModule:UpdateBookList( book_id , group_id , star , for_init )
	if not self.bookList then
		self.bookList = {}		
	end

	--这书不存在
	if not self.bookList[book_id] then
		self.bookList[book_id] = {}
		table.insert( self.bookList[book_id] , { id = group_id , star = star } )
	--有这个书
	elseif self.bookList[book_id] then
		local t1 = {}
		for i , v in pairs(self.bookList) do
			for ii , vv in ipairs( v ) do
				t1[vv.id] = vv.star
			end
		end
		--存在这个id
		if t1[ group_id ] then
			for i , v in pairs(self.bookList) do
				for ii , vv in ipairs( v ) do
					if vv.id == group_id then
						vv.star = star
					end
				end
			end
		else
			table.insert( self.bookList[book_id] , { id = group_id , star = star } )
		end
	end

	local t1 = {}
	for i , v in ipairs( self.bookList[book_id] ) do
		if v.star ~= 0 then
			table.insert( t1 , v )
		end
	end
	self.bookList[book_id] = t1

	--统计红点
	if not for_init then
		CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.DIVINATION , nil , Modules.RedMgr.SystemID.DIVINATION_BOOK)
	end
end

--获得这个书的这个id的星级
function Modules.DivinationModule:ToBookAndIdGetStar( book_id , id )
	if self.bookList and self.bookList[book_id] then
		for i , v in ipairs( self.bookList[book_id] ) do
			if v.id == id then
				return v.star
			end
		end
	end
	return 0
end

--获得历史收藏值
function Modules.DivinationModule:GetHistoryCollectionValue( )
	if not self.zhanbuData then
		return 0 
	end
	return self.zhanbuData.history_collection_value
end

--获得当前收藏值
function Modules.DivinationModule:GetNowCollectionValue( )
	local value = 0
	if not self.bookList then
		return value
	end
	for book_id , book_lst in pairs( self.bookList ) do
		for _ , v in pairs( book_lst ) do
			local pro = self:GetCollectionGroupCnf( v.id )
			value = value + pro.activate_collection_value + ( pro.upgrade_collection_value * ( v.star - 1 ) )
		end
	end
	return value
end

--获得当前卡库等级(历史或现在)
function Modules.DivinationModule:GetCurLowCardGroupId( info )
	if not self.zhanbuData then
		return
	end
	return self.zhanbuData.cur_low_card_group_id
end

--获得当前卡组等级(现在)
function Modules.DivinationModule:GetLowCardGroupMapIdx( info )
	if not self.zhanbuData then
		return
	end
	return self.zhanbuData.low_card_group_map_idx
end

--获得当前屌丝卡组信息
function Modules.DivinationModule:GetLowBCardGroupInfo( )
	if not self.zhanbuData then
		return
	end

	local t1 = {}
	--低级卡组最近恢复时间
	t1.low_card_group_last_recover_time = self.zhanbuData.low_card_group_last_recover_time
	--当前低级卡组拥有的刷新次数
	t1.cur_low_card_group_refresh_times = self.zhanbuData.cur_low_card_group_refresh_times
	--今日低级卡组翻卡次数
	t1.today_low_card_group_open_card_times = self.zhanbuData.today_low_card_group_open_card_times
	--今日低级卡组已经花费刷新的次数(需要消耗的刷新次数)
	t1.today_low_card_group_cost_refresh_times = self.zhanbuData.today_low_card_group_cost_refresh_times

	if #self.zhanbuData.low_card_group_list <= 0 then
		for i = 1 , 10 do
			table.insert( self.zhanbuData.low_card_group_list , { card_id = 0 , is_already_open = 0} )  
		end
	end
	--低级卡组列表
	t1.low_card_group_list = self.zhanbuData.low_card_group_list
	
	return t1
end

--获得当前土豪卡组信息
function Modules.DivinationModule:GetTyrantCardGroupInfo( )
	if not self.zhanbuData then
		return
	end

	local t1 = {}
	--高级卡组最近刷新时间
	t1.high_card_group_last_refresh_time = self.zhanbuData.high_card_group_last_refresh_time

	if #self.zhanbuData.high_card_group_list <= 0 then
		for i = 1 , 10 do
			table.insert( self.zhanbuData.high_card_group_list , { card_id = 0 , is_already_open = 0} )  
		end
	end

	--高级卡组列表
	t1.high_card_group_list = self.zhanbuData.high_card_group_list

	--今日低级卡组已经花费刷新的次数(需要消耗的刷新次数)
	t1.today_high_card_group_cost_refresh_times = self.zhanbuData.today_high_card_group_cost_refresh_times
	
	return t1
end

--按照类型获得卡组列表
function Modules.DivinationModule:ToTypeGetCardLst( type_ )
	if type_ == Macros.Game.CARD_TYPE.LOW then
		return self:GetLowBCardGroupInfo().low_card_group_list
	elseif type_ == Macros.Game.CARD_TYPE.TALL then
		return self:GetTyrantCardGroupInfo().high_card_group_list
	end
end

--获得我的排名信息
function Modules.DivinationModule:GetMyRankInfo( )
	local info = {}
	if self.RankInfo then
		info.rank = self.RankInfo.rank
		info.rank_value = self.RankInfo.rank_value
	end

	return info
end

--获得排行榜
function Modules.DivinationModule:GetRankLst( )
	local t1 = {}
	if self.RankInfo and self.RankInfo.rank_list then
		for i , v in ipairs( self.RankInfo.rank_list ) do
			table.insert( t1 , v )
			t1[i].rank = i
		end
	end
	return t1

end

--获得排行榜第一
function Modules.DivinationModule:GetRankNO1( )
	local t1 = {}
	if self.RankInfo and self.RankInfo.rank_list then
		for i , v in ipairs( self.RankInfo.rank_list ) do
			t1 = v 
			t1.rank = i
			break
		end
	end
	return t1
end


--获得当前激活的收藏成就
function Modules.DivinationModule:GetNowOpenAchievement( )
	if self.zhanbuData then
		return self.zhanbuData.achievement_level
	end
	return 0
end

--卡牌数据变化处理
function Modules.DivinationModule:UPDataNoticeData( info , for_init )
    if info.flag == 1 then
        local type_ , info_ = ObjIDTool.GetInfo(info.info_id)
        if not info_ then return end

        info.quality = info_.quality
        info.type = info_.type
        info.type2 = info_.type2
        info.name = info_.name
        info.new = info_.new
        info.icon = info_.icon
        info.bigicon = info_.bigicon
        info.new = true

        --更新卡牌
        if self.divinationLst[info.uid] then
        	self.divinationLst[info.uid] = info

        	if not for_init then
				self:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_UPDATE_CARD, info)
        	end

        --增加卡牌
        else
        	self.divinationLst[info.uid] = info

            if not for_init then
                self:AddToNewRecordMap( info.uid )
				self:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_ADD_CARD, info)
            end
        end

    --删除卡牌
    else
        self.divinationLst[info.uid] = nil  

        if not for_init then
	        self:DelFrmNewRecordMap( info.uid )
			self:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_REMOVE_CARD, info)
		end
    end

    --改变卡牌
    if not for_init then
		self:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_CHANGE_CARD, info)
	end
end

--添加到新增记录表
function Modules.DivinationModule:AddToNewRecordMap( id , ignore_save )   
    self.__new_record_map = self.__new_record_map or {}

    local card_data = self:GetDivination(id)
    if card_data and card_data.new then
        self.__new_record_map[id] = self.__new_record_map[id] or {}
        self.__new_record_map[id].type = true

        --保存
        if not ignore_save then
            self:SaveNewRecordMap()
        end
    end
end

--获得新增记录
function Modules.DivinationModule:GetNewRecord( id )  
    if not self.__new_record_map then
        return
    end

    if id then
        return self.__new_record_map[id]
    end
end

--获取新增记录表
function Modules.DivinationModule:GetNewRecordMap()
    return self.__new_record_map
end

--从新增记录表中删除
function Modules.DivinationModule:DelFrmNewRecordMap( id , ignore_save )   
    if not self.__new_record_map then
        return
    end

    if self.__new_record_map[id] then
        self.__new_record_map[id] = nil

        --保存
        if not ignore_save then 
            self:SaveNewRecordMap()
        end
    end    
end

--本地保存
function Modules.DivinationModule:SaveNewRecordMap( )  
    if not self.__new_record_map then
        return
    end

    local text = ""
    for k , v in pairs( self.__new_record_map ) do
        text = text .. k .. "<GH>"
    end
    Cookies.Set( Cookies.LevelType.ROLE , Macros.Setting.NewCard , text )

    GlobalEventSystem:FireNextFrame(Macros.Event.NewModule.PACK_EXTEND , Macros.Game.SystemID.DIVINATION_PACK )
end

--卡牌列表
function Modules.DivinationModule:GetDivinationLit( ... )
	return self.divinationLst
end

--按照uid获得卡牌
function Modules.DivinationModule:GetDivination(uid)
	return self.divinationLst[uid]
end

--按照infoId获得持有数量
-- ID , 去除操作的 , 去除英雄的
function Modules.DivinationModule:ToInfoIdGetDivinationNum( info_id , no_operate , no_hero )
	local index = 0
 	for k , v in pairs( self:GetDivinationLit() ) do
 		if v.info_id == info_id then
 			if no_operate then
 				if v.hero_id == 0 and v.break_level == 0 and v.level <= 1 then
 					index = index + 1
 				end
 			elseif no_hero then
 				if v.hero_id == 0 then
 					index = index + 1
 				end 			
 			else
	 			index = index + 1
	 		end
 		end
 	end
 	return index
end


--获得该类型的装备
function Modules.DivinationModule:GetTypeDivination( type )
	local all = self:GetDivinationLit()
	local t1 = {}
	for i , v in pairs(all) do
		local type_ , info_ = ObjIDTool.GetInfo(v.info_id)
		if info.type == type and v.card_heroid == 0 then
			table.insert(t1,v)
		end
	end
	return t1
end

--背包数据
function Modules.DivinationModule:PackData( UID )
	local lst = self:GetDivinationLit()
	local t1 = {}
	for k , v in pairs( lst ) do
		local info = self:ToInfoIdGetDivinationInfo( v.info_id , v.level , v.break_level )
		if v.uid ~= UID then
			info.uid = v.uid
			table.insert( t1 , info )
		end
	end

	table.sort( t1 , function ( a , b )
		if a.quality > b.quality then
			return a.quality > b.quality
		elseif a.quality == b.quality then
			if a.id < b.id then
				return a.id < b.id
			end
		end
	end )

	return t1
end

--获得英雄总数量和上限
function Modules.DivinationModule:GetDivinationSumAndMax()
    local num = self:GetDivinationLit()
    return CPPGameLib.GetTableLength(num) , CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.DIVINATION_PACK_MAX)
end

--按照配置ID获取信息
function Modules.DivinationModule:ToInfoIdGetDivinationInfo( id , level_ , break_ )
	local info = CPPGameLib.GetConfig("Card", id )
	if not info then return end
	level_ = level_ or 1
	break_ = break_ or 0 

	--基础属性
	local t1 = {}
	for i , v in ipairs( info.base_pro_type_list ) do
		table.insert( t1 , { type = v , value = info.base_pro_value_list[i] } )
	end
	table.sort( t1 , function ( a , b )
		return a.type < b.type
	end )

	--升级属性
	local t2 = {}
	for i , v in ipairs( info.level_up_pro_type_list ) do
		table.insert( t2 , { type = v , value = info.level_up_pro_value_list[i] * (level_ - 1)  } )
	end

	local t6 = {}
	for k , v in pairs( t1 ) do
		if not t6[v.type] then
			t6[v.type] = v.value
		end	
	end
	for k , v in pairs( t2 ) do
		if not t6[v.type] then
			t6[v.type] = v.value
		else
			t6[v.type] = t6[v.type] + v.value
		end	
	end
	local t8 = {}
	for k , v in pairs( t6 ) do
		table.insert( t8 , { type = k , value = v } )
	end

	--突破属性
	local t3 = {}
	for i , v in ipairs( info.break_pro_type_list ) do
		table.insert( t3 , { type = v , value = info.break_pro_value_list[i] * break_ } )
	end

	local t7 = {}
	for k , v in pairs( t1 ) do
		if not t7[v.type] then
			t7[v.type] = v.value
		end	
	end
	for k , v in pairs( t3 ) do
		if not t7[v.type] then
			t7[v.type] = v.value
		else
			t7[v.type] = t7[v.type] + v.value
		end	
	end
	local t9 = {}
	for k , v in pairs( t7 ) do
		table.insert( t9 , { type = k , value = v } )
	end


	--全属性整合
	local t4 = {}
	for k , v in pairs( t1 ) do
		if not t4[v.type] then
			t4[v.type] = v.value
		end	
	end

	for k , v in pairs( t2 ) do
		if not t4[v.type] then
			t4[v.type] = v.value
		else
			t4[v.type] = t4[v.type] + v.value
		end	
	end

	for k , v in pairs( t3 ) do
		if not t4[v.type] then
			t4[v.type] = v.value
		else
			t4[v.type] = t4[v.type] + v.value
		end	
	end

	local t5 = {}
	for k , v in pairs(t4) do
		table.insert( t5 , { type = k , value = v } )
	end
	table.sort( t5 , function ( a , b )
		return a.type < b.type
	end )

	return { 
				id = id , 
				name = info.name , 
				type = info.type , 
				type2 = info.type2 , 
				quality = info.quality , 
				to_exp = info.to_exp ,
				base_pro = t1 ,  
				up_pro = t2 ,  
				break_pro = t3 ,  
				property = t5 ,

				up_proEx = t8,
				break_proEx = t9,

			}
end

--用类型取得他最弱的装备的id
function Modules.DivinationModule:ToTypeGetWeakestId( item_type )
    local t1 = {}
    for k,v in pairs(CPPGameLib.GetConfig("Card", nil, nil , true )) do
        if v.type == item_type and v.quality >= Macros.Game.QualityType.GREEN then
            table.insert( t1 , { id = k })
        end
    end 

    if #t1 ~= 0 then
        table.sort( t1, function ( a , b )
            return a.id < b.id
        end )
        return t1[1].id
    end
end

--获取某卡牌增加若干经验后，可以升多少级
function Modules.DivinationModule:ToExpGetUpLevel( uid , exp )
	local info = self:GetDivination(uid)
	local cnf = self:ToInfoIdGetDivinationInfo( info.info_id )
	--按品质取得信息
	local cardCnf = CPPGameLib.GetConfig("CardLevelUp", info.quality )
	
	local allExp
	local maxInfo = self:GetDivinationUpMAX( uid )
	local endLevel = info.level
	if exp > 0 then
		allExp = info.exp + exp
		for i = info.level , maxInfo.level do
			local levelExp = cardCnf.level_list[ i ].exp
			if allExp - levelExp < 0 then
				break
			else
				allExp = allExp - levelExp
				endLevel = i + 1
			end
		end
		endLevel = math.min(endLevel, maxInfo.level)
	end

	return endLevel - info.level, maxInfo.level
end

--通过卡牌类型和品质获取对应的卡牌列表
function Modules.DivinationModule:GetDivinationListByCardTypeAndQuality(card_type,quality)
	if not card_type or not quality then
		return
	end
	--特殊卡牌列表
	if not self.__sp_divination_list then
		self.__sp_divination_list = {}
		for _,card_info in pairs(CPPGameLib.GetConfig("Card",nil,nil,true)) do
			self.__sp_divination_list[card_info.type] = self.__sp_divination_list[card_info.type] or {}
			self.__sp_divination_list[card_info.type][card_info.quality] = self.__sp_divination_list[card_info.type][card_info.quality] or {}
			table.insert(self.__sp_divination_list[card_info.type][card_info.quality],card_info.icon)
		end
	end
	return self.__sp_divination_list[card_type][quality] and self.__sp_divination_list[card_type][quality][1]
end

--材料整理
function Modules.DivinationModule:ToExpGetDivinationUpEXP( needEXP , quality )

	local lst = CPPGameLib.GetConfig("Card", nil , nil , true )

	local qualityLst = {}
	quality = quality or { }
	for i , v in ipairs( quality ) do
		qualityLst[v] = true
	end

	--遍历身上所需品质的卡片
	local t1 = {}
	for i , v in pairs( lst ) do
		local cnf = self:ToInfoIdGetDivinationInfo( i )
		if qualityLst[cnf.quality] then
			table.insert( t1 , { uid = i , infoid = i , quality = cnf.quality , to_exp = cnf.to_exp } )
		end
	end

	table.sort( t1 , function( a , b )
  		if a.quality < b.quality then
  			return a.quality < b.quality
  		elseif a.quality == b.quality then
  			return a.infoid < b.infoid
  		end
	end )

	local exp = 0
	local t2 = {}
	for i,v in ipairs( t1 ) do
		if exp < needEXP then
			exp = exp + v.to_exp
			table.insert( t2 , v )
		end
	end
end

--获得比身上强的
function Modules.DivinationModule:ToUidGetStronger( uid )
	local cnf = self:ToInfoIdGetDivinationInfo( uid )
	local lst = CPPGameLib.GetConfig("Card", nil , nil , true )
	local t1 = {}
	for i , v in pairs(lst) do
		if v.type == cnf.type and v.quality < cnf.quality then
			table.insert( t1 , v )
			break
		end
	end
	return t1
end

--商店整合
function Modules.DivinationModule:GetDivinationDestinyShopLst( index )
	local lst = CPPGameLib.GetConfig("DestinyShop", nil , nil , true )
	local t1 = {}
	for k , v in pairs( lst ) do
		if v.type == index then
			table.insert( t1 , v )
			t1[#t1].id = k
		end
	end
	table.sort( t1 , function ( a , b )
		return a.id < b.id
	end )
	return t1
end

--根据uid获得宿命
function Modules.DivinationModule:ToUidGetFate( uid )
	local info = self:GetDivination( uid )
	
	local t1 = {}
	local cnf = CPPGameLib.GetConfig("Hero", nil , nil , true )
	for k , v in pairs( cnf ) do
		if v.relation_card_list then
			for ii , vv in ipairs( v.relation_card_list ) do

				for iii , vvv in ipairs( vv ) do
					if vvv == info.type then
						local name = v.relation_card_reinforcementname_list[1]
						if v.relation_card_reinforcementname_list[i] then
							name = v.relation_card_reinforcementname_list[i]
						end

						local content = v.relation_card_reinforcementdes_list[1]
						if v.relation_card_reinforcementdes_list[i] then
							content = v.relation_card_reinforcementdes_list[i]
						end
						table.insert( t1 , { id = k , name = v.name , describeName = name , describeContent = content } )
					end
				end

			end

		end

	end

	return t1
end

--卡牌排序
function Modules.DivinationModule:GetDivinationSort( ... )
    local t1 = {}
    local t2 = {}

    for k,v in pairs( self:GetDivinationLit() ) do
        if v.hero_id ~= 0 then
            table.insert(t1,v)
        else
            table.insert(t2,v)
        end
    end

    function OnSort( a , b )
        if a.quality > b.quality then
            return a.quality > b.quality
        elseif a.quality == b.quality then
            if a.level > b.level then
                return a.level > b.level
            elseif  a.level == b.level then
                if a.break_level > b.break_level then
                    return a.break_level > b.break_level
                elseif a.break_level == b.break_level then
                    return a.info_id < b.info_id
                end
            end
        end
    end
    table.sort( t1, OnSort )
    table.sort( t2, OnSort )
    for i,v in ipairs(t2) do
        table.insert(t1,v)
    end

    return t1
end

--设置选中的卡牌
function Modules.DivinationModule:SetOnDivinationUid( uid )
	self.onDivinationUid = uid
end

--获得选中的卡牌
function Modules.DivinationModule:GetOnDivinationUid(  )
	return self.onDivinationUid
end

--设置移动状态
function Modules.DivinationModule:SetShowMoveOpen( var )
	self.showMove = var
end

--获得移动状态
function Modules.DivinationModule:GetShowMoveOpen()
	return self.showMove
end

--获得所有未穿戴卡牌(可被作为材料使用的列表
function Modules.DivinationModule:GetAllDivinationWithoutWear( type_ )
	--未穿戴
	local t1 = {}
	--穿戴
	local t2 = {}

	for k , v in pairs( self.divinationLst ) do
		if type_ and v.type == type_ then
			if v.hero_id ~= 0 then
				table.insert( t2 , v )
			else
				table.insert( t1 , v )
			end
		else
			if v.hero_id ~= 0 then
				table.insert( t2 , v )
			else
				table.insert( t1 , v )
			end
		end
	end
	return t1 , t2
end

--次数恢复倒计时
function Modules.DivinationModule:Countdown(  )
	--低级卡组最近的恢复时间
	local lowInfo = self:GetLowBCardGroupInfo()
	if lowInfo then
		local lowRenewTime = lowInfo.low_card_group_last_recover_time
		local endTime = lowRenewTime + ( ConfigAdapter.Common.GetLowBDivinationCD() * 60 )

		if self.lowRenewTime then
			GlobalTimerQuest:CancelQuest( self.lowRenewTime )
			self.lowRenewTime = nil
		end

		local data = self:GetLowBCardGroupInfo()
		if data.cur_low_card_group_refresh_times == ConfigAdapter.Common.GetLowBDivinationFreeMax() then
			return
		end

		self.lowRenewTime = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			if endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") <= 0 then
				self:LowCardGroupRefreshTimesUpdateReq()
			else
				if not self.lowCountdownTime then
					self.LowCountdownTime = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
				end
				GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_LOW_RENEW)
			end
		end),1 ,-1)
	end
end
--次数恢复倒计时
function Modules.DivinationModule:GetLowCountdownTime(  )
	return self.LowCountdownTime
end

--高级次数恢复倒计时
function Modules.DivinationModule:HighCountdown(  )
	--低级卡组最近的恢复时间
	local lowInfo = self:GetTyrantCardGroupInfo()
	if lowInfo then
		local highRenewTime = lowInfo.high_card_group_last_refresh_time

		local endTime = highRenewTime + ( ConfigAdapter.Common.GetTyrantDivinationCD() * 60 )

		if self.highRenewTime then
			GlobalTimerQuest:CancelQuest( self.highRenewTime )
			self.highRenewTime = nil
		end

		self.highRenewTime = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
			if endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") <= 0 then
				self.highCountdownNum = 1
				GlobalEventSystem:FireNextFrame( Macros.Event.DivinationModule.DIVINATION_HIGH_RENEW )
				if self.highRenewTime then
					GlobalTimerQuest:CancelQuest( self.highRenewTime )
					self.highRenewTime = nil
				end
			else
				self.highCountdownNum = 0
			end
			
			self.highCountdownTime = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

			GlobalEventSystem:FireNextFrame( Macros.Event.DivinationModule.DIVINATION_HIGH_RENEW )
		end),1 ,-1)
	end
end

--次数
function Modules.DivinationModule:GetHighCountdownTime(  )
	return self.highCountdownNum , self.highCountdownTime
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--占卜
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明: 翻卡请求 消息Id: 23113
function Modules.DivinationModule:OpenCardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_OPEN_CARD_REQ)
	data.type = info.type
	data.card_idx_list = info.card_idx_list
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 翻卡响应 消息Id: 23114
function Modules.DivinationModule:OpenCardResp( info )
	local t1 = {}
	for i , v in ipairs( info.card_idx_list ) do
		v.card_idx = v.card_idx
		t1[ v.card_idx ] = v
	end

	--LowB
	if info.type == Macros.Game.CARD_TYPE.LOW then
		for i , v in ipairs( self.zhanbuData.low_card_group_list ) do
			if t1[ i ] then
				v.card_id = t1[ i ].card_id
				v.is_already_open = 1
			end
		end
		self.zhanbuData.today_low_card_group_open_card_times = self.zhanbuData.today_low_card_group_open_card_times + #info.card_idx_list
	--high
	elseif info.type == Macros.Game.CARD_TYPE.TALL then
		for i , v in ipairs( self.zhanbuData.high_card_group_list ) do
			if t1[ i ] then
				v.card_id = t1[ i ].card_id
				v.is_already_open = 1
			end
		end
	end
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_DRAW , info )
end

--翻牌价格计算
--翻卡类型 , 翻卡数量
function Modules.DivinationModule:DrawCalculate( Cardtype , index )
	print("xy" , "---------index" , index)
	--背包上限判断
	local packType = true
	GlobalModulesMgr:Call( ModuleType.SUNDRY , "PackVacancy", Macros.Game.ObjType.FAIRY , 
                            index , true , 
                            function ( sum , text )
                            	if sum - index <= 0 then
                            		packType = false
                            	end
                            end 
                        )
	--价钱判断
	--需要类型
    local needType_ = nil
    --需要值
    local needvalue = nil
    if Cardtype == Macros.Game.CARD_TYPE.LOW then
        needType_ = Macros.Game.RolePropType.GOLD
        needvalue = ConfigAdapter.Common.GetLowBDivinationDraw()
        local card = self:GetLowBCardGroupInfo()
        --今日翻牌的次数
        local open_times = card.today_low_card_group_open_card_times
        --剩余免费翻牌次数
        local free_times = math.max(0, ConfigAdapter.Common.GetLowBDivinationFree() - open_times)
        --免费翻牌次数大于待翻牌次数
        if free_times >= index then
            needvalue = 0
        --免费翻牌次数小于待翻牌次数
        else
            needvalue = needvalue * ( index - free_times )
        end
    elseif Cardtype == Macros.Game.CARD_TYPE.TALL then
        needType_ = ConfigAdapter.Common.GetTyrantDivinationDraw()
        needvalue = math.max( 1 , index )
    end

    local consumeType = true
	local hold = ObjIDTool.GetOwnNum( needType_ )
	if hold < needvalue then
		consumeType = false
	end

	if packType == true and consumeType == true then
		return true , consumeType , needType_
	else
		return false , consumeType , needType_
	end
end

--洗牌计算
function Modules.DivinationModule:ShuffleCalculate( Cardtype )
	local consumeType = nil
	local consumeValue = nil
	local free = 0

    if Cardtype == Macros.Game.CARD_TYPE.LOW then
        consumeType = Macros.Game.RolePropType.DIAMOND

        local data = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
        free = data.cur_low_card_group_refresh_times

        local valueLst = ConfigAdapter.Common.GetLowBDivinationExpend()
        local num = data.today_low_card_group_cost_refresh_times

        if num and num ~= 0 and valueLst[num + 1] then
            consumeValue = valueLst[num + 1]
        elseif num and num ~= 0 and not valueLst[num + 1] then
            consumeValue = valueLst[#valueLst]
        else
            consumeValue = valueLst[1]
        end
    elseif Cardtype == Macros.Game.CARD_TYPE.TALL then
        consumeType = Macros.Game.RolePropType.DIAMOND

        local valueLst = ConfigAdapter.Common.GetTyrantDivinationExpend()

        local data = CallModuleFunc(ModuleType.DIVINATION, "GetTyrantCardGroupInfo" )
        local time = nil
        free , time = CallModuleFunc(ModuleType.DIVINATION, "GetHighCountdownTime" )

        local num = data.today_high_card_group_cost_refresh_times

        if num and  num ~= 0 and valueLst[num + 1] then
            consumeValue = valueLst[num + 1]
        elseif num and num ~= 0 and not valueLst[num + 1] then
            consumeValue = valueLst[#valueLst]
        else
            consumeValue = valueLst[1]
        end
    end

    --免费次数 , 货币类型 , 货币值
    return free , consumeType , consumeValue
end

--获得当前还没抽的卡牌的总消耗{屌丝}
function Modules.DivinationModule:GetDrawAllLowConsume( index )
	local groupInfo = self:GetLowBCardGroupInfo()

	index = index or 0
	local num = 0 - index
	for i , v in ipairs( groupInfo.low_card_group_list ) do
		if v.card_id == 0 then
			num = num + 1
		end
	end

	if groupInfo.today_low_card_group_open_card_times <= ConfigAdapter.Common.GetLowBDivinationFree() then
		local shengyu = ConfigAdapter.Common.GetLowBDivinationFree() - groupInfo.today_low_card_group_open_card_times
		num = num - shengyu
	end
	return num * ConfigAdapter.Common.GetLowBDivinationDraw()
end

--获得当前还没抽的卡牌的总消耗{高级}
function Modules.DivinationModule:GetDrawAllTallConsume( index )
	local groupInfo = self:GetTyrantCardGroupInfo()

	index = index or 0
	local num = 0 - index
	for i , v in ipairs( groupInfo.high_card_group_list ) do
		if v.card_id == 0 then
			num = num + 1
		end
	end

	return num
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--提升信息
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明: 卸下卡牌请求 消息Id: 23128
function Modules.DivinationModule:CollectionUnloadCardReq( info )
	--阵容飘字
	--需要立即触发
	CallModuleFunc(ModuleType.HERO,"SaveHeroPropertyBeforeChange")
	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_COLLECTION_UNLOAD_CARD_REQ)
	data.formation_pos = info.formation_pos
	data.card_slot = info.card_slot
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 卸下卡牌响应 消息Id: 23129
function Modules.DivinationModule:CollectionUnloadCardResp( info )
	--阵容飘字
	CallModuleFunc(ModuleType.HERO,"SetIsHaveChangePropertyNeedWindWords",true)
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_DISCHARGE)
end

--按uid获得比当前卡牌强的卡牌
function Modules.DivinationModule:ToUidGetStrongCard( uid )
	local now = self:GetDivination( uid )
	local lst = self:GetDivinationLit( )
	local t1 = {}
	for i , v in pairs( lst ) do
		if v.type == now.type then
			if v.level > now.level or 
				v.break_level > now.break_level or 
				v.quality > now.quality then
				table.insert( t1 , v )
			end
		end
	end
	return t1
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--升级
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明: 卡牌升级请求 消息Id: 23107
-- "uid__I",   --升级的卡牌UID
-- "cost_card_list__T__id@I", -- 作为消耗的卡牌列表
function Modules.DivinationModule:CardLevelUpReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CARD_LEVEL_UP_REQ)
	data.uid = info.uid
	data.cost_card_list = info.cost_card_list
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 卡牌升级响应 消息Id: 23108
function Modules.DivinationModule:CardLevelUpResp( info )
	self:SetSelectedStrengthenMaterial()
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_UP)
end

--选中材料
function Modules.DivinationModule:SetSelectedStrengthenMaterial( uid , var )
	if uid then
		self.dogFood[ uid ] = var
	else
		self.dogFood = {}
	end
end

--选中材料
function Modules.DivinationModule:GetOnDogFood( uid )
	if uid then
		return self.dogFood[ uid ]
	else
		return self.dogFood
	end
end

--获得这个东西能提供多少经验
function Modules.DivinationModule:GetOneDogFoodExp( uid )
	local info = self:GetDivination( uid )
	local exp = info.exp
	local cnf = CPPGameLib.GetConfig("CardLevelUp", info.quality )
	local data = cnf.level_list[ info.level ]
	if data then
		exp = exp + data.total_exp
	end
	local cnf = CPPGameLib.GetConfig("Card", info.info_id )
	if cnf then
		exp = exp + cnf.to_exp
	end
	return exp
end

--获取某卡牌可升级到的最高等级、下一等级、当前等级信息
function Modules.DivinationModule:GetDivinationUpMAX( uid )
	local info = self:GetDivination( uid )

	--按品质取得信息
	local cnf = CPPGameLib.GetConfig("CardLevelUp", info.quality )

	table.sort( cnf.level_list , function ( a , b )
		return a.level < b.level
	end )

	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	--寻找到最高等级
	local max_info = {}
	for i , v in ipairs( cnf.level_list ) do
		max_info = v
		if v.need_role_level > roleLevel then
			break
		end
	end

	--寻找到下一等级
	local info = self:GetDivination( uid )
	local next_info = {}
	for i , v in ipairs( cnf.level_list ) do
		if v.level > info.level then
			next_info = v
			break
		end
	end

	--寻找到当前等级
	local info = self:GetDivination( uid )
	local now_info = {}
	for i = 0 , #cnf.level_list do
		if cnf.level_list[i] then
			if cnf.level_list[i].level == info.level then
				now_info = cnf.level_list[i]
				break
			end
		end
	end

	--最高等级信息、下一等级信息、当前等级信息
	return max_info , next_info , now_info
end

--按照uid判断装备是否能强化
function Modules.DivinationModule:ToUidGetCardIsStrengthen( uid )

    local info = self:GetDivination(uid)

    --目前最高升到这个等级
    local level = self:GetDivinationUpMAX( uid ).level

	--当前需要的经验
	local _ , __ , now_info = self:GetDivinationUpMAX( uid )
	local exp = now_info.exp or 0
	--材料列表
	local dogFoodLst = self:GetDogFoodLst()
	local dogFoodExp = info.exp
	for i , v in ipairs( dogFoodLst ) do
		if v.quality < Macros.Game.QualityType.PURPLE then
			dogFoodExp = dogFoodExp + self:GetOneDogFoodExp( v.uid )
		end
	end

    if dogFoodExp >= exp and info.level < level then
        return "true"
    else
        return "false"
    end
end

--可被作为材料使用的列表
function Modules.DivinationModule:GetDogFoodLst( uid )
	local t1 = self:GetAllDivinationWithoutWear()

	local material_type = GlobalCustomizedLogicAdapterMgr:Call("GetMaterialTypeOfCardStrengthen")

	local t2 = {}
	for i, v in ipairs( t1 ) do
		if not material_type or material_type == v.type2 then
			table.insert( t2 , v )
		end
	end

	table.sort( t2 , function( a , b )
		--经验卡牌优先
		if a.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP and b.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP then
			if a.quality ~= b.quality then
				return a.quality < b.quality
			else
		 		return a.info_id < b.info_id
		 	end
		elseif a.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP and b.type2 ~= Macros.Game.CARD_QUALITY_TYPE.EXP then
			return true
		elseif a.type2 ~= Macros.Game.CARD_QUALITY_TYPE.EXP and b.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP then
			return false
		else	
			if a.quality ~= b.quality then
				return a.quality < b.quality
			else
		 		return a.info_id < b.info_id
		 	end
	 	end
	end )	

	local t3 = {}
	for i , v in ipairs( t2 ) do
		if v.uid ~= uid then
			table.insert( t3 , v )
		end
	end

	return t3
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--突破
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明: 卡牌突破请求 消息Id: 23109
-- "card_uid__I",  --卡牌UID
function Modules.DivinationModule:CardBreakReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CARD_BREAK_REQ)
	data.card_uid = info
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 卡牌突破响应 消息Id: 23110
-- "card_uid__I",  --卡牌UID
function Modules.DivinationModule:CardBreakResp( info )
	GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_BREAK)
end

--当前突破最高能达到什么程度
function Modules.DivinationModule:GetDivinationBreakMAX( uid )
	local info = self:GetDivination( uid )
	--按品质取得信息
	local cnf = CPPGameLib.GetConfig("CardBreak", info.quality )

	table.sort( cnf.break_level_list , function ( a , b )
		return a.break_level < b.break_level
	end )

	local info = self:GetDivination( uid )
	--寻找到下一等级
	local next_info = {}
	for i = 0 , 999 do
		if cnf.break_level_list[i] then
			v = cnf.break_level_list[i]
			if v.break_level > info.break_level then
				next_info = v
				break
			end
		end
	end

	--寻找到最高等级
	local max_info = {}
	for i = 100 , 0 do
		if cnf.break_level_list[i] then
			v = cnf.break_level_list[i]
			if v.need_card_level <= info.level then
				max_info = v
				break
			end
		end
	end

	return next_info , max_info 
end

--根据uid获得卡牌能否突破
function Modules.DivinationModule:ToUidGetCardIsBreak( uid )

    local info = self:GetDivination(uid)

    --目前最高升到这个等级
    local _ , break_level = self:GetDivinationBreakMAX( uid ).break_level

    local cnf = CPPGameLib.GetConfig("CardBreak", info.quality )
    cnf = cnf.break_level_list[ info.break_level ]
    need_card_level = cnf.need_card_level

    --卡牌等级需求
    local levelType = true
    if need_card_level > info.level then
    	levelType = false
    end

    --突破消耗货币
    local moneyType = true
    for i , v in ipairs( cnf.cost_type_list ) do
    	local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , v )
    	if num < cnf.cost_value_list[i] then
    		moneyType = false
    		break
    	end
    end

    --突破消耗物品
    local itemType = true
    for i , v in ipairs( cnf.cost_item_id_list ) do
    	local num = ObjIDTool.GetOwnNum( v )
    	if num < cnf.cost_item_num_list[i] then
    		moneyType = false
    		break
    	end
    end

    --突破消耗自己
    local oneselfType = true
	if cnf.cost_same_card_num and cnf.cost_same_card_num ~= 0 then
		--数量
		local index = self:ToInfoIdGetDivinationNum( info.info_id , nil , true )
		if index - 1 <= 0 then
			oneselfType = false
		end
	end

    if levelType == true and moneyType == true and itemType == true and oneselfType == true and need_card_level ~= 0 then
        return "true"
    else
        return "false"
    end	
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--收藏室
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--获得这个书
function Modules.DivinationModule:GetBookLst( book_id )
	if self.bookList and self.bookList[book_id] then
		return self.bookList[book_id]
	else
		return {}
	end
end

--获得获得收藏书表
function Modules.DivinationModule:GetCollectionBookCnf( id )
	local cnf = CPPGameLib.GetConfig("CollectionBook", nil , nil , true )

	local t1 = {}
	for i , v in pairs( cnf ) do
		table.insert( t1 , v )
		t1[#t1].id = i
		t1[#t1].itemType = "shoucang"
	end

	table.sort( t1 , function( a , b )
		return a.id < b.id 
	end )

	if id then
		local t2 = {}
		for i , v in ipairs( t1 ) do
			if v.id == id then
				t2 = v
				break
			end
		end

		local t3 = {}
		local book = self:GetBookLst( id )
		for i , v in ipairs( book ) do
			t3[ v.id ] = v.star
		end

		local t4 = {}
		for _ , v in ipairs( t2.collection_group_list ) do
			local sort = 0 
			local type_ = self:ToGroupIdGetUpStarType( v )

			--激活过的
			if t3[v] then
				local max = self:GetCollectionGroupCnf( v ).max_star
				--满星了
				if t3[ v ] >= max then
					sort = 5
				--没满星
				else
					local starType = self:ToGroupIdGetUpStarType( v )
					--能升星
					if starType == true then
						sort = 2
					--不能升星
					else
						sort = 3
					end
				end
			--没激活过
			else
				local starType = self:ToGroupIdGetUpStarType( v )
				--能激活
				if starType == true then
					sort = 1
				--不能激活
				else
					sort = 4
				end
			end
			table.insert( t4 , { id = v , sort = sort } )
		end

		table.sort( t4 , function ( a , b )
			if a.sort < b.sort then
				return a.sort < b.sort
			elseif a.sort == b.sort then
				if a.id < b.id then
					return a.id < b.id
				end
			end
		end )
		t1 = t2
		t1.lst = t4
	end

	return t1
end

--获得获得收藏书表
function Modules.DivinationModule:GetCollectionGroupCnf( id )
	local cnf = CPPGameLib.GetConfig("CollectionGroup", id , false )
	return cnf
end

--按照卡组ID获得属性
function Modules.DivinationModule:ToGroupIdGetProLst( book_id , id , star )
	local cnf = self:GetCollectionGroupCnf( id )	
	star = star or 1
	if book_id then
		star = self:ToBookAndIdGetStar( book_id , id )
	end
	local t1 = {}
	if cnf then
		for i , v in ipairs( cnf.activate_pro_type_list ) do
			table.insert( t1 , { type = v , value = cnf.activate_pro_value_list[i] + ( math.max( 0 , (star - 1) ) * cnf.upgrade_pro_value_list[i] ) } )
		end
	end
	return t1
end

--按照卡组ID获得能否升星状态
function Modules.DivinationModule:ToGroupIdGetUpStarType( id )
	local cnf = self:GetCollectionGroupCnf( id )
	if not cnf then return end
	for i , v in ipairs( cnf.cost_card_list ) do
		local num = self:ToInfoIdGetDivinationNum( v , true )
		if num <= 0 then
			return false
		end
	end
	return true
end

--获得收集项升星信息
function Modules.DivinationModule:GetGatherUpgradeInfo( id )
	local cnf = self:GetCollectionGroupCnf( id )

	local t1 = {}
	local collection = cnf.activate_collection_value
	for i = 1 , cnf.max_star do
		local t2 = {}
		for ii , vv in ipairs( cnf.activate_pro_type_list ) do
			table.insert( t2 , { type = vv , value = cnf.activate_pro_value_list[ii] + ( cnf.upgrade_pro_value_list[ii] * i ) } )  
		end
		if i ~= 1 then
			collection = collection + cnf.upgrade_collection_value
		end
		table.insert( t1 , { star = i , pro = t2 , name = cnf.name , collectionValue = collection } )
	end

	return t1
end

--获得卡组关卡
function Modules.DivinationModule:GetCardGroupMap( id )
	local cnf = CPPGameLib.GetConfig("CardGroupMap", id )
	return cnf
end

--获得卡组信息
function Modules.DivinationModule:GetCollectionInfo( id )
	local cnf = CPPGameLib.GetConfig("CollectionInfo", id )
	return cnf
end

--获得当前卡组信息
function Modules.DivinationModule:GetNowCollectionInfo( id )
    local index = self:GetLowCardGroupMapIdx()
    local cnf = self:GetCardGroupMap(index)
    local info = self:GetCollectionInfo( id or cnf.card_group_id)
    return info
end

--获得占卜属性加成
function Modules.DivinationModule:GetPlusPropMap(card_list)
	local plus_prop_map = {}

	--卡牌属性加成
	if card_list then
		local card_data, config_card
		for _, card_info in ipairs(card_list) do
			card_data = self:GetDivination(card_info.card_id)
			config_card = CPPGameLib.GetConfig("Card", card_data and card_data.info_id or 0, false)
			if card_data and config_card then
				--基础属性
				if config_card.base_pro_type_list then
					for i, pro_type in ipairs(config_card.base_pro_type_list) do
						-- print("skfb", "基础属性 ", pro_type, config_card.base_pro_value_list[i])
						Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, config_card.base_pro_value_list[i])
					end
				end

				--升级属性
				if config_card.level_up_pro_type_list then
					for i, pro_type in ipairs(config_card.level_up_pro_type_list) do
						-- print("skfb", "升级属性 ", pro_type, (card_data.level - 1) * config_card.level_up_pro_value_list[i])
						Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, (card_data.level - 1) * config_card.level_up_pro_value_list[i])
					end					
				end

				--突破属性
				if config_card.break_pro_type_list then
					for i, pro_type in ipairs(config_card.break_pro_type_list) do
						-- print("skfb", "突破属性 ", pro_type, card_data.break_level * config_card.break_pro_value_list[i])
						Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, card_data.break_level * config_card.break_pro_value_list[i])
					end					
				end
			end
		end
	end

	--成就属性加成
	local achievement_level = self:GetNowOpenAchievement()
	local config_achievement = CPPGameLib.GetConfig("ZhanBuAchievement", achievement_level, false)
	if config_achievement and config_achievement.pro_type_list then
		for i, pro_type in ipairs(config_achievement.pro_type_list) do
			-- print("skfb", "成就属性加成 ", pro_type, config_achievement.pro_value_list[i])
			Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, config_achievement.pro_value_list[i])
		end
	end
	
	--收藏室属性加成
	if self.bookList then
		local config_collect_group, group_star
		for book_id, collect_group_list in pairs(self.bookList) do
			for _, group_info in ipairs(collect_group_list) do
				group_star = group_info.star
				if group_star > 0 then
					config_collect_group = CPPGameLib.GetConfig("CollectionGroup", group_info.id)
					if config_collect_group then
						for i, pro_type in ipairs(config_collect_group.activate_pro_type_list) do
							-- print("skfb", "收藏室属性加成 ", pro_type, config_collect_group.activate_pro_value_list[i] + (group_star - 1) * config_collect_group.upgrade_pro_value_list[i])
							Modules.BattleFormula:AddPlusPropMap(plus_prop_map, pro_type, config_collect_group.activate_pro_value_list[i] + (group_star - 1) * config_collect_group.upgrade_pro_value_list[i])
						end												
					end
				end				
			end
		end
	end

	return plus_prop_map
end

--现在收藏卡牌缩提供的属性
function Modules.DivinationModule:GetNowCollectProLst( )
	if not self.bookList then
		return {}
	end

	local t1 = {}
	for _ , v in pairs( self.bookList ) do
		for ii , vv in ipairs( v ) do
			local lst = self:ToGroupIdGetProLst( nil , vv.id , vv.star )
			for iii , vvv in ipairs( lst ) do
				if not t1[vvv.type] then
					t1[vvv.type] = 0
				end
				t1[vvv.type] = t1[vvv.type] + vvv.value
			end
		end
	end

	return t1
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--成就
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--获得成就表
function Modules.DivinationModule:GetAchievementCnf( id )
	local cnf = CPPGameLib.GetConfig("ZhanBuAchievement", nil , nil , true )

	local t1 = {}
	for i , v in pairs( cnf ) do
		table.insert( t1 , v )
		t1[#t1].id = i
		t1[#t1].itemType = "chengjiu"
	end

	table.sort( t1 , function( a , b )
		return a.id < b.id 
	end )

	if id then
		local t2 = {}
		for i , v in ipairs( t1 ) do
			if v.id == id then
				t2 = v
				break
			end
		end
		t1 = t2
	end

	return t1
end

--获得成就属性
function Modules.DivinationModule:GetAchievementPro( )
	local t1 = {}
	local level = self:GetNowOpenAchievement()
	if level <= 0 then
		return t1
	end

	local cnf = self:GetAchievementCnf()
	for _ , v in ipairs( cnf ) do
		if v.level == level then
			for i , vv in ipairs( v.pro_type_list ) do
				if not t1[vv] then
					t1[vv] = 0
				end
				t1[vv] = t1[vv] + v.pro_value_list[i] 
			end
		end
	end
	return t1
end

---------------------------------------------------------------------------------------------------------------------------
--杂项
---------------------------------------------------------------------------------------------------------------------------
function Modules.DivinationModule:SetIscombat( var )
	self.IscombatReturn = var
end

function Modules.DivinationModule:GetIscombat( info )
	return self.IscombatReturn
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.DivinationModule:CheckBattleCondition( type_ )
	local mapIdx = self:GetLowCardGroupMapIdx( )
	local info = self:GetCardGroupMap( mapIdx )
	if info.is_the_last ~= 1 then
		return true , info.map_id
	end
end