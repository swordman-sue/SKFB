 
--[[
%% @module: 战斗界面
%% @author: swordman sue
%% @created: 2016-08-15
--]]

Modules = Modules or {}

Modules.BattleView = Modules.BattleView or BaseClass(BaseView)

--初始化精灵操作区域
function Modules.BattleView:__InitSpriteOperRegion()
	local panel_sprite_oper = self:GetWidget("Panel_SpritesOper")

	local Panel_caozuo = self:GetWidget("Panel_caozuo")
	if Macros.Global.DesignSize.width < 1000 then
		panel_sprite_oper:SetPosition( -15 , 110 )	
	end

	self.__panel_sprite_skill_cancel_top = self:GetWidget("Panel_SpriteSkillCancelTop")
	self.__panel_sprite_skill_cancel_top:SetVisible(false)

	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local config_battle_sprite = CPPGameLib.GetConfig("BattleSpriteToggle", battle_info.type, false)

	local spirit_system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.SPIRIT)
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")

	if not config_battle_sprite or config_battle_sprite.trigger_type ~= Macros.Global.TRUE then
		panel_sprite_oper:SetVisible(false)
		return

	--当前没有出战
	elseif spirit_info and spirit_info.cur_use_spirit == 0 then
		panel_sprite_oper:SetVisible(true)
		local path = Resource.PathTool.GetSpiritHeadPath( 50001 , "sprite_icon_" )
		local sprite = Util:Sprite( path )
		self:GetWidget("Panel_sprite_icon"):AddChild( sprite )
		PosTool.Center( sprite )  
		sprite:SetGray(true)

		self:GetWidget("Image_SpriteComSkillBG"):SetVisible(false)
		self:GetWidget("Image_SpriteSpSkillBG"):SetVisible(false)
	
		local spirit_system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.SPIRIT)
		local img_5 = self:GetWidget("Image_5")
		local text = CPPGameLib.GetString("common_open_on_sprite")

		if spirit_info and #spirit_info.spirit_list ~= 0 then	
			text = CPPGameLib.GetString("common_played_on_sprite")
		end

		local common_open_on_role_level = Util:Label( text , Macros.TypefaceSize.slightly , Macros.Color.white ,140,0)
		common_open_on_role_level:SetAnchorPoint( 0 , 0.5 )
		common_open_on_role_level:SetPosition( self:GetWidget("Image_SpriteComSkillBG"):GetPositionX() - 30 , self:GetWidget("Image_SpriteComSkillBG"):GetPositionY() )
		img_5:AddChild( common_open_on_role_level )

		return 
	end

	panel_sprite_oper:SetVisible(true)

	Util:SetLayoutBGColor(self.__panel_sprite_skill_cancel_top, Macros.Color.lilyBlack, 204)	

	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	local sprite_vo = sprite:GetVO()

	--创建技能图标
	local function create_skill_icon(parent_node, skill_id)
		local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
		if not config_skill then
			return
		end

		local config_sprite_train_info = ConfigAdapter.Spirit.GetTrainInfo(sprite_vo.info_id, sprite_vo.train_stage)
		local config_sprite_talent_info = ConfigAdapter.Spirit.GetTalentInfo(sprite_vo.info_id, config_sprite_train_info and config_sprite_train_info.skill_level or 1)

		local clipping_node = Util:CircleClippingNode(0, 0, 36)
		parent_node:AddChild(clipping_node)
		PosTool.Center(clipping_node)

		--技能图标
		local sp_skill_icon = Util:Sprite(Resource.PathTool.GetSpiritSkillIconPath(skill_id))
		sp_skill_icon:SetAnchorPoint(0.5, 0.5)
		clipping_node:AddChild(sp_skill_icon)

		--触碰回调
		parent_node:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(parent_node, 
			function(x, y)				
				self:__OnDownSpriteSkill(skill_id, x, y)
			end, 
			function(x, y)
				self:__OnMoveSpriteSkill(skill_id, x, y)
			end,
			function(x, y)
				self:__OnUpSpriteSkill(skill_id, x, y)
			end,
			function(x, y)
				self:__OnCancelSpriteSkill(skill_id, x, y)
			end,nil,0)

		local skill_info = {}
		skill_info.parent_node = parent_node
		skill_info.clipping_node = clipping_node
		skill_info.sp_skill_icon = sp_skill_icon
		skill_info.used_time = 0
		skill_info.max_use_times = config_skill.max_use_times or 1
		skill_info.pick_effect_id = config_skill.sprite_pick_effect_id

		--技能CD
		skill_info.cd_time = config_skill.cd_time or 0
		--处理精灵天赋技能减少CD
		if config_sprite_talent_info then
			if skill_id == sprite_vo.common_skill_id and config_sprite_talent_info.decrease_public_cd then
				skill_info.cd_time = math.max(0, skill_info.cd_time - config_sprite_talent_info.decrease_public_cd)
			elseif skill_id == sprite_vo.special_skill_id and config_sprite_talent_info.decrease_private_cd then
				skill_info.cd_time = math.max(0, skill_info.cd_time - config_sprite_talent_info.decrease_private_cd)
			end
		end

		self.__sprite_skill_map[skill_id] = skill_info

		--可释放特效
		self:__HandleSpriteSkillEffect(skill_id, true)
	end

	create_skill_icon(self:GetWidget("Image_SpriteComSkill"), sprite_vo.common_skill_id)
	create_skill_icon(self:GetWidget("Image_SpriteSpSkill"), sprite_vo.special_skill_id)

	local path = Resource.PathTool.GetSpiritHeadPath( sprite_vo.info_id , "sprite_icon_" )
	local sprite = Util:Sprite( path )
	self:GetWidget("Panel_sprite_icon"):AddChild( sprite )
	PosTool.Center( sprite )  
end

--战役开启处理
function Modules.BattleView:__HandleBattleStartForSprite()
	--战役开始，给精灵技能默认CD（前提是该技能未处于CD中，且释放次数未达到上限）
	local birth_type = CallModuleFunc(ModuleType.BATTLE, "GetBirthType", Scene.ObjType.HERO)
	if birth_type == Macros.Game.FBBirthType.RUN_INTO then
		for skill_id, skill_info in pairs(self.__sprite_skill_map) do
			if not skill_info.in_cd and skill_info.used_time < skill_info.max_use_times then
				if not skill_info.prog_cd then
					local sp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "bg_cd"))
					skill_info.prog_cd = ProgressTimer.CreateWithImageLabel(sp)
					skill_info.prog_cd:SetType(ProgressTimer.TypeRadial)
					skill_info.prog_cd:SetMidpoint(COCOPoint(0.5, 0.5))
					skill_info.prog_cd:SetAnchorPoint(0.5, 0.5)
					skill_info.prog_cd:SetReverseDirection(true)
					skill_info.clipping_node:AddChild(skill_info.prog_cd)
				end
				skill_info.in_cd = true
				skill_info.prog_cd:SetPercentage(100)
				skill_info.parent_node:SetTouchEnabled(false)
				CPPActionManager.StopAllActions(skill_info.prog_cd)
				CPPActionManager.ProgressTo(skill_info.prog_cd, ConfigAdapter.Common.GetSpiritSkillCDForBirth(), 0, GlobalCallbackMgr:AddCallBackFunc(function()
					skill_info.in_cd = false
					skill_info.parent_node:SetTouchEnabled(true)
				end))
			end
		end
	end
end

--按下精灵技能图标
function Modules.BattleView:__OnDownSpriteSkill(skill_id, x, y)	
	if not self:__CheckCanReleaseSpriteSkill(skill_id, true) then
		return
	end
	local skill_info = self.__sprite_skill_map[skill_id]
	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	local render_group = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_SHADOW)
	local pick_effect_pos = NodeUnity.ConvertToNodePositon(render_group:GetNode(), COCOPoint(x, 0))
	pick_effect_pos.y = sprite and (sprite:GetPixelY() + Scene.SpiritOffset.y) or 256
	self.__sprite_pick_effect_id = GlobalEffectMgr:CreateScenePosEffect({id = skill_info.pick_effect_id, pos_x = pick_effect_pos.x, pos_y = pick_effect_pos.y, loop = true}, Scene.RenderQueue.GRQ_SHADOW)				

	self.__panel_sprite_skill_cancel_top:SetVisible(true)
end

--移动精灵技能图标
function Modules.BattleView:__OnMoveSpriteSkill(skill_id, x, y)
	local effect = GlobalEffectMgr:GetEffect(self.__sprite_pick_effect_id)
	if effect then
		local render_group = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_SHADOW)
		local pick_effect_pos = NodeUnity.ConvertToNodePositon(render_group:GetNode(), COCOPoint(x, 0))
		effect:SetPosition(pick_effect_pos.x, effect:GetPositionY())
		--拖动的时候减速
		if not self.__is_on_delta_speedtime and not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeonIncludeRobot") then
			self.__is_on_delta_speedtime = true
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", ConfigAdapter.Common.GetSpiritSkillMoveDeltaSpeedTimeFactor() or 1)
		end
	end	
end

--弹起精灵技能图标
function Modules.BattleView:__OnUpSpriteSkill(skill_id, x, y, is_release, is_skip)
	self:__DelSpriteSkillPickEffect()
	self.__panel_sprite_skill_cancel_top:SetVisible(false)
	--[[
		防止玩家直接点击精灵技能导致的失误释放
		-当点击精灵技能图标时，会出现范围选择的圈
		-但如果玩家在精灵技能图标的范围内松开点击，则等于取消释放]]--
	--拖动的时候减速
	--跳过的时候不进行还原速度
	if self.__is_on_delta_speedtime and not is_skip then		
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", self:CallModuleFunc("GetACCFactor") or 1)
	end
	self.__is_on_delta_speedtime = false
	if is_release then
		self:__ReleaseSpriteSkill(skill_id, x, y)
	end
end

--弹起精灵技能图标
function Modules.BattleView:__OnCancelSpriteSkill(skill_id, x, y)
	self:__OnUpSpriteSkill(skill_id, x, y,true)
end

--检测精灵技能释放条件
function Modules.BattleView:__CheckCanReleaseSpriteSkill(skill_id, check_down, x, y)

	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllObjsEnterRegion") and not check_down then
		--有参战对象未进入视野，过滤
		return
	end

	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") and not check_down then
		--有参战对象释放怒气/合击技能中
		return
	end

	if not check_down then
		--在撤销区域
		local pos = COCOPoint(x, y)
		if NodeUnity.HitTest(self.__panel_sprite_skill_cancel_top, pos) then
			return
		end
	end

	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if not sprite:CanDoAttack(skill_id, nil, true) and not check_down then
		--不满足攻击条件
		return
	end

	local skill_info = self.__sprite_skill_map[skill_id]
	if not skill_info then
		return
	end	

	if skill_info.used_time >= skill_info.max_use_times then
		--已达次数上限
		return
	end

	if skill_info.in_cd then
		--CD中
		return
	end

	return true
end

--释放精灵技能
function Modules.BattleView:__ReleaseSpriteSkill(skill_id, x, y)
	if not self:__CheckCanReleaseSpriteSkill(skill_id, nil, x, y) then
		return
	end

	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if sprite then
		self:__DeleteSpriteSkillLeadEffect()
		
		local render_group = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_BUILD_AND_PLAYER)
		local pick_effect_pos = NodeUnity.ConvertToNodePositon(render_group:GetNode(), COCOPoint(x, 0))
		local world_pos = Vector2D.New(GlobalScene:PixelToWorld(pick_effect_pos.x, pick_effect_pos.y))
		world_pos.y = self:CallModuleFunc("GetBirthPosY")
		local vo = sprite:GetVO()
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "TriggerSpriteSkill", vo.obj_id, skill_id, world_pos)
	end
end

--移除精灵选取特效
function Modules.BattleView:__DelSpriteSkillPickEffect()
	if self.__sprite_pick_effect_id then
		GlobalEffectMgr:DelEffect(self.__sprite_pick_effect_id)
		self.__sprite_pick_effect_id = nil
	end
end

--处理精灵技能CD
function Modules.BattleView:__HandleSpriteSkillReleased(skill_id)
	local skill_info = self.__sprite_skill_map[skill_id]
	if not skill_info then
		return
	end
	skill_info.used_time = skill_info.used_time + 1

	--达到次数上限，灰掉
	local over_times = skill_info.used_time >= skill_info.max_use_times
	skill_info.sp_skill_icon:SetGray(over_times)

	--未达次数上限，进入CD
	if not over_times and skill_info.cd_time > 0 then
		if not skill_info.prog_cd then
			local sp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "bg_cd"))
			skill_info.prog_cd = ProgressTimer.CreateWithImageLabel(sp)
			skill_info.prog_cd:SetType(ProgressTimer.TypeRadial)
			skill_info.prog_cd:SetMidpoint(COCOPoint(0.5, 0.5))
			skill_info.prog_cd:SetAnchorPoint(0.5, 0.5)
			skill_info.prog_cd:SetReverseDirection(true)
			skill_info.clipping_node:AddChild(skill_info.prog_cd)
		end

		skill_info.in_cd = true
		skill_info.prog_cd:SetPercentage(100)
		CPPActionManager.StopAllActions(skill_info.prog_cd)
		CPPActionManager.ProgressTo(skill_info.prog_cd, skill_info.cd_time, 0, GlobalCallbackMgr:AddCallBackFunc(function()
			skill_info.in_cd = false
			self:__HandleSpriteSkillEffect(skill_id, true)
		end))
	end

	self:__HandleSpriteSkillEffect(skill_id, false)
end

--处理暂停相关
function Modules.BattleView:__HandlePasueForSpriteSkill()
	for _, skill_info in pairs(self.__sprite_skill_map) do
		if skill_info.prog_cd then
			CPPActionManager.PauseAction(skill_info.prog_cd)
		end
	end
end

--处理恢复相关
function Modules.BattleView:__HandleResumeForSpriteSkill()
	for _, skill_info in pairs(self.__sprite_skill_map) do
		if skill_info.prog_cd then
			CPPActionManager.ResumeAction(skill_info.prog_cd)
		end
	end
end

--处理技能可释放特效
function Modules.BattleView:__HandleSpriteSkillEffect(skill_id, var)
	local skill_info = self.__sprite_skill_map[skill_id]
	if not skill_info then
		return
	end

	if var then
		if not skill_info.can_released_effect_id then
			local effect_root
			skill_info.can_released_effect_id, effect_root = self:__CreateEffect({res_name = "UI_zhandoujineng"}, skill_info.parent_node)
			PosTool.Center(effect_root)
		end
	end
	GlobalEffectMgr:SetEffectVisible(skill_info.can_released_effect_id, var)
end

--添加强化版怒气/合击技能指引特效
function Modules.BattleView:__CreatSpriteSkillLeadEffect()
	self:__DeleteSpriteSkillLeadEffect()

	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if not sprite then
		return
	end
	local sprite_vo = sprite:GetVO()

	local skill_info = self.__sprite_skill_map[sprite_vo.special_skill_id]
	if not skill_info then
		return
	end

	local effect_root
	skill_info.lead_effect_id, effect_root = self:__CreateEffect({res_name = "UI_zhiyintuodong2"}, skill_info.parent_node)
	PosTool.Center(effect_root)
end

--移除强化版怒气/合击技能指引特效
function Modules.BattleView:__DeleteSpriteSkillLeadEffect()
	local sprite = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetTeammateSprite")
	if not sprite then
		return
	end
	local sprite_vo = sprite:GetVO()

	local skill_info = self.__sprite_skill_map[sprite_vo.special_skill_id]
	if skill_info and skill_info.lead_effect_id then
		self:__DelEffect(skill_info.lead_effect_id)
		skill_info.lead_effect_id = nil
	end
end

