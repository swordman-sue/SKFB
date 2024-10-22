GUI = GUI or {}

GUI.TemplArenaFightFiveTimesCell = GUI.TemplArenaFightFiveTimesCell or BaseClass(GUI.Template)

function GUI.TemplArenaFightFiveTimesCell:__init(index,enemy_icon,fb_type,target_id)
	self.__is_auto_load = true
	self.__open_callback = function()
		self:CreateView(index,enemy_icon,fb_type,target_id)
	end
	self:InitTemplate(GUI.TemplateRegistry.ARENA_FIGHT_FIVETIMES_CELL)
end

function GUI.TemplArenaFightFiveTimesCell:__AddToParent()
	
end

function GUI.TemplArenaFightFiveTimesCell:CreateView(index,enemy_icon,fb_type,target_id)
	print("ldx","index,enemy_icon,fb_type,target_id: ",index,enemy_icon,fb_type,target_id)
	--获取角色头像信息
	local role_icon_id = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
	--第几战
	Util:WidgetLabel(self:GetWidget("Text_times"), CPPGameLib.GetString("tower_battle",index), Macros.TypefaceSize.popup, Macros.Color.headline )
	--自己头像
	local panel_ownhead = self:GetWidget("Panel_ownHead") 
	local role_icon = LogicGUI.RoleIcon.New()
	panel_ownhead:AddChild(role_icon:GetNode())
	PosTool.Center(role_icon)
	role_icon:SetData(role_icon_id)
	--敌人头像
	local panel_enemyhead = self:GetWidget("Panel_enemyHead") 
	role_icon = LogicGUI.RoleIcon.New()
	panel_enemyhead:AddChild(role_icon:GetNode())
	PosTool.Center(role_icon)
	role_icon:SetData(enemy_icon)
	role_icon:GetNode():SetFlipX(true)
	--战斗中
	self.__text_onfighting = self:GetWidget("Text_onFighting")
	Util:WidgetLabel(self.__text_onfighting, CPPGameLib.GetString("arena_onfighting_text"), Macros.TypefaceSize.largish, Macros.Color.content )
	--特效容器
	self.__panel_fighteffect = self:GetWidget("Panel_fightEffect")
	--物品容器
	self.__panel_head = self:GetWidget("Panel_head")	
	--失败文字
	self.__image_falied = self:GetWidget("Image_falied")	
	--获得
	self.__text_reward_title = self:GetWidget("Text_reward_title")
	Util:WidgetLabel(self.__text_reward_title, CPPGameLib.GetString("arena_get_something"), Macros.TypefaceSize.largish, Macros.Color.keypoint )	
	--金币数
	self.__text_goldnum = self:GetWidget("Text_goldNum")
	Util:WidgetLabel(self.__text_goldnum,"", Macros.TypefaceSize.largish, Macros.Color.content )
	
	--经验数
	self.__text_expnum = self:GetWidget("Text_expNum")
	Util:WidgetLabel(self.__text_expnum,"", Macros.TypefaceSize.largish, Macros.Color.content )
	
	--声望数
	self.__text_reputationnum = self:GetWidget("Text_reputationNum")
	Util:WidgetLabel(self.__text_reputationnum,"", Macros.TypefaceSize.largish, Macros.Color.content )

	--self:SetPanelVisible(false)
	print("skfb","CPPSceneManager:GetDeltaSpeedTime()1111111111111111111111111111111: ",CPPSceneManager:GetDeltaSpeedTime())
	--播放请求前的动画
	self:PlayActionTimeline("animation_beforeReq", false,function ()
		--请求快速战斗
		CallModuleFunc(ModuleType.BATTLE, "StartQuickBattle", fb_type,target_id,Macros.Global.TRUE)
		--隐藏头像
		panel_ownhead:SetVisible(false)
		panel_enemyhead:SetVisible(false)		
	end)
end

function GUI.TemplArenaFightFiveTimesCell:RunEffect()
	--播放特效 
	local effect
	self.__effect_id,effect = self:__CreateEffect({res_name = "UI_xiaodao",time_scale = 2}, self.__panel_fighteffect)
	PosTool.Center(effect)
end

function GUI.TemplArenaFightFiveTimesCell:SetPanelVisible(is_visible)
	--隐藏货币
	self:GetWidget("Image_bg"):SetVisible(is_visible)
	self:GetWidget("Image_gold"):SetVisible(is_visible)
	self:GetWidget("Image_exp"):SetVisible(is_visible)
	self:GetWidget("Image_reputation"):SetVisible(is_visible)
	self.__text_reward_title:SetVisible(is_visible)	
	self.__text_goldnum:SetVisible(is_visible)
	self.__text_expnum:SetVisible(is_visible)
	self.__text_reputationnum:SetVisible(is_visible)
	self.__panel_head:SetVisible(is_visible)
	self.__image_falied:SetVisible(is_visible)
end

function GUI.TemplArenaFightFiveTimesCell:CallBackData(is_win, reward_map, card_data)
	print("ldx","is_win, reward_map, card_data: ",is_win)
	--self:SetPanelVisible(true)
	self.__text_onfighting:SetVisible(false)
	if is_win then
		local icon = LogicGUI.IconLayer.New()
		self.__panel_head:AddChild(icon:GetNode())
		PosTool.Center(icon)
		icon:SetData({id = card_data.card_id,num = card_data.card_num})
		icon:ShowItemName(true,100,0)
	end
	self.__text_goldnum:SetText(reward_map[Macros.Game.RolePropType.GOLD] or 0)
	self.__text_expnum:SetText(reward_map[Macros.Game.RolePropType.EXP] or 0)
	self.__text_reputationnum:SetText(reward_map[Macros.Game.RolePropType.ARENA_PRESTIGE] or 0)
	self:__DelEffect(self.__effect_id)
	self.__effect_id = nil
	--播放回应后的动画
	local animation_name = is_win and "animation_afterRespWin" or "animation_afterRespLose"
	self:PlayActionTimeline(animation_name, false,function ()
		self:FireNextFrame(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_CELL_RESP)
	end)
end