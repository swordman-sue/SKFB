
--[[
%% @module: 英雄对象(多人场景)
%% @author: swordman sue
%% @created: 2017-09-26
--]]

Scene = Scene or {}

Scene.MtHero = Scene.MtHero or BaseClass(Scene.Obj)

function Scene.MtHero:__init()    
    --身体动画信息
    self.__body_anim_play_info = {}
    self.__body_anim_play_info.anim_type = self.__vo.anim_type
    self.__body_anim_play_info.inner_layer = RenderObj.InnerLayer.BODY
    self.__body_anim_play_info.path = Resource.PathTool.GetModelPath(self.__vo.model_type, self.__vo.model_id)

    self.__obj_type = Scene.ObjType.MT_HERO

    --加载VO
    self:__LoadVO()

    GlobalVOMgr:AddHeroVO(self.__vo.obj_id, self.__vo)
end

function Scene.MtHero:__delete()
    --移除携带精灵
    if self.__vo.sprite_obj_id then
        GlobalScene:DeleteObj(self.__vo.sprite_obj_id)
        self.__vo.sprite_obj_id = nil
    end

    GlobalVOMgr:DeleteHeroVO(self.__vo.obj_id)

    self.__body_anim_play_info = nil
    GlobalEffectMgr:DelEffectsByTargetID(self.__vo.obj_id)        
end

function Scene.MtHero:PlayAnimation(act_index, loop, time_scale)
    if time_scale == nil then
        time_scale = self:TopAnimTimeScale()
    end

    --身体
    self:__SetupAnimPlayInfo(self.__body_anim_play_info, act_index, loop, time_scale)
    self.__model:PlayAnimation(self.__body_anim_play_info)

    --其他
    --....
end

function Scene.MtHero:SetDir(var)
    if Scene.Obj.SetDir(self, var) then
        self.__model:SetDir(self.__dir, self.__body_anim_play_info.inner_layer)        
    end
end

function Scene.MtHero:CanDoSpecialMove()
    if self.__vo.in_gathering then
        return
    end
    return Scene.Obj.CanDoSpecialMove(self)
end

--播放拾取成功特效
function Scene.MtHero:PlayPickupSuccessEffect()    
    GlobalEffectMgr:CreateSceneTargetEffect({id = 210000, scale = self.__vo.model_scale_inver}, self.__vo.obj_id, RenderObj.InnerLayer.BODY_EFFECT_UP)
end

