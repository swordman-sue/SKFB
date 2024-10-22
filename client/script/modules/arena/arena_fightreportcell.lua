Modules = Modules or {}

Modules.TemplArenaFightReportCell = Modules.TemplArenaFightReportCell or BaseClass(GUI.ScrollItem)

function Modules.TemplArenaFightReportCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.ARENA_FIGHTREPORT_CELL)	
end

function Modules.TemplArenaFightReportCell:SetData(data)
	--回放按钮
	Util:WidgetButton(self:GetWidget("Button_replayVideo"),"",function ()
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_lookForwardTo"))
	end)
	--胜负
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ARENA, data.is_win == 0 and "failedflag" or "winflag",true)
	self:GetWidget("Image_resultFlag"):SetImage(sp) 
	--排名变化	
	local str
	if data.after_rank < data.before_rank then			
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips")	
		str = CPPGameLib.GetString("arena_rank_upto")	
		Util:WidgetLabel(self:GetWidget("Text_changeNum"), data.before_rank - data.after_rank,Macros.TypefaceSize.popup,Macros.Color.red)
	elseif data.after_rank == data.before_rank then	
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ARENA, "yellow_arrows")
		str = CPPGameLib.GetString("arena_rank_nochange")
		self:GetWidget("Text_changeNum"):SetVisible(false)
		self:GetWidget("Text_ranknum"):SetVisible(false)
	else		
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "red_arrow")	
		str = CPPGameLib.GetString("arena_rank_downto")
		Util:WidgetLabel(self:GetWidget("Text_changeNum"), data.after_rank - data.before_rank,Macros.TypefaceSize.popup,Macros.Color.red)
	end
	self:GetWidget("Image_resultArrows"):SetImage(sp) 
	Util:WidgetLabel(self:GetWidget("Text_rankText"),str,Macros.TypefaceSize.normal,Macros.Color.content)
	Util:WidgetLabel(self:GetWidget("Text_ranknum"),data.after_rank,Macros.TypefaceSize.largish,Macros.Color.red)		
	--对方名字	
	Util:WidgetLabel(self:GetWidget("Text_playerName"),data.target_name,Macros.TypefaceSize.largish,Macros.Color.headline)
	--等级
	Util:WidgetLabel(self:GetWidget("Text_leveltitle"),CPPGameLib.GetString("arena_leveltitle"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--对方等级	
	Util:WidgetLabel(self:GetWidget("Text_level"),data.target_level,Macros.TypefaceSize.normal,Macros.Color.content)
	--时间
	local text_time = self:GetWidget("Text_time")	
	Util:WidgetLabel(text_time,"",Macros.TypefaceSize.normal,Macros.Color.red)
	local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")	
	local min = math.floor((severTime - data.battle_time)/60)
	if min == 0 then
		min = 1
	end
	if min < 60 then
		text_time:SetText(CPPGameLib.GetString("friend_timeMinText",min))
	elseif min >=60 and min < 1440 then
		text_time:SetText(CPPGameLib.GetString("friend_timeHourText",math.floor(min/60)))
	elseif min >= 1440 then
		text_time:SetText(CPPGameLib.GetString("friend_timeDayText",math.floor(min/1440)))
	end
	--敌人头像框
	local IsRobot,config = ObjIDTool.GetArenaRobotData(data.target_id)
	local panel_enemyicon = self:GetWidget("Panel_enemyIcon")
	local enemyIcon = LogicGUI.RoleIcon.New(false)
	panel_enemyicon:AddChild(enemyIcon:GetNode())
	PosTool.Center(enemyIcon)
	enemyIcon:SetData(IsRobot and config.id or data.head_icon)
	--enemyIcon:adaptParentContentSize()
	--自己头像框
	local own_icon = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
	local panel_ownicon = self:GetWidget("Panel_ownIcon")
	local ownIcon = LogicGUI.RoleIcon.New(false)
	panel_ownicon:AddChild(ownIcon:GetNode())
	PosTool.Center(ownIcon)
	ownIcon:SetData(own_icon)
	--是否被攻击
	self:GetWidget("Image_attack"):SetVisible(data.is_attack == Macros.Global.TRUE)
	self:GetWidget("Image_defence"):SetVisible(data.is_attack == Macros.Global.FALSE)
end