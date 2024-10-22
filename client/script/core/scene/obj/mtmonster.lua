
--[[
%% @module: 怪物对象(多人场景)
%% @author: swordman sue
%% @created: 2017-09-26
--]]

Scene = Scene or {}

Scene.MtMonster = Scene.MtMonster or BaseClass(Scene.Obj)

function Scene.MtMonster:__init()    
    --身体动画信息
    self.__body_anim_play_info = {}
    self.__body_anim_play_info.anim_type = self.__vo.anim_type
    self.__body_anim_play_info.inner_layer = RenderObj.InnerLayer.BODY
    self.__body_anim_play_info.path = Resource.PathTool.GetModelPath(self.__vo.model_type, self.__vo.model_id)

    self.__obj_type = Scene.ObjType.MT_MONSTER

    --加载VO
    self:__LoadVO()

    if self.__vo.touch_callback then
        self.__model:SetTouchSwallow(true)
        self.__model:SetTouchRect(self.__vo.touch_rect[1], self.__vo.touch_rect[2], self.__vo.touch_rect[3], self.__vo.touch_rect[4])
        self.__model:SetTouchFun(nil, nil, function()
            self.__vo.touch_callback(self.__vo.obj_id)
        end, 500)
    end

    GlobalVOMgr:AddMonsterVO(self.__vo.obj_id, self.__vo)
end

function Scene.MtMonster:__delete()
    GlobalVOMgr:DeleteMonsterVO(self.__vo.obj_id)

    if self.__in_battle_effect_id then
        GlobalEffectMgr:DelEffect(self.__in_battle_effect_id)
        self.__in_battle_effect_id = nil
    end

    self.__body_anim_play_info = nil
    GlobalEffectMgr:DelEffectsByTargetID(self.__vo.obj_id)        
end

function Scene.MtMonster:PlayAnimation(act_index, loop, time_scale)
    if time_scale == nil then
        time_scale = self:TopAnimTimeScale()
    end

    --身体
    self:__SetupAnimPlayInfo(self.__body_anim_play_info, act_index, loop, time_scale)
    self.__model:PlayAnimation(self.__body_anim_play_info)

    --其他
    --....
end

function Scene.MtMonster:SetInBattle(is_in_battle)
    if self.__vo.is_boss then
        return
    end

    if is_in_battle then
        --切换到待机
        self:DoIdle(true)

        --战斗中特效
        if not self.__in_battle_effect_id then
            local x, y = GlobalScene:WorldToPixel(self:GetWorldPos())
            self.__in_battle_effect_id = GlobalEffectMgr:CreateScenePosEffect(
                {res_name = "UI_gongcheng_zhandouzhong", loop = true, pos_x = x, pos_y = y + 180}, Scene.RenderQueue.GRQ_AIR)        
        end
    else
        GlobalEffectMgr:DelEffect(self.__in_battle_effect_id)
        self.__in_battle_effect_id = nil
    end
end

function Scene.MtMonster:SetDir(var)
    if Scene.Obj.SetDir(self, var) then
        self.__model:SetDir(self.__dir, self.__body_anim_play_info.inner_layer)        
    end
end

--设置血量
function Scene.MtMonster:SetHp(hp)
    Scene.Obj.SetHp(self, hp)

    self:SetHpBarVisible(self.__vo.hp < self.__vo.max_hp)    
end

function Scene.MtMonster:SetHpBarVisible(var)
    if self.__vo.is_boss then
        return
    end

    Scene.Obj.SetHpBarVisible(self, var)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Scene.MtMonster:__LoadVO()
    Scene.Obj.__LoadVO(self)

    self:SetHpBarVisible(self.__vo.hp < self.__vo.max_hp)
end

