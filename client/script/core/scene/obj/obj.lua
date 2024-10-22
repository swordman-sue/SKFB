
--[[
%% @module: 场景对象基类
%% @author: swordman sue
%% @created: 2016-07-11
--]]

Scene = Scene or {}

Scene.Obj = Scene.Obj or BaseClass(EventAndTimerWrapper)

local get_pos_node
local del_pos_node
local HPBarDefPos = COCOPoint(0, 115)
local NameBarDefPos = COCOPoint(0, 130)
local SpriteHalfHeadDefPos = COCOPoint(0, 175)

function Scene.Obj:__init(vo)
    self.__vo = vo
	self.__obj_type = Scene.ObjType.UNKNOWN

	--逻辑坐标 (格子坐标，左上角为原点)
	self.__logic_pos = Vector2D.New(0, 0)

	--世界坐标 (像素坐标，左上角为原点)
	self.__world_pos = Vector2D.New(0, 0)

	--朝向
	self.__dir = MathTool.HORZ_UNKNOWN

	--创建模型
	self.__model = RenderObj.New(self.__vo.render_queue)
	self.__model:AddToScene()

	--初始化状态机
	self:__InitStateMachine()
end

function Scene.Obj:__delete()
    self.is_deleted = true

	del_pos_node(self)

    --销毁血条
    if self.__hp_bar then
        self.__hp_bar:DeleteMe()
        self.__hp_bar = nil
    end

	--精灵上特效
    if self.__bless_spirit_effectid then
        GlobalEffectMgr:DelEffect(self.__bless_spirit_effectid)
        self.__bless_spirit_effectid = nil
    end
    --精灵下特效
    if self.__down_bless_spirit_effectid then
        GlobalEffectMgr:DelEffect(self.__down_bless_spirit_effectid)
        self.__down_bless_spirit_effectid = nil
    end

    --QTE圆圈特效(TODO QTE圆圈特效)
    for i=1,2 do
        if self["__qte_match_effect_id"..i] then
            GlobalEffectMgr:DelEffect(self["__qte_match_effect_id"..i])
        end
    end

    --删除QTE计时器
    if self.__qte_pause_timer then
        GlobalTimerQuest:CancelQuest(self.__qte_pause_timer)
        self.__qte_pause_timer = nil
    end

	--删除状态切换计时器
    if self.__change_status_delay_timer then
        GlobalTimerQuest:CancelQuest(self.__change_status_delay_timer)
        self.__change_status_delay_timer = nil
    end

	--删除模型
	if self.__model then
		self.__model:DeleteMe()
		self.__model = nil
	end

    --删除AI
	if self.__ai_key then
		GlobalAIMgr:DeleteAI(self.__ai_key)
		self.__ai_key = nil
	end

	--删除VO
	if self.__vo then
		self.__vo:DeleteMe()
		self.__vo = nil
	end

    --删除状态机
	self:__DeleteStateMachine()
end

function Scene.Obj:Update(now_time, elapse_time)
    if not self.__vo then
        return
    end
    if self.__is_pause then
        return
    end

    self.__state_machine:Update(elapse_time)	

    --处理延时回调
    if self.__delay_action_list then
    	self.__delay_action_checking = true
	    local del_list
	    for i, delay_info in ipairs(self.__delay_action_list) do
	    	delay_info.pass_time = delay_info.pass_time + elapse_time
	    	if delay_info.pass_time >= delay_info.delay_time then
	    		if delay_info.callback then
	    			delay_info.callback()
	    		end
	    		del_list = del_list or {}
	    		table.insert(del_list, i)
	    	end
	    end
	    if del_list then
	    	for i = #del_list, 1, -1 do
	    		table.remove(self.__delay_action_list, del_list[i])
	    	end
	    end
    	self.__delay_action_checking = false

    	if self.__delay_action_cache_list then
    		for _, delay_info in ipairs(self.__delay_action_cache_list) do
    			table.insert(self.__delay_action_list, delay_info)
    		end
    		self.__delay_action_cache_list = nil
    	end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--数据相关
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:GetVO()
	return self.__vo
end

function Scene.Obj:GetID()
	return self.__vo.obj_id
end

function Scene.Obj:GetType()
	return self.__obj_type
end

--是否己方英雄/己方助战英雄/己方精灵
function Scene.Obj:IsHero(check_assist)
	if check_assist then
		if self:IsSpirit() or self:IsHeroAssist() then
			return self:GetBindObjType() == Scene.ObjType.HERO
		end
	end
	return self.__obj_type == Scene.ObjType.HERO
end

--是否助战英雄
function Scene.Obj:IsHeroAssist()
	return self.__obj_type == Scene.ObjType.HERO_ASSIST
end

--是否敌方英雄镜像
function Scene.Obj:IsHeroMirror()
	return self:IsMonster() and self.__vo.is_hero_mirror
end

--是否PVP己方英雄/己方助战英雄/己方精灵
function Scene.Obj:IsHeroPVP(check_assist)
	if check_assist then
		if self:IsSpirit() or self:IsHeroAssist() then
			return self:GetBindObjType() == Scene.ObjType.HERO_PVP
		end
	end
	return self.__obj_type == Scene.ObjType.HERO_PVP
end

--是否英雄相关
function Scene.Obj:IsHeroRelevant()
	return self:IsHero() or 
		self:IsHeroAssist() or
		self:IsHeroMirror() or
		self:IsHeroPVP() or
		self:IsMonsterPVP()
end

--是否怪物
function Scene.Obj:IsMonster()
	return self.__obj_type == Scene.ObjType.MONSTER
end

--是否X怪物
function Scene.Obj:IsXMonster()
	return self.__obj_type == Scene.ObjType.X_MONSTER
end

--是否PVP敌方英雄/敌方助战英雄/敌方精灵
function Scene.Obj:IsMonsterPVP(check_assist)
	if check_assist then
		if self:IsSpirit() or self:IsHeroAssist() then
			return self:GetBindObjType() == Scene.ObjType.MONSTER_PVP
		end
	end
	return self.__obj_type == Scene.ObjType.MONSTER_PVP
end

--是否弹道
function Scene.Obj:IsBullet()
	return self.__obj_type == Scene.ObjType.BULLET
end

--是否精灵
function Scene.Obj:IsSpirit()
	return self.__obj_type == Scene.ObjType.SPIRIT
end

function Scene.Obj:GetMoveSpeed()
    if self.__vo.battle_attr then
    	return self.__vo.battle_attr.speed
    end
    return self.__vo.speed
end

--开启AI
function Scene.Obj:TurnonAI()
    if not self.__vo.ai_type or self.__vo.ai_type == AI.Type.UNKNOW then
        return
    end

    if not self.__ai_key then
        self.__ai_key = GlobalAIMgr:CreateAI({type = self.__vo.ai_type, obj_id = self.__vo.obj_id})        
    end
end

function Scene.Obj:SetAIKey(key)
	self.__ai_key = key
end

function Scene.Obj:GetAIKey()
	return self.__ai_key
end

function Scene.Obj:GetAI()
	return GlobalAIMgr:GetAI(self.__ai_key)
end

function Scene.Obj:PauseAI()
	if self.__ai_key then
		GlobalAIMgr:PauseAIByKey(self.__ai_key)
	end
end

function Scene.Obj:ResumeAI()
	if self.__ai_key then
		GlobalAIMgr:ResumeAIByKey(self.__ai_key)
	end
end

--设置跳过边界检测
function Scene.Obj:SetSkipBoundTest(var)
    self.__skip_bound_test = var
end

--是否跳过边界检测
function Scene.Obj:GetSkipBoundTest()
    return self.__skip_bound_test
end

--------------------------------------------------------------------------------------------------------------------------------
--模型、动画相关
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:SetLogicPos(posx, posy, update_to_model)
	self.__logic_pos.x = posx
	self.__logic_pos.y = posy
	self.__world_pos.x, self.__world_pos.y = GlobalScene:LogicToWorld(self.__logic_pos.x, self.__logic_pos.y)

	-- 更新位置到模型
	self:__UpdateModelPos(update_to_model)
end

function Scene.Obj:SetWorldPos(posx, posy, update_to_model)
	self.__world_pos.x = posx
	self.__world_pos.y = posy
	self.__logic_pos.x, self.__logic_pos.y = GlobalScene:WorldToLogic(self.__world_pos.x, self.__world_pos.y)

	-- 更新位置到模型
	self:__UpdateModelPos(update_to_model)
end

function Scene.Obj:GetLogicPos()
	return self.__logic_pos.x, self.__logic_pos.y
end

function Scene.Obj:GetLogicPosVec()
	return self.__logic_pos
end

function Scene.Obj:GetLogicPosX()
	return self.__logic_pos.x
end

function Scene.Obj:GetLogicPosY()
	return self.__logic_pos.y
end

function Scene.Obj:GetWorldPos()
	return self.__world_pos.x, self.__world_pos.y
end

function Scene.Obj:GetWorldPosVec()
	return self.__world_pos
end

function Scene.Obj:GetWorldPosX()
	return self.__world_pos.x
end

function Scene.Obj:GetWorldPosY()
	return self.__world_pos.y
end

function Scene.Obj:GetPixelX()
	local x, y = GlobalScene:WorldToPixel(self.__world_pos.x, self.__world_pos.y)
	return x
end

function Scene.Obj:GetPixelY()
	local x, y = GlobalScene:WorldToPixel(self.__world_pos.x, self.__world_pos.y)
	return y
end

function Scene.Obj:SetDir(var)
	if var == self.__dir then
		return false
	end
	self.__dir = var
	return true

	--override me
end

function Scene.Obj:GetDir()
	return self.__dir
end

function Scene.Obj:GetModel()
	return self.__model
end

function Scene.Obj:GetModelPos()
	local node = self.__model:GetNode()
	return node:GetPosition()
end

function Scene.Obj:GetModelHeight()
	--override me
end

--设置模型缩放比例
function Scene.Obj:SetScaleFactor(scale)
	self.__model:SetScaleFactor(scale)
end

--获取模型缩放比例
function Scene.Obj:GetScaleFactor()
	return self.__model:GetScaleFactor()
end

function Scene.Obj:SetVisible(var)
	self.__model:SetVisible(var)
end

function Scene.Obj:IsVisible()
	return self.__model:IsVisible()
end

function Scene.Obj:SetZOrder(var)
	self.__model:SetZOrder(var)
end

function Scene.Obj:GetZOrder()
	return self.__model:GetZOrder()
end

function Scene.Obj:SetColor(r, g, b)
end

function Scene.Obj:PushColor(color)
	self:SetColor(unpack(color))

	self.__color_stack = self.__color_stack or {}
	table.insert(self.__color_stack, color)
end

function Scene.Obj:PopColor()
	if not self.__color_stack then
		return
	end

	table.remove(self.__color_stack)

	local color = self.__color_stack[#self.__color_stack] or Macros.Color.lilyWhite
	self:SetColor(unpack(color))
end

function Scene.Obj:SetHighLight(var)
	--override me
end

function Scene.Obj:SetGray(var)
end

function Scene.Obj:SetGrayEx(var)
    local delta = var and 1 or -1
    self.__gray_count = self.__gray_count or 0
    self.__gray_count = self.__gray_count + delta

    self:SetGray(self.__gray_count > 0)
end

function Scene.Obj:SetOpacity(var)
	--override me
end

function Scene.Obj:PushOpacity(var)
	self:SetOpacity(var)

	self.__opacity_stack = self.__opacity_stack or {}
	table.insert(self.__opacity_stack, var)
end

function Scene.Obj:PopOpacity()
	table.remove(self.__opacity_stack)

	local var = self.__opacity_stack[#self.__opacity_stack] or 255
	self:SetOpacity(var)
end

function Scene.Obj:PlayAnimation(act_index, loop, time_scale)
	--override me
end

function Scene.Obj:SetAnimTimeScale(scale)
	--override me
end

function Scene.Obj:GetAnimTimeScale()
	--override me
end

function Scene.Obj:PushAnimTimeScale(scale)
	self:SetAnimTimeScale(scale)

	self.__anim_time_scale_stack = self.__anim_time_scale_stack or {}
	table.insert(self.__anim_time_scale_stack, scale)
end

function Scene.Obj:PopAnimTimeScale()
	table.remove(self.__anim_time_scale_stack)

	local scale = self.__anim_time_scale_stack[#self.__anim_time_scale_stack] or 1
	self:SetAnimTimeScale(scale)
end

function Scene.Obj:TopAnimTimeScale()
	if not self.__anim_time_scale_stack then
		return
	end
	return self.__anim_time_scale_stack[#self.__anim_time_scale_stack]
end

function Scene.Obj:PauseAnimation()
	--override me
end

function Scene.Obj:ResumeAnimation()
	--override me
end

function Scene.Obj:PauseActions()
	self.__model:PauseActions()
end

function Scene.Obj:ResumeActions()
	self.__model:ResumeActions()	
end

function Scene.Obj:Pause()
    if self.__is_pause then
        return
    end
    self.__is_pause = true

    self:PauseAI()
    self:PauseAnimation()
    self:PauseActions()
end

function Scene.Obj:Resume()
    if not self.__is_pause then
        return
    end
    self.__is_pause = false

    self:ResumeAI()
    self:ResumeAnimation()
    self:ResumeActions()
    self:ResumeExpand()
end

function Scene.Obj:ResumeExpand()
end

--设置跟随特效显隐
function Scene.Obj:SetFollowEffectVisible(var)
	self.__model:SetFollowEffectVisible(var)
end

--插入延时动作
function Scene.Obj:InsertDelayAction(callback, delay_time)
	local info = {}
	info.pass_time = 0
	info.delay_time = delay_time or 0
	info.callback = callback

	if self.__delay_action_checking then
		self.__delay_action_cache_list = self.__delay_action_cache_list or {}
		table.insert(self.__delay_action_cache_list, info)
	else
		self.__delay_action_list = self.__delay_action_list or {}
		table.insert(self.__delay_action_list, info)
	end
end

--设置血量
function Scene.Obj:SetHp(hp)
    hp = math.max(0, hp)
    hp = math.min(hp, self.__vo.max_hp)
    self.__vo.hp = hp

    --更新头顶血条
    if self.__hp_bar then
        self.__hp_bar:SetProgress(self.__vo.hp)
    end
end

--改变血条位置
function Scene.Obj:ChangeHpBarPosition(delta_x, delta_y)
    if self.__hp_bar then
        self.__hp_bar:SetPosition(HPBarDefPos.x + delta_x, (HPBarDefPos.y + delta_y) * self.__vo.model_scale_inver)
    end
end

--设置血条显隐
function Scene.Obj:SetHpBarVisible(var)
    if self.__hp_bar then
        self.__hp_bar:SetVisible(var)
    end
end

--设置血条透明度
function Scene.Obj:SetHpBarOpacity(var)
    if self.__hp_bar then
        self.__hp_bar:SetOpacity(var)
    end
end

--改变精灵祝福半身像位置
function Scene.Obj:ChangeSpriteBlessHalfHeadPosition(delta_x, delta_y)
    if self.__sp_sprite_half_head_name then
        self.__sp_sprite_half_head_name:SetPosition(SpriteHalfHeadDefPos.x + delta_x, (SpriteHalfHeadDefPos.y + delta_y) * self.__vo.model_scale_inver)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:__LoadVO()
	self:SetDir(self.__vo.dir)
	self:SetLogicPos(self.__vo.org_pos_x, self.__vo.org_pos_y)	

    --缩放
    self:SetScaleFactor(self.__vo.model_scale)    

    --血条
    if self.__vo.show_hp_bar then
        local imgdata_bar = Resource.LoadTool.GetImageData(self.__vo.hp_bar_image_info.atlas_type, self.__vo.hp_bar_image_info.bar_image_name)
        local imgdata_bg = Resource.LoadTool.GetImageData(self.__vo.hp_bar_image_info.atlas_type, self.__vo.hp_bar_image_info.bg_image_name)
        self.__hp_bar = GUI.ProgressBarLayer.New(imgdata_bar, imgdata_bg, self.__vo.hp, self.__vo.max_hp)
        -- self.__hp_bar:SetScaleFactor(self.__vo.model_scale_inver)
        self:ChangeHpBarPosition(0, 0)
        self.__model:AddChild(self.__hp_bar, RenderObj.InnerLayer.HP_BAR, RenderObj.InnerLayer.HP_BAR)
    end

    --名字
    if self.__vo.name_info then
        local name_info = self.__vo.name_info
        local lab_name = Util:Label(name_info.name, name_info.size, name_info.color, nil, nil, name_info.stroke)
        lab_name:SetAnchorPoint(0.5, 0)
        lab_name:SetScaleFactor(name_info.scale or 1)
        lab_name:SetPosition(NameBarDefPos.x, NameBarDefPos.y * self.__vo.model_scale_inver)
        self.__model:AddChild(lab_name, RenderObj.InnerLayer.NAME_BAR, RenderObj.InnerLayer.NAME_BAR)

        --称号、昵称
        if (self.__vo.title_id and self.__vo.title_id > 0) or (self.__vo.nickname_id and self.__vo.nickname_id > 0) then
            local sp_title_nickname = Util:TitleAndNickNameSpite(self.__vo.title_id, self.__vo.nickname_id, lab_name:GetContentWidth(), lab_name:GetContentHeight())
            sp_title_nickname:SetAnchorPoint(0.5, 0)
            sp_title_nickname:SetScaleFactor(name_info.scale or 1)
            sp_title_nickname:SetPosition(NameBarDefPos.x, NameBarDefPos.y * self.__vo.model_scale_inver)
            self.__model:AddChild(sp_title_nickname, RenderObj.InnerLayer.NAME_BAR, RenderObj.InnerLayer.NAME_BAR)
        end                
    end

    --影子
    if self.__vo.show_shadow then
        self.__sp_shadow = Util:Sprite(Resource.PathTool.GetModelShadowPath())
        self.__sp_shadow:SetOpacity(128)
        GlobalScene:AddToRenderGroup(self.__sp_shadow, Scene.RenderQueue.GRQ_SHADOW)
        self.__model:BindFollowNode(self.__sp_shadow, nil, Scene.RenderQueue.GRQ_SHADOW)
    end	
end

--更新模型位置
function Scene.Obj:__UpdateModelPos()
    local pixelx, pixely = GlobalScene:WorldToPixel(self.__world_pos.x, self.__world_pos.y)
	self.__model:SetPosition(pixelx, pixely)
end

function Scene.Obj:__SetupAnimPlayInfo(anim_play_info, act_index, loop, time_scale)
	anim_play_info.act_index = act_index
	anim_play_info.loop = loop
	anim_play_info.time_scale = time_scale	
	anim_play_info.dir = self.__dir
	anim_play_info.def_path = Resource.PathTool.GetSkeletonDefaultPath()
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_pos_node = function(obj_self)
	if not obj_self.__pos_node then
		obj_self.__pos_node = LabelEx.CreateWithString("(0, 0)", Util.FontType, 24)
		obj_self.__pos_node:SetColor(255, 0, 0)
		obj_self.__pos_node:SetAnchorPoint(0.5, 1)
		obj_self.__pos_node:SetStroke(true)
		obj_self.__pos_node:SetStrokeColor(0, 0, 0, 255)
		obj_self.__model:AddChild(obj_self.__pos_node)
	end
	return obj_self.__pos_node
end

del_pos_node = function(obj_self)
	if obj_self.__pos_node then
		obj_self.__pos_node:RemoveSelf(true)
		obj_self.__pos_node = nil
	end
end
