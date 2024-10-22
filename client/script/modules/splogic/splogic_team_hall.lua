
--[[
%% @module: 组队大厅
%% @author: swordman sue
%% @created: 2017-09-25
--]]

Modules = Modules or {}

Modules.SpTeamHallModule = Modules.SpTeamHallModule or BaseClass(Modules.SpMultiDungeonModule)

function Modules.SpTeamHallModule:__init()
end

function Modules.SpTeamHallModule:__delete()
end

--初始化
function Modules.SpTeamHallModule:Initialize()
	Modules.SpMultiDungeonModule.Initialize(self)
end

--释放
function Modules.SpTeamHallModule:Dispose()
	Modules.SpMultiDungeonModule.Dispose(self)
end

function Modules.SpTeamHallModule:Update(now_time, elapse_time)
	Modules.SpMultiDungeonModule.Update(self, now_time, elapse_time)
end

--进入GamePlaying状态
function Modules.SpTeamHallModule:GamePlayingStartHandle()   
    Modules.SpMultiDungeonModule.GamePlayingStartHandle(self)

    --打开主界面
	CallModuleFunc(ModuleType.TEAM, "OpenView", BaseViewType.TEAM)

	local info = CallModuleFunc(ModuleType.TEAM, "GetBossRewardData")
	if not info then return end
	
	local t1 = {}
    for i,v in ipairs(info.lst) do
    	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
		if role_id == v.role_id then
			--通常奖励
			for ii , vv in ipairs( v.reward_list ) do
				table.insert( t1 , { id = vv.Item_Id , num = vv.Item_Num } )
			end
			--额外奖励——mvp
			if v.extra.mvp then
				local data = v.extra.mvp[1]
				table.insert( t1 , { id = data.item_id , num = data.item_num } )
			end
			--额外奖励——击杀
			if v.extra.kill then
				local data = v.extra.kill[1]
				table.insert( t1 , { id = data.item_id , num = data.item_num } )
			end
		end
    end


    if #t1 ~= 0 then
	    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	end
    CallModuleFunc(ModuleType.TEAM, "GetBossRewardData" , true )
end

function Modules.SpTeamHallModule:GamePlayingStopHandle()
	CallModuleFunc(ModuleType.TEAM, "CloseView")
	
	Modules.SpMultiDungeonModule.GamePlayingStopHandle(self)
end
