--
-- @brief 聊天信息小界面
-- @author: yjg
-- @date: 2017年5月20日10:41:30
--

Modules = Modules or {}

Modules.MainCityChatLayer = Modules.MainCityChatLayer or BaseClass(UILayer)

function Modules.MainCityChatLayer:__init(node)

    --信息显示
    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetContentSize(node:GetContentWidth() - 5 , node:GetContentHeight() - 8 )
    self.lst_list:SetAnchorPoint(0.5, 0.5)
    self:AddChild(self.lst_list)
    PosTool.Center( self.lst_list )

    --聊天
    self._Recruit_Update_Free_event = self:BindGlobalEvent(Macros.Event.ChatModule.CHAT_MAIN, function()
        self:Cell()
    end)

    self:Cell()
end

function Modules.MainCityChatLayer:__delete()

end

function Modules.MainCityChatLayer:Cell(  )
    local messageLst = CallModuleFunc(ModuleType.CHAT, "GetMainChat")
    local layer = Util:Layout( self.lst_list:GetContentWidth() , self.lst_list:GetContentHeight() )

    local info = messageLst[ #messageLst ]
    if not info then return end
    --频道
    local channel = ""
    --私聊
    if info.chat_type == Macros.Game.CHAT.PRIVATE then
        channel = string.format( Macros.Color.pm_rt , CPPGameLib.GetString("pingdao_show_1") )
    --世界
    elseif info.chat_type == Macros.Game.CHAT.WORLD then
        channel = string.format( Macros.Color.world_rt , CPPGameLib.GetString("pingdao_show_2") )
    --公会
    elseif info.chat_type == Macros.Game.CHAT.UNION then
        channel = string.format( Macros.Color.union_rt , CPPGameLib.GetString("pingdao_show_3") )
    --队伍
    elseif info.chat_type == Macros.Game.CHAT.TEAM then
        channel = string.format( Macros.Color.union_rt , CPPGameLib.GetString("pingdao_show_4") )
    end

    --名字
    local name = string.format( Macros.Color.green_rt , info.role_name .. CPPGameLib.GetString("common_colon") )

    local viewInfo = GlobalViewMgr:GetCurViewInfo()
    if (viewInfo and viewInfo.view_type == "TEAM_BOSS_BATTLE") or (viewInfo and viewInfo.view_type == "TEAM") then
        if info.server_id then
            name =  string.format( Macros.Color.green_rt , "S"..info.server_id .."." ) ..  name 
        end
    end

    --内容
    local text = ""
    if info.chat_text then
        local chat_text = CallModuleFunc(ModuleType.CHAT, "GetChatTextWithSmile", info )
        text = string.format(Macros.Color.white_rt , chat_text or " " )
    end
    local t = channel..name..text

    local tab = Util:RichText( t , Macros.TypefaceSize.slightly , self.lst_list:GetContentWidth() - 10 , -1 )
    layer:SetContentSize( self.lst_list:GetContentWidth() , tab:GetContentHeight() )
    tab:SetAnchorPoint(0.5,0.5)
    layer:AddChild( tab )
    PosTool.Center( tab )

    self.lst_list:PushBackCustomItem( layer )
    self.lst_list:ScrollToBottom()
end
