
--[[
%% @module: 怪物攻城
%% @author: ldx
%% @created: 2017-07-01
--]]

Modules = Modules or {}

Modules.MonsterStormCastleView = Modules.MonsterStormCastleView or BaseClass(BaseView)

function Modules.MonsterStormCastleView:__init()
	self.__layout_name = "monster_storm_castle"
	self.__title_bar_params = {
		close_func = function ()
			if self.__boss_reward_red_dot:IsVisible() then
				LogicGUI.TemplAffirmTips.New( { 
					type = LogicGUI.TemplAffirmTipsType.YES_NO,
					content	= CPPGameLib.GetString("monster_stormcastle_boss_reward_tips"),
					cancel_text = CPPGameLib.GetString("monster_stormcastle_boss_reward_cancel_txt") ,
					confirm_text = CPPGameLib.GetString("monster_stormcastle_boss_reward_sure_txt"),
					confirm_func = function ()
						Modules.TemplMonsterStormCastleRewardView.New(self.__player_info.from_chapter,self.__cur_chapter,self.__player_info.clear_up_boss_reward_record_list,self.__cur_boss_uid)
					end,
					cancel_func = function ()
						self.__is_close_func = true
						self:CallModuleFunc("AttackCityLeaveReq")
						self:CloseManual()	
					end,
					close_func = function ()
						
					end,
						} )

			else
				self.__is_close_func = true
				self:CallModuleFunc("AttackCityLeaveReq")
				self:CloseManual()	
			end						
		end,
		title = CPPGameLib.GetString("monster_stormcastle_title"),
		show_power = true,
		show_gold = true,
		show_diamond = true,				
	}
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MONSTER_STORMCASTLE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MONSTER_STORMCASTLE,true)
	self.__open_callback = function ()		
		self:__OpenCallback()		

		local is_need = self:CallModuleFunc("GetIsNeedReqData")
		if is_need then			
			self:CallModuleFunc("AttackCityEnterReq")
		else
			self.__player_list,self.__monster_list,self.__treasure_list,self.__player_info,self.__cur_chapter,self.__rank_list,self.__own_rank_info = self:CallModuleFunc("GetAllList")			
			self:__InitPlayerModuleAndPosition()
			self:__InitMonsterModuleAndPosition()
			self:__InitResurgenceMessage()
			self:__InitTreasureBoxAndPosition()			
			self:__RefrashRankList()
			self:UpdateOwnIntergralAndRank()			
		end
		--设置是否需要重新请求数据
		self:CallModuleFunc("SetIsNeedReqData",true)
	end
end

function Modules.MonsterStormCastleView:__Dispose()

	--设置是否需要重新请求数据
	if not self.__is_close_func then
		self:CallModuleFunc("SetIsNeedReqData",false)
	end

	if self.liaotian then
		self.liaotian:DeleteMe()
		self.liaotian = nil
	end

	if self.__monster_hp_bar then
		self.__monster_hp_bar:DeleteMe()
		self.__monster_hp_bar = nil
	end

	for role_id,hero_module in pairs(self.__hero_module_list) do
		hero_module:DeleteMe()
		self.__hero_module_list[role_id] = nil
	end

	for role_id,spirit_module in pairs(self.__spirit_module_list) do
		spirit_module:DeleteMe()
		self.__spirit_module_list[role_id] = nil
	end

	for uid,monster_module in pairs(self.__monster_module_list) do
		monster_module:DeleteMe()
		self.__monster_module_list[uid] = nil
	end
	
	if self.__activity_over_timer then
		GlobalTimerQuest:CancelQuest(self.__activity_over_timer)
		self.__activity_over_timer = nil
	end

	if self.__open_box_effectid then
		self:__DelEffect(self.__open_box_effectid)
		self.__open_box_effectid = nil
	end

	if self.__open_box_timer then
		GlobalTimerQuest:CancelQuest(self.__open_box_timer)
		self.__open_box_timer = nil
	end

	if self.__resurgence_count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__resurgence_count_down_timer)
		self.__resurgence_count_down_timer = nil
	end	

	if self.__boss_dead_refrash_count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__boss_dead_refrash_count_down_timer)
		self.__boss_dead_refrash_count_down_timer = nil
	end	

	if self.__boss_reward_count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__boss_reward_count_down_timer)
		self.__boss_reward_count_down_timer = nil
	end
end

function Modules.MonsterStormCastleView:__OpenCallback()	
	--设置定时器
	self:SetUpdateFunc(function ()
		if self.__update_move then
			self:__MovingBottomLayer()
		end
		if self.__update_hero_zorder then
			if self.__hero_module_list and self.__hero_module_list[self.__moving_hero_role_id] then
				local model = self.__hero_module_list[self.__moving_hero_role_id]:GetModel()
				local cur_posx,cur_posy = model:GetPosition()
				self.__hero_module_list[self.__moving_hero_role_id]:SetZOrder(1001 - cur_posy)
				if self.__spirit_module_list and self.__spirit_module_list[self.__moving_hero_role_id] then
					self.__spirit_module_list[self.__moving_hero_role_id]:SetZOrder(1001 - cur_posy)
				end
			end
		end
	end)
	self.__own_role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleID")
	--怪物Boss信息
	self.__panel_enemy = self:GetWidget("Panel_Enemy")
	--怪物Boss名字
	local panel_enemyname = self:GetWidget("Panel_EnemyName")
	self.__monster_name = Util:RichText("", Macros.TypefaceSize.largish)
	panel_enemyname:AddChild(self.__monster_name)
	self.__monster_name:SetAnchorPoint(1,0.5)
	PosTool.RightCenter(self.__monster_name)
	--怪物Boss头像
	self.__panel_enemyhead = self:GetWidget("Panel_EnemyHead")
	--怪物Boss血量
	self.__text_enemyhp = self:GetWidget("Text_EnemyHP")
	self.__text_enemyhp:SetZOrder(2)
	Util:WidgetLabel(self.__text_enemyhp,"", Macros.TypefaceSize.slightly, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--怪物Boss血条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MONSTER_STORMCASTLE, "prog_hp_hero_big")
	self.__monster_hp_bar = GUI.ProgressBar9Layer.New(imgdata_bar, nil, 302, 19)
	self.__monster_hp_bar:SetAnchorPoint(0.5, 0.5)
	self.__monster_hp_bar:SetFlipX(true)	
	self:GetWidget("Panel_EnemyHP"):AddChild(self.__monster_hp_bar:GetNode())	
	PosTool.Center(self.__monster_hp_bar,4,0)

	--积分
	Util:WidgetLabel(self:GetWidget("Text_intergralTxt"),CPPGameLib.GetString("monster_stormcastle_myintergraltxt"), Macros.TypefaceSize.largish, Macros.Color.Special)
	self.__text_intergralnum = self:GetWidget("Text_intergralNum")
	Util:WidgetLabel(self.__text_intergralnum,"", Macros.TypefaceSize.largish, Macros.Color.btn)
	--排名
	Util:WidgetLabel(self:GetWidget("Text_rankTxt"),CPPGameLib.GetString("monster_stormcastle_myranktxt"), Macros.TypefaceSize.largish, Macros.Color.Special)
	self.__text_ranknum = self:GetWidget("Text_rankNum")
	Util:WidgetLabel(self.__text_ranknum,"", Macros.TypefaceSize.largish, Macros.Color.btn)
	--活动剩余时间
	Util:WidgetLabel(self:GetWidget("Text_residueTimeTxt"),CPPGameLib.GetString("monster_stormcastle_residuetimetxt"), Macros.TypefaceSize.largish, Macros.Color.Special)
	self.__text_residuetimenum = self:GetWidget("Text_residueTimeNum")
	Util:WidgetLabel(self.__text_residuetimenum,"", Macros.TypefaceSize.largish, Macros.Color.green)
	--积分背景
	self.__image_intergralbg = self:GetWidget("Image_intergralBg")

	--鼓舞效果次数
	self.__text_inspiretimesnum = self:GetWidget("Text_inspireTimesNum")
	Util:WidgetLabel(self.__text_inspiretimesnum,"", Macros.TypefaceSize.largish, Macros.Color.green)
	Util:WidgetLabel(self:GetWidget("Text_inspireTimesTxt"),CPPGameLib.GetString("monster_stormcastle_inspiretimestxt"), Macros.TypefaceSize.largish, Macros.Color.Special)
	--鼓舞效果（伤害和积分）
	self.__image_inspirebg = self:GetWidget("Image_inspireBg")
	--伤害
	self.__text_harmnum = self:GetWidget("Text_harmNum")
	Util:WidgetLabel(self.__text_harmnum,"", Macros.TypefaceSize.largish, Macros.Color.green)
	self.__text_harmtxt = self:GetWidget("Text_harmTxt")
	Util:WidgetLabel(self.__text_harmtxt,"", Macros.TypefaceSize.largish, Macros.Color.Special)
	--积分
	self.__text_intergralpercentnum = self:GetWidget("Text_intergralPercentNum")
	Util:WidgetLabel(self.__text_intergralpercentnum,"", Macros.TypefaceSize.largish, Macros.Color.green)	
	Util:WidgetLabel(self:GetWidget("Text_intergralPercentTxt"),CPPGameLib.GetString("monster_stormcastle_intergralpercenttxt"), Macros.TypefaceSize.largish, Macros.Color.Special)
	--鼓舞背景
	self.__image_inspirebg = self:GetWidget("Image_inspireBg")

	--倒计时背景
	self.__image_countdowntimebg = self:GetWidget("Image_countDownTimeBg")
	--信息一
	self.__text_statustxtone = self:GetWidget("Text_statusTxtOne")
	Util:WidgetLabel(self.__text_statustxtone,"", Macros.TypefaceSize.largish, Macros.Color.golden,0,0,Macros.Color.common_stroke)
	--信息二
	self.__text_statustxttwo = self:GetWidget("Text_statusTxtTwo")
	Util:WidgetLabel(self.__text_statustxttwo,"", Macros.TypefaceSize.largish, Macros.Color.golden,0,0,Macros.Color.common_stroke)
	--复活面板
	self.__panel_resurgence = self:GetWidget("Panel_resurgence")
	self.__panel_resurgence:SetVisible(false)
	--复活倒计时文字
	self.__text_countdownresurgence = self:GetWidget("Text_countDownResurgence")
	Util:WidgetLabel(self.__text_countdownresurgence,"",38, Macros.Color.golden)
	--复活按钮
	Util:WidgetButton(self:GetWidget("Button_resurgence"),"",function ()
		local diamond_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
		if diamond_num < ConfigAdapter.Common.GetMonsterStormCastleResurgenceCost() then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_diamond_no_enough"))
			return
		end
		--请求复活
		self:CallModuleFunc("AttackCityReliveReq",Macros.Global.FALSE)
		--停止计时器
		if self.__resurgence_count_down_timer then
			GlobalTimerQuest:CancelQuest(self.__resurgence_count_down_timer)
			self.__resurgence_count_down_timer = nil
		end	
	end)
	Util:WidgetLabel(self:GetWidget("Text_resurgenceCost"),ConfigAdapter.Common.GetMonsterStormCastleResurgenceCost(),Macros.TypefaceSize.tab, Macros.Color.btn,0,0,Macros.Color.button_yellow)
	Util:WidgetLabel(self:GetWidget("Text_resurgenceTxt"),CPPGameLib.GetString("monster_stormcastle_resurgencetxt"),Macros.TypefaceSize.tab, Macros.Color.btn,0,0,Macros.Color.button_yellow)
	--开宝箱面板
	self.__panel_opentreasurebox = self:GetWidget("Panel_openTreasureBox")
	self.__panel_opentreasurebox:SetVisible(false)
	--特效容器
	self.__panel_boxeffect = self:GetWidget("Panel_boxEffect")
	--开箱进度条
	self.__loadingbar = self:GetWidget("LoadingBar")
	--开启文字
	Util:WidgetLabel(self:GetWidget("Text_boxOpening"),CPPGameLib.GetString("monster_stormcastle_opening_box"),Macros.TypefaceSize.headlineII, Macros.Color.btn,0,0,Macros.Color.common_stroke)
	--聊天窗口容器
	local panel_chat = self:GetWidget("Panel_Chat")	
	--聊天窗口
	self.liaotian = Modules.MainCityChatLayer.New( panel_chat )
	self.liaotian:SetAnchorPoint( 0.5,0.5 )
	panel_chat:AddChild(self.liaotian:GetNode())
	PosTool.Center(self.liaotian)
	--点击打开聊天窗口
	local layer = Util:Layout( panel_chat:GetContentWidth() , panel_chat:GetContentHeight() ,function ( ... )
        CallModuleFunc(ModuleType.CHAT, "OpenView", BaseViewType.CHAT)
    end )
    layer:SetAnchorPoint(0.5,0.5)
    panel_chat:AddChild(layer , 99 )
    PosTool.Center( layer )  

    --规则按钮
    self.Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(2)怪物攻城" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Monster")
        end )
    end )
    self.Button_rule:SetZOrder(100)

    local union_rule = Util:Name(CPPGameLib.GetString("union_rule"))
    union_rule:SetAnchorPoint( 0.5,0.5 )
    union_rule:SetPosition( self.Button_rule:GetContentWidth()/2 , 0 )
    self.Button_rule:AddChild( union_rule )  

	--系统入口
	self:__InitSystemList()
	--创建多个层
	self:__CreateLayers()
	--初始化各个信息
	self:__InitMessage()
end

function Modules.MonsterStormCastleView:__RegistAllEvents()
	--进入怪物攻城响应
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_ENTER_RESP, function(_,player_list)
		self.__player_list = player_list
		CPPGameLib.PrintTable("skfb",player_list,"player_list  ")
		self:__InitPlayerModuleAndPosition()
	end)

	--新玩家进入怪物攻城通知
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_ENTER_NOTICE, function(_,info)
		self:__CreatePlayerModule(info)
	end)

	--玩家离开怪物攻城通知
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_LEAVE_NOTICE, function(_,cur_role_id)
		--删除玩家和精灵模型
		for role_id,hero_module in pairs(self.__hero_module_list) do
			if role_id == cur_role_id then
				hero_module:DeleteMe()
				self.__hero_module_list[role_id] = nil
				break
			end
		end
		for role_id,spirit_module in pairs(self.__spirit_module_list) do
			if role_id == cur_role_id then
				spirit_module:DeleteMe()
				self.__spirit_module_list[role_id] = nil
				break
			end
		end
		self.__player_message[cur_role_id] = nil
	end)

	--新玩家移动通知
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_GOTO_NOTICE, function(_,info)
		if self.__player_list and self.__player_list[info.role_id] and self.__hero_module_list and self.__hero_module_list[info.role_id] then
			self:__MovingHero(info.dest_x,info.dest_y,info.role_id)
		end
	end)

	--活动倒计时
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_TIME_RESP, function(_,system_id,show_txt,is_open)
		if system_id == Macros.Game.SystemID.MONSTER_STORMCASTLE then
			self.__text_statustxttwo:SetText(show_txt)
			self.__panel_enemy:SetVisible(false)
			if is_open then
				self:__InitMessage()
			end
		end
	end)

	--阵容红点
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		if system_id == Macros.Game.SystemID.HERO_LINEUP then
			self:__UpdateLineupBtnRedDot()
		end
	end)

	--怪物列表
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_MONTSER_LIST_NOTICE, function(_,monster_list,cur_chapter)
		if self.__is_no_first_refrash then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_monster_refrash"))
		else
			self.__is_no_first_refrash = true
		end
		self.__monster_list = monster_list
		self.__cur_chapter = cur_chapter
		self:__InitMonsterModuleAndPosition()
	end)

	--怪物数据通知
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.ATTACKCITY_MONTSER_DATA_NOTICE, function(_,monster_info,is_dead,monster_uid)
		if monster_info and self.__monster_module_list[monster_info.uid] then	
			if not self.__cur_boss_uid or (self.__cur_boss_uid and monster_info.uid ~= self.__cur_boss_uid) then	
				local model = self.__monster_module_list[monster_info.uid]:GetNode()
				local cur_model = self.__monster_module_list[monster_info.uid]:GetModel()
				local hp_bar = cur_model:GetChildLayerByTag(RenderObj.InnerLayer.HP_BAR)
				if hp_bar then
					hp_bar:SetProgress(monster_info.total_hp - monster_info.damage)
					hp_bar:SetVisible(monster_info.damage > 0)
				end
				CPPActionManager.StopAllActions(self.__monster_module_list[monster_info.uid]:GetNode())
				self.__monster_module_list[monster_info.uid]:PlayAnimation(RenderObj.ActionIndex.IDLE, true)
				local is_in_battle = self:JudgeIsInBattle(monster_info.start_battle_time)
				if is_in_battle then
					if not monster_info.finghting_effect_id then
						local effect
						monster_info.finghting_effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_zhandouzhong",loop = true,scale = 1.5/monster_info.show_scale},self.__monster_module_list[monster_info.uid]:GetNode(),RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
						PosTool.CenterTop(effect,0,280)	
					end				
				else
					if monster_info.finghting_effect_id then
						self:__DelEffect(monster_info.finghting_effect_id)
						monster_info.finghting_effect_id = nil
					end
					self:RunAction(monster_info.uid)	
				end
			else
				self:__CountDownMonsterResidueNum()
			end
		else
			if is_dead then
				self.__total_monster_num = self.__total_monster_num - 1
				if self.__cur_boss_uid and monster_uid == self.__cur_boss_uid then
					self.__monster_module_list[monster_uid]:SetTouchFun(nil,nil,nil)
					self.__cur_boss_uid = nil
					-- local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
					-- self.__player_info.kill_boss_time = cur_server_time
					self.__is_on_waiting = true
					--BOSS死亡特效
					local model = self.__monster_module_list[monster_uid]:GetNode()
					local effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_bosssiwang",loop = false},self.__touch_layer:GetNode(),1000 - model:GetPositionY())
					effect:SetPosition(model:GetPositionX(),model:GetPositionY())
					self.__monster_module_list[monster_uid]:PlayAnimation(RenderObj.ActionIndex.DEAD, false)
					CPPActionManager.DelayTo(self.__monster_module_list[monster_uid]:GetNode(),2,GlobalCallbackMgr:AddCallBackFunc(function ()
			    		self.__monster_module_list[monster_uid]:DeleteMe()
						self.__monster_module_list[monster_uid] = nil						
			    	end))
				else
					if self.__monster_module_list[monster_uid] then
						self.__monster_module_list[monster_uid]:DeleteMe()
						self.__monster_module_list[monster_uid] = nil
					end
				end
				self:__CountDownMonsterResidueNum()
			end
		end
	end)

	--宝箱列表通知
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.TREASURE_BOX_LIST_NOTICE, function(_,treasure_list)
		self.__treasure_list = treasure_list
		self:__InitTreasureBoxAndPosition()
	end)

	--宝箱消失通知
	self:BindGlobalEvent(Macros.Event.MonsterStormCastleModule.TREASURE_BOX_DISAPPEAR_NOTICE, function(_,treasure_info)
		if self.__treasure_box_list and self.__treasure_box_list[treasure_info.uid] then
			self.__treasure_box_list[treasure_info.uid]:RemoveSelf(true)
			self.__treasure_box_list[treasure_info.uid] = nil
		end
	end)

	--错误码
	self:BindGlobalEvent(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, function(_,error_code)
		if error_code == 20006 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_in_battle_tips"))
		elseif error_code == 20010 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_target_dead_tips"))
		end
	end)
end

--鼓舞特效
function Modules.MonsterStormCastleView:AddSpireEffect()
	local effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_guwu"},self.__image_inspirebg,3)
	PosTool.Center(effect)
end

--判断是否在战斗
function Modules.MonsterStormCastleView:JudgeIsInBattle(start_battle_time)
	if not start_battle_time then
		return
	end
	--战斗时间
	if start_battle_time > 0 then 
		local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC,"GetServerTime")	
		return (cur_server_time - start_battle_time) <= ConfigAdapter.Common.GetMonsterStormCastleBattleCD()
	else
		return false
	end
end

--初始化宝箱和位置
function Modules.MonsterStormCastleView:__InitTreasureBoxAndPosition()
	self.__treasure_box_list = self.__treasure_box_list or {}	
	for uid,treaure_info in pairs(self.__treasure_list) do	
		if not self.__treasure_box_list[uid] then	
			local item_type,item_config = ObjIDTool.GetInfo(treaure_info.info_id)		 
			local imgdata_box = Resource.PathTool.GetItemPath(item_config.res_id)
			self.__treasure_box_list[uid] = Util:Sprite(imgdata_box,function ()
				self:__MovingHero(treaure_info.map_x,treaure_info.map_y,nil,function ()
					self:CallModuleFunc("TreasureBoxLockReq",uid)
				end)				
			end)
			self.__treasure_box_list[uid]:SetAnchorPoint(0.5,0)
			self.__touch_layer:AddChild(self.__treasure_box_list[uid],999 - treaure_info.map_y)
			self.__treasure_box_list[uid]:SetPosition(treaure_info.map_x,treaure_info.map_y)
			--刷宝箱特效
			if self.__treasure_refrash then
				local effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_shuabaoxiang",loop = false},self.__touch_layer:GetNode(),999 - treaure_info.map_y)
				effect:SetPosition(treaure_info.map_x,treaure_info.map_y)
			end
		end
	end
	if not self.__treasure_refrash then
		self.__treasure_refrash = true
	end
end

--初始化怪物模型和位置
function Modules.MonsterStormCastleView:__InitMonsterModuleAndPosition()
	if not self.__cur_chapter then
		return
	end
	self.__monster_module_list = self.__monster_module_list or {}
	if not self.__is_activity_opened then
		self.__is_activity_opened = #self.__monster_module_list > 0
	end
	--当前是否为Boss
	local cur_monster_config = CPPGameLib.GetConfig("AttackCityMonster",self.__cur_chapter)
	self.__is_on_waiting = cur_monster_config.is_boss == Macros.Global.TRUE and #self.__monster_list == 0 and true or false
	--统一缩小比例
	local common_zoom = ConfigAdapter.Common.GetMonsterStormCastleHeroZoom()
	--血条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bar_red")
	local imgdata_bar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bg")
	--计算怪物数量
	self.__total_monster_num = 0
	for uid,monster_info in pairs(self.__monster_list) do
		self.__total_monster_num = self.__total_monster_num + 1	
		if not self.__monster_module_list[uid] then
			--关卡信息
			local gamelevel_info = CPPGameLib.GetConfig("GameLevel",monster_info.map_id)
			--怪物信息
			local monster_config = CPPGameLib.GetConfig("Monster",gamelevel_info.icon_id)
			monster_info.show_scale = monster_config.show_scale * common_zoom
			--怪物模型					
			self.__monster_module_list[uid] = LogicGUI.Model.New({info_id = gamelevel_info.icon_id,model_type = LogicGUI.ModelType.MONSTER, parent = self.__touch_layer,zorder = 1000 - monster_info.map_y,
																	dir = (monster_info.is_boss == Macros.Global.FALSE) and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT})	
			self.__monster_module_list[uid]:SetScaleFactor(monster_config.show_scale * common_zoom)			
			self.__monster_module_list[uid]:SetPosition(monster_info.map_x,monster_info.map_y)
			local scale_factor = 1/monster_info.show_scale
			if monster_info.is_boss == Macros.Global.FALSE then
				self.__monster_module_list[uid]:SetTouchRect(-35*scale_factor, 0, 70 * scale_factor, 100 * scale_factor)
			else
				self.__monster_module_list[uid]:SetTouchRect(-100*scale_factor, 0, 200*scale_factor, 200*scale_factor)
				self.__cur_boss_uid = uid
			end
			self.__monster_module_list[uid]:SetTouchFun(nil,nil,function ()	
				local is_in_battle = self:JudgeIsInBattle(monster_info.start_battle_time)
				print("skfb","is_in_battleis_in_battleis_in_battle ",is_in_battle)			
				if not is_in_battle then
					if self.__on_weaking_status then
						GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_on_weaking_tips"))
						return
					end
					local model = self.__monster_module_list[uid]:GetModel()
					local cur_posx,cur_posy = model:GetPosition()						
					self.__cur_monster_info = monster_info	
					self:__MovingHero(cur_posx,cur_posy,nil,nil,true)				
				else
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_in_battle_tips"))
				end
			end)
			local is_in_battle = self:JudgeIsInBattle(monster_info.start_battle_time)
			if is_in_battle then
				local effect
				monster_info.finghting_effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_zhandouzhong",loop = true,scale = 1/monster_info.show_scale},self.__monster_module_list[uid]:GetNode(),RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
				PosTool.CenterTop(effect,0,280)	
			end

			--关卡名字
			local model = self.__monster_module_list[uid]:GetModel()
			if monster_info.is_boss == Macros.Global.FALSE then
				local gamelevel_name = Util:Label(gamelevel_info.name, Macros.TypefaceSize.headlineII, Macros.Color.orange,0,0,Macros.Color.orange_stroke)				
				model:AddChild(gamelevel_name,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
				gamelevel_name:SetPosition(0,230)
			end

			--血条
			if monster_info.is_boss == Macros.Global.FALSE then
				local hp_bar = GUI.ProgressBarLayer.New(imgdata_bar, imgdata_bar_bg, monster_info.total_hp - monster_info.damage, monster_info.total_hp)
		        hp_bar:SetPosition(0, 200)
		        hp_bar:SetScaleXFactor(2)
		        hp_bar:SetScaleYFactor(1.5)
		        model:AddChild(hp_bar, RenderObj.InnerLayer.HP_BAR, RenderObj.InnerLayer.HP_BAR)
		        hp_bar:SetVisible(monster_info.damage > 0)
		    end

	        if self.__monster_refrash then
		        --刷新特效
		        local refrash_name = monster_info.is_boss == Macros.Global.FALSE and "UI_gongcheng_shuaguai" or "UI_boss_chuxian"
				local effect_id,effect = self:__CreateEffect({res_name = refrash_name,loop = false},monster_info.is_boss == Macros.Global.FALSE and self.__touch_layer:GetNode() or self:GetNode(),1000 - monster_info.map_y)
				effect:SetPosition(monster_info.is_boss == Macros.Global.FALSE and monster_info.map_x or Macros.Global.DesignSize.width/2,monster_info.is_boss == Macros.Global.FALSE and monster_info.map_y or Macros.Global.DesignSize.height/2)
			end
			local is_in_battle = self:JudgeIsInBattle(monster_info.start_battle_time)
	        --怪物移动动作
	        if monster_info.is_boss == Macros.Global.FALSE and not is_in_battle then
			    self["funId_"..uid] = GlobalCallbackMgr:AddCallBackFunc(function ()
			    	local random_num = MathTool.GetRandom(-75,75)
			    	local move_x = self.__monster_list[uid].map_x + random_num
			    	local model = self.__monster_module_list[uid]:GetModel()
					local cur_posx,cur_posy = model:GetPosition()	
			    	local cur_dir = cur_posx > move_x and MathTool.HORZ_LEFT or MathTool.HORZ_RIGHT
			    	self.__monster_module_list[uid]:SetDir(cur_dir)
					self.__monster_module_list[uid]:PlayAnimation(RenderObj.ActionIndex.RUN, true)
					CPPActionManager.MoveTo(model:GetNode(),math.abs(move_x - cur_posx)/(ConfigAdapter.Common.GetMonsterStormCastlePlayerSpeed()/2),move_x,cur_posy,GlobalCallbackMgr:AddCallBackFunc(function()
						self.__monster_module_list[uid]:PlayAnimation(RenderObj.ActionIndex.IDLE, true)
						self:RunAction(uid)
					end))	
				end)
			    self:RunAction(uid)		
			end	
		end
	end

	if not self.__monster_refrash then
		self.__monster_refrash = true
	end
	--计算剩余怪物数量
	self:__CountDownMonsterResidueNum()
end

--初始化玩家模型和位置
function Modules.MonsterStormCastleView:RunAction(uid)
	local random_num = MathTool.GetRandom(0.5,2)
	CPPActionManager.DelayTo(self.__monster_module_list[uid]:GetNode(),random_num,self["funId_"..uid])
end

--计算剩余怪物数量
function Modules.MonsterStormCastleView:__CountDownMonsterResidueNum()
	if self.__is_activity_opened then		
		--正在Boss
		local boss_icon_id,blood_txt,boss_name		
		if self.__cur_boss_uid and self.__cur_boss_uid > 0 then
			local cur_hp = Util:NumTransform(self.__monster_list[self.__cur_boss_uid].total_hp - self.__monster_list[self.__cur_boss_uid].damage)
			local total_hp = Util:NumTransform(self.__monster_list[self.__cur_boss_uid].total_hp)
			blood_txt = CPPGameLib.GetString("monster_stormcastle_total_hp",tostring(cur_hp),tostring(total_hp))  
			--关卡信息
			local gamelevel_info = CPPGameLib.GetConfig("GameLevel",self.__monster_list[self.__cur_boss_uid].map_id)
			local monster_info = CPPGameLib.GetConfig("AttackCityMonster",self.__cur_chapter)
			--怪物信息
			boss_icon_id = gamelevel_info.icon_id
			boss_name = GUI.RichText.GetColorWord(monster_info.boss_name, Macros.Color.red_hex, Macros.Color.red_stroke_hex, 2)
			--血量比例
			self.__monster_hp_bar:SetProgressMax(self.__monster_list[self.__cur_boss_uid].total_hp)
			self.__monster_hp_bar:SetProgress(self.__monster_list[self.__cur_boss_uid].total_hp - self.__monster_list[self.__cur_boss_uid].damage)
		elseif self.__is_on_waiting then
			if not self.__cur_chapter then
				return
			end
			self.__is_on_waiting = false
			--打完Boss等刷新下一波
			blood_txt = CPPGameLib.GetString("monster_stormcastle_hide_blood")
			local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
			if self.__player_info.kill_boss_time ~= 0 and (cur_server_time < (self.__player_info.kill_boss_time + ConfigAdapter.Common.GetMonsterStormCastleKillBossRefrashCD())) then
				self.__boss_dead_refrash_count = self.__player_info.kill_boss_time + ConfigAdapter.Common.GetMonsterStormCastleKillBossRefrashCD() - cur_server_time
				local boss_info
				for wave_num = self.__cur_chapter+1,MathTool.MAX_NUM do
					local monster_info = CPPGameLib.GetConfig("AttackCityMonster",wave_num)
					if monster_info.is_boss == Macros.Global.TRUE then
						boss_info = monster_info
						break
					end
				end
				--头像
				local gamelevel_info = CPPGameLib.GetConfig("GameLevel",boss_info.map_id_list[1])
				boss_icon_id = gamelevel_info.icon_id
				--名字
				boss_name = CPPGameLib.GetString("monster_stormcastle_boss_dead_refrash",self.__boss_dead_refrash_count)
				boss_name = GUI.RichText.GetColorWord(boss_name, Macros.Color.orange_hex, Macros.Color.orange_stroke_hex, 2)
				--血量
				self.__monster_hp_bar:SetProgressMax(100)
				self.__monster_hp_bar:SetProgress(100)
				if not self.__boss_dead_refrash_count_down_timer then
					self.__boss_dead_refrash_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
					function()
						self.__boss_dead_refrash_count = self.__boss_dead_refrash_count - 1
						local boss_name = CPPGameLib.GetString("monster_stormcastle_boss_dead_refrash",self.__boss_dead_refrash_count)
						boss_name = GUI.RichText.GetColorWord(boss_name, Macros.Color.orange_hex, Macros.Color.orange_stroke_hex, 2)
						--名字
						self.__monster_name:SetText(boss_name)
						if self.__boss_dead_refrash_count == 0 then											
							--停止计时器
							if self.__boss_dead_refrash_count_down_timer then
								GlobalTimerQuest:CancelQuest(self.__boss_dead_refrash_count_down_timer)
								self.__boss_dead_refrash_count_down_timer = nil
							end	
						end
					end), 1,-1)
				end
			end
		else
			--停止计时器
			if self.__boss_dead_refrash_count_down_timer then
				GlobalTimerQuest:CancelQuest(self.__boss_dead_refrash_count_down_timer)
				self.__boss_dead_refrash_count_down_timer = nil
			end
			blood_txt = CPPGameLib.GetString("monster_stormcastle_hide_blood")
			local countdown_num = 0
			local boss_info
			for wave_num = self.__cur_chapter+1,MathTool.MAX_NUM do
				local monster_info = CPPGameLib.GetConfig("AttackCityMonster",wave_num)
				if monster_info.is_boss == Macros.Global.TRUE then
					boss_info = monster_info
					break
				end
				countdown_num = countdown_num + monster_info.refresh_monster_num
			end
			local gamelevel_info = CPPGameLib.GetConfig("GameLevel",boss_info.map_id_list[1])
			--怪物信息
			boss_icon_id = gamelevel_info.icon_id
			boss_name = CPPGameLib.GetString("monster_stormcastle_kill_monster_tips",tostring(countdown_num+self.__total_monster_num),boss_info.boss_name or "")
			boss_name = GUI.RichText.GetColorWord(boss_name, Macros.Color.orange_hex, Macros.Color.orange_stroke_hex, 2)
			--血量
			self.__monster_hp_bar:SetProgressMax(100)
			self.__monster_hp_bar:SetProgress(100)
		end
		--BOSS头像
		if boss_icon_id then
			local icon_path = Resource.PathTool.GetHeadPathByObjID(boss_icon_id, true)
			if not self.__boss_head_icon then		
		      	self.__boss_head_icon = Util:Sprite(icon_path)
		     	self.__panel_enemyhead:AddChild(self.__boss_head_icon, LogicGUI.IconLayer.InnerZOrder.ICON)
		     	PosTool.Center(self.__boss_head_icon)
		    else
		    	self.__boss_head_icon:SetImage(icon_path)
			end
		end
		--BOSS血量		
		if blood_txt then
			self.__text_enemyhp:SetText(blood_txt)
		end
		--名字
		if boss_name then
			self.__monster_name:SetText(boss_name)	
		end
	end
end

--初始化玩家模型和位置
function Modules.MonsterStormCastleView:__InitPlayerModuleAndPosition()	
	self.__hero_module_list = self.__hero_module_list or {}
	self.__spirit_module_list = self.__spirit_module_list or {}
	self.__player_message = self.__player_message or {}
	CPPGameLib.PrintTable("skfb",self.__player_list,"self.__player_list ")
	for role_id,player_info in pairs(self.__player_list) do	
		self:__CreatePlayerModule(player_info)
	end	
end

--初始化系统图标列表
function Modules.MonsterStormCastleView:__InitSystemList()
	local panel_systementry = self:GetWidget("Panel_SystemEntry")
	--阵容
	self.__btn_lineup = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_lineup"), function()		
		CallModuleFunc(ModuleType.HERO,"OpenView", BaseViewType.HERO_LINEUP)
	end)
	panel_systementry:AddChild(self.__btn_lineup)
	self.__btn_lineup:SetPosition(150,40)
	local label = Util:Label(CPPGameLib.GetString("dungeon_lineup_btn"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
	self.__btn_lineup:AddChild(label)
	PosTool.CenterBottom(label,0,12)
	--红点
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__entry_red_dot:SetAnchorPoint(1, 1)
	self.__btn_lineup:AddChild(self.__entry_red_dot,1)
	PosTool.RightTop(self.__entry_red_dot)
	--更新阵容红点
	self:__UpdateLineupBtnRedDot()	

	--首领奖励
	self.__btn_boss_reward = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_boss"), function()
		--是否有BOSS奖励可以领取
		if self.__boss_reward_red_dot:IsVisible() then
			Modules.TemplMonsterStormCastleRewardView.New(self.__player_info.from_chapter,self.__cur_chapter,self.__player_info.clear_up_boss_reward_record_list,self.__cur_boss_uid)
		else
			--活动未开始或者是已经结束
			if not self.__is_activity_opened then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_no_today_tips_3"))
				return 	
			elseif self.__is_activity_opened and self.__is_activity_over then
				local system_info = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonInfo",Macros.Game.SystemID.MONSTER_STORMCASTLE) 
				local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
				if system_info and (cur_server_time - system_info.time_list[4] >= ConfigAdapter.Common.GetMonsterStormCastleDelayTimeToReward()) then
					--红点
					self.__boss_reward_red_dot:SetVisible(false)
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_no_today_tips_3"))
					return 	
				end
			end
			Modules.TemplMonsterStormCastleRewardView.New(self.__player_info.from_chapter,self.__cur_chapter,self.__player_info.clear_up_boss_reward_record_list,self.__cur_boss_uid)
		end
	end)
	panel_systementry:AddChild(self.__btn_boss_reward)
	self.__btn_boss_reward:SetPosition(self.__btn_lineup,40)
	local label = Util:Label(CPPGameLib.GetString("monster_stormcastle_boss_rewardtxt"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
	self.__btn_boss_reward:AddChild(label)
	PosTool.CenterBottom(label,0,12)
	--BOSS奖励
	self.__boss_reward_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__boss_reward_red_dot:SetAnchorPoint(1, 1)
	self.__btn_boss_reward:AddChild(self.__boss_reward_red_dot,1)
	PosTool.RightTop(self.__boss_reward_red_dot)
	local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.MONSTER_STORMCASTLE)
	self.__boss_reward_red_dot:SetVisible(red_num > 0)

	--排行榜
	self.__btn_rank = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_rank"), function()
		CallModuleFunc(ModuleType.RANK,"RankListRequest",{type = Macros.Game.RankType.MONSTER_STORMCITY})
	end)
	panel_systementry:AddChild(self.__btn_rank)
	self.__btn_rank:SetPosition(self.__btn_boss_reward,40)
	local label = Util:Label(CPPGameLib.GetString("rank_title"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
	self.__btn_rank:AddChild(label)
	PosTool.CenterBottom(label,0,12)

	--鼓舞
	self.__btn_inspire = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_strong"), function()
		print("ldx","self.__is_activity_opened self.__is_activity_over: ",self.__is_activity_opened,self.__is_activity_over)
		--活动未开始或者是已经结束
		if not self.__is_activity_opened or self.__is_activity_over then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_no_today_tips_3"))
			return 				
		end
		if self.__player_info.encourage_times >= #CPPGameLib.GetConfig("AttackCityEncourage", nil, nil , true ) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_encourage_enough_tips"))
			return 			
		end
		Modules.TemplMonsterStormCastleEncourage.New(self.__player_info.encourage_times)
	end)
	panel_systementry:AddChild(self.__btn_inspire)
	self.__btn_inspire:SetPosition(self.__btn_rank,40)
	local label = Util:Label(CPPGameLib.GetString("monster_stormcastle_inspiretxt"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
	self.__btn_inspire:AddChild(label)
	PosTool.CenterBottom(label,0,12)

	self.Button_rule:SetPosition(self.__btn_inspire, self.Button_rule:GetPositionY())


end

--更新阵容按钮红点
function Modules.MonsterStormCastleView:__UpdateLineupBtnRedDot()
	local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.HERO_LINEUP, Macros.Game.SystemID.MAX)	
	self.__entry_red_dot:SetVisible(red_num > 0)
end

--更新Boss奖励按钮红点
function Modules.MonsterStormCastleView:__UpdateBossRewardBtnRedDot()
	if not self.__cur_chapter then
		return
	end	
	local is_visible = false
	for i,reward_info in ipairs(CPPGameLib.GetConfig("AttackCityBossReward", nil, nil , true )) do
		if (self.__player_info.from_chapter <= reward_info.chapter) and (reward_info.chapter < self.__cur_chapter) then
			if not self.__player_info.clear_up_boss_reward_record_list[reward_info.chapter] then
				is_visible = true
				break
			end
		elseif reward_info.chapter == self.__cur_chapter and not self.__cur_boss_uid then
			if not self.__player_info.clear_up_boss_reward_record_list[reward_info.chapter] then
				is_visible = true
				break
			end
		else
		end
	end
	self.__boss_reward_red_dot:SetVisible(is_visible)
end

--初始化信息
function Modules.MonsterStormCastleView:__InitMessage()	
	--预告开始
	self.__is_activity_notice_opened = CallModuleFunc(ModuleType.LIMIT_DUNGEON,"IsLimitDungeonOpen",Macros.Game.SystemID.MONSTER_STORMCASTLE)
	--检查活动是否开始
	self.__is_activity_opened,self.__is_activity_over = CallModuleFunc(ModuleType.LIMIT_DUNGEON,"IsLimitDungeonOpen",Macros.Game.SystemID.MONSTER_STORMCASTLE,true)	

	local enemy_visible = false
	local intergralbg_visible = true
	local countdowntimebg_visible = true
	if not self.__is_activity_notice_opened or self.__is_activity_over then
		--活动未开始或者是已经结束
		intergralbg_visible = false
		self.__text_statustxtone:SetText(CPPGameLib.GetString("monster_stormcastle_no_today_tips_1"))
		self.__text_statustxttwo:SetText(CPPGameLib.GetString("monster_stormcastle_no_today_tips_2"))		
	elseif self.__is_activity_notice_opened and not self.__is_activity_opened and not self.__is_activity_over then
		--预告时间内但活动还未开始
		self.__text_residuetimenum:SetText(CPPGameLib.GetString("monster_stormcastle_unopenedtxt"))
		self.__text_ranknum:SetText(0)
		self.__text_intergralnum:SetText(0)
		self.__text_statustxtone:SetText(CPPGameLib.GetString("monster_stormcastle_opened_counddowntxt"))
	elseif self.__is_activity_opened and not self.__is_activity_over then	
		enemy_visible = true
		countdowntimebg_visible = false
		--活动正在进行中
		--倒计时
		if not self.__activity_over_timer then
			self.__activity_over_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
			function()
				self:__CountDownActivityOverTime()
			end), 1, -1)
			self:__CountDownActivityOverTime()
		end	
	end
	self.__panel_enemy:SetVisible(enemy_visible)
	self.__image_intergralbg:SetVisible(intergralbg_visible)
	self.__image_inspirebg:SetVisible(intergralbg_visible)
	self.__image_countdowntimebg:SetVisible(countdowntimebg_visible)
end

--初始化复活或者虚弱信息
function Modules.MonsterStormCastleView:__InitResurgenceMessage(player_info)
	self.__player_info = player_info or self.__player_info
	local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	self.__resurgence_count_time = 0
	if self.__player_info.relive_cd > 0 then		
		if cur_server_time < (self.__player_info.battle_over_time + self.__player_info.relive_cd) then
			self.__resurgence_count_time = (self.__player_info.battle_over_time + self.__player_info.relive_cd) - cur_server_time
			self.__resurgence_type = 1
			--如果已经过了复活CD则直接到复活点复活并播放复活特效
			if self.__resurgence_count_time <= 0 then
				--请求复活
				self:CallModuleFunc("AttackCityReliveReq",Macros.Global.TRUE)
			else
				--播放死亡动作
				if self.__hero_module_list and self.__hero_module_list[self.__own_role_id] then
					self.__hero_module_list[self.__own_role_id]:PlayAnimation(RenderObj.ActionIndex.DEAD,false)
				end
				self.__on_deading_status = true
				--显示复活层
				self.__panel_resurgence:SetVisible(true)
			end
		end
	elseif self.__player_info.weak_cd > 0 then
		if cur_server_time < (self.__player_info.battle_over_time + self.__player_info.weak_cd) then
			self.__resurgence_count_time = (self.__player_info.battle_over_time + self.__player_info.weak_cd) - cur_server_time
			self.__resurgence_type = 2
			--标记虚弱状态
			self.__on_weaking_status = true
			--倒计时背景
			self.__image_countdowntimebg:SetVisible(true)
			--信息一
			self.__text_statustxtone:SetText(CPPGameLib.GetString("monster_stormcastle_on_weaking"))
		end
	end

	--倒计时
	if not self.__resurgence_count_down_timer and self.__resurgence_count_time > 0 then
		self.__resurgence_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self.__resurgence_count_time = self.__resurgence_count_time - 1
			if self.__resurgence_count_time > 0 then
				if self.__resurgence_type == 1 then
					self.__text_countdownresurgence:SetText(CPPGameLib.GetString("monster_stormcastle_on_resurgenceing_waitingtime",self.__resurgence_count_time))
				else
					self.__text_statustxttwo:SetText(CPPGameLib.GetString("monster_stormcastle_on_weaking_waitingtime",self.__resurgence_count_time))
				end
			else
				if self.__resurgence_type == 1 then	
					self.__on_deading_status = false				
					--请求复活
					self:CallModuleFunc("AttackCityReliveReq",Macros.Global.TRUE)
				else
					--标记虚弱状态
					self.__on_weaking_status = false
					--倒计时背景
					self.__image_countdowntimebg:SetVisible(false)
				end
				--停止计时器
				if self.__resurgence_count_down_timer then
					GlobalTimerQuest:CancelQuest(self.__resurgence_count_down_timer)
					self.__resurgence_count_down_timer = nil
				end	
			end
		end), 1,-1)
		if self.__resurgence_type == 1 then
			self.__text_countdownresurgence:SetText(CPPGameLib.GetString("monster_stormcastle_on_resurgenceing_waitingtime",self.__resurgence_count_time))
		else
			self.__text_statustxttwo:SetText(CPPGameLib.GetString("monster_stormcastle_on_weaking_waitingtime",self.__resurgence_count_time))
		end
	end	

	--鼓舞次数
	self.__text_inspiretimesnum:SetText(self.__player_info.encourage_times)
	
	--鼓舞效果（伤害和积分）
	--伤害
	if self.__player_info.encourage_times > 0 then
		local encourage_config = CPPGameLib.GetConfig("AttackCityEncourage",self.__player_info.encourage_times)
		local value_type,value = Util:Nature(encourage_config.pro_type, encourage_config.pro_value, true)
		--值
		self.__text_harmnum:SetText(value)
		self.__text_harmtxt:SetText(value_type) 
		--积分
		local add_intergral_rate = MathTool.TransProbCoeffPercentage(encourage_config.score_add_rate)
		local worth = string.format("+%s%%",tostring(add_intergral_rate))
		self.__text_intergralpercentnum:SetText(worth)
	else
		local encourage_config = CPPGameLib.GetConfig("AttackCityEncourage",1)
		self.__text_harmnum:SetText("+0")
		local type_name = Macros.Game.BattlePropName[encourage_config.pro_type] or ""
		self.__text_harmtxt:SetText(type_name) 
		--积分
		self.__text_intergralpercentnum:SetText(CPPGameLib.GetString("common_add8",0))
	end

	--领取BOSS奖励整理
	self.__player_info.clear_up_boss_reward_record_list = {}
	for i,chapter_info in ipairs(self.__player_info.take_boss_reward_record_list) do
		self.__player_info.clear_up_boss_reward_record_list[chapter_info.chapter] = chapter_info.chapter
	end
	self:__UpdateBossRewardBtnRedDot()

	--判断是否是打完Boss倒计时
	local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	if self.__player_info.kill_boss_time ~= 0 and (cur_server_time < (self.__player_info.kill_boss_time + ConfigAdapter.Common.GetMonsterStormCastleKillBossRefrashCD())) then
		self.__is_on_waiting = true
		self:__CountDownMonsterResidueNum()
	end

	--雕像
	local statue_id = self.__player_info.no1_head_img_id == 0 and 31440 or self.__player_info.no1_head_img_id
	local statue_name = self.__player_info.no1_head_img_id == 0 and CPPGameLib.GetString("monster_stormcastle_statue_none") or self.__player_info.no1_role_name
	if not self.__statue_module then
		local hero_config = CPPGameLib.GetConfig("Hero",statue_id)
		--雕像		
		self.__statue_module = LogicGUI.Model.New({info_id = statue_id, parent = self.__statue_layer,zorder = 2})	
		self.__statue_module:SetScaleFactor(hero_config.city_scale)
		self.__statue_module:SetGray(true)		
		self.__statue_module:SetToPlayTime(0)
		self.__statue_module:SetPosition(445,455)
		--雕像名字
		self.__statue_name = Util:Label(statue_name, Macros.TypefaceSize.normal, Macros.Color.golden , 0, 0, Macros.Color.common_stroke)
		self.__statue_layer:AddChild(self.__statue_name,2)
		self.__statue_name:SetPosition(445,440)
	else
		self.__statue_module:SetInfoID(statue_id)
		self.__statue_name:SetString(statue_name)
	end
end

--隐藏复活界面
function Modules.MonsterStormCastleView:VisiblePanelResurgence(visible,is_free_relive)
	self.__panel_resurgence:SetVisible(visible)
	if is_free_relive == Macros.Global.FALSE then
		self.__hero_module_list[self.__own_role_id]:PlayAnimation(RenderObj.ActionIndex.IDLE, true)
	end
	--复活
	if self.__player_list[self.__own_role_id] then
		local effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_fuhuo"},self.__touch_layer:GetNode(),1002 - self.__player_list[self.__own_role_id].map_y)
		effect:SetPosition(self.__player_list[self.__own_role_id].map_x,self.__player_list[self.__own_role_id].map_y)
	end
end

--更新自己积分和排名
function Modules.MonsterStormCastleView:UpdateOwnIntergralAndRank(own_rank_info)
	self.__own_rank_info = own_rank_info or self.__own_rank_info
	if self.__own_rank_info then
		self.__text_intergralnum:SetText(self.__own_rank_info.score)
		self.__text_ranknum:SetText(self.__own_rank_info.rank == 0 and CPPGameLib.GetString("monster_stormcastle_no_on_rank") or self.__own_rank_info.rank)
	end
end

--更新阵容按钮红点
function Modules.MonsterStormCastleView:__CountDownActivityOverTime()
	local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local system_info = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonInfo",Macros.Game.SystemID.MONSTER_STORMCASTLE) 
	if system_info then
		local show_txt
		if system_info.time_list[4] - cur_server_time > 0 then
			show_txt = TimeTool.TransTimeStamp("MM:SS",system_info.time_list[4] - cur_server_time)
		else
			show_txt = CPPGameLib.GetString("monster_stormcastle_haveendtxt")
			if self.__activity_over_timer then
				GlobalTimerQuest:CancelQuest(self.__activity_over_timer)
				self.__activity_over_timer = nil
			end		
			--self:__InitMessage()
			self:__ActivityOverNotice()
		end
		self.__text_residuetimenum:SetText(show_txt)
	end	
end

--创建玩家模型
function Modules.MonsterStormCastleView:__CreatePlayerModule(player_info)
	local org_pos = 150
	if self.__hero_module_list and not self.__hero_module_list[player_info.role_id] then
		--统一缩小比例
		local common_zoom = ConfigAdapter.Common.GetMonsterStormCastleHeroZoom()		
		local hero_config = CPPGameLib.GetConfig("Hero",player_info.head_img_id)
		local oppose_zoom = 1/(hero_config.city_scale * common_zoom)
		--英雄模型
		self.__hero_module_list[player_info.role_id] = LogicGUI.Model.New({info_id = player_info.head_img_id, parent = self.__touch_layer,zorder = 1001 - player_info.map_y})	
		self.__hero_module_list[player_info.role_id]:SetScaleFactor(hero_config.city_scale * common_zoom)
		
		--角色名字
		local role_name = Util:Label(player_info.role_name, Macros.TypefaceSize.slightly, player_info.role_id == self.__own_role_id and Macros.Color.monster_stormcastle or Macros.Color.btn ,0,0,
									player_info.role_id == self.__own_role_id and Macros.Color.monster_stormcastle_stroke or Macros.Color.common_stroke)
		local model = self.__hero_module_list[player_info.role_id]:GetModel()
		model:AddChild(role_name,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
		role_name:SetScaleFactor(oppose_zoom) 
		role_name:SetPosition(0,org_pos/(hero_config.city_scale * common_zoom))	
		--称号
		if player_info.title_id ~= 0 then
			local titlesp = Resource.PathTool.GetTitleIconPath("sp_title_"..player_info.title_id,true)
	        local title_sp = Util:Sprite(titlesp)
	        title_sp:SetPosition(0,(org_pos+40)/(hero_config.city_scale * common_zoom))
	        title_sp:SetScaleFactor(oppose_zoom) 
	        model:AddChild(title_sp,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
		end
		--昵称
		if player_info.nickname_id ~= 0 then
			--前昵称
			local frontnicksp = Resource.PathTool.GetTitleIconPath("sp_prefix_"..player_info.nickname_id,true)
	        local front_nicksp = Util:Sprite(frontnicksp)
	        front_nicksp:SetAnchorPoint(1,0.5)
	        front_nicksp:SetScaleFactor(oppose_zoom)
	        front_nicksp:SetPosition(-(role_name:GetContentWidth()/2*oppose_zoom + 5),org_pos/(hero_config.city_scale * common_zoom))
	        model:AddChild(front_nicksp,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
	        --后昵称
			local backnicksp = Resource.PathTool.GetTitleIconPath("sp_suffix_"..player_info.nickname_id)
	        local back_nicksp = Util:Sprite(backnicksp)
	        back_nicksp:SetAnchorPoint(0,0.5)
	        back_nicksp:SetScaleFactor(oppose_zoom)
	        back_nicksp:SetPosition((role_name:GetContentWidth()/2*oppose_zoom)+5,org_pos/(hero_config.city_scale * common_zoom))
	        model:AddChild(back_nicksp,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
		end
		--精灵模型
		if player_info.sprite_id ~= 0 then
			self.__spirit_module_list[player_info.role_id] = LogicGUI.Model.New({info_id = player_info.sprite_id,
								parent = self.__touch_layer,model_type = LogicGUI.ModelType.SPRITE,zorder = 1001 - player_info.map_y})		
			self.__spirit_module_list[player_info.role_id]:SetScaleFactor(common_zoom)
			self.__spirit_module_list[player_info.role_id]:SetPosition(player_info.map_x - 50,player_info.map_y + 70)
		end
	end
	self.__hero_module_list[player_info.role_id]:SetPosition(player_info.map_x,player_info.map_y)	
	self.__hero_module_list[player_info.role_id]:SetZOrder(1001 - player_info.map_y)
	if self.__spirit_module_list and self.__spirit_module_list[player_info.role_id] then
		self.__spirit_module_list[player_info.role_id]:SetZOrder(1001 - player_info.map_y)
		self.__spirit_module_list[player_info.role_id]:SetPosition(player_info.map_x - 50,player_info.map_y + 70)
	end
	if player_info.role_id == self.__own_role_id then		
		self:__MovingBottomLayer()
		if self.__on_deading_status then
			self.__hero_module_list[self.__own_role_id]:PlayAnimation(RenderObj.ActionIndex.DEAD,false)
		else
			self.__hero_module_list[player_info.role_id]:PlayAnimation(RenderObj.ActionIndex.IDLE, true)			
		end	
	end
end

--创建多个层
--[[
--最底层 父亲层 背景层
--触摸层
]]--
function Modules.MonsterStormCastleView:__CreateLayers()
	--天空层
	self.__sky_layer_params = {image_path_list = {"bg_monster_stormcastle_sky_scene"},image_size = COCOSize(1704, 640)}
	self.__sky_layer = MonsterStormCastleBG.New(self.__sky_layer_params)					
	self:AddChild(self.__sky_layer, Macros.Global.z_order_bottom)
	--山水层
	self.__mountain_layer_params = {image_path_list = {"bg_monster_stormcastle_mountain_scene"},image_size = COCOSize(1704, 640)}
	self.__mountain_layer = MonsterStormCastleBG.New(self.__mountain_layer_params)					
	self:AddChild(self.__mountain_layer, Macros.Global.z_order_bottom)
	--主城层
	self.__maincity_layer_params = {image_path_list = {"bg_monster_stormcastle_maincity_scene"},image_size = COCOSize(1704, 640)}
	self.__maincity_layer = MonsterStormCastleBG.New(self.__maincity_layer_params)					
	self:AddChild(self.__maincity_layer, Macros.Global.z_order_bottom)
	--雕像层
	self.__statue_layer_params = {image_path_list = {"bg_monster_stormcastle_statue_scene"},image_size = COCOSize(1704, 640)}
	self.__statue_layer = MonsterStormCastleBG.New(self.__statue_layer_params)					
	self:AddChild(self.__statue_layer, Macros.Global.z_order_bottom)
	local layout = Util:Layout(230, 125, function ()
		CallModuleFunc(ModuleType.RANK,"RankListRequest",{type = Macros.Game.RankType.MONSTER_STORMCITY})
	end)
	self.__statue_layer:GetNode():AddChild(layout,3)
	layout:SetPosition(958,350)
	--地面层
	self.__ground_layer_params = {image_path_list = {"bg_monster_stormcastle_ground_scene"},image_size = COCOSize(1704, 640)}
	self.__ground_layer = MonsterStormCastleBG.New(self.__ground_layer_params)					
	self:AddChild(self.__ground_layer, Macros.Global.z_order_bottom)
	--近景层
	self.__close_layer_params = {image_path_list = {"bg_monster_stormcastle_close_scene"},image_size = COCOSize(1704, 640)}
	self.__close_layer = MonsterStormCastleBG.New(self.__close_layer_params)					
	self:AddChild(self.__close_layer, Macros.Global.z_order_bottom)
	--触摸层
	local could_move_area_list = ConfigAdapter.Common.GetMonsterStormCastleCouldMoveArea()
	self.__touch_layer_params = {image_size = COCOSize(could_move_area_list[2] - could_move_area_list[1], could_move_area_list[4] - could_move_area_list[3]),func = function (x,y)
		local layer_pos = NodeUnity.ConvertToNodePositon(self.__touch_layer:GetNode(), COCOPoint(x,y))
		if self.__move_effect_id then
			self:__DelEffect(self.__move_effect_id)
			self.__move_effect_id = nil
		end
		local effect
		self.__move_effect_id,effect = self:__CreateEffect({res_name = "UI_gongcheng_dimian",loop = false},self.__touch_layer:GetNode())
		effect:SetPosition(layer_pos.x,layer_pos.y)
		self:__MovingHero(math.max(layer_pos.x,0),math.max(layer_pos.y,0))
	end}
	self.__touch_layer = MonsterStormCastleBG.New(self.__touch_layer_params)					
	self.__ground_layer:AddChild(self.__touch_layer,8)
	self.__touch_layer:SetPosition(could_move_area_list[1],could_move_area_list[3])

	--若背景图可点击，则设置根节点可穿透
	self.__root_widget:SetTouchSwallow(false)	
end

function Modules.MonsterStormCastleView:__MovingHero(x,y,role_id,func,is_moving_fighting)
	--print("ldx","TTTTTTTTTTTTTTTTTTTTTTTTTTT: ")
	role_id = role_id or self.__own_role_id
	--当前移动英雄
	self.__moving_hero_role_id = role_id
	self.__update_hero_zorder = true
	if self.__hero_module_list[role_id] then
		CPPActionManager.StopAllActions(self.__hero_module_list[role_id]:GetNode())
		if self.__spirit_module_list[role_id] then
			CPPActionManager.StopAllActions(self.__spirit_module_list[role_id]:GetNode())
		end
	end
	local model = self.__hero_module_list[role_id]:GetModel()
	local cur_posx,cur_posy = model:GetPosition()	
	local cur_dir = cur_posx > x and MathTool.HORZ_LEFT or MathTool.HORZ_RIGHT
	local cur_time = math.sqrt(math.pow(cur_posx - x,2)+math.pow(cur_posy - y,2))/ConfigAdapter.Common.GetMonsterStormCastlePlayerSpeed()
	local model_list = {self.__hero_module_list[role_id],self.__spirit_module_list[role_id]}
	local pos_list = {{pos_x = x,pos_y = y},{pos_x = cur_dir == MathTool.HORZ_RIGHT and (x - 50) or (x+50),pos_y = y + 70}}
	if role_id == self.__own_role_id then
		self.__update_move = true
		self.__is_moving_fighting = is_moving_fighting
		self:CallModuleFunc("AttackCityGotoReq",{src_x = math.floor(cur_posx),src_y = math.floor(cur_posy),dest_x = math.floor(x),dest_y = math.floor(y)})
	end
	for num,model in ipairs(model_list) do		
		if not self.__player_message[role_id] or self.__player_message[role_id].cur_dir ~= cur_dir or self.__player_message[role_id].change_move then			
			self.__player_message[role_id] = self.__player_message[role_id] or {}
			model:SetDir(cur_dir)
			model:PlayAnimation(RenderObj.ActionIndex.RUN, true)
			if num == #model_list then
				self.__player_message[role_id].cur_dir = cur_dir
			end
		end
		if num == #model_list then
			self.__player_message[role_id].change_move = false
		end		
		CPPActionManager.MoveTo(model:GetNode(),cur_time,pos_list[num].pos_x,pos_list[num].pos_y,GlobalCallbackMgr:AddCallBackFunc(function ()
			model:PlayAnimation(RenderObj.ActionIndex.IDLE, true)
			self.__player_message[role_id].change_move = true
			if num == 1 then
				self.__player_list[role_id].map_x = pos_list[num].pos_x
				self.__player_list[role_id].map_y = pos_list[num].pos_y
				if not self.__spirit_module_list[role_id] then
					self.__update_move = false
				end
			elseif num == #model_list and role_id == self.__own_role_id then
				if self.__is_moving_fighting then
					--self.__cur_monster_info
					self.__is_moving_fighting = false


					
					if self.__cur_monster_info and self.__cur_monster_info.uid and self.__monster_module_list[self.__cur_monster_info.uid] then
						CPPActionManager.StopAllActions(self.__monster_module_list[self.__cur_monster_info.uid]:GetNode())
						self.__monster_module_list[self.__cur_monster_info.uid]:PlayAnimation(RenderObj.ActionIndex.IDLE, true)						
						--请求挑战怪物
						CallModuleFunc(ModuleType.BATTLE, "BattleStartReq",Macros.Game.FBType.ATTACK_CITY,self.__cur_monster_info.uid, self.__cur_monster_info.map_id)					
					end



				end
				if func then
					func()
				end
				self.__update_move = false
				self.__update_hero_zorder = false
			end
		end))		
	end	
end

function Modules.MonsterStormCastleView:__MovingBottomLayer()
	local model = self.__hero_module_list[self.__own_role_id]:GetModel()
	local cur_posx,cur_posy = model:GetPosition()
	--print("ldx","LLLLLLLLLLLLLL； ",cur_posx,cur_posy)
	local pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__touch_layer:GetNode(), self.__ground_layer:GetNode(), COCOPoint(cur_posx,cur_posy))
	--如果小于屏幕一半则为屏幕一半
	local actrual_posx,actrual_posy
	if pos.x < Macros.Global.DesignSize.width/2 then
		actrual_posx = Macros.Global.DesignSize.width/2
	else
		actrual_posx = pos.x
	end
	if pos.y < Macros.Global.DesignSize.height/2 then
		actrual_posy = Macros.Global.DesignSize.height/2
	else
		actrual_posy = pos.y
	end
	if actrual_posx >= self.__ground_layer:GetWidth() - Macros.Global.DesignSize.width/2 then
		actrual_posx = self.__ground_layer:GetWidth() - Macros.Global.DesignSize.width/2
	end
	if actrual_posy >= self.__ground_layer:GetHeight() - Macros.Global.DesignSize.height/2 then
		actrual_posy = self.__ground_layer:GetHeight() - Macros.Global.DesignSize.height/2
	end
	local ground_move_x = Macros.Global.DesignSize.width/2 - actrual_posx
	local ground_move_y = Macros.Global.DesignSize.height/2 - actrual_posy
	local speed_list = ConfigAdapter.Common.GetMonsterStormCastleSceneSpeed()
	--移动速度
	self.__sky_layer:SetPosition(speed_list[1]*ground_move_x,speed_list[1]*ground_move_y)
	self.__mountain_layer:SetPosition(speed_list[2]*ground_move_x,speed_list[2]*ground_move_y)
	self.__maincity_layer:SetPosition(speed_list[3]*ground_move_x,speed_list[3]*ground_move_y)
	self.__statue_layer:SetPosition(speed_list[4]*ground_move_x,speed_list[4]*ground_move_y)
	self.__ground_layer:SetPosition(speed_list[5]*ground_move_x,speed_list[5]*ground_move_y)
	self.__close_layer:SetPosition(speed_list[6]*ground_move_x,speed_list[6]*ground_move_y)
end

--打开开箱子界面
function Modules.MonsterStormCastleView:__OpenBoxView(open_uid)
	self.__panel_opentreasurebox:SetVisible(true)
	if not self.__open_box_effectid then
		local effect
		self.__open_box_effectid,effect = self:__CreateEffect({res_name = "UI_gongcheng_kaibaoxiang"},self.__panel_boxeffect)
		PosTool.Center(effect,0,50)
	end
	self.__loadingbar:SetPercent(0)
	--开箱倒计时
	if not self.__open_box_timer then
		self.__box_count_num = 1
		self.__open_box_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self.__box_count_num = self.__box_count_num + 1
			self.__loadingbar:SetPercent(3.5*self.__box_count_num)
			if self.__box_count_num >= (ConfigAdapter.Common.GetMonsterStormCastleOpenBoxTime() * 10) then
				if self.__open_box_effectid then
					self:__DelEffect(self.__open_box_effectid)
					self.__open_box_effectid = nil
				end
				GlobalTimerQuest:CancelQuest(self.__open_box_timer)
				self.__open_box_timer = nil
				self:CallModuleFunc("TreasureBoxPickUpReq",open_uid)
				self.__panel_opentreasurebox:SetVisible(false)				
			end
		end), 0.1, -1)	
		self.__loadingbar:SetPercent(3.5*self.__box_count_num)	
	end	
end

--刷新排行榜
function Modules.MonsterStormCastleView:__RefrashRankList(rank_list)
	self.__rank_list = rank_list or self.__rank_list
	if #self.__rank_list > 0 then
		if self.__no_rank_name then
			self.__no_rank_name:SetVisible(false)
		end
		for i,role_info in ipairs(self.__rank_list) do
			if i <= 3 then
				if not self["__rank_name_"..i] then
					--名字底
					local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MONSTER_STORMCASTLE, "bg_rank")
					local rank_bg = Util:Sprite(sp)
					self.__statue_layer:GetNode():AddChild(rank_bg,2)
					rank_bg:SetAnchorPoint(0.5,0.5)
					rank_bg:SetPosition(1072,432 - (i-1)*31)
					--排名
					sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MONSTER_STORMCASTLE, "rank_txt_"..i,true)
					local rank_txt = Util:Sprite(sp)
					rank_bg:AddChild(rank_txt,2)
					rank_txt:SetAnchorPoint(0,0.5)
					PosTool.LeftCenter(rank_txt,3.5,-1)
					--名字
					self["__rank_name_"..i] = Util:Label(role_info.role_name, Macros.TypefaceSize.normal, Macros.Color.btn)
					rank_bg:AddChild(self["__rank_name_"..i],2)
					self["__rank_name_"..i]:SetAnchorPoint(0.5,0.5)
					PosTool.Center(self["__rank_name_"..i],31,-1)	
				else
					self["__rank_name_"..i]:SetString(role_info.role_name)
				end
			end
		end
	else
		if not self.__no_rank_name then
			self.__no_rank_name = Util:Label(CPPGameLib.GetString("monster_stormcastle_no_rank"), Macros.TypefaceSize.normal, Macros.Color.btn)
			self.__statue_layer:GetNode():AddChild(self.__no_rank_name,6)
			self.__no_rank_name:SetAnchorPoint(0.5,0.5)
			self.__no_rank_name:SetPosition(1075,400)
		end
	end
end

function Modules.MonsterStormCastleView:__ActivityOverNotice()
	self.__is_moving_fighting = false
	--清理怪物
	if self.__monster_module_list then
		for uid,monster_module in pairs(self.__monster_module_list) do
			monster_module:DeleteMe()
			self.__monster_module_list[uid] = nil
		end
	end
	--清理宝箱
	if self.__treasure_box_list then
		for uid,treasure_box in pairs(self.__treasure_box_list) do
			treasure_box:RemoveSelf(true)
			self.__treasure_box_list[uid] = nil
		end			
	end
	self.__monster_list = {}
	self.__treasure_list = {}
	self:CallModuleFunc("ActivityOverNotice")
	--Boss奖励
	self.__boss_reward_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			self:__CountDownBossReward()
	end), 1,-1)
end

--首领
function Modules.MonsterStormCastleView:__CountDownBossReward()
	local system_info = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "GetLimitDungeonInfo",Macros.Game.SystemID.MONSTER_STORMCASTLE) 
	local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	if system_info and (cur_server_time - system_info.time_list[4] >= ConfigAdapter.Common.GetMonsterStormCastleDelayTimeToReward()) then
		self.__btn_boss_reward:SetVisible(false)
		if self.__boss_reward_count_down_timer then
			GlobalTimerQuest:CancelQuest(self.__boss_reward_count_down_timer)
			self.__boss_reward_count_down_timer = nil
		end
	end
end

MonsterStormCastleBG = MonsterStormCastleBG or BaseClass(UILayer)

--[[
@ params：
	必须：
    image_path_list		[string]贴图路径
    image_size			[cocosize]背景图原尺寸
    image_anchor 		[cocopoint]背景图锚点
    func				[function]点击回调
--]]
function MonsterStormCastleBG:__init(params)
	self.image_size = params.image_size or Macros.Global.BGDefaultSize
	self.image_anchor = params.image_anchor or COCOPoint(0.5, 0)

	self:SetAnchorPoint(0, 0)
	self:SetContentSize(self.image_size.width, self.image_size.height)
	if params.func then
		self:SetTouchEnabled(true)
		self:SetTouchFun(nil, nil, params.func)
	end

	--贴图路径
	self.image_path_list = params.image_path_list
	if self.image_path_list then
		for i,image_path in ipairs(self.image_path_list) do
			local image_node = Util:Sprite(Resource.PathTool.GetUIResPath("bigbg/"..image_path))
			image_node:SetAnchorPoint(self.image_anchor.x, self.image_anchor.y)
			self:AddChild(image_node,i*2)
			PosTool.CenterBottom(image_node)
		end
	end
end
