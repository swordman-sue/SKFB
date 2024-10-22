
--[[
%% @module: 场景
%% @author: swordman sue
%% @created: 2016-07-11
%% @description: 负责创建和管理地图、场景对象....
--]]

Scene = Scene or {}

Scene.Scene = Scene.Scene or BaseClass(EventAndTimerWrapper)

--渲染层次(注：前景、中景、远景、地形层的数值不能修改)
Scene.RenderQueue =
{
    GRQ_INVALID = 0,
    GRQ_DISTANCE = 10,          --远景层
    GRQ_MEDIUM = 15,            --中景层
    GRQ_TERRAIN = 20,           --地形层
    GRQ_UI_DOWN = 25,           --底层UI
    GRQ_SHADOW = 30,            --阴影层
    GRQ_SHADOW_EFFECT = 31,     --阴影特效层
    GRQ_BUILD_AND_PLAYER = 35,  --建筑和人物
    GRQ_FRONT = 40,             --前景层
    GRQ_AIR = 45,               --天空层
    GRQ_AIR_TOP = 50,           --天空上层(飘血特效...)
    GRQ_UI_DOWN_II = 55,        --底层UI
    GRQ_UI = 60,                --UI层
    GRQ_UI_UP = 65,             --上层UI
    GRQ_UI_UP_II = 70,          --上层UI2(公告、指引、剧情)
    GRQ_UI_TOP = 75,            --顶层UI(协议超时提示、连接服务器失败提示、界面loading、协议loading)
    GRQ_UI_MAX = 80,            --顶层UI(断线重连、指引防废号提示框、世界之心特效)
    GRQ_UI_MAX_II = 85,         --顶层UI(游戏消息弹窗ShowGameDialog)

    GRQ_MAX = 8888,             --层次上限
}

Scene.ObjType =
{
    UNKNOWN     = 0,
    HERO        = 1,
    MONSTER     = 2,
    BULLET      = 3,
    DROPBOX     = 4,
    SPIRIT      = 5,
    HERO_ASSIST = 6,
    X_MONSTER   = 7,
    HERO_PVP    = 8,
    MONSTER_PVP = 9,
    MT_HERO     = 10,
    MT_MONSTER  = 11,
    MT_ITEM     = 12,
    MT_SPIRIT   = 13, 
}

Scene.ObjID = Scene.ObjID or 0

--场景人数上限，obj_id：[0, Scene.MaxObjNum-1]
Scene.MaxObjNum = 1000000

--场景尺寸上限
Scene.MaxSize = COCOSize(16384, 16384)
--场景尺寸上限对应的幂
Scene.MaxSizePower = {14, 14}
--场景对象深度上限
Scene.MaxZOrder = Scene.MaxSize.height * Scene.MaxSize.width + Scene.MaxSize.width

--精灵与英雄的坐标偏移
Scene.SpiritFollowSpace = 20
Scene.SpiritOffset = Vector2D.New(32, -200)

--场景切换超时时间
Scene.SceneChangeTimeout = 6

local get_render_group
local is_render_queue_need_sort

function Scene.Scene.CreateInstance()
	if not GlobalScene then
		GlobalScene = Scene.Scene.New()
	end
end

function Scene.Scene.DelInstance()
	if GlobalScene then
		GlobalScene:DeleteMe()
		GlobalScene = nil
	end
end

function Scene.Scene:__init()
	self.__node = CPPGameScene.Node()
    CPPNodeUnity.Retain(self.__node)
	CPPSceneManager:RunWithScene(self.__node)

	self:__InitScene()
	self:__NewRes()
end

function Scene.Scene:__delete()
	self:Dump(false)	
	self:__DelRes()

	self.__camera:DeleteMe()
	self.__camera = nil
	self.__map:DeleteMe()
	self.__map = nil

	for _, render_group in pairs(self.__render_group_map) do
		render_group:DeleteMe()
	end
	self.__render_group_map = nil

    CPPNodeUnity.Release(self.__node)    
end

function Scene.Scene:Update(now_time, elapse_time)
    if self.__is_in_pause then
        return
    end

	self:__UpdateBefore(now_time, elapse_time)
	self.__camera:Update(now_time, elapse_time)
	self.__map:Update(now_time, elapse_time)
	self:__UpdateAfter(now_time, elapse_time)
end

function Scene.Scene:Pause()
    if self.__is_in_pause then
        return
    end
    self.__is_in_pause = true

    for _, obj in pairs(self.__obj_map) do
        obj:Pause()
    end
end

function Scene.Scene:Resume()
    if not self.__is_in_pause then
        return
    end
    self.__is_in_pause = false

    for obj_id, obj in pairs(self.__obj_map) do
        if self.__screen_pause_info.is_triggered then
            if obj_id == self.__screen_pause_info.trigger_obj_id or obj:IsSpirit() then
                obj:Resume()
            end
        else
            obj:Resume()
        end
    end
end

--切换场景
function Scene.Scene:ChangeScene(type, id)
    if not GlobalVOMgr:SetSceneInfo(type, id) then
        LogTool.LogError("[Scene.Scene:ChangeScene] 切换场景失败")
        return false
    end

    if GlobalStateMgr:IsSceneChangeState() then
        --处于场景切换状态中，先调用之前的HandleQuitSceneChangeState
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleQuitSceneChangeState", function()
            self:FireNextFrame(Macros.Event.GameState.SCENE_CHANGE_STATE_QUIT)
        end)                 
    end

    --清空协议loading
    GlobalNetAdapter:ResetNetLoading()

    --清空双端对象ID映射关系
    CallModuleFunc(ModuleType.SCENE, "ClearRecordOfObjID")

    --销毁旧的特殊逻辑
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "Stop")

    --创建新的特殊逻辑
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "Start")

    --进入切场景状态
    GlobalStateMgr:ToSceneChangeState()

    return true
end

function Scene.Scene:__InitScene()
    self.__render_group_map = self.__render_group_map or {}
    self.__camera = self.__camera or Scene.Camera.New()
    self.__map = self.__map or Scene.Map.New()

    self.__obj_map = {}
    self.__mainrole_obj_id = nil

    -- 延时处理的函数列表
    self.__delay_handle_funcs = {}

    self.__is_in_update = false
    self.__is_in_pause = false

    -- 技能触发的全屏暂停信息
    self.__screen_pause_info = {}
    self.__screen_pause_info.is_triggered = false

    self.__sort_rg_childs_interval = self.__sort_rg_childs_interval or 4

    -- 重置渲染组显隐
    local render_group
    for _, render_queue in pairs(Scene.RenderQueue) do
        if not Scene.Scene.IsUIRenderQueue(render_queue) then
            render_group = self:GetRenderGroup(render_queue, false)
            if render_group then            
                render_group:SetVisible(true)
                render_group:SetTouchEnabled(false)
                render_group:SetTouchFun(nil, nil, nil)
            end
        end
    end    
end

function Scene.Scene:Dump(clear_camera)
    if clear_camera ~= false then
        self.__camera:Clear()
    end

    -- 清空地图资源
    self:__UnloadSceneRes()    
    
    -- 清空场景对象
    for _, v in pairs(self.__obj_map) do
        v:DeleteMe()
    end
    self.__obj_map = {}

    -- 清空非UI渲染组
    local render_group
    for _, render_queue in pairs(Scene.RenderQueue) do
        if not Scene.Scene.IsUIRenderQueue(render_queue) then
            render_group = self:GetRenderGroup(render_queue, false)
            if render_group then            
                render_group:RemoveAllChild()
            end
        end
    end

    -- 全屏暂停
    self.__screen_pause_info.is_triggered = false
end

function Scene.Scene:RegistAllEvents()
    -- 进入SceneChangingState
    local scene_changing_enter_response = function(_)
        self:__InitScene()
        self:__LoadSceneRes()
    end
    self:BindGlobalEvent(Macros.Event.GameState.SCENE_CHANGE_STATE_ENTER, scene_changing_enter_response)

    -- 进入GamePlayingState
    local game_playing_enter_response = function()
        -- TEMP：临时处理地图预加载失败导致的地图创建失败问题
        self.__map:LoadForRemedy()
    end
    self:BindGlobalEvent(Macros.Event.GameState.GAME_PLAY_STATE_ENTER, game_playing_enter_response)

    -- 退出GamePlayingState
    local game_playing_quit_response = function(_)
        self:Dump()
    end
    self:BindGlobalEvent(Macros.Event.GameState.GAME_PLAY_STATE_QUIT, game_playing_quit_response) 
end

function Scene.Scene:__UpdateBefore(now_time, elapse_time)
    if self.__is_in_pause then
        return
    end

    -- 更新场景对象
    self.__is_in_update = true

    for _, v in pairs(self.__obj_map) do
        v:Update(now_time, elapse_time)
    end

    self.__is_in_update = false

    -- 调用延时函数
    for _, v in ipairs(self.__delay_handle_funcs) do
        v()
    end
    self.__delay_handle_funcs = {}
end

function Scene.Scene:__UpdateAfter(now_time, elapse_time)
    self:__SortRenderGroupChilds(now_time, elapse_time)
end

--------------------------------------------------------------------------------------------------------------------------------
--渲染组
--------------------------------------------------------------------------------------------------------------------------------
--是否UI相关层
function Scene.Scene.IsUIRenderQueue(render_queue)
    return render_queue == Scene.RenderQueue.GRQ_UI_DOWN or
            render_queue == Scene.RenderQueue.GRQ_UI_DOWN_II or
            render_queue == Scene.RenderQueue.GRQ_UI or
            render_queue == Scene.RenderQueue.GRQ_UI_UP or 
            render_queue == Scene.RenderQueue.GRQ_UI_UP_II or
            render_queue == Scene.RenderQueue.GRQ_UI_TOP or 
            render_queue == Scene.RenderQueue.GRQ_UI_MAX or 
            render_queue == Scene.RenderQueue.GRQ_UI_MAX_II 
end

--是否跟随GRQ_TERRAIN
function Scene.Scene.IsFollowTerrain(render_queue)
    return render_queue == Scene.RenderQueue.GRQ_SHADOW or
            render_queue == Scene.RenderQueue.GRQ_SHADOW_EFFECT or
            render_queue == Scene.RenderQueue.GRQ_BUILD_AND_PLAYER or
            render_queue == Scene.RenderQueue.GRQ_AIR or
            render_queue == Scene.RenderQueue.GRQ_AIR_TOP
end

function Scene.Scene:AddToRenderGroup(child, render_queue, order, tag)
	local render_group = get_render_group(self, render_queue)
    if render_group then
    	render_group:AddChild(child, order, tag)

        --设置渲染组深度脏位
        self:SetRendrGroupZOrderDirty(render_queue)
    end
end

function Scene.Scene:GetRenderGroup(render_queue, auto_create)
    local render_group = get_render_group(self, render_queue, auto_create)
    return render_group
end

function Scene.Scene:GetRenderGroupChild(render_queue, tag)
    local render_group = self:GetRenderGroup(render_queue, false)
    if render_group then
        return render_group:GetChildByTag(tag)
    end
end

function Scene.Scene:SetRenderGroupsVisible(render_queue_list, var)
    local render_group
    for _, render_queue in ipairs(render_queue_list) do
        render_group = self:GetRenderGroup(render_queue)
        if render_group then
            render_group:SetVisible(var)
        end
    end
end

function Scene.Scene:SetRenderGroupChildVisible(render_queue, tag, visible)
    local render_group_child = self:GetRenderGroupChild(render_queue, tag)
    if not render_group_child then
        return
    end

    render_group_child:SetVisible(visible)
end

function Scene.Scene:SetRenderGroupPos(x, y, render_queue)
	local render_group = get_render_group(self, render_queue)
    if render_group then
    	render_group:SetPosition(x, y)
    end
end

function Scene.Scene:GetRenderGroupPos(x, y, render_queue)
    local render_group = get_render_group(self, render_queue)
    if render_group then
        render_group:GetPosition()
    end
end

function Scene.Scene:SetRenderGroupScale(scale, render_queue)
    local render_group = get_render_group(self, render_queue)
    if render_group then
        render_group:SetScaleFactor(scale)
    end
end

--设置渲染组深度脏位
function Scene.Scene:SetRendrGroupZOrderDirty(render_queue)
    if is_render_queue_need_sort(render_queue) then
        self.__zorder_dirty_groups = self.__zorder_dirty_groups or {}
        self.__zorder_dirty_groups[render_queue] = true
    end    
end

--设置渲染组触碰区域
function Scene.Scene:SetRenderGroupTouchArena(render_queue, x, y, width, height)
    local render_group = get_render_group(self, render_queue)
    if render_group then
        render_group:SetTouchRect(x, y, width, height)
    end
end

--设置渲染组触碰回调
function Scene.Scene:SetRenderGroupTouchFunc(render_queue, down_func, move_func, up_func)
    local render_group = get_render_group(self, render_queue)
    if render_group then
        render_group:SetTouchFun(down_func, move_func, up_func)
    end
end

--设置渲染组的子节点进行深度排序的间隔
function Scene.Scene:SetRenderGroupSortInterval(var)
    self.__sort_rg_childs_interval = var or 4
end

--对渲染组的子节点进行深度排序
function Scene.Scene:SortRenderGroupChildsForce()
    self.__sort_rg_childs_elapse_time = MathTool.MAX_NUM
    self:__SortRenderGroupChilds()
end

--对渲染组的子节点进行深度排序
function Scene.Scene:__SortRenderGroupChilds(now_time, elapse_time)
    self.__sort_rg_childs_elapse_time = self.__sort_rg_childs_elapse_time or 0
    self.__sort_rg_childs_elapse_time = self.__sort_rg_childs_elapse_time + (elapse_time or 0)

    if self.__screen_pause_info.is_triggered then
        --暂停其他对象中，过滤(怒气/合击技能中，将释放者置于顶层)
        return
    end

    if self.__sort_rg_childs_elapse_time < self.__sort_rg_childs_interval then
        return
    end
    self.__sort_rg_childs_elapse_time = 0

    if self.__zorder_dirty_groups then
        local render_group
        for render_queue, _ in pairs(self.__zorder_dirty_groups) do
            render_group = get_render_group(self, render_queue)
            if render_group then
                render_group:SortAllChildByPos(Scene.MaxSizePower)
            end
        end
        self.__zorder_dirty_groups = nil
    end
end

--清空所有渲染组
function Scene.Scene:ClearAllRenderGroups()
    local render_group
    for _, render_queue in pairs(Scene.RenderQueue) do
        render_group = self:GetRenderGroup(render_queue, false)
        if render_group then            
            render_group:RemoveAllChild()
        end
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--地图
--------------------------------------------------------------------------------------------------------------------------------
--是否存在渲染组对应的地图
function Scene.Scene:HasMapGroup(render_queue)
    return self.__map:HasGroup(render_queue)
end

--获取各渲染组对应的地图数据
function Scene.Scene:GetMapGroupList()
	return self.__map:GetGroupList()    
end

--获取渲染组对应的地图尺寸
function Scene.Scene:GetMapGroupSize(render_queue)
	return self.__map:GetGroupSize(render_queue)
end

--获取地形层对应的地图尺寸
function Scene.Scene:GetMapSize()
    return self:GetMapGroupSize(Scene.RenderQueue.GRQ_TERRAIN)
end

--获取地图尺寸(单位：格子)
function Scene.Scene:GetMapTileSize()
    local map_width, map_height = self:GetMapSize()
    return map_width / Scene.Map.TileUnitSize.width, map_height / Scene.Map.TileUnitSize.height
end

--获取加载完成标识
function Scene.Scene:IsMapLoaded()
    return self.__map:IsLoaded()
end

--获取加载进度信息
function Scene.Scene:GetMapLoadInfo()
    return self.__map:GetLoadInfo()
end

--置灰地图块
function Scene.Scene:SetMapGray(var)
    self.__map:SetGray(var)
end

--获取地图模式
function Scene.Scene:GetMapMode()
    return self.__map:GetMode()
end

--------------------------------------------------------------------------------------------------------------------------------
--镜头
--------------------------------------------------------------------------------------------------------------------------------
--绑定镜头
function Scene.Scene:BindCamera(obj_id, is_update_pos, fixed_scale)
    local obj = self:GetObj(obj_id)
    self.__camera:RegisterObserver(obj)
    self.__camera:SetFixedScale(fixed_scale)

    if is_update_pos ~= false then
        self.__camera:UpdatePosDirect(obj:GetWorldPosVec())
    end
end

--解绑镜头
function Scene.Scene:UnBindCamera()
    self.__camera:UnRegisterObserver()
end

--获取镜头中心位置
function Scene.Scene:GetCameraPos()
    return self.__camera:GetNowPos()
end

--更新镜头中心位置
function Scene.Scene:SetCameraPos(pos, is_smooth, smooth_end_callback, smooth_speed, lock_max_camera_y)
    if not is_smooth then
        self.__camera:UpdatePosDirect(pos)
    else
        self.__camera:UpdatePosSmooth(pos, smooth_speed, smooth_end_callback, lock_max_camera_y)
    end
end

--设置镜头到场景中心
function Scene.Scene:SetCameraCenter()
    local map_width, map_height = self:GetMapSize()
    self:SetCameraPos(Vector2D.New(map_width / 2, map_height / 2))
end

--获取镜头缩放
function Scene.Scene:GetCameraScale()
    return self.__camera:GetNowScale()
end

--更新镜头缩放
function Scene.Scene:SetCameraScale(scale, is_smooth, smooth_end_callback, smooth_speed)
    if not is_smooth then
        self.__camera:UpdateScaleDirect(scale)
    else
        self.__camera:UpdateScaleSmooth(scale, smooth_speed, smooth_end_callback)
    end
end

--获取镜头可视大小
function Scene.Scene:GetCameraVisualSize(render_queue, calc_scale)
    return self.__camera:GetVisualSize(render_queue, calc_scale)
end

--获取镜头包围盒
function Scene.Scene:GetCameraBounding(render_queue, calc_scale)
    local camera_now_pos = self.__camera:GetNowPos()
    local camera_visual_width, camera_visual_height = self.__camera:GetVisualSize(render_queue, calc_scale)

    local camera_bound = {}
    camera_bound.org = Vector2D.New(camera_now_pos.x - camera_visual_width / 2, camera_now_pos.y - camera_visual_height / 2)
    camera_bound.size = COCOSize(camera_visual_width, camera_visual_height)
    return camera_bound
end

--[[
地形、地形跟随层的镜头震动
@ params：
    震动类型, 震幅, 震频，持续时间
--]]
function Scene.Scene:CameraShake(type, range, frequency, last_time)
    self.__camera:TerrainShake(type, range, frequency, last_time)
end

--暂停镜头(战斗跳过)
function Scene.Scene:PauseCamera()
    self.__camera:Pause()
end

--恢复镜头(战斗跳过)
function Scene.Scene:ResumeCamera()
    self.__camera:Resume()
end

--------------------------------------------------------------------------------------------------------------------------------
--场景对象
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Scene:GetObj(obj_id)
    if not obj_id then
        return nil
    end
    return self.__obj_map[obj_id]
end

function Scene.Scene:GetMainRoleObjID()
    return self.__mainrole_obj_id
end

function Scene.Scene:GetMainRoleObj()
    return self:GetObj(self.__mainrole_obj_id)
end

function Scene.Scene:GetObjMap()
    return self.__obj_map
end

function Scene.Scene:DeleteObj(obj_id)
    local obj = self.__obj_map[obj_id]
    if obj ~= nil then
        if obj.is_deleted then
            return
        end
        local del_func = function()
            if not obj_id or not self.__obj_map[obj_id] then
                return
            end
            local obj = self.__obj_map[obj_id]
            self.__obj_map[obj_id] = nil
            obj:DeleteMe()
        end

        if self.__is_in_update then
            -- 更新过程延迟删除
            table.insert(self.__delay_handle_funcs, del_func)
        else
            -- 直接删除对象
            del_func()
        end
    end
end

function Scene.Scene:SetObjGray(var)
    for _, obj in pairs(self.__obj_map) do
        obj:SetGrayEx(var)
    end
end

--创建英雄
function Scene.Scene:CreateHero(vo, turnon_ai)
    local obj = self:__CreateObj(vo, Scene.Hero)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end
    return obj
end

--创建英雄镜像
function Scene.Scene:CreateHeroMirror(vo, turnon_ai)
    local obj = self:__CreateObj(vo, Scene.Monster)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end
    return obj
end

--创建助战英雄
function Scene.Scene:CreateHeroAssist(vo, turnon_ai, bind_obj_type)
    local obj = self:__CreateObj(vo, Scene.HeroAssist)
    obj:SetBindObjType(bind_obj_type)    
    if turnon_ai then
        obj:TurnonAI()
    end
    return obj
end

--创建PVP英雄
function Scene.Scene:CreateHeroPVP(vo, turnon_ai)
    local obj = self:__CreateObj(vo, Scene.HeroPVP)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end
    return obj
end

--创建英雄(多人场景)
function Scene.Scene:CreateMtHero(vo)
    local obj = self:__CreateObj(vo, Scene.MtHero)
    CallModuleFunc(ModuleType.SCENE, "RecordObjID", Scene.ObjType.MT_HERO, vo.obj_id, vo.role_id)
    obj:DoIdle()

    if vo.role_id == CallModuleFunc(ModuleType.ROLE, "GetRoleID") then
        self.__mainrole_obj_id = vo.obj_id
    end
    return obj
end

--创建怪物
function Scene.Scene:CreateMonster(vo, turnon_ai)
    local obj = self:__CreateObj(vo, Scene.Monster)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end

    --免疫击飞
    if vo.immune_ctrl_skill or vo.immune_ctrl_skill2 then
        obj:SetForbidBeAttackedFloat(true)
    end

    return obj
end

--在原地创建一个x怪物，释放x技能后销毁
function Scene.Scene:CreateXMonster(vo, ea_skill_info)
    local obj = self:__CreateObj(vo, Scene.XMonster)    
    obj:SetDir(MathTool.HORZ_LEFT)
    obj:DoAttack(ea_skill_info.id, nil, ea_skill_info.type)        

    return obj
end

--创建PVP怪物
function Scene.Scene:CreateMonsterPVP(vo, turnon_ai)
    local obj = self:__CreateObj(vo, Scene.MonsterPVP)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end
    return obj
end

--创建怪物(多人场景)
function Scene.Scene:CreateMtMonster(vo, turnon_ai)
    local obj = self:__CreateObj(vo, Scene.MtMonster)
    CallModuleFunc(ModuleType.SCENE, "RecordObjID", Scene.ObjType.MT_MONSTER, vo.obj_id, vo.uid)
    obj:DoIdle()

    if turnon_ai then
        obj:TurnonAI()
    end
    return obj
end

--创建弹道
function Scene.Scene:CreateBullet(vo)
    local obj = self:__CreateObj(vo, Scene.Bullet)
    obj:DoIdle()

    local dst_pos = Vector2D.New(vo.dst_pos_x, vo.dst_pos_y)
    if vo.parabola_factor then
        --抛物线弹道
        obj:DoParabolaChase(dst_pos)
    else
        --非抛物线弹道
        obj:DoMove(dst_pos)
    end
    return obj
end

--创建掉落宝箱
function Scene.Scene:CreateDropBox(vo)
    local obj = self:__CreateObj(vo, Scene.DropBox)
    obj:DoIdle()
    return obj
end

--创建采集物(多人场景)
function Scene.Scene:CreateMtItem(vo)
    local obj = self:__CreateObj(vo, Scene.MtItem)
    CallModuleFunc(ModuleType.SCENE, "RecordObjID", Scene.ObjType.MT_ITEM, vo.obj_id, vo.uid)
    obj:DoIdle()
    return obj
end

--创建精灵
function Scene.Scene:CreateSpirit(vo, turnon_ai, bind_obj_type)
    local obj = self:__CreateObj(vo, Scene.Spirit)
    obj:SetBindObjType(bind_obj_type)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end    
    return obj
end

--创建精灵(多人场景)
function Scene.Scene:CreateMtSpirit(vo, turnon_ai, bind_obj_id)
    local obj = self:__CreateObj(vo, Scene.MtSpirit)
    obj:SetBindObjID(bind_obj_id)
    obj:DoIdle()
    if turnon_ai then
        obj:TurnonAI()
    end    
    return obj
end

--我方全体死亡
function Scene.Scene:DeleteAllHeros()
    self:ResumeOtherObjs()
    self:Resume()

    local obj
    local hero_vo_map = GlobalVOMgr:GetHeroMap()
    for obj_id, _ in pairs(hero_vo_map) do
        obj = self:GetObj(obj_id)
        obj:DoDead()
        self:DeleteObj(obj_id)
    end
end

--敌方全体死亡
function Scene.Scene:DeleteAllMonsters()
    self:ResumeOtherObjs()
    self:Resume()

    local obj
    local monster_vo_map = GlobalVOMgr:GetMonsterMap()
    for obj_id, _ in pairs(monster_vo_map) do
        obj = self:GetObj(obj_id)
        obj:DoDead()
        self:DeleteObj(obj_id)
    end
end

--暂停其他对象
function Scene.Scene:PauseOtherObjs(trigger_obj_id,ignore_camera)
    if self.__screen_pause_info.is_triggered then
        return
    end
    self.__screen_pause_info.is_triggered = true
    self.__screen_pause_info.trigger_obj_id = trigger_obj_id

    for obj_id, obj in pairs(self.__obj_map) do
        if obj_id ~= trigger_obj_id then
            if obj:IsBullet() then
                obj:SetVisible(false)
                obj:Pause()

            elseif not obj:IsSpirit() then
                obj:Pause()
            end
        end
    end

    --暂停镜头
    if not ignore_camera then
        self:PauseCamera()
    end
    --暂停BUFF
    GlobalBuffMgr:PauseByScreen(trigger_obj_id)
    --暂停AI
    GlobalAIMgr:PauseByScreen()
    --暂停倒计时
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "PauseByScreen")
end

--恢复其他对象
function Scene.Scene:ResumeOtherObjs()
    if not self.__screen_pause_info.is_triggered then
        return
    end
    self.__screen_pause_info.is_triggered = false

    local trigger_obj_id = self.__screen_pause_info.trigger_obj_id
    for obj_id, obj in pairs(self.__obj_map) do
        if obj_id ~= trigger_obj_id then
            if obj:IsBullet() then
                --恢复场景中的弹道
                obj:SetVisible(true)
                obj:Resume()

            elseif not obj:IsSpirit() then
                obj:Resume()
            end
       end
    end
    self.__screen_pause_info.trigger_obj_id = nil

    --恢复镜头
    self:ResumeCamera()
    --恢复BUFF
    GlobalBuffMgr:ResumeByScreen(trigger_obj_id)
    --恢复AI
    GlobalAIMgr:ResumeByScreen()  
    --恢复倒计时
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "ResumeByScreen")
end

function Scene.Scene:GenerateObjID()
    Scene.ObjID = Scene.ObjID + 1
    return Scene.ObjID
end

function Scene.Scene:__CreateObj(vo, obj_class)
    local obj_id = vo.obj_id > 0 and vo.obj_id or self:GenerateObjID()
    if self.__obj_map[obj_id] then
        self.__obj_map[obj_id]:DeleteMe()
    end

    vo.obj_id = obj_id
    local obj = obj_class.New(vo)
    self.__obj_map[obj_id] = obj
    return obj
end

--------------------------------------------------------------------------------------------------------------------------------
--坐标
--------------------------------------------------------------------------------------------------------------------------------
--GRQ_TERRAIN层坐标转为render_queue层的坐标
function Scene.Scene:Pixel2RenderGroup(x, y, render_queue)
    if render_queue == Scene.RenderQueue.GRQ_DISTANCE
    or render_queue == Scene.RenderQueue.GRQ_MEDIUM
    or render_queue == Scene.RenderQueue.GRQ_FRONT then
        local cur_group_width, cur_group_height = self:GetMapGroupSize(render_queue)
        local terrain_group_width, terrain_group_height = self:GetMapGroupSize(Scene.RenderQueue.GRQ_TERRAIN)
        return x * cur_group_width / terrain_group_width, y * cur_group_height / terrain_group_height
    end
    return x, y
end

--render_queue层的坐标转为GRQ_TERRAIN层坐标
function Scene.Scene:RenderGroup2Pixel(x, y, render_queue)
    if render_queue == Scene.RenderQueue.GRQ_DISTANCE
    or render_queue == Scene.RenderQueue.GRQ_MEDIUM
    or render_queue == Scene.RenderQueue.GRQ_FRONT then
        local cur_group_width, cur_group_height = self:GetMapGroupSize(render_queue)
        local terrain_group_width, terrain_group_height = self:GetMapGroupSize(Scene.RenderQueue.GRQ_TERRAIN)
        return x / cur_group_width * terrain_group_width, y / cur_group_height * terrain_group_height
    end
    return x, y
end

function Scene.Scene:FlipWorldPos(x, y)
    local _, map_height = self:GetMapSize()
    return x, map_height - y
end

-- 逻辑坐标转世界坐标
function Scene.Scene:LogicToWorld(x, y)
    return x * Scene.Map.TileUnitSize.width, y * Scene.Map.TileUnitSize.height
end
function Scene.Scene:LogicToWorldVec(pos)
    return Vector2D.New(pos.x * Scene.Map.TileUnitSize.width, pos.y * Scene.Map.TileUnitSize.height)
end

-- 世界坐标转逻辑坐标
function Scene.Scene:WorldToLogic(x, y)
    return x / Scene.Map.TileUnitSize.width, y / Scene.Map.TileUnitSize.height
end
function Scene.Scene:WorldToLogicVec(pos)
    return Vector2D.New(pos.x / Scene.Map.TileUnitSize.width, pos.y / Scene.Map.TileUnitSize.height)
end

-- 逻辑坐标转OpenGL坐标
function Scene.Scene:LogicToPixel(x, y)
    x, y = self:LogicToWorld(x, y)
    return self:FlipWorldPos(x, y)
end
function Scene.Scene:LogicToPixelVec(pos)
    x, y = self:LogicToWorld(x, y)
    return Vector2D.New(self:FlipWorldPos(pos.x, pos.y))
end

-- OpenGL坐标转逻辑坐标
function Scene.Scene:PixelToLogic(x, y)
    x, y = self:FlipWorldPos(x, y)
    return self:WorldToLogic(x, y)
end
function Scene.Scene:PixelToLogicVec(pos)
    x, y = self:FlipWorldPos(x, y)
    return Vector2D.New(self:WorldToLogic(pos.x, pos.y))
end

-- 世界坐标转OpenGL坐标
function Scene.Scene:WorldToPixel(x, y)
    return self:FlipWorldPos(x, y)
end
function Scene.Scene:WorldToPixelVec(pos)
    return Vector2D.New(self:FlipWorldPos(pos.x, pos.y))
end

-- OpenGL坐标转世界坐标
function Scene.Scene:PixelToWorld(x, y)
    return self:FlipWorldPos(x, y)
end
function Scene.Scene:PixelToWorldVec(pos)
    return Vector2D.New(self:FlipWorldPos(pos.x, pos.y))
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--获取渲染组
get_render_group = function(obj_self, render_queue, auto_create)
	local render_group = obj_self.__render_group_map[render_queue]
	if not render_group and auto_create ~= false then
		render_group = Layer.New()
        if Scene.Scene.IsUIRenderQueue(render_queue) then
            --UI渲染组，设置尺寸
            render_group:SetContentSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
        end
		obj_self.__node:AddLayer(render_group:GetNode(), render_queue, render_queue)
		obj_self.__render_group_map[render_queue] = render_group
	end
	return render_group
end

--检测渲染组是否需要排序
is_render_queue_need_sort = function(render_queue)
    return render_queue == Scene.RenderQueue.GRQ_BUILD_AND_PLAYER
end


