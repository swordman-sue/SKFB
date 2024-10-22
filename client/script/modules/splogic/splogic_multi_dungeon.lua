
--[[
%% @module: 多人场景
%% @author: swordman sue
%% @created: 2017-09-25
--]]

Modules = Modules or {}

Modules.SpMultiDungeonModule = Modules.SpMultiDungeonModule or BaseClass(Modules.SplogicModule)

function Modules.SpMultiDungeonModule:__init()
    self.__item_notice_toggle = false
    self.__monster_notice_toggle = false
end

function Modules.SpMultiDungeonModule:__delete()
end

--初始化
function Modules.SpMultiDungeonModule:Initialize()
	Modules.SplogicModule.Initialize(self)
end

--释放
function Modules.SpMultiDungeonModule:Dispose()
	Modules.SplogicModule.Dispose(self)
end

function Modules.SpMultiDungeonModule:Update(now_time, elapse_time)
	Modules.SplogicModule.Update(self, now_time, elapse_time)
end

--进入GamePlaying状态
function Modules.SpMultiDungeonModule:GamePlayingStartHandle()   
    Modules.SplogicModule.GamePlayingStartHandle(self)

    --播放BGM
    self:PlayBGM()

    --创建场景对象
    self:__CreateObjs()

    --创建触碰区域
    self:__CreateTouchArea()

    self.__is_started = true

    --强制执行场景对象深度排序
    GlobalScene:SetRenderGroupSortInterval(0.5)
    GlobalScene:SortRenderGroupChildsForce()

    GlobalScene:BindCamera(GlobalScene:GetMainRoleObjID(), true, true)

    --设置运行帧率
    -- CPPSceneManager:SetGameFrame(Macros.Global.FPS_Dungeon)
end

--离开GamePlaying状态
function Modules.SpMultiDungeonModule:GamePlayingStopHandle()
    Modules.SplogicModule.GamePlayingStopHandle(self)

    --停止BGM
    self:StopBGM()
end

--播放BGM
function Modules.SpMultiDungeonModule:PlayBGM()
    local config_scene = GlobalVOMgr:GetSceneConfig()
    if config_scene and config_scene.bgm_name then
        GlobalSoundMgr:PlayBGM(config_scene.bgm_name, nil, config_scene.bgm_volume, true)
    end
end

--处理进入切场景状态
function Modules.SpMultiDungeonModule:HandleEnterSceneChangeState(callback)
    --打开loading界面
    CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.UI_LOADING_II, function()
        if callback then callback(true) end
    end)
end

--处理离开切场景状态
function Modules.SpMultiDungeonModule:HandleQuitSceneChangeState(callback)
    --关闭loading界面
    GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)    

    if callback then callback() end
end

--处理场景进入通知
function Modules.SpMultiDungeonModule:HandleEnterSceneNotice(protocol_data)
    if not self.__is_started then
        return
    end
    
    local obj = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_HERO, protocol_data.role_id)
    if obj then
        local pos_x, pos_y = GlobalScene:PixelToLogic(protocol_data.map_x, protocol_data.map_y)

        --更新已在场景的玩家
        obj:SetLogicPos(pos_x, pos_y)

        --更新携带的精灵位置
        local obj_vo = obj:GetVO()
        local sprite = GlobalScene:GetObj(obj_vo.sprite_obj_id)
        if sprite then
            local sprite_vo = sprite:GetVO()
            pos_x = pos_x + sprite_vo.offset_frm_hero.x / Scene.Map.TileUnitSize.width * obj:GetDir()
            pos_y = pos_y + sprite_vo.offset_frm_hero.y / Scene.Map.TileUnitSize.height
            sprite:SetLogicPos(pos_x, pos_y)
        end
    else
        --创建进入场景的玩家
        self:__CreateHeroAndSprite(protocol_data)
    end
end

--处理场景离开通知
function Modules.SpMultiDungeonModule:HandleLeaveSceneNotice(protocol_data)
    local obj_id = CallModuleFunc(ModuleType.SCENE, "ConvertSID2CID", Scene.ObjType.MT_HERO, protocol_data.role_id)
    GlobalScene:DeleteObj(obj_id)
end

--处理英雄行走通知
function Modules.SpMultiDungeonModule:HandleHeroGotoNotice(protocol_data)
    local obj = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_HERO, protocol_data.role_id)
    if obj then
        local end_pos = Vector2D.New(GlobalScene:PixelToWorld(protocol_data.dest_x, protocol_data.dest_y))
        obj:DoSpecialMove(end_pos, true)
    end
end

--处理怪物数据通知
function Modules.SpMultiDungeonModule:HandleMonsterDataNotice(monster_info)
    local obj = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_MONSTER, monster_info.uid)
    if not obj then
        return
    end

    --更新血量
    obj:SetHp(monster_info.total_hp - monster_info.damage)

    if monster_info.damage >= monster_info.total_hp then
        --死亡
        obj:DoDead()
    else
        --更新状态
        local obj_vo = obj:GetVO()
        obj_vo.start_battle_time = monster_info.start_battle_time
        obj:SetInBattle(obj_vo:IsInBattle())
    end
end

--处理怪物新增通知
function Modules.SpMultiDungeonModule:HandleMonsterAddNotice(monster_info)
    if not self.__is_started then
        return
    end

    if not self.__monster_notice_toggle then
        return
    end

    self:__CreateMonster(monster_info)
end

--处理采集物新增通知
function Modules.SpMultiDungeonModule:HandleItemAddNotice(item_info)
    if not self.__is_started then
        return
    end

    if not self.__item_notice_toggle then
        return
    end

    self:__CreateItem(item_info)
end

--处理采集物消失通知
function Modules.SpMultiDungeonModule:HandleItemDisappearNotice(item_info)
    local obj_id = CallModuleFunc(ModuleType.SCENE, "ConvertSID2CID", Scene.ObjType.MT_ITEM, item_info.uid)
    GlobalScene:DeleteObj(obj_id)
end

--处理采集物锁定响应
function Modules.SpMultiDungeonModule:HandleItemLockResp(protocol_data)
    if protocol_data.uid == 0 then
        GlobalTipMsg.GetInstance():Show(self:__GetItemLockFailedTips())
    else
        self:__PickupItem(protocol_data.uid)        
    end 
end

--处理采集物锁定通知
function Modules.SpMultiDungeonModule:HandleItemLockNotify(protocol_data)
    local item = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_ITEM, protocol_data.uid)
    if item then
        item:PlayPickupEffect()
    end
end

--处理采集物拾取响应
function Modules.SpMultiDungeonModule:HandleItemPickupResp(protocol_data)
    local item = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_ITEM, protocol_data.uid)
    if item then
        item:StopPickupEffect()

        --拾取成功飘字
        local px = item:GetPixelX()
        local py = item:GetPixelY()
        local lab_tips = Util:Label("+1", Macros.TypefaceSize.largish, Macros.Color.green, nil, nil, Macros.Color.green_stroke)
        lab_tips:SetAnchorPoint(0.5, 0.5)
        lab_tips:SetPosition(px, py)
        GlobalScene:AddToRenderGroup(lab_tips, Scene.RenderQueue.GRQ_AIR_TOP)
        ActionManager.GetInstance():RunFadeToAndMoveByEffect(lab_tips, 0.6, 0, 100, 0, function()
            lab_tips:RemoveSelf()
        end)

        --拾取成功特效
        local main_role = GlobalScene:GetMainRoleObj()
        if main_role then
            main_role:PlayPickupSuccessEffect()
        end
    end    
end

--处理采集物拾取通知
function Modules.SpMultiDungeonModule:HandleItemPickupNotify(protocol_data)
    local item = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_ITEM, protocol_data.uid)
    if item then
        item:StopPickupEffect()

        --拾取成功飘字
        local px = item:GetPixelX()
        local py = item:GetPixelY()
        local lab_tips = Util:Label("+1", Macros.TypefaceSize.largish, Macros.Color.green, nil, nil, Macros.Color.green_stroke)
        lab_tips:SetAnchorPoint(0.5, 0.5)
        lab_tips:SetPosition(px, py)
        GlobalScene:AddToRenderGroup(lab_tips, Scene.RenderQueue.GRQ_AIR_TOP)
        ActionManager.GetInstance():RunFadeToAndMoveByEffect(lab_tips, 0.6, 0, 100, 0, function()
            lab_tips:RemoveSelf()
        end)

        --拾取成功特效
        local obj = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_HERO, protocol_data.role_id)
        if obj then
            obj:PlayPickupSuccessEffect()
        end        
    end    
end

function Modules.SpMultiDungeonModule:IsPauseWhenDeActive()
    return false
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpMultiDungeonModule:__CreateHeroAndSprite(player_info)
    local hero_vo = Scene.MtHeroVO.New()
    hero_vo:ParseInfo(player_info)
    hero_vo.dir = MathTool.HORZ_RIGHT
    GlobalScene:CreateMtHero(hero_vo)

    if player_info.sprite_id > 0 then
        local bind_obj = GlobalScene:GetObj(hero_vo.obj_id)
        local sprite_vo = Scene.MtSpiritVO.New()
        sprite_vo:ParseInfo({info_id = player_info.sprite_id})
        sprite_vo.dir = MathTool.HORZ_RIGHT
        sprite_vo.org_pos_x = hero_vo.org_pos_x + sprite_vo.offset_frm_hero.x / Scene.Map.TileUnitSize.width * bind_obj:GetDir()
        sprite_vo.org_pos_y = hero_vo.org_pos_y + sprite_vo.offset_frm_hero.y / Scene.Map.TileUnitSize.height
        GlobalScene:CreateMtSpirit(sprite_vo, true, hero_vo.obj_id)

        --记录携带精灵对象ID
        hero_vo.sprite_obj_id = sprite_vo.obj_id
    end
end

function Modules.SpMultiDungeonModule:__CreateMonster(monster_info)
    local vo = Scene.MtMonsterVO.New()
    vo:ParseInfo(monster_info)
    vo.dir = MathTool.HORZ_RIGHT
    vo.touch_callback = function(obj_id)
        --点击怪物回调
        local obj = GlobalScene:GetObj(obj_id)
        if obj then
            local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
            if vo:IsInBattle() then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("scene_monster_in_battle_tips"))
            else                
                local main_role = GlobalScene:GetMainRoleObj()
                if main_role and main_role:CanDoSpecialMove() then
                    local dst_pos_x, dst_pos_y = obj:GetWorldPos()
                    local dst_pos = Vector2D.New(dst_pos_x + MathTool.GetRandom() * vo.collision_area.width, dst_pos_y + MathTool.GetRandom() * vo.collision_area.height)

                    --先移动到怪物位置，再攻击
                    main_role:DoSpecialMove(dst_pos, true, nil, nil, function(_, is_finished)
                        if is_finished then
                            if vo:IsInBattle() then
                                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("scene_monster_in_battle_tips"))
                            else
                                self:__AttackMonster(vo)
                            end
                        end
                    end)

                    --同步移动
                    self:__SynchornizeMove(dst_pos)                    
                end
            end
        end
    end
    local monster = GlobalScene:CreateMtMonster(vo, not monster_info.is_boss)
    monster:SetInBattle(vo:IsInBattle())
    return vo,monster
end

function Modules.SpMultiDungeonModule:__CreateItem(item_info)
    local vo = Scene.MtItemVO.New()
    vo:ParseInfo(item_info)
    vo.dir = MathTool.HORZ_RIGHT
    vo.touch_callback = function(obj_id)    
        local item = GlobalScene:GetObj(obj_id)
        if item then
            local main_role = GlobalScene:GetMainRoleObj()
            if main_role and main_role:CanDoSpecialMove() then
                local dst_pos_x, dst_pos_y = item:GetWorldPos()
                vo.pickup_item_pos = Vector2D.New(dst_pos_x + MathTool.GetRandom() * vo.collision_area.width, dst_pos_y + MathTool.GetRandom() * vo.collision_area.height)

                --移动到场景物品附近
                main_role:DoSpecialMove(vo.pickup_item_pos, true, nil, nil, function(_, is_finished)
                    if is_finished then
                        --锁定场景物品
                        local uid = CallModuleFunc(ModuleType.SCENE, "ConvertCID2SID", Scene.ObjType.MT_ITEM, obj_id)
                        CallModuleFunc(ModuleType.SCENE, "ItemLockReq", uid)
                    end
                end)

                --同步移动
                self:__SynchornizeMove(vo.pickup_item_pos)                                        
            end
        end
    end
    GlobalScene:CreateMtItem(vo)
end

--创建场景对象
function Modules.SpMultiDungeonModule:__CreateObjs()
    --自己&携带精灵
    local mainrole_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
    local sprite_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritData")
    local player_info = {}
    player_info.role_id = mainrole_info.role_id
    player_info.head_img_id = mainrole_info.head_icon
    player_info.role_name = mainrole_info.role_name
    player_info.sprite_id = sprite_info and sprite_info.cur_use_spirit or 0
    player_info.map_x, player_info.map_y = CallModuleFunc(ModuleType.SCENE, "GetMyPos")
    player_info.title_id, player_info.nickname_id = CallModuleFunc(ModuleType.TITLE, "GetTitleId")
    player_info.server_id = mainrole_info.server_id
    player_info.is_mainrole = true
    self:__CreateHeroAndSprite(player_info)

    --其他玩家&携带精灵
    local player_map = CallModuleFunc(ModuleType.SCENE, "GetPlayerMap")
    if player_map then
        for _, player_info in pairs(player_map) do
            if player_info.role_id ~= mainrole_info.role_id then
                self:__CreateHeroAndSprite(player_info)
            end
        end     
    end

    --怪物
    local monster_map = CallModuleFunc(ModuleType.SCENE, "GetMonsterList")
    if monster_map then
        for _, monster_info in pairs(monster_map) do
            self:__CreateMonster(monster_info)
        end
    end

    --采集物
    local item_map = CallModuleFunc(ModuleType.SCENE, "GetItemList")
    if item_map then
        for _, item_info in pairs(item_map) do
            self:__CreateItem(item_info)
        end
    end
end

--创建触碰区域
function Modules.SpMultiDungeonModule:__CreateTouchArea()
    local config_scene = GlobalVOMgr:GetSceneConfig()
    local touch_move_area = config_scene.touch_move_area
    if touch_move_area then
        GlobalScene:SetRenderGroupTouchArena(Scene.RenderQueue.GRQ_TERRAIN, 0, 0, Scene.MaxSize.width, Scene.MaxSize.height)
        GlobalScene:SetRenderGroupTouchFunc(Scene.RenderQueue.GRQ_TERRAIN, nil, nil, function(x, y)
            local main_role = GlobalScene:GetMainRoleObj()
            if main_role and main_role:CanDoSpecialMove() then
                --边界处理
                x = math.max(touch_move_area[1], x)
                x = math.min(x, touch_move_area[1] + touch_move_area[3] - 1)
                y = math.max(touch_move_area[2], y)
                y = math.min(y, touch_move_area[2] + touch_move_area[4] - 1)

                --移动
                local dst_pos = GlobalScene:PixelToWorldVec(Vector2D.New(x, y))
                main_role:DoSpecialMove(dst_pos, true)

                --同步移动
                self:__SynchornizeMove(dst_pos)
            end
        end)
    end
end

--获取采集物锁定失败提示
function Modules.SpMultiDungeonModule:__GetItemLockFailedTips()
    return CPPGameLib.GetString("scene_item_lock_failed_tips")
end

--执行采集逻辑
function Modules.SpMultiDungeonModule:__PickupItem(uid)
    local item = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_ITEM, uid)
    local main_role = GlobalScene:GetMainRoleObj()
    if not item or not main_role then
        return
    end

    local main_role_vo = main_role:GetVO()
    local item_vo = item:GetVO()

    local item_pos = item_vo.pickup_item_pos or item:GetWorldPosVec()
    main_role:DoSpecialMove(item_pos, true, nil, nil, function(_, is_finished)
        if is_finished then
            local obj = CallModuleFunc(ModuleType.SCENE, "GetObjBySID", Scene.ObjType.MT_ITEM, uid)
            if obj then
                obj:PlayPickupEffect(ConfigAdapter.Common.GetMonsterStormCastleOpenBoxTime())
            end

            --延时采集
            main_role_vo.in_gathering = true
            if main_role_vo.gather_timer then
                GlobalTimerQuest:CancelQuest(main_role_vo.gather_timer)
            end
            main_role_vo.gather_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
                main_role_vo.in_gathering = false
                main_role_vo.gather_timer = nil
                --请求采集
                CallModuleFunc(ModuleType.SCENE, "ItemPickupReq", uid)
            end), ConfigAdapter.Common.GetMonsterStormCastleOpenBoxTime())            
        end
    end)

    --同步移动
    self:__SynchornizeMove(item_pos)                        
end

--执行打怪处理
function Modules.SpMultiDungeonModule:__AttackMonster(vo)
end

--同步移动
function Modules.SpMultiDungeonModule:__SynchornizeMove(dst_pos)
    local config_scene = GlobalVOMgr:GetSceneConfig()
    if config_scene and config_scene.is_sync_move ~= Macros.Global.FALSE then
        local main_role = GlobalScene:GetMainRoleObj()
        if main_role then
            local src_pos = main_role:GetWorldPosVec()
            CallModuleFunc(ModuleType.SCENE, "HeroGotoReq", src_pos, dst_pos)
        end
    end
end
