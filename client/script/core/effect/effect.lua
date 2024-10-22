
--[[
%% @module: 特效对象
%% @author: swordman sue
%% @created: 2016-07-13
--]]

Effect = Effect or {}

Effect.EffectType =
{
	PARTICLE = 1,	--粒子
	SKELETON = 2,	--骨骼
	SEQFRAME = 3,	--序列帧
}

local handle_params

--------------------------------------------------------------------------------------------------------------------------------
--特效基类
--------------------------------------------------------------------------------------------------------------------------------
Effect.BaseEffect = Effect.BaseEffect or BaseClass()

--[[
@ params：
	必须：
	type				[number]特效类型
    res_name			[string]特效文件名

	可选：   
    loop				[bool]是否循环(默认false)
    scale 				[number]缩放系数(默认1)
    time_scale			[number]速度系数(默认1)
    dir 				[number]朝向(默认MathTool.HORZ_RIGHT)
	pos_x, pos_y		[number]OGL坐标(默认0,0)
	end_callback 		[function]结束回调
	act_index 			[number]动作索引(默认RenderObj.ActionIndex.IDLE，用于骨骼特效)
--]]
function Effect.BaseEffect:__init(params)
	self.__config_id = params.config_id

	self.__real_res_name = self:__TransFileName(params.res_name)
	self.__res_name = tostring(params.type).."_"..self.__real_res_name
end

function Effect.BaseEffect:__delete()
end

function Effect.BaseEffect:GetConfigID()
	return self.__config_id
end

function Effect.BaseEffect:GetResName()
	return self.__res_name
end

function Effect.BaseEffect:SetID(var)
	self.__id = var
end

function Effect.BaseEffect:GetID()
	return self.__id
end

function Effect.BaseEffect:SetRootID(var)
	self.__root_id = var
end

function Effect.BaseEffect:GetRootID()
	return self.__root_id
end

function Effect.BaseEffect:SetIndex(var)
	self.__index = var
end

function Effect.BaseEffect:GetIndex()
	return self.__index
end

function Effect.BaseEffect:SetBindType(var)
	self.__bind_type = var
end

function Effect.BaseEffect:GetBindType()
	return self.__bind_type
end

function Effect.BaseEffect:SetTargetID(var)
	self.__target_id = var
end

function Effect.BaseEffect:GetTargetID()
	return self.__target_id
end

function Effect.BaseEffect:Pause()
end

function Effect.BaseEffect:Resume()
end

function Effect.BaseEffect:SetPosition(x, y)
	local node = self:GetNode()
	node:SetPosition(x, y)	
end

function Effect.BaseEffect:SetVisible(var)
	local node = self:GetNode()
	node:SetVisible(var)	
end

function Effect.BaseEffect:SetFlipX(var)
	local node = self:GetNode()
	node:SetFlipX(var)
end

function Effect.BaseEffect:SetAnim(act_index, loop, end_callback)
end

function Effect.BaseEffect:SetColor(r, g, b)
end

function Effect.BaseEffect:SetOpacity(var)
end

function Effect.BaseEffect:__TransFileName(res_name)	
	return res_name
end

--------------------------------------------------------------------------------------------------------------------------------
--粒子特效
--------------------------------------------------------------------------------------------------------------------------------
Effect.ParticleEffect = Effect.ParticleEffect or BaseClass(Effect.BaseEffect)

function Effect.ParticleEffect:__init(params)	
	handle_params(params)

	self.__particle_node = CPPParticleSprite.CreateAsync(self.__real_res_name)
	if not self.__particle_node then
		LogTool.LogError("[Effect.ParticleEffect:__init] 创建粒子特效失败")
		return
	end	
	CPPNodeUnity.Retain(self.__particle_node)

	self.__particle_node:SetCycle(params.loop)
	self.__particle_node:SetScaleFactor(params.scale)
	self.__particle_node:SetTimeScale(params.time_scale)
	self.__particle_node:SetFlipX(params.dir ~= MathTool.HORZ_RIGHT)
	self.__particle_node:SetPosition(params.pos_x, params.pos_y)
	if not params.loop then
		self.__particle_node:SetPlayEndListener(GlobalCallbackMgr:AddCallBackFunc(function()
			GlobalEffectMgr:DelInnerEffect(self.__id)
			if params.end_callback then
				params.end_callback()
			end
		end))
	end
end

function Effect.ParticleEffect:__delete()
	if self.__particle_node then
		self.__particle_node:RemoveSelf(true)
		CPPNodeUnity.Release(self.__particle_node)
		self.__particle_node = nil
	end
end

function Effect.ParticleEffect:GetNode()
	return self.__particle_node
end

function Effect.ParticleEffect:SetTimeScale(var)
	self.__particle_node:SetTimeScale(var)
end

function Effect.ParticleEffect:Pause()
	self.__particle_node:SetMansualUpdate(true)
end

function Effect.ParticleEffect:Resume()
	self.__particle_node:SetMansualUpdate(false)
end

function Effect.ParticleEffect:SetColor(r, g, b)
	--暂不支持
end

function Effect.ParticleEffect:SetOpacity(var)
	--暂不支持
end

--多语言相关处理
function Effect.ParticleEffect:__TransFileName(res_name)
	return LanguageHelper.TransFileName(res_name, "")
end

--------------------------------------------------------------------------------------------------------------------------------
--骨骼特效
--------------------------------------------------------------------------------------------------------------------------------
Effect.SkeletonEffect = Effect.SkeletonEffect or BaseClass(Effect.BaseEffect)

function Effect.SkeletonEffect:__init(params)
	self.__anim_play_info = params

	handle_params(params)

	params.def_path = Resource.PathTool.GetSkeletonDefaultPath("skeleton_alpha")
	params.path = Resource.PathTool.GetSkeletonEffectPath(self.__real_res_name)
	params.act_index = params.act_index or RenderObj.ActionIndex.IDLE
	if not params.loop then
		local external_end_callback = params.end_callback
		params.end_callback = function()
			GlobalEffectMgr:DelInnerEffect(self.__id)
			if external_end_callback then
				external_end_callback()
			end
		end
	end
	self.__skeleton_render = SkeletonRender.New(params)
	local skeleton_node = self.__skeleton_render:GetSkeletonNode()
	if not skeleton_node then
		LogTool.LogError("[Effect.SkeletonEffect] 创建骨骼特效失败")
		return
	end

	CPPNodeUnity.Retain(skeleton_node)
	skeleton_node:RemoveSelf(false)
	skeleton_node:SetPosition(params.pos_x, params.pos_y)
end

function Effect.SkeletonEffect:__delete()	
	local skeleton_node = self.__skeleton_render:GetSkeletonNode()
	if skeleton_node then
		skeleton_node:RemoveSelf(true)
	end
	if self.__skeleton_render then
		self.__skeleton_render:DeleteMe()
		self.__skeleton_render = nil
	end
	CPPNodeUnity.Release(skeleton_node)
end

function Effect.SkeletonEffect:GetNode()
	return self.__skeleton_render:GetSkeletonNode()
end

function Effect.SkeletonEffect:SetTimeScale(scale)
	self.__skeleton_render:SetTimeScale(scale)
end

function Effect.SkeletonEffect:Pause()
	self.__skeleton_render:PauseAnimation()
end

function Effect.SkeletonEffect:Resume()
	self.__skeleton_render:ResumeAnimation()
end

function Effect.SkeletonEffect:SetAnim(act_index, loop, end_callback)
	if self.__skeleton_render then
		self.__anim_play_info.act_index = act_index
		self.__anim_play_info.loop = loop == nil and true or false
		if not self.__anim_play_info.loop then
			self.__anim_play_info.end_callback = function()
				GlobalEffectMgr:DelInnerEffect(self.__id)
				if end_callback then
					end_callback()
				end
			end
		else
			self.__anim_play_info.end_callback = nil
		end
		self.__skeleton_render:PlayAnimation(self.__anim_play_info)
	end
end

function Effect.SkeletonEffect:SetColor(r, g, b)
	if self.__skeleton_render then
		self.__skeleton_render:SetColor(r, g, b)
	end
end

function Effect.SkeletonEffect:SetOpacity(var)
	if self.__skeleton_render then
		self.__skeleton_render:SetOpacity(var)
	end
end

--多语言相关处理
function Effect.SkeletonEffect:__TransFileName(res_name)
	return LanguageHelper.TransFileName(res_name, ".ssdz")
end

--------------------------------------------------------------------------------------------------------------------------------
--序列帧特效
--------------------------------------------------------------------------------------------------------------------------------
Effect.SeqFrameEffect = Effect.SeqFrameEffect or BaseClass(Effect.BaseEffect)

function Effect.SeqFrameEffect:__init(params)
end

function Effect.SeqFrameEffect:__delete()
end

function Effect.SeqFrameEffect:GetNode()
end

function Effect.SeqFrameEffect:SetTimeScale()
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
handle_params = function(params)
	params.loop = params.loop ~= nil and params.loop or false
	params.scale = params.scale or 1.0
	params.time_scale = params.time_scale or 1.0
	params.dir = params.dir or MathTool.HORZ_RIGHT
	params.pos_x = params.pos_x or 0
	params.pos_y = params.pos_y or 0
end
