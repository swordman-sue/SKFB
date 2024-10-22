
--[[
%% @module: 精灵对象(多人场景)
%% @author: swordman sue
%% @created: 2017-11-15
--]]

Scene = Scene or {}

Scene.MtSpirit = Scene.MtSpirit or BaseClass(Scene.Spirit)

function Scene.MtSpirit:__init()
    self.__obj_type = Scene.ObjType.MT_SPIRIT
end

--设置绑定的对象ID
function Scene.MtSpirit:SetBindObjID(id)
    self.__bind_obj_id = id
end

--获取绑定的对象ID
function Scene.MtSpirit:GetBindObjID()
    return self.__bind_obj_id
end
