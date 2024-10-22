LogicGUI = LogicGUI or {}

LogicGUI.TemplCreateTheTeam = LogicGUI.TemplCreateTheTeam or BaseClass(GUI.Template)

function LogicGUI.TemplCreateTheTeam:__init(cur_team_id,cur_team_dungeon_id,member_data_list)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self.__cur_team_id = cur_team_id
		self.__cur_team_dungeon_id = cur_team_dungeon_id
		self:InitBaseUI(member_data_list)
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_CREATE_THE_TEAM)
end

function LogicGUI.TemplCreateTheTeam:__delete()
	--队友模型
	for i=1,Macros.Game.MAX_TEAMMATE_NUM do
		if self["__teammate_model_"..i] then
			self["__teammate_model_"..i]:DeleteMe()
			self["__teammate_model_"..i] = nil
		end
	end
end

function LogicGUI.TemplCreateTheTeam:__RegistAllEvents()
    --新成员加入队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_NEW_MEMBER_JOIN_TEAM, function(_,protocol_data)
        --队友位置初始化
		for i=1,Macros.Game.MAX_TEAMMATE_NUM do
			if not self.__team_member_data_list[i] then
				self.__team_member_data_list[i] = protocol_data
				break
			end
		end

		self.__btn_opengame:SetText(CPPGameLib.GetTableLength(self.__team_member_data_list) >= 4 and CPPGameLib.GetString("team_open_immediate_refresh") or CPPGameLib.GetString("team_open_immediate_match"))
		--初始化队员位置信息
		self:InitMemberPosData()
    end) 
    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function()
        self:DeleteMe()
    end)
    --离开队伍通知
    self:BindGlobalEvent(Macros.Event.TeamModule.LEAVE_THE_TEAM, function(_,protocol_data,is_timeout)
    	print("kx","Macros.Event.TeamModule.LEAVE_THE_TEAM ",is_timeout,debug.traceback())
    	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
    	if is_timeout then
    		--活动结束
    		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_over_activity_tips"))
    		self:DeleteMe()
    	elseif protocol_data.role_id == role_info.role_id and protocol_data.is_task_team == 0 then
	        self:DeleteMe()
	    elseif protocol_data.role_id ~= role_info.role_id and protocol_data.is_task_team == 0 then
	    	--队友位置初始化
			for i=1,Macros.Game.MAX_TEAMMATE_NUM do
				if self.__team_member_data_list[i] and self.__team_member_data_list[i].role_id == protocol_data.role_id then
					--提示
        			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_member_leave_the_team_tips",self.__team_member_data_list[i].role_name or ""))
					table.remove(self.__team_member_data_list,i)
					break
				end
			end
			self.__btn_opengame:SetText(CPPGameLib.GetTableLength(self.__team_member_data_list) >= 4 and CPPGameLib.GetString("team_open_immediate_refresh") or CPPGameLib.GetString("team_open_immediate_match"))
			--初始化队员位置信息
			self:InitMemberPosData()
	    end
    end)
end

function LogicGUI.TemplCreateTheTeam:InitBaseUI(member_data_list)
	--没有则返回
	if not self.__cur_team_id or not self.__cur_team_dungeon_id then
		return
	end
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"", function ()
		--确认弹框
        LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("team_leave_team_affirm_tips"),
                                        confirm_text = CPPGameLib.GetString("common_confirm"),
                                        confirm_func = function ()
                                            --离开队伍
                                            CallModuleFunc(ModuleType.TEAM,"TeamDungeonLeaveReq")
                                        end,cancel_text = CPPGameLib.GetString("common_cancel"),
                                        cancel_func = function ()
                                        end,})
		
	end)
	--副本配置
	local fb_config = CPPGameLib.GetConfig("TeamFBChapter",self.__cur_team_dungeon_id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("team_create_team_title",fb_config.dungeon_name,fb_config.recommend_level[1],fb_config.recommend_level[2]),Macros.TypefaceSize.tab, Macros.Color.btn)
	--等待队长信息
	self.__text_waitingteamleader = self:GetWidget("Text_WaitingTeamLeader")
	Util:WidgetLabel(self.__text_waitingteamleader,CPPGameLib.GetString("team_waiting_leader_matching"), Macros.TypefaceSize.popup, Macros.Color.Special)
	--开始游戏
	self.__btn_opengame = self:GetWidget("Button_OpenGame")	
	Util:WidgetButton(self.__btn_opengame, "", function ()
		if #self.__team_member_data_list >= 4 then
			--进入副本
			CallModuleFunc(ModuleType.TEAM,"TeamDungeonTaskStartReq")
		else
			--请求快速匹配
        	CallModuleFunc(ModuleType.TEAM,"TeamDungeonMatchReq",{dungeon_id = self.__cur_team_dungeon_id,is_team = Macros.Global.TRUE})
		end
	end, Macros.TypefaceSize.tab, Macros.Color.btn, Macros.Color.button_red)
	--队伍数据
	self.__team_member_data_list = member_data_list
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	if not member_data_list then
		self.__is_team_leader = true
		self.__team_member_data_list = {}		
		table.insert(self.__team_member_data_list,{role_id = role_info.role_id,head_icon = role_info.head_icon,role_name = role_info.role_name
													,power = role_info.battle_value,server_id = role_info.server_id})
	elseif member_data_list then
		self.__is_team_leader = self.__team_member_data_list[1].role_id == role_info.role_id
	end
	--初始化队员位置信息
	self:InitMemberPosData()
	--等待队长信息
	self.__text_waitingteamleader:SetVisible(not self.__is_team_leader and true or false)
	--开始游戏	
	self.__btn_opengame:SetVisible(self.__is_team_leader and true or false)
	self.__btn_opengame:SetText(#self.__team_member_data_list >= 4 and CPPGameLib.GetString("team_open_immediate_refresh") or CPPGameLib.GetString("team_open_immediate_match"))
end

--初始化队员位置信息
function LogicGUI.TemplCreateTheTeam:InitMemberPosData()
	--队友位置初始化
	for i=1,Macros.Game.MAX_TEAMMATE_NUM do
		--队员容器
		if not self["__panel_teammate"..i] then
			local panel_pos = self:GetWidget("Panel_pos"..i)
			self["__panel_teammate"..i] = WidgetUnity.GetWidgetByName(panel_pos, "Panel_TeamMate")
			self["__panel_teammate"..i]:SetZOrder(1)
			self["__button_add"..i] = WidgetUnity.GetWidgetByName(panel_pos, "Button_add")
			Util:WidgetButton(self["__button_add"..i],"",function ()
				--请求邀请
				CallModuleFunc(ModuleType.TEAM,"TeamDungeonRecommendListReq")
			end)
			local image_bg = WidgetUnity.GetWidgetByName(self["__panel_teammate"..i], "Image_23")
			image_bg:SetZOrder(2)
			--玩家名字
			self["__text_teammatename"..i] = WidgetUnity.GetWidgetByName(self["__panel_teammate"..i], "Text_TeamMateName")
			self["__text_teammatename"..i]:SetZOrder(2)
			Util:WidgetLabel(self["__text_teammatename"..i],"", Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
			--玩家战力
			self["__text_teammatebattlevalue"..i] = WidgetUnity.GetWidgetByName(self["__panel_teammate"..i], "Text_TeamMateBattleValue")
			self["__text_teammatebattlevalue"..i]:SetZOrder(2)
			Util:WidgetLabel(self["__text_teammatebattlevalue"..i],"", Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
			--光效环
			local effect
			self["__panel_light_effectid"..i],effect = self:__CreateEffect({res_name = "UI_zudui_fangjiandengguang",loop = true},self["__panel_teammate"..i],1)
			PosTool.CenterTop(effect,0,30)
			--特效环
			self["__panel_effectid"..i],effect = self:__CreateEffect({res_name = "UI_zudui_fangjianguangquan",loop = true},panel_pos,0)
			PosTool.CenterBottom(effect)			
		end
		--光效环
		if self["__panel_light_effectid"..i] then
			local effect = GlobalEffectMgr:GetEffect(self["__panel_light_effectid"..i])
			if effect then
				effect:SetVisible(self.__team_member_data_list[i] and true or false)
			end
		end
		--加按钮
		self["__button_add"..i]:SetVisible(not self.__team_member_data_list[i] and true or false)
		--容器
		self["__panel_teammate"..i]:SetVisible(self.__team_member_data_list[i] and true or false)

		--模型
		if not self["__teammate_model_"..i] and self.__team_member_data_list[i] then
			local hero_config = CPPGameLib.GetConfig("Hero",self.__team_member_data_list[i].head_icon or self.__team_member_data_list[i].head_img_id)
			--创建
			self["__teammate_model_"..i] = LogicGUI.Model.New({info_id = self.__team_member_data_list[i].head_icon or self.__team_member_data_list[i].head_img_id,parent = self["__panel_teammate"..i],scale = hero_config.ui_scale or 1,zorder = 0})
			self["__teammate_model_"..i]:SetPosition(80,0)
		elseif self.__team_member_data_list[i] and self["__teammate_model_"..i] then
			local hero_config = CPPGameLib.GetConfig("Hero",self.__team_member_data_list[i].head_icon or self.__team_member_data_list[i].head_img_id)
			--设置
			self["__teammate_model_"..i]:SetInfoID(self.__team_member_data_list[i].head_icon or self.__team_member_data_list[i].head_img_id)
			self["__teammate_model_"..i]:SetScaleFactor(hero_config.ui_scale or 1)
		elseif not self.__team_member_data_list[i] and self["__teammate_model_"..i] then
			--删除
			self["__teammate_model_"..i]:DeleteMe()
			self["__teammate_model_"..i] = nil
		end
		--名字
		print("ldx","self.__team_member_data_list[i] ",self.__team_member_data_list.role_name)
		self["__text_teammatename"..i]:SetText(CPPGameLib.GetString("team_player_name",self.__team_member_data_list[i] and self.__team_member_data_list[i].server_id or 0,self.__team_member_data_list[i] and self.__team_member_data_list[i].role_name or ""))
		--玩家战力
		self["__text_teammatebattlevalue"..i]:SetText(CPPGameLib.GetString("dungeon_power",self.__team_member_data_list[i] and self.__team_member_data_list[i].power or 0))
	end
end