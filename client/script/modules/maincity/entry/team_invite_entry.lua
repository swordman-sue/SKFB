--[[
%% @module: 组队邀请入口
%% @author: ldx
%% @created: 2018-12-22
--]]

Modules = Modules or {}

Modules.TeamInviteEntry = Modules.TeamInviteEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.TeamInviteEntry:__init()
		
end

function Modules.TeamInviteEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)
	
	--打开组队邀请信息
	self:BindGlobalEvent(Macros.Event.MaincityModule.OPEN_TEAM_INVITE_MESSAGE, function()
		self:__OpenTeamInviteMessage()
	end)
end

function Modules.TeamInviteEntry:__HandleEntryTouch()
	self:__OpenTeamInviteMessage()
end

--打开组队邀请信息
function Modules.TeamInviteEntry:__OpenTeamInviteMessage()
	--组队邀请
	self.__accept_invite_notify_list = CallModuleFunc(ModuleType.TEAM,"GetInvitedToJoinTheTeamList")
	if #self.__accept_invite_notify_list > 0 then
		--副本信息
    	local fb_config = CPPGameLib.GetConfig("TeamFBChapter",self.__accept_invite_notify_list[1].dungeon_id)
		--确认弹框
	    self.__templ_affirm_tips = LogicGUI.TemplAffirmTips.New({type = LogicGUI.TemplAffirmTipsType.YES_NO,content = CPPGameLib.GetString("team_invite_to_team_maincity",
	    							self.__accept_invite_notify_list[1].server_id or 0,self.__accept_invite_notify_list[1].role_name or "",fb_config.dungeon_name),
	                                    confirm_text = CPPGameLib.GetString("team_gamelevel_join_the_team"),
	                                    confirm_func = function ()
	                                    	--组队邀请
											self.__accept_invite_notify_list = CallModuleFunc(ModuleType.TEAM,"GetInvitedToJoinTheTeamList")
	                                    	if #self.__accept_invite_notify_list > 0 then
	                                            --接受组队邀请请求
												CallModuleFunc(ModuleType.TEAM,"TeamDungeonAcceptInviteReq",{team_id = self.__accept_invite_notify_list[1].team_id,dungeon_id = self.__accept_invite_notify_list[1].dungeon_id})
	                                    	end
	                                    end,cancel_text = CPPGameLib.GetString("team_gamelevel_reject_the_team"),
	                                    cancel_func = function ()
	                                    	--从邀请队列里删除当前的邀请信息
											CallModuleFunc(ModuleType.TEAM,"RemoveInvitedJoinTeamMessage",true)
	                                    end})
   	end
end