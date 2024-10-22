
--[[
%% @module: 组队界面
%% @author: ldx
%% @created: 2017-09-01
--]]

Modules = Modules or {}

Modules.TeamView = Modules.TeamView or BaseClass(BaseView)

local STATUS_TYPE = {
    USUALING = 1,
    MATCHING = 2,
    GROUPING = 3,
    ENTERING_MAP = 4,
}

function Modules.TeamView:__init()
    self.__layout_name = "team" 
    
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("team_system_title"), 
        show_diamond = true,
        show_power = true,
        show_stamina = true,
        show_gold = true,
        is_matching = true,
        close_func = function()
            --正在进入世界地图
            if self.__is_moving_in_world_map then
                return
            end
            if self.__cur_status == STATUS_TYPE.MATCHING then
                if self.__is_in_team_match then
                    --提示
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_matching_tips"))
                else
                    --确认弹框
                    LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("team_leave_the_hall_tips"),
                                                    confirm_text = CPPGameLib.GetString("common_confirm"),
                                                    confirm_func = function ()
                                                        --取消匹配
                                                        CallModuleFunc(ModuleType.TEAM,"TeamDungeonCancelMatchReq",true)
                                                    end,cancel_text = CPPGameLib.GetString("common_cancel"),
                                                    cancel_func = function ()
                                                    end,})
                end
            else
                CallModuleFunc(ModuleType.SCENE,"LeaveReturnCity")
            end
        end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    -- self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_DAILY_DIFFICULTLEVEL)
    -- self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_DAILY_ITEM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM,true)
end

function Modules.TeamView:__OpenCallback()    
    CallModuleFunc(ModuleType.TEAM,"TeamDungeonDataReq")
    --self:__ShowUI()
end

function Modules.TeamView:__RegistAllEvents()
    --组队副本数据响应
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_DATA_RESP, function(_,team_dungeon_data)
        self.__team_dungeon_data = team_dungeon_data
        self:__ShowUI()
    end)

    --阵容红点
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
        if system_id == Macros.Game.SystemID.HERO_LINEUP then
            self:__UpdateLineupBtnRedDot()
        end
    end)

    --加入队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.INVITED_TO_JOIN_TEAM_NOTICE, function(_,protocol_data)
        --获取邀请加入队伍信息队列
        self.__accept_invite_notify_list = self:CallModuleFunc("GetInvitedToJoinTheTeamList")
        self:AfterTeamDungeonCreateTeam(protocol_data)
    end)

    --组队界面显隐
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_CREATE_TEAM_VIEW_VISIBLE, function(_,is_visible)
        if self.__teammate_view then
            self.__teammate_view:SetVisible(is_visible)
        end
    end)

    --拒绝加入队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_REJECT_JOIN_TEAM, function()
        --获取邀请加入队伍信息队列
        self.__accept_invite_notify_list = self:CallModuleFunc("GetInvitedToJoinTheTeamList")
        self:__HandleInviteJoinTeamMessage()
    end)

    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function(_,is_match,is_timeout)
        if self.__teammate_view then
            self.__teammate_view = nil
        end
        if is_match then
            --隐藏扭蛋
            self.__panel_gashaponmachine:SetVisible(false)
            self.__title_bar:SetVisible(true)
            self.__panel_chat:SetVisible(true)
        end
        if not is_match then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString(is_timeout and "team_over_activity_tips" or "team_break_the_team_tips"))
            --状态
            self.__cur_status = STATUS_TYPE.USUALING
        end
    end)

    --离开队伍通知
    self:BindGlobalEvent(Macros.Event.TeamModule.LEAVE_THE_TEAM, function(_,protocol_data)
        local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
        if protocol_data and protocol_data.role_id == role_info.role_id and protocol_data.is_task_team == 0 then
            if self.__teammate_view then
                self.__teammate_view = nil
            end
            --状态
            self.__cur_status = STATUS_TYPE.USUALING
        end
    end)

    --抽奖响应
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_LOTTERY_RESP, function()
        self.__is_on_lottery_req = false
        self.__btn_gashapon:SetTouchEnabled(true)
        --更新货币总类
        self:UpdateGashaponCoinType()
    end)

    --错误码
    self:BindGlobalEvent(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, function(_,error_code)
        --TODO 组队错误码 记得主界面也要改
        if error_code == 20015 or error_code == 20016 then
            CallModuleFunc(ModuleType.TEAM,"RemoveInvitedJoinTeamMessage")
        end
    end)

    --匹配成功进入世界地图
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_MATCH_SUCCESS, function()
        self:FireNextFrame(Macros.Event.TeamModule.BREAK_THE_TEAM,true)
        self.__cur_status = STATUS_TYPE.ENTERING_MAP
        self.__title_bar:SetZOrder(0) 
        --匹配信息隐藏
        self.__image_matching:SetVisible(false)
        --先关闭界面
        self.__title_bar:SetZOrder(Macros.Global.z_order_top)
        self.__panel_choosegamelevel:SetVisible(false)
        --屏蔽层
        local layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function ()
            -- body
        end)
        layout:SetAnchorPoint(0.5,0.5)
        self:GetNode():AddChild(layout,Macros.Global.z_order_top)
        PosTool.Center(layout)
        --匹配成功特效
        local effect_id,effect = self:__CreateEffect({id = 100082},layout)
        PosTool.Center(effect)
        --报时进入副本
        self.__match_success_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            --匹配信息隐藏
            self.__image_matching:SetVisible(false)
            --倒计时
            self.__move_in_world_map_count_down = self.__move_in_world_map_count_down - 1
            if self.__move_in_world_map_count_down > 0 then
                self.__txt_match_success_count_down:SetString(CPPGameLib.GetString("team_move_in_world_map",self.__move_in_world_map_count_down))
            else                
                if self.__match_success_count_down_timer then
                    --定时器取消
                    GlobalTimerQuest:CancelQuest(self.__match_success_count_down_timer)
                    self.__match_success_count_down_timer = nil
                end
                --切换场景
                GlobalScene:ChangeScene(Macros.Game.SceneType.TEAM_WORLD_MAP)
            end
        end), 1, 3) 
        self.__move_in_world_map_count_down = 3
        self.__txt_match_success_count_down = Util:Label(CPPGameLib.GetString("team_move_in_world_map",self.__move_in_world_map_count_down), Macros.TypefaceSize.tab, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
        layout:AddChild(self.__txt_match_success_count_down,MathTool.MAX_NUM)
        PosTool.Center(self.__txt_match_success_count_down,0,-100)        
    end)

    --限时组队结束
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_END, function(_, system_id)
        if system_id == Macros.Game.SystemID.LIMIT_TEAM then
            self.__opentime_image:SetVisible(true)
            --当前状态
            if self.__cur_status == STATUS_TYPE.MATCHING then
                --取消匹配
                self:CallModuleFunc("TeamDungeonCancelMatchReq",false,true)
            
            elseif self.__cur_status == STATUS_TYPE.GROUPING then
                --退出队伍
                self:CallModuleFunc("TeamDungeonLeaveReq",true)
            else
            end
        end
    end)
end

function Modules.TeamView:__Dispose()
    if self.__chat then
        self.__chat:DeleteMe()
        self.__chat = nil
    end

    if self.__manange_people then
        self.__manange_people:DeleteMe()
        self.__manange_people = nil
    end

    if self.__shop_people then
        self.__shop_people:DeleteMe()
        self.__shop_people = nil
    end

    if self.__choose_gamelevel_scrollview then
        self.__choose_gamelevel_scrollview:DeleteMe()
        self.__choose_gamelevel_scrollview = nil
    end

    for i,panel in ipairs(self.__group_list) do
        panel:RemoveSelf(true)
        panel = nil
    end

    --定时器取消
    if self.__match_success_count_down_timer then        
        GlobalTimerQuest:CancelQuest(self.__match_success_count_down_timer)
        self.__match_success_count_down_timer = nil
    end

    --定时器取消
    if self.__lottery_req_count_down_timer then        
        GlobalTimerQuest:CancelQuest(self.__lottery_req_count_down_timer)
        self.__lottery_req_count_down_timer = nil
    end

    if self.__team_hall_effect_id then
        GlobalEffectMgr:DelEffect(self.__team_hall_effect_id)
        self.__team_hall_effect_id = nil
    end
end

function Modules.TeamView:__ShowUI()
    --是否被邀请进来的
    local invite_from_maincity_data = self:CallModuleFunc("GetInviteFromMainCityData")
    --设置是否在匹配状态
    self:CallModuleFunc("SetIsOnMatching") 
    --开放时间
    self.__opentime_image = self:GetWidget("Image_2")
    local is_open,is_over = CallModuleFunc(ModuleType.LIMIT_DUNGEON,"IsLimitDungeonOpen",Macros.Game.SystemID.LIMIT_TEAM,true)
    self.__txt_opentime = self:GetWidget("Text_OpenTime")
    self.__opentime_image:SetVisible(is_over and true or (not is_open))
    local time_info = CPPGameLib.GetConfig("ActivityAdvanceNotice",Macros.Game.SystemID.LIMIT_TEAM)
    local time_format_list = {}
    --拼凑展示时间
    if time_info then
        for i,time_num in ipairs(time_info.start_time) do
            local s_f = string.sub(time_num,1,string.len(time_num) == 4 and 2 or 1)
            local s_b = string.sub(time_num,string.len(time_num) == 4 and 3 or 2,string.len(time_num))
            local over_num = time_info.over_time[i]
            local o_f = string.sub(over_num,1,string.len(over_num) == 4 and 2 or 1)
            local o_b = string.sub(over_num,string.len(over_num) == 4 and 3 or 2,string.len(over_num))
            time_format_list[i] = CPPGameLib.GetString("team_time_format",
                string.len(time_num) == 3 and CPPGameLib.GetString("team_time_sp_format",s_f) or s_f,s_b,
                string.len(over_num) == 3 and CPPGameLib.GetString("team_time_sp_format",o_f) or o_f,o_b)
        end
    end
    Util:WidgetLabel(self.__txt_opentime,#time_info.start_time == 1 and CPPGameLib.GetString("team_open_time",time_format_list[1] or "") or CPPGameLib.GetString("team_open_time_sp",time_format_list[1] or "",time_format_list[2] or ""), Macros.TypefaceSize.normal, Macros.Color.btn, 200, 60, Macros.Color.common_stroke)
    --组队容器
    self.__panel_dungeonteam = self:GetWidget("Panel_dungeonTeam")
    --选择关卡容器
    self.__panel_choosegamelevel = self:GetWidget("Panel_chooseGameLevel")
    self.__panel_choosegamelevel:SetZOrder(1) 
    --扭蛋机容器
    self.__panel_gashaponmachine = self:GetWidget("Panel_gashaponMachine")
    WidgetUnity.SetWidgetListener(self.__panel_gashaponmachine, nil, nil,function ()
        --关闭扭蛋机         
        --是否正在请求 
        if not self.__is_on_lottery_req then
            --定时器取消
            if self.__lottery_req_count_down_timer then                
                GlobalTimerQuest:CancelQuest(self.__lottery_req_count_down_timer)
                self.__lottery_req_count_down_timer = nil
            end
            self.__panel_gashaponmachine:SetVisible(false)
            self.__title_bar:SetVisible(true)
            self.__panel_chat:SetVisible(true)
        end
    end)

    --聊天窗口容器
    self.__panel_chat = self:GetWidget("Panel_Chat") 
    self.__panel_chat:SetZOrder(2)

    --聊天窗口
    self.__chat = Modules.MainCityChatLayer.New(self.__panel_chat)
    self.__chat:SetAnchorPoint( 0.5,0.5 )
    self.__panel_chat:AddChild(self.__chat:GetNode())
    PosTool.Center(self.__chat)

    --点击打开聊天窗口
    local layer = Util:Layout( self.__panel_chat:GetContentWidth() , self.__panel_chat:GetContentHeight() ,function ()        
        CallModuleFunc(ModuleType.CHAT, "OpenView", BaseViewType.CHAT)
    end )
    layer:SetAnchorPoint(0.5,0.5)
    self.__panel_chat:AddChild(layer , 99 )
    PosTool.Center( layer )  

    local panel_systementry = self:GetWidget("Panel_SystemEntry")  
    panel_systementry:SetZOrder(2)

    --阵容
    self.__btn_lineup = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_lineup"), function()   
        if self.__cur_status == STATUS_TYPE.MATCHING then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_matching_tips"))  
            return
        end    
        CallModuleFunc(ModuleType.HERO,"OpenView", BaseViewType.HERO_LINEUP)
    end)
    panel_systementry:AddChild(self.__btn_lineup)
    self.__btn_lineup:SetPosition(150,40)
    local label = Util:Label(CPPGameLib.GetString("dungeon_lineup_btn"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
    self.__btn_lineup:AddChild(label)
    PosTool.CenterBottom(label,0,12)

    --红点
    local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
    self.__entry_red_dot:SetAnchorPoint(1, 1)
    self.__btn_lineup:AddChild(self.__entry_red_dot,1)
    PosTool.RightTop(self.__entry_red_dot)
    --更新阵容红点
    self:__UpdateLineupBtnRedDot()  

    --帮助
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_rule")
    self.__btn_help = Util:Button(sp, function()
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Team")
        end )
    end)
    panel_systementry:AddChild(self.__btn_help)
    self.__btn_help:SetPosition(-56,40)

    --帮助文字
    local label = Util:Label(CPPGameLib.GetString("team_btn_help"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
    self.__btn_help:AddChild(label)
    PosTool.CenterBottom(label,0,12)

    --初始化各个模块入口
    self:__InitModulesEntrys()

    --初始化匹配信息
    self:__InitMatchingMessage()   
    --初始化队伍邀请信息
    self:__InitInviteJoinTeamMessage()          
    --获取邀请加入队伍信息队列
    self.__accept_invite_notify_list = self:CallModuleFunc("GetInvitedToJoinTheTeamList")
    self:__HandleInviteJoinTeamMessage()
    --如果从主界面被邀请进队伍
    if invite_from_maincity_data then
        self:AfterTeamDungeonCreateTeam(invite_from_maincity_data)
    end
    --设置是否从主界面同意进入组队
    self:CallModuleFunc("SetInviteFromMainCityDataNil")
    --是否打开
    local is_open_gashaponmachine = unpack(self.__open_data)
    if is_open_gashaponmachine then
        --打开扭蛋机
        self:CallModuleFunc("TeamDungeonLotteryRecordReq")
    end
end

--初始化各个模块入口
function Modules.TeamView:__InitModulesEntrys()  
    --删除队列
    self.__group_list = {}
    --扭蛋机
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "gashapon_machine")
    local gashapon_machine = Util:Sprite(sp)   
    GlobalScene:AddToRenderGroup(gashapon_machine,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    gashapon_machine:SetPosition(163.5,429.5)
    table.insert(self.__group_list,gashapon_machine)
    --扭蛋机容器
    local panel_gashaponmachine = Util:Layout(140, 235, function ()
        --打开扭蛋机
        self:CallModuleFunc("TeamDungeonLotteryRecordReq")
    end)
    panel_gashaponmachine:SetAnchorPoint(1,0)
    GlobalScene:AddToRenderGroup(panel_gashaponmachine,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    panel_gashaponmachine:SetPosition(271,325)
    table.insert(self.__group_list,panel_gashaponmachine)
    --扭蛋系统名字背景
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "bg_system_name")
    local image_gashaponmachinename = Util:Sprite(sp)   
    image_gashaponmachinename:SetAnchorPoint(0.5,1)
    panel_gashaponmachine:AddChild(image_gashaponmachinename)
    PosTool.CenterTop(image_gashaponmachinename,0,1)
    --扭蛋系统名字
    self:__CreateRichTextWithWidget(image_gashaponmachinename,CPPGameLib.GetString("team_gashaponmachine_name"))

    --荣誉榜
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "list_of_glory")
    local list_of_glory = Util:Sprite(sp)   
    GlobalScene:AddToRenderGroup(list_of_glory,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    list_of_glory:SetPosition(353,505)
    table.insert(self.__group_list,list_of_glory)
    --荣誉榜容器
    local panel_list_of_glory = Util:Layout(130, 146, function ()
        --打开荣誉榜
        self.__team_glory_view = LogicGUI.TemplTeamGloryRankView.New()
    end)
    GlobalScene:AddToRenderGroup(panel_list_of_glory,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    panel_list_of_glory:SetPosition(275,434)
    table.insert(self.__group_list,panel_list_of_glory)

    --荣誉榜名字背景
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "bg_system_name")
    local image_listofgloryname = Util:Sprite(sp)   
    image_listofgloryname:SetAnchorPoint(0.5,1)
    panel_list_of_glory:AddChild(image_listofgloryname)
    PosTool.CenterTop(image_listofgloryname)

    --荣誉榜系统名字
    self:__CreateRichTextWithWidget(image_listofgloryname,CPPGameLib.GetString("team_list_of_glory_name"))

    --管理员容器
    local panel_npc = Util:Layout(130, 160, function ()
        --检测限时活动状态
        local is_open,is_over = CallModuleFunc(ModuleType.LIMIT_DUNGEON,"IsLimitDungeonOpen",Macros.Game.SystemID.LIMIT_TEAM,true)
        if not is_open and not is_over then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_unopen_activity_tips")) 
            return 
        elseif is_over then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_over_activity_tips")) 
            return  
        end
        --TODO 点击管理员
        self.__title_bar:SetZOrder(0) 
        --判断状态
        if self.__cur_status == STATUS_TYPE.MATCHING then
           --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_matching_tips"))       
        elseif self.__cur_status == STATUS_TYPE.GROUPING then  
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_grouping_tips")) 
        else
            --未组队
            self.__panel_choosegamelevel:SetVisible(true)   
            self:__InitChooseGameLevelView()    
        end 
    end)
    panel_npc:SetAnchorPoint(0,1)
    -- panel_proofread:AddChild(panel_npc)
    GlobalScene:AddToRenderGroup(panel_npc,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    panel_npc:SetPosition(509,544)
    table.insert(self.__group_list,panel_npc)
    --管理员名字背景
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "bg_system_name")
    local image_npcname = Util:Sprite(sp)   
    image_npcname:SetAnchorPoint(0.5,1)
    panel_npc:AddChild(image_npcname)
    PosTool.CenterTop(image_npcname)
    --管理员名字
    self:__CreateRichTextWithWidget(image_npcname,CPPGameLib.GetString("team_npc_name"))
    --长桌
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "table_top")
    local npc_desk = Util:Sprite(sp)   
    GlobalScene:AddToRenderGroup(npc_desk,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    -- panel_proofread:AddChild(npc_desk)
    npc_desk:SetPosition(567.5,391)
    table.insert(self.__group_list,npc_desk)

    --创建管理人员
    self.__manange_people = LogicGUI.Model.New({parent = panel_npc,info_id = 30449,scale = 0.65,dir = MathTool.HORZ_LEFT})
    self.__manange_people:SetPosition(panel_npc:GetContentWidth()/2,0)

    --原石商店
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "goods_shelf")
    local shelf_shop = Util:Sprite(sp)  
    GlobalScene:AddToRenderGroup(shelf_shop,Scene.RenderQueue.GRQ_SHADOW_EFFECT) 
    shelf_shop:SetPosition(912,427.5)
    table.insert(self.__group_list,shelf_shop)
    --原石商店容器
    local panel_shop_npc = Util:Layout(230, 290, function ()
        if self.__cur_status == STATUS_TYPE.MATCHING then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_matching_tips"))  
            return
        elseif self.__cur_status == STATUS_TYPE.GROUPING then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_on_grouping_tips"))  
            return
        end
        --打开原石商店
        GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.GEM)
    end)
    panel_shop_npc:SetAnchorPoint(1,1)
    GlobalScene:AddToRenderGroup(panel_shop_npc,Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    panel_shop_npc:SetPosition(1058,583)
    table.insert(self.__group_list,panel_shop_npc)
    
    --原石商店系统背景
    sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "bg_system_name")
    local image_shopnpcname = Util:Sprite(sp)   
    image_shopnpcname:SetAnchorPoint(0.5,1)
    panel_shop_npc:AddChild(image_shopnpcname)
    PosTool.LeftTop(image_shopnpcname,82,0)
    
    --原石商店名字
    self:__CreateRichTextWithWidget(image_shopnpcname,CPPGameLib.GetString("team_shop_name"))

    --创建管理人员
    self.__shop_people = LogicGUI.Model.New({parent = panel_shop_npc,info_id = 32200,scale = 0.65,dir = MathTool.HORZ_LEFT})
    self.__shop_people:SetPosition(0,20)

    --说话
    for i=1,2 do
        --刷新说话信息
        self["funId"..i] = GlobalCallbackMgr:AddCallBackFunc(function ()
            if self["__talk_bubble"..i] then
                CPPActionManager.FadeOut(self["__talk_bubble"..i], 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
                    local randomNum = MathTool.GetRandom(5,10)
                     CPPActionManager.DelayTo(self["__talk_bubble"..i],randomNum,GlobalCallbackMgr:AddCallBackFunc(function()
                         self:RunBubbleAction(i)
                    end)) 
                end))
            end
        end)
        if i == 2 then
            CPPActionManager.DelayTo(panel_shop_npc,3,GlobalCallbackMgr:AddCallBackFunc(function()
                 self:RunBubbleAction(i)
            end))
        else
            self:RunBubbleAction(i)
        end        
    end

    --组队大厅特效   
    local effect
    self.__team_hall_effect_id,effect = GlobalEffectMgr:CreateScenePosEffect({res_name = "UI_zudui_dating",loop = true}, Scene.RenderQueue.GRQ_SHADOW_EFFECT)
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition(568,320)
end

function Modules.TeamView:GetOpenParams()
    return {self.__is_open_gashaponmachine}
end

--创建系统名字富文本
function Modules.TeamView:__CreateRichTextWithWidget(entry_widget,module_txt)
    local entry_txt = Util:RichText(string.format(Macros.Color.team_headline_rt, module_txt), Macros.TypefaceSize.largish)
    entry_txt:SetAnchorPoint(0.5, 0.5)
    entry_widget:AddChild(entry_txt)
    PosTool.Center(entry_txt)
end

--更新阵容按钮红点
function Modules.TeamView:__UpdateLineupBtnRedDot()
    local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.HERO_LINEUP, Macros.Game.SystemID.MAX)  
    self.__entry_red_dot:SetVisible(red_num > 0)
end

--气泡
function Modules.TeamView:RunBubbleAction(show_index)
    self:ShowTalk(show_index)
    -- local randomNum = MathTool.GetRandom(5,10)
    CPPActionManager.DelayTo(self["__talk_bubble"..show_index],3,self["funId"..show_index])  
end

--气泡文字
function Modules.TeamView:ShowTalk(show_index)
    if not self["__talk_bubble"..show_index] then
        --气泡
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "lead_tips_bg")
        self["__talk_bubble"..show_index] = Util:Sprite(sp)
        self["__talk_bubble"..show_index]:SetFlipX(true)
        self["__talk_bubble"..show_index]:SetAnchorPoint(1,0)
        GlobalScene:AddToRenderGroup(self["__talk_bubble"..show_index],Scene.RenderQueue.GRQ_SHADOW_EFFECT)
        self["__talk_bubble"..show_index]:SetPosition(show_index == 1 and 550 or 800,show_index == 1 and 450 or 400)
        table.insert(self.__group_list,self["__talk_bubble"..show_index])
        --气泡文字
        self["__talk_bubble_txt"..show_index] = Util:Label("", 32, Macros.Color.keypoint,270,125)
        self["__talk_bubble_txt"..show_index]:SetAnchorPoint(1,0.5)
        self["__talk_bubble_txt"..show_index]:SetTextVertiacalAlign(LabelEx.ALIGIN_CENTER)
        self["__talk_bubble"..show_index]:AddChild(self["__talk_bubble_txt"..show_index])
        PosTool.RightCenter(self["__talk_bubble_txt"..show_index],-10,0)
    end
    self["__talk_bubble"..show_index]:SetOpacity(0)  
    --随机文字
    local tips_config = CPPGameLib.GetConfig("TeamNPCTips",show_index)
    local randomNum = MathTool.GetRandom(1,#tips_config.tips_list)
    local tips_info =  CPPGameLib.GetConfig("GameTipsList",tips_config.tips_list[randomNum] or 1)  
    self["__talk_bubble_txt"..show_index]:SetString(tips_info and tips_info.text or "")    
    self["__talk_bubble"..show_index]:SetScaleFactor(0.1)
    self["__talk_bubble"..show_index]:SetOpacity(255)
    CPPActionManager.ScaleTo(self["__talk_bubble"..show_index],0.3,0.5,nil)  
end

--初始化选关界面
function Modules.TeamView:__InitChooseGameLevelView()
    --是否初始化选择关卡界面
    if self.__is_init_choose_gameLevel then
        return
    else
        self.__is_init_choose_gameLevel = true
    end
    --标题
    Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("team_gamelevel_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_chooseGameLevelClose"),"",function ()
        self.__title_bar:SetZOrder(Macros.Global.z_order_top)
        self.__panel_choosegamelevel:SetVisible(false)
    end)

    --关卡选择容器
    local panel_choosegamelevelscrollview = self:GetWidget("Panel_chooseGameLevelScrollview")
    local params = {
        item_class = Modules.TeamChooseGameLevelViewItem,
        item_width = 446,
        item_height = 127,
        row = 4,
        col = 1,
        item_space_r = 7,
        view_width = 446,
        view_height = 434,
        selectable = true,
        select_callback = function (index,data) 
            self.__selected_dungeon_data = data           
            --选中回调
            self:__SelectedCallBack(data)
        end,
        select_filter_func = function (index,data)
            --打开第一个不用进行判断
            if index == 1 then
                return true
            end
            --判断等级是否满足           
            local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
            return role_level >= data.limit_level
        end,
        select_filter_func_callback = function (index,data)            
            local tips_str = data.limit_level == 999 and CPPGameLib.GetString("team_unopen_for_now") or CPPGameLib.GetString("team_recommend_level_tips",data.limit_level)
            GlobalTipMsg.GetInstance():Show(tips_str)
        end,
    }
    self.__choose_gamelevel_scrollview = GUI.ScrollView.New(params)
    panel_choosegamelevelscrollview:AddChild(self.__choose_gamelevel_scrollview:GetNode())
    PosTool.Center(self.__choose_gamelevel_scrollview)
    --角色等级
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    --副本数据
    local total_data = {}
    for i,fb_data in ipairs(CPPGameLib.GetConfig("TeamFBChapter", nil, nil, true)) do
        table.insert(total_data,{config_id = fb_data.dungeon_id,role_level = role_level})
    end
    self.__choose_gamelevel_scrollview:SetDataList(total_data)
    
    --关卡名字
    self.__text_gameleveltitle = self:GetWidget("Text_gamelevelTitle")
    Util:WidgetLabel(self.__text_gameleveltitle, "", Macros.TypefaceSize.popup, Macros.Color.Special, 0, 0, Macros.Color.content_stroke)
    --关卡描述
    self.__text_gameleveldes = self:GetWidget("Text_gamelevelDes")
    Util:WidgetLabel(self.__text_gameleveldes, "", Macros.TypefaceSize.normal, Macros.Color.keypoint , 312, 70)
    --奖励预览
    Util:WidgetLabel(self:GetWidget("Text_rewardPreLook"),CPPGameLib.GetString("team_gamelevel_reward_prelook"), Macros.TypefaceSize.largish, Macros.Color.btn)
    --奖励预览容器
    self.__panel_rewardscrollview = self:GetWidget("Panel_rewardScrollview")   
    --剩余次数
    self.__text_retaintimes = self:GetWidget("Text_retainTimes")
    Util:WidgetLabel(self.__text_retaintimes, "", Macros.TypefaceSize.normal, Macros.Color.keypoint )
    --创建队伍
    self.__button_createteam = self:GetWidget("Button_createTeam")
    Util:WidgetButton(self.__button_createteam,CPPGameLib.GetString("team_gamelevel_create_group"),function ()        
        --判断等级是否满足           
        local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
        if role_level < self.__selected_dungeon_data.limit_level then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_lineup_invalid_level"))
            return 
        end
        --创建队伍请求
        self:CallModuleFunc("TeamDungeonCreateTeamReq",{dungeon_id = self.__selected_dungeon_id})
    end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow )
    --快速匹配
    self.__button_quickmatching = self:GetWidget("Button_quickMatching")
    Util:WidgetButton(self.__button_quickmatching,CPPGameLib.GetString("team_gamelevel_quick_matching"),function () 
        --判断等级是否满足           
        local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
        if role_level < self.__selected_dungeon_data.limit_level then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_lineup_invalid_level"))
            return 
        end         
        --请求快速匹配
        self:CallModuleFunc("TeamDungeonMatchReq",{dungeon_id = self.__selected_dungeon_id,is_team = Macros.Global.FALSE})
    end, Macros.TypefaceSize.tab, Macros.Color.btn, Macros.Color.button_red)
    --选择
    self.__choose_gamelevel_scrollview:SelectItem(1)
end

--初始化匹配信息
function Modules.TeamView:__InitMatchingMessage()
    --匹配信息背景
    self.__image_matching = self:GetWidget("Image_Matching")
    --匹配关卡名字
    self.__text_mathching_gamelevel_name = self:GetWidget("Text_MathchingGameLevelName")
    Util:WidgetLabel(self.__text_mathching_gamelevel_name, "", Macros.TypefaceSize.largish, Macros.Color.Special)
    --匹配人数
    self.__text_mathching_peoplenum = self:GetWidget("Text_MathchingPeopleNum")
    Util:WidgetLabel(self.__text_mathching_peoplenum,"", Macros.TypefaceSize.normal, Macros.Color.btn)
    --取消匹配
    self.__button_cancelmatching = self:GetWidget("Button_CancelMatching")
    Util:WidgetButton(self.__button_cancelmatching,CPPGameLib.GetString("team_gamelevel_cancel_matching"),function ()
        --请求取消匹配
        self:CallModuleFunc("TeamDungeonCancelMatchReq")         
    end, Macros.TypefaceSize.tab, Macros.Color.btn, Macros.Color.button_yellow )  
    --跳点
    for i=1,6 do
        self["__txt_jumppoint"..i] = self:GetWidget("Text_jumpPoint"..i)
        self["__txt_jumppoint"..i]:SetVisible(false)
        Util:WidgetLabel(self["__txt_jumppoint"..i],".", Macros.TypefaceSize.normal, Macros.Color.btn)
    end 
    --开始跳动
    self.__jump_count_num = 0
    self.__jump_point_funid = GlobalCallbackMgr:AddCallBackFunc(function()
        self.__jump_count_num = 0
        self.__txt_jumppoint6:SetVisible(true)
        CPPActionManager.DelayTo(self.__txt_jumppoint1,0.5,GlobalCallbackMgr:AddCallBackFunc(function()
            self:__JumpPointAction()
        end))
    end)
    self:__JumpPointAction()
end

--初始化队伍邀请信息
function Modules.TeamView:__JumpPointAction()
    self.__jump_count_num = self.__jump_count_num + 1
    self["__txt_jumppoint"..self.__jump_count_num]:SetVisible(true)
    --上一个隐藏
    if self["__txt_jumppoint"..(self.__jump_count_num-1)] and (self.__jump_count_num-1)%2 ~= 0 then
        --上一个不为整数则隐藏
        self["__txt_jumppoint"..(self.__jump_count_num-1)]:SetVisible(false)
    end
    --下一个隐藏
    if self["__txt_jumppoint"..(self.__jump_count_num + 1)] and self.__jump_count_num%2 == 0 then
        --当前数为2的整数倍则直接显示下一个
        self:__JumpPointAction()
        return
    elseif self["__txt_jumppoint"..(self.__jump_count_num + 1)] then
        self["__txt_jumppoint"..(self.__jump_count_num+1)]:SetVisible(false)
    end
    --0.2秒后显示下一个
    CPPActionManager.DelayTo(self.__txt_jumppoint1,0.2,self.__jump_count_num >= 6 and self.__jump_point_funid or GlobalCallbackMgr:AddCallBackFunc(function()
        self:__JumpPointAction()
    end))
end

--初始化队伍邀请信息
function Modules.TeamView:__InitInviteJoinTeamMessage()
    --队伍邀请信息背景
    self.__image_invitejointeam = self:GetWidget("Image_InviteJoinTeam")
    --邀请队伍要去的关卡名字
    self.__text_inviteplayer_name = self:GetWidget("Text_InvitePlayerName")
    Util:WidgetLabel(self.__text_inviteplayer_name, "", Macros.TypefaceSize.largish, Macros.Color.btn)
    --邀请队伍信息
    self.__text_invitejointeamrole = Util:RichText("", Macros.TypefaceSize.normal,0,0,Macros.Color.btn_hex)
    self.__text_invitejointeamrole:SetAnchorPoint(0.5,0.5)
    self.__image_invitejointeam:AddChild(self.__text_invitejointeamrole)
    PosTool.Center(self.__text_invitejointeamrole,0,6)
    --加入队伍
    self.__button_jointheteam = self:GetWidget("Button_JoinTheTeam")
    Util:WidgetButton(self.__button_jointheteam,CPPGameLib.GetString("team_gamelevel_join_the_team"),function ()
        --接受组队邀请请求
        self:CallModuleFunc("TeamDungeonAcceptInviteReq",{team_id = self.__accept_invite_notify_list[1].team_id,dungeon_id = self.__accept_invite_notify_list[1].dungeon_id})
    end, Macros.TypefaceSize.popup, Macros.Color.team_btn_join)  
    --拒绝加入队伍
    self.__button_closeinvitation = self:GetWidget("Button_CloseInvitation")
    Util:WidgetButton(self.__button_closeinvitation,CPPGameLib.GetString("team_gamelevel_reject_the_team"),function ()
        --从邀请队列里删除当前的邀请信息
        self:CallModuleFunc("RemoveInvitedJoinTeamMessage")
    end, Macros.TypefaceSize.popup, Macros.Color.team_btn_reject)  
end

--处理队伍邀请信息
function Modules.TeamView:__HandleInviteJoinTeamMessage()
    if not self.__accept_invite_notify_list or #self.__accept_invite_notify_list < 1 then
        self.__image_invitejointeam:SetVisible(false)
        return
    end
    local invite_info = self.__accept_invite_notify_list[1]
    --副本信息
    local fb_config = CPPGameLib.GetConfig("TeamFBChapter",invite_info.dungeon_id)
    --邀请队伍要去的关卡名字
    self.__text_invitejointeamrole:SetText(CPPGameLib.GetString("team_invite_to_team",fb_config.dungeon_name))
    --邀请队伍信息
    self.__text_inviteplayer_name:SetText(CPPGameLib.GetString("team_player_name",invite_info.server_id or 0,invite_info.role_name or ""))
    --队伍邀请信息显示
    self.__image_invitejointeam:SetVisible(true)
end

--处理匹配信息
function Modules.TeamView:__HandleQuickMatchingMessage(protocol_data) 
    local fb_config = CPPGameLib.GetConfig("TeamFBChapter",protocol_data.dungeon_id)
    --邀请队伍要去的关卡名字
    self.__text_mathching_gamelevel_name:SetText(fb_config.dungeon_name)
    --匹配人数
    self.__text_mathching_peoplenum:SetText(CPPGameLib.GetString("team_gamelevel_matching_people_num",1,Macros.Game.MAX_TEAMMATE_NUM))
    --先关闭界面
    self.__title_bar:SetZOrder(Macros.Global.z_order_top)
    self.__panel_choosegamelevel:SetVisible(false)  
    --匹配信息可见
    self.__image_matching:SetVisible(true)
    --当前状态
    self.__cur_status = STATUS_TYPE.MATCHING
    --是否在队伍中的匹配
    self.__is_in_team_match = protocol_data.is_team == 1 and true or false
end

--处理匹配信息
function Modules.TeamView:__HandleMatchTargetNumNotify(match_num) 
    if self.__text_mathching_peoplenum then
         --匹配人数
        self.__text_mathching_peoplenum:SetText(CPPGameLib.GetString("team_gamelevel_matching_people_num",match_num or 1,Macros.Game.MAX_TEAMMATE_NUM))
    end
end

--打开扭蛋机
function Modules.TeamView:__OpenGashaponMachine(lottery_record_list) 
    if not lottery_record_list then
        return
    end
    local gashapon_posx = 498
    self.__panel_gashaponmachine:SetVisible(true)
    self.__title_bar:SetVisible(false)
    self.__panel_chat:SetVisible(false)
    --特效
    if self.__gashaponmachine_ui_effect_id then
        self:__DelEffect(self.__gashaponmachine_ui_effect_id)
        self.__gashaponmachine_ui_effect_id = nil
    end
    self.__gashaponmachine_ui_effect_id,effect = self:__CreateEffect({id = 100079,loop = true},self:GetWidget("Panel_CenterMachine"),3)
    effect:SetAnchorPoint(0.5,0)
    effect:SetZOrder(2)
    effect:SetPosition(gashapon_posx,3)
    --创建扭蛋机
    if not self.__gashaponmachine_effect_id then       
        --扭蛋机
        local effect
        self.__gashaponmachine_effect_id, effect = self:__CreateEffect({res_name = "UI_gashaponmachine", type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE},self:GetWidget("Panel_CenterMachine"),3)
        effect:SetAnchorPoint(0.5,0)
        effect:SetZOrder(1)
        effect:SetPosition(gashapon_posx,3)  
        --屏蔽层
        local shield_layout = Util:Layout(400, 640, function ()
            -- 
        end)
        shield_layout:SetAnchorPoint(0.5,0)
        self:GetWidget("Panel_CenterMachine"):AddChild(shield_layout,0)
        shield_layout:SetPosition(gashapon_posx,3)
        --扭蛋按钮
        self.__btn_gashapon = self:GetWidget("Button_gashapon")
        self.__btn_gashapon:SetZOrder(2)
        Util:WidgetButton(self.__btn_gashapon, CPPGameLib.GetString("team_gashaponmachine"),function ()
            --是否扭10次
            local is_ten_times = self.__checkbox_coin:GetSelectedState()
            local gashapon_times = is_ten_times and 10 or 1
            local is_cost_item = 1
            local coin_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetTeamGashaponMachineCoin())
            local is_buy_pass = false
            local judge_func = function (diamonds,cost_diamonds,coin_req)
                if not coin_req then
                    --判断钻石是否足够
                    if diamonds >= cost_diamonds then
                        is_buy_pass = true
                        is_cost_item = 0
                    else
                        LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("team_gashapon_no_diamonds_tips")
                            ,confirm_func = function ()
                                self.__is_open_gashaponmachine = true
                                --判断钻石是否足够
                                GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.CHARGE)
                            end,})
                    end
                    --提示
                    if not is_buy_pass then                                
                        return 
                    end
                end
                self.__is_on_lottery_req = true
                self.__btn_gashapon:SetTouchEnabled(false)
                --扭蛋机待机特效
                if self.__gashaponmachine_ui_effect_id then
                    local ui_effect = GlobalEffectMgr:GetEffect(self.__gashaponmachine_ui_effect_id)
                    if ui_effect then
                        ui_effect:SetVisible(false)
                    end
                end
                --动作
                GlobalEffectMgr:SetEffectAnim(self.__gashaponmachine_effect_id,is_ten_times and RenderObj.ActionIndex.SP_SKILL or RenderObj.ActionIndex.ATTACK)
                --闪灯特效
                local ui_effect_id,effect = self:__CreateEffect({id = 100080},self.__panel_gashaponmachine,3)
                effect:SetAnchorPoint(0.5,0)
                effect:SetZOrder(2)
                effect:SetPosition(Macros.Global.DesignSize.width/2+18,3)  
                --爆炸特效
                local ui_boom_effect_id,boom_effect = self:__CreateEffect({id = 100081},self.__panel_gashaponmachine,3)
                boom_effect:SetAnchorPoint(0.5,0)
                boom_effect:SetZOrder(2)
                boom_effect:SetPosition(Macros.Global.DesignSize.width/2+18,3)   
                --延时播放
                CPPActionManager.DelayTo(self.__btn_gashapon,1.5,GlobalCallbackMgr:AddCallBackFunc(function()
                    --闪灯特效
                    if ui_effect_id then
                        self:__DelEffect(ui_effect_id)
                        ui_effect_id = nil
                    end
                    --爆炸特效
                    if ui_boom_effect_id then
                        self:__DelEffect(ui_boom_effect_id)
                        ui_boom_effect_id = nil
                    end
                    GlobalEffectMgr:SetEffectAnim(self.__gashaponmachine_effect_id,RenderObj.ActionIndex.IDLE)
                    --扭蛋机待机特效
                    if self.__gashaponmachine_ui_effect_id then
                        local ui_effect = GlobalEffectMgr:GetEffect(self.__gashaponmachine_ui_effect_id)
                        if ui_effect then
                            ui_effect:SetVisible(true)
                        end                    
                    end
                    --请求扭蛋
                    self:CallModuleFunc("TeamDungeonLotteryReq",{times = gashapon_times,is_cost_item = is_cost_item})
                end))
            end
            if gashapon_times > coin_num then                              
                --消耗钻石
                local cost_diamonds = is_ten_times and ConfigAdapter.Common.GetGashaponCostDiamondTenTime() or ConfigAdapter.Common.GetGashaponCostDiamondOneTime()
                --当前钻石
                local diamonds = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
                --获取是否显示提示
                local is_show = self:CallModuleFunc("GetIsShowGashaponCostDiamondsTips")
                if is_show then
                    --扭蛋币不足则消耗钻石
                    LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("team_gashapon_cost_diamonds_tips",cost_diamonds)
                        ,confirm_func = function (is_hide)
                            self:CallModuleFunc("SetIsShowGashaponCostDiamondsTips",is_hide)
                            judge_func(diamonds,cost_diamonds)
                    end,show_checkbox_content = CPPGameLib.GetString("common_no_show")})
                else
                    judge_func(diamonds,cost_diamonds)
                end  
            else
                judge_func(nil,nil,true)
            end                              
        end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.gashaponmachine_stroke )   
        --扭蛋提示文字
        self:GetWidget("Image_coinTxt"):SetZOrder(2)
        self:GetWidget("Image_coinTxt"):IgnoreContentAdaptWithSize(true)
        --复选框       
        self.__checkbox_coin = self:GetWidget("CheckBox_coin")
        self.__checkbox_coin:SetSelectedState(false)
        self.__checkbox_coin:SetZOrder(2)
        WidgetUnity.SetCheckboxListener(self.__checkbox_coin,function ()
            -- 10次 self.__text_diamondnum:SetText(10)
        end, function ()
            -- 1次  self.__text_diamondnum:SetText(1)
        end)
        --投10个币
        self.__text_coinnum = self:GetWidget("Text_coinNum")
        self.__text_coinnum:SetZOrder(2)
        Util:WidgetLabel(self.__text_coinnum,CPPGameLib.GetString("team_gashapon_ten_times"), Macros.TypefaceSize.normal, Macros.Color.btn)
        --货币类型
        self.__image_type = self:GetWidget("Image_type")      
        self.__image_type:IgnoreContentAdaptWithSize(true)
        self.__image_type:SetZOrder(2)
        self.__image_type:SetScaleFactor(0.6)
        --货币数量        
        self.__text_diamondnum = self:GetWidget("Text_diamondNum")
        self.__text_diamondnum:SetZOrder(2)
        Util:WidgetLabel(self.__text_diamondnum,"", Macros.TypefaceSize.normal, Macros.Color.btn)     
        --帮助
        Util:WidgetButton(self:GetWidget("Button_help"),"",function ()
            local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
            help:SetLoadedCallback( function ( ... )
               help:SetCell("Rule_TeamLottery")
            end )
        end)
        --帮助文字
        Util:WidgetLabel(self:GetWidget("Text_help"),CPPGameLib.GetString("team_btn_help"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
        --奖励预览
        local btn_rewardpreLook = self:GetWidget("Button_rewardPreLook")
        btn_rewardpreLook:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_cumulative_recharge"),TextureResType.LOCAL)
        Util:WidgetButton(btn_rewardpreLook,"",function ()
            --打开预览奖励
            LogicGUI.TemplTeamLotteryRewardPreLook.New()
        end)
        --奖励预览文字
        Util:WidgetLabel(self:GetWidget("Text_rewardPreLook"),CPPGameLib.GetString("team_btn_rewardpreLook"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
        --列表容器
        self.__listview_rewardprelook = self:GetWidget("ListView_RewardPreLook")
    end   
    --先移除所有所有控件
    self.__listview_rewardprelook:RemoveAllItems()
    --抽奖记录
    for i,lottery_record_info in ipairs(lottery_record_list) do
        --富文本
        local item_type,item_info = ObjIDTool.GetInfo(lottery_record_info.item_id)
        if not item_info then
            return
        end
        local quality_str = string.format(Macros.Quality_RT[item_info.quality or 0],item_info.name)
        local rt_str = CPPGameLib.GetString("team_lottery_record_rt",CPPGameLib.GetString("team_lottery_record_player_name",lottery_record_info.server_id,
                lottery_record_info.role_name),quality_str,lottery_record_info.item_num)
        local rich_txt = Util:RichText(rt_str, Macros.TypefaceSize.slightly, 280, 0, Macros.Color.btn_hex)
        rich_txt:SetAnchorPoint(0,1)
        --控件
        local layout = Util:Layout(rich_txt:GetContentWidth(), rich_txt:GetContentHeight())
        layout:AddChild(rich_txt)
        PosTool.LeftTop(rich_txt)
        --插入条目
        self.__listview_rewardprelook:PushBackCustomItem(layout)
    end
    --更新货币总类
    self:UpdateGashaponCoinType()
    
    if not self.__lottery_req_count_down_timer then
        --延时请求
        self.__lottery_req_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            self.__is_count_down_lottery_req = true
            self:CallModuleFunc("TeamDungeonLotteryRecordReq")
        end), 3, -1)
    end
    --先整理
    self.__listview_rewardprelook:ForceDoLayout()
    if not self.__is_count_down_lottery_req then
        --移动
        local inner_posx,inner_posy = self.__listview_rewardprelook:GetInnerContainerPos()
        self.__listview_rewardprelook:StartAutoScrollToDestination(COCOPoint(inner_posx,0),3,false)
    else
        self.__listview_rewardprelook:ScrollToBottom()
    end
    self.__is_count_down_lottery_req = false
end

--更新货币总类
function Modules.TeamView:UpdateGashaponCoinType()
    --货币数量        
    local coin_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetTeamGashaponMachineCoin())
    coin_num = coin_num or 0
    --货币类型
    local type,config = ObjIDTool.GetInfo(ConfigAdapter.Common.GetTeamGashaponMachineCoin())
    if coin_num <= 0 then
        local huobi_info = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.DIAMOND)
        type,config = ObjIDTool.GetInfo(huobi_info.big_icon)
    end 
    local path = Resource.PathTool.GetItemPath(config.res_id)       
    self.__image_type:SetImage(path,TextureResType.LOCAL)
    if coin_num <= 0 then
        coin_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    end
    self.__text_diamondnum:SetText(string.format("X%d",coin_num))
end

--选中回调
function Modules.TeamView:__SelectedCallBack(data)
    self.__selected_dungeon_id = data.config_id
    --配置表信息
    local config_fb = CPPGameLib.GetConfig("TeamFBChapter",data.config_id)
    --关卡名字
    self.__text_gameleveltitle:SetText(config_fb.dungeon_name)
    --关卡描述
    self.__text_gameleveldes:SetText(config_fb.dungeon_des)
    --奖励预览容器
    if not self.__reward_scrollview then        
        local params = {
            item_class = Modules.TeamChooseGameLevelPreLookItem,
            item_width = 80,
            item_height = 80,
            row = 1,
            col = 4,
            horizon = true,
            item_space_c = 10,
            view_width = 305,
            view_height = 80,
        }
        self.__reward_scrollview = GUI.ScrollView.New(params)
        self.__panel_rewardscrollview:AddChild(self.__reward_scrollview:GetNode())
        PosTool.Center(self.__reward_scrollview)
    end
    --构造数据
    local prelook_data = {}
    for i,cur_item_id in ipairs(config_fb.reward_prelook_list) do
        local data = {item_id = cur_item_id}
        table.insert(prelook_data,data)
    end
    CPPGameLib.PrintTable("ldx",prelook_data,"prelook_dataprelook_data: ")
    self.__reward_scrollview:SetDataList(prelook_data)
    --剩余次数      
    self.__text_retaintimes:SetText(CPPGameLib.GetString("team_remain_challenge_times",math.max(0,self.__team_dungeon_data.dungeon_fight_times - 
            self.__team_dungeon_data.daily_battle_times)))
end

--取消匹配
function Modules.TeamView:AfterCancelMatching() 
    --匹配信息隐藏
    self.__image_matching:SetVisible(false)
    --当前状态
    self.__cur_status = STATUS_TYPE.USUALING
end

--创建队伍
function Modules.TeamView:AfterTeamDungeonCreateTeam(protocol_data) 
    --队伍邀请信息隐藏
    if not self.__image_invitejointeam then
        self:__InitInviteJoinTeamMessage()
    end
    self.__image_invitejointeam:SetVisible(false)
    --先关闭界面
    self.__title_bar:SetZOrder(Macros.Global.z_order_top)
    self.__panel_choosegamelevel:SetVisible(false)
    --当前队伍id
    self.__cur_team_id = protocol_data.team_id
    --当前队伍副本id
    self.__cur_team_dungeon_id = protocol_data.dungeon_id
    --当前状态
    self.__cur_status = STATUS_TYPE.GROUPING
    --创建队伍界面
    self.__teammate_view = LogicGUI.TemplCreateTheTeam.New(self.__cur_team_id,self.__cur_team_dungeon_id,protocol_data.member_list)
end

--------------------------------------------------------------------------------------------
--组队副本关卡列表条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.TeamChooseGameLevelViewItem = Modules.TeamChooseGameLevelViewItem or BaseClass(GUI.ScrollItem)

function Modules.TeamChooseGameLevelViewItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TeamChooseGameLevelViewItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(446,127)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TeamChooseGameLevelViewItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    --配置表信息
    local config_fb = CPPGameLib.GetConfig("TeamFBChapter",self.__data.config_id)
    data.limit_level = config_fb.recommend_level[1]
    --框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "dungeon_list_frame")
    local frame = Util:Sprite(sp)
    self.layout:AddChild(frame)
    PosTool.Center(frame)
    --关卡图
    local image_challenge_bg = Util:Sprite(Resource.PathTool.GetUIResPath("dungeon_challenge_bg/"..config_fb.gamelevel_id))
    self.layout:AddChild(image_challenge_bg)
    image_challenge_bg:SetGray(config_fb.recommend_level[1] > self.__data.role_level) 
    image_challenge_bg:SetZOrder(-1)
    image_challenge_bg:SetScaleFactor(0.70)
    PosTool.Center(image_challenge_bg)    
    --关卡名字
    local label = Util:Label(config_fb.dungeon_name, Macros.TypefaceSize.popup, Macros.Color.Special)
    label:SetAnchorPoint(0,0.5)
    self.layout:AddChild(label)
    label:SetPosition(10,22)
    --进入等级
    label = Util:Label(config_fb.recommend_level[1] == 999 and CPPGameLib.GetString("team_unopen_for_now") or CPPGameLib.GetString("team_gamelevel_recommend_level",config_fb.recommend_level[1],config_fb.recommend_level[2]), Macros.TypefaceSize.normal, Macros.Color.golden,0,0,Macros.Color.common_stroke)
    label:SetAnchorPoint(0,0.5)
    self.layout:AddChild(label)
    label:SetPosition(260,22)
end

function Modules.TeamChooseGameLevelViewItem:SetSelected(var)
    if var then
        if not self.__selected_image then
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "bg_sp9_selected")
            self.__selected_image = Util:Sprite9(sp)
            self.__selected_image:SetContentSize(self.layout:GetContentWidth()+5,self.layout:GetContentHeight()+5)
            self.layout:AddChild(self.__selected_image, Macros.Global.z_order_top - 1)
            PosTool.Center(self.__selected_image)
        end
    else
        if self.__selected_image then
            self.__selected_image:RemoveSelf(true)
            self.__selected_image = nil
        end
    end 
end

--------------------------------------------------------------------------------------------
--组队副本关卡奖励预览列表条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.TeamChooseGameLevelPreLookItem = Modules.TeamChooseGameLevelPreLookItem or BaseClass(GUI.ScrollItem)

function Modules.TeamChooseGameLevelPreLookItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TeamChooseGameLevelPreLookItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,80)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TeamChooseGameLevelPreLookItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    --图标
    local icon = LogicGUI.IconLayer.New()
    self.layout:AddChild(icon:GetNode())
    PosTool.Center(icon)
    icon:SetData({id = self.__data.item_id})
end
