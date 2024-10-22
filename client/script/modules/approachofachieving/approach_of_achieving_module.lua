Modules = Modules or {}

Modules.ApproachOfAchievingModule = Modules.ApproachOfAchievingModule or BaseClass(BaseModule)

function Modules.ApproachOfAchievingModule:__init()
	-- self:__ParseAndSaveMainDungeonData()
end

function Modules.ApproachOfAchievingModule:__delete()
	-- body
end

--解析主线副本掉落数据
function Modules.ApproachOfAchievingModule:__ParseMainDungeonDropObjData()
	self.__ParseDungeonDataList = {}
	local dungeon_drop_list = {CPPGameLib.GetConfig("MainFuben",nil,nil,true),CPPGameLib.GetConfig("EliteFBChapter",nil,nil,true)}
	for i,dungeon_config in ipairs(dungeon_drop_list) do
		for chapter_index,chapter_info in ipairs(dungeon_config) do
			--print("ldx","chapter_index: ",chapter_index)
			for gamelevel_index,gamelist_id in ipairs(chapter_info.gamelevel_list) do
				local gamelist_info = CPPGameLib.GetConfig("GameLevel",gamelist_id)
				if gamelist_info.drop_obj_id_list then
					for i,obj_id in ipairs(gamelist_info.drop_obj_id_list) do
						self.__ParseDungeonDataList[obj_id] = self.__ParseDungeonDataList[obj_id] or {}
						local info = {}
						info.chapter_index = chapter_index
						info.gamelevel_index = gamelevel_index
						info.approach_des = "["..chapter_info.name.."-"..(gamelist_info.name or CPPGameLib.GetString("dungeon_normal_gamelevel")).."]"..CPPGameLib.GetString("approach_drop")
						info.gamelevel_battle_times_max = gamelist_info.battle_times_max
						table.insert(self.__ParseDungeonDataList[obj_id],info)
					end
				end
			end
		end
	end		
end

--获取主线副本掉落数据
function Modules.ApproachOfAchievingModule:__GetMainDungeonDropObjDataByObjId(obj_id)
	if not self.__ParseDungeonDataList then
		self:__ParseMainDungeonDropObjData()
	end
	local ss = CPPGameLib.GetTableStr(self.__ParseDungeonDataList,"self.__ParseDungeonDataList: ")
	-- LogTool.LogInfo(ss)
	print("ldx",obj_id,"obj_id: !")
	--CPPGameLib.PrintTable("ldx",)
	--CPPGameLib.PrintTable("ldx",self.__ParseDungeonDataList[obj_id],"self.__ParseDungeonDataList[obj_id]")
	return self.__ParseDungeonDataList[obj_id]
end

function  Modules.ApproachOfAchievingModule:__InsertApproachInfo(approach_list,item_info)
	if item_info and item_info.systemid then
		for i,approach_id in ipairs(item_info.systemid) do
			if not approach_list[approach_id] then
				approach_list[approach_id] = approach_id
			end
		end
	end
	return approach_list
end

--获取途径列表
function Modules.ApproachOfAchievingModule:__GetApproachList(info_id)
	local approach_list = {}
	local pieceid,pieceInfo
	local item_type,item_info = ObjIDTool.GetInfo(info_id)	
	approach_list = self:__InsertApproachInfo(approach_list,item_info)
	--特殊处理
	if item_type == Macros.Game.ObjType.HERO then
		pieceInfo = CPPGameLib.GetConfig("HeroCompound",info_id,false)		
	elseif item_type == Macros.Game.ObjType.EQUIP then
		pieceInfo = CPPGameLib.GetConfig("EquipmentCompose",info_id,false)
	elseif item_type == Macros.Game.ObjType.TREASURE then
		pieceInfo = CPPGameLib.GetConfig("TreasureCompose",info_id,false)
	else
		
	end
	if pieceInfo then
		item_type,item_info = ObjIDTool.GetInfo(pieceInfo.item_id)	
		approach_list = self:__InsertApproachInfo(approach_list,item_info)
		pieceid = pieceInfo.item_id or pieceInfo.cost_item_list[1]
	end
	return approach_list,pieceid
end

--处理途径数据
function Modules.ApproachOfAchievingModule:HandleApproachOfAchievingData(info_id)
	--途径信息
	if not info_id then
		return
	end
	print("ldx","info_idinfo_idinfo_idHandleApproachOfAchievingData ",info_id)
	local approach_list,pieceid = self:__GetApproachList(info_id)
	local handler_approach_of_achieving_list = {}
	for _,approach_id in pairs(approach_list) do
		local approach_info = CPPGameLib.GetConfig("ApproachOfAchieving",approach_id)
		--主线副本
		if approach_id == Macros.Game.SystemID.DUNGEON_GAMELEVEL or approach_id == Macros.Game.SystemID.DUNGEON_ELITE then
			local maindungeon_dropobj_datalist 
			local fb_type = approach_id == Macros.Game.SystemID.DUNGEON_GAMELEVEL and Macros.Game.FBType.MAIN_DUNGEON or 
								Macros.Game.FBType.ELITE_DUNGEON
			--整装的途径信息
			local info_list = self:__GetMainDungeonDropObjDataByObjId(info_id)
			--碎片的途径信息
			if pieceid then
				piece_list = self:__GetMainDungeonDropObjDataByObjId(pieceid)
				if piece_list and info_list then
					maindungeon_dropobj_datalist = CPPGameLib.MergeTable2List(info_list, piece_list)
				else
					maindungeon_dropobj_datalist = info_list and info_list or piece_list
				end
			else
				maindungeon_dropobj_datalist = info_list
			end
			if maindungeon_dropobj_datalist then
				for i,gamelevel_info in ipairs(maindungeon_dropobj_datalist) do
					local info = {}
					info.approach_name = approach_info.approach_name
					info.approach_icon_res_name = approach_info.approachIcon_id
					info.approach_systemid = approach_info.systemid
					info.approach_des = gamelevel_info.approach_des				
					info.extra_view_params = {gamelevel_info.chapter_index,gamelevel_info.gamelevel_index}
					info.approach_id = approach_id
					info.system_type = approach_info.system_type
					local dungeon_gamelevel_info = CallModuleFunc(ModuleType.DUNGEON,"GetGameLevelInfo",gamelevel_info.chapter_index,gamelevel_info.gamelevel_index,fb_type)
					info.isOpen = dungeon_gamelevel_info ~= nil 
					local gamelevel_battle_times_max = gamelevel_info.gamelevel_battle_times_max
					local battle_times = gamelevel_battle_times_max - (dungeon_gamelevel_info and dungeon_gamelevel_info.battle_times or 0)					
					info.limit_times_str = CPPGameLib.GetString("approach_limit_times",battle_times,gamelevel_battle_times_max)
					table.insert(handler_approach_of_achieving_list,info)
				end
			end
		else
			local info = {}
			info.approach_name = approach_info.approach_name
			info.approach_icon_res_name = approach_info.approachIcon_id
			info.approach_systemid = approach_info.systemid
			info.approach_des = approach_info.approach_des
			info.approach_id = approach_id
			info.system_type = approach_info.system_type
			info.isOpen = GlobalModulesMgr:IsOpen(approach_info.systemid)	
			if approach_id == Macros.Game.SystemID.TOWER_SHOP then
				info.extra_view_params = {info_id}
			elseif approach_id == Macros.Game.SystemID.PLUNDEREDTREASURES then				
				--夺宝特殊处理
				--获取物品类型和信息表
				local item_type,item_info = ObjIDTool.GetInfo(info_id)	
				if item_type == Macros.Game.ObjType.TREASURE or item_type == Macros.Game.ObjType.ITEM and item_info.sub_type == Macros.Game.NormalItemSubType.TREASURE_DEBRIS then
					--圣器或者圣器碎片
					if item_type == Macros.Game.ObjType.ITEM and tem_info and tem_info.sub_type == Macros.Game.NormalItemSubType.TREASURE_DEBRIS then
						item_type,item_info = ObjIDTool.GetInfo(item_info.target_id)
					end
					if item_info then
						info.extra_view_params = {0,false,item_info.type}
					end
				end
			elseif approach_id == Macros.Game.APPROACH_SPECIAL_TYPE.MAIN_DUNGEON_ALL_GAMELEVEL then
				info.isOpen = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.DUNGEON_GAMELEVEL)	
			elseif approach_id == Macros.Game.APPROACH_SPECIAL_TYPE.HERO_DUNGEON_ALL_GAMELEVEL then
				info.isOpen = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL)	
			elseif approach_id == Macros.Game.APPROACH_SPECIAL_TYPE.ELITE_DUNGEON_ALL_GAMELEVEL then
				info.isOpen = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.DUNGEON_ELITE)	
			elseif approach_id == Macros.Game.SystemID.QUEEN then
				info.isOpen = CallModuleFunc(ModuleType.QUEEN , "GetRetainTerm")
			end			
			table.insert(handler_approach_of_achieving_list,info)		
		end
	end
	CPPGameLib.PrintTable("ldx",handler_approach_of_achieving_list,"handler_approach_of_achieving_list: ")
	--途径排序
	table.sort(handler_approach_of_achieving_list,function (approach_info_1,approach_info_2)
		if (approach_info_1.isOpen and approach_info_2.isOpen) or (not approach_info_1.isOpen and not approach_info_2.isOpen) then
			if approach_info_1.system_type == approach_info_2.system_type then
				if approach_info_1.approach_id == approach_info_2.approach_id then
					if approach_info_1.extra_view_params and approach_info_2.extra_view_params then
						if approach_info_1.extra_view_params[1] == approach_info_2.extra_view_params[1] then
							return approach_info_1.extra_view_params[2] < approach_info_2.extra_view_params[2]
						else
							return approach_info_1.extra_view_params[1] < approach_info_2.extra_view_params[1]
						end
					else
						return true
					end
				else
					return approach_info_1.approach_id < approach_info_2.approach_id
				end
			else
				return approach_info_1.system_type > approach_info_2.system_type
			end
		elseif approach_info_1.isOpen and not approach_info_2.isOpen then
			return true
		elseif not approach_info_1.isOpen and approach_info_2.isOpen then
			return false
		else
		end
	end)
	return handler_approach_of_achieving_list
end