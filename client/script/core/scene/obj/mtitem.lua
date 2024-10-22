
--[[
%% @module: 场景物品对象vo(多人场景)
%% @author: swordman sue
%% @created: 2017-09-28
--]]

Scene = Scene or {}

Scene.MtItem = Scene.MtItem or BaseClass(Scene.Obj)

function Scene.MtItem:__init()    
    self.__obj_type = Scene.ObjType.MT_ITEM

    --加载VO
    self:__LoadVO()

    if self.__vo.touch_callback then
        self.__model:SetTouchSwallow(true)
        self.__model:SetTouchRect(self.__vo.touch_rect[1], self.__vo.touch_rect[2], self.__vo.touch_rect[3], self.__vo.touch_rect[4])
        self.__model:SetTouchFun(nil, nil, function()
            self.__vo.touch_callback(self.__vo.obj_id)
        end)
    end

    GlobalVOMgr:AddDropBoxVO(self.__vo.obj_id, self.__vo)
end

function Scene.MtItem:__delete()
    self:StopPickupEffect()

    GlobalVOMgr:DeleteDropBoxVO(self.__vo.obj_id)

    GlobalEffectMgr:DelEffectsByTargetID(self.__vo.obj_id)        
end

function Scene.MtItem:PlayAnimation(act_index, loop, time_scale)
    if not self.__effect_id then
        local effect_node
        self.__effect_id, effect_node = GlobalEffectMgr:CreateSceneTargetEffect(
            {res_name = self.__vo.effect, loop = true, type = Effect.EffectType.SKELETON}, self.__vo.obj_id, RenderObj.InnerLayer.BODY)        
    end
end

--播放采集中特效
function Scene.MtItem:PlayPickupEffect(prog_time)    
    local x, y = GlobalScene:WorldToPixel(self:GetWorldPos())

    if not self.__gather_effect_id then
        self.__gather_effect_id = GlobalEffectMgr:CreateScenePosEffect(
            {id = self.__vo.gather_effect, loop = true, pos_x = x, pos_y = y}, Scene.RenderQueue.GRQ_AIR)        
    end

    if prog_time and not self.__pickup_bar then
        local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bar_green2")
        local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bg")
        self.__pickup_bar = GUI.ProgressBarLayer.New(imgdata_bar, imgdata_bg, 0, 1000)
        self.__pickup_bar:SetSpeed(1000 / prog_time)
        self.__pickup_bar:SetToProgress(1000)
        self.__pickup_bar:SetPosition(x, y - 20)
        GlobalScene:AddToRenderGroup(self.__pickup_bar, Scene.RenderQueue.GRQ_AIR_TOP)
    end 
end

--停止采集中特效
function Scene.MtItem:StopPickupEffect()
    if self.__gather_effect_id then
        GlobalEffectMgr:DelEffect(self.__gather_effect_id)
        self.__gather_effect_id = nil
    end

    if self.__pickup_bar then
        self.__pickup_bar:DeleteMe()
        self.__pickup_bar = nil
    end
end

