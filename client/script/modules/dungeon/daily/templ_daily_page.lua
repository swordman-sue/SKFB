--[[
--日常副本挑战
]]--
GUI = GUI or {}
GUI.TemplDailyDungeonPage = GUI.TemplDailyDungeonPage or BaseClass(GUI.Template)

function GUI.TemplDailyDungeonPage:__init(fb_type,cur_diffcultLevel)
	self.__cur_fb_type = fb_type
	self.__cur_diffcult_level = cur_diffcultLevel
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_DAILY_DIFFICULTLEVEL)
	self:__ShowUI()
end

function GUI.TemplDailyDungeonPage:__delete()
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end
end

function GUI.TemplDailyDungeonPage:__AddToParent()
	
end

function GUI.TemplDailyDungeonPage:__ShowUI()
	--日常副本信息
	local fb_info = CPPGameLib.GetConfig("DailyChapter",self.__cur_fb_type)
	--英雄	
	--CPPGameLib.PrintTable("ldx",fb_info,"fb_info")
    local gamelevel_info = CPPGameLib.GetConfig("GameLevel",fb_info.gamelevel_list[self.__cur_diffcult_level])
	local monster_info = CPPGameLib.GetConfig("Monster",gamelevel_info.icon_id)
	local panel_heromodule = self:GetWidget("Panel_heroModule")
	-- local hero_id = monster_info.hero_id and monster_info.hero_id or monster_info.model_id
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end
	if not self.__hero_module then
		self.__hero_module = LogicGUI.Model.New({info_id = gamelevel_info.icon_id,parent = panel_heromodule,model_type = LogicGUI.ModelType.MONSTER,dir = MathTool.HORZ_LEFT})		
		self.__hero_module:SetPosition(2.5,10)
	else
		self.__hero_module:SetInfoID(gamelevel_info.icon_id)
	end
	self.__hero_module:SetScaleFactor(fb_info.model_size)
	--根据条件显示对应的文字
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON,"GetChapterInfo",self.__cur_fb_type,Macros.Game.FBType.DAILY_DUNGEON)
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local str,color,stroke_color	
	if chapter_info.__map_idx_record >= self.__cur_diffcult_level then
		--已通关
		str = CPPGameLib.GetString("daliy_dungeon_have_opend")
		color = Macros.Color.green 
		stroke_color = Macros.Color.green_stroke
	else
		color = Macros.Color.red 
		stroke_color = Macros.Color.red_stroke
		if fb_info.gamelevel_limit_list[self.__cur_diffcult_level] > role_level then
			--战队等级开启
			str = CPPGameLib.GetString("daliy_dungeon_level_limit_opend",fb_info.gamelevel_limit_list[self.__cur_diffcult_level])			
		else
			if self.__cur_diffcult_level == (chapter_info.__map_idx_record + 1) then
				--推荐战力
				color = Macros.Color.golden 
				stroke_color = Macros.Color.common_stroke				
				-- local battle_info = CPPGameLib.GetConfig("Battle",gamelevel_info.battle_list[1])
				-- local monster_battle_value = 0
				-- for s=1,5 do
				-- 	if battle_info["moster_"..s] ~= 0 then
				-- 		monster_battle_value =  monster_battle_value + CallModuleFunc(ModuleType.POWER, "CalcMonsterPower", battle_info["moster_"..s])
				-- 	end
				-- end
				str = CPPGameLib.GetString("daliy_dungeon_recommend_power",gamelevel_info.recommended_power) 
			else
				--通关前置条件
				local diff_str = CPPGameLib.GetString("daliy_dungeon_diffcultlevel_"..(self.__cur_diffcult_level - 1))
				str = CPPGameLib.GetString("daliy_dungeon_diffcultlevel_limit_opend",diff_str)
			end
		end
	end
	Util:WidgetLabel(self:GetWidget("Text_intraduce"), str, 26, color, 0, 0, stroke_color)
	--模式
	local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_DAILY, "difficulty_level_"..self.__cur_diffcult_level, true)
	local img_difficultlevel = self:GetWidget("Image_difficultlevel")
	img_difficultlevel:SetImage(image_data)
end

function GUI.TemplDailyDungeonPage:ResetData(fb_type,cur_diffcultLevel)
	self.__cur_fb_type = fb_type
	self.__cur_diffcult_level = cur_diffcultLevel
	self:__ShowUI()
end

