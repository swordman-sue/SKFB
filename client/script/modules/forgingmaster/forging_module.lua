--
-- @brief 锻造大师模块 
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--

Modules = Modules or {}

Modules.ForgeModule = Modules.ForgeModule or BaseClass(BaseModule)

function Modules.ForgeModule:__init()

	self.Uplv = 0
end

function Modules.ForgeModule:__delete()
end

--获得装备的属性
function Modules.ForgeModule:GetProperty( pos , index )

	local _ , equipInfo , treasureInfo = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", pos )
	local equipNum = CPPGameLib.GetTableLength( equipInfo )
	local fourInOne = {}
	local allLst = {}
	if equipNum == 4 then
		fourInOne[1] = { type = "strengthen_level" , site = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList , cnf = "EquipmentStrengthenForge" }
		fourInOne[2] = { type = "refine_level" , site = ConfigAdapter.Forge.GetEquipRefinePlusPropList , cnf = "EquipmentRefineForge" }
		local t1 = {}
		for k,v in pairs(equipInfo) do
			table.insert( t1 , v )
		end

		table.insert( allLst , t1 )
		table.insert( allLst , t1 )
	end

	local treasureNum = CPPGameLib.GetTableLength( treasureInfo )
	if treasureNum == 2 then
		if not fourInOne[1] then
			fourInOne[1] = { type = "strengthen_level" , site = ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList , cnf = "TreasureStrengthenForge" }
			fourInOne[2] = { type = "refine_level" , site = ConfigAdapter.Forge.GetTreasureRefinePlusPropList , cnf = "TreasureRefineForge" }
		else
			fourInOne[3] = { type = "strengthen_level" , site = ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList , cnf = "TreasureStrengthenForge" }
			fourInOne[4] = { type = "refine_level" , site = ConfigAdapter.Forge.GetTreasureRefinePlusPropList , cnf = "TreasureRefineForge" }
		end
		local t1 = {}
		for k,v in pairs(treasureInfo) do
			table.insert( t1 , v )
		end

		table.insert( allLst , t1 )
		table.insert( allLst , t1 )
	end

	local lst = allLst[index]
	table.sort( lst ,function ( a , b )
		return a[ fourInOne[index].type ] < b[ fourInOne[index].type ]
	end )

	local t1 = lst[1]
	local level = t1[ fourInOne[index].type ]
	return self:Whole( level , fourInOne[index])
end

function Modules.ForgeModule:Whole( showLv , fourInOne )
	--当前等级
	local typeLst , valueLst , showLv , cnf =  fourInOne.site( showLv )

	--找到最高的等级
  	local t1 = {}
  	for k,v in pairs( CPPGameLib.GetConfig( fourInOne.cnf , nil , nil ,true) ) do
  		table.insert( t1 , v )
  	end	

  	table.sort( t1 , function ( a , b )
  		return a.showLv < b.showLv
  	end )

  	local t2 = {}
  	for i,v in ipairs(t1) do
  		if v.showLv > showLv then
  			t2 = v
  			break
  		end
  	end

  	local max = t1
  	table.sort( max , function ( a , b )
  		return a.showLv > b.showLv
  	end )
  	max = max[1]

  	--类型列表 数值列表 当前强化等级 下级强化等级 标题

  	if showLv == 0 then
  		local t1 = {}
  		t1.level = cnf.showLv
  		t1.lst = {}

  		local t3 = {}
  		t3.lst = {}
  		t3.level = t2.showLv
  		for i,v in ipairs( t2.pro_type_list ) do
  			table.insert( t3.lst , { type = v , value = t2.pro_value_list[i] } )
  		end
  		return t1 , t3 , cnf.strengthen_level or cnf.refine_level , t2.strengthen_level or t2.refine_level , cnf.lvname , max
  	elseif showLv == t1[1].showLv then

  		local t2 = {}
  		t2.lst = {}
  		t2.level = t1[#t1].showLv
  		for i,v in ipairs(t1[#t1].pro_type_list) do
  			table.insert( t2.lst , { type = v , value = t1[#t1].pro_value_list[i] } )
  		end
  		return t2 ,t2 , t1[#t1].strengthen_level or t1[#t1].refine_level , t1[#t1].strengthen_level or t1[#t1].refine_level , t1[#t1].lvname , max
  	else

  		local t1 = {}
  		t1.lst = {}
  		t1.level = cnf.showLv
  		for i,v in ipairs( typeLst ) do
  			table.insert( t1.lst , { type = v , value = valueLst[i] } )
  		end

  		local t3 = {}
  		t3.lst = {}
  		t3.level = t2.showLv
  		for i,v in ipairs( t2.pro_type_list ) do
  			table.insert( t3.lst , { type = v , value = t2.pro_value_list[i] } )
  		end

  		return t1 , t3 , cnf.strengthen_level or cnf.refine_level , t2.strengthen_level or t2.refine_level , cnf.lvname , max
  	end
end

function Modules.ForgeModule:EquipIdSort( index , data )

	local t1 = {}
	if data == 0 then
		t1 = {
			[1] = 0
		}		
		return t1
	end
	
	for k,v in pairs(data) do
		local info = CallModuleFunc(ModuleType.EQUIP, "GetEquip",v)
		table.insert(t1 , info)
	end
	for i,v in ipairs(t1) do
		local info = CPPGameLib.GetConfig("Equipment", v.info_id,false)
		v.item_type = info.item_type
		v.icon = info.res_id
		v.name = info.name
		v.quality = info.quality
		v.description = info.description
	end
	table.sort( t1, function ( a , b )
		return a.item_type < b.item_type
	end )

	local t_1 ,t_2 ,t_3 = self:GetProperty(index,t1)
	for i,v in ipairs(t1) do
		v.max = t_3
	end
	return t1
end

function Modules.ForgeModule:TreasureIdSort( index , data )
	local t1 = {}
	if data == 0 then
		t1 = {
			[1] = 0
		}		
		return t1
	end
	
	for k,v in pairs(data) do
		local info = CallModuleFunc(ModuleType.TREASURE, "GetTreasure",v)
		table.insert(t1 , info)
	end
	for i,v in ipairs(t1) do
		local info = CPPGameLib.GetConfig("Treasure", v.info_id,false)
		v.item_type = info.type
		v.icon = info.res_id
		v.name = info.name
		v.quality = info.quality
		v.description = info.description
	end

	table.sort( t1, function ( a , b )
		return a.item_type < b.item_type
	end )
	local t_1 ,t_2 ,t_3 = self:GetProperty(index,t1)

	for i,v in ipairs(t1) do
		v.max = t_3
	end

	return t1
end


function Modules.ForgeModule:ToUidGetMaster( type_ , uid )
	--定路径
	local lab = {  
			[Macros.Game.ObjType.EQUIP]	= { 
										[1] = {type_ = "EquipmentStrengthenForge" , value = "strengthen_level"},
										[2] = {type_ = "EquipmentRefineForge" , value = "refine_level"},
									},
			[Macros.Game.ObjType.TREASURE]	= { 
										[1] = {type_ = "TreasureStrengthenForge" , value = "strengthen_level"},
										[2] = {type_ = "TreasureRefineForge" , value = "refine_level"},
									},									
	}

	--找到身上的装备	
	--总列表
	local t1 = {}
   	for i=1,5 do
   		--取阵位信息
		local lineup_info = CallModuleFunc(ModuleType.HERO, "GetLineupInfo" ,i )
			--装备 										 --腰带
		if uid == lineup_info.clothes_equipment_uid or uid == lineup_info.belt_equipment_uid or 
			--武器 										 --头盔
			uid == lineup_info.weapon_equipment_uid or uid == lineup_info.hat_equipment_uid or 
			--徽记 										 --纹章
			uid == lineup_info.attack_treasure_uid or uid == lineup_info.hat_equipment_uid then

			CPPGameLib.PrintTable(lineup_info.belt_equipment_uid,"lineup_info.belt_equipment_uid-------------------")

			 t1 = { 
					pos = i , 											--阵位
					hero = lineup_info.uid ,  							--英雄
					clothes =  lineup_info.clothes_equipment_uid, 		--衣服
					belt = lineup_info.belt_equipment_uid, 				--腰带
					weapon = lineup_info.weapon_equipment_uid, 			--武器
					hat = lineup_info.hat_equipment_uid,				--头盔
					attack = lineup_info.attack_treasure_uid,			--徽记
					defence = lineup_info.defence_treasure_uid,			--纹章
				}
			break
		end
	end


	local firstLst = {}
	if type_ == Macros.Game.ObjType.EQUIP then
		--装备列表
	  	firstLst = { 
  				[1] = t1.clothes,
  				[2] = t1.belt,
  				[3] = t1.weapon,
  				[4] = t1.hat,
  			}
	 elseif type_ == Macros.Game.ObjType.TREASURE then
		--圣器列表
	  	firstLst = { 
  				[1] = t1.attack,
  				[2] = t1.defence,
  			}
	 end

  	--如果有残缺的
  	local key = true
  	for i,v in ipairs(firstLst) do
  		if v == 0 then
  			key = false
  			return false
  		end
  	end

  	--装备信息列表
  	local transfersLst = {}
  	for i,v in ipairs(firstLst) do
  		if type_ == Macros.Game.ObjType.EQUIP then
	  		local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip" , v)
	  		table.insert( transfersLst , { uid = v , info_id = equipInfo.info_id , strengthen_level = equipInfo.strengthen_level , refine_level = equipInfo.refine_level  }  )
  		elseif type_ == Macros.Game.ObjType.TREASURE then
			local equipInfo = CallModuleFunc(ModuleType.TREASURE, "GetTreasure" , v)
			table.insert( transfersLst , { uid = v , info_id = equipInfo.info_id , strengthen_level = equipInfo.strengthen_level , refine_level = equipInfo.refine_level  }  )
  		end
  	end

  	--排序结果
  	local sort = {}
  	for i,v in ipairs( lab[type_] ) do
	  	--强化排序
	  	table.sort( transfersLst, function ( a , b )
	  		return a[v.value] < b[v.value]
	  	end )	  	
	  	table.insert(sort , { lst = transfersLst[1] , type_ = v.value }  )
  	end

  	local endLst = {}
  	--锻造大师
  	for _ , v in ipairs(sort) do
  		if v.lst then
			local lv = v.lst[lab[type_][_].value]
			for i=1,50 do
				local cnf = CPPGameLib.GetConfig( lab[type_][_].type_ , lv , false)
				if not cnf and lv ~= 0 then
					lv = lv - 1
				else
	  				endLst[lab[type_][_].value] = cnf
	  				break	
				end
			end
		end
  	end

  	return endLst
end