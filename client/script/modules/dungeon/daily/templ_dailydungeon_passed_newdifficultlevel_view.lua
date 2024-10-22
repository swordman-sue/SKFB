LogicGUI = LogicGUI or {}

LogicGUI.TemplDailyDungeonPassedNewDifficultLevel = LogicGUI.TemplDailyDungeonPassedNewDifficultLevel or BaseClass(GUI.Template)

function LogicGUI.TemplDailyDungeonPassedNewDifficultLevel:__init(fb_type,record_idx,fun)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:__InitUI(fb_type,record_idx,fun)
	end	
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_PASSED_NEW_DIFFICULTLEVEL)
end

function LogicGUI.TemplDailyDungeonPassedNewDifficultLevel:__InitUI(fb_type,record_idx,fun)
	local fb_info = CPPGameLib.GetConfig("DailyChapter",fb_type)
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local icon_type = ObjIDTool.GetInfo(fb_info.icon_id)
	local icon_path_func = LogicGUI.IconLayer.IconPathFuncMap[icon_type]
	local icon_path, icon_scale = icon_path_func(fb_info.icon_id)
	--适配
	local image = self:GetWidget("Image_6")
	image:IgnoreContentAdaptWithSize(true)
	--货币
	for i=1,2 do
		self:GetWidget("Image_icon"..i):IgnoreContentAdaptWithSize(true)
		self:GetWidget("Image_icon"..i):SetImage(icon_path,TextureResType.LOCAL)
	end
	--解锁
	if role_level < fb_info.gamelevel_limit_list[record_idx + 1] then
		--未解锁新难度
		Util:WidgetLabel(self:GetWidget("Text_lockText"),CPPGameLib.GetString("daliy_dungeon_unlock_newdifficult_level"),Macros.TypefaceSize.largish, Macros.Color.btn)
		--解锁难度
		self:GetWidget("Text_lockLevel"):SetVisible(false)
		--条件
		Util:WidgetLabel(self:GetWidget("Text_otherText"),CPPGameLib.GetString("daliy_dungeon_conditional_text"),Macros.TypefaceSize.largish, Macros.Color.btn)
		--开启等级
		Util:WidgetLabel(self:GetWidget("Text_otherDes"),CPPGameLib.GetString("daliy_dungeon_level_limit_opend",fb_info.gamelevel_limit_list[record_idx + 1]),Macros.TypefaceSize.largish, Macros.Color.red)
	else
		--解锁
		Util:WidgetLabel(self:GetWidget("Text_lockText"),CPPGameLib.GetString("daliy_dungeon_lock_text"),Macros.TypefaceSize.largish, Macros.Color.btn)
		--解锁难度
		Util:WidgetLabel(self:GetWidget("Text_lockLevel"),CPPGameLib.GetString("daliy_dungeon_diffcultlevel_"..(record_idx+1)),Macros.TypefaceSize.largish, Macros.Color.Special)
		--掉落
		Util:WidgetLabel(self:GetWidget("Text_otherText"),CPPGameLib.GetString("daliy_dungeon_drop_text"),Macros.TypefaceSize.largish, Macros.Color.btn)
		--数量增加
		Util:WidgetLabel(self:GetWidget("Text_otherDes"),CPPGameLib.GetString("daliy_dungeon_drop_item_add_text",fb_info.name),Macros.TypefaceSize.largish, Macros.Color.btn)
	end
	
	--首通
	Util:WidgetLabel(self:GetWidget("Text_firstPassText"),CPPGameLib.GetString("daliy_dungeon_firstpassed_text"),Macros.TypefaceSize.largish, Macros.Color.btn)
	--首通难度
	Util:WidgetLabel(self:GetWidget("Text_firstPassLevel"),CPPGameLib.GetString("daliy_dungeon_diffcultlevel_"..record_idx),Macros.TypefaceSize.largish, Macros.Color.btn)
	--奖励
	Util:WidgetLabel(self:GetWidget("Text_rewardText"),CPPGameLib.GetString("daliy_dungeon_reward_text"),Macros.TypefaceSize.largish, Macros.Color.btn)
	--挑战次数增加
	Util:WidgetLabel(self:GetWidget("Text_rewardDes"),CPPGameLib.GetString("daliy_dungeon_challengetimes_add_text"),Macros.TypefaceSize.largish, Macros.Color.Special)

	--点击屏幕继续
	local text_continute = self:GetWidget("Text_continute")
	Util:WidgetLabel(text_continute,CPPGameLib.GetString("common_continue"),Macros.TypefaceSize.popup, Macros.Color.blinkcontent,0,0,Macros.Color.common_stroke)

	--播放动画
	self:PlayActionTimeline("animation0", false,function ()
		text_continute:SetVisible(true)
		WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
			if fun then
				fun()
			end
			self:DeleteMe()
		end)
	end)
end