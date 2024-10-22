
--[[
%% @module: 渲染对象基类
%% @author: swordman sue
%% @created: 2016-07-12
--]]

RenderObj = RenderObj or BaseClass(Layer)

--内部层次
RenderObj.InnerLayer =
{
	SHADOW = 5,
	RIDER_DOWN = 15,		--坐骑下层
	BODY_EFFECT_DOWN = 25,	--下层特效
	BODY = 35,				--身体层
	BODY_EFFECT_UP = 45,	--上层特效
	RIDER_UP = 55,			--坐骑上层
	WEAPON = 65,			--武器层
	HP_BAR = 70,			--血条层
	NAME_BAR = 75,			--名字、称号、昵称
	HP_EFFECT = 80,			--飘血特效
	ALL_KIND_OF_TIPS = 999,	--各种提示
}

--动画类型
RenderObj.AnimType =
{
	UNKNOWN = 0,
	SKELETON = 1,			--骨骼
	SEQFRAME = 2,			--序列帧
}

--动作索引(定义后不允许修改)
RenderObj.ActionIndex =
{
	IDLE = 1,
	IDLE2 = 15,
	IDLE3 = 16,
	RUN = 2,
	ATTACK = 3,
	ATTACK_NEAR = 4,
	SP_SKILL = 5,
	ANGER_SKILL = 6,
	JOINT_SKILL = 7,
	HIT = 10,
	HIT_FLY = 11,
	REPEL = 12,
	DEAD = 13,
	WIN = 14,

	BOSS_SKILL1 = 17,
	BOSS_SKILL2 = 18,
	BOSS_SKILL3 = 19,
	BOSS_SKILL4 = 20,

	ANIMATION = 30,
}

--动作名(可根据动作资源修改)
--[[
待机：idle
跑步：run
受击：hit
击退：repel
普攻：attack
特殊技能：sp_skill
怒气技能：anger_skill
合击技能：joint_skill
死亡：dead
胜利：win
BOSS技能：boss_skill1
BOSS技能：boss_skill2
BOSS技能：boss_skill3
BOSS技能：boss_skill4
--]]
RenderObj.ActionName =
{
	[RenderObj.ActionIndex.IDLE] = "idle",
	[RenderObj.ActionIndex.RUN] = "run",
	[RenderObj.ActionIndex.ATTACK] = "attack",
	[RenderObj.ActionIndex.ATTACK_NEAR] = "attack_near",
	[RenderObj.ActionIndex.SP_SKILL] = "sp_skill",
	[RenderObj.ActionIndex.ANGER_SKILL] = "anger_skill",
	[RenderObj.ActionIndex.JOINT_SKILL] = "joint_skill",
	[RenderObj.ActionIndex.HIT] = "hit",
	[RenderObj.ActionIndex.HIT_FLY] = "hit_fly",
	[RenderObj.ActionIndex.REPEL] = "repel",
	[RenderObj.ActionIndex.DEAD] = "dead",
	[RenderObj.ActionIndex.WIN] = "win",

	[RenderObj.ActionIndex.IDLE2] = "idle2",
	[RenderObj.ActionIndex.IDLE3] = "idle3",

	[RenderObj.ActionIndex.BOSS_SKILL1] = "boss_skill1",
	[RenderObj.ActionIndex.BOSS_SKILL2] = "boss_skill2",
	[RenderObj.ActionIndex.BOSS_SKILL3] = "boss_skill3",
	[RenderObj.ActionIndex.BOSS_SKILL4] = "boss_skill4",

	[RenderObj.ActionIndex.ANIMATION] = "animation",
}

--跟随节点类型
RenderObj.FollowNodeType = 
{
	NODE = 1,
	OBJ = 2,
	EFFECT_ID = 3,
}

--跟随节点的位置更新类型
RenderObj.FollowNodePosUpdateType = 
{
	UPDATE_XY = 1,
	UPDATE_X = 2,
}

function RenderObj:__init(render_queue)
	self.__render_queue = render_queue or Scene.RenderQueue.GRQ_BUILD_AND_PLAYER
end

function RenderObj:__delete()
	--移除绑定的跟随节点
	if self.__follow_node_map then
		for _, info in pairs(self.__follow_node_map) do
			self:__DelFollowNode(info)
		end
		self.__follow_node_map = nil
	end
end

--添加到场景
function RenderObj:AddToScene()
	GlobalScene:AddToRenderGroup(self, self.__render_queue)
end

--添加到父容器
function RenderObj:AddToParent(parent, order, tag)
	if not parent then
		return
	end

	if IsClassOrInheritClass(parent, Layer) or IsClassOrInheritClass(parent, UILayer) then
		parent:AddChild(self, order, tag)
	else
		parent:AddChild(self:GetNode(), order, tag)
	end
end

--绑定跟随节点(被绑定的跟随节点，必须通过UnbindFollowNode或者RenderObj:__delete移除)
function RenderObj:BindFollowNode(node, node_type, render_queue, pos_update_type, offset_x, offset_y)
	local info = {}
	info.node = node
	info.node_type = node_type or RenderObj.FollowNodeType.NODE
	info.render_queue = render_queue or Scene.RenderQueue.GRQ_BUILD_AND_PLAYER
	info.pos_update_type = pos_update_type or RenderObj.FollowNodePosUpdateType.UPDATE_XY
	info.offset_x = offset_x or 0
	info.offset_y = offset_y or 0

	self.__follow_node_id = self.__follow_node_id or 0
	self.__follow_node_id = self.__follow_node_id + 1
	self.__follow_node_map = self.__follow_node_map or {}
	self.__follow_node_map[self.__follow_node_id] = info

	--初始化跟随节点位置
	local x, y = self:GetPosition()
	local pixel_x, pixel_y = GlobalScene:RenderGroup2Pixel(x, y, self.__render_queue)
	x, y = GlobalScene:Pixel2RenderGroup(pixel_x, pixel_y, info.render_queue)

	local node = self:__GetFollowNode(info)
	if node then
		node:SetPosition(x + info.offset_x, y + info.offset_y)
	end

	return self.__follow_node_id
end

--解绑跟随节点
function RenderObj:UnbindFollowNode(id)
	if not id then
		return
	end
	if not self.__follow_node_map then
		return
	end
	
	self:__DelFollowNode(self.__follow_node_map[id])
	self.__follow_node_map[id] = nil
end

--获取跟随节点
function RenderObj:GetFollowNode(id)
	if not self.__follow_node_map then
		return
	end

	return self:__GetFollowNode(self.__follow_node_map[id])
end

--设置跟随特效显隐
function RenderObj:SetFollowEffectVisible(var)
	if self.__follow_node_map then
		local node
		for _, info in pairs(self.__follow_node_map) do
			if info.node_type == RenderObj.FollowNodeType.EFFECT_ID then
				node = self:__GetFollowNode(info)
				if node then
					node:SetVisible(var)
				end
			end
		end
	end	
end

--[[
播放动画
@ params：
	anim_type			[number]动画类型
	inner_layer			[number]内部层次

	--动画参数(参考对应动画类)
	--....
--]]

function RenderObj:PlayAnimation(anim_play_info)
	local child_render = self:GetChildLayerByTag(anim_play_info.inner_layer)
	if not child_render or child_render:GetAnimType() ~= anim_play_info.anim_type then
		if child_render then
			child_render:DeleteMe()
		end
		CPPGameLib.Switch(anim_play_info.anim_type)
		{
			[RenderObj.AnimType.SKELETON] = function()
				child_render = SkeletonRender.New(anim_play_info)
			end,
			[RenderObj.AnimType.SEQFRAME] = function()
				child_render = SeqFrameRender.New(anim_play_info)
			end
		}
		self:AddChild(child_render, anim_play_info.inner_layer, anim_play_info.inner_layer)
	else
		child_render:PlayAnimation(anim_play_info)
	end
end

function RenderObj:SetAnimEndCallback(callback, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetAnimEndCallback(callback)
	end
end

function RenderObj:PauseAnimation(inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:PauseAnimation()
	end
end

function RenderObj:ResumeAnimation(inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:ResumeAnimation()
	end
end

function RenderObj:SetAnimTimeScale(scale, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetTimeScale(scale)
	end
end

function RenderObj:GetAnimTimeScale()
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		return child:GetTimeScale()
	end
	return 1.0
end

function RenderObj:SetPosition(x, y)
	local pixel_x, pixel_y = x, y
	x, y = GlobalScene:Pixel2RenderGroup(pixel_x, pixel_y, self.__render_queue)
	Layer.SetPosition(self, x, y)

	--更新跟随节点位置
	self:__UpdateFollowNodesPosition(pixel_x, pixel_y)

	--设置渲染组深度脏位
	GlobalScene:SetRendrGroupZOrderDirty(self.__render_queue)
end

function RenderObj:SetScaleFactor(scale, inner_layer)
	if not inner_layer then
		Layer.SetScaleFactor(self, scale)
	else
		local child = self:GetChildLayerByTag(inner_layer)
		if child then
			child:SetScaleFactor(scale)
		end
	end
end

function RenderObj:GetScaleFactor(inner_layer)
	if not inner_layer then
		return Layer.GetScaleFactor(self)
	else
		local child = self:GetChildLayerByTag(inner_layer)
		if child then
			return child:GetScaleFactor()
		end
	end
	return 0
end

function RenderObj:SetRotation(rotation, inner_layer)
	if not inner_layer then
		Layer.SetRotation(self, rotation)
	else
		local child = self:GetChildLayerByTag(inner_layer)
		if child then
			child:SetRotation(rotation)
		end
	end
end

function RenderObj:SetDir(var, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetDir(var)
	end
end

function RenderObj:SetColor(r, g, b, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetColor(r, g, b)
	end
end

function RenderObj:SetHighLight(var, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetHighLight(var)
	end
end

function RenderObj:SetGray(var, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetGray(var)
	end
end

function RenderObj:SetOpacity(var, inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		child:SetOpacity(var)
	end
end

function RenderObj:SetVisible(var, inner_layer)
	if not inner_layer then
		Layer.SetVisible(self, var)

		if self.__follow_node_map then
			local node
			for _, info in pairs(self.__follow_node_map) do
				node = self:__GetFollowNode(info)
				if node then
					node:SetVisible(var)
				end
			end
		end
	else
		local child = self:GetChildLayerByTag(inner_layer)
		if child then
			child:SetVisible(var)
		end
	end
end

function RenderObj:IsVisible(inner_layer)
	if not inner_layer then
		return Layer.IsVisible(self)
	else
		local child = self:GetChildLayerByTag(inner_layer)
		if child then
			return child:IsVisible()
		end
	end
end

--获取对应的世界坐标
function RenderObj:GetWorldPos()
	local pixel_x, pixel_y = self:GetPosition()
	pixel_x, pixel_y = GlobalScene:RenderGroup2Pixel(pixel_x, pixel_y, self.__render_queue)
	return GlobalScene:PixelToWorld(pixel_x, pixel_y)
end

--获取模型高度
function RenderObj:GetModelHeight(inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		return child:GetHeight()
	end
end

--获取模型宽度
function RenderObj:GetModelWidth(inner_layer)
	local child = self:GetChildLayerByTag(inner_layer)
	if child then
		return child:GetWidth()
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--获取跟随节点
function RenderObj:__GetFollowNode(info)
	local node
	if not info then
		return node
	end

	if info.node_type == RenderObj.FollowNodeType.NODE then
		node = info.node

	elseif info.node_type == RenderObj.FollowNodeType.OBJ then
		node = info.node:GetNode()

	elseif info.node_type == RenderObj.FollowNodeType.EFFECT_ID then
		node = GlobalEffectMgr:GetEffect(info.node)
	end
	return node
end

--删除跟随节点
function RenderObj:__DelFollowNode(info)
	if not info then
		return
	end

	if info.node then
		if info.node_type == RenderObj.FollowNodeType.NODE then
			info.node:RemoveSelf(true)

		elseif info.node_type == RenderObj.FollowNodeType.OBJ then
			info.node:DeleteMe()

		elseif info.node_type == RenderObj.FollowNodeType.EFFECT_ID then
			GlobalEffectMgr:DelEffect(info.node)
		end
		info.node = nil
	end
end

--更新跟随节点位置
function RenderObj:__UpdateFollowNodesPosition(pixel_x, pixel_y)
	if self.__follow_node_map then
		local x, y, node
		for _, info in pairs(self.__follow_node_map) do
			x, y = GlobalScene:Pixel2RenderGroup(pixel_x, pixel_y, info.render_queue)

			node = self:__GetFollowNode(info)
			if node then
				if info.pos_update_type == RenderObj.FollowNodePosUpdateType.UPDATE_XY then
					node:SetPosition(x + info.offset_x, y + info.offset_y)

				elseif info.pos_update_type == RenderObj.FollowNodePosUpdateType.UPDATE_X then
					node:SetPosition(x + info.offset_x, node:GetPositionY())
				end		
			end			
		end
	end
end