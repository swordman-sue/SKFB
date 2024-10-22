
Modules = Modules or {}

--战斗英雄操作模板
Modules.TemplBattleHeroOper = Modules.TemplBattleHeroOper or BaseClass(GUI.Template)

local ZorderMap = 
{
	HEAD = 2,
	CD = 3,
	PROF = 4,
	ASSIST_NAME = 5,
}

--[[
@ params：
	parent_node
	x, y
	obj_id
	sp_effect_notice_func
--]]
function Modules.TemplBattleHeroOper:__init(params)
	self.__layout_mode = GUI.Template.LayoutMode.CODE	

	self.__params = params

	self.__ref_in_pause = RefObj.New()

	self:InitTemplate(nil, params.parent_node)
	self:SetPosition(params.x, params.y)

	WidgetUnity.SetWidgetListener(self.__root_widget, 
		function(x, y)
			self.__touch_beg_pos = self.__touch_beg_pos or Vector2D.New(x, y)
			self.__touch_beg_pos.x = x
			self.__touch_beg_pos.y = y
		end,
		function(x, y)
			-- self:__CheckShowAngerOrJointSkillSpEffect(x, y)
		end, 
		function(x, y)
			self:__CheckTriggerAngerOrJointSkill(x, y)
		end,
		function(x, y)
			self:__CheckTriggerAngerOrJointSkill(x, y)
		end)

	self:CheckAngerOrJointReleasableEffect()
	self:CheckAngerOrJointReleasableEffectPlus()

	local cd_info = self:__GetAngerOrJointCommonCDInfo()
	if cd_info and cd_info.in_cd then
		self:__HandleAngerOrJointSkillCD()
		self:__HandleAngerOrJointSkillCD(true, cd_info.cd_time - cd_info.cd_elapse_time)
	end
end

function Modules.TemplBattleHeroOper:__delete()
	if self.__hp_bar then
		self.__hp_bar:DeleteMe()
		self.__hp_bar = nil
	end
end

function Modules.TemplBattleHeroOper:__RegistAllEvents()
	--根据合击副将受限类BUFF解除情况，更新怒气满特效	
	local function update_anger_enough_effect(obj_id)
		if not self:__IsAngerEnough() then
			return
		end

		local joint_attached_hero_map = Skill.SkillLogic:GetAttachedObjsOfJointSkill(self.__params.obj_id)
		if joint_attached_hero_map then
			if joint_attached_hero_map[obj_id] then
				local can_release_joint_skill = Skill.SkillLogic:CheckJointSkillCondition(self.__params.obj_id)
				if can_release_joint_skill ~= self.__can_release_joint_skill then
					self:__CreateAngerEnoughEffect()

					if self:__IsAngerEnough(true) then
						self:__CreateAngerEnoughEffect(true)
					end
				end
			end
		end
	end
	--英雄死亡
	self:BindGlobalEvent(Macros.Event.SceneObj.HERO_DEAD, function(_, obj_id)
		if self.__params.obj_id == obj_id then
			self:__HandleDead()
		else
			update_anger_enough_effect(obj_id)
		end
	end)
	--禁止攻击改变	
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_FORBID_ATTACK_CHANGE, function(_, obj_id)
		update_anger_enough_effect(obj_id)
	end)
	--禁止特殊技能改变
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_FORBID_SP_SKILL_CHANGE, function(_, obj_id)
		update_anger_enough_effect(obj_id)
	end)
	--禁止怒气/合击技能改变
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_FORBID_ANGER_OR_JOINT_SKILL_CHANGE, function(_, obj_id)
		update_anger_enough_effect(obj_id)
	end)

	--怒气/合击技能释放的事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE, function(_, obj_id)
		if obj_id == self.__params.obj_id then
			self:__HandleAngerOrJointSkillCD()
			self:__CreateAngerOrJointReleaseEffect()
			self:__DeleteAngerOrJointToReleaseEffect()
		else
			self:__HandlePasue()
		end
	end)

	--怒气/合击技能释放完成的事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_SKILL_RELEASE_END, function(_, obj_id)
		if obj_id == self.__params.obj_id then
			self:__HandleAngerOrJointSkillCD(true)
		else
			self:__HandleResume()
		end
	end)	

	--游戏暂停
	self:BindGlobalEvent(Macros.Event.SpLogicModule.PAUSE, function(_)
		self:__HandlePasue()
	end)	

	--游戏恢复
	self:BindGlobalEvent(Macros.Event.SpLogicModule.RESUME, function(_)
		self:__HandleResume()
	end)	

	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		self:__CheckSectionOperateSP()
	end)  

	--QTE事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE, function(_, obj_id)
		if obj_id == self.__params.obj_id then
			self:__CreateAngerOrJointReleaseEffect()
			self:__DeleteAngerOrJointToReleaseEffect()
		else
			self:__HandlePasue()
		end
	end)	

	--QTE结束事件
	self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_ANGER_OR_JOINT_QTE_END, function(_, obj_id)
		if obj_id ~= self.__params.obj_id then
			self:__HandleResume()
		end
	end)
end

function Modules.TemplBattleHeroOper:__CreateWidgetFromLuaCode()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	local obj_vo = obj:GetVO()

	local layout = Util:Layout()
	layout:SetAnchorPoint(0, 0)
	layout:SetContentSize(70, 128)
	layout:SetTouchEnabled(true)

	--头像
    self.__icon_head = LogicGUI.IconLayer.New(false, false)
    self.__icon_head:SetData({id = obj_vo.info_id, obj_info = {quality_level = obj_vo.quality_level}})
    layout:AddChild(self.__icon_head:GetNode(), ZorderMap.HEAD)
    PosTool.Center(self.__icon_head:GetNode(), 0 , 10 )
    self.__icon_head:SetScaleFactor( 0.95 )

	--职业
	local img_profession = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "icon_profession_"..obj_vo.profession, true))
	img_profession:SetAnchorPoint(0, 1)
	img_profession:SetPosition( 0 , 80 )
	self.__icon_head:AddChild(img_profession, ZorderMap.PROF)


	local prog_hp_hero_bg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_hp_hero_bg") )
	prog_hp_hero_bg:SetAnchorPoint( 0.5 , 0.5 )
	prog_hp_hero_bg:SetPosition( layout:GetContentWidth()/2 , 28 )
	layout:AddChild( prog_hp_hero_bg )

	--血条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "prog_hp_hero")
	self.__hp_bar = GUI.ProgressBarLayer.New(imgdata_bar, nil, obj_vo.hp, obj_vo.max_hp)	
	layout:AddChild(self.__hp_bar:GetNode())
	PosTool.CenterBottom(self.__hp_bar, 0, 28)

	if obj_vo.is_assist then
		local lab_assist = Util:Label(CPPGameLib.GetString("battle_assist_hero"), Macros.TypefaceSize.minimum, Macros.Color.white, nil, nil, Macros.Color.white_stroke)
		lab_assist:SetAnchorPoint(0.5, 0)
		layout:AddChild(lab_assist, ZorderMap.ASSIST_NAME)		
		PosTool.CenterBottom(lab_assist, 0, 30)
	end

	return layout
end

--对象是否死亡
function Modules.TemplBattleHeroOper:__IsObjDead()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj or obj:IsDead() then
		return true
	end
end

--更新血量
function Modules.TemplBattleHeroOper:UpdateHP(var)
	if self.__hp_bar then
		self.__hp_bar:SetProgress(var)
	end
end

--检测怒气/合击技能可释放特效
function Modules.TemplBattleHeroOper:CheckAngerOrJointReleasableEffect()
	local var = self:__IsAngerEnough()
	if var == self.__anger_enough_record then
		return
	end
	self.__anger_enough_record = var

	if not self.__anger_enough_record then
		self:__DeleteAngerEnoughEffect()
	else
		self:__HandleAngerEnough()
	end
end

--检测真.怒气、真.合击技能可释放特效
function Modules.TemplBattleHeroOper:CheckAngerOrJointReleasableEffectPlus()
	local var = self:__IsAngerEnough(true)
	if var == self.__anger_enough_record_plus then
		return
	end
	self.__anger_enough_record_plus = var

	if not self.__anger_enough_record_plus then
		self:__DeleteAngerEnoughEffect(true)
	else
		self:__HandleAngerEnough(true)
	end
end

--添加强化版怒气/合击技能指引特效
function Modules.TemplBattleHeroOper:CreateAngerOrJointSkillLeadEffect()
	self:DeleteAngerOrJointSkillLeadEffect()

	local effect
	self.__anger_or_joint_skill_lead_effect_id, effect = self:__CreateEffect({res_name = "UI_zhiyintuodong1"}, self.__icon_head:GetNode() , LogicGUI.IconLayer.InnerZOrder.MAX)
	if effect then
		PosTool.Center(effect)
	end	
end

--移除强化版怒气/合击技能指引特效
function Modules.TemplBattleHeroOper:DeleteAngerOrJointSkillLeadEffect()
	if self.__anger_or_joint_skill_lead_effect_id then
		self:__DelEffect(self.__anger_or_joint_skill_lead_effect_id)
		self.__anger_or_joint_skill_lead_effect_id = nil
	end
end

function Modules.TemplBattleHeroOper:SetInSpHide(var)
	self.__in_sp_hide = var
	GlobalEffectMgr:SetEffectVisible(self.__anger_full_effect_id, self:__CanAngerOrJointEffectVisible())
	GlobalEffectMgr:SetEffectVisible(self.__anger_full_effect_id_plus, self:__CanAngerOrJointEffectVisible())
end

--特效
function Modules.TemplBattleHeroOper:ShowQTEGradeEffect()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj or obj:IsDead() then
		return
	end
	return obj:ShowQTEGradeEffect()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--怒气是否充足
function Modules.TemplBattleHeroOper:__IsAngerEnough(is_plus)
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj or obj:IsDead() then
		return
	end
	return obj:IsAngerEnough(is_plus)
end

--处理怒气充足
function Modules.TemplBattleHeroOper:__HandleAngerEnough(is_plus)
	if not is_plus then
		self:__CreateAngerEnoughEffect()
		self:__CheckSectionOperateSP()
	else
		self:__CreateAngerEnoughEffect(true)
	end
end

--是否在QTE状态
function Modules.TemplBattleHeroOper:__IsInQTEStatus()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj or obj:IsDead() then
		return
	end
	return obj:IsInState("qte")
end

--是否在转换QTE镜头
function Modules.TemplBattleHeroOper:__IsInTurningQTECamera()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj or obj:IsDead() then
		return
	end
	return obj:IsInTurningQTECamera()
end

--处理自己阵亡
function Modules.TemplBattleHeroOper:__HandleDead()
	self.__icon_head:SetGray(true)
	self:__DeleteAngerEnoughEffect()
	self:__DeleteAngerEnoughEffect(true)
	if self.__prog_cd then
		self.__prog_cd:RemoveSelf(true)
		self.__prog_cd = nil
	end

	self:UpdateHP(0)
end

--处理怒气/合击技能CD
function Modules.TemplBattleHeroOper:__HandleAngerOrJointSkillCD(do_action, cd_time)
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj then
		return
	end

	if not self.__prog_cd then
		local sp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "bg_cd"))
		self.__prog_cd = ProgressTimer.CreateWithImageLabel(sp)
		self.__prog_cd:SetType(ProgressTimer.TypeRadial)
		self.__prog_cd:SetMidpoint(COCOPoint(0.5, 0.5))
		self.__prog_cd:SetAnchorPoint(0.5, 1)
		self.__prog_cd:SetReverseDirection(true)
		self:AddChild(self.__prog_cd, ZorderMap.CD)
		PosTool.CenterTop(self.__prog_cd, 0, -14)
	end

	if do_action then
		local obj_vo = obj:GetVO()
		CPPActionManager.ProgressTo(self.__prog_cd, cd_time or obj_vo.anger_or_joint_common_cd_time, 0, GlobalCallbackMgr:AddCallBackFunc(function()
			GlobalEffectMgr:SetEffectVisible(self.__anger_full_effect_id, self:__CanAngerOrJointEffectVisible(true))
			GlobalEffectMgr:SetEffectVisible(self.__anger_full_effect_id_plus, self:__CanAngerOrJointEffectVisible(true))
		end))
	else
		self.__prog_cd:SetPercentage(100)
		CPPActionManager.StopAllActions(self.__prog_cd)

		GlobalEffectMgr:SetEffectVisible(self.__anger_full_effect_id, false)
		GlobalEffectMgr:SetEffectVisible(self.__anger_full_effect_id_plus, false)		
	end
end

--处理暂停
function Modules.TemplBattleHeroOper:__HandlePasue()
	self.__ref_in_pause:Retain()
	if self.__ref_in_pause:GetRefCount() == 1 then
		if self.__prog_cd then
			CPPActionManager.PauseAction(self.__prog_cd)
		end
	end
end

--处理恢复
function Modules.TemplBattleHeroOper:__HandleResume()
	self.__ref_in_pause:Release()
	if not self.__ref_in_pause:IsValid() then
		if self.__prog_cd then
			CPPActionManager.ResumeAction(self.__prog_cd)
		end
	end
end

--创建怒气/合击特效
function Modules.TemplBattleHeroOper:__CreateAngerEnoughEffect(is_plus, effect_name)
	self:__DeleteAngerEnoughEffect(is_plus)

	self.__can_release_joint_skill = Skill.SkillLogic:CheckJointSkillCondition(self.__params.obj_id)

	if self:__IsObjDead() then
		return
	end

	if not is_plus then
		local anger_full_effect_name = self.__can_release_joint_skill and "UI_hejitouxiangzhuangtai" or "UI_nuqitouxiangzhuangtai"
		local anger_full_effect
		self.__anger_full_effect_id, anger_full_effect = self:__CreateEffect({res_name = effect_name or anger_full_effect_name}, self.__icon_head:GetNode() , LogicGUI.IconLayer.InnerZOrder.MAX)
		if anger_full_effect then
			PosTool.Center(anger_full_effect)
			anger_full_effect:SetVisible(self:__CanAngerOrJointEffectVisible())
		end
	else
		local anger_full_effect_plus		
		self.__anger_full_effect_id_plus, anger_full_effect_plus = self:__CreateEffect({res_name = effect_name or "UI_nuqitouxiangjiaqiang"}, self.__icon_head:GetNode() , LogicGUI.IconLayer.InnerZOrder.MAX)
		if anger_full_effect_plus then
			PosTool.Center(anger_full_effect_plus)
			anger_full_effect_plus:SetVisible(self:__CanAngerOrJointEffectVisible())
		end
	end
end

--销毁怒气/合击特效
function Modules.TemplBattleHeroOper:__DeleteAngerEnoughEffect(is_plus)
	self:__DeleteAngerOrJointToReleaseEffect()

	if not is_plus then
		if self.__anger_full_effect_id then
			self:__DelEffect(self.__anger_full_effect_id)
			self.__anger_full_effect_id = nil
		end
	else
		if self.__in_show_anger_or_joint_skill_sp_effect then
			self.__in_show_anger_or_joint_skill_sp_effect = nil
			self.__params.sp_effect_notice_func(self.__params.obj_id)
		end

		if self.__anger_full_effect_id_plus then
			self:__DelEffect(self.__anger_full_effect_id_plus)
			self.__anger_full_effect_id_plus = nil
		end
	end
end

--创建怒气/合击技能释放特效
function Modules.TemplBattleHeroOper:__CreateAngerOrJointReleaseEffect()
	local _, effect_node = self:__CreateEffect({res_name = "UI_nuqitouxiangdianji", loop = false}, self.__icon_head:GetNode() , LogicGUI.IconLayer.InnerZOrder.MAX)
	PosTool.Center(effect_node)
end

--创建怒气/合击技能待释放特效
function Modules.TemplBattleHeroOper:__CreateAngerOrJointToReleaseEffect()
	if not self:__IsAngerEnough() then
		return
	end
	
	self:__DeleteAngerOrJointToReleaseEffect()

	local effect_node
	self.__anger_or_joint_skill_to_release_effect_id, effect_node = self:__CreateEffect({res_name = "UI_nuqitouxiangxuanzhuan"}, self.__icon_head:GetNode() , LogicGUI.IconLayer.InnerZOrder.MAX)
	PosTool.Center(effect_node)
end

function Modules.TemplBattleHeroOper:__DeleteAngerOrJointToReleaseEffect()
	if self.__anger_or_joint_skill_to_release_effect_id then
		self:__DelEffect(self.__anger_or_joint_skill_to_release_effect_id)
		self.__anger_or_joint_skill_to_release_effect_id = nil
	end
end

--检测新手指引
function Modules.TemplBattleHeroOper:__CheckSectionOperateSP()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj then
		return
	end

	local obj_vo = obj:GetVO()

	--新手指引特殊操作检测(英雄释放怒气技能)
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RELEASE_ANGER_SKILL, self.__icon_head:GetNode(), obj_vo.info_id)					

	--新手指引特殊操作检测(英雄释放合击技能)
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RELEASE_JOINT_SKILL, self.__icon_head:GetNode(), obj_vo.info_id)					
end

--检测触发怒气/合击技能
function Modules.TemplBattleHeroOper:__CheckTriggerAngerOrJointSkill(up_x, up_y)
	if CallModuleFunc(ModuleType.STORY, "IsInShield") then
		return
	end

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info and (battle_info.type == Macros.Game.FBType.ARENA) then
		return
	end

	--展示QTE评分特效
	if not CallModuleFunc(ModuleType.BATTLE, "GetAutoFight") then
		if self:__IsInQTEStatus() and not self:__IsInTurningQTECamera() then
			if self:ShowQTEGradeEffect() then
				self:__CreateAngerOrJointReleaseEffect()
				self:__DeleteAngerOrJointToReleaseEffect()
			end
			return
		end
	end

	if self.__in_show_anger_or_joint_skill_sp_effect then
		if self:__IsAngerEnough(true) then
			self:__CreateAngerEnoughEffect(true)
		end				
		self.__in_show_anger_or_joint_skill_sp_effect = nil
		self.__params.sp_effect_notice_func(self.__params.obj_id)
	end

	if up_y - self.__touch_beg_pos.y >= 80 then
		--移除强化版怒气/合击技能指引特效
		self:DeleteAngerOrJointSkillLeadEffect()
		--真.怒气、真.合击技能
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "TriggerAngerOrJointSkill", self.__params.obj_id, true)
	else
		--创建怒气/合击技能待释放特效
		self:__CreateAngerOrJointToReleaseEffect()

		--怒气/合击技能
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "TriggerAngerOrJointSkill", self.__params.obj_id)
	end
end

--检测显示真.怒气/合击技能特殊特效
function Modules.TemplBattleHeroOper:__CheckShowAngerOrJointSkillSpEffect(x, y)
	if CallModuleFunc(ModuleType.STORY, "IsInShield") then
		return
	end

	if not self.__touch_beg_pos then
		return
	end

	if not self.__anger_full_effect_id_plus then
		return
	end

	if self.__in_show_anger_or_joint_skill_sp_effect then
		return
	end

	if math.abs(self.__touch_beg_pos.y - y)	>= 12 then
		self:__CreateAngerEnoughEffect(true, "UI_nuqitouxiangjiaqiang1")
		self.__in_show_anger_or_joint_skill_sp_effect = true		
		self.__params.sp_effect_notice_func(self.__params.obj_id, true)
	end
end

--怒气/合击特效是否可见
function Modules.TemplBattleHeroOper:__CanAngerOrJointEffectVisible(ignore_cd)
	if ignore_cd then
		return not self.__in_sp_hide
	end

	local cd_info = self:__GetAngerOrJointCommonCDInfo()
	return (not cd_info or not cd_info.in_cd) and not self.__in_sp_hide
end

--获取怒气/合击通用CD信息
function Modules.TemplBattleHeroOper:__GetAngerOrJointCommonCDInfo()
	local obj = GlobalScene:GetObj(self.__params.obj_id)
	if not obj then
		return
	end
	local obj_ai = obj:GetAI()
	if not obj_ai then
		return
	end
	return obj_ai:GetAngerOrJointCommonCDInfo()	
end

