
--[[
%% @module: 精灵对象状态机(多人场景)
%% @author: swordman sue
%% @created: 2017-11-15
--]]

Scene = Scene or {}

Scene.MtSpirit = Scene.MtSpirit or BaseClass(Scene.Spirit)

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.MtSpirit:__QuitStateMove(state_name)
	Scene.BattleObj.__QuitStateMove(self, state_name)

    local bind_obj = GlobalScene:GetObj(self.__bind_obj_id)
    if bind_obj then
    	self:SetDir(bind_obj:GetDir())
    end
end
