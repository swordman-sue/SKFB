
--[[
%% @module: 骨骼动画
%% @author: swordman sue
%% @created: 2016-07-12
--]]

SkeletonRender = SkeletonRender or BaseClass(Layer)

SkeletonRender.ANIMATION_COMPLETE = 2
SkeletonRender.ANIMATION_EVENT = 3

--播放指定帧
local PlayFixedFrame

--[[
播放动画
@ anim_play_info：
	必须：
	path				[string]文件路径
    act_index			[number]动作索引

	可选：   
    is_sync				[bool]是否同步加载(默认false)
    loop				[bool]是否循环(默认true)
    scale 				[number]缩放系数(默认1)
    time_scale			[number]速度系数(默认1)
    dir 				[number]朝向(默认MathTool.HORZ_RIGHT)
    end_callback 		[function]结束回调
    event_callback 		[function]事件回调
    def_path			[string]默认文件路径
    fixed_frame_time 	[number]指定帧时间
    mount_effect_info	[table]挂载的特效信息(slot_name：挂载节点名称，effect_params：特效创建参数, zorder：层次)
    is_fixed_last_frame_time	[bool]是否指定为最后一帧(默认false)

	动作1->暂停->动作2
    next_act_index		[number]下一个动作索引
--]]
function SkeletonRender:__init(anim_play_info)
	if not anim_play_info.path or anim_play_info.path == "" then
		anim_play_info.path = Resource.PathTool.GetSkeletonDefaultPath("skeleton_alpha")
	end
 
	self.__anim_type = RenderObj.AnimType.SKELETON
	self.__is_sync = anim_play_info.is_sync ~= nil and anim_play_info.is_sync or false

	--同步/异步加载处理
	local def_path = anim_play_info.def_path or Resource.PathTool.GetSkeletonDefaultPath("skeleton_alpha")
	local path = self.__is_sync and anim_play_info.path or def_path
	self.__skeleton_node = SkeletonAnimationEx.CreateWithFile(path)
	if not self.__skeleton_node then 
		LogTool.LogError("[SkeletonRender:PlayAnimation] 创建骨骼失败. path:%s", path)
		return
	end
	self.__skeleton_node:SetPosition(0.0, 0.0)
	self.__skeleton_node:SetAnimationListener(GlobalCallbackMgr:AddCallBackFunc(function(action_name, event_type, evnet_name, int_value, float_value, string_value)
		self:__RunAnimationFunc(action_name, event_type, evnet_name, int_value, float_value, string_value)
	end))	
	
	--重设资源
	self:__ResetSkeleton(anim_play_info, true)

	self:AddChild(self.__skeleton_node)	

	--缩放处理
	self:SetScaleFactor(anim_play_info.scale or 1)
end

function SkeletonRender:__delete()
	if self.__mount_effect_id then
		GlobalEffectMgr:DelEffect(self.__mount_effect_id)
		self.__mount_effect_id = nil
	end

	self.__skeleton_node:ClearAnimationListener()
	self.__skeleton_node = nil
end

function SkeletonRender:PlayAnimation(anim_play_info)
	if not anim_play_info or not anim_play_info.path then
		return
	end

	if self.__in_async_loading then
		self.__anim_play_info_in_async = anim_play_info
		return
	end

	self.__is_sync = anim_play_info.is_sync ~= nil and anim_play_info.is_sync or false

	if self.__path ~= anim_play_info.path then
		--重设资源
		self:__ResetSkeleton(anim_play_info)
	else
		--资源没变化，播放相应动作
		self:__PlayAnimationReal(anim_play_info)
	end

	--缩放处理
	self:SetScaleFactor(anim_play_info.scale or 1)
end

function SkeletonRender:GetAnimType()
	return self.__anim_type
end

function SkeletonRender:GetSkeletonNode()
	return self.__skeleton_node
end

function SkeletonRender:GetActIndex()
	return self.__act_index
end

function SkeletonRender:GetNextActIndex()
	return self.__next_act_index
end

function SkeletonRender:SetAnimEndCallback(callback_func)
	if self.__end_callback == callback_func then
		return
	end

	self.__end_callback = callback_func
end

function SkeletonRender:SetAnimEventCallback(callback_func)
	if self.__event_callback == callback_func then
		return
	end

	self.__event_callback = callback_func
end

function SkeletonRender:ClearCallbackStatus()
	self.__end_callback = nil
	self.__event_callback = nil
end

---设置面向
function SkeletonRender:SetDir(var)
	if self.__dir == var then
		return
	end
	self.__dir = var
	self:SetFlipX(var ~= MathTool.HORZ_RIGHT)
end

--设置水平翻转
function SkeletonRender:SetFlipX(var)
	self.__skeleton_node:SetFlipX(var)
end

--设置垂直翻转
function SkeletonRender:SetFlipY(var)
	self.__skeleton_node:SetFlipY(var)
end

function SkeletonRender:SetScaleFactor(var)
	self.__skeleton_node:SetScaleFactor(var)
end

function SkeletonRender:GetScaleFactor()
	return self.__skeleton_node:GetScaleFactor()
end

function SkeletonRender:IsAnimationExist(state_index)
	return self.__skeleton_node:IsAnimationExist(RenderObj.ActionName[state_index])
end

function SkeletonRender:PauseAnimation()
	if self.__is_pause then
		return
	end
	self.__is_pause = true

	self.__skeleton_node:StopAnimation()
end

function SkeletonRender:ResumeAnimation()
	if not self.__is_pause then
		return
	end
	self.__is_pause = false

	self.__skeleton_node:ResumeAnimation()
end

function SkeletonRender:GetWidth()
	return self.__skeleton_node:GetWidth()
end

function SkeletonRender:GetHeight()
	return self.__skeleton_node:GetHeight()
end

function SkeletonRender:GetOriginX()
	return self.__skeleton_node:GetOriginX()
end

function SkeletonRender:GetOriginY()
	return self.__skeleton_node:GetOriginY()
end

function SkeletonRender:SetColor(r, g, b)
	return self.__skeleton_node:SetColor(r, g, b)
end

function SkeletonRender:SetGray(is_gray)
	return self.__skeleton_node:SetGray(is_gray)
end

function SkeletonRender:SetOpacity(var)
	self.__skeleton_node:SetOpacity(var)
end

function SkeletonRender:SetHighLight(is_high_light)
	return self.__skeleton_node:SetHighLight(is_high_light)
end

function SkeletonRender:SetTimeScale(time_scale)
	if self.__time_scale == time_scale then
		return
	end
	self.__time_scale = time_scale
	self.__skeleton_node:SetTimeScale(self.__time_scale)
end

function SkeletonRender:GetTimeScale()
	return self.__skeleton_node:GetTimeScale()
end

function SkeletonRender:SetHighLight(var)
	self.__skeleton_node:SetHighLight(var)
end

function SkeletonRender:SetRotation(rotation)
	self.__skeleton_node:SetRotation(rotation)
end

function SkeletonRender:GetMountEffectID()
	return self.__mount_effect_id
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--播放动作
function SkeletonRender:__PlayAnimationReal(anim_play_info)
	anim_play_info.act_index = anim_play_info.act_index or RenderObj.ActionIndex.IDLE
	anim_play_info.loop = anim_play_info.loop == nil and true or anim_play_info.loop
	anim_play_info.time_scale = anim_play_info.time_scale or 1
	anim_play_info.dir = anim_play_info.dir or MathTool.HORZ_RIGHT

	if not self:__CheckSameAnim(anim_play_info) then
		--设置动作、循环、指定帧
		self.__act_index = anim_play_info.act_index
		self.__next_act_index = 0
		self.__loop = anim_play_info.loop
		self.__fixed_frame_time = anim_play_info.fixed_frame_time
		self.__is_fixed_last_frame_time = anim_play_info.is_fixed_last_frame_time

		--清空回调
		self:ClearCallbackStatus()	

		--播放动作
		self.__skeleton_node:SetAnimation(0, RenderObj.ActionName[self.__act_index], self.__loop) 

		--设置动作、回调
		self.__next_act_index = anim_play_info.next_act_index
		self.__end_callback = anim_play_info.end_callback
		self.__event_callback = anim_play_info.event_callback		

		--设置播放速率
		self:SetTimeScale(anim_play_info.time_scale)

		--设置朝向
		self:SetDir(anim_play_info.dir)

		--播放指定帧
		PlayFixedFrame(self, anim_play_info)
	end
end

--播放动作
function SkeletonRender:__SetAnimation(anim_play_info)
end

--执行动作回调
function SkeletonRender:__RunAnimationFunc(action_name, event_type, evnet_name, int_value, float_value, string_value)
	if action_name ~= RenderObj.ActionName[self.__act_index] then
		return
	end

	if event_type == SkeletonRender.ANIMATION_COMPLETE then
		--结束回调
		if self.__end_callback then
			self.__end_callback(action_name)
		end

		--切换动作
		if self.__next_act_index and self.__next_act_index ~= 0 then
			self:SetActorState(self.__next_act_index, true)
		end

	elseif event_type == SkeletonRender.ANIMATION_EVENT then
		if self.__event_callback then
			self.__event_callback(action_name, evnet_name, int_value, float_value, string_val)
		end
	end
end

--重设资源
function SkeletonRender:__ResetSkeleton(anim_play_info, for_init)
	self.__act_index = nil

	--异步
	if not self.__is_sync then
		if for_init then
			--播放默认资源的待机动作
			self.__skeleton_node:SetAnimation(0, RenderObj.ActionName[RenderObj.ActionIndex.IDLE], true) 
		end

		--加载资源
		self.__in_async_loading = true
		Resource.LoadTool.LoadSkeletonAsync(anim_play_info.path, function(path)
			self.__path = path
			self.__in_async_loading = false
			
			if self.__skeleton_node then
				--重设新资源
				self.__skeleton_node:ResetSkeleton(self.__path)

				--播放相应动作
				self:__PlayAnimationReal(anim_play_info)

				--播放加载中缓存下来的动作
				if self.__anim_play_info_in_async then
					if self.__path ~= self.__anim_play_info_in_async.path or not self:__CheckSameAnim(self.__anim_play_info_in_async) then
						self:PlayAnimation(self.__anim_play_info_in_async)
					end
					self.__anim_play_info_in_async = nil
				end

				--处理挂载特效
				self:__HandleMountEffect(anim_play_info.mount_effect_info)
			end
		end)

	--同步
	else
		--加载并重设新资源
		if not for_init then
			self.__skeleton_node:ResetSkeleton(self.__path)
		end

		--播放相应动作
		self:__PlayAnimationReal(anim_play_info)

		--处理挂载特效
		self:__HandleMountEffect(anim_play_info.mount_effect_info)
	end
end

--处理挂载特效
function SkeletonRender:__HandleMountEffect(mount_effect_info)
	if not mount_effect_info then
		return
	end
	if not self.__skeleton_node then
		return
	end
	local slot_node = self.__skeleton_node:GetNodeBySlotName(mount_effect_info.slot_name)
	if not slot_node then
		return
	end

	if self.__mount_effect_id then
		GlobalEffectMgr:DelEffect(self.__mount_effect_id)
	end
	self.__mount_effect_id = GlobalEffectMgr:CreateUIEffect(mount_effect_info.effect_params, slot_node, mount_effect_info.zorder)
end

function SkeletonRender:__CheckSameAnim(anim_play_info)
	if self.__act_index == anim_play_info.act_index and self.__loop == anim_play_info.loop and 
		self.__time_scale == anim_play_info.time_scale and self.__dir == anim_play_info.dir and
		self.__fixed_frame_time == anim_play_info.fixed_frame_time and self.__is_fixed_last_frame_time == anim_play_info.is_fixed_last_frame_time then
		return true
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--播放指定帧
PlayFixedFrame = function(self, anim_play_info)
	--屏蔽暂停
	local is_pause = self.__is_pause
	self:ResumeAnimation()

	--播放指定帧
	local fixed_frame_time = anim_play_info.is_fixed_last_frame_time and 
	self.__skeleton_node:GetPlayAnimationTime() or anim_play_info.fixed_frame_time
	if fixed_frame_time then
		self.__skeleton_node:SetRunAnimationEx(true)
		self.__skeleton_node:SetToPlayTime(fixed_frame_time)
		self.__skeleton_node:SetRunAnimationEx(false)
	else
		self.__skeleton_node:SetRunAnimationEx(true)
	end

	--还原暂停
	if is_pause then
		self:PauseAnimation()
	end
end