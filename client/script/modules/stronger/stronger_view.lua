--[[
%% @module: 我要变强界面
%% @author: ldx
%% @created: 2017-06-21
--]]


Modules = Modules or {} 


Modules.StrongerView = Modules.StrongerView or BaseClass(BaseView)

function Modules.StrongerView:__init()
	self.__layout_name = "stronger"	
    self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
  	 	self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STRONGER)    
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STRONGER,true) 
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_STRONGER_SYSTEM_CELL) 
end

function Modules.StrongerView:__OpenCallback( )

	CallModuleFunc(ModuleType.STRONGER , "SetisFirstOpen" , false )

	local tab_index,is_weak_first = unpack(self.__open_data)
	tab_index = tab_index or 1
    --关闭按钮  
    Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
        self:CloseManual()
    end)
    		
    --已上阵英雄队列
    local panel_scrollview = self:GetWidget("Panel_scrollview")
    local params = {
    	item_class = Modules.StrongerViewLineupItem,
	    item_width = 100,
	    item_height	= 80,
	    row	= 5,
	    col	= 1,
	    item_space_r = 9,
	    view_width  = 100,
	    view_height = 448,
	    selectable = true,
	    select_callback	= function (index,data)
	    	self.__selected_index = index
	    	self:__SelectedHeroCallBack(data)
	    end
	}
	self.__hero_lineup_scrollview = GUI.ScrollView.New(params)
	panel_scrollview:AddChild(self.__hero_lineup_scrollview:GetNode())
	PosTool.Center(self.__hero_lineup_scrollview)
	--上阵英雄
	local list = {}
	local cur_battle_value
	local lineup_list = CallModuleFunc(ModuleType.HERO,"GetLineupList")
	for i, lineup_info in ipairs(lineup_list) do
		if lineup_info.uid ~= 0 then
			local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",lineup_info.uid)
			local data = {}
			data = CPPGameLib.CopyTbl(hero_info)
			data.lineup_index = i
			if is_weak_first then
				--战力
			    local hero_detail_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", i)
			    local battle_value = CallModuleFunc(ModuleType.POWER, "CalcMonsterPowerEx",hero_detail_info)
			   	if not cur_battle_value or (cur_battle_value and battle_value < cur_battle_value) then
			   		tab_index = i
			   		cur_battle_value = battle_value
			   	end
			end
			table.insert(list, data)
		end
	end
	self.__hero_lineup_scrollview:SetDataList(list)
    --标题
    Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("stronger_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
    --英雄名字
    self.__text_heroname = self:GetWidget("Text_heroName")
    Util:WidgetLabel(self.__text_heroname,"",Macros.TypefaceSize.popup, Macros.Color.btn,0,0,Macros.Color.common_stroke)
    --战力进度条
    self.__loading_bar = self:GetWidget("LoadingBar")
    --英雄战力
    self.__panel_battlevalue = self:GetWidget("Panel_battleValue")
	--推荐战力文字
	Util:WidgetLabel(self:GetWidget("Text_upgradeSpace"),CPPGameLib.GetString("stronger_upgradespace"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--推荐战力数字
	self.__text_upgradespacenum = self:GetWidget("Text_upgradeSpaceNum")
	Util:WidgetLabel(self.__text_upgradespacenum,"", Macros.TypefaceSize.largish, Macros.Color.keypoint )
	--提升空间队列
	self.__panel_stronglist = self:GetWidget("Panel_strongList")
	self.__grid_type_map = 
	{
		["weapon_equipment_uid"] = Macros.Game.EquipType.WEAPON,
		["hat_equipment_uid"] = Macros.Game.EquipType.HELMET,
		["belt_equipment_uid"] = Macros.Game.EquipType.BELT,
		["clothes_equipment_uid"] = Macros.Game.EquipType.CLOTH,
		["attack_treasure_uid"] = Macros.Game.TreasureType.EMBLEM,
		["defence_treasure_uid"] = Macros.Game.TreasureType.GAD,
	}
	--默认第一人
	self.__hero_lineup_scrollview:SelectItem(tab_index)
end	

function Modules.StrongerView:__RegistAllEvents()    
	
end

--获取打开参数
function Modules.StrongerView:GetOpenParams()
	return {self.__selected_index}
end

function Modules.StrongerView:__Dispose()  
	if self.__hero_lineup_scrollview then
		self.__hero_lineup_scrollview:DeleteMe()
		self.__hero_lineup_scrollview = nil
	end
	if self.__recommend_system_scrollview then
		self.__recommend_system_scrollview:DeleteMe()
		self.__recommend_system_scrollview = nil
	end
	if self.__arrow_effect_id then
		self:__DelEffect(self.__arrow_effect_id)
		self.__arrow_effect_id = nil
	end
end

function Modules.StrongerView:__SelectedHeroCallBack(data)  
	--删除软指引特效
	if self.__arrow_effect_id then
		self:__DelEffect(self.__arrow_effect_id)
		self.__arrow_effect_id = nil
	end
	self.__cur_selecetd_data = data
	--配置英雄信息
	local hero_config = CPPGameLib.GetConfig("Hero",self.__cur_selecetd_data.info_id)
	--角色等级
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	--英雄队列里阵位的装备信息
	local tbl_all,tbl_equip,tbl_treasure,tbl_special_equip,tbl_special_treasure = CallModuleFunc(ModuleType.HERO,"GetLineupEquipInfo",self.__cur_selecetd_data.lineup_index)
	local cur_equip_grid_type,cur_equip_uid,cur_treasure_grid_type,cur_treasure_uid
	for _,grid_type in pairs(self.__grid_type_map) do
		if not cur_equip_grid_type and tbl_special_equip[grid_type] then
			cur_equip_grid_type = grid_type
			cur_equip_uid = tbl_special_equip[grid_type].uid
		end
		if not cur_treasure_grid_type and tbl_special_treasure[grid_type] then
			cur_treasure_grid_type = grid_type
			cur_treasure_uid = tbl_special_treasure[grid_type].uid
		end
	end
	--配置推荐我要变强信息
	local stronger_re_config = CPPGameLib.GetConfig("StrongerRecommended",role_level)
	local recommended_system_list = {}
	for i,recommended_id in ipairs(stronger_re_config.recommended_id) do
		recommended_system_list[recommended_id] = recommended_id
	end	
	--英雄名字
	local hero_name = self.__cur_selecetd_data.break_level > 0 and CPPGameLib.GetString("common_add6",hero_config.name,self.__cur_selecetd_data.break_level) 
						or hero_config.name
    self.__text_heroname:SetText(hero_name)
    self.__text_heroname:SetColor(Macros.QualityColor(self.__cur_selecetd_data.quality_level))
    self.__text_heroname:SetStrokeColor(Macros.QualityColorStroke(self.__cur_selecetd_data.quality_level))   
    --战力
    local hero_detail_info = CallModuleFunc(ModuleType.BATTLE,"GetBattleHeroInfo",self.__cur_selecetd_data.lineup_index,true,true)
    local battle_value = CallModuleFunc(ModuleType.POWER, "CalcMonsterPowerEx",hero_detail_info)
    if not self.__battle_value then
    	self.__battle_value = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true))
	    self.__battle_value:SetAnchorPoint(0, 0.5)  
	    self.__panel_battlevalue:AddChild(self.__battle_value:GetNode())	    	    	
	    PosTool.LeftCenter(self.__battle_value)
    end
    self.__battle_value:SetText(tostring(battle_value))
    --推荐战力
    self.__text_upgradespacenum:SetText( Util:NumTransform(stronger_re_config and  stronger_re_config.recommended_power or 0 ) ) 
    --构造推荐系统队列
    local recommended_list = {}
    local total_value = 0
    local total_limit_value = 0
    for recommended_id,info in ipairs(CPPGameLib.GetConfig("Stronger", nil, nil , true )) do
    	if self.__cur_selecetd_data.quality_level >= info.open_quality and role_level >= info.open_level then
	    	local data = {}
	    	data = CPPGameLib.CopyTbl(info)
	    	data.uid = self.__cur_selecetd_data.uid
	    	data.info_id = self.__cur_selecetd_data.info_id
	    	data.lineup_index = self.__cur_selecetd_data.lineup_index
	    	data.sort_index = recommended_id
	    	data.is_recommend = recommended_system_list[recommended_id] and true or false
	    	data.link_txt = CPPGameLib.GetString("stronger_link_level")
	    	--各项进度
	    	local cur_value,limit_value
	    	if info.system_id == Macros.Game.SystemID.HERO_UPGRADE then
	    		--升级
	    		cur_value = self.__cur_selecetd_data.level
	    		limit_value = role_level	    		
	    	elseif info.system_id == Macros.Game.SystemID.HERO_BREAK then
	    		data.link_txt = CPPGameLib.GetString("stronger_link_break_level")
	    		--进阶
	    		cur_value = self.__cur_selecetd_data.break_level
	    		local break_info
	    		for i=0,table.maxn(CPPGameLib.GetConfig("HeroBreakLevelup",nil,nil,true)) do
	    			local cur_info = CPPGameLib.GetConfig("HeroBreakLevelup",i)
	    			if cur_info.need_hero_level > role_level then
	    				break
	    			end
	    			break_info = cur_info
	    		end
	    		limit_value = break_info and (break_info.break_level + 1) or 0
	    		limit_value = math.min(limit_value,table.maxn(CPPGameLib.GetConfig("HeroBreakLevelup",nil,nil,true)))
	    	elseif info.system_id == Macros.Game.SystemID.HERO_TRAIN then
	    		--培养
	    		local train_least_level
	    		for i,train_info in ipairs(self.__cur_selecetd_data.train_pro_list) do
	    			train_least_level = train_least_level or train_info.pro_level
	    			if train_least_level > train_info.pro_level then
	    				train_least_level = train_info.pro_level
	    			end
	    		end	   
	    		cur_value = train_least_level	
	    		local train_config = CPPGameLib.GetConfig("HeroTrain",1)
	    		local train_info
	    		for i=0,table.maxn(train_config.pro_level_list) do
	    			local cur_info = train_config.pro_level_list[i]
	    			if cur_info.need_hero_level > role_level then
	    				break
	    			end
	    			train_info = cur_info
	    		end
	    		limit_value = train_info and (train_info.pro_level + 1) or 0
	    		limit_value = math.min(limit_value,table.maxn(train_config.pro_level_list))
	    	elseif info.system_id == Macros.Game.SystemID.HERO_POTENTIAL then
	    		--潜能
	    		cur_value = self.__cur_selecetd_data.potential_level	
	    		local potential_info
	    		for i=0,table.maxn(CPPGameLib.GetConfig("HeroDestiny",nil,nil,true)) do
	    			local cur_info = CPPGameLib.GetConfig("HeroDestiny",i)
	    			if cur_info.need_level > role_level then
	    				break
	    			end
	    			potential_info = cur_info
	    		end
	    		limit_value = potential_info and (potential_info.level + 1) or 0
	    		limit_value = math.min(limit_value,table.maxn(CPPGameLib.GetConfig("HeroDestiny",nil,nil,true)))
	    	elseif info.system_id == Macros.Game.SystemID.HERO_AWAKEN then
	    		data.link_txt = CPPGameLib.GetString("stronger_link_break_level")
	    		--觉醒
	    		local special_id = hero_config.init_quality * 10 + hero_config.profession
				local awaken_config = CPPGameLib.GetConfig("HeroAwakening", special_id)
				local outside_break
				local awaken_star = 0
				local awaken_level = 0
				for star=0,Macros.Game.HERO_AWAKE_STAR_MAX do
					for level=0,Macros.Game.HERO_AWAKE_LEVEL_MAX do
						if awaken_config[star][level] then
							if awaken_config[star][level].need_hero_level > role_level then
								outside_break = true
								break
							end
							awaken_star = star
							awaken_level = level
						end
					end
					if outside_break then
						break
					end
				end
				cur_value = self.__cur_selecetd_data.awaken_star* 10 + self.__cur_selecetd_data.awaken_level
				limit_value = awaken_star* 10 + awaken_level
	    	elseif info.system_id == Macros.Game.SystemID.EQUIP_INTENSIFY then
	    		--装备强化
	    		data.cur_equip_grid_type = cur_equip_grid_type
	    		data.cur_equip_uid = cur_equip_uid
	    		local equip_num = 0
	    		local total_strengthen_level = 0
	    		for _,equip_info in pairs(tbl_equip) do
	    			equip_num = equip_num + 1
	    			total_strengthen_level = total_strengthen_level + equip_info.strengthen_level
	    		end
	    		cur_value = math.floor(total_strengthen_level/equip_num)    		
	    		limit_value = role_level*2
	    	elseif info.system_id == Macros.Game.SystemID.EQUIP_REFINE then
	    		--装备精炼
	    		data.cur_equip_grid_type = cur_equip_grid_type
	    		data.cur_equip_uid = cur_equip_uid
	    		local equip_num = 0
	    		local total_refine_level = 0
	    		for _,equip_info in pairs(tbl_equip) do
	    			equip_num = equip_num + 1
	    			total_refine_level = total_refine_level or 0
	    			total_refine_level = total_refine_level + equip_info.refine_level
	    		end
	    		cur_value = math.floor(total_refine_level/equip_num) 
	    		local config_equip_info = CPPGameLib.GetConfig("EquipmentRefine",1)
	    		local equip_refine_info
	    		for i=0,table.maxn(config_equip_info.level_list) do
	    			local cur_info = config_equip_info.level_list[i]
	    			if cur_info.need_level > role_level then
	    				break
	    			end
	    			equip_refine_info = cur_info
	    		end
	    		limit_value = equip_refine_info and (equip_refine_info.refine_level + 1) or 0
	    		limit_value = math.min(limit_value,table.maxn(config_equip_info.level_list))
	    	elseif info.system_id == Macros.Game.SystemID.TREASURE_INTENSIFY then
	    		--圣器强化
	    		data.cur_treasure_grid_type = cur_treasure_grid_type
	    		data.cur_treasure_uid = cur_treasure_uid
	    		local total_strengthen_level = 0
	    		for _,treasure_info in pairs(tbl_treasure) do
	    			total_strengthen_level = total_strengthen_level or 0
	    			total_strengthen_level = total_strengthen_level + treasure_info.strengthen_level
	    		end
	    		cur_value = math.floor(total_strengthen_level/2) 	
	    		local config_treasure_info = CPPGameLib.GetConfig("TreasureStrengthen",2)
	    		local treaure_strengthen_info
	    		for i,cur_info in ipairs(config_treasure_info.level_list) do
		  			if cur_info.need_level > role_level then
		  				break
		  			end
		  			treaure_strengthen_info = cur_info
		  		end  		
	    		limit_value = treaure_strengthen_info and (treaure_strengthen_info.strengthen_level + 1) or 0
	    		limit_value = math.min(limit_value,#config_treasure_info.level_list)
	    	elseif info.system_id == Macros.Game.SystemID.TREASURE_REFINE then
	    		--圣器精炼
	    		data.cur_treasure_grid_type = cur_treasure_grid_type
	    		data.cur_treasure_uid = cur_treasure_uid
	    		local total_refine_level = 0
	    		for _,treasure_info in pairs(tbl_treasure) do
	    			total_refine_level = total_refine_level or 0
	    			total_refine_level = total_refine_level + treasure_info.refine_level
	    		end
	    		cur_value = math.floor(total_refine_level/2) 	
	    		local config_treasure_info = CPPGameLib.GetConfig("TreasureRefine",2)
	    		local treaure_refine_info
	    		for i=0,table.maxn(config_treasure_info.level_list) do
	    			local cur_info = config_treasure_info.level_list[i]
	    			if cur_info.need_level > role_level then
	    				break
	    			end
	    			treaure_refine_info = cur_info
	    		end 		
	    		limit_value = treaure_refine_info and (treaure_refine_info.refine_level + 1) or 0
	    		limit_value = math.min(limit_value,table.maxn(config_treasure_info.level_list))
	    	elseif info.system_id == Macros.Game.SystemID.SPIRIT_ENHANCE then
                --精灵强化
                cur_value = 0 
                limit_value = 0
                local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
                if spirit_data.cur_use_spirit ~= 0 then
                    if spirit_data.spirit_info_map[spirit_data.cur_use_spirit] then
                        cur_value = spirit_data.spirit_info_map[spirit_data.cur_use_spirit].enhance_level
                    end
                    local config_spirit_info = CPPGameLib.GetConfig("SpiritEnhace",spirit_data.cur_use_spirit)
                    local spirit_enhance_info
                    for i=0,table.maxn(config_spirit_info.enhance_list) do
                        local cur_info = config_spirit_info.enhance_list[i]
                        if cur_info.need_level > role_level then
                            break
                        end
                        spirit_enhance_info = cur_info
                    end
                    limit_value = spirit_enhance_info.enhance_level
                end               
            elseif info.system_id == Macros.Game.SystemID.SPIRIT_SUBLIMATE then
                --精灵升华
                cur_value = 0 
                limit_value = 1
                local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
                if spirit_data.cur_use_spirit ~= 0 then
                    if spirit_data.spirit_info_map[spirit_data.cur_use_spirit] then
                        cur_value = spirit_data.spirit_info_map[spirit_data.cur_use_spirit].train_stage
                    end
                    local config_spirit_enhance_info = CPPGameLib.GetConfig("SpiritEnhace",spirit_data.cur_use_spirit)
                    local spirit_enhance_info
                    for i=0,table.maxn(config_spirit_enhance_info.enhance_list) do
                        local cur_info = config_spirit_enhance_info.enhance_list[i]
                        if cur_info.need_level > role_level then
                            break
                        end
                        spirit_enhance_info = cur_info
                    end
                    local limit_enhance_level = spirit_enhance_info.enhance_level
                    local config_spirit_sublimate_info = CPPGameLib.GetConfig("SpriteTrain",spirit_data.cur_use_spirit)
                    local spirit_sublimate_info
                    for i=1,table.maxn(config_spirit_sublimate_info.stage_list) do
                        local cur_info = config_spirit_sublimate_info.stage_list[i]
                        if cur_info.need_enhance_level > limit_enhance_level then
                            break
                        end
                        spirit_sublimate_info = cur_info
                    end
                    limit_value = spirit_sublimate_info.stage
                end
	    	end
	    	data.cur_value = cur_value
    		data.limit_value = limit_value
    		if data.cur_value >= data.limit_value then
    			data.is_recommend = false 
    		end   		
    		data.percent = limit_value == 0 and 0 or (cur_value/limit_value)*100
    		total_value = total_value or 0
    		total_limit_value = total_limit_value or 0
    		total_value = total_value + cur_value
    		total_limit_value = total_limit_value + limit_value
	    	table.insert(recommended_list,data)
	    end
    end
    --进度条
    if total_limit_value == 0 then
    	self.__loading_bar:SetPercent(0)
    else
    	self.__loading_bar:SetPercent((total_value/total_limit_value)*100)
    end   
    --排序
    table.sort(recommended_list,function (data1,data2)
    	if data1.is_recommend and data2.is_recommend then
    		if data1.percent ~= data2.percent then
    			return data1.percent < data2.percent
	    	else
	    		return data1.sort_index < data2.sort_index
	    	end
    	elseif data1.is_recommend and not data2.is_recommend then
    		return true
    	elseif not data1.is_recommend and data2.is_recommend then
    		return false
    	else
    		if data1.percent ~= data2.percent then
    			return data1.percent < data2.percent
	    	else
	    		return data1.sort_index < data2.sort_index
	    	end
    	end
    end)
    --系统队列
    if not self.__recommend_system_scrollview then
	    local params = {
	    	item_class = Modules.TemplStrongerSystemCell,
		    item_width = 298,
		    item_height	= 105,
		    row	= 2,
		    col	= 2,
		    item_space_r = 10,
		    item_space_c = 20,
		    view_height = 316,
		    touchable = true,
		    touch_callback	= function (index,data)
		    	if data.system_id == Macros.Game.SystemID.HERO_UPGRADE or data.system_id == Macros.Game.SystemID.HERO_BREAK 
		    		or data.system_id == Macros.Game.SystemID.HERO_TRAIN or data.system_id == Macros.Game.SystemID.HERO_POTENTIAL 
		    		or data.system_id == Macros.Game.SystemID.HERO_AWAKEN then
		    		GlobalModulesMgr:OpenSystemModule(data.system_id, {data.uid})
		    	elseif data.system_id == Macros.Game.SystemID.EQUIP_INTENSIFY or 
	    			data.system_id == Macros.Game.SystemID.EQUIP_REFINE then
	    			if not data.cur_equip_grid_type then
	    				GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_LINEUP,{BaseViewSubType.HeroLineup.MAIN,data.lineup_index,Macros.Game.HeroLineup.HERO_LINEUP,1})
	    				-- GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("stronger_equip_no"))
	    				return
	    			end
		    		local need_data = 
					{
						lineup_index = data.lineup_index,
						item_type = data.cur_equip_grid_type,
						equip_uid = data.cur_equip_uid,
						btn = true,
						hero_id = data.info_id
					}
					CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY,data.system_id == Macros.Game.SystemID.EQUIP_INTENSIFY and 2 or 3, need_data)
		    	elseif data.system_id == Macros.Game.SystemID.TREASURE_INTENSIFY 
	    			or data.system_id == Macros.Game.SystemID.TREASURE_REFINE then
	    			if not data.cur_treasure_grid_type then
	    				GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_LINEUP,{BaseViewSubType.HeroLineup.MAIN,data.lineup_index,Macros.Game.HeroLineup.HERO_LINEUP,2})
	    				--GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("stronger_treasure_no"))
	    				return
	    			end
		    		local need_data = 
					{
						lineup_index = data.lineup_index,
						item_type = data.cur_treasure_grid_type,
						treasure_uid = data.cur_treasure_uid,
						btn = true,
						hero_id = data.info_id
					}
					CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURE_LIFT,data.system_id == Macros.Game.SystemID.TREASURE_INTENSIFY and 2 or 3, need_data)
		    	elseif data.system_id == Macros.Game.SystemID.SPIRIT_ENHANCE or 
	    			data.system_id == Macros.Game.SystemID.SPIRIT_SUBLIMATE then
	    			GlobalModulesMgr:OpenSystemModule(data.system_id,{data.system_id == Macros.Game.SystemID.SPIRIT_ENHANCE and 2 or 3})
		    	end
		    end
		}
		self.__recommend_system_scrollview = GUI.ScrollView.New(params)
		self.__panel_stronglist:AddChild(self.__recommend_system_scrollview:GetNode())
		PosTool.Center(self.__recommend_system_scrollview)
   	end
   	self.__recommend_system_scrollview:SetDataList(recommended_list)
   	--我要变强软指引
   	if role_level <= ConfigAdapter.Common.GetStrongerWeakLeadRoleLevel() then
   		local item = self.__recommend_system_scrollview:GetItem(1)
   		if item then
	   		local arrow_effect
			--去提升
			local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "hero_updatetips")
			self.__upgrade_tips = SpriteEx.CreateWithImageFile(imgdata)
			self.__upgrade_tips:SetAnchorPoint(1,0.5)
			item:GetNode():AddChild(self.__upgrade_tips,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
			PosTool.RightCenter(self.__upgrade_tips,20)
			self.__upgrade_string = Util:Label(CPPGameLib.GetString("stronger_go_for_promote"),Macros.TypefaceSize.normal,nil,0,0,Macros.Color.common_stroke)
			self.__upgrade_tips:AddChild(self.__upgrade_string)
			PosTool.Center(self.__upgrade_string,0,5)				
			self.__funId = GlobalCallbackMgr:AddCallBackFunc(function ()
				ActionManager.GetInstance():RunJumpByAction(self.__upgrade_tips,0.8,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
					self:RunAddAction()
				end))	
			end)
			self:RunAddAction()
		end		
   	end 
end

function Modules.StrongerView:RunAddAction()
	ActionManager.GetInstance():RunJumpByAction(self.__upgrade_tips,0.8,0,20,0,0,self.__funId)
end

--------------------------------------------------------------------------------------------
--英雄列表条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.StrongerViewLineupItem = Modules.StrongerViewLineupItem or BaseClass(GUI.ScrollItem)

function Modules.StrongerViewLineupItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.StrongerViewLineupItem:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(100,80)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.StrongerViewLineupItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	--头像
	if not self.__icon then
		self.__icon = LogicGUI.IconLayer.New(false,false)
	    self.__layout:AddChild(self.__icon:GetNode())
	    self.__icon:SetAnchorPoint(0,0.5)
	    PosTool.LeftCenter(self.__icon)  
	end   
    self.__icon:SetData({id = self.__data.info_id,obj_info = self.__data})
end

function Modules.StrongerViewLineupItem:SetSelected(var)
	if var then
		if not self.__selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW,"bg_sp9_selected")
			self.__selected_image = Util:Sprite(sp)
			self.__icon:AddChild(self.__selected_image,3)
			PosTool.Center(self.__selected_image)
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"glow_arrows")
			local sprite = Util:Sprite(sp)
			self.__selected_image:AddChild(sprite)
			PosTool.RightCenter(sprite,10,0)
		end
	else
		if self.__selected_image then
			self.__selected_image:RemoveSelf(true)
			self.__selected_image = nil
		end
	end	
end