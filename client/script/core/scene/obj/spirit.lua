
--[[
%% @module: 精灵对象
%% @author: swordman sue
%% @created: 2017-01-19
--]]

Scene = Scene or {}

Scene.Spirit = Scene.Spirit or BaseClass(Scene.BattleObj)

local mount_effect_offset = COCOPoint(-40, -20)

function Scene.Spirit:__init()
    self.__obj_type = Scene.ObjType.SPIRIT

    if not self.__vo.ignore_mount_effect then
        self.__body_anim_play_info.mount_effect_info = {slot_name = "guadian", effect_params = {res_name = "jingling_tuowei", loop = true, pos_x = mount_effect_offset.x, pos_y = mount_effect_offset.y}}
    end

	--加载VO
	self:__LoadVO()

    GlobalVOMgr:AddSpiritVO(self.__vo.obj_id, self.__vo)    
end

function Scene.Spirit:__delete()
	GlobalVOMgr:DeleteSpiritVO(self.__vo.obj_id)
end

function Scene.Spirit:DoAngerChange(change_anger)  
end

function Scene.Spirit:SetDir(var)
    Scene.BattleObj.SetDir(self, var)

    local mount_effect_id = self:__GetMountEffectID()
    local mount_effect = GlobalEffectMgr:GetEffect(mount_effect_id)
    if mount_effect then
        mount_effect:SetPosition(self.__dir * mount_effect_offset.x, mount_effect_offset.y)
    end
end

--设置绑定的对象类型
function Scene.Spirit:SetBindObjType(type)
    self.__bind_obj_type = type
end

--获取绑定的对象类型
function Scene.Spirit:GetBindObjType()
    return self.__bind_obj_type
end

--绑定的对象类型为英雄
function Scene.Spirit:CheckBindObjType(...)
    local arg_list = {...}
    for _, arg in ipairs(arg_list) do
        if self.__bind_obj_type == arg then
            return true
        end
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--添加脚底烟尘特效
function Scene.Spirit:__AddFogEffect(effect_config_id, flip_dir)
end

--移除脚底烟尘特效
function Scene.Spirit:__DelFogEffect(fog_follow_id)
end

function Scene.Spirit:__GetMountEffectID()
    local child_layer = self.__model:GetChildLayerByTag(self.__body_anim_play_info.inner_layer)
    if child_layer then
        return child_layer:GetMountEffectID()
    end
end

function Scene.Spirit:__ShowMountEffect(var)
    local mount_effect_id = self:__GetMountEffectID()
    GlobalEffectMgr:SetEffectVisible(mount_effect_id, var)
end