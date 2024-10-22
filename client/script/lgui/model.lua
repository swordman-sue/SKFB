
--[[
%% @module: UI模型对象
%% @author: swordman sue
%% @created: 2016-08-10
注：需手动释放(DeleteMe)
--]]

LogicGUI = LogicGUI or {}

LogicGUI.Model = LogicGUI.Model or BaseClass()

LogicGUI.Model.global_cache_id = 0
LogicGUI.Model.global_cache_map = {}

LogicGUI.ModelType = 
{
	HERO = 1,
	MONSTER = 2,
	SPRITE = 3,
}

--[[
@ params：
	必须：
    parent				[table]父节点
    model_type			[number]类型(默认LogicGUI.ModelType.HERO)

    可选：
	info_id				[number]配置ID
	path 				[string]模型路径
    act_index			[number]动作索引(默认RenderObj.ActionIndex.IDLE)
    dir 				[number]朝向(默认MathTool.HORZ_RIGHT)
    loop 				[bool]是否循环(默认true)
    scale 				[number]缩放系数(默认1)
    fixed_frame_time	[number]指定帧时间
    mount_effect_info	[table]挂载的特效信息(slot_name：挂载节点名称，effect_params：特效创建参数)
    direct_get_model    [boolean]是否直接取模型

    zorder 				[number]层次
    tag 				[number]标签
--]]
function LogicGUI.Model:__init(params)
	if not params.parent then
		LogTool.LogError("[LogicGUI.Model:__init] 参数params无效")
		return
	end

	params.act_index = params.act_index or RenderObj.ActionIndex.IDLE
	params.dir = params.dir or MathTool.HORZ_RIGHT
	params.loop = params.loop == nil and true or params.loop

	self.__model_type = params.model_type or LogicGUI.ModelType.HERO
	self.__dir = params.dir
	self.__scale = params.scale or 1
    self.__fixed_frame_time = params.fixed_frame_time
    self.__mount_effect_info = params.mount_effect_info

	--创建模型
	self.__model = RenderObj.New(Scene.RenderQueue.GRQ_UI)
	self.__model:AddToParent(params.parent, params.zorder, params.tag)

    --身体动画信息
    self.__body_anim_play_info = {}
    self.__body_anim_play_info.anim_type = RenderObj.AnimType.SKELETON
    self.__body_anim_play_info.inner_layer = RenderObj.InnerLayer.BODY

    if params.info_id then
	    self:SetInfoID(params.info_id, params.act_index, params.loop,params.direct_get_model)

	elseif params.path then
	    self:SetPath(params.path, params.act_index, params.loop)
	end

    if StartConfig.IsDebug then
	    LogicGUI.Model.global_cache_id = LogicGUI.Model.global_cache_id + 1
	    self.__global_cache_id = LogicGUI.Model.global_cache_id
	    LogicGUI.Model.global_cache_map[self.__global_cache_id] = debug.traceback()    
	end
end

function LogicGUI.Model:__delete()
	if StartConfig.IsDebug then
	    LogicGUI.Model.global_cache_map[self.__global_cache_id] = nil
	end

	if self.__model then
		self.__model:DeleteMe()
		self.__model = nil
	end
end

function LogicGUI.Model.PrintCache()
	print(8, "---------------LogicGUI.Model:PrintCache---------------")
	for _, info in pairs(LogicGUI.Model.global_cache_map) do
		LogTool.LogInfo(info)
	end
end

function LogicGUI.Model:GetModel()
	return self.__model
end

function LogicGUI.Model:GetNode()
	return self.__model:GetNode()
end

--播放动作
function LogicGUI.Model:PlayAnimation(act_index, loop)
	act_index = act_index or RenderObj.ActionIndex.IDLE

	if not self.__body_anim_play_info.path then
		LogTool.LogError("[LogicGUI.Model:PlayAnimation] 动作资源路径未初始化")
		return
	end    

    self:__SetupAnimPlayInfo(self.__body_anim_play_info, act_index, loop)
    self.__model:PlayAnimation(self.__body_anim_play_info)
    return true
end

--播放技能动作&特效
function LogicGUI.Model:PlaySkillAnim(skill_id)
	local act_index = Skill.SkillLogic:GetAttackActIndex(skill_id)
	self.__end_callback_interval = function()
		self:PlayAnimation(nil, true)
	end
	if not self:PlayAnimation(act_index, false) then
		self.__end_callback_interval = nil
	end

	local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
	if config_skill then
		--攻击特效(挂载到攻击者身上)
		--上层
		if config_skill.attack_effect_id then
			GlobalEffectMgr:CreateUIEffect({id = config_skill.attack_effect_id, dir = self.__dir, loop = false}, self.__model, RenderObj.InnerLayer.BODY_EFFECT_UP)
		end
		--下层
		if config_skill.down_attack_effect_id then
			GlobalEffectMgr:CreateUIEffect({id = config_skill.down_attack_effect_id, dir = self.__dir, loop = false}, self.__model, RenderObj.InnerLayer.BODY_EFFECT_DOWN)
		end
	end
end

--设置配置ID
function LogicGUI.Model:SetInfoID(id, act_index, loop, is_direct)
    if CPPSdkTools.IsInExamine() then
		local safe_monster_list_for_examine = ConfigAdapter.Common.GetSafeMonsterListForExamine() or Macros.Game.SafeMonsterListForExamine
    	
		--审核状态下，指向不侵权的模型
		if self.__model_type == LogicGUI.ModelType.MONSTER then
			id = safe_monster_list_for_examine[MathTool.GetRandom(#safe_monster_list_for_examine)]
		end
    end
	
	self.infoID = id
	if not id then
		return
	end	

	self:__ParseInfo(id, is_direct)

	loop = loop == nil and true or loop
	return self:PlayAnimation(act_index, loop)
end

--设置资源路径
function LogicGUI.Model:SetPath(path, act_index, loop)
	if not path then
		return
	end

	self:__ParseInfo(path)

	loop = loop == nil and true or loop
	return self:PlayAnimation(act_index, loop)
end

--设置模型类型
function LogicGUI.Model:SetModelType(var)
	self.__model_type = var
end

--设置朝向
function LogicGUI.Model:SetDir(var)
	if var == self.__dir then
		return
	end
	self.__dir = var	

	if self.__model then
		self.__model:SetDir(var, self.__body_anim_play_info.inner_layer)
	end
end

--获取朝向
function LogicGUI.Model:GetDir()
	return self.__dir
end

function LogicGUI.Model:SetPosition(x, y)
	if self.__model then
		self.__model:SetPosition(x, y)
	end
end

function LogicGUI.Model:SetScaleFactor(var)
	if self.__model then
		self.__model:SetScaleFactor(var)		
	end
end

function LogicGUI.Model:SetZOrder(var)
	if self.__model then
		self.__model:SetZOrder(var)
	end
end

function LogicGUI.Model:SetGray(var)
	if self.__model then
		self.__model:SetGray(var, self.__body_anim_play_info.inner_layer)
	end
end

function LogicGUI.Model:SetTouchRect(x, y, width, height)
	if self.__model then
		self.__model:SetTouchRect(x, y, width, height)
	end
end

function LogicGUI.Model:SetTouchFun(touch_down, touch_move, touch_up)
	if self.__model then
		self.__model:SetTouchFun(touch_down, touch_move, function(x, y, in_rect)
			if in_rect == Macros.Global.TRUE and touch_up then
				touch_up()
			end
		end)
	end
end

function LogicGUI.Model:SetTouchSwallow(var)
	if self.__model then
		self.__model:SetTouchSwallow(var)
	end
end

function LogicGUI.Model:SetColor(r, g, b)
	if self.__model then
		self.__model:SetColor(r, g, b, self.__body_anim_play_info.inner_layer)
	end
end

function LogicGUI.Model:SetOpacity(var)
	if self.__model then
		self.__model:SetOpacity(var, self.__body_anim_play_info.inner_layer)
	end
end

--暂停动画
function LogicGUI.Model:PauseAnimation()
	if self.__model then
		self.__model:PauseAnimation(self.__body_anim_play_info.inner_layer)
	end
end

--恢复动画
function LogicGUI.Model:ResumeAnimation()
	if self.__model then
		self.__model:ResumeAnimation(self.__body_anim_play_info.inner_layer)
	end
end

--播放到指定帧
function LogicGUI.Model:SetToPlayTime(time, act_index)
	self.__fixed_frame_time = time
	self:PlayAnimation(act_index)
end

--设置播放速度系数
function LogicGUI.Model:SetTimeScale(var)
	if self.__model then
		self.__model:SetAnimTimeScale(var, self.__body_anim_play_info.inner_layer)
	end
end

--设置旋转角度
function LogicGUI.Model:SetRotation(rotation)
	if self.__model then
		self.__model:SetRotation(rotation)
	end
end

--设置播放完成回调(回调触发后，自动置空)
function LogicGUI.Model:SetEndCallback(callback)
	self.__end_callback = callback
end

function LogicGUI.Model:GetHeight()
	if self.__model then
		return self.__model:GetModelHeight(self.__body_anim_play_info.inner_layer)
	end
end

function LogicGUI.Model:GetWidth()
	if self.__model then
		return self.__model:GetModelWidth(self.__body_anim_play_info.inner_layer)
	end
end

function LogicGUI.Model:GetInfoId()
	return self.infoID
end

--可升级提示动作
function LogicGUI.Model:AddUpgradeTips(tip_string,scale_x,scale_y)
	scale_x = scale_x or 1
	scale_y = scale_y or 1
	local scale = self.__model:GetScaleFactor()
	if not self.__upgrade_tips then
		local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "hero_updatetips")
		self.__upgrade_tips = SpriteEx.CreateWithImageFile(imgdata)
		self.__model:AddChild(self.__upgrade_tips,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
		self.__upgrade_string = Util:Label("",Macros.TypefaceSize.normal,nil,0,0,Macros.Color.common_stroke)
		self.__upgrade_tips:AddChild(self.__upgrade_string)
		PosTool.Center(self.__upgrade_string,0,5)				
		self.__funId = GlobalCallbackMgr:AddCallBackFunc(function ()
			ActionManager.GetInstance():RunJumpByAction(self.__upgrade_tips,0.8,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
				self:RunAddAction()
			end))	
		end)
	else
		CPPActionManager.StopAllActions(self.__upgrade_tips)		
	end
	self.__upgrade_tips:SetScaleFactor(0.8/scale)
	self.__upgrade_tips:SetPosition(80/scale*scale_x,130/scale*scale_y)
	self.__upgrade_string:SetString(tip_string)
	self.__upgrade_tips:SetVisible(true)
	self:RunAddAction()
end

--可升级提示动作
function LogicGUI.Model:RunAddAction()
	ActionManager.GetInstance():RunJumpByAction(self.__upgrade_tips,0.8,0,20,0,0,self.__funId)
end

--取消可升级提示动作
function LogicGUI.Model:HideUpgradeTips()
	if self.__upgrade_tips then
		CPPActionManager.StopAllActions(self.__upgrade_tips)
		self.__upgrade_tips:SetVisible(false)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function LogicGUI.Model:__ParseInfo(data, is_direct)	
	if type(data) == "number" and is_direct then
		local model_type
		if self.__model_type == 1 then
			model_type = "hero"
		elseif self.__model_type == 2 then
			model_type = "monster"
		elseif self.__model_type == 3 then
			model_type = "sprite"
		elseif self.__model_type == 4 then
			model_type = "npc"
		end
		self.__body_anim_play_info.path = Resource.PathTool.GetModelPath(model_type, data)  

	elseif type(data) == "number" then
	    self.__body_anim_play_info.path = Resource.PathTool.GetModelPathEx(data, self.__model_type == LogicGUI.ModelType.MONSTER)    	
	else
		self.__body_anim_play_info.path = data
	end
end

function LogicGUI.Model:__SetupAnimPlayInfo(anim_play_info, act_index, loop)
	anim_play_info.act_index = act_index
	anim_play_info.loop = loop
	anim_play_info.dir = self.__dir
	anim_play_info.scale = self.__scale
	anim_play_info.fixed_frame_time = self.__fixed_frame_time
	anim_play_info.mount_effect_info = self.__mount_effect_info
	anim_play_info.end_callback = function()
		--内部播放完成回调
		if self.__end_callback_interval then
			self.__end_callback_interval()
			self.__end_callback_interval = nil
		end

		--播放完成回调
		if self.__end_callback then
			self.__end_callback()
			self.__end_callback = nil
		end
	end
end

