--[[
%% @module: 组队入口
%% @author: ldx
%% @created: 2018-12-22
--]]

Modules = Modules or {}

Modules.TeamEntry = Modules.TeamEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.TeamEntry:__init()
	self:__UpdateLimmitActivitySpHandle()
end

function Modules.TeamEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)
	
	--重排图标位置
	self:BindGlobalEvent(Macros.Event.MaincityModule.RESET_REGION_ENTRYS, function(_, system_id)
		if system_id == self.__system_id then
			self:__UpdateLimmitActivitySpHandle()
		end
	end)
end

function Modules.TeamEntry:__HandleEntryTouch()
	if GlobalModulesMgr:IsOpen(self.__system_id, true) then
		CallModuleFunc(ModuleType.SCENE, "EnterSceneReq", Macros.Game.SceneType.TEAM_HALL, 1)	
	end
end

--更新限时活动图标时的特殊处理
function Modules.TeamEntry:__UpdateLimmitActivitySpHandle()
	local is_open,is_over = CallModuleFunc(ModuleType.LIMIT_DUNGEON, "IsLimitDungeonOpen", self.__system_id)
	--限时组队
	local is_exist = self:IsEffectExist("UI_zhandoujineng")
	if not is_exist and is_open then
		--环绕特效
		local effect
		self.__entry_effect_map["UI_zhandoujineng"],effect = self:__CreateEffect({res_name = "UI_zhandoujineng",scale = 1.1},self.__entry_btn)
		PosTool.Center(effect,0,-6)
	elseif is_exist and is_over then
		self:DeleteEffectById("UI_zhandoujineng")
	end
end