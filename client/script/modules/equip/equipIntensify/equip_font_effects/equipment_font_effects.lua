
--[[
%% @module: 装备强化字体特效
%% @author: yjg
%% @created: 2016年12月20日22:04:17
--]]

LogicGUI = LogicGUI or {}

LogicGUI.EquipmentIntensifyFontEffects = LogicGUI.EquipmentIntensifyFontEffects or BaseClass()

-- 层 数据 要不要加上装备大师 移动位置
function LogicGUI.EquipmentIntensifyFontEffects:__init( parent_node , params , movex , movey ,fun)
end

function LogicGUI.EquipmentIntensifyFontEffects:__delete()
end

function LogicGUI.EquipmentIntensifyFontEffects:GetEquipmentIntensifyTab( params , pos )
	self.key = false
	local t1 = self:Intensify( params , pos )
	local t3 = {
			[1] = t1}

	return t3
end

function LogicGUI.EquipmentIntensifyFontEffects:Intensify( params , pos )
   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
   	--图片的间距是 10
   	local interval = 0

	--强化提示字
	local texe = ""
	if params.crit_times ~= 0 then
		if self.key == false then
			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "strengthen_double" , true)
			local strengthen_double = Util:Sprite(sp)
			interval = 15
			table.insert( tab.lst , { node = strengthen_double , interval = interval } )
		end
		-- "强化 %d 次,暴击 %d 次,等级提升 %d"
		texe = CPPGameLib.GetString("equip_qianghuabaoji" , params.type , params.crit_times , params.type + params.crit_times ) 
	else
		-- "强化 %d 次,等级提升 %d"
		texe = CPPGameLib.GetString("equip_qianghua" , params.type , params.type ) 
	end

	interval = 5
	local intensifyText = Util:Name(texe , Macros.TypefaceSize.tab)
	table.insert( tab.lst , { node = intensifyText , interval = interval , pos = { x = pos[1].x , y = pos[1].y }  } )

	--获得这个装备的信息
	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip" , params.equipment_uid)
    -- CPPGameLib.PrintTable(equipInfo,"equipInfo")
	--获得装备的属性
	local now , formerly , nex = CallModuleFunc(ModuleType.EQUIP, "GetStrengthenPropPlusMap" , params.equipment_uid)
	
	--属性显示
	local intensifyPropertyText = nil
	if now.lst[1].value == formerly.lst[1].value then
		intensifyPropertyText = Util:Nature( now.lst[1].type , ( now.lst[1].value ) * ( params.type + params.crit_times ) , nil , nil , true )
	else
		intensifyPropertyText = Util:Nature( now.lst[1].type , ( now.lst[1].value - formerly.lst[1].value) * ( params.type + params.crit_times ) , nil , nil , true )
	end

	local intensifyProperty = Util:Name(intensifyPropertyText , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
	interval = 5
	table.insert( tab.lst , { node = intensifyProperty , interval = interval , pos = { x = pos[2].x , y = pos[2].y } } )

	return tab
end


--[[
%% @module: 装备精炼
%% @author: yjg
%% @created: 2016年12月20日22:04:17
--]]

LogicGUI = LogicGUI or {}

LogicGUI.EquipmentRefineFontEffects = LogicGUI.EquipmentRefineFontEffects or BaseClass(UILayer)

-- 层 数据 要不要加上装备大师 移动位置
function LogicGUI.EquipmentRefineFontEffects:__init( )
end

function LogicGUI.EquipmentRefineFontEffects:__delete()
end

function LogicGUI.EquipmentRefineFontEffects:GetEquipmentRefineTab( params , pos )	
	local t1 = self:Intensify( params , pos )
	local t3 = {
			[1] = t1 }
	return t3
end

function LogicGUI.EquipmentRefineFontEffects:Intensify( params , pos )
   	local tab = {}
   	tab.lst = {}
   	tab.type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION
   	--图片的间距是 10
   	local interval = 0

	--获得这个装备的信息
	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip" , params.equipment_uid)
	local type_ , equipInfoLst = ObjIDTool.GetInfo(equipInfo.info_id)

	local texe = CPPGameLib.GetString("equip_refineSucceed" , equipInfoLst.name , params.refine_level  ) 

	local intensifyText = Util:Name(texe , Macros.TypefaceSize.tab )
	interval = 5
	table.insert( tab.lst , { node = intensifyText , interval = interval , pos = { x = pos[1].x , y = pos[1].y } } )

	--获得装备的属性
	local now , formerly , nex  = CallModuleFunc(ModuleType.EQUIP, "GetRefinePropPlusMap",equipInfo.uid)
	local intensifyProperty = {}
	for i,v in ipairs(now.lst) do

		local intensifyPropertyText = nil
		if v.value == formerly.lst[i].value then
			intensifyPropertyText = Util:Nature( v.type , (v.value) , nil , nil , true )
		else
			intensifyPropertyText = Util:Nature( v.type , (v.value - formerly.lst[i].value) , nil , nil , true )
		end

		intensifyProperty[i] = Util:Name(intensifyPropertyText , Macros.TypefaceSize.popup , Macros.Game.QualityType.GREEN )
		table.insert( tab.lst , { node = intensifyProperty[i] , interval = interval , pos = { x = pos[i + 1].x , y = pos[i + 1].y } } )
	end
	return tab
end
