
--[[
%% @module: 掉落宝箱(怪物死亡后几率掉落)
%% @author: swordman sue
%% @created: 2016-08-17
--]]

Scene = Scene or {}

Scene.DropBox = Scene.DropBox or BaseClass(Scene.Obj)

function Scene.DropBox:__init()	
	self.__obj_type = Scene.ObjType.DROPBOX

	--加载VO
	self:__LoadVO()

    GlobalVOMgr:AddDropBoxVO(self.__vo.obj_id, self.__vo)    
end

function Scene.DropBox:__delete()
	GlobalVOMgr:DeleteDropBoxVO(self.__vo.obj_id)	
end

