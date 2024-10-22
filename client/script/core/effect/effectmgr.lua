
--[[
%% @module: 特效管理器
%% @author: swordman sue
%% @created: 2016-07-13
%% @description: 创建和管理特效对象
--]]

Effect = Effect or {}

Effect.EffectMgr = Effect.EffectMgr or BaseClass()

Effect.EffectMgr.identify_count = Effect.EffectMgr.identify_count or 0
Effect.EffectMgr.inner_identify_count = Effect.EffectMgr.inner_identify_count or 0

Effect.EffectMgr.BindType =
{
	BIND_SCENE = 0,
	BIND_TARGET = 1,
	BIND_UI = 2,
}

local gen_effect_root_id
local gen_inner_effect_id
local get_effect_class
local add_effect
local handle_params
local get_effect_delay_time
local get_effect_offset
local get_effect_scale

function Effect.EffectMgr.CreateInstance()
	if not GlobalEffectMgr then
		GlobalEffectMgr = Effect.EffectMgr.New()
	end
end

function Effect.EffectMgr.DelInstance()
	if GlobalEffectMgr then
		GlobalEffectMgr:DeleteMe()
		GlobalEffectMgr = nil
	end
end

function Effect.EffectMgr:__init()
	-- 特效对象表
	self.__inner_effect_map = {}

	-- 特效对象计数表
	self.__inner_effect_count_map = {}

	-- 绑定到场景对象的特效查询表，key:obj_id value:inner_effect_id_map
	self.__inner_effect_of_tgt_lookup_map = {}

	-- 特效根节点表
	self.__effect_root_map = {}

	-- 绑定到场景对象的特效查询表, key:obj_id value:effect_root_id_map
	self.__effect_root_of_tgt_lookup_map = {}

	-- 根节点特效ID与内部特效ID的关联表，key:根节点特效ID value:内部特效ID表
	self.__id_associated_map = {}

	-- 根节点特效ID与场景对象ID的关联表，key:根节点特效ID value:场景对象ID
	self.__id_associated_map2 = {}

	-- 延迟创建表，key:根节点特效ID value:特效构造信息表
	self.__delay_create_map = {}

    GlobalRunner:AddRunObj(self)	
end

function Effect.EffectMgr:__delete()
	self:__ClearAllEffect(true)

	GlobalRunner:RemoveRunObj(self)
end

function Effect.EffectMgr:PrintCache()
	print(8, "---------------Effect.EffectMgr:PrintCache---------------")
	for _, effect in pairs(self.__inner_effect_map) do
		print(8, effect:GetResName())
	end
end

function Effect.EffectMgr:Update(now_time, elapse_time)
	if self.__is_pause then
		return
	end

	local del_id_list
	for effect_root_id, create_info_list in pairs(self.__delay_create_map) do
		local is_all_created = self:__CheckCreate(create_info_list, elapse_time)
		if is_all_created then
			del_id_list = del_id_list or {}
			table.insert(del_id_list, effect_root_id)
		end
	end

	if del_id_list then
		for _, effect_root_id in ipairs(del_id_list) do
			self.__delay_create_map[effect_root_id] = nil
		end
	end
end

--[[
切场景相关销毁
--]]
function Effect.EffectMgr:Dump()
    self.__is_pause = false
	self:__ClearAllEffect(false)
end

function Effect.EffectMgr:Pause()
	if self.__is_pause then
		return
	end
	self.__is_pause = true

	for _, effect in pairs(self.__inner_effect_map) do
		effect:Pause()
	end
end

function Effect.EffectMgr:Resume()
    if not self.__is_pause then
        return
    end
    self.__is_pause = false

	for _, effect in pairs(self.__inner_effect_map) do
		effect:Resume()
	end
end

--[[
@ params：
	必须：
	id					[number]特效ID
	或
	type				[number]特效类型
    res_name			[string]特效文件名

	可选：   
	参考effect.lua
--]]

--创建场景特效
function Effect.EffectMgr:CreateScenePosEffect(params, render_queue)
	render_queue = render_queue or Scene.RenderQueue.GRQ_BUILD_AND_PLAYER

	local invalid_params = handle_params(params)
	if not invalid_params then
		return
	end

	local effect_root_id, effect_root = self:__CreateEffectRoot(params)
	GlobalScene:AddToRenderGroup(effect_root, render_queue, params.zorder, params.id and params.id or -1)

	self:__InitCreateInfos(effect_root_id, params, Effect.EffectMgr.BindType.BIND_SCENE)

	return effect_root_id, effect_root
end

--创建场景对象特效
function Effect.EffectMgr:CreateSceneTargetEffect(params, obj_id, inner_layer)
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		LogTool.LogError("[Effect.EffectMgr:CreateSceneTargetEffect] 找不到对应的场景对象，obj_id:%d", obj_id)
		return
	end
	local obj_model = obj:GetModel()
	if not obj_model then
		LogTool.LogError("[Effect.EffectMgr:CreateSceneTargetEffect] 找不到场景对象的模型")
		return
	end

	local invalid_params = handle_params(params)
	if not invalid_params then
		return
	end

	local effect_root_id, effect_root = self:__CreateEffectRoot(params)
	obj_model:AddChild(effect_root, inner_layer, params.id and params.id or -1)

	self:__InitCreateInfos(effect_root_id, params, Effect.EffectMgr.BindType.BIND_TARGET, obj_id)

	return effect_root_id, effect_root
end

--创建UI特效
function Effect.EffectMgr:CreateUIEffect(params, parent, zorder)
	zorder = zorder or 0

	if not parent then
		LogTool.LogError("[Effect.EffectMgr:CreateUIEffect] 参数parent无效")		
		return
	end

	local invalid_params = handle_params(params)
	if not invalid_params then
		return
	end
	params.loop = params.loop == nil and true or params.loop

	local effect_root_id, effect_root = self:__CreateEffectRoot(params)
	parent:AddChild(effect_root, zorder, params.id and params.id or -1)

	self:__InitCreateInfos(effect_root_id, params, Effect.EffectMgr.BindType.BIND_UI)

	return effect_root_id, effect_root
end

--获取特效根节点
function Effect.EffectMgr:GetEffect(effect_root_id)
	if not effect_root_id then
		return
	end
	return self.__effect_root_map[effect_root_id]
end

--获取特效对象
function Effect.EffectMgr:GetInnerEffect(effect_root_id, effect_index)
	effect_index = effect_index or 1
	if not effect_root_id then
		return
	end

	local effect_id_list = self.__id_associated_map[effect_root_id]
	if not effect_id_list then
		return
	end

	return self:__GetInnerEffect(effect_id_list[effect_index])	
end

--检测某对象上的同名特效是否超出指定数量
function Effect.EffectMgr:TestEffectNumByResName(target_id, res_name, limit_num)
    if not target_id or not res_name then
        return false
    end
    limit_num = limit_num or 0

    local ret = true
    local effect_num = 0
	self:__TargetEffectsIter(target_id, function(inner_effect_id, effect)
		if effect and effect:GetResName() == res_name then
			effect_num = effect_num + 1
			if effect_num >= limit_num then
				ret = false
				return true
			end			
    	end		
	end)

    return ret
end

--检测某对象上的同ID特效是否超出指定数量
function Effect.EffectMgr:TestEffectNumByConfigID(target_id, config_id, limit_num)
    if not target_id or not config_id then
        return false
    end
    limit_num = limit_num or 0

    local ret = true
    local effect_num = 0
	self:__TargetEffectRootsIter(target_id, function(effect_root_id, effect_root)
		if effect_root and effect_root:GetTag() == config_id then
			effect_num = effect_num + 1
			if effect_num >= limit_num then
				ret = false
				return true
			end			
    	end		
	end)

    return ret
end

--删除指定场景对象绑定的所有特效
function Effect.EffectMgr:DelEffectsByTargetID(target_id)
    local del_list
	self:__TargetEffectRootsIter(target_id, function(effect_root_id, effect_root)
		del_list = del_list or {}
		table.insert(del_list, effect_root_id)		
	end)

    if del_list then
		for _, effect_root_id in pairs(del_list) do
			self:DelEffect(effect_root_id)
		end
	end
end

--暂停指定场景对象绑定的所有特效
function Effect.EffectMgr:PaueEffectsByTargetID(target_id)
	self:__TargetEffectsIter(target_id, function(inner_effect_id, effect)
		effect:Pause()
	end)
end

--恢复指定场景对象绑定的所有特效
function Effect.EffectMgr:ResumeEffectsByTargetID(target_id)
	self:__TargetEffectsIter(target_id, function(inner_effect_id, effect)
		effect:Resume()
	end)
end

--暂停特效
function Effect.EffectMgr:PasueEffect(effect_root_id)
	self:__InnerEffectsIter(effect_root_id, function(inner_effect)
		inner_effect:Pause()
	end)
end

--恢复特效
function Effect.EffectMgr:ResumeEffect(effect_root_id)
	self:__InnerEffectsIter(effect_root_id, function(inner_effect)
		inner_effect:Resume()
	end)
end

--设置指定场景对象绑定的所有特效的显隐
function Effect.EffectMgr:SetEffectsVisibleByTargetID(target_id, var)
	self:__TargetEffectRootsIter(target_id, function(effect_root_id, effect_root)
		effect_root:SetVisible(var)
	end)

	local target = GlobalScene:GetObj(target_id)
	if target then
		target:SetFollowEffectVisible(var)
	end
end

--设置指定特效显隐
function Effect.EffectMgr:SetEffectVisible(effect_root_id, var)
	local effect_root = self:GetEffect(effect_root_id)
	if effect_root then
		effect_root:SetVisible(var)
	end
end

--设置指定特效朝向
function Effect.EffectMgr:SetEffectDir(effect_root_id, var)
	self:__InnerEffectsIter(effect_root_id, function(inner_effect)
		inner_effect:SetFlipX(var ~= MathTool.HORZ_RIGHT)
	end)
end

--设置骨骼特效动作
function Effect.EffectMgr:SetEffectAnim(effect_root_id, act_index, loop, end_callback)
	self:__InnerEffectsIter(effect_root_id, function(inner_effect)
		inner_effect:SetAnim(act_index, loop, end_callback)
	end)	
end

--删除特效
function Effect.EffectMgr:DelEffect(effect_root_id)
	if not effect_root_id then
		return
	end

	--移除延迟创建信息
	self.__delay_create_map[effect_root_id] = nil

	--移除ID关联信息
	local effect_id_list = self.__id_associated_map[effect_root_id]
	if effect_id_list then
		for _, inner_effect_id in pairs(effect_id_list) do
			self:DelInnerEffect(inner_effect_id, false)
		end
	end
	self.__id_associated_map[effect_root_id] = nil

	--更新关联表
	local target_id = self.__id_associated_map2[effect_root_id]
	self.__id_associated_map2[effect_root_id] = nil

	--更新查询表
	self:__DelEffectRootOfTgtRecord(target_id, effect_root_id)

	--移除根节点、根节点信息
	local effect_root = self.__effect_root_map[effect_root_id]
	if effect_root then
		effect_root:RemoveSelf(true)
		CPPNodeUnity.Release(effect_root)
	end
	self.__effect_root_map[effect_root_id] = nil
end

--删除内部特效
function Effect.EffectMgr:DelInnerEffect(inner_effect_id, del_frm_root)
	if not inner_effect_id then
		return
	end

	local effect = self.__inner_effect_map[inner_effect_id]
	if effect then
		local bind_type = effect:GetBindType()
		local target_id = effect:GetTargetID()
		local effect_root_id = effect:GetRootID()
		local effect_index = effect:GetIndex()

		--更新计数表
		self:__DelInnerEffectCountByType(bind_type)

		--更新查询表
		self:__DelInnerEffectOfTgtRecord(target_id, inner_effect_id)

		--销毁特效
		effect:DeleteMe()

		self.__inner_effect_map[inner_effect_id] = nil

		--更新ID关联信息
		if del_frm_root ~= false then
			local effect_id_list = self.__id_associated_map[effect_root_id]
			if effect_id_list then
				effect_id_list[effect_index] = nil
				if not next(effect_id_list) then
					self:DelEffect(effect_root_id)
				end				
			end
		end		
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--初始化创建信息
function Effect.EffectMgr:__InitCreateInfos(effect_root_id, params, bind_type, ...)
	local res_name_list = params.res_name_list
	local delay_time_list = params.delay_time_list
	local offset_list = params.offset_list
	local scale_list = params.scale_list
	params.res_name_list = nil
	params.delay_time_list = nil
	params.offset_list = nil
	params.scale_list = nil

	--处理外部结束回调
	local inner_end_count = 0
	local external_end_callback = params.end_callback
	local inner_effect_end_callback = function()
		inner_end_count = inner_end_count + 1
		if inner_end_count >= #res_name_list then
			if external_end_callback then
				external_end_callback()
			end
		end
	end
	params.end_callback = nil

	--绑定对象
	local target_id
	if bind_type == Effect.EffectMgr.BindType.BIND_TARGET then
		target_id = select(1, ...)

		--更新关联表
		self.__id_associated_map2[effect_root_id] = target_id

		--更新查询表
		self:__AddEffectRootOfTgtRecord(target_id, effect_root_id)
	end

	--重置延迟创建表、ID关联信息表
	self.__delay_create_map[effect_root_id] = {}
	self.__id_associated_map[effect_root_id] = {}

	for index, res_name in ipairs(res_name_list) do
		--初始化ID关联信息
		self.__id_associated_map[effect_root_id][index] = 0

		local create_info = {}		
		--公共参数
		create_info.bind_type = bind_type
		create_info.effect_root_id = effect_root_id
		create_info.effect_index = index
		create_info.create_delay_time = get_effect_delay_time(delay_time_list, index)
		create_info.create_elapse_time = 0

		--特殊参数
		create_info.target_id = target_id

		--特效对象参数
		create_info.params = CPPGameLib.CopyTblShallow(params) --此处使用浅拷贝，前提是params中的字段都是值类型
		--配置ID
		create_info.params.config_id = params.id
		--资源名
		create_info.params.res_name = res_name
		--偏移
		create_info.params.pos_x, create_info.params.pos_y = get_effect_offset(offset_list, index)
		create_info.params.pos_x = create_info.params.pos_x * params.dir
		--缩放
		create_info.params.scale = get_effect_scale(scale_list, index)
		--结束回调
		create_info.params.end_callback = inner_effect_end_callback

		--初始化延迟创建表
		table.insert(self.__delay_create_map[effect_root_id], create_info)
	end

	--进行首次特效创建检测
	self:__CheckCreate(self.__delay_create_map[effect_root_id], 0)
end

--特效创建检测
function Effect.EffectMgr:__CheckCreate(create_info_list, elapse_time)
	local is_all_created = true
	for _, create_info in ipairs(create_info_list) do
		if not create_info.has_created then
			create_info.create_elapse_time = create_info.create_elapse_time + elapse_time
			if create_info.create_elapse_time >= create_info.create_delay_time then
				create_info.has_created = true
				self:__CreateInnerEffect(create_info)
			else
				is_all_created = false
			end
		end
	end	
	return is_all_created
end

--创建特效对象
function Effect.EffectMgr:__CreateInnerEffect(create_info)
	local params = create_info.params
	local effect_root = self:GetEffect(create_info.effect_root_id)

	local effect_class = get_effect_class(params.type)
	local effect_obj = effect_class.New(params)
	if effect_obj and effect_obj:GetNode() and effect_root then
		effect_obj:SetBindType(create_info.bind_type)
		effect_obj:SetRootID(create_info.effect_root_id)
		effect_obj:SetIndex(create_info.effect_index)
		effect_obj:SetID(gen_inner_effect_id())
		effect_obj:SetTargetID(create_info.target_id)

		self:__AddInnerEffect(effect_obj)
		effect_root:AddChild(effect_obj:GetNode(), effect_obj:GetIndex())

	elseif effect_obj then
		effect_obj:DeleteMe()
		LogTool.LogError("[Effect.EffectMgr:__CreateInnerEffect] 创建特效对象失败")
	end						
end

--创建特效根节点
function Effect.EffectMgr:__CreateEffectRoot(params)
	local effect_root = Node.Create()
	effect_root:SetCascadeOpacityEnabled(true)
	CPPNodeUnity.Retain(effect_root)

	--根节点位置
	local effect_root_id = gen_effect_root_id()
	if params.pos_x and params.pos_y then
		effect_root:SetPosition(params.pos_x, params.pos_y)
	end

	--根节点缩放
	if params.scale then
		effect_root:SetScaleFactor(params.scale)
	end

	--播放音效
	if params.sound_id and not params.forbid_sound then
		GlobalSoundMgr:PlayEffectByConfigID(params.sound_id)
	end

	--延时回调
	if params.delay_time and params.delay_callback then
		local delay_callback = params.delay_callback
		CPPActionManager.DelayTo(effect_root, params.delay_time, GlobalCallbackMgr:AddCallBackFunc(function()
			delay_callback()
		end))
	end
	
	self.__effect_root_map[effect_root_id] = effect_root
	return effect_root_id, effect_root
end

--获取内部特效对象
function Effect.EffectMgr:__GetInnerEffect(inner_effect_id)
	if not inner_effect_id then
		return
	end
	return self.__inner_effect_map[inner_effect_id]
end

--清空所有特效
function Effect.EffectMgr:__ClearAllEffect(cleanup)
	for _, effect in pairs(self.__inner_effect_map) do
		effect:DeleteMe()
	end

	for id, effect_root in pairs(self.__effect_root_map) do
		effect_root:RemoveSelf(true)
		CPPNodeUnity.Release(effect_root)
	end

	self.__inner_effect_map = not cleanup and {} or nil
	self.__inner_effect_count_map = not cleanup and {} or nil
	self.__inner_effect_of_tgt_lookup_map = not cleanup and {} or nil
	self.__effect_root_map = not cleanup and {} or nil
	self.__effect_root_of_tgt_lookup_map = not cleanup and {} or nil
	self.__id_associated_map = not cleanup and {} or nil
	self.__id_associated_map2 = not cleanup and {} or nil
	self.__delay_create_map = not cleanup and {} or nil	
end

--添加内部特效
function Effect.EffectMgr:__AddInnerEffect(effect)
	if not effect then
		return
	end

	local bind_type = effect:GetBindType()
	local effect_root_id = effect:GetRootID()
	local effect_index = effect:GetIndex()
	local inner_effect_id = effect:GetID()
	local target_id = effect:GetTargetID()

	self.__inner_effect_map[inner_effect_id] = effect

	-- 更新计数表
	self:__AddInnerEffectCountByType(bind_type)

	-- 更新查询表
	self:__AddInnerEffectOfTgtRecord(target_id, inner_effect_id)

	--更新ID关联信息
	local effect_id_list = self.__id_associated_map[effect_root_id]
	if effect_id_list then
		effect_id_list[effect_index] = inner_effect_id
	end
end

function Effect.EffectMgr:__AddEffectRootOfTgtRecord(target_id, effect_root_id)
	if not target_id then
		return
	end

	self.__effect_root_of_tgt_lookup_map[target_id] = self.__effect_root_of_tgt_lookup_map[target_id] or {}
	self.__effect_root_of_tgt_lookup_map[target_id][effect_root_id] = true
end

function Effect.EffectMgr:__DelEffectRootOfTgtRecord(target_id, effect_root_id)
	if not target_id then
		return
	end	
	self.__effect_root_of_tgt_lookup_map[target_id] = self.__effect_root_of_tgt_lookup_map[target_id] or {}
	self.__effect_root_of_tgt_lookup_map[target_id][effect_root_id] = nil
end

function Effect.EffectMgr:__AddInnerEffectOfTgtRecord(target_id, inner_effect_id)
	if not target_id then
		return
	end
	self.__inner_effect_of_tgt_lookup_map[target_id] = self.__inner_effect_of_tgt_lookup_map[target_id] or {}
	self.__inner_effect_of_tgt_lookup_map[target_id][inner_effect_id] = true
end

function Effect.EffectMgr:__DelInnerEffectOfTgtRecord(target_id, inner_effect_id)
	if not target_id then
		return
	end
	self.__inner_effect_of_tgt_lookup_map[target_id] = self.__inner_effect_of_tgt_lookup_map[target_id] or {}
	self.__inner_effect_of_tgt_lookup_map[target_id][inner_effect_id] = nil
end

function Effect.EffectMgr:__AddInnerEffectCountByType(bind_type)
	self.__inner_effect_count_map[bind_type] = self.__inner_effect_count_map[bind_type] or 0
	self.__inner_effect_count_map[bind_type] = self.__inner_effect_count_map[bind_type] + 1
end

function Effect.EffectMgr:__DelInnerEffectCountByType(bind_type)
	self.__inner_effect_count_map[bind_type] = self.__inner_effect_count_map[bind_type] or 0
	self.__inner_effect_count_map[bind_type] = self.__inner_effect_count_map[bind_type] - 1
end

function Effect.EffectMgr:__GetInnerEffectCountByType(bind_type)
	self.__inner_effect_count_map[bind_type] = self.__inner_effect_count_map[bind_type] or 0
	return self.__inner_effect_count_map[bind_type]
end

--绑定在指定场景对象上的所有特效的迭代器
function Effect.EffectMgr:__TargetEffectsIter(target_id, iter_func)
	if not target_id or not iter_func then
		return
	end

    local inner_effect_id_map = self.__inner_effect_of_tgt_lookup_map[target_id]
    if inner_effect_id_map then
    	local effect
    	for inner_effect_id, _ in pairs(inner_effect_id_map) do
    		effect = self:__GetInnerEffect(inner_effect_id)
    		if iter_func(inner_effect_id, effect) then
    			break
    		end
    	end
    end
end

--绑定在指定场景对象上的所有特效根节点的迭代器
function Effect.EffectMgr:__TargetEffectRootsIter(target_id, iter_func)
	if not target_id or not iter_func then
		return
	end

    local effect_root_id_map = self.__effect_root_of_tgt_lookup_map[target_id]
    if effect_root_id_map then
    	local effect
    	for effect_root_id, _ in pairs(effect_root_id_map) do
    		effect = self:GetEffect(effect_root_id)
    		if iter_func(effect_root_id, effect) then
    			break
    		end
    	end
    end
end

--某根节点特效ID下，所有内部特效的迭代器
function Effect.EffectMgr:__InnerEffectsIter(effect_root_id, iter_func)
	local effect_id_list = self.__id_associated_map[effect_root_id]
	if effect_id_list then
		local inner_effect
		for _, inner_effect_id in pairs(effect_id_list) do
			inner_effect = self:__GetInnerEffect(inner_effect_id)
			if iter_func(inner_effect) then
				break
			end
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
gen_effect_root_id = function()
	Effect.EffectMgr.identify_count = Effect.EffectMgr.identify_count + 1
	return Effect.EffectMgr.identify_count
end

gen_inner_effect_id = function()
	Effect.EffectMgr.inner_identify_count = Effect.EffectMgr.inner_identify_count + 1
	return Effect.EffectMgr.inner_identify_count
end

get_effect_class = function(type)
	local effect_class
	CPPGameLib.Switch(type)
	{
		[Effect.EffectType.PARTICLE] = function()
			effect_class = Effect.ParticleEffect
		end,
		[Effect.EffectType.SKELETON] = function()
			effect_class = Effect.SkeletonEffect
		end,
		[Effect.EffectType.SEQFRAME] = function()
			effect_class = Effect.SeqFrameEffect
		end,
	}
	return effect_class
end

handle_params = function(params)
	--朝向
	params.dir = params.dir or MathTool.HORZ_RIGHT

	if params.id then
		local config_effect = CPPGameLib.GetConfig("Effect", params.id)
		if not config_effect then
			LogTool.LogInfo("[handle_params] 特效ID无效")
			return false
		end
		params.type = config_effect.type
		--特效文件列表
		params.res_name_list = config_effect.res_name_list
		--特效延时创建列表
		params.delay_time_list = config_effect.delay_time_list
		--特效偏移列表
		params.offset_list = config_effect.offset_list
		--特效缩放列表
		params.scale_list = config_effect.scale_list
		--音效ID
		params.sound_id = config_effect.sound_id

		if not params.res_name_list then
			LogTool.LogInfo("[handle_params] 配置中找不到res_name_list字段，特效ID：%d", params.id)
			return false
		end
	else
		params.type = params.type or Effect.EffectType.PARTICLE
		params.res_name_list = {}
		table.insert(params.res_name_list, params.res_name)
	end

	return true
end

get_effect_delay_time = function(delay_time_list, index)
	if not delay_time_list or not index then
		return 0 
	end

	return delay_time_list[index] or 0
end

get_effect_offset = function(offset_list, index)
	if not offset_list or not index then
		return 0, 0
	end
	if not offset_list[index] then
		return 0, 0
	end

	return offset_list[index][1] or 0, offset_list[index][2] or 0
end

get_effect_scale = function(scale_list, index)
	if not scale_list or not index then
		return 1
	end
	return scale_list[index] or 1
end
