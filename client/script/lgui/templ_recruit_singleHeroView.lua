LogicGUI = LogicGUI or {}

LogicGUI.TemplRecruitSingleHeroView = LogicGUI.TemplRecruitSingleHeroView or BaseClass(GUI.Template)

function LogicGUI.TemplRecruitSingleHeroView:__init(info_id,state,fun,recruit_time)
	self.__is_auto_load = true
	if state ~= Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then
		self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_onecallrecruit")}
	else 
		self.__bg_params = {type = BaseView.BGType.SOLID,color = {0,0,0,100}}
	end
	self.__open_callback = function ()
		self.__fun = fun
		self.__recruit_time = recruit_time
		self.__hero_infoid = info_id
		self.__recruit_state = state
		self:__InitBaseUI()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_SINGLE_RECRUIT)		
	self:InitTemplate(GUI.TemplateRegistry.HERO_SINGLE_RECRUIT_VIEW)
end

function LogicGUI.TemplRecruitSingleHeroView:__InitBaseUI()
	
	--英雄配置
	local hero_config	
	if self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then
		hero_config = CPPGameLib.GetConfig("Monster",self.__hero_infoid)
	else
		hero_config = CPPGameLib.GetConfig("Hero",self.__hero_infoid)
	end
	--图片适配
    self:GetWidget("Image_2_0"):IgnoreContentAdaptWithSize(true)
	--特效容器
	self.__panel_specialeffects = self:GetWidget("Panel_specialEffects")
	self.__panel_specialeffects:SetZOrder(-2)
	--继续文字
	self.__text_touscreencontinute = self:GetWidget("Text_touScreenContinute")
	self.__text_touscreencontinute:SetTouchEnabled(true)

	local str = self.__recruit_state == Macros.Game.HERO_RECRUIT.MORE_RECRUIT and CPPGameLib.GetString("hero_recruit_touchscreen")
				 or (self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW and CPPGameLib.GetString("hero_recruit_touchscreen") or "")
	Util:WidgetLabel(self.__text_touscreencontinute,str, Macros.TypefaceSize.headline,Macros.Color.blinkcontent,0,0,Macros.Color.common_stroke)		
	--副本背景
	self.__image_dungeonBg = self:GetWidget("Image_dungeonBg")
	self.__image_dungeonBg:SetZOrder(-3)
	self.__image_dungeonBg:SetVisible(self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW)
	if self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then		
		self.__image_dungeonBg:SetPosition(self.__image_dungeonBg:GetPositionX(),self.__image_dungeonBg:GetPositionY() + 50)
		self:GetWidget("Panel_oneCallRecruit"):SetPosition(self:GetWidget("Panel_oneCallRecruit"):GetPositionX(),-50)
		self.__text_touscreencontinute:SetPosition(self.__text_touscreencontinute:GetPositionX(),self.__text_touscreencontinute:GetPositionY() + 30)
		self.__image_dungeonBg:IgnoreContentAdaptWithSize(true)
		self.__image_dungeonBg:SetImage(Resource.PathTool.GetUIResPath("bigbg/bg_sp_dungeon_heroinfo"),TextureResType.LOCAL)
	end
	--继续按钮
	self.__button_continnueRecruit = self:GetWidget("Button_continnueRecruit")
	--确定按钮
	self.__button_sure = self:GetWidget("Button_sure")
	--继续招募按钮
	if (self.__recruit_state == Macros.Game.HERO_RECRUIT.LOW_RECRUIT) or (self.__recruit_state == Macros.Game.HERO_RECRUIT.HIGH_RECRUIT) then
		Util:WidgetButton(self.__button_continnueRecruit,CPPGameLib.GetString("hero_recruitContinute"),function ()
			if self.__is_all_annimation_run then
				self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_UPDATE,Macros.Game.HERO_RECRUIT_DETAIL_TYPE.RECRUIT_ONE_TIME,true)
				self:DeleteMe()
			end
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_red)
		--免费更新
		self._Recruit_Update_Free_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_RECRUIT_FREE_UPDATE, function()
			if self.__is_all_annimation_run then
				self.__text_freetimes:SetVisible(true)
				self.__image_recruitcoin:SetVisible(false)
				self.__text_recruitcoinnum:SetVisible(false)
				self.__text_haveown:SetVisible(false)
			end
		end)
		--确定
		Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("hero_awaken_buttonText4"),function ()
			if self.__is_all_annimation_run then
				self:FireNextFrame(Macros.Event.HeroModule.HERO_RECRUIT_UPDATE,Macros.Game.HERO_RECRUIT_DETAIL_TYPE.RECRUIT_ONE_TIME)
				self:DeleteMe()
			end
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)		
	else	
		local function close_func()
			if self.__is_all_annimation_run then
				self:FireNextFrame(Macros.Event.HeroModule.HERO_MORE_RECRUIT_CONTINUTE)
				self:DeleteMe()
				if self.__fun then
					self.__fun()
				end
			end
		end
		WidgetUnity.SetWidgetListener(self:GetWidget("Panel_oneCallRecruit"), nil, nil, close_func)
		WidgetUnity.SetWidgetListener(self.__text_touscreencontinute, nil, nil, close_func)
	end
	--恭喜飘带
	self.__Image_congratrulation = self:GetWidget("Image_congratrulation")
	--英雄类型
	self.__image_type = self:GetWidget("Image_type")
	local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON,"icon_profession_"..hero_config.profession,true)
	self.__image_type:SetImage(image_data) 
	--英雄阵营
	self.__image_camp = self:GetWidget("Image_camp")
	self.__image_camp:SetVisible(self.__recruit_state ~= Macros.Game.HERO_RECRUIT.DUNGEON_SHOW)
	if self.__recruit_state ~= Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then 		
		image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"camp_icon_"..hero_config.camp,true)
		self.__image_camp:SetImage(image_data) 
	end
	--英雄名字
	self.__hero_name = self:GetWidget("Text_name")
	Util:WidgetLabel(self.__hero_name,"", Macros.TypefaceSize.popup)
	self.__hero_name:SetText(hero_config.name)
	local quality = hero_config.init_quality and hero_config.init_quality or hero_config.quality
	self.__hero_name:SetColor(unpack(Macros.Quality[quality]))
	self.__hero_name:SetStrokeColor(unpack(Macros.Quality_Stroke[quality]))
	--英雄资质
	local text_intelligence = self:GetWidget("Text_intelligence")
	Util:WidgetLabel(text_intelligence,"", Macros.TypefaceSize.popup,Macros.Color.golden,0,0,Macros.Color.common_stroke)		
	--英雄定位
	Util:WidgetLabel(self:GetWidget("Text_heroLocation"),CPPGameLib.GetString("hero_recruit_location"), Macros.TypefaceSize.largish,Macros.Color.orange,0,0,Macros.Color.orange_stroke)
	--定位描述
	local text_locationdes = self:GetWidget("Text_locationDes")
	Util:WidgetLabel(text_locationdes,"", Macros.TypefaceSize.popup,Macros.Color.golden,0,0,Macros.Color.common_stroke)	
	--标记是否拥有怒气技能
	self.__is_have_angryskill = false
	local angry_info
	local skill_list
	--是否合击的主将
	local is_initiative_hero
	--是否有资格
	local is_show_text
	--定位描述
	local location_str = ""
	--资质
	local intelligence_str
	--副本怪物
	if self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then
		if hero_config.hero_id then
			local hero_info = CPPGameLib.GetConfig("Hero",hero_config.hero_id)
			skill_list = hero_info.skill_list
			is_initiative_hero = hero_info.joint_hero_list ~= nil
			is_show_text = hero_info.joint_hero_list ~= nil
			location_str = hero_config.location_des and hero_config.location_des or hero_info.location_des
			text_intelligence:SetVisible(hero_info.Qualifications and true or false)
			intelligence_str = hero_info.Qualifications
		else
			skill_list = hero_config.skill_list
			is_initiative_hero = false
			is_show_text = false
			location_str = hero_config.location_des
			text_intelligence:SetVisible(hero_config.Qualifications and true or false)
			intelligence_str = hero_config.Qualifications
		end
	else
		skill_list = hero_config.skill_list
		is_initiative_hero = hero_config.joint_hero_list ~= nil
		is_show_text = true
		location_str = hero_config.location_des
		text_intelligence:SetVisible(hero_config.Qualifications and true or false)
		intelligence_str = hero_config.Qualifications
	end
	text_locationdes:SetText(location_str or "")
	text_intelligence:SetText(CPPGameLib.GetString("hero_intelligence",intelligence_str and intelligence_str or 0))
	for _,skill_id in ipairs(skill_list) do
		local skill_info = CPPGameLib.GetConfig("Skill",skill_id)
		if skill_info and skill_info.type == Macros.Game.SkillType.ANGER then
			angry_info = skill_info
			self.__is_have_angryskill = true
			break
		end
	end
	if self.__is_have_angryskill then
		--英雄怒气技能
		Util:WidgetLabel(self:GetWidget("Text_heroAngry"),CPPGameLib.GetString("hero_recruit_angry"), Macros.TypefaceSize.largish,Macros.Color.orange,0,0,Macros.Color.orange_stroke)
		--英雄怒气技能名字
		Util:WidgetLabel(self:GetWidget("Text_angrySkillName"),angry_info.name, Macros.TypefaceSize.popup,Macros.Color.golden,0,0,Macros.Color.common_stroke)
		--英雄怒气技能描述
		Util:WidgetLabel(self:GetWidget("Text_angrySkillDes"),angry_info.brief_des and angry_info.brief_des or "", Macros.TypefaceSize.largish,Macros.Color.btn,223,80)		
	end
	--标记是否拥有合击技能
	self.__is_have_jointskill = false
	local joint_info
	for _,skill_id in ipairs(skill_list) do
		local skill_info = CPPGameLib.GetConfig("Skill",skill_id)
		if skill_info and skill_info.type == Macros.Game.SkillType.JOINT and is_initiative_hero and is_show_text then
			joint_info = skill_info
			self.__is_have_jointskill = true
			break
		elseif skill_info and skill_info.type == Macros.Game.SkillType.JOINT_ATTACHED and (not is_initiative_hero) and is_show_text then
			joint_info = skill_info
			self.__is_have_jointskill = true
			break
		end
	end
	if self.__is_have_jointskill then
		--英雄合击技能
		Util:WidgetLabel(self:GetWidget("Text_heroTogether"),CPPGameLib.GetString("hero_recruit_joint"), Macros.TypefaceSize.largish,Macros.Color.orange,0,0,Macros.Color.orange_stroke)
		--英雄合击技能名字
		Util:WidgetLabel(self:GetWidget("Text_togetherSkillName"),joint_info.name, Macros.TypefaceSize.popup,Macros.Color.golden,0,0,Macros.Color.common_stroke)
		--英雄合击技能描述
		Util:WidgetLabel(self:GetWidget("Text_togetherSkillDes"),joint_info.brief_des and joint_info.brief_des or "", Macros.TypefaceSize.largish,Macros.Color.btn,223,80)		
	end
	--英雄模型容器
	self.__panel_heromodule = self:GetWidget("Panel_heroModule")
	self.__panel_heromodule:SetZOrder(-1)
	--消耗货币
	self.__image_recruitcoin = self:GetWidget("Image_recruitCoin") 
	self.__image_recruitcoin:IgnoreContentAdaptWithSize(true)
	--消耗货币数量
	self.__text_recruitcoinnum = self:GetWidget("Text_recruitCoinNum")
	Util:WidgetLabel(self.__text_recruitcoinnum,"", Macros.TypefaceSize.largish,Macros.Color.btn)
	--本次免费
	self.__text_freetimes = self:GetWidget("Text_freeTimes")
	Util:WidgetLabel(self.__text_freetimes,CPPGameLib.GetString("hero_recruitForFreeOnce"), Macros.TypefaceSize.largish,Macros.Color.btn)
	--拥有数量
	self.__text_haveown = self:GetWidget("Text_haveOwn")
	Util:WidgetLabel(self.__text_haveown,"", Macros.TypefaceSize.largish,Macros.Color.btn)	
	if self.__recruit_time then
		--高级召唤提示
		local str = ""
		if ConfigAdapter.Common.GetHighRecruitMustShowHero() - self.__recruit_time ~= 1 then
			str = CPPGameLib.GetString("hero_recruitEncourage", ConfigAdapter.Common.GetHighRecruitMustShowHero() - self.__recruit_time)
		else
			str = CPPGameLib.GetString("hero_recruitEncourage_1")
		end
		if not GlobalCustomizedLogicAdapterMgr:Call("IsHideRecruitEncourageTips") then
			self.__recruit_encouragetext = Util:RichText(str,Macros.TypefaceSize.headlineII,0,0,Macros.Color.blinkcontent_hex,Macros.Color.common_stroke_hex,2)
			self.__recruit_encouragetext:SetAnchorPoint(0.5,0.5)
			self:GetWidget("Panel_oneCallRecruit"):AddChild(self.__recruit_encouragetext)
			self.__recruit_encouragetext:SetPosition(480,165)
			self.__recruit_encouragetext:SetVisible(false)
			self.__recruit_encouragetext:SetOpacity(0)
		end
	end
	--播放特效
	self:__RunEffect()
end

function LogicGUI.TemplRecruitSingleHeroView:__RunEffect()	
	--英雄配置
	local hero_config
	if self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then
		hero_config = CPPGameLib.GetConfig("Monster",self.__hero_infoid)
	else
		hero_config = CPPGameLib.GetConfig("Hero",self.__hero_infoid)
	end
	-- 闪光特效
	local effectid,effect = self:__CreateEffect({res_name = "nuqi_qishou",loop = false},self.__panel_specialeffects,3)
	PosTool.Center(effect)
	self.__delay_timer1 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self.__delay_timer1 = nil
		--转圈
		effectid, effect = self:__CreateEffect({res_name = "UI_boss_xinxi",loop = true}, self.__panel_specialeffects,2)
		PosTool.Center(effect)
		local hero_id
		local hero_dir 
		if self.__recruit_state == Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then
			hero_id = hero_config.hero_id and hero_config.hero_id or hero_config.model_id
			hero_dir = MathTool.HORZ_LEFT
		else
			hero_id = self.__hero_infoid
			hero_dir = MathTool.HORZ_RIGHT
		end
		self.__hero_module = LogicGUI.Model.New({info_id = hero_id, parent = self.__panel_heromodule,dir = hero_dir})
		self.__hero_module:SetPosition(2.5,2.5)
		self.__hero_module:SetScaleFactor(hero_config.show_scale)
		--品质
		if self.__recruit_state ~= Macros.Game.HERO_RECRUIT.DUNGEON_SHOW then
			local quality = hero_config.init_quality and hero_config.init_quality or hero_config.quality
			self:RunSpecialEffectAction(quality)
		end
		--恭喜飘带
		self.__Image_congratrulation:SetVisible(self.__recruit_state ~= Macros.Game.HERO_RECRUIT.DUNGEON_SHOW)
		--播放动画
		self:__RunAnimation()				
	end), 0.3)

	-- --光柱特效
	-- local effect = nil
	-- if self.LightEffect_Id then
	-- 	self:__DelEffect(self.LightEffect_Id)
	-- 	self.LightEffect_Id = nil
	-- end
	-- --背景特效
	-- if self.Effect_Id then
	-- 	self:__DelEffect(self.Effect_Id)
	-- 	self.Effect_Id = nil
	-- end	
	-- self.LightEffect_Id, effect = self:__CreateEffect({id = 100023,loop = false}, self.__panel_heromodule,2)
	-- PosTool.Center(effect)	
end

function LogicGUI.TemplRecruitSingleHeroView:__SetPanelData()
	if self.__recruit_state == Macros.Game.HERO_RECRUIT.LOW_RECRUIT or self.__recruit_state == Macros.Game.HERO_RECRUIT.HIGH_RECRUIT then
		--消耗货币
		local IsFree = nil
		if self.__recruit_state == Macros.Game.HERO_RECRUIT.LOW_RECRUIT then
			IsFree = CallModuleFunc(ModuleType.HERO,"GetHeroLowRecruitFreeState")		
			if not IsFree then
				local type,LowRecruitItemInfo = ObjIDTool.GetInfo(ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
				local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetLowRecruitConsumeGoodID())
				local friend_point = CallModuleFunc(ModuleType.ROLE,"GetTotalFriendShipPoint")		
				local value_list = ConfigAdapter.Common.GetFriendshipPointConsume()	
				if recruitGoodNums > 0 or friend_point < value_list[1] then
					self.__image_recruitcoin:SetImage(Resource.PathTool.GetItemPath(LowRecruitItemInfo.res_id),TextureResType.LOCAL)
				else 
					local sp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT)
					self.__image_recruitcoin:SetImage(sp)
				end
				self.__text_recruitcoinnum:SetText((recruitGoodNums > 0 or friend_point < value_list[1]) and 1 or value_list[1])				
				self.__text_haveown:SetText(CPPGameLib.GetString("hero_recruitHaveOwn",(recruitGoodNums > 0 or friend_point < value_list[1]) and recruitGoodNums or friend_point))
			end
		else
			IsFree = CallModuleFunc(ModuleType.HERO,"GetHeroHighRecruitFreeState")		
			if not IsFree then
				local recruitGoodNums = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
				if recruitGoodNums >= 1 then
					local type,HighRecruitItemInfo = ObjIDTool.GetInfo(ConfigAdapter.Common.GetHighRecruitConsumeGoodID())
					self.__image_recruitcoin:SetImage(Resource.PathTool.GetItemPath(HighRecruitItemInfo.res_id),TextureResType.LOCAL)
					self.__text_recruitcoinnum:SetText(1)
					self.__text_haveown:SetText(CPPGameLib.GetString("hero_recruitHaveOwn",recruitGoodNums))
				else
					self.__image_recruitcoin:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..Macros.Game.RolePropType.DIAMOND))
					self.__text_recruitcoinnum:SetText(ConfigAdapter.Common.GetHighRecruitConsumeGoods())
					local diamonds = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
					self.__text_haveown:SetText(CPPGameLib.GetString("hero_recruitHaveOwn",diamonds))
				end									
			end
		end
		self:__SetPanelVisible(1,IsFree)
	else
		self:__SetPanelVisible(2)
	end
end

function LogicGUI.TemplRecruitSingleHeroView:__SetPanelVisible(state,IsFree)
	--免费文字，其他文字
	local free_txt_visible,other_txt_visible
	if state == 1 then
		free_txt_visible = IsFree
		other_txt_visible = not IsFree
	else
		free_txt_visible = false
		other_txt_visible = false
	end
	self.__text_freetimes:SetVisible(free_txt_visible)
	self.__image_recruitcoin:SetVisible(other_txt_visible)
	self.__text_recruitcoinnum:SetVisible(other_txt_visible)
	self.__text_haveown:SetVisible(other_txt_visible)
	self.__button_continnueRecruit:SetVisible(state == 1)		
	self.__button_sure:SetVisible(state == 1)
	self.__text_touscreencontinute:SetVisible(state == 2)
end

--运行编辑器动画
function LogicGUI.TemplRecruitSingleHeroView:__RunAnimation()
	self:PlayActionTimeline("animation_heroinfo",false,function ()
		self:PlayActionTimeline("animation_herolocation",false,function ()
			if self.__is_have_angryskill then
				self:PlayActionTimeline("animation_heroangry",false,function ()
					if self.__is_have_jointskill then
						local dig = 10
						self.__delay_timer2 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
							self.__delay_timer2 = nil
							CPPActionManager.JumpBy(self:GetWidget("Panel_oneCallRecruit"),0.03,0,dig,0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
								CPPActionManager.JumpBy(self:GetWidget("Panel_oneCallRecruit"),0.03,0,-(2*dig),0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
									CPPActionManager.JumpBy(self:GetWidget("Panel_oneCallRecruit"),0.03,0,dig,0,1)
								end))
							end))					
						end), 0.53)
						
						self:PlayActionTimeline("animation_herojoint",false,function ()
							self:__AfterRunAnimation()
						end)
					else
						self:__AfterRunAnimation()
					end
				end)
			else
				self:__AfterRunAnimation()
			end
		end)
	end)	
end

--动画结束后
function LogicGUI.TemplRecruitSingleHeroView:__AfterRunAnimation()
	self:__SetPanelData()
	local bri_time = 0.1
	self.__delay_timer3 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self.__delay_timer3 = nil
		CPPActionManager.FadeIn(self.__button_continnueRecruit,bri_time,nil)
		CPPActionManager.FadeIn(self.__button_sure,bri_time,nil)
		CPPActionManager.FadeIn(self.__image_recruitcoin,bri_time,nil)
		CPPActionManager.FadeIn(self.__text_recruitcoinnum,bri_time,nil)
		CPPActionManager.FadeIn(self.__text_freetimes,bri_time,nil)
		CPPActionManager.FadeIn(self.__text_haveown,bri_time,nil)
		if self.__recruit_encouragetext then
			self.__recruit_encouragetext:SetVisible(true)
			CPPActionManager.FadeIn(self.__recruit_encouragetext,bri_time,nil)
		end
		CPPActionManager.FadeIn(self.__text_touscreencontinute,bri_time,GlobalCallbackMgr:AddCallBackFunc(function()
			self.__is_all_annimation_run = true	
			self:__CheckSectionOperateSP()
		end))					
	end), 0.4)
end


function LogicGUI.TemplRecruitSingleHeroView:RunSpecialEffectAction(init_quality)
	if init_quality <= Macros.Game.QualityType.BLUE then
		self.Effect_Id, effect = self:__CreateEffect({id = 100019}, self.__panel_specialeffects,1)
	elseif init_quality == Macros.Game.QualityType.PURPLE then
		self.Effect_Id, effect = self:__CreateEffect({id = 100020}, self.__panel_specialeffects,1)
	elseif init_quality == Macros.Game.QualityType.ORANGE then
		self.Effect_Id, effect = self:__CreateEffect({id = 100018}, self.__panel_specialeffects,1)
	elseif init_quality == Macros.Game.QualityType.RED then
		self.Effect_Id, effect = self:__CreateEffect({res_name = "UI_zhaommubeiguanghongse"}, self.__panel_specialeffects,1)		
	end	
	PosTool.Center(effect)
end

function LogicGUI.TemplRecruitSingleHeroView:__delete()
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end

	for i=1,3 do
		if self["__delay_timer"..i] then
	        GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
	        self["__delay_timer"..i] = nil
	    end
	end
	
end

--新手指引特殊操作检测
function LogicGUI.TemplRecruitSingleHeroView:__CheckSectionOperateSP()
	--选择主线关卡（关卡ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_OBTAIN_VIEW_CONTINUE, self.__text_touscreencontinute)				

	--召唤英雄展示点击继续
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RECRUIT_HERO_OBTAIN_VIEW_CONTINUE, self.__button_sure)				
end
