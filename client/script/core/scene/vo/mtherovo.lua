
--[[
%% @module: 英雄对象vo(多人场景)
%% @author: swordman sue
%% @created: 2017-09-26
--]]

Scene = Scene or {}

Scene.MtHeroVO = Scene.MtHeroVO or BaseClass(Scene.ObjVO)

function Scene.MtHeroVO:__init()
    self.anim_type = RenderObj.AnimType.SKELETON

    --英雄ID
    self.info_id = 0    

    --是否显示影子
    self.show_shadow = true

    self.role_id = 0
end

function Scene.MtHeroVO:__delete()
end

function Scene.MtHeroVO:ParseInfo(player_info)
    if not player_info then
        return
    end

    self.role_id = player_info.role_id
    self.info_id = player_info.head_img_id
    self.title_id = player_info.title_id
    self.nickname_id = player_info.nickname_id
    self.org_pos_x, self.org_pos_y = GlobalScene:PixelToLogic(player_info.map_x, player_info.map_y)
    self.is_mainrole = player_info.is_mainrole

    --名字信息
    self.name_info = {}
    self.name_info.name = string.format("s%d.%s", player_info.server_id, player_info.role_name)
    self.name_info.size = Macros.TypefaceSize.slightly
    self.name_info.color = self.is_mainrole and Macros.Color.monster_stormcastle or Macros.Color.btn
    self.name_info.stroke = self.is_mainrole and Macros.Color.monster_stormcastle_stroke or Macros.Color.common_stroke
    
    local config_hero = CPPGameLib.GetConfig("Hero", self.info_id)
    if not config_hero then
        return
    end
    local config_scene = GlobalVOMgr:GetSceneConfig()
    if not config_scene then
        return
    end

    self.model_type = config_hero.model_type
    self.model_id = config_hero.model_id
    self.model_scale = (config_hero.battle_scale or 1) * (config_scene.model_scale or 1)
    self.model_scale_inver = 1 / self.model_scale
    self.speed = ConfigAdapter.Common.GetMonsterStormCastlePlayerSpeed() or 0

    --名字信息
    self.name_info.scale = self.model_scale_inver * ConfigAdapter.Common.GetMonsterStormCastleHeroZoom()
end

