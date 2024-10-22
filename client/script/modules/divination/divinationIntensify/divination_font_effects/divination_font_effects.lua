
--[[
%% @module: 卡牌强化字体特效
%% @author: yjg
%% @created: 2017年12月6日19:39:04
--]]

LogicGUI = LogicGUI or {}

LogicGUI.DivinationIntensifyFontEffects = LogicGUI.DivinationIntensifyFontEffects or BaseClass()

-- 层 数据 要不要加上装备大师 移动位置
function LogicGUI.DivinationIntensifyFontEffects:__init( parent_node , params , movex , movey , fun )
end

function LogicGUI.DivinationIntensifyFontEffects:__delete()
end


function LogicGUI.DivinationIntensifyFontEffects:GetDivinationIntensifyTab( params , pos )
	-- local t1 = self:Master( params )
	local t1 = self:Intensify( params , pos )
	local t3 = {
			[1] = t1}
	return t3
end

function LogicGUI.DivinationIntensifyFontEffects:Intensify( params , pos )
   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION

	--获得装备现在的属性
	local now = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo", params.info_id , params.level )
-- 
	--获得装备以前的属性
	local before = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo", params.info_id , params.level - 1 )

	--强化提示字
	-- 升级成功
	local texe = CPPGameLib.GetString("divination_updateSuccessfully" , params.level ) 
	local intensifyText = Util:Name( texe , Macros.TypefaceSize.tab )
	interval = 5
	table.insert( tab.lst , { node = intensifyText , interval = interval , pos = { x = pos[1].x , y = pos[1].y } } )

	local intensifyProperty = {}

	--属性显示
	for i,v in ipairs(now.up_pro) do
		
		local value = v.value - before.up_pro[i].value
		if v.value == before.up_pro[i].value then
			value = before.up_pro[i].value
		end
		local type_ , info_ = Util:Nature( v.type , value , true )
		local intensifyProperty = Util:Name( type_ .." ".. info_ , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
		table.insert( tab.lst , { node = intensifyProperty , interval = interval , pos = { x = pos[i + 1].x , y = pos[i + 1].y } } )
	end
	
   	return tab
end



-- --[[
-- %% @module: 卡牌突破
-- %% @author: yjg
-- %% @created: 2017年12月6日20:42:44
-- --]]

-- LogicGUI = LogicGUI or {}

-- LogicGUI.DivinationRefineFontEffects = LogicGUI.DivinationRefineFontEffects or BaseClass()

-- -- 层 数据 要不要加上装备大师 移动位置
-- function LogicGUI.DivinationRefineFontEffects:__init( parent_node , params , movex , movey , fun )
-- end

-- function LogicGUI.DivinationRefineFontEffects:__delete()
-- end


-- function LogicGUI.DivinationRefineFontEffects:GetTreasureRefineTab( params , pos )
-- 	local t1 = self:Intensify( params , pos )
-- 	local t3 = {
-- 			[1] = t1 }
-- 	return t3
-- end

-- function LogicGUI.DivinationRefineFontEffects:Intensify( params , pos )
--    	local tab = {}
--    	tab.lst = {}
--    	tab.type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION

-- 	--获得装备的属性
-- 	local now , formerly , nex  = CallModuleFunc(ModuleType.TREASURE, "GetRefinePropPlusMap", params.uid , params.refine_level)
	
-- 	--精炼提示字
-- 	local texe = CPPGameLib.GetString("treasure_refineSucceed" , params.refine_level ) 
-- 	local intensifyText = Util:Name( texe , Macros.TypefaceSize.tab )
-- 	interval = 5
-- 	table.insert( tab.lst , { node = intensifyText , interval = interval , pos = { x = pos[1].x , y = pos[1].y } } )

-- 	local intensifyProperty = {}

-- 	--属性显示
-- 	for i,v in ipairs(now.lst) do
-- 		local value = v.value - formerly.lst[i].value
-- 		if v.value == formerly.lst[i].value then
-- 			value = v.value
-- 		end
-- 		local type_ , info_ = Util:Nature( v.type , value , true )
-- 		local intensifyProperty = Util:Name( type_ .." ".. info_ , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
-- 		table.insert( tab.lst , { node = intensifyProperty , interval = interval , pos = { x = pos[i+1].x , y = pos[i+1].y } } )
-- 	end
	
--    	return tab
-- end
