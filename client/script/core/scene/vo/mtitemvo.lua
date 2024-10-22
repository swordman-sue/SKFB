
--[[
%% @module: 场景物品对象vo(多人场景)
%% @author: swordman sue
%% @created: 2017-09-28
--]]

Scene = Scene or {}

Scene.MtItemVO = Scene.MtItemVO or BaseClass(Scene.ObjVO)

function Scene.MtItemVO:__init()
    self.uid = 0

    --采集物ID
    self.info_id = 0    

    --是否显示影子
    self.show_shadow = true

    --碰撞范围
    self.collision_area = COCOSize(32, 32)
end

function Scene.MtItemVO:__delete()
end

function Scene.MtItemVO:ParseInfo(item_info)
    if not item_info then
        return
    end

    self.uid = item_info.uid
    self.info_id = item_info.info_id
    self.org_pos_x, self.org_pos_y = GlobalScene:PixelToLogic(item_info.map_x, item_info.map_y)

    local config_gather = CPPGameLib.GetConfig("SceneGather", self.info_id)
    if not config_gather then
        return
    end

    self.name = config_gather.name
    self.effect = config_gather.effect
    self.gather_effect = config_gather.gather_effect
    self.touch_rect = config_gather.touch_rect or {-25, 0, 64, 64}
end

