 
--[[
%% @module: 战斗界面
%% @author: swordman sue
%% @created: 2016-08-15
--]]

Modules = Modules or {}

Modules.BattleView = Modules.BattleView or BaseClass(BaseView)

function Modules.BattleView:__init()
	self.__layout_name = "battle"	
	self.__is_force_stretch = true
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

	self.__ref_in_pause = RefObj.New()
	self.__sprite_skill_map = {}
end

function Modules.BattleView:__OpenCallback()
	self:__InitTopInfo()
	self:__InitBottomInfo()
	self:__HandleBattleStart()
	self:__HandleLead()
end

function Modules.BattleView:__Dispose()
	self:__DelSpriteSkillPickEffect()

	if self.__hero_hp_bar_bg then
		self.__hero_hp_bar_bg:DeleteMe()
		self.__hero_hp_bar_bg = nil
	end
	if self.__hero_hp_bar then
		self.__hero_hp_bar:DeleteMe()
		self.__hero_hp_bar = nil
	end

	if self.__monster_hp_bar_bg then
		self.__monster_hp_bar_bg:DeleteMe()
		self.__monster_hp_bar_bg = nil
	end	
	if self.__monster_hp_bar then
		self.__monster_hp_bar:DeleteMe()
		self.__monster_hp_bar = nil
	end

	if self.__skip_timer then
		GlobalTimerQuest:CancelQuest(self.__skip_timer)
		self.__skip_timer = nil
	end

	CPPGameLib.ClearObjList(self.__templ_hero_oper_map)
	self.__templ_hero_oper_map = nil
end

function Modules.BattleView:__RegistAllEvents()
	--战斗开始
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_START, function()
		self:__HandleBattleStart()
	end)

	--战斗倒计时
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_TIME_STEP, function(_, remain_time)
		self:__UpdateRemainTime(remain_time)
	end)

	self.__hero_record_hp = 0
	self.__monster_record_hp = 0

	--血量变化
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_HP_CHANGE, function(_, obj_id, obj_hp, change_hp)
		local obj = GlobalScene:GetObj(obj_id)
		if not obj then
			return
		end

		if obj:IsHero() or obj:IsHeroPVP() then
			local hero_cur_hp = GlobalVOMgr:CalcTotalHeroHP()
			self.__hero_hp_txt:SetText(hero_cur_hp.."/"..self.__hero_total_hp)

			self.__hero_hp_bar:SetProgress(hero_cur_hp)

			if self.__templ_hero_oper_map[obj_id] then
				self.__templ_hero_oper_map[obj_id]:UpdateHP(obj_hp)
			end

			if not self.hero_hp then
				self.hero_hp = true
				if self.__hero_record_hp < hero_cur_hp then
					self.__hero_hp_bar_bg:SetProgress( hero_cur_hp )
					self.hero_hp = nil
				else
					CPPActionManager.DelayTo(self.__hero_hp_bar_bg:GetNode() , 0.5 , GlobalCallbackMgr:AddCallBackFunc(function ()
						self.__hero_hp_bar_bg:SetToProgress( self.__hero_record_hp )
						self.hero_hp = nil
					end))
				end
			end
			self.__hero_record_hp = hero_cur_hp

		elseif obj:IsMonster() or obj:IsMonsterPVP() then
			self:__HandleMonsterHpChange(change_hp)
		end
	end)

	--怒气变化
	self:BindGlobalEvent(Macros.Event.SceneObj.COMMON_ANGER_CHANGE, function(_, old_anger, new_anger)
		--检测怒气/合击技能可释放特效
		for _, template in pairs(self.__templ_hero_oper_map) do
			template:CheckAngerOrJointReleasableEffect()
			template:CheckAngerOrJointReleasableEffectPlus()
		end		

		--更新公共怒气进度
		self.__anger_bar:SetPercentage( (new_anger / GlobalVOMgr:GetMaxAnger()) * 100 )

		if new_anger >= 100 then
			GlobalEffectMgr:SetEffectVisible( self.sp_bomb[1] , true )
		else
			GlobalEffectMgr:SetEffectVisible( self.sp_bomb[1] , false )
		end
		if new_anger >= 200 then
			GlobalEffectMgr:SetEffectVisible( self.sp_bomb[2] , true )
		else
			GlobalEffectMgr:SetEffectVisible( self.sp_bomb[2] , false )
		end
		if new_anger >= 300 then
			GlobalEffectMgr:SetEffectVisible( self.sp_bomb[3] , true )
		else
			GlobalEffectMgr:SetEffectVisible( self.sp_bomb[3] , false )
		end
	end)

	--宝箱增加效果
	self:BindGlobalEvent(Macros.Event.SceneObj.DROPBOX_DEAD, function(_, obj_id, drop_info)		
		if not self:IsOpen() then
			return
		end

		self:__HandleDropBox(obj_id, drop_info)
	end)	

	--所有英雄死亡
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_ALL_HERO_DEAD, function()
		self.__img_pause:SetVisible(false)
		self.__panel_skip:SetVisible(false)
	end)

	--所有怪物死亡
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_ALL_MONSTER_DEAD, function()
		self.__img_pause:SetVisible(false)
		self.__panel_skip:SetVisible(false)

		--怪物死亡自动取消精灵技能的释放
		self:__OnUpSpriteSkill()
	end)

	--进入战斗跳过
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_SKIP, function()
		self:SetVisible(false)

		--战斗跳过自动取消精灵技能的释放
		self:__OnUpSpriteSkill(nil, nil, nil, nil, true)
	end)

	--结束战斗跳过
	-- self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_SKIP_ABORT, function()
	-- 	self:SetVisible(true)
	-- end)

	--精灵技能释放的事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_SPRITE_SKILL_RELEASE, function(_, obj_id, skill_id)
		local obj = GlobalScene:GetObj(obj_id)
		if not obj or (not obj:IsHero(true) and not obj:IsHeroPVP(true)) then
			return
		end
		self:__HandleSpriteSkillReleased(skill_id)
		self:__HandlePasue()
	end)

	--精灵技能释放完成的事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_SPRITE_SKILL_RELEASE_END, function()
		self:__HandleResume()
	end)	

	--怒气/合击技能释放的事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE, function(_, obj_id)
		self:__HandlePasue()
	end)

	--怒气/合击技能释放完成的事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE_END, function(_, obj_id)
		self:__HandleResume()
	end)	

	--游戏暂停
	self:BindGlobalEvent(Macros.Event.SpLogicModule.PAUSE, function(_)
		self:__HandlePasue()
	end)	

	--游戏恢复
	self:BindGlobalEvent(Macros.Event.SpLogicModule.RESUME, function(_)
		self:__HandleResume()
	end)		

	--监听特殊指引开启事件
	self:BindGlobalEvent(Macros.Event.LeadModule.START_SP, function(_, lead_id)
		--释放强化版怒气/合击技能
		if lead_id == Macros.Game.LeadSpecialID.RELEASE_ANGER_OR_JOINT_PLUS_SKILL then
			local templ = self.__templ_hero_oper_map[self.__first_hero_oper_id or 0]
			if templ then
				templ:CreateAngerOrJointSkillLeadEffect()
			end

		--释放精灵技能
		elseif lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_1 
				or lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_2
				or lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_3
				or lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_4
				or lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_5 then
			self:__CreatSpriteSkillLeadEffect()

		--好友助战指引
		elseif lead_id == Macros.Game.LeadSpecialID.FRIEND_ASSIST_LEAD then
			if self.__template_bt_hero_op_assist then
				self.__template_bt_hero_op_assist:__CreateLeadEffect()
			end
		end
	end)  

	--QTE
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE, function(_, obj_id)		
		local panel_sprite_oper = self:GetWidget("Panel_SpritesOper")
		panel_sprite_oper:SetVisible(false)
	end)

	--QTE结束
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE_END, function(_, obj_id)		
		local panel_sprite_oper = self:GetWidget("Panel_SpritesOper")
		panel_sprite_oper:SetVisible(true)
	end)

	--QTE连击
	self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_QTE_COMBO, function(_, combo_hit_times)		
		-- self:__HandleQTEComboTimes(combo_hit_times)	
	end)
	
end

function Modules.BattleView:__InitTopInfo()
	local battle_info = self:CallModuleFunc("GetCurInfo")
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
	local my_name, my_head_id = self:__GetMyNameAndHead(battle_info)

	self.__img_dropbox = self:GetWidget("Image_DropBox")

	--我方名字
	local img_me_name = self:GetWidget("Image_MeName")
	local lab_role_name = Util:Label(my_name)
	lab_role_name:SetAnchorPoint( 1 , 0.5 )
	lab_role_name:SetPosition( img_me_name:GetContentWidth() - 40 , img_me_name:GetContentHeight()/2 )
	img_me_name:AddChild(lab_role_name)

	--我方血条	 --【第三版UI替换】
	widget = self:GetWidget("Image_MgHpBg")
	self.__hero_hp_txt = self:GetWidget("Text_MyHP")
	self.__hero_hp_txt:SetZOrder(2)

	--我方血条
	--减少
	local prog_hp_hero_big1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_hp_hero_big1")
	self.__hero_hp_bar_bg = GUI.ProgressBar9Layer.New(prog_hp_hero_big1, nil, 382 , 22  )
	self.__hero_hp_bar_bg:SetAnchorPoint(0.5, 0.5)
	-- self.__hero_hp_bar_bg:SetOpacity( 90 )
	widget:AddChild(self.__hero_hp_bar_bg:GetNode() )
	PosTool.Center(self.__hero_hp_bar_bg , 2 )

	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_hp_hero_big")
	self.__hero_hp_bar = GUI.ProgressBar9Layer.New(imgdata_bar, nil, 382 , 22  )
	self.__hero_hp_bar:SetAnchorPoint(0.5, 0.5)
	widget:AddChild(self.__hero_hp_bar:GetNode() )
	PosTool.Center(self.__hero_hp_bar , 2 )

	widget = self:GetWidget("Image_MgHpBgFrame")
	widget:SetZOrder( 10 )

	--敌方名字
	local enemy_name, enemy_head_id, is_monster = self:__GetEnemyNameAndHead( battle_info )

	--敌方名字
	local img_enemy_name = self:GetWidget("Image_EnemyName")

	self.lab_enemy_name = Util:Label( enemy_name )
	self.lab_enemy_name:SetAnchorPoint( 0 , 0.5 )
	self.lab_enemy_name:SetPosition( 40 , img_enemy_name:GetContentHeight()/2 )
	img_enemy_name:AddChild(self.lab_enemy_name)

	--敌方血条 --【第三版UI替换】
	widget = self:GetWidget("Image_EnemyHpBg")

	self.__monster_hp_txt = self:GetWidget("Text_EnemyHP")
	self.__monster_hp_txt:SetZOrder(2)

	prog_hp_hero_big1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_hp_enemy_big1")
	self.__monster_hp_bar_bg = GUI.ProgressBar9Layer.New(prog_hp_hero_big1, nil, 382 , 22 )
	self.__monster_hp_bar_bg:SetAnchorPoint(0.5, 0.5)
	widget:AddChild(self.__monster_hp_bar_bg:GetNode())	
	PosTool.Center(self.__monster_hp_bar_bg , 4 )

	imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_hp_enemy_big2")
	self.__monster_hp_bar = GUI.ProgressBar9Layer.New(imgdata_bar, nil, 382 , 22 )
	self.__monster_hp_bar:SetAnchorPoint(0.5, 0.5)
	widget:AddChild(self.__monster_hp_bar:GetNode())	
	PosTool.Center(self.__monster_hp_bar , 4 )

	widget = self:GetWidget("Image_EnemyHpBgFrame")
	widget:SetZOrder( 10 )

	--战役场次
	self.__txt_battle_times = self:GetWidget("Text_BattleTimes")
	
	--倒计时
	local Image_9 = self:GetWidget("Image_9")
	local dun = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_dun")
	Image_9:SetImage( dun )
	local img_dun = Util:Sprite( dun )
	Image_9:SetContentSize( img_dun:GetContentWidth() , img_dun:GetContentHeight() )

    local img_num = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_num")
    if not self.__txt_countdown then
	    self.__txt_countdown = GUI.ImgFont.New(img_num , GUI.ImgFont.ImgType.Num4 )
	    self.__txt_countdown:SetText(tostring(0))
		self.__txt_countdown:SetAnchorPoint(0.5,0.5)
		self.__txt_countdown:SetScaleFactor(0.95)
	    Image_9:AddChild(self.__txt_countdown:GetNode()) 
	    PosTool.Center( self.__txt_countdown , 0 , 10 )
	end

	--加速 --【第三版UI替换】
	local acc_index = self:CallModuleFunc("GetACCIndex")
	self.__btn_acc = self:GetWidget("Button_ACC")
	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_acc"..acc_index)
	self.__btn_acc:SetNormalImage(imgdata)

	--加速的背景
	self.__img_acc = self:GetWidget("Image_ACC")
	self.__img_acc:IgnoreContentAdaptWithSize(true)
	imgdata = acc_index ~= 1 and 
	Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_pauseBg_2") or 
	Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_pauseBg_1")
	self.__img_acc:SetImage(imgdata)
	PosTool.Center(self.__btn_acc)

	WidgetUnity.SetWidgetListener(self.__img_acc, nil, nil, function()
		self:__HandleChangeACC()
	end)
	self.__img_acc:SetVisible(config_gamelevel.show_acc_btn ~= Macros.Global.FALSE)

	--暂停 --【第三版UI替换】
	self.__img_pause = self:GetWidget("Button_Pause")
	WidgetUnity.SetWidgetListener( self.__img_pause , nil, nil, function()		
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "Pause")
	end)
	self.__img_pause:SetVisible(config_gamelevel.show_pause_btn ~= Macros.Global.FALSE)

	--自动战斗 --【第三版UI替换】
	local panel_auto_tips = self:GetWidget("Panel_AutoTips")
	local cbox_auto = self:GetWidget("CheckBox_Auto")
	cbox_auto:IgnoreContentAdaptWithSize(true)
	cbox_auto:SetPosition( cbox_auto:GetPositionX() , cbox_auto:GetPositionY() )

	local img_auto = self:GetWidget("Image_Auto")
	self:__AutoSetType( battle_info.type , cbox_auto )

	local function change_auto(var)
		panel_auto_tips:SetVisible(var)	
		if var then
			img_auto:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "btn_auto_open") )
			if self.effect_id then
				GlobalEffectMgr:SetEffectVisible(self.effect_id, true)
			else
				local effect_com
				self.effect_id, effect_com = self:__CreateEffect({res_name = "UI_zidongzhandou"}, img_auto )
				effect_com:SetAnchorPoint(0.5 , 0.5)
				PosTool.Center(effect_com)
			end
		else
			img_auto:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "btn_auto_close") )
			if self.effect_id then
				GlobalEffectMgr:SetEffectVisible(self.effect_id, false)
			end
		end
	end
	WidgetUnity.SetCheckboxListener(cbox_auto, 
		function()
			if self:__AutoOpen( battle_info.type ) then
				change_auto(true)
			else
				cbox_auto:SetSelectedState( true )
			end
		end,
		function()
			if self:__AutoClose( battle_info.type ) then
				change_auto(false)
			else
				cbox_auto:SetSelectedState( true )
			end
		end)

	local is_auto = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAutoFight")
	change_auto(is_auto)
	cbox_auto:SetSelectedState(is_auto)

	if config_gamelevel.show_auto_fight_btn == Macros.Global.FALSE then
		local layer = Util:Layout( cbox_auto:GetContentWidth() , cbox_auto:GetContentHeight() , function ( ... )
		end )	
		layer:SetAnchorPoint( 0.5 , 0.5 )
		cbox_auto:AddChild( layer )
		PosTool.Center( layer )

        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon")
        local lock_img = Util:Sprite(sp)       
        layer:AddChild(lock_img,1)
        PosTool.Center(lock_img)
        lock_img:SetScaleFactor( 0.6 )
	end

	--掉落信息
	self.__txt_dropbox_num = self:GetWidget("Text_DropBoxNum")
	self.__txt_dropbox_num:SetText(0)	
end

function Modules.BattleView:__InitBottomInfo()
	--战斗跳过
	self.__panel_skip = self:GetWidget("Panel_Skip")
	self.__image_skip = self:GetWidget("Image_Skip")
	WidgetUnity.SetWidgetListener(self.__panel_skip, nil, nil, function()		
		self:CallModuleFunc("Skip")
	end)

	--QTE
	-- self.__sp_qte_perfect = self:GetWidget("Sprite_qte_perfect")
	-- self.__sp_qte_perfect_num = self:GetWidget("Sprite_qte_perfect_num")
	-- self:__HandleQTEComboTimes()

	--初始化英雄操作区域
	self:__InitHeroOperRegion()

	--初始化精灵操作区域
	self:__InitSpriteOperRegion()

	--初始化公共怒气
	self:__InitCommonAnger()
end

--初始化公共怒气
function Modules.BattleView:__InitCommonAnger()

	local Panel_anger_bar = self:GetWidget("Panel_anger_bar")
	local cur_anger = GlobalVOMgr:GetAnger()
	local max_anger = GlobalVOMgr:GetMaxAnger()
	
	local prog_anger_bar = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_anger_bar"))
	self.__anger_bar = ProgressTimer.CreateWithImageLabel( prog_anger_bar )
	self.__anger_bar:SetType(ProgressTimer.TypeRadial)
	self.__anger_bar:SetMidpoint(COCOPoint(0.5, 0.5))
	self.__anger_bar:SetAnchorPoint(0.5, 0.5)
	self.__anger_bar:SetReverseDirection(false)
	Panel_anger_bar:AddChild(self.__anger_bar)
	PosTool.Center( self.__anger_bar )
	
	CPPActionManager.RotateTo( self.__anger_bar , 0 , 120 )
	self.__anger_bar:SetPercentage( (cur_anger / max_anger) * 100 )

	self.sp_bomb = {}
	local Image_anger_frame = self:GetWidget("Image_anger_frame")
	Image_anger_frame:SetZOrder( 20 )
	local lst = {
			[1] = { name = "UI_nuqijindutiao1" , dir = MathTool.HORZ_LEFT , posx = 2 , posy = -44 },
			[2] = { name = "UI_nuqijindutiao2" , dir = MathTool.HORZ_RIGHT , posx = -37 , posy = 19 },
			[3] = { name = "UI_nuqijindutiao2" , dir = MathTool.HORZ_LEFT , posx = 39 , posy = 19 },
	}

	for i = 1 , 3 do
		local effectsCon = nil
		self.sp_bomb[i], effectsCon = self:__CreateEffect( {res_name = lst[i].name , dir = lst[i].dir } , Image_anger_frame )
		PosTool.Center(effectsCon , lst[i].posx , lst[i].posy )
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[i] , false)
	end
	
	if cur_anger >= 100 then
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[1] , true )
	else
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[1] , false )
	end
	if cur_anger >= 200 then
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[2] , true )
	else
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[2] , false )
	end
	if cur_anger >= 300 then
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[3] , true )
	else
		GlobalEffectMgr:SetEffectVisible( self.sp_bomb[3] , false )
	end
end

--初始化英雄操作区域
function Modules.BattleView:__InitHeroOperRegion()
	local vo
	local hero_vo_list = GlobalVOMgr:GetHeroList()
	Modules.BattleFormula:SortBattleHeroList(hero_vo_list)

	local panel = self:GetWidget("Image_hero_anger")
	local panel_height = panel:GetContentHeight()
	local item_width = 70
	local item_interval = 15

	local function handle_func(obj_id, for_weak)
		for id, template in pairs(self.__templ_hero_oper_map) do
			if id ~= obj_id then
				template:SetInSpHide(for_weak)
			end
		end
	end

	local allw = 0
	--己方英雄
	self.__first_hero_oper_id = nil
	self.__templ_hero_oper_map = {}
	local Panel_iconlst = self:GetWidget("Panel_iconlst")
	for i, vo in ipairs(hero_vo_list) do
		local template = Modules.TemplBattleHeroOper.New(
		{
			parent_node = Panel_iconlst,
			x = (Panel_iconlst:GetPositionX() - item_interval) - (i - 1) * (item_width + item_interval) ,
			y = Panel_iconlst:GetContentHeight() / 2 - 5,
			obj_id = vo.obj_id,
			sp_effect_notice_func = function(obj_id, for_weak)
				handle_func(obj_id, for_weak)
			end
		})
		template:SetAnchorPoint( 0 , 0.5)
		self.__templ_hero_oper_map[vo.obj_id] = template

		--美国队长
		if vo.info_id == 30301 then
			self.__first_hero_oper_id = vo.obj_id
		end
		allw = allw + item_width
	end

	local Image_hero_anger = self:GetWidget("Image_hero_anger")
	Image_hero_anger:SetCascadeOpacityEnabled(true)
	Image_hero_anger:SetContentSize( Image_hero_anger:GetContentWidth() + allw - item_interval , Image_hero_anger:GetContentHeight() )

	local Panel_autoSecen = self:GetWidget("Panel_autoSecen")
	Panel_autoSecen:SetPosition( Image_hero_anger:GetContentWidth() , Panel_autoSecen:GetPositionY() )

	--助战英雄
	local panel_hero_assist = self:GetWidget("Panel_HeroAssist")
	panel_hero_assist:SetPosition(Image_hero_anger:GetPositionX() - Image_hero_anger:GetContentWidth() - 50 , panel_hero_assist:GetPositionY() )

	local battle_info = self:CallModuleFunc("GetCurInfo")
	if battle_info.assist_hero_id and battle_info.assist_hero_id > 0 then
		self.__template_bt_hero_op_assist = Modules.TemplBattleHeroOperAssist.New(
		{
			parent_node = panel_hero_assist,
			info_id = battle_info.assist_hero_id
		})
		self.__template_bt_hero_op_assist:SetAnchorPoint(0.5, 0.5)
		PosTool.Center(self.__template_bt_hero_op_assist )
		panel_hero_assist:SetVisible(true)
	else
		panel_hero_assist:SetVisible(false)
	end
end

--战役开启处理
function Modules.BattleView:__HandleBattleStart()
	self:__HandleBattleStartForSprite()
	self:__HandleSkipForBattleStart()

	local time = 0.3 
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)

	--英雄血量
	if battle_info.battle_index <= 1 then
		self.__hero_total_hp = GlobalVOMgr:CalcTotalHeroMaxHP()

		self.__hero_hp_bar_bg:SetProgressMax(self.__hero_total_hp)
		self.__hero_hp_bar_bg:SetSpeed( 5 , self.__hero_total_hp*time )
		self.__hero_hp_bar_bg:SetProgress(self.__hero_total_hp)

		self.__hero_hp_bar:SetProgressMax(self.__hero_total_hp)
		self.__hero_hp_bar:SetProgress(self.__hero_total_hp)

		self.__hero_hp_txt:SetText(self.__hero_total_hp.."/"..self.__hero_total_hp)
	end

	--怪物血量
	self:__InitMonsterHp()

	--倒计时
	self:__UpdateRemainTime()

	--波数
	self:__UpdateBattleIndex()

	local battle_info = self:CallModuleFunc("GetCurInfo")

	--章节数据
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)

	--敌方名字
	local enemy_name, enemy_head_id, is_monster = self:__GetEnemyNameAndHead( battle_info )
	self.lab_enemy_name:SetString( enemy_name )

	--暂停按钮
	if config_gamelevel.show_pause_btn ~= Macros.Global.FALSE then
		self.__img_pause:SetVisible(true)
	end
end

--处理战斗跳过信息
function Modules.BattleView:__HandleSkipForBattleStart()
	local battle_info = self:CallModuleFunc("GetCurInfo")
	local config_battle = CPPGameLib.GetConfig("Battle", battle_info.battle_id)
	--TODO：没通关的情况下，每天有若干次免费的跳过次数，其余需要使用钻石
	if not battle_info.no_passed_flag and config_battle and config_battle.skip_time then
		local skip_time = config_battle.skip_time		
		self.__panel_skip:SetVisible(true)
		self.__image_skip:SetGray(true)
		self.__image_skip:RemoveAllChild(true)
		self.__panel_skip:SetTouchEnabled(false)

		local txt_skip = Util:Label(tostring(skip_time), Macros.TypefaceSize.headline, Macros.Color.red)
		self.__image_skip:AddChild(txt_skip)
		PosTool.Center(txt_skip)

		if self.__skip_timer then
			GlobalTimerQuest:CancelQuest(self.__skip_timer)
		end
		self.__skip_timer = GlobalTimerQuest:AddPeriodQuest(
			GlobalCallbackMgr:AddCallBackFunc(function()
				skip_time = skip_time - 1
				txt_skip:SetString(tostring(skip_time))
				if skip_time <= 0 then
					txt_skip:RemoveSelf(true)
					self.__image_skip:SetGray(false)
					self.__panel_skip:SetTouchEnabled(true)

					GlobalTimerQuest:CancelQuest(self.__skip_timer)
					self.__skip_timer = nil
				end
			end), 1, -1)		
	else
		self.__panel_skip:SetVisible(false)
	end	
end

--游戏暂停
function Modules.BattleView:__HandlePasue()
	self.__ref_in_pause:Retain()

	if self.__ref_in_pause:GetRefCount() == 1 then
		self:__HandlePasueForSpriteSkill()
	end
end

--游戏恢复
function Modules.BattleView:__HandleResume()
	self.__ref_in_pause:Release()

	if not self.__ref_in_pause:IsValid() then
		self:__HandleResumeForSpriteSkill()
	end
end

--处理指引相关
function Modules.BattleView:__HandleLead()
	local is_have_lead_tag = CallModuleFunc(ModuleType.BATTLE,"GetHandleLeadTag")
	if is_have_lead_tag then		
		local sp_lead_id,sp_lead_id_two = CallModuleFunc(ModuleType.LEAD,"GetCurLeadSpLeadId")
		if sp_lead_id then
			CallModuleFunc(ModuleType.BATTLE,"SetHandleLeadTag")
			--释放精灵技能
			if sp_lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_1 
					or sp_lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_2
					or sp_lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_3
					or sp_lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_4
					or sp_lead_id == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_5 then
				self:__CreatSpriteSkillLeadEffect()
			elseif sp_lead_id_two then
				if sp_lead_id_two == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_1 
					or sp_lead_id_two == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_2
					or sp_lead_id_two == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_3
					or sp_lead_id_two == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_4
					or sp_lead_id_two == Macros.Game.LeadSpecialID.RELEASE_SPRITE_SKILL_5 then
					self:__CreatSpriteSkillLeadEffect()
				end
			end
			--好友助战指引
			if sp_lead_id == Macros.Game.LeadSpecialID.FRIEND_ASSIST_LEAD or (sp_lead_id_two and sp_lead_id_two == Macros.Game.LeadSpecialID.FRIEND_ASSIST_LEAD) then
				if self.__template_bt_hero_op_assist then
					self.__template_bt_hero_op_assist:__CreateLeadEffect()
				end
			end
		end
	end
end

--处理掉落物
function Modules.BattleView:__HandleDropBox(obj_id, drop_info)
	--掉落物飞入效果
	local fly_time = 1
	local obj = GlobalScene:GetObj(obj_id)
	if obj then
		local obj_model = obj:GetModel()
		local beg_world_pos = NodeUnity.ConvertToWorldPosition(obj_model:GetNode(), COCOPoint(0, 0))
		local tgt_world_pos = NodeUnity.ConvertToWorldPosition(self.__img_dropbox, COCOPoint(0, 0))
		local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, drop_info.img_name)
		ActionManager.GetInstance():ImgMoveToEffectEX(1, imgdata, beg_world_pos, tgt_world_pos, nil, fly_time)				
	end

	--更新掉落物
    CPPActionManager.DelayTo(self.__txt_dropbox_num, fly_time, GlobalCallbackMgr:AddCallBackFunc(function()
    	self:__HandleDropBoxRefresh(drop_info)
    end))
end

--处理掉落物刷新
function Modules.BattleView:__HandleDropBoxRefresh(drop_info)
	if not self:IsOpen() then
		return
	end

	CallModuleFunc(ModuleType.BATTLE, "UpdateDropNum", 1)
	local obj_num = CallModuleFunc(ModuleType.BATTLE, "GetDropNum")
	self.__txt_dropbox_num:SetText(tostring(obj_num))
end

--开启自动
function Modules.BattleView:__AutoOpen( type )
	if type ~= Macros.Game.FBType.ARENA then
		self:CallModuleFunc("SetAutoFight", true)
		return true
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("battle_arena") )
		return 
	end
end

--关闭自动
function Modules.BattleView:__AutoClose( type )
	if type ~= Macros.Game.FBType.ARENA then
		self:CallModuleFunc("SetAutoFight", false)
		return true
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("battle_arena") )
		return
	end
end

--设置自动
function Modules.BattleView:__AutoSetType( type , node )
	if type == Macros.Game.FBType.ARENA then
		local layer = Util:Layout( node:GetContentWidth() , node:GetContentHeight() , function ( ... )
		end )	
		layer:SetAnchorPoint( 0.5 , 0.5 )
		node:AddChild( layer )
		PosTool.Center( layer )

		local lock_iconSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon")
		local lock_icon = Util:Sprite( lock_iconSp )
		lock_icon:SetAnchorPoint( 0.5 , 0.5 )
		node:AddChild( lock_icon )
		PosTool.Center( lock_icon )
		lock_icon:SetScaleFactor( 0.6 )
	end
end

--获取我方名字头像
function Modules.BattleView:__GetMyNameAndHead( battle_info )
	local my_name = CallModuleFunc(ModuleType.ROLE, "GetName")
	local my_head_id = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)

	if battle_info.type == Macros.Game.FBType.LEAD then
		--新手副本敌我双方名字
		my_name = ConfigAdapter.Dungeon.GetLeadBattleNames()

		--新手副本我方头像
		my_head_id = ConfigAdapter.Dungeon.GetLeadBattleHeadID()
	end

	return my_name, my_head_id
end

--获取敌方名字头像
function Modules.BattleView:__GetEnemyNameAndHead( battle_info )
	local enemy_name, enemy_head_id

	--章节数据
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
	--战役数据
	local config_battle = CPPGameLib.GetConfig("Battle", battle_info.battle_id or config_gamelevel.battle_list[1] )
	--怪物数据
    local config_monster = CPPGameLib.GetConfig("Monster", config_battle.representativeId)

	enemy_name = config_monster.name
	enemy_head_id = config_battle.representativeId
	local is_monster = true

	--新手副本
	if battle_info.type == Macros.Game.FBType.LEAD then
		_, enemy_name = ConfigAdapter.Dungeon.GetLeadBattleNames()

	--竞技场
	elseif battle_info.type == Macros.Game.FBType.ARENA then
		is_monster = false
		local _, _, name, id = CallModuleFunc(ModuleType.ARENA, "GetFightResultViewMessage")
		enemy_name = name
		enemy_head_id = id

	--夺宝
	elseif battle_info.type == Macros.Game.FBType.PLUNDERED_TREASURES then
		local data = CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"GetOnPlaye")
    	enemy_name = data.role_name
    	enemy_head_id = data.head_image_id or data.hero_list[1]
    	is_monster = false

	--1V1
	elseif battle_info.type == Macros.Game.FBType.PVP then
		enemy_name = CallModuleFunc(ModuleType.PVP,"GetSaveRandomInfo")
		enemy_head_id = battle_info.target_data.head_icon
		is_monster = false

	--无尽
	elseif battle_info.type == Macros.Game.FBType.CHASM then
		local wave = CallModuleFunc(ModuleType.CHASM, "GetCurWave")
		local monster = CPPGameLib.GetConfig("EndlessLandWave", wave)
		if monster then
			local monster_id = monster.monster_id_list[5]
			--怪物数据
		    local config_monster = CPPGameLib.GetConfig("Monster", monster_id)

			enemy_head_id = monster_id
			enemy_name = config_monster.name
			is_monster = true
		end

	--天梯赛
	elseif battle_info.type == Macros.Game.FBType.LADDER then
		local enemy_info = CallModuleFunc(ModuleType.LADDER, "GetMyOpponent")
		if enemy_info then
			enemy_head_id = enemy_info.head_img_id
			enemy_name = enemy_info.role_name
			is_monster = false
		end
	--积分赛
	elseif battle_info.type == Macros.Game.FBType.INTEGRAL_RACE then
		is_monster = false
		local name, id = CallModuleFunc(ModuleType.INTEGRAL_RACE, "GetIntegralRaceBattleMessage")
		enemy_name = name
		enemy_head_id = id
	--资源战
	elseif battle_info.type == Macros.Game.FBType.RESOURCE_WAR and #battle_info.hero_mirror_list > 0 then		
		local chapter_data = CallModuleFunc(ModuleType.RESOURCE_WAR, "GetChapterData",battle_info.param2)
		if chapter_data and chapter_data.role_name then
			is_monster = false
			enemy_name = chapter_data.role_name
			enemy_head_id = chapter_data.head_img
		end
	end

	return enemy_name, enemy_head_id, is_monster
end

function Modules.BattleView:__ChangeEnemyName( name )
	self.lab_enemy_name:SetString( name )
end

--更新倒计时
function Modules.BattleView:__UpdateRemainTime(remain_time)
	remain_time = remain_time or CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetRemainTime")
	self.__txt_countdown:SetText(TimeTool.TransTimeStamp("MM:SS", remain_time))
end

--更新波数
function Modules.BattleView:__UpdateBattleIndex()
	local battle_info = self:CallModuleFunc("GetCurInfo")
	self.__txt_battle_times:SetText( battle_info.battle_index.."/"..battle_info.battle_num )
end

--初始化怪物血量
function Modules.BattleView:__InitMonsterHp(cur_hp, total_hp)
	self.__monster_cur_hp = cur_hp or GlobalVOMgr:CalcTotalMonsterHP()
	self.__monster_total_hp = total_hp or GlobalVOMgr:CalcTotalMonsterMaxHP()

	self.__monster_hp_bar_bg:SetProgressMax(self.__monster_total_hp)
	self.__monster_hp_bar_bg:SetSpeed( 5 , self.__monster_total_hp*0.3 )
	self.__monster_hp_bar_bg:SetProgress(self.__monster_cur_hp)

	self.__monster_hp_bar:SetProgressMax(self.__monster_total_hp)
	self.__monster_hp_bar:SetProgress(self.__monster_cur_hp)

	self.__monster_hp_txt:SetText(self.__monster_cur_hp.."/"..self.__monster_total_hp)
end

--处理怪物血量变化 
function Modules.BattleView:__HandleMonsterHpChange(change_hp, cur_hp)
	self.__monster_cur_hp = cur_hp or GlobalVOMgr:CalcTotalMonsterHP()
	self.__monster_hp_txt:SetText(self.__monster_cur_hp.."/"..self.__monster_total_hp)
	self.__monster_hp_bar:SetProgress(self.__monster_cur_hp)

	if not self.monster_hp then
		self.monster_hp = true
		if self.__monster_record_hp < self.__monster_cur_hp then
			self.__monster_hp_bar_bg:SetProgress( self.__monster_cur_hp )
			self.monster_hp = nil
		else
			CPPActionManager.DelayTo(self.__hero_hp_bar_bg:GetNode() , 0.5 , GlobalCallbackMgr:AddCallBackFunc(function ()
				self.__monster_hp_bar_bg:SetToProgress( self.__monster_record_hp , 2)
				self.monster_hp = nil
			end))
		end
	end

	self.__monster_record_hp = self.__monster_cur_hp
end

--处理战斗加速
function Modules.BattleView:__HandleChangeACC()
	self:CallModuleFunc("ChangeACCIndex")

	local acc_index = self:CallModuleFunc("GetACCIndex")

	--加速底框
	local imgdata = acc_index ~= 1 and 
	Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_pauseBg_2") or 
	Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_pauseBg_1")
	self.__img_acc:SetImage(imgdata)

	--加速标签
	imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_acc"..acc_index)
	self.__btn_acc:SetNormalImage(imgdata)
	PosTool.Center(self.__btn_acc)
end

--处理QTE连击
function Modules.BattleView:__HandleQTEComboTimes(combo_hit_times)
	self.__sp_qte_perfect:SetVisible(combo_hit_times > 0)
	self.__sp_qte_perfect_num:SetVisible(combo_hit_times > 0)

	combo_hit_times = math.max(combo_hit_times,1)
	combo_hit_times = math.min(combo_hit_times,6)
	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_qte_perfect"..combo_hit_times)
	self.__sp_qte_perfect_num:SetImage(imgdata)
	if combo_hit_times > 0 then
		CPPActionManager.StopAllActions(self.__sp_qte_perfect)
		CPPActionManager.StopAllActions(self.__sp_qte_perfect_num)
		self.__sp_qte_perfect_num:SetScaleFactor(5)
		CPPActionManager.ScaleTo(self.__sp_qte_perfect_num, 0.3, 1)
	end
end