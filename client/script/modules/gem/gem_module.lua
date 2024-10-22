--[[
%% @module: 宝石
%% @author: yjg
%% @created: 2017年9月20日16:43:36
--]]

--关于宝石穿上卸下应该是可以在本地控制才对

Modules = Modules or {}

Modules.GemModule = Modules.GemModule or BaseClass(BaseModule)

function Modules.GemModule:__init()
end

function Modules.GemModule:__delete()
end

-- 消息说明:宝石列表请求 消息Id: 25301
function Modules.GemModule:GemListReq( info )
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石列表请求 消息Id: 25301") 

    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GEM_LIST_REQ)
    GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:宝石列表响应 消息Id: 25302
function Modules.GemModule:GemListResp( info )
-- "gem_list__T__id@I##num@I", -- 宝石列表
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石列表响应 消息Id: 25302") 
	-- "gem_hole_open_list__T__hole@I", -- 孔位开启列表

	if not self.userPack then
		self.userPack = {}
	end
	for i,v in ipairs( info.gem_list ) do
        local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , v.id )
        local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , gemCnf.type )
		self.userPack[v.id] = { id = v.id , num = v.num , max = inlayCnf.inlay_num_limit , type = gemCnf.type  }
	end

	if not self.openList then
		self.openList = {}
	end
	for i,v in ipairs( info.gem_hole_open_list ) do
		self.openList[i] = v.hole
	end
end

-- 消息说明:宝石合成请求 消息Id: 25303
function Modules.GemModule:GemComposeReq( info )
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石合成请求 消息Id: 25303")

    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GEM_COMPOSE_REQ)
    protocol.gem_num = info.gem_num
    protocol.gem_id = info.gem_id
    GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:宝石合成响应 消息Id: 25304
function Modules.GemModule:GemComposeResp( info )
-- "gem_num__H",   --要合成的宝石数量
-- "gem_id__I",    --宝石Id
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石合成响应 消息Id: 25304")

	local gemCnf = self:GetGemCnf( info.gem_id )
	local t1 = {}
    table.insert(t1,{id = gemCnf.new_gem_from_compose , num = info.gem_num })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	GlobalEventSystem:FireNextFrame(Macros.Event.GemModule.COMPOUND)
end

-- 消息说明:宝石镶嵌请求 消息Id: 25305
function Modules.GemModule:GemInlayReq( info )
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石镶嵌请求 消息Id: 25305")

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GEM_INLAY_REQ)
	protocol.formation_slot = info.formation_slot - 1 
	protocol.gem_inlay_list = info.gem_inlay_list
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:宝石镶嵌响应 消息Id: 25306
function Modules.GemModule:GemInlayResp( info )
-- "formation_slot__H",    --阵型槽位
-- "gem_inlay_list__T__id@I##hole@H", -- 宝石镶嵌列表
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石镶嵌响应 消息Id: 25306")
	-- --本地处理 在列表里减少这个道具
	for i,v in ipairs( info.gem_inlay_list ) do
		self.onVacancy = v.hole
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.GemModule.INLAY)
end

-- 消息说明:宝石卸下请求 消息Id: 25307
function Modules.GemModule:GemUnloadReq( info )
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石卸下请求 消息Id: 25307")
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GEM_UNLOAD_REQ)
	protocol.formation_slot = info.formation_slot - 1
	protocol.gem_hole = info.gem_hole

	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:宝石卸下响应 消息Id: 25308
function Modules.GemModule:GemUnloadResp( info )
-- "gem_hole__H",  --宝石孔位
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石卸下响应 消息Id: 25308")

	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_discharge_succeed"))

	GlobalEventSystem:FireNextFrame(Macros.Event.GemModule.DISCHARGE)
end

-- 消息说明:宝石孔位开启请求 消息Id: 25309
function Modules.GemModule:GemHoleOpenReq( info )
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石孔位开启请求 消息Id: 25309")

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GEM_HOLE_OPEN_REQ)
	protocol.formation_slot = info.formation_slot - 1
	protocol.gem_hole = info.gem_hole
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:宝石孔位开启响应 消息Id: 25310
function Modules.GemModule:GemHoleOpenResp( info )
-- "formation_slot__H",    --阵型槽位
-- "gem_hole__H",  --宝石孔位
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石孔位开启响应 消息Id: 25310")

	self.openList[info.formation_slot + 1 ] = info.gem_hole

	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_open_succeed"))
	
	GlobalEventSystem:FireNextFrame(Macros.Event.GemModule.DISCHARGE)
end

-- 消息说明:宝石数据通知 消息Id: 25311
function Modules.GemModule:GemDataNotify( info )
-- "gem_id__I",    --宝石ID
-- "gem_num__I",   --宝石数量
-- "flag__C",  --操作标志(0=减少,1=添加或者更新)
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石数据通知 消息Id: 25311")
	self:UpdatePack( info )
end

-- 消息说明:宝石分解请求 消息Id: 25312
function Modules.GemModule:GemDecomposeReq( info )
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石分解请求 消息Id: 25312")

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GEM_DECOMPOSE_REQ)
	protocol.gem_id = info.gem_id
	protocol.gem_num = info.gem_num
	GlobalNetAdapter:OnSend(protocol)
end

-- 消息说明:宝石分解响应 消息Id: 25313
function Modules.GemModule:GemDecomposeResp( info )
	-- "gem_id__I",	--宝石ID
	-- "gem_num__I",	--宝石数量
	CPPGameLib.PrintTable("xy" , info ,"消息说明:宝石分解响应 消息Id: 25312")
	local cnf = self:GetGemCnf( info.gem_id )

	local t1 = {}
    table.insert( t1 , { type = Macros.Game.RolePropType.GEM , num = cnf.gem_coin * info.gem_num })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    GlobalEventSystem:FireNextFrame( Macros.Event.GemModule.DECOMPOSE )
end

--更新本地数据
function Modules.GemModule:UpdatePack( data )
	--没有就新加
	if not self.userPack[ data.gem_id ] then
        local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , data.gem_id )
        local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , gemCnf.type )
		self.userPack[ data.gem_id ] = { id = data.gem_id , num = data.gem_num , max = inlayCnf.inlay_num_limit , type = gemCnf.type  }
		--通知新增
		self:EventGemAdd( data.gem_id )
	else
		--减少就减少
		if data.flag == 0 then
			self.userPack[ data.gem_id ].num = self.userPack[ data.gem_id ].num - data.gem_num
		--增加就增加
		else
			self.userPack[ data.gem_id ].num = self.userPack[ data.gem_id ].num + data.gem_num
		end
		--通知刷新
		self:EventGemUp( data.gem_id )
	end

	--没有了
	if self.userPack[ data.gem_id ].num <= 0 then
		self.userPack[ data.gem_id ] = nil
		--通知删除
		self:EventGemRemove( data.gem_id )
	end
end

--按id获得背包物品
function Modules.GemModule:GetPack( id )
    return self.userPack[id]
end

--当前阵位开启等级
function Modules.GemModule:GetOpenLevel( index )
	if not index then
		return
	end
	return self.openList[ index ] 
end

--保存选中的阵位的英雄
function Modules.GemModule:SetOnHeroUid( uid )
	self.onHeroUid = uid
	GlobalEventSystem:FireNextFrame(Macros.Event.GemModule.SELECT_HERO)
end

--获得当前阵位上的宝石列表
function Modules.GemModule:GetOnGemLocationLst( )
	-- return self.onHeroUid
	--根据英雄uid获取当前阵位信息
	local lineup_index , lineup_info = CallModuleFunc(ModuleType.HERO , "GetLineupInfoByUID" , self.onHeroUid )

	local t1 = {}
	for i , v in ipairs( lineup_info.gem_list.__list ) do
		t1[i] = v.gem_id
	end
	
	return t1 , lineup_index
end

--获得当前阵位上没有宝石的阵位列表
function Modules.GemModule:GetNoGemLocationLst( )
	local t1 , index = self:GetOnGemLocationLst()
	local openLevel = self:GetOpenLevel(index)
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local t2 = {}
	for i , v in ipairs( t1 ) do
		local holeCnf = self:GetGemHoleCnf( i )
		if v == 0 and i <= openLevel and holeCnf.role_level <= roleLevel then
			table.insert( t2 , {noGemPos = i , openlevel = holeCnf.role_level } )
		end
	end
	return t2
end

--选择空位
function Modules.GemModule:SetOnVacancy( pos )
	self.onVacancy = pos
end

--选择空位
function Modules.GemModule:GetOnVacancy( )
	return self.onVacancy
end

--打孔选择
function Modules.GemModule:SetOpenHole( pos )
	self.openHole = pos
end

--打孔选择
function Modules.GemModule:GetOpenHole( pos )
	return self.openHole
end

--宝石列表
function Modules.GemModule:GetGemLit( ... )
	local t1 = {}
	for k , v in pairs( self.userPack ) do
		local gemCnf = self:GetGemCnf( v.id )
		local inlayCnf = self:GetGemInlayCnf( gemCnf.type )
		table.insert( t1 , { gem_id = v.id , sotr = gemCnf.power , num = v.num , max = inlayCnf.inlay_num_limit , type = gemCnf.type } )
	end
	table.sort( t1 , function ( a , b )
 		return a.sotr > b.sotr
	end )
	return t1
end

--宝石条目属性
function Modules.GemModule:GetGemProperty( id )
	local gemCnf = self:GetGemCnf( id )

	local t3 = {}
	--宝石属性
	local t1 = {}
	for i , v in ipairs( gemCnf.init_pro_type_list ) do
		table.insert( t1 , { type = v , num = gemCnf.init_pro_value_list[i] } )
	end

	table.insert( t3 , { name = CPPGameLib.GetString("gem_gem_property") , lst = t1 } )  

	--技能属性
	local skillCnf = self:GetGemSkillCnf( gemCnf.skill_id )
	local skillLst = skillCnf.skill_list[ gemCnf.skill_level ]

	local t2 = {}
	for i , v in ipairs( skillLst.pro_type_list ) do
		table.insert( t2 , { type = v , num = skillLst.pro_value_list[i] , skill = gemCnf.skill_id , skill_level = gemCnf.skill_level } )
	end

	table.insert( t3 , { name = CPPGameLib.GetString("gem_skill_property") , lst = t2 } )  

	return t3
end

--当前等级符合显示格子
function Modules.GemModule:GetNowLevelShow()
	local holeCnf = self:GetGemHoleCnf()
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local nowIconLst = {}
	local index = 0
	for i , v in ipairs( holeCnf ) do
		if v.role_level <= roleLevel then
			index = i
			table.insert( nowIconLst , v )
		end
	end

	if holeCnf[index + 1] then
		table.insert( nowIconLst , holeCnf[index + 1] )
	end

	return nowIconLst
end

--当前阵位的属性
function Modules.GemModule:GetNowStationNature()
	local lst = self:GetOnGemLocationLst()
	CPPGameLib.PrintTable("xy" , lst , "lst1") 

	--宝石属性解析

	--属性
	local gemLst = {}
	--技能
	local skillLst = {}

	for i , v in ipairs( lst ) do
		if v ~= 0 then
			local property = self:GetGemProperty(v)
			--属性
			table.insert( gemLst , property[1] )
			--技能
			table.insert( skillLst , property[2] )
		end
	end
	CPPGameLib.PrintTable("xy" , gemLst , "gemLst") 

	--宝石属性
	local t1 = {}
	for _ , v in ipairs( gemLst ) do
		for i , vv in ipairs( v.lst ) do
			if not t1[vv.type] then
				t1[vv.type] = 0
			end
			t1[vv.type] = t1[vv.type] + vv.num
		end
	end

	local gemLst = {}
	for i,v in pairs(t1) do
		table.insert( gemLst , { type = i , num = v } )
	end
	table.sort( gemLst, function ( a , b )
		return a.type < b.type
	end )

	--宝石技能
	local t1 = {}
	for i , v in ipairs( skillLst ) do
		for ii , vv in ipairs( v.lst ) do
			if not t1[ vv.skill ] then
				t1[ vv.skill ] = {}
			end
			table.insert( t1[ vv.skill ] , vv )
			t1[ vv.skill ][ #t1[ vv.skill ] ].skill = nil
		end
	end

	local t3 = {}
	for k , v in pairs( t1 ) do
		local t2 = {}
		for i , vv in ipairs( v ) do
			if not t2[vv.type] then
				t2[vv.type] = {}
				t2[vv.type].type = vv.type
				t2[vv.type].level = 0
				t2[vv.type].num = 0
				t2[vv.type].skill = k
			end
			t2[vv.type].level = t2[vv.type].level + vv.skill_level

			local cnf = self:GetGemSkillCnf( k )
		end
		table.insert( t3 , t2 )
	end

	for i , v in ipairs( t3 ) do
		for ii , vv in pairs( v ) do
			local cnf = self:GetGemSkillCnf( vv.skill )
			local t3_1 = {}
			for iii , vvv in ipairs( cnf.skill_list[ vv.level ].pro_type_list ) do
				t3_1[ vvv ] = cnf.skill_list[ vv.level ].pro_value_list[iii]
			end

			if t3_1[ vv.type ] then
				vv.num = t3_1[ vv.type ]
			end
		end
	end

	local t4 = {}
	for i , v in ipairs( t3 ) do
		local t5 = {}
		for ii , vv in pairs( v ) do
			table.insert( t5 , vv )
		end
		table.insert( t4 , t5 )
	end

	local skillLst = {}
	skillLst = t4
	
	return gemLst , skillLst
end

--宝石列表
function Modules.GemModule:GetGemCnf( id )
	local gemCnf = nil
	if id then
		gemCnf = CPPGameLib.GetConfig("Gem", id )
	else
		gemCnf = CPPGameLib.GetConfig("Gem", nil , nil , true )
	end
	return gemCnf
end

--宝石坑位列表
function Modules.GemModule:GetGemHoleCnf( id )
	local gemCnf = nil
	if id then
		gemCnf = CPPGameLib.GetConfig("GemHole", id , false )
	else
		gemCnf = CPPGameLib.GetConfig("GemHole", nil , nil , true )
	end
	return gemCnf
end

--宝石技能列表
function Modules.GemModule:GetGemSkillCnf( id )
	local gemCnf = nil
	if id then
		gemCnf = CPPGameLib.GetConfig("GemSkill", id )
	else
		gemCnf = CPPGameLib.GetConfig("GemSkill", nil , nil , true )
	end 
	return gemCnf
end

--宝石技能列表
function Modules.GemModule:GetGemInlayCnf( id )
	local gemCnf = nil
	if id then
		gemCnf = CPPGameLib.GetConfig("GemInlayLimit", id )
	else
		gemCnf = CPPGameLib.GetConfig("GemInlayLimit", nil , nil , true )
	end
	return gemCnf
end

--宝石刷新
function Modules.GemModule:EventGemUp( id )
	print("xy" , "==========宝石刷新")
	GlobalEventSystem:FireNextFrame( Macros.Event.GemModule.UP_CONTENT , id )
end

--宝石增加
function Modules.GemModule:EventGemAdd( id )
	print("xy" , "==========宝石增加")
	--刷列表
	local info = {}
	info.type = "add"
	info.id = id
	GlobalEventSystem:FireNextFrame( Macros.Event.GemModule.UP_LST , info )
end

--宝石失去
function Modules.GemModule:EventGemRemove( id )
	print("xy" , "==========宝石失去")
	--刷列表
	local info = {}
	info.type = "remove"
	info.id = id
	GlobalEventSystem:FireNextFrame( Macros.Event.GemModule.UP_LST , info )
end


--宝石失去
function Modules.GemModule:SetOpenType( ver )
	self.openType = ver
end

--宝石失去
function Modules.GemModule:GetOpenType( id )
	return self.openType
end

--当前选中英雄
function Modules.GemModule:SetOnHero( id )
	self.onHero = id
end

--当前选中英雄
function Modules.GemModule:GetOnHero( id )
	return self.onHero
end

--界面第一次打开
function Modules.GemModule:SetNoeOpen( ver , ver_ )
	print("xy" , "SetNoeOpen................." , ver , ver_ )
	self.noeOpen = ver
	self.noeOpen_ = ver_
end

--获得第一次打开
function Modules.GemModule:GetNoeOpen( id )
	return self.noeOpen , self.noeOpen_
end

--获得宝石属性加成
function Modules.GemModule:GetPlusPropMap(gem_list)
	local plus_prop_map
	local skill_map
	if gem_list then
		for _, gem_info in ipairs(gem_list) do
			local config_gem = CPPGameLib.GetConfig("Gem", gem_info.gem_id, false)						
			if config_gem then
				--记录宝石技能数据
				skill_map = skill_map or {}
				skill_map[config_gem.skill_id] = skill_map[config_gem.skill_id] or 0
				skill_map[config_gem.skill_id] = skill_map[config_gem.skill_id] + config_gem.skill_level

				--宝石基础属性加成
				if config_gem.init_pro_type_list then
					for i, pro_type in ipairs(config_gem.init_pro_type_list) do
						plus_prop_map = plus_prop_map or {}
						plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
						plus_prop_map[pro_type] = plus_prop_map[pro_type] + config_gem.init_pro_value_list[i]
					end
				end
			end
		end
	end

	--宝石技能属性加成
	if skill_map then
		for skill_id, skill_level in pairs(skill_map) do
			local config_gem_skill = CPPGameLib.GetConfig("GemSkill", skill_id, false)
			if config_gem_skill and config_gem_skill.skill_list then
				local config_gem_skill_level = config_gem_skill.skill_list[skill_level]
				if config_gem_skill_level and config_gem_skill_level.pro_type_list then
					for i, pro_type in ipairs(config_gem_skill_level.pro_type_list) do
						plus_prop_map = plus_prop_map or {}
						plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
						plus_prop_map[pro_type] = plus_prop_map[pro_type] + config_gem_skill_level.pro_value_list[i]
					end
				end
			end
		end
	end	

	return plus_prop_map
end

--宝石能否合成
function Modules.GemModule:ToIdGetGemIsCompound( id )
	local cnf = self:GetGemCnf( id )
	local info = self:GetPack( id )
	if info and cnf.compose_cost_num ~= 0 and info.num >= cnf.compose_cost_num then
		return true
	end
	return false
end