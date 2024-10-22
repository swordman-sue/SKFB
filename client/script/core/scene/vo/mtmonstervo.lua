
--[[
%% @module: 怪物对象vo(多人场景)
%% @author: swordman sue
%% @created: 2017-09-27
--]]

Scene = Scene or {}

Scene.MtMonsterVO = Scene.MtMonsterVO or BaseClass(Scene.ObjVO)

function Scene.MtMonsterVO:__init()
    self.anim_type = RenderObj.AnimType.SKELETON

    self.uid = 0

    --怪物ID
    self.info_id = 0

    --是否显示影子
    self.show_shadow = true

    --名字信息
    self.name_info = nil

    --是否显示血条
    self.show_hp_bar = true
    self.hp_bar_image_info = {}
    self.hp_bar_image_info.atlas_type = Resource.UITextureAtlasType.COMMON_PROGRESS
    self.hp_bar_image_info.bg_image_name = "prog_bar_bg"
    self.hp_bar_image_info.bar_image_name = "prog_bar_bar_red"

    --类型
    self.ai_type = AI.Type.MT_MONSTER        

    --碰撞范围
    self.collision_area = COCOSize(32, 32)
end

function Scene.MtMonsterVO:__delete()
end

function Scene.MtMonsterVO:ParseInfo(monster_info)
    if not monster_info then
        return
    end

    self.uid = monster_info.uid
    self.map_id = monster_info.map_id
    self.hp = monster_info.total_hp - monster_info.damage
    self.max_hp = monster_info.total_hp
    self.org_pos_x, self.org_pos_y = GlobalScene:PixelToLogic(monster_info.map_x, monster_info.map_y)
    self.is_boss = monster_info.is_boss
    self.start_battle_time = monster_info.start_battle_time
    self.touch_rect = self.is_boss and {-80, 0, 160, 312} or {-50, 0, 100, 160}

    local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.map_id, false)
    if not config_gamelevel then
        return
    end

    self.info_id = config_gamelevel.icon_id

    --名字信息
    self.name = config_gamelevel.name
    if not self.is_boss then
        self.name_info = {}
        self.name_info.name = self.name
        self.name_info.size = Macros.TypefaceSize.slightly
        self.name_info.color = Macros.Color.orange
        self.name_info.stroke = Macros.Color.orange_stroke
    end
    
    local config_monster = CPPGameLib.GetConfig("Monster", self.info_id, false)
    if not config_monster then
        return
    end
    local config_scene = GlobalVOMgr:GetSceneConfig()
    if not config_scene then
        return
    end

    self.model_type = config_monster.model_type
    self.model_id = config_monster.model_id
    self.model_scale = (config_monster.battle_scale or 1) * (config_scene.model_scale or 1)    
    self.model_scale_inver = 1 / self.model_scale
    self.speed = config_monster.init_move_speed or 0

    --名字信息
    if self.name_info then
        self.name_info.scale = self.model_scale_inver * ConfigAdapter.Common.GetMonsterStormCastleHeroZoom()
    end
end

--是否战斗中
function Scene.MtMonsterVO:IsInBattle()
    return CallModuleFunc(ModuleType.SCENE, "JudgeIsInBattle", self.start_battle_time, self.is_boss)
end

