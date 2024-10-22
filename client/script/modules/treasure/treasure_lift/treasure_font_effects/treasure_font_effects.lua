
--[[
%% @module: 圣器强化字体特效
%% @author: yjg
%% @created: 2016年12月20日22:04:17
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TreasureIntensifyFontEffects = LogicGUI.TreasureIntensifyFontEffects or BaseClass()

-- 层 数据 要不要加上装备大师 移动位置
function LogicGUI.TreasureIntensifyFontEffects:__init( parent_node , params , movex , movey , fun )
end

function LogicGUI.TreasureIntensifyFontEffects:__delete()
end


function LogicGUI.TreasureIntensifyFontEffects:GetTreasureIntensifyTab( params , pos )
	-- local t1 = self:Master( params )
	local t1 = self:Intensify( params , pos )
	local t3 = {
			[1] = t1}
	return t3
end

function LogicGUI.TreasureIntensifyFontEffects:Master( params )

   --列出所有的圣器信息
   	local treasureLst = {}
   	for i=1,5 do
		local lineup_info = CallModuleFunc(ModuleType.HERO, "GetLineupInfo" ,i )
		table.insert( treasureLst , { 
							[1] = lineup_info.attack_treasure_uid ,
							[2] = lineup_info.defence_treasure_uid ,
						} 
					)
   	end

	--拿出这件圣器所在阵位的全部圣器
	local cover = {}
	for frontId , frontLst  in ipairs(treasureLst) do
		for _ , uid in ipairs(frontLst) do
			if params.uid == uid then
				cover = frontLst
				break
			end
		end
	end

	if #cover <= 0 then
	   	local tab = {}
	   	tab.lst = {}
	   	tab.type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE
		return tab
	end
	
	--如果这个阵位的装备不齐
	for i,v in ipairs(cover) do
		if v == 0 then
		   	local tab = {}
		   	tab.lst = {}
		   	tab.type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE
			return tab
		end
	end


	local StrengthenLst = {}
	--按照uid取得所有的装备强化等级
	for i,v in ipairs(cover) do
		local equipInfo = CallModuleFunc(ModuleType.TREASURE, "GetTreasure" , v )

		local old = equipInfo.strengthen_level
		if equipInfo.uid == params.uid then
			old = equipInfo.strengthen_level - 1
		end

		table.insert( StrengthenLst , { lv = equipInfo.strengthen_level , uid = equipInfo.uid , old = old } )
	end


	--找到等级最低的那个
	table.sort( StrengthenLst , function ( a , b )
		return a.lv < b.lv
	end )
	local nowMinType , nowMinValue , nowMinlevel =  ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList( StrengthenLst[1].lv )

	--找到等级最低的那个
	table.sort( StrengthenLst , function ( a , b )
		return a.old < b.old
	end )
	local oldMinType , oldMinValue , oldMinlevel =  ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList( StrengthenLst[1].old )

   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE
   	--图片的间距是 10
   	local interval = 0

   	--如果现在的大师和以前的大师不一样
   	if nowMinlevel ~= oldMinlevel then
   		--圣器强化大师达成
   		local forge_equip_intensify_master_reach = Util:LabCenter({ 
   																	{ zi = CPPGameLib.GetString("forge_treasure_intensify_master_reach") , ys = 100 ,dx = Macros.TypefaceSize.tab , mb = true },
   																	{ zi = nowMinlevel , Macros.Game.QualityType.GREEN , dx = Macros.TypefaceSize.tab , mb = true },
   																	{ zi = CPPGameLib.GetString("forge_reach") , ys = Macros.Game.QualityType.GREENWHITE , dx = Macros.TypefaceSize.tab , mb = true },
													   			})
   		interval = 5
   		table.insert( tab.lst , { node = forge_equip_intensify_master_reach , interval = interval } )

   		local natureText = {}
   		for i,v in ipairs(nowMinType) do
   			local nature = Util:Nature( nowMinType[i] , nowMinValue[i] , nil , nil , true )
   			natureText[i] = Util:Name(nature , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
	   		interval = 5
	   		table.insert( tab.lst , { node = natureText[i] , interval = interval } )
   		end
   	end

   	return tab
end

function LogicGUI.TreasureIntensifyFontEffects:Intensify( params , pos )
   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION

	--获得装备的属性
	local now , formerly , nex  = CallModuleFunc(ModuleType.TREASURE, "GetStrengthenPropPlusMap", params.uid , params.strengthen_level)

	--强化提示字
	-- "强化 %d 次,等级提升 %d"
	local texe = CPPGameLib.GetString("treasure_intensifySucceed" , params.new_level ) 
	local intensifyText = Util:Name( texe , Macros.TypefaceSize.tab )
	interval = 5
	table.insert( tab.lst , { node = intensifyText , interval = interval , pos = { x = pos[1].x , y = pos[1].y } } )

	local intensifyProperty = {}

	--属性显示
	for i,v in ipairs(now.lst) do
		local value = v.value - formerly.lst[i].value
		if v.value == formerly.lst[i].value then
			value = formerly.lst[i].value
		end
		local type_ , info_ = Util:Nature( v.type , value , true )
		local intensifyProperty = Util:Name( type_ .." ".. info_ , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
		table.insert( tab.lst , { node = intensifyProperty , interval = interval , pos = { x = pos[i + 1].x , y = pos[i + 1].y } } )
	end
	
   	return tab
end


--[[
%% @module: 圣器精炼字体特效
%% @author: yjg
%% @created: 2016年12月20日22:04:17
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TreasureRefineFontEffects = LogicGUI.TreasureRefineFontEffects or BaseClass()

-- 层 数据 要不要加上装备大师 移动位置
function LogicGUI.TreasureRefineFontEffects:__init( parent_node , params , movex , movey , fun )
end

function LogicGUI.TreasureRefineFontEffects:__delete()
end


function LogicGUI.TreasureRefineFontEffects:GetTreasureRefineTab( params , pos )
	local t1 = self:Intensify( params , pos )
	local t3 = {
			[1] = t1 }
	return t3
end

function LogicGUI.TreasureRefineFontEffects:Master( params )

   --列出所有的装备信息
	local treasureLst = {}
	for i = 1 , 5 do
		local lineup_info = CallModuleFunc(ModuleType.HERO, "GetLineupInfo" ,i )

		table.insert( treasureLst , { 
							[1] = lineup_info.attack_treasure_uid ,
							[2] = lineup_info.defence_treasure_uid ,
						} )
	end

	--拿出这件装备所在阵位的全部装备
	local cover = {}
	for frontId , frontLst  in ipairs(treasureLst) do
		for _ , uid in ipairs(frontLst) do
			if params.uid == uid then
				cover = frontLst
				break
			end
		end
	end

	if #cover <= 0 then
	   	local tab = {}
	   	tab.lst = {}
	   	tab.type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE
		return tab
	end

	--如果这个阵位的装备不齐
	for i,v in ipairs(cover) do
		if v == 0 then
		   	local tab = {}
		   	tab.lst = {}
		   	tab.type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE
			return tab
		end
	end


	CPPGameLib.PrintTable("xy", params ,"params")

	local StrengthenLst = {}
	--按照uid取得所有的装备强化等级
	for i,v in ipairs(cover) do
		local equipInfo = CallModuleFunc(ModuleType.TREASURE, "GetTreasure" , v )

		local old = equipInfo.refine_level
		if equipInfo.uid == params.uid then
			old = params.refine_level - 1
		end

		table.insert( StrengthenLst , { lv = equipInfo.refine_level , uid = equipInfo.uid , old = old } )
	end

	CPPGameLib.PrintTable("xy", StrengthenLst ,"StrengthenLst")


	--找到等级最低的那个
	table.sort( StrengthenLst , function ( a , b )
		return a.lv < b.lv
	end )
	local nowMinType , nowMinValue , nowMinlevel =  ConfigAdapter.Forge.GetTreasureRefinePlusPropList( StrengthenLst[1].lv )

	--找到等级最低的那个
	table.sort( StrengthenLst , function ( a , b )
		return a.old < b.old
	end )
	local oldMinType , oldMinValue , oldMinlevel =  ConfigAdapter.Forge.GetTreasureRefinePlusPropList( StrengthenLst[1].old )

   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE
   	--图片的间距是 10
   	local interval = 0
   	print("xy" , "nowMinlevel" , nowMinlevel)
	print("xy" , "oldMinlevel" , oldMinlevel)


   	-- --如果现在的大师和以前的大师不一样
   	if nowMinlevel ~= oldMinlevel then
   		--圣器精炼大师达成
   		local forge_equip_intensify_master_reach = Util:LabCenter({ 
   																	{ zi = CPPGameLib.GetString("forge_treasure_refine_master_reach") , ys = 100 ,dx = Macros.TypefaceSize.tab , mb = true },
   																	{ zi = nowMinlevel , ys = Macros.Game.QualityType.GREEN , dx = Macros.TypefaceSize.tab , mb = true },
   																	{ zi = CPPGameLib.GetString("forge_reach") , ys = Macros.Game.QualityType.GREENWHITE , dx = Macros.TypefaceSize.tab , mb = true },
													   			})
   		interval = 5
   		table.insert( tab.lst , { node = forge_equip_intensify_master_reach , interval = interval } )

   		local natureText = {}
   		for i,v in ipairs(nowMinType) do
   			local nature = Util:Nature( nowMinType[i] , nowMinValue[i] , nil , nil , true )
   			natureText[i] = Util:Name(nature , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
	   		interval = 5
	   		table.insert( tab.lst , { node = natureText[i] , interval = interval } )
   		end
   	end
   	return tab
end

function LogicGUI.TreasureRefineFontEffects:Intensify( params , pos )
   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION

	--获得装备的属性
	local now , formerly , nex  = CallModuleFunc(ModuleType.TREASURE, "GetRefinePropPlusMap", params.uid , params.refine_level)
	
	--精炼提示字
	local texe = CPPGameLib.GetString("treasure_refineSucceed" , params.refine_level ) 
	local intensifyText = Util:Name( texe , Macros.TypefaceSize.tab )
	interval = 5
	table.insert( tab.lst , { node = intensifyText , interval = interval , pos = { x = pos[1].x , y = pos[1].y } } )

	local intensifyProperty = {}

	--属性显示
	for i,v in ipairs(now.lst) do
		local value = v.value - formerly.lst[i].value
		if v.value == formerly.lst[i].value then
			value = v.value
		end
		local type_ , info_ = Util:Nature( v.type , value , true )
		local intensifyProperty = Util:Name( type_ .." ".. info_ , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
		table.insert( tab.lst , { node = intensifyProperty , interval = interval , pos = { x = pos[i+1].x , y = pos[i+1].y } } )
	end
	
   	return tab
end
