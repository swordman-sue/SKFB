
--[[
%% @module: 掉落宝箱状态机
%% @author: swordman sue
%% @created: 2016-08-17
--]]

function Scene.DropBox:__EnterStateIdle(state_name)
    self.__s_idle_state = self.__s_idle_state or {}	
    self.__s_idle_state.pass_time = 0
    self.__s_idle_state.last_time = 0.5

    if self.__vo.drop_info.is_world_star then
      --世界之心
      local _, effect_root = GlobalEffectMgr:CreateSceneTargetEffect(
          {res_name = self.__vo.drop_info.img_name, dir = self:GetDir(), loop = true, scale = 0.2}, self.__vo.obj_id, RenderObj.InnerLayer.BODY)      

      local offset = self.__vo.world_star_offset
      ActionManager.GetInstance():BezierBy(effect_root, 0.5, COCOPoint(offset.x, offset.y), COCOPoint(0, 0), COCOPoint(offset.x / 2, -offset.y / 2))
    else
      --普通掉落
      local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, self.__vo.drop_info.img_name)
      local img_box = SpriteEx.CreateWithImageFile(imgdata)
      self.__model:AddChild(img_box)
    end
end

function Scene.DropBox:__UpdateStateIdle(state_name, elapse_time)
    if self.__vo.drop_info.is_world_star then
      return
    end

	  self.__s_idle_state.pass_time = self.__s_idle_state.pass_time + elapse_time
	  if self.__s_idle_state.pass_time >= self.__s_idle_state.last_time then
		  GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.DROPBOX_DEAD, self.__vo.obj_id, self.__vo.drop_info)

	    GlobalScene:DeleteObj(self.__vo.obj_id)
	  end
end

