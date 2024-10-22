
--[[
%% @module: 助战英雄对象
%% @author: swordman sue
%% @created: 2017-05-11
--]]

Scene = Scene or {}

Scene.HeroAssist = Scene.HeroAssist or BaseClass(Scene.BattleObj)

function Scene.HeroAssist:__init()
	self.__obj_type = Scene.ObjType.HERO_ASSIST

	--加载VO
	self:__LoadVO()
end

function Scene.HeroAssist:__delete()
end

--怒气是否充足
function Scene.HeroAssist:IsAngerEnough(is_plus)
    return true
end

--设置绑定的对象类型
function Scene.HeroAssist:SetBindObjType(type)
    self.__bind_obj_type = type
end

--获取绑定的对象类型
function Scene.HeroAssist:GetBindObjType()
    return self.__bind_obj_type
end

--绑定的对象类型为英雄
function Scene.HeroAssist:CheckBindObjType(...)
    local arg_list = {...}
    for _, arg in ipairs(arg_list) do
        if self.__bind_obj_type == arg then
            return true
        end
    end
end