--
-- @brief 助战界面
-- @auther: ldx
-- @date 2018年3月29日 18:17:36
--

Modules = Modules or {}
Modules.AssistView = Modules.AssistView or BaseClass(BaseView)

function Modules.AssistView:__init()
	self.__layout_name = "assist_in_fighting_view"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_assist")}
	self.__title_bar_params = {
		close_func = function ()
			self:CloseManual()		
		end,
		title = CPPGameLib.GetString("assist_title"),			
	}
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SIXTH_MAN)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SIXTH_MAN,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_ASSIST_IN_FIGHTING_CELL)
	self.__open_callback = function ()
		self:__OpenCallback()
	end
end

function Modules.AssistView:__OpenCallback()
	self.__params1,self.__params2,self.__params3,self.__help_battle_target_list,self.__today_used_help_battle_times,self.__func = unpack(self.__open_data)
	
	--角色信息
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.FRIEND_ASSIST)
	--次标题
	Util:WidgetLabel(self:GetWidget("Text_AssistTitle"),CPPGameLib.GetString("assist_sec_title"), Macros.TypefaceSize.tab, Macros.Color.Special)
	
	--今日可助战
	local txt_assistlimit = self:GetWidget("Text_assistLimit")
	txt_assistlimit:SetAnchorPoint(1,0.5)
	txt_assistlimit:SetPosition(170,261.5)
	Util:WidgetLabel(txt_assistlimit,CPPGameLib.GetString("assist_limit"), Macros.TypefaceSize.normal, Macros.Color.btn)
	txt_assistlimit:SetVisible(is_open)
	--次数
	self.__txt_assistlimitnum = self:GetWidget("Text_assistLimitNum")
	Util:WidgetLabel(self.__txt_assistlimitnum,ConfigAdapter.Common.GetToalAssistTimes() - self.__today_used_help_battle_times, Macros.TypefaceSize.normal, Macros.Color.btn)
	self.__txt_assistlimitnum:SetVisible(is_open)
	--陌生人助战提示
	local txt_strangertips = self:GetWidget("Text_StrangerTips")
	Util:WidgetLabel(txt_strangertips,CPPGameLib.GetString("dungeon_stranger_tips"), Macros.TypefaceSize.minimum, Macros.Color.Special)
	txt_strangertips:SetVisible(is_open)
	
	--友情点
	local txt_friendpoint = self:GetWidget("Text_friendPoint")
	Util:WidgetLabel(txt_friendpoint,CPPGameLib.GetString("dungeon_friend_point"), Macros.TypefaceSize.normal, Macros.Color.btn)
	txt_friendpoint:SetVisible(is_open)
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT)
	self:GetWidget("Image_friendpoint"):SetImage(sp)
	self:GetWidget("Image_friendpoint"):SetVisible(is_open)
	--友情点点数
	local txt_friendpointnum = self:GetWidget("Text_friendPointNum")
	Util:WidgetLabel(txt_friendpointnum,CPPGameLib.GetString("common_add9",role_info.total_friendship_point), Macros.TypefaceSize.slightly, Macros.Color.Special)
	txt_friendpointnum:SetVisible(is_open)
	--友情点上限
	local txt_friendpointlimit = self:GetWidget("Text_friendPointLimit")
	Util:WidgetLabel(txt_friendpointlimit,CPPGameLib.GetString("assist_limit_friendpoint",role_info.today_friendship_point,ConfigAdapter.Common.GetDaliyFriendPointLimit()), Macros.TypefaceSize.normal, Macros.Color.btn)
	txt_friendpointlimit:SetVisible(is_open)
	--线
	self:GetWidget("Image_7"):SetVisible(is_open)

	--刷新按钮	
	local btn_refrash = self:GetWidget("Button_Refrash")
	btn_refrash:SetVisible(is_open)
	btn_refrash:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 1)
	WidgetUnity.SetWidgetListener(btn_refrash,nil, nil, function(x,y,in_cd)
		if in_cd then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("assist_refrash_tips"))
			return
		end
		if self.__friend_message_scrollview then
			--请求刷新
			CallModuleFunc(ModuleType.BATTLE,"BattleAssistTargetReq",nil,nil,nil,nil,true)
		end
	end, nil, 2 * 1000,nil,true)

	self.__hero_model_list = {}
	local lineup_hero_list = CallModuleFunc(ModuleType.BATTLE, "GetLineupHeroList")
	for lineup_index, lineup_info in ipairs(lineup_hero_list) do		
		local panel_lineup = self:GetWidget("Panel_Lineup"..lineup_index)
		local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", lineup_info.lineup_index)
		--阵位配置
		local pos_info = ConfigAdapter.Hero.GetLineupPosInfo(lineup_info.lineup_index)
		--阵位开放
		self:GetWidget("Image_LineupLock"..lineup_index):SetVisible(not lineup_info.is_open)		
		if hero_info then
			local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
			local hero_model = LogicGUI.Model.New({info_id = hero_info.info_id,parent = panel_lineup})
			hero_model:SetScaleFactor(config_hero.battle_scale or 1)
			hero_model:SetPosition(50,0)
			table.insert(self.__hero_model_list,hero_model)
		else
			local text_locklevel = self:GetWidget("Text_LineupOpenLevel"..lineup_index)			
			Util:WidgetLabel(text_locklevel,CPPGameLib.GetString("dungeon_assist_open_level",pos_info.need_level), Macros.TypefaceSize.popup, Macros.Color.btn, 80, 70, Macros.Color.button_yellow)
		end
	end
	--推荐等级
	--获取关卡id
	local gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(self.__params2, self.__params3, self.__params1)
	local gamelevel_info = CPPGameLib.GetConfig("GameLevel",gamelevel_id,false)
	self.__cur_recommended_level = gamelevel_info and gamelevel_info.recommended_level or 0
	local txt_recommendlevel = self:GetWidget("Text_RecommendLevel")	
	Util:WidgetLabel(txt_recommendlevel,CPPGameLib.GetString("dungeon_gamelevel_recommend_level",self.__cur_recommended_level), Macros.TypefaceSize.largish, role_info.level >= self.__cur_recommended_level and Macros.Color.btn or Macros.Color.red,0,0,
					role_info.level >= self.__cur_recommended_level and Macros.Color.lilyBlack_stroke or Macros.Color.red_stroke)
	txt_recommendlevel:SetVisible(gamelevel_info and gamelevel_info.recommended_level and true or false)

	--出战按钮
	local btn_fight = self:GetWidget("Button_fight")
	self.__fightting_effect_id, effect = self:__CreateEffect({res_name = "UI_xiaodao",scale = 0.8}, btn_fight,2)
	effect:SetAnchorPoint(0.5,0.5)
	PosTool.LeftCenter(effect,75,15)
	Util:WidgetButton(btn_fight,"",function ()
		--判断等级是否满足推荐等级
		local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		if role_level < self.__cur_recommended_level then
			--确认弹框
	        LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("dungeon_gamelevel_recommend_content",self.__cur_recommended_level),
	                                        confirm_text = CPPGameLib.GetString("common_confirm"),
	                                        confirm_func = function ()
	                                            --精灵信息
												self:HandleBattleMessage()
	                                        end,cancel_text = CPPGameLib.GetString("common_cancel"),
	                                        cancel_func = function ()
	                                        end,})
			return
		end
		--精灵信息
		self:HandleBattleMessage()					
	end)
	local _, btn_fight_effect = self:__CreateEffect({res_name = "UI_zhiyinshouzhi"}, btn_fight, 10)
	PosTool.Center(btn_fight_effect, -18)	

	--指引容器
	self.__panel_lead = self:GetWidget("Panel_lead")
	WidgetUnity.SetWidgetListener(self.__panel_lead, nil, nil,function ()
		--删除特效
		if self.__assist_lead_effectid then
			self:__DelEffect(self.__assist_lead_effectid)
			self.__assist_lead_effectid = nil
		end
		self.__panel_lead:SetVisible(false)  
		--添加手指软指引
		self.__lead_player_attack_effectid, effect = self:__CreateEffect({res_name = "UI_zhiyinshouzhi"}, btn_fight,3)
		effect:SetAnchorPoint(0.5,0.5)
		PosTool.LeftCenter(effect,75,-5)
		--助战特殊处理
		CallModuleFunc(ModuleType.LEAD,"FriendAssistSpHandle")
	end)
	self.__panel_lead:SetVisible(false)
	--指引提示
	Util:WidgetLabel(self:GetWidget("Text_LeadTips"),CPPGameLib.GetString("hero_recruit_touchscreen_2"), Macros.TypefaceSize.popup, Macros.Color.btn,0,0,Macros.Color.common_stroke)

	--初始化滚动容器
	self:__InitFriendMessageScrollView()

	--初始化精灵
	self:__InitSpiritMessage()

	--回调
	if self.__func then
		self.__func()
	end	
end

function Modules.AssistView:__Dispose()
	--移除模型
	for _,hero_model in ipairs(self.__hero_model_list) do
		hero_model:DeleteMe()
	end
	self.__hero_model_list = {}

	--移除容器
	if self.__friend_message_scrollview then
		self.__friend_message_scrollview:DeleteMe()
		self.__friend_message_scrollview = nil
	end

	if self.__spirit_model then
		self.__spirit_model:DeleteMe()
		self.__spirit_model = nil
	end

	--删除特效
	if self.__assist_lead_effectid then
		self:__DelEffect(self.__assist_lead_effectid)
		self.__assist_lead_effectid = nil
	end

	if self.__lead_player_attack_effectid then
		self:__DelEffect(self.__lead_player_attack_effectid)
		self.__lead_player_attack_effectid = nil
	end
end

function Modules.AssistView:HandleBattleMessage()
	if self.__selected_target_id then
		CallModuleFunc(ModuleType.DUNGEON,"SetAssistRoleID",self.__selected_target_id,self.__target_type,self.__friend_role_name)
	end

	--精灵技能
	local spirit_common_skill_id
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID", self.__on_fight_spiritid)
	if spirit_info then
		spirit_common_skill_id = CallModuleFunc(ModuleType.SPIRIT, "GetCommonKillId")
	end
	self:CallModuleFunc("HandleGameLevelChallenge", self.__params2, self.__params3, self.__params1, self.__selected_target_id, self.__on_fight_spiritid, spirit_common_skill_id)
end

function Modules.AssistView:__InitFriendMessageScrollView()
	local is_shield_friend = true
	--判断系统是否开放
	local txt_unopenedtips = self:GetWidget("Text_unopenedtips")
	local img_unopenedtips = self:GetWidget("Image_unopenedtips")
	--检查是否是屏蔽助战的副本类型
	local tips_str1
	local could_use_assist_list = ConfigAdapter.Common.GetCouldUseAssistFriendDungeonTypeList()
	for i,fb_type in ipairs(could_use_assist_list) do
		if fb_type == self.__params1 then
			is_shield_friend = false
			break
		end
	end
	local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.FRIEND_ASSIST)
	local is_open_scrollview = is_open and not is_shield_friend and true or false
	if is_open_scrollview then
		--先构造数据
		local select_index = self:__InitAssistData()			
		--好友信息
		local panel_scrollview = self:GetWidget("Panel_ScrollView")
		local params = {
			item_class = Modules.TemplDungeonAssistInFightingCell,
		    item_width = 175,
		    item_height	= 262,
		    row	= 1,
		    col	= 5,
		    item_space_c = 1,
		    horizon = true,
		    view_width = 880,
		    view_height = 262,
		    selectable = true,
		    select_callback	= function (item_index,item_data)
		    	if item_data then
		    		if not self.__shield_seleceted_tips then
		    			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("assist_assist_help_tips"..item_data.target_type))
		    		end
		    		self.__shield_seleceted_tips = false
		    		self.__selected_target_id = item_data.id
		    		self.__target_type = item_data.target_type
		    		self.__friend_role_name = item_data.name
		    	end
		    end,
		    select_filter_func = function (item_index,item_data)
		    	--次数用完
		    	if ConfigAdapter.Common.GetToalAssistTimes() - self.__today_used_help_battle_times <= 0 then					
					if item_data.target_type ~= Macros.Game.ASSIST_TAGGET.STRANGER then
						return false
					end
				end
		    	if item_data.target_type == Macros.Game.ASSIST_TAGGET.GUILD then
		    		--军团
		    		if CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.GUILD_ID) == 0 then
		    			return false
		    		end		    		
		    	end
		    	return item_data.id and true or false
		    end,
    		select_filter_func_callback = function (item_index,item_data)
    			--次数用完
		    	if ConfigAdapter.Common.GetToalAssistTimes() - self.__today_used_help_battle_times <= 0 then					
					if item_data.target_type ~= Macros.Game.ASSIST_TAGGET.STRANGER then
						GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("assist_today_assist_times_used_tips"))
						return
					end
				end
    			if item_data.target_type == Macros.Game.ASSIST_TAGGET.GUILD then
		    		--军团
		    		if CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.GUILD_ID) == 0 then
		    			local spirit_system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.UNION)		    			
		    			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("assist_join_guild_tips",spirit_system_info.level))
		    			return
		    		end		    		
		    	end
		    	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("assist_no_assist_tips"..item_data.target_type))
    		end,
		}
		self.__friend_message_scrollview = GUI.ScrollView.New(params)
		panel_scrollview:AddChild(self.__friend_message_scrollview:GetNode())
		PosTool.Center(self.__friend_message_scrollview)
		self.__friend_message_scrollview:SetDataList(self.__assist_init_data)
		--选择
		if select_index then
			self.__friend_message_scrollview:SelectItem(select_index)
		end		
	else
		local other_tips = (self.__params1 == Macros.Game.FBType.TOWER_COMMON or self.__params1 == Macros.Game.FBType.TOWER_ELITE) and CPPGameLib.GetString("dungeon_assist_tower_ban") or 
							(self.__params1 == Macros.Game.FBType.HERO_DUNGEON and CPPGameLib.GetString("dungeon_assist_herodungeon_ban") or "")
		local tips_str2 = not is_open and CPPGameLib.GetString("dungeon_assist_unopend",open_level) or other_tips
		Util:WidgetLabel(txt_unopenedtips,tips_str2, Macros.TypefaceSize.headlineII , Macros.Color.keypoint,250,80)
	end
	--判断系统是否开放
	txt_unopenedtips:SetPosition(txt_unopenedtips:GetPositionX() - 30,txt_unopenedtips:GetPositionY()+15)
	txt_unopenedtips:SetVisible(not is_open_scrollview)
	img_unopenedtips:SetVisible(not is_open_scrollview)
end

--初始化精灵相关信息
function Modules.AssistView:__InitSpiritMessage()
	--精灵数据
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")	
	local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT)
	--开放等级
	self:GetWidget("Image_SpiritLock"):SetVisible(not is_open)
	Util:WidgetLabel(self:GetWidget("Text_SpiritOpenLevel"),CPPGameLib.GetString("dungeon_assist_open_level",open_level or 0), Macros.TypefaceSize.popup, Macros.Color.btn, 70, 70, Macros.Color.button_yellow)
	--提示
	local txt_spirittips = self:GetWidget("Text_SpiritTips")
	Util:WidgetLabel(txt_spirittips,CPPGameLib.GetString("assist_touch_spirit_tips"), Macros.TypefaceSize.minimum, Macros.Color.btn)
	txt_spirittips:SetVisible(is_open)
	--点击
	local panel_spirit = self:GetWidget("Panel_Spirit")
	WidgetUnity.SetWidgetListener(panel_spirit, nil, nil,function ()
		--检查等级
		if not is_open then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_spirit_open_level",open_level))
			return
		end
		--打开选择精灵界面
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_SET , false )
	end)
	--创建精灵	
	self.__on_fight_spiritid = #spirit_info.spirit_list <= 0 and 50001 or spirit_info.cur_use_spirit
	self.__spirit_model = LogicGUI.Model.New({info_id = self.__on_fight_spiritid,parent = panel_spirit,model_type = LogicGUI.ModelType.SPRITE})		
	self.__spirit_model:SetScaleFactor(1)
	self.__spirit_model:SetPosition(68,50)
end

function Modules.AssistView:__RegistAllEvents()
	--精灵出战回调
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_ON_FIGHT_RESP, function(_,spirit_id)
		--请求挑战关卡
		-- local all_data_list = {self.__selected_target_id,self.__on_fight_spiritid,self.__cur_common_skillid}
		-- local count_index = 0
		-- for i=1,10 do
		-- 	if self["__params"..i] then
		-- 		count_index = count_index + 1
		-- 		table.insert(all_data_list,count_index,self["__params"..i])
		-- 	end 
		-- end
		self.__on_fight_spiritid = spirit_id
		self.__spirit_model:SetInfoID(self.__on_fight_spiritid)
	end)

	--刷新助战列表
	self:BindGlobalEvent(Macros.Event.DungeonModule.REFRASH_ASSIST_LIST, function(_,help_battle_target_list)		
		if self.__friend_message_scrollview then
			--刷新提示
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("assist_refrash_succ_tips"))
			self.__help_battle_target_list = help_battle_target_list
			--先构造数据
			local select_index = self:__InitAssistData()
			self.__friend_message_scrollview:SetDataList(self.__assist_init_data)	
			--屏蔽选中提示
			self.__shield_seleceted_tips = true		
			--选择
			if select_index then
				self.__friend_message_scrollview:SelectItem(select_index)
			end	
		end
	end)
end

function Modules.AssistView:__InitAssistData()
	self.__assist_init_data = {}	
	local assist_init_data = {}
	for i=1,5 do
		--军团1 好友2 陌生人2
		table.insert(assist_init_data,{target_type = i <= 1 and Macros.Game.ASSIST_TAGGET.GUILD 
			or (i <= 3 and Macros.Game.ASSIST_TAGGET.FRIEND or Macros.Game.ASSIST_TAGGET.STRANGER)})
	end
	
	local select_index
	--赋值
	for index,init_data in ipairs(assist_init_data) do
		local copy_data
		--目标
		for _,friend_info in ipairs(self.__help_battle_target_list) do
			if friend_info.target_type == init_data.target_type and not friend_info.is_add then
				copy_data = CPPGameLib.CopyTblShallow(friend_info)
				friend_info.is_add = true
				if ConfigAdapter.Common.GetToalAssistTimes() - self.__today_used_help_battle_times <= 0 then					
					if friend_info.target_type == Macros.Game.ASSIST_TAGGET.STRANGER then
						select_index = select_index or index
					end
				else
					select_index = select_index or index
				end
				break
			end
		end
		copy_data = copy_data or CPPGameLib.CopyTblShallow(init_data)
		table.insert(self.__assist_init_data,copy_data)
	end
	return select_index
end
