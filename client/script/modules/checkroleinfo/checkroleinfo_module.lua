--
-- @brief 查看玩家信息 
-- @author: yjg
-- @date: 2016年8月8日15:53:48
--

Modules = Modules or {}

Modules.CheckRoleInfoModule = Modules.CheckRoleInfoModule or BaseClass(BaseModule)

function Modules.CheckRoleInfoModule:__init()
	--默认聊天界面不是打开
 	self.chat_type = false
 	self.info = {}

end

function Modules.CheckRoleInfoModule:__delete()

end

-- 角色详细请求
function Modules.CheckRoleInfoModule:RoleDetailsReq( type,id )
    -- CPPGameLib.PrintTable(id,"角色详细请求")
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ROLE_DETAILS_REQ)
    data.type = type
    data.role_id = id
    GlobalNetAdapter:OnSend(data)
end

-- 角色详细响应
function Modules.CheckRoleInfoModule:RoleDetailsResp( info )
	CPPGameLib.PrintTable(info,"角色详细响应")
	self.info = info

    if self.info.type == Macros.Game.HeroMessageReq_Type.FRIEND then
    elseif self.info.type == Macros.Game.HeroMessageReq_Type.RANK then
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PLAYER_DETAILMESSAGE, false, info)
    elseif self.info.type == Macros.Game.HeroMessageReq_Type.CHAT then
        GlobalEventSystem:FireNextFrame(Macros.Event.ChatModule.CHAT_CHECK,info)
    else
        
    end     
end

--获得角色
function  Modules.CheckRoleInfoModule:GetPalyInfo( ... )
	return self.info
end

--删除好友
function Modules.CheckRoleInfoModule:RemoveFriend(info)
    CPPGameLib.PrintTable(info,"删除好友")
    CallModuleFunc(ModuleType.FRIEND, "FriendDelRequest",info) 
end

--添加好友
function Modules.CheckRoleInfoModule:AddFriend(info)
    CPPGameLib.PrintTable(info,"添加好友")
    local data = {}
    data.target_id = info
    CallModuleFunc(ModuleType.FRIEND, "FriendAddRequest",data) 
end

--拉黑玩家
function Modules.CheckRoleInfoModule:AddShield(info)
    CPPGameLib.PrintTable(info,"拉黑玩家")
    CallModuleFunc(ModuleType.FRIEND, "BlackAddRequest",info) 
end

--取消拉黑
function Modules.CheckRoleInfoModule:RemoveShield(info)
    CPPGameLib.PrintTable(info,"取消拉黑")
    CallModuleFunc(ModuleType.FRIEND, "BlackDelRequest",info)
end
--私聊玩家
function Modules.CheckRoleInfoModule:privately()

end

--查看阵容
function Modules.CheckRoleInfoModule:check()

end

--切磋
function Modules.CheckRoleInfoModule:hit()

end
