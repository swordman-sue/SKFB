
--[[
%% @module: 弹道对象
%% @author: swordman sue
%% @created: 2016-07-21
--]]

Scene = Scene or {}

Scene.Bullet = Scene.Bullet or BaseClass(Scene.BattleObj)

function Scene.Bullet:__init()
    self.__obj_type = Scene.ObjType.BULLET

    --已攻击对象ID表
    self.__attacked_obj_ids = {}    

	--加载VO
	self:__LoadVO()

    GlobalVOMgr:AddBulletVO(self.__vo.obj_id, self.__vo)    
end

function Scene.Bullet:__delete()
    self.__attacked_obj_ids = nil
	GlobalVOMgr:DeleteBulletVO(self.__vo.obj_id)
end

function Scene.Bullet:SetDir(var)
    Scene.Obj.SetDir(self, var)
end

function Scene.Bullet:PlayAnimation()
end

function Scene.Bullet:PauseAnimation()
    GlobalEffectMgr:PaueEffectsByTargetID(self.__vo.obj_id)
end

function Scene.Bullet:ResumeAnimation()
    GlobalEffectMgr:ResumeEffectsByTargetID(self.__vo.obj_id)    
end

function Scene.Bullet:GetCreateObjID()
    return self.__vo.create_obj_id
end

function Scene.Bullet:GetCreateObj()
    return GlobalScene:GetObj(self.__vo.create_obj_id)
end

