
--[[
%% @module: 组队世界地图界面
%% @author: ldx
%% @created: 2017-09-27
--]]

Modules = Modules or {}

Modules.TeamWorldMapView = Modules.TeamWorldMapView or BaseClass(BaseView)

local scale_to_time = 0.3

local progress_to_time = 3

function Modules.TeamWorldMapView:__init()
    self.__layout_name = "team_world_map" 

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM_WORLD_MAP)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM_WORLD_MAP,true)
end

function Modules.TeamWorldMapView:__OpenCallback()   
    --设置定时器
    self:SetUpdateFunc(function (elapse_time)
        --进度条
        if self.__smooth_end_time and self.__smooth_end_time > 0 then
            self.__smooth_elapse_time = self.__smooth_elapse_time + elapse_time
            local energy = self.__team_dungeon_before_energy + self.__smooth_cur_speed * self.__smooth_elapse_time
            if CPPGameEngine:GetNowTime() - self.__smooth_end_time >= 0 then
                --是否升级
                if self.__is_change_anther_progress then
                    --升级效果
                    if self.__box_treasure_level_up then
                        self.__box_treasure_level_up()
                    end
                    self.__is_change_anther_progress = false
                    local cur_stage                    
                    cur_stage,self.__cur_energy_limit,self.__before_energy_limit = self:FindCurWhichStage(self.__team_dungeon_task_data.energy)
                    self.__team_dungeon_before_energy = self.__before_energy_limit
                    --loadingbar速度
                    self.__smooth_cur_speed = (self.__cur_energy_limit - self.__before_energy_limit)/progress_to_time
                    self.__smooth_elapse_time = 0 
                    energy = self.__before_energy_limit                
                    --变化                        
                    self.__smooth_end_time = CPPGameEngine:GetNowTime() + (self.__team_dungeon_task_data.energy - self.__before_energy_limit)/self.__smooth_cur_speed                                      
                else
                    self.__smooth_end_time = 0
                    energy = self.__team_dungeon_task_data.energy
                end                                                   
            end
            --设置进度
            self.__box_progress_loadingbar:SetPercent(math.min(1,((energy - self.__before_energy_limit)/(self.__cur_energy_limit - self.__before_energy_limit)))*100)
        end
        --移动背景
        if self.__update_move then
            self:__MovingBottomLayer()
        end
    end) 
     --创建层
    self:__CreateLayers()  
    self:__ShowUI()    
end

function Modules.TeamWorldMapView:__RegistAllEvents()
    --前往NPC
    self:BindGlobalEvent(Macros.Event.TeamModule.MOVE_TO_NPC_NOTICE, function()
        --队长先移动
        if self.__team_model1 then
            self.__update_move = true                                                  
            --更新队伍移动
            self:__UpdateTeamMoving()        
        end
        self.__is_work_to_npc = true
    end)

    --对话结束响应
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_DIALOG_OVER_RESP, function(_,is_accept_task) 
        if is_accept_task then
            --任务内容
            local lab = GUI.RichText.GetColorWord(self.__config_task.desc2..CPPGameLib.GetString("team_task_num",self.__cur_task_complete_value,self.__total_task_num), Macros.Color.btn_hex)
            self.__text_curtask:SetText(lab)    
        end  
        --新任务
        if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.QUESTION then
            --答题                      
            if is_accept_task then         
                self:__InitQuestion()
            end           
        elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.GATHER 
            or self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.BATTLE_MONSTER then
            if is_accept_task then
                self.__pos_list = {pos_x = self.__config_task.task_param3[1],pos_y = self.__config_task.task_param3[2]}
                --特殊移动
                self.__special_move = true
                --队长先移动
                if self.__team_model1 then
                    self.__update_move = true                                                  
                    --更新队伍移动                   
                    self:__UpdateTeamMoving(function ()
                        self:RunFunctionByTaskType()
                    end)     
                end
                self.__is_work_to_npc = true               
            end 
        elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.CONTENT then
            --对话   
            if not self.__is_new_task then         
                self:UpdateTaskCompleteValue(false,is_accept_task) 
            end       
        end       
        self.__is_new_task = false
    end)

    --新任务通知
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_NEW_TASK_NOTIFY, function()
        self.__is_new_task = true
        --获取当前任务id
        self.__new_task_data = self:CallModuleFunc("GetCurTaskData")
        self.__is_finish_task_link = CallModuleFunc(ModuleType.TEAM,"GetIsFinishTaskLink")        
        self:SetNewTaskContent()
    end)

    --答题通知
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY, function(_,protocol_data)        
        for i=1,4 do
            if self.__team_dungeon_task_data.member_list[i] and self.__team_dungeon_task_data.member_list[i].role_id == protocol_data.role_id then
                self:ShowChatContent(i,CPPGameLib.GetString("team_question_answer_notify"..protocol_data.answer) or "")
                break
            end
        end       
    end)

    --答题结束后显示正确答案
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_SHOW_RIGHT_ANSWER, function(_,is_pass) 
        --答题计时器
        if self.__question_count_down_timer then
            GlobalTimerQuest:CancelQuest(self.__question_count_down_timer)
            self.__question_count_down_timer = nil
        end     
        if self.__question_config then           
            --勾
            self["__image_righttick"..(self.__question_config.right_answer or 1)]:SetVisible(true) 
            --2秒后其他
            CPPActionManager.DelayTo(self["__image_righttick"..(self.__question_config.right_answer or 1)],2,GlobalCallbackMgr:AddCallBackFunc(function()               
                --BOSS
                if self.__is_time_to_boss then
                    self.__panel_question:SetVisible(false)
                    return
                end
                --获取当前任务id
                self.__new_task_data = self:CallModuleFunc("GetCurTaskData")
                --是否是新任务
                if self.__new_task_data.is_new_task == Macros.Global.TRUE or is_pass then
                    self.__is_new_task = true
                    self.__is_finish_task_link = CallModuleFunc(ModuleType.TEAM,"GetIsFinishTaskLink")   
                    self.__panel_question:SetVisible(false)
                    self:SetNewTaskContent()
                else
                    self:__InitQuestion()
                end
            end))
        end             
    end)

    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function()
        CallModuleFunc(ModuleType.TEAM, "ShowTeamDismissNotifyView")
    end)    

    --离开队伍通知
    self:BindGlobalEvent(Macros.Event.TeamModule.LEAVE_THE_TEAM, function(_,protocol_data)
        --任务队伍
        if protocol_data.is_task_team == 1 then
            --模型
            for i=1,Macros.Game.MAX_TEAMMATE_NUM do
                --先停止之前的动作
                if self["__team_model"..i] then
                    CPPActionManager.StopAllActions(self["__team_model"..i]:GetNode())
                end   
            end
            --初始化
            self:__InitTeamMate(true)
        end
    end)

    --任务剩余时间
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_REMAIN_TIME, function(_,remain_time)
        self:TaskRemainCountDown(remain_time)
    end)  

    --任务时间到了要去打BOSS
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_TASK_TIME_TO_BOSS, function()
        --时间未到或者任务数未满
        if not self.__is_time_to_boss then
            self.__is_time_to_boss = true    
            self.__is_finish_task_link = false   
            --提示
            GlobalTipMsg.GetInstance():Show(self.__config_tasklink.time_end_tips or "")    
            --模型
            for i=1,Macros.Game.MAX_TEAMMATE_NUM do
                if self["__team_model"..i] then
                    --先停止之前的动作
                    CPPActionManager.StopAllActions(self["__team_model"..i]:GetNode())                     
                end
            end
            if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.QUESTION then
                --答题                      
                --答题计时器
                if self.__question_count_down_timer then
                    GlobalTimerQuest:CancelQuest(self.__question_count_down_timer)
                    self.__question_count_down_timer = nil
                end
                if self.__panel_question then
                    self.__panel_question:SetVisible(false)
                end
            elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.CONTENT then
                --对话
                --结束剧情
                CallModuleFunc(ModuleType.STORY, "Stop")
            end
            self:SetNewTaskContent()
        end
    end) 

    --飞能量到法阵
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_FLY_TO_ENERGY_ARRAY, function()     
        self:TeamDungeonFlyToEnergyArray()        
    end) 

    --快捷聊天
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_QUICK_CHAT_RESP, function(_,protocol_data)     
        self:ShowChatContent(protocol_data)
        --缩小效果
        if self.__is_own_chat then
            self:OpenQuickChatView()
            --禁止打开
            --倒计时
            if not self.__chat_count_down_timer then                  
                self.__chat_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
                function()
                    self.__chat_count_down_num = self.__chat_count_down_num - 1
                    self.__btn_chat:SetText(self.__chat_count_down_num == 0 and "" or self.__chat_count_down_num)
                    self.__btn_chat:SetEnabled(self.__chat_count_down_num == 0 and true or false)
                    if self.__chat_count_down_num == 0 then
                        --对话计时器
                        if self.__chat_count_down_timer then
                            GlobalTimerQuest:CancelQuest(self.__chat_count_down_timer)
                            self.__chat_count_down_timer = nil
                        end 
                    end
                end), 1, -1)
            end 
            self.__chat_count_down_num = 5
            self.__btn_chat:SetText(5)
            self.__btn_chat:SetEnabled(false)  
        end              
    end) 

    --BOSS结算奖励
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_BATTLE_RESULT, function()     
        --结束剧情
        CallModuleFunc(ModuleType.STORY, "Stop")
        --进入组队大厅
        CallModuleFunc(ModuleType.SCENE, "EnterSceneReq", Macros.Game.SceneType.TEAM_HALL, 1)     
    end) 
    
end

function Modules.TeamWorldMapView:__Dispose()
    if self.__chat_scrollview then
        self.__chat_scrollview:DeleteMe()
        self.__chat_scrollview = nil
    end
    --答题计时器
    if self.__question_count_down_timer then
        GlobalTimerQuest:CancelQuest(self.__question_count_down_timer)
        self.__question_count_down_timer = nil
    end 
    --对话计时器
    if self.__chat_count_down_timer then
        GlobalTimerQuest:CancelQuest(self.__chat_count_down_timer)
        self.__chat_count_down_timer = nil
    end 
    --模型
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do
        if self["__team_model"..i] then
            self["__team_model"..i]:DeleteMe()
            self["__team_model"..i] = nil
        end
    end
    --NPC模型
    if self.__npc_model then
        self.__npc_model:DeleteMe()
        self.__npc_model = nil
    end
    --BOSS模型
    if self.__team_boss_model then
        self.__team_boss_model:DeleteMe()
        self.__team_boss_model = nil
    end
end

function Modules.TeamWorldMapView:__ShowUI()
    --队伍任务数据
    self.__team_dungeon_task_data = self:CallModuleFunc("GetTeamDungeonTaskData")
    --获取当前任务数据
    self.__new_task_data = self:CallModuleFunc("GetCurTaskData")
    if not self.__team_dungeon_task_data or not self.__new_task_data.task_id then
        return
    end    
    --任务链配置
    self.__config_tasklink = CPPGameLib.GetConfig("TeamDungeonTaskLink",self.__team_dungeon_task_data.task_link_id)  
    --副本配置
    self.__config_dungeon = CPPGameLib.GetConfig("TeamFBChapter",self.__team_dungeon_task_data.dungeon_id)     
    --邀请背景
    self.__image_invitebg = self:GetWidget("Image_InviteBg")
    --特效   
    local effect_id,effect = self:__CreateEffect({res_name = "UI_zudui_mubiao",loop = true},self.__image_invitebg)
    PosTool.Center(effect)
    --任务链内容
    if not self.__config_tasklink and not self.__config_dungeon then
        return
    end     
    self:__InitTeamMate()
    --任务内容
    self.__task_list_content = Util:RichText("", Macros.TypefaceSize.normal, 0, 0, Macros.Color.golden_hex)
    self.__task_list_content:SetAnchorPoint(0,0.5)
    self.__image_invitebg:AddChild(self.__task_list_content)
    self.__task_list_content:SetPosition(10.5,84)
    --当前内容
    self.__text_curtask = Util:RichText("", Macros.TypefaceSize.normal, 305, 48, Macros.Color.btn_hex)
    self.__text_curtask:SetAnchorPoint(0,1)
    self.__image_invitebg:AddChild(self.__text_curtask)
    self.__text_curtask:SetPosition(10.5,58)
    --设置新任务内容
    --是否要去打Boss
    self.__is_time_to_boss = self:CallModuleFunc("GetIsTimeToBoss")
    if not self.__is_time_to_boss then
        --任务数量达到上限也会去打BOSS
        self.__is_finish_task_link = CallModuleFunc(ModuleType.TEAM,"GetIsFinishTaskLink")  
    else
        --提示
        GlobalTipMsg.GetInstance():Show(self.__config_tasklink.time_end_tips or "") 
    end
    self:SetNewTaskContent()
    --点击前往
    self.__tick_to_task = Util:RichText(string.format(Macros.Color.team_link,1,CPPGameLib.GetString("team_tick_to_move")), Macros.TypefaceSize.normal, 0, 0, Macros.Color.green_hex)
    self.__tick_to_task:SetAnchorPoint(0.5,0.5)
    self.__image_invitebg:AddChild(self.__tick_to_task)
    self.__tick_to_task:SetPosition(232.5,23)
    self.__tick_to_task:SetVisible(self.__is_own_leader)
    local OnLinkClicked = function (x,y,link_type,user_data,str_url)
        --是否已经前往NPC
        if self.__is_work_to_npc then
            return
        end
        --判断是否队长
        if self.__is_own_leader then 
            --请求移动
            self:CallModuleFunc("TeamDungeonNextNPCReq",{npc = self.__npc_config and self.__npc_config.npc_id or 1})                     
        end
    end  
    local link_func = GlobalCallbackMgr:AddCallBackFunc(OnLinkClicked)
    self.__tick_to_task:SetLuaTouchLink(link_func)
    --快捷聊天容器
    self.__panel_chatscrollview = self:GetWidget("Panel_ChatScrollView")
    --打开快捷聊天
    self.__btn_chat = self:GetWidget("Button_chat")
    Util:WidgetButton(self.__btn_chat, "",function ()
        self:OpenQuickChatView()
    end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
    --适配队长中心位置
    self:__MovingBottomLayer()
    --倒计时
    self.__txt_countdown = self:GetWidget("Text_CountDown")   
    Util:WidgetLabel(self.__txt_countdown, "", Macros.TypefaceSize.normal, Macros.Color.golden)   
    --BOSS
    local gamelevel_config = CPPGameLib.GetConfig("GameLevel",self.__config_tasklink.boss_map_id)
    local boss_config = CPPGameLib.GetConfig("Monster",gamelevel_config.icon_id)
    self.__team_boss_model = LogicGUI.Model.New({parent = self.__hero_layer,model_type = LogicGUI.ModelType.MONSTER,info_id = gamelevel_config.icon_id,scale = boss_config.show_scale,zorder = 960 - self.__config_tasklink.boss_world_map_pos[2],dir = self.__config_tasklink.horz_dir or MathTool.HORZ_LEFT})     
    self.__team_boss_model:SetPosition(self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2])  
    self.__team_boss_model:SetTouchRect(-150, 0, 300, 200)
    self.__team_boss_model:SetTouchFun(nil, nil, function ()
        --是否已经前往NPC
        if self.__is_work_to_npc then
            return
        end
        --判断是否队长
        if self.__is_own_leader and self.__is_finish_task_link then 
            --请求移动
            self:CallModuleFunc("TeamDungeonNextNPCReq",{npc = self.__npc_config and self.__npc_config.npc_id or 1})                     
        end
    end)
    --魔王宝箱
    self.__image_treasurebg = self:GetWidget("Image_TreasureBg")
    --进度条
    self.__box_progress_loadingbar = self:GetWidget("LoadingBar")
    --宝箱
    self.__boss_treasure_box = self:GetWidget("Image_box")
    --宝箱名字
    self.__txt_boxname = self:GetWidget("Text_BoxName")
    Util:WidgetLabel(self.__txt_boxname,"", Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.common_stroke) 
    --BOSS奖励宝箱
    local is_have_energyfly = CallModuleFunc(ModuleType.TEAM,"GetIsHaveEnergyFly")
    --宝箱资源文字
    self.__box_res_list = {[1] = "green_box_close",[2] = "blue_box_close",[3] = "purple_box_close",[4] = "orange_box_close"}
    --能量阶段
    local team_dungeon_before_energy = CallModuleFunc(ModuleType.TEAM,"GetTeamDungeonBeforeEnergy")
    self.__team_dungeon_before_energy = is_have_energyfly and team_dungeon_before_energy or self.__team_dungeon_task_data.energy
    local energy_limit
    self.__team_energy_stage,energy_limit,self.__before_energy_limit = self:FindCurWhichStage(self.__team_dungeon_before_energy)
    --宝箱名字
    self.__txt_boxname:SetText(CPPGameLib.GetString("team_world_map_box_name",self.__team_energy_stage))   
    --设置进度
    self.__box_progress_loadingbar:SetPercent(math.min(1,((self.__team_dungeon_before_energy - self.__before_energy_limit)/(energy_limit - self.__before_energy_limit)))*100)
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__box_res_list[self.__team_energy_stage]) 
    --设置宝箱样式
    self.__boss_treasure_box:SetImage(sp)
    WidgetUnity.SetWidgetListener(self.__boss_treasure_box, nil, nil,function ()
        -- 奖励预览 
        LogicGUI.TemplTeamWorldMapRewardView.New(self.__config_dungeon.show_reward_list[self.__team_energy_stage]) 
    end)
    -- --法阵
    -- local effect
    -- self.__gashaponmachine_effect_id, effect = self:__CreateEffect({res_name = "UI_erengy_array", type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE},self.__hero_layer,3)
    -- effect:SetPosition(self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2])
    -- --获取内对象
    -- local inner_effect = GlobalEffectMgr:GetInnerEffect(self.__gashaponmachine_effect_id)
    -- inner_effect:SetOpacity(255*(0.3 + 0.7*(math.min(1,self.__team_dungeon_task_data.energy/self.__config_dungeon.energy_limit[#self.__config_dungeon.energy_limit]))))
    --屏蔽层
    self.__panel_shield = self:GetWidget("Panel_shield")
    --队长提示
    self.__is_first_enter_scene = self:CallModuleFunc("GetIsFirstEnterScene")
    if self.__is_first_enter_scene then
        self.__team_boss_model:GetNode():SetVisible(false)
        self:CallModuleFunc("SetIsFirstEnterScene")
        self.__panel_shield:SetVisible(true)
        --提示
        GlobalTipMsg.GetInstance():Show(self.__is_own_leader and GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_become_the_leader_tips")) or CPPGameLib.GetString("team_become_teammate_tips",self.__team_dungeon_task_data.member_list[1].server_id,
                        self.__team_dungeon_task_data.member_list[1].role_name))
        --入场动画
        self:__FirstEnterAnimation()
    else
        --BOSS特效
        if self.__config_tasklink.body_effect then
            --身上特效
            local effect_id, effect = self:__CreateEffect({id = self.__config_tasklink.body_effect,loop = true},self.__hero_layer,960 - self.__config_tasklink.boss_world_map_pos[2] + 1)
            effect:SetPosition(self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2])
        end
        if self.__config_tasklink.back_effect then
            --背后特效
            local effect_id, effect = self:__CreateEffect({id = self.__config_tasklink.back_effect,loop = true},self.__hero_layer,960 - self.__config_tasklink.boss_world_map_pos[2] - 1)
            effect:SetPosition(self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2])
        end
    end
    --宝箱升级function
    self.__box_treasure_level_up = function ()
        local t_posx = self.__boss_treasure_box:GetPositionX()
        local t_posy = self.__boss_treasure_box:GetPositionY()
        --升级特效
        local effect_id, effect = self:__CreateEffect({res_name = "UI_zudui_fazhen_shengji",loop = false,scale = 0.8},self.__image_treasurebg,2)
        effect:SetPosition(t_posx,t_posy-30)
        --升级特效1
        local effect_id, effect = self:__CreateEffect({id = 100086,loop = false,scale = 0.8},self.__image_treasurebg,3)
        effect:SetPosition(t_posx,t_posy-30)
        --宝箱变化
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__box_res_list[self.__team_energy_stage]) 
        self.__boss_treasure_box:SetImage(sp)  
        --宝箱名字
        self.__txt_boxname:SetText(CPPGameLib.GetString("team_world_map_box_name",self.__team_energy_stage)) 
    end
    --是否飞入法阵能量
    if is_have_energyfly then               
        self:TeamDungeonFlyToEnergyArray()  
    end
    --倒计时
    local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local remain_time = (self.__team_dungeon_task_data.task_start_time + self.__config_tasklink.task_time) - server_time
    self:TaskRemainCountDown(remain_time)  
    --答题容器
    self.__panel_question = self:GetWidget("Panel_Question")  
end

function Modules.TeamWorldMapView:__FirstEnterAnimation() 
    --入场动画
    self.__animation_layout = Util:Layout(10, 10)
    self.__hero_layer:AddChild(self.__animation_layout)
    self.__animation_layout:SetPosition(self.__team_dungeon_task_data.member_list[1].pos_x,self.__team_dungeon_task_data.member_list[1].pos_y)
    self.__update_move = true
    --先将视野从队长移动到BOSS身上
    self.__team_boss_model:GetNode():SetVisible(true)
    self.__team_boss_model:PlaySkillAnim(self.__config_tasklink.boss_skill_id)
    CPPActionManager.MoveTo(self.__animation_layout,2,self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2]+150,GlobalCallbackMgr:AddCallBackFunc(function ()
               
    end))
    --播放BOSS技能效果和动作             
    self.__team_boss_model:SetEndCallback(function ()
        --BOSS特效
        if self.__config_tasklink.body_effect then
            --身上特效
            local effect_id, effect = self:__CreateEffect({id = self.__config_tasklink.body_effect,loop = true},self.__hero_layer,960 - self.__config_tasklink.boss_world_map_pos[2] + 1)
            effect:SetPosition(self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2])
        end
        if self.__config_tasklink.back_effect then
            --背后特效
            local effect_id, effect = self:__CreateEffect({id = self.__config_tasklink.back_effect,loop = true},self.__hero_layer,960 - self.__config_tasklink.boss_world_map_pos[2] - 1)
            effect:SetPosition(self.__config_tasklink.boss_world_map_pos[1],self.__config_tasklink.boss_world_map_pos[2])
        end
        --再将视野从BOSS移动到队长身上
        CPPActionManager.MoveTo(self.__animation_layout,0.5,self.__team_dungeon_task_data.member_list[1].pos_x,self.__team_dungeon_task_data.member_list[1].pos_y,GlobalCallbackMgr:AddCallBackFunc(function ()
            self.__enter_scene_label = Util:RichText(self.__config_tasklink.task_content, Macros.TypefaceSize.normal, 0, 0, Macros.Color.golden_hex)
            self:GetNode():AddChild(self.__enter_scene_label,960)
            self.__enter_scene_label:SetPosition(Macros.Global.DesignSize.width/2,Macros.Global.DesignSize.height/2)
            self.__enter_scene_label:SetScaleFactor(2)
            --停留0.5秒
            CPPActionManager.DelayTo(self.__animation_layout,0.5,GlobalCallbackMgr:AddCallBackFunc(function ()
                local target_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__image_invitebg, self:GetNode(), COCOPoint(10.5,84))
                --飞过去左上角
                ActionManager.GetInstance():RunScaleToAndMoveToEffectEx(self.__enter_scene_label,0.5,target_pos.x,target_pos.y,0.3,function ()
                    self.__enter_scene_label:RemoveSelf(true)
                    self.__enter_scene_label = nil
                    self.__is_first_enter_scene = false
                    self.__panel_shield:SetVisible(false)
                end)
            end))
        end))
    end)
end

function Modules.TeamWorldMapView:__CreateLayers() 
    --背景层
    self.__bottom_layer_params = {is_image_path = true,image_anchor = {x = 0,y = 0}}
    self.__bottom_layer = TeamWorldMapBG.New(self.__bottom_layer_params)                    
    self:AddChild(self.__bottom_layer, Macros.Global.z_order_bottom)    
    --人物层
    self.__hero_layer = TeamWorldMapBG.New({image_anchor = {x = 0,y = 0},func = function ()
        if not self.__is_own_leader then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_mate_touch_tips"))
        end
    end})  
    self.__hero_layer:SetContentSize(self.__bottom_layer:GetWidth(),self.__bottom_layer:GetHeight())
    self:AddChild(self.__hero_layer, Macros.Global.z_order_bottom + 1)  
end

--初始化队员列表
function Modules.TeamWorldMapView:__InitTeamMate(is_recover_move)
    --队伍背景
    if not self.__image_teammatebg then
        self.__image_teammatebg = self:GetWidget("Image_TeamMateBg")
    end
    local org_pos = 200
    local common_zoom = ConfigAdapter.Common.GetTeamWorldMapHeroZoom()
    --角色id
    CPPGameLib.PrintTable("ldx",self.__team_dungeon_task_data.member_list,"self.__team_dungeon_task_data.member_list22222222")
    local role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleID")
    self.__is_own_leader = role_id == self.__team_dungeon_task_data.member_list[1].role_id
    --队伍位置
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do   
        local is_self = false    
        --队员容器
        self["__panel_pos"..i] = self:GetWidget("Panel_Pos"..i)       
        --队长标记
        if i == 1 then
            self:GetWidget("Image_LeaderTag"):SetZOrder(1)
        end
        --聊天背景
        self["__image_chatbg"..i] = self:GetWidget("Image_ChatBg"..i)
        --聊天内容
        self["__text_chat"..i] = self:GetWidget("Text_Chat"..i) 
        Util:WidgetLabel(self["__text_chat"..i], "", Macros.TypefaceSize.normal, Macros.Color.btn) 
        self["__panel_pos"..i]:SetVisible(self.__team_dungeon_task_data.member_list[i] and true or false) 
        if self.__team_dungeon_task_data.member_list[i] then
            --自己在队伍中的位置
            if self.__team_dungeon_task_data.member_list[i].role_id == role_id then
                self.__in_team_pos = i
                is_self = true
            end
            --头像
            local sp = Resource.PathTool.GetHeroHeadPath(self.__team_dungeon_task_data.member_list[i].head_icon)
            if not self["__panel_icon_pos"..i] then                
                --头像
                self["__panel_icon_pos"..i] = Util:Sprite(sp)
                self["__panel_pos"..i]:AddChild(self["__panel_icon_pos"..i])
                PosTool.Center(self["__panel_icon_pos"..i])
            end
            self["__panel_icon_pos"..i]:SetImage(sp)
            --创建模型位置
            local config = CPPGameLib.GetConfig("Hero",self.__team_dungeon_task_data.member_list[i].head_icon)
            self.__team_dungeon_task_data.member_list[i].pos_x = self.__team_dungeon_task_data.member_list[i].pos_x or (self.__config_tasklink.player_map_pos[1] + (i-1)*self.__config_tasklink.player_map_spacing[1])
            self.__team_dungeon_task_data.member_list[i].pos_y = self.__team_dungeon_task_data.member_list[i].pos_y or (self.__config_tasklink.player_map_pos[2] + (i-1)*self.__config_tasklink.player_map_spacing[2])
            print("skfb","self.__team_dungeon_task_data.member_list[i].pos_x ",self.__team_dungeon_task_data.member_list[i].pos_x,self.__team_dungeon_task_data.member_list[i].pos_y)
            local oppose_zoom = 1.5/config.city_scale
            if config and not self["__team_model"..i] then               
                self["__team_model"..i] = LogicGUI.Model.New({parent = self.__hero_layer,info_id = self.__team_dungeon_task_data.member_list[i].head_icon,zorder = 960 - self.__team_dungeon_task_data.member_list[i].pos_y})    
                --角色名字
                self["__team_role_name"..i] = Util:Label(is_self and "" or self.__team_dungeon_task_data.member_list[i].role_name, Macros.TypefaceSize.minimum,Macros.Color.btn,0,0,Macros.Color.common_stroke)
                self["__team_model"..i]:GetModel():AddChild(self["__team_role_name"..i],RenderObj.InnerLayer.ALL_KIND_OF_TIPS)                                    
                self["__team_role_name"..i]:SetPosition(0,org_pos) 
            elseif config and self["__team_model"..i] then   
                self["__team_model"..i]:SetInfoID(self.__team_dungeon_task_data.member_list[i].head_icon)               
                --角色名字
                self["__team_role_name"..i]:SetString(is_self and "" or self.__team_dungeon_task_data.member_list[i].role_name)
            end  
            self["__team_role_name"..i]:SetScaleFactor(oppose_zoom)               
            self["__team_model"..i]:SetScaleFactor(config.city_scale * common_zoom)   
            self["__team_model"..i]:SetPosition(self.__team_dungeon_task_data.member_list[i].pos_x,self.__team_dungeon_task_data.member_list[i].pos_y) 
           
           --队长标识
            if i == 1 and not self.__own_leader_tag_effect then
                local effect_id
                effect_id,self.__own_leader_tag_effect = self:__CreateEffect({res_name = "UI_zuduiduizhang",loop = true,scale = 2},self["__team_model"..i]:GetNode(),RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
                self.__own_leader_tag_effect:SetPosition(0,org_pos-250) 
            end
            --自己的标识
            if is_self and not self["__team_myself_tag"..i] then
                local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "glow_arrows")
                self["__team_myself_tag"..i] = Util:Sprite(sp)
                self["__team_model"..i]:GetModel():AddChild(self["__team_myself_tag"..i],RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
                self["__team_myself_tag"..i]:SetRotation(90)                
                self["__team_myself_tag"..i]:SetPosition(0,org_pos) 
            end
            --显隐
            if self["__team_myself_tag"..i] then 
                self["__team_myself_tag"..i]:SetVisible(is_self)   
                self["__team_myself_tag"..i]:SetScaleFactor(oppose_zoom)             
            end
        else
            if self["__team_model"..i] then
                self["__team_model"..i]:DeleteMe()
                self["__team_model"..i] = nil
            end 
            self["__team_myself_tag"..i] = nil
            self["__team_role_name"..i] = nil           
        end   
    end
    --队长先移动
    if self.__team_model1 and is_recover_move and self.__update_move then                                                
        --更新队伍移动
        self:__UpdateTeamMoving()        
    end
end

--打开快捷聊天
function Modules.TeamWorldMapView:OpenQuickChatView()
    if not self.__chat_scrollview then
        local params = {
            item_class = Modules.TemplQuickChatCell,
            item_width = 224,
            item_height = 33,
            row = 5,
            col = 1,
            item_space_r = 0,
            view_width = 224,
            view_height = 165,
            touchable = true,
            touch_callback = function (index,data)
                -- 请求发送快捷聊天
                CallModuleFunc(ModuleType.TEAM,"TeamDungeonShortCutChatReq",data.index)              
            end,
        }
        self.__chat_scrollview = GUI.ScrollView.New(params)
        self.__panel_chatscrollview:AddChild(self.__chat_scrollview:GetNode())
        PosTool.Center(self.__chat_scrollview)
        --读取快捷聊天表
        self.__tml_chat_list = {}
        for i,chat_info in ipairs(CPPGameLib.GetConfig("TeamDungeonQuickChat",nil,nil,true)) do
            table.insert(self.__tml_chat_list,{txt = chat_info.text,index = i})
        end
        self.__chat_scrollview:SetDataList(self.__tml_chat_list)
        self.__panel_chatscrollview:SetScaleFactor(0.3)
        self.__is_opening_quickchat = true
    end   
    self.__is_opening_quickchat = not self.__is_opening_quickchat
    --当前开启
    if self.__is_opening_quickchat then
        --缩小效果
        CPPActionManager.ScaleTo(self.__panel_chatscrollview, scale_to_time, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
            self.__panel_chatscrollview:SetVisible(false)
        end))
    else
        --放大效果    
        self.__panel_chatscrollview:SetVisible(true)
        CPPActionManager.ScaleTo(self.__panel_chatscrollview,scale_to_time,1)  
    end    
end

--任务剩余时间倒计时
function Modules.TeamWorldMapView:TaskRemainCountDown(remain_time)   
    --倒计时
    if remain_time > 0 then
        local show_txt = TimeTool.TransTimeStamp("MM:SS",remain_time)
        self.__txt_countdown:SetText(CPPGameLib.GetString("team_task_retain_time",show_txt))
    else
        self.__txt_countdown:SetText(CPPGameLib.GetString("team_to_fight_boss"))
        --超时提示
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_to_fight_boss"))        
    end  
end

--显示对话内容
function Modules.TeamWorldMapView:ShowChatContent(data,content)   
    if not data then
        return
    end
    local index,content_text
    if content then
        index = data
        content_text = content
    else    
        local content_info = CPPGameLib.GetConfig("TeamDungeonQuickChat",data.chat_id)
        content_text = content_info.text
        for i=1,4 do
            if self.__team_dungeon_task_data.member_list[i].role_id == data.role_id then
                index = i
                break
            end
        end
    end   
    self.__is_own_chat = self.__in_team_pos == index
    self["__image_chatbg"..index]:SetVisible(false)
    --设置文字内容
    self["__text_chat"..index]:SetText(content_text)
    if content then
        self["__text_chat"..index]:SetColor(unpack(Macros.Color.golden))
    else
        self["__text_chat"..index]:SetColor(unpack(Macros.Color.btn))
    end
    --设置聊天内容背景
    self["__image_chatbg"..index]:SetContentWidth(self["__text_chat"..index]:GetContentWidth()+12)
    self["__image_chatbg"..index]:SetScaleFactor(0.1)
    self["__image_chatbg"..index]:SetVisible(true)
    CPPActionManager.ScaleTo(self["__image_chatbg"..index],scale_to_time,1) 
    --解锁
    CPPActionManager.DelayTo(self["__image_chatbg"..index],5, GlobalCallbackMgr:AddCallBackFunc(function()
        self["__image_chatbg"..index]:SetVisible(false)
    end))
end

--初始化答题相关
function Modules.TeamWorldMapView:__InitQuestion()
    if not self.__init_question then
        self.__init_question = true        
        --答题背景
        self.__image_questionbg = self:GetWidget("Image_QuestionBg")
        --答题题目
        self.__text_questiontitle = self:GetWidget("Text_QuestionTitle")
        self.__text_questiontitle:SetZOrder(2)
        Util:WidgetLabel(self.__text_questiontitle, "", Macros.TypefaceSize.popup,  Macros.Color.headline )
        --答题内容
        self.__text_questioncontent = self:GetWidget("Text_QuestionContent")
        self.__text_questioncontent:SetZOrder(2)
        Util:WidgetLabel(self.__text_questioncontent, "", Macros.TypefaceSize.normal,  Macros.Color.keypoint,472,100)
        --答题倒计时
        self.__text_questioncounttime = self:GetWidget("Text_QuestionCountTime")
        self.__text_questioncounttime:SetZOrder(2)
        Util:WidgetLabel(self.__text_questioncounttime, "", Macros.TypefaceSize.normal, Macros.Color.red)  
        --选项
        for i=1,4 do
            --选项答案
            self["__btn_answer"..i] = self:GetWidget("Button_Answer"..i)
            self["__btn_answer"..i]:SetZOrder(2)
            Util:WidgetButton(self["__btn_answer"..i], "",function ()
                --问题数据
                if not self.__question_config then
                    return
                end
                --答题请求
                self:CallModuleFunc("TeamDungeonAnswerQuestionReq",{is_right = self.__question_config.right_answer == i and 1 or 0,answer = i}) 
                --设置红色选项
                local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_WORLD_MAP, "bg_answer_selected")
                self["__btn_answer"..i]:SetNormalImage(sp)
                self["__text_answercontent"..i]:SetColor(unpack( Macros.Color.Special ))
                --全部都不可点击
                for i=1,4 do
                    self["__btn_answer"..i]:SetTouchEnabled(false)
                end
                --答题计时器
                if self.__question_count_down_timer then
                    GlobalTimerQuest:CancelQuest(self.__question_count_down_timer)
                    self.__question_count_down_timer = nil
                end
                --倒计时
                self.__text_questioncounttime:SetVisible(false)
            end)
            --勾
            self["__image_righttick"..i] = self:GetWidget("Image_RightTick"..i)
            --选项内容
            self["__text_answercontent"..i] = self:GetWidget("Text_AnswerContent"..i)
            Util:WidgetLabel(self["__text_answercontent"..i], "", Macros.TypefaceSize.normal, Macros.Color.keypoint)
        end  
    end
    --问题数据   
    self.__question_config = CPPGameLib.GetConfig("TeamQuestionData",self.__new_task_data.question_id)
    if not self.__question_config or not self.__npc_config then
        return
    end
    --初始化问题和选项
    self.__text_questiontitle:SetText(CPPGameLib.GetString("common_str_with_colon",tostring(self.__npc_config.npc_name)))
    self.__text_questioncontent:SetText(self.__question_config.question_content)
    --选项
    for i=1,4 do
        --设置选项
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_WORLD_MAP, "bg_answer_unselected")
        self["__btn_answer"..i]:SetNormalImage(sp)
        self["__btn_answer"..i]:SetTouchEnabled(true)
        self["__text_answercontent"..i]:SetColor(unpack( Macros.Color.keypoint ))
        --勾
        self["__image_righttick"..i]:SetVisible(false)
        --选项内容
        self["__text_answercontent"..i]:SetText(CPPGameLib.GetString("team_question_answer"..i)..self.__question_config.question_answer[i])
    end
    self.__panel_question:SetVisible(true)
    --NPC半身像
    local res_name = Resource.PathTool.GetHalfHeadPath(self.__npc_config.model_id)
    if not self.__npc_half_head then
        self.__npc_half_head = Util:Sprite(res_name)
        self.__npc_half_head:SetAnchorPoint(1, 0.5)
        self.__npc_half_head:SetFlipX(false)
        self.__image_questionbg:AddChild(self.__npc_half_head,1)
        PosTool.Center(self.__npc_half_head,-120,0)
    else
        self.__npc_half_head:SetImage(res_name)
    end
    --答题计时器
    if self.__question_count_down_timer then
        GlobalTimerQuest:CancelQuest(self.__question_count_down_timer)
        self.__question_count_down_timer = nil
    end 
    self.__question_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            self.__question_count_num = self.__question_count_num + 1
            --答题时间结束
            if self.__question_count_num == ConfigAdapter.Common.GetTeamQuestionTime() then
                self.__text_questioncounttime:SetVisible(false)
                --全部都不可点击
                for i=1,4 do
                    self["__btn_answer"..i]:SetTouchEnabled(false)
                end
                --超时提示
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_question_over_time"))
                --定时器取消
                if self.__question_count_down_timer then
                    GlobalTimerQuest:CancelQuest(self.__question_count_down_timer)
                    self.__question_count_down_timer = nil
                end 
                --答题请求
                self:CallModuleFunc("TeamDungeonAnswerQuestionReq",{is_right = Macros.Global.FALSE,answer = Macros.Global.FALSE}) 
            else
                self.__text_questioncounttime:SetText(CPPGameLib.GetString("team_question_answer_time",ConfigAdapter.Common.GetTeamQuestionTime() - self.__question_count_num))
            end
        end), 1, -1)
    --倒计时
    self.__text_questioncounttime:SetVisible(true)
    self.__text_questioncounttime:SetText(CPPGameLib.GetString("team_question_answer_time",ConfigAdapter.Common.GetTeamQuestionTime()))
    self.__question_count_num = 0   
end

--背景层移动
function Modules.TeamWorldMapView:__MovingBottomLayer()
    if not self.__team_model1 then
        return
    end
    --层次
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do
        if self["__team_model"..i] then
            local model = self["__team_model"..i]:GetModel()
            --获取当前位置
            local cur_posx,cur_posy = model:GetPosition()  
            self["__team_model"..i]:SetZOrder(960 - cur_posy)
        end
    end   
    --队长模型
    local model = self.__team_model1:GetModel()
    local cur_posx,cur_posy = model:GetPosition()
    if self.__is_first_enter_scene and self.__animation_layout then
        cur_posx,cur_posy = self.__animation_layout:GetPosition()
    end  
    local pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__hero_layer:GetNode(), self.__bottom_layer:GetNode(), COCOPoint(cur_posx,cur_posy))
    --如果小于屏幕一半则为屏幕一半
    local actrual_posx,actrual_posy
    if pos.x < Macros.Global.DesignSize.width/2 then
        actrual_posx = Macros.Global.DesignSize.width/2
    else
        actrual_posx = pos.x
    end
    if pos.y < Macros.Global.DesignSize.height/2 then
        actrual_posy = Macros.Global.DesignSize.height/2
    else
        actrual_posy = pos.y
    end
    if actrual_posx >= self.__bottom_layer:GetWidth() - Macros.Global.DesignSize.width/2 then
        actrual_posx = self.__bottom_layer:GetWidth() - Macros.Global.DesignSize.width/2
    end
    if actrual_posy >= self.__bottom_layer:GetHeight() - Macros.Global.DesignSize.height/2 then
        actrual_posy = self.__bottom_layer:GetHeight() - Macros.Global.DesignSize.height/2
    end
    local ground_move_x = Macros.Global.DesignSize.width/2 - actrual_posx
    local ground_move_y = Macros.Global.DesignSize.height/2 - actrual_posy
    --移动速度
    self.__bottom_layer:SetPosition(ground_move_x,ground_move_y)
    self.__hero_layer:SetPosition(ground_move_x,ground_move_y)
end

--英雄移动
function Modules.TeamWorldMapView:__MovingHero(hero_model,pos_x,pos_y,func,is_idle)
    if not hero_model then
        return
    end    
    --先停止之前的动作
    CPPActionManager.StopAllActions(hero_model:GetNode())    
    --是否设置站着
    if is_idle then
        hero_model:PlayAnimation(RenderObj.ActionIndex.IDLE, true)
        return
    end
    --获取模型
    local model = hero_model:GetModel()
    --获取当前位置
    local cur_posx,cur_posy = model:GetPosition()   
    local cur_dir = cur_posx > pos_x and MathTool.HORZ_LEFT or MathTool.HORZ_RIGHT
    local cur_time = math.sqrt(math.pow(cur_posx - pos_x,2)+math.pow(cur_posy - pos_y,2))/ConfigAdapter.Common.GetTeamWorldMapPlayerSpeed()
    --判断方向改变后再重新设置    
    if cur_dir ~= hero_model:GetDir() or hero_model.__body_anim_play_info.act_index == RenderObj.ActionIndex.IDLE then 
        hero_model:SetDir(cur_dir)
        hero_model:PlayAnimation(RenderObj.ActionIndex.RUN, true)
    end
    --是否有回调
    CPPActionManager.MoveTo(hero_model:GetNode(),tonumber(string.format("%.6f", cur_time)),pos_x,pos_y,GlobalCallbackMgr:AddCallBackFunc(function ()
        if func then
            func()
        end
    end))
end  

--更新队伍移动
function Modules.TeamWorldMapView:__UpdateTeamMoving(func)
    --移动完的结束回调
    self.__move_to_callback = function ()       
        self.__update_move = false 
        --先停止运动
        for i=1,Macros.Game.MAX_TEAMMATE_NUM do
            if self["__team_model"..i] then
                self:__MovingHero(self["__team_model"..i],nil,nil,nil,true)
                local model = self["__team_model"..i]:GetModel()
                --获取当前位置
                local cur_posx,cur_posy = model:GetPosition()  
                self["__team_model"..i]:SetZOrder(960 - cur_posy)
                self.__team_dungeon_task_data.member_list[i].pos_x = tonumber(string.format("%.2f",cur_posx))
                self.__team_dungeon_task_data.member_list[i].pos_y = tonumber(string.format("%.2f",cur_posy))
                --特殊移动还原位置
                if self.__special_move then
                    self.__team_dungeon_task_data.member_list[i].pos_x = self.__team_dungeon_task_data.member_list[1].pos_x + (i-1)*self.__config_tasklink.player_map_spacing[1]
                    self.__team_dungeon_task_data.member_list[i].pos_y = self.__team_dungeon_task_data.member_list[1].pos_y + (i-1)*self.__config_tasklink.player_map_spacing[2]
                end                
            end
        end
        --如果是移动到BOSS则不用对话
        if self.__is_time_to_boss or self.__is_finish_task_link then
            --如果是队长则请求打BOSS 
            if self.__is_own_leader then
                CallModuleFunc(ModuleType.TEAM,"TeamDungeonBossTaskStartReq")
            end 
        elseif func and self.__special_move then
            func()
            self.__special_move = false
        else
            local story_id = self.__is_init_task and self.__config_task.task_param2[self.__cur_task_complete_value + 1] or self.__config_task.task_param2[self.__cur_task_complete_value + 2]
            --对话
            CallModuleFunc(ModuleType.STORY, "Start",story_id or 1001 , nil, function()
                --如果是队长则请求对话结束
                if self.__is_own_leader then
                    if self.__is_init_task then                
                        --接任务对话结束请求
                        self:CallModuleFunc("TeamDungeonAcceptTaskReq")
                    else
                        --对话结束请求
                        self:CallModuleFunc("TeamDungeonDialogOverReq")
                    end
                end
            end)
        end             
    end
    if self.__special_move then
        self:__SpecialMovingHeroFunc()
    else
        --判断是否在同一个位置
        --队长模型
        local model = self.__team_model1:GetModel()
        local cur_posx,cur_posy = model:GetPosition()   
        if math.abs(cur_posx - self.__pos_list.pos_x) <= 1 and math.abs(cur_posy - self.__pos_list.pos_y) <= 1 then
            self.__move_to_callback() 
            return
        end
        self:__MovingHeroFunc()
    end
end

--循环功能
function Modules.TeamWorldMapView:__MovingHeroFunc()
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do
        if i ~= 1 and self["__team_model"..i] and self["__team_model"..(i-1)] then           
            self:__MovingHero(self["__team_model"..i],self.__team_dungeon_task_data.member_list[i-1].pos_x,self.__team_dungeon_task_data.member_list[i-1].pos_y,i == 2 and function ()
                self:__CycleFunc()
            end or nil)
        elseif  i == 1 and self["__team_model"..i] then
            self:__MovingHero(self.__team_model1,self.__pos_list.pos_x,self.__pos_list.pos_y,function ()               
                self.__move_to_callback()                
            end)
        end
    end
end

--特殊移动
function Modules.TeamWorldMapView:__SpecialMovingHeroFunc()
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do
        if i ~= 1 and self["__team_model"..i] and self["__team_model"..(i-1)] then           
            self:__MovingHero(self["__team_model"..i],self.__team_dungeon_task_data.member_list[i-1].pos_x,self.__team_dungeon_task_data.member_list[i-1].pos_y,function ()
                --获取当前队员位置
                local model = self["__team_model"..i]:GetModel()
                local cur_posx,cur_posy = model:GetPosition()   
                if math.abs(cur_posx - self.__pos_list.pos_x) <= 1 and math.abs(cur_posy - self.__pos_list.pos_y) <= 1 then
                    --播放特效
                    self:__PlayDisapperEffect(i)
                    --进入场景
                    if i == #self.__team_dungeon_task_data.member_list then
                        self.__move_to_callback()
                    end
                else
                    self:__CycleFunc()
                end                
            end)
        elseif i == 1 and self["__team_model"..i] then
            self:__MovingHero(self.__team_model1,self.__pos_list.pos_x,self.__pos_list.pos_y,function ()                                  
                --播放特效   
                self:__PlayDisapperEffect(i)
            end)
        end
    end
end

--播放特效
function Modules.TeamWorldMapView:__PlayDisapperEffect(i)
    if self.__cur_max_play_num and self.__cur_max_play_num >= i then
        return
    end
    self.__cur_max_play_num = i
    if self["__team_model"..i] then
        local effect_id,effect = self:__CreateEffect({id = 100087,loop = false,scale = 0.5,time_scale = 2},self.__hero_layer,960)
        effect:SetPosition(self.__pos_list.pos_x,self.__pos_list.pos_y)       
        local model = self["__team_model"..i]:GetModel()
        local cur_posx,cur_posy = model:GetPosition()  
        --消失
        self["__team_model"..i]:GetNode():SetVisible(false)
    end
end

--循环功能
function Modules.TeamWorldMapView:__CycleFunc()
    --所有停
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do
        if self["__team_model"..i] then
            --先停止之前的动作
            CPPActionManager.StopAllActions(self["__team_model"..i]:GetNode()) 
            local model = self["__team_model"..i]:GetModel()
            --获取当前位置
            local cur_posx,cur_posy = model:GetPosition()  
            self.__team_dungeon_task_data.member_list[i].pos_x = tonumber(string.format("%.2f",cur_posx))
            self.__team_dungeon_task_data.member_list[i].pos_y = tonumber(string.format("%.2f",cur_posy))
        end
    end
    --特殊移动
    if self.__special_move then
        self:__SpecialMovingHeroFunc()
    else
        self:__MovingHeroFunc()
    end
      
    -- if i == 2 then
    --     self:__MovingHero(self["__team_model"..i],self.__pos_list.pos_x,self.__pos_list.pos_y,function ()
    --         self:__CycleFunc(i)
    --     end)
    -- else
    --     if math.abs(self.__team_dungeon_task_data.member_list[i].pos_x - self.__team_dungeon_task_data.member_list[i-1].pos_x) <= 5 then
    --         local model = self["__team_model"..(i-1)]:GetModel()
    --         --获取当前位置
    --         local cur_posx,cur_posy = model:GetPosition()  
    --         self.__team_dungeon_task_data.member_list[i-1].pos_x = tonumber(string.format("%.2f",cur_posx))
    --         self.__team_dungeon_task_data.member_list[i-1].pos_y = tonumber(string.format("%.2f",cur_posy))
    --     end
    --     self:__MovingHero(self["__team_model"..i],self.__team_dungeon_task_data.member_list[i-1].pos_x,self.__team_dungeon_task_data.member_list[i-1].pos_y,function ()
    --         self:__CycleFunc(i)
    --     end)
    -- end
end

--设置新任务内容
function Modules.TeamWorldMapView:SetNewTaskContent()
    --任务配置
    self.__config_task = CPPGameLib.GetConfig("TeamDungeonTask",self.__new_task_data.task_id)
    CPPGameLib.PrintTable("skfb",self.__config_task,"self.__config_task 111111111111111111111")
    if not self.__config_task then
        return
    end
    --任务链内容
    local content_str = ""
    if not self.__is_time_to_boss and not self.__is_finish_task_link then
        content_str = CPPGameLib.GetString("tower_amount",self.__new_task_data.complete_task_num or 1,self.__config_tasklink.task_num)
    end
    local lab = GUI.RichText.GetColorWord(self.__config_tasklink.task_content..content_str, Macros.Color.golden_hex)
    self.__task_list_content:SetText(lab)
    --根据任务类型来获取总数量
    self.__total_task_num = 0
    self.__cur_task_complete_value = 0
    if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.QUESTION then
        --答题
        self.__total_task_num = 1
    elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.GATHER then
        --采集
        self.__total_task_num = self.__config_task.task_param1[2]   
    elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.BATTLE_MONSTER then
        --打怪
        self.__total_task_num = self.__config_task.task_param1[2]   
    elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.CONTENT then
        --对话
        self.__total_task_num = #self.__config_task.task_param1
    end
    --判断是否借取任务
    if not self.__is_time_to_boss and not self.__is_finish_task_link then
        local is_accept_task = CallModuleFunc(ModuleType.TEAM,"GetIsHaveAcceptTask")
        if is_accept_task then
            self:RunFunctionByTaskType()
        else
            --任务内容
            self:UpdateTaskCompleteValue(true)
        end
    else
        --任务内容
        self:UpdateTaskCompleteValue(true)
    end   
end

--更新当前任务进度
function Modules.TeamWorldMapView:UpdateTaskCompleteValue(init_task,accept_task) 
    --NPC模型
    if self.__npc_model then
        self.__npc_model:DeleteMe()
        self.__npc_model = nil
    end
    if self.__jump_arrow then
        self.__jump_arrow:SetVisible(false)
    end
    if self.__npc_name then
        self.__npc_name:SetVisible(false)
    end
    if self.__talk_bubble then
        self.__talk_bubble:SetVisible(false)
    end
    --讨伐BOSS 
    if self.__is_time_to_boss or self.__is_finish_task_link then
        local lab = GUI.RichText.GetColorWord(self.__config_tasklink.boss_task_content, Macros.Color.btn_hex)
        self.__text_curtask:SetText(lab)
        --BOSS位置
        local x,y = self:CountTeamLeaderMovePosition(true)
        self.__pos_list = {pos_x = x,pos_y = y}
        --完成任务
        if self.__is_finish_task_link then
            self.__is_work_to_npc = false
            return
        end
        --队长先移动
        if self.__team_model1 then
            self.__update_move = true                                                  
            --更新队伍移动
            self:__UpdateTeamMoving()        
        end
        self.__is_work_to_npc = true
        return
    end
    local npc_id
    self.__is_init_task = init_task
    if not init_task and not accept_task then
        self.__cur_task_complete_value = self.__cur_task_complete_value + 1
    end
    if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.CONTENT then
        npc_id = init_task and self.__config_task.task_npc or self.__config_task.task_param1[self.__cur_task_complete_value+1]
    else
        npc_id = self.__config_task.task_npc
    end
    --任务内容
    local lab = GUI.RichText.GetColorWord((self.__is_init_task and self.__config_task.desc or self.__config_task.desc2)..(self.__is_init_task and "" or CPPGameLib.GetString("team_task_num",self.__cur_task_complete_value,self.__total_task_num)), Macros.Color.btn_hex)
    self.__text_curtask:SetText(lab)
    --npc配置
    npc_id = npc_id or self.__config_task.task_param1[#self.__config_task.task_param1]
    self.__npc_config = CPPGameLib.GetConfig("TeamNPCData",npc_id,false)
    print("ldx","npc_idnpc_idnpc_idnpc_id: ",npc_id)
    if not self.__npc_config then
        return
    end
    self.__is_work_to_npc = false
    
    --NPC位置
    local x,y = self:CountTeamLeaderMovePosition()
    self.__pos_list = {pos_x = x,pos_y = y}
    if not self.funId and not self.__jump_arrow then
        local arrow_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_WORLD_MAP,"arrow")
        self.__jump_arrow = Util:Sprite(arrow_sp)
        self.__hero_layer:AddChild(self.__jump_arrow,12)     
        --回调  
        self.funId = GlobalCallbackMgr:AddCallBackFunc(function ()
            ActionManager.GetInstance():RunJumpByAction(self.__jump_arrow,0.5,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
                self:RunAddAction(self.__jump_arrow)
            end))   
        end)
        self:RunAddAction(self.__jump_arrow)
    end
    self.__jump_arrow:SetPosition(self.__npc_config.npc_pos[1],self.__npc_config.npc_pos[2] + 170)
    self.__jump_arrow:SetVisible(true)
    self.__jump_arrow:SetZOrder(960 - self.__npc_config.npc_pos[2] + 2)
    --刷新说话信息
    if not self.__bubble_funid then
        self.__bubble_funid = GlobalCallbackMgr:AddCallBackFunc(function ()
            if self.__talk_bubble then
                CPPActionManager.FadeOut(self.__talk_bubble, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
                     CPPActionManager.DelayTo(self.__talk_bubble,5,GlobalCallbackMgr:AddCallBackFunc(function()
                         self:RunBubbleAction()
                    end)) 
                end))
            end
        end)
    end
    self.__cur_npc_dir = self.__npc_config.npc_pos[1] >= self.__team_dungeon_task_data.member_list[1].pos_x and MathTool.HORZ_LEFT or MathTool.HORZ_RIGHT
    self:RunBubbleAction()
    --NPC名字
    if not self.__npc_name then
        self.__npc_name = Util:Label("", Macros.TypefaceSize.slightly, Macros.Color.orange, 0, 0, Macros.Color.orange_stroke)
        self.__hero_layer:AddChild(self.__npc_name,11)
    end
    self.__npc_name:SetVisible(true)
    self.__npc_name:SetString(self.__npc_config.npc_name or "")
    self.__npc_name:SetPosition(self.__npc_config.npc_pos[1],self.__npc_config.npc_pos[2] + 115)
    self.__npc_name:SetZOrder(960 - self.__npc_config.npc_pos[2] + 1)
    --npc模型
    self.__npc_model = LogicGUI.Model.New({parent = self.__hero_layer,info_id = self.__npc_config.model_id,scale = self.__npc_config.model_scale,zorder = 960 - self.__npc_config.npc_pos[2],model_type = self.__npc_config.model_type,
        dir = self.__cur_npc_dir,direct_get_model = true})
    self.__npc_model:SetPosition(self.__npc_config.npc_pos[1],self.__npc_config.npc_pos[2])
    self.__npc_model:SetTouchRect(-50, 0, 100, 150)
    self.__npc_model:SetTouchFun(nil,nil,function ()
        --是否已经前往NPC
        if self.__is_work_to_npc then
            return
        end
        --判断是否队长
        if self.__is_own_leader then 
            --请求移动
            self:CallModuleFunc("TeamDungeonNextNPCReq",{npc = self.__npc_config and self.__npc_config.npc_id or 1})                     
        end     
    end)
end

function Modules.TeamWorldMapView:RunAddAction(node)
    ActionManager.GetInstance():RunJumpByAction(node,0.5,0,20,0,0,self.funId)
end

--气泡
function Modules.TeamWorldMapView:RunBubbleAction()
    if self.__talk_bubble then
        CPPActionManager.StopAllActions(self.__talk_bubble)
    end
    self:ShowTalk()
    -- local randomNum = MathTool.GetRandom(5,10)
    CPPActionManager.DelayTo(self.__talk_bubble,3,self.__bubble_funid)  
end

--气泡文字
function Modules.TeamWorldMapView:ShowTalk()
    if not self.__talk_bubble then
        --气泡
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "lead_tips_bg")
        self.__talk_bubble = Util:Sprite(sp)        
        self.__talk_bubble:SetAnchorPoint(1,0)
        self.__hero_layer:AddChild(self.__talk_bubble,15)        
        --气泡文字
        self.__talk_bubble_txt = Util:Label("", 32, Macros.Color.keypoint,270,125)
        self.__talk_bubble_txt:SetAnchorPoint(1,0.5)
        self.__talk_bubble_txt:SetTextVertiacalAlign(LabelEx.ALIGIN_CENTER)
        self.__talk_bubble:AddChild(self.__talk_bubble_txt)
        PosTool.RightCenter(self.__talk_bubble_txt,-10,0)
    end
    self.__talk_bubble:SetFlipX(self.__cur_npc_dir == MathTool.HORZ_LEFT and true or false)
    self.__talk_bubble:SetAnchorPoint(self.__cur_npc_dir == MathTool.HORZ_LEFT and 1 or 0,0)
    self.__talk_bubble:SetPosition(self.__cur_npc_dir == MathTool.HORZ_LEFT and (self.__npc_config.npc_pos[1] - 50) or (self.__npc_config.npc_pos[1] + 30),self.__npc_config.npc_pos[2] + 80)
    self.__talk_bubble:SetZOrder(960 - self.__npc_config.npc_pos[2] + 4)
    self.__talk_bubble:SetVisible(true) 
    self.__talk_bubble:SetOpacity(0)  
    --随机文字
    local randomNum = MathTool.GetRandom(1,#self.__npc_config.bubble_str_list)
    self.__talk_bubble_txt:SetString(self.__npc_config.bubble_str_list[randomNum] or "")    
    self.__talk_bubble:SetScaleFactor(0.1)
    self.__talk_bubble:SetOpacity(255)
    CPPActionManager.ScaleTo(self.__talk_bubble,0.3,0.5,nil)  
end

--飞入法阵充能
function Modules.TeamWorldMapView:TeamDungeonFlyToEnergyArray()
    CallModuleFunc(ModuleType.TEAM,"SetIsHaveEnergyFly")
    local t_posx = self.__boss_treasure_box:GetPositionX()
    local t_posy = self.__boss_treasure_box:GetPositionY()
    --模型
    for i=1,Macros.Game.MAX_TEAMMATE_NUM do
        if self.__team_dungeon_task_data.member_list[i] and self["__team_model"..i] then
            --转换坐标
            local translate_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__hero_layer:GetNode(),self.__image_treasurebg,COCOPoint(self.__team_dungeon_task_data.member_list[i].pos_x,self.__team_dungeon_task_data.member_list[i].pos_y + 50))
            --粒子特效
            self["__array_effect_id"..i],effect = self:__CreateEffect({res_name = "UI_zudui_fazhen_nengliang",loop = true},self.__image_treasurebg,960)               
            effect:SetPosition(translate_pos.x,translate_pos.y)
            --特效移动
            CPPActionManager.MoveTo(effect,0.8,translate_pos.x,translate_pos.y + 100,GlobalCallbackMgr:AddCallBackFunc(function ()
                local cur_effect = GlobalEffectMgr:GetEffect(self["__array_effect_id"..i])
                --延时飞行
                CPPActionManager.DelayTo(cur_effect,1.2,GlobalCallbackMgr:AddCallBackFunc(function ()
                    local cur_effect = GlobalEffectMgr:GetEffect(self["__array_effect_id"..i])
                    --转换坐标
                    -- local translate_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__image_treasurebg,self.__hero_layer:GetNode(),COCOPoint(t_posx,t_posy))
                    CPPActionManager.MoveTo(cur_effect,1.5,t_posx,t_posy,GlobalCallbackMgr:AddCallBackFunc(function ()
                        --删除
                        self:__DelEffect(self["__array_effect_id"..i])
                        self["__array_effect_id"..i] = nil
                        --发光
                        local effect_id, effect = self:__CreateEffect({id = 100083},self.__image_treasurebg,1)
                        effect:SetPosition(self.__box_progress_loadingbar:GetPositionX(),self.__box_progress_loadingbar:GetPositionY())                                         
                        if i == 1 then
                            --进度变化
                            self.__team_dungeon_before_energy = CallModuleFunc(ModuleType.TEAM,"GetTeamDungeonBeforeEnergy")
                            local cur_stage
                            cur_stage,self.__cur_energy_limit,self.__before_energy_limit = self:FindCurWhichStage(self.__team_dungeon_before_energy)
                            --loadingbar速度
                            self.__smooth_cur_speed = (self.__cur_energy_limit - self.__before_energy_limit)/progress_to_time
                            self.__smooth_elapse_time = 0   
                            print("ldx","KKKKKKKKKKKKKKKKKKKKKKKKK: ",self.__team_dungeon_before_energy,self.__cur_energy_limit,self.__team_dungeon_task_data.energy)                         
                            --变化
                            local cur_stage,energy_limit = self:FindCurWhichStage(self.__team_dungeon_task_data.energy)
                            if cur_stage > self.__team_energy_stage then
                                self.__team_energy_stage = cur_stage       
                                self.__is_change_anther_progress = true                             
                                self.__smooth_end_time = CPPGameEngine:GetNowTime() + (self.__cur_energy_limit - self.__team_dungeon_before_energy)/self.__smooth_cur_speed                                                             
                            else                                                                                                     
                                self.__smooth_end_time = CPPGameEngine:GetNowTime() + (self.__team_dungeon_task_data.energy - self.__team_dungeon_before_energy)/self.__smooth_cur_speed
                            end
                                                       
                            -- --法阵浮现
                            -- if self.__gashaponmachine_effect_id then
                            --     local inner_effect = GlobalEffectMgr:GetInnerEffect(self.__gashaponmachine_effect_id)
                            --     if inner_effect then
                            --         inner_effect:SetOpacity(255*(0.3 + 0.7*(math.min(1,self.__team_dungeon_task_data.energy/self.__config_dungeon.energy_limit[#self.__config_dungeon.energy_limit]))))
                            --     end
                            -- end             
                        end
                    end))
                end))
            end))            
        end
    end
end

--寻找当前法阵能量的阶段
function Modules.TeamWorldMapView:FindCurWhichStage(cur_energy_num)
    local cur_stage = 1
    local energy_limit
    for i,energy_num in ipairs(self.__config_dungeon.energy_limit) do
        if cur_energy_num < energy_num then
            energy_limit = energy_num
            break
        end
        cur_stage = cur_stage + 1
    end
    energy_limit = energy_limit or self.__config_dungeon.energy_limit[#self.__config_dungeon.energy_limit]
    return cur_stage,energy_limit,self.__config_dungeon.energy_limit[cur_stage - 1] or 0
end

--计算移动位置
function Modules.TeamWorldMapView:CountTeamLeaderMovePosition(is_boss)
    local x = 0
    local y = 0
    local traget_pos = is_boss and self.__config_tasklink.boss_world_map_pos or self.__npc_config.npc_pos
    if traget_pos and self.__team_dungeon_task_data.member_list[1] then
        if math.pow((math.pow((traget_pos[1]-self.__team_dungeon_task_data.member_list[1].pos_x),2)+ math.pow((traget_pos[2]-self.__team_dungeon_task_data.member_list[1].pos_y),2)),0.5) ~= 0 then
            x = traget_pos[1] - 100*(traget_pos[1] - self.__team_dungeon_task_data.member_list[1].pos_x)/math.pow((math.pow((traget_pos[1]-self.__team_dungeon_task_data.member_list[1].pos_x),2)+ math.pow((traget_pos[2]-self.__team_dungeon_task_data.member_list[1].pos_y),2)),0.5)
        else
            x = traget_pos[1]
        end
        if math.pow((math.pow((traget_pos[1]-self.__team_dungeon_task_data.member_list[1].pos_x),2)+ math.pow((traget_pos[2]-self.__team_dungeon_task_data.member_list[1].pos_y),2)),0.5) ~= 0 then
            y = traget_pos[2] - 100*(traget_pos[2] - self.__team_dungeon_task_data.member_list[1].pos_y)/math.pow((math.pow((traget_pos[1]-self.__team_dungeon_task_data.member_list[1].pos_x),2)+ math.pow((traget_pos[2]-self.__team_dungeon_task_data.member_list[1].pos_y),2)),0.5)
        else
            y = traget_pos[2]
        end       
    end
    return math.floor(x),math.floor(y)
end

--计算移动位置
function Modules.TeamWorldMapView:RunFunctionByTaskType()
    if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.GATHER then
        print("skfb","RunFunctionByTaskTypeRunFunctionByTaskTypeRunFunctionByTaskTypeRunFunctionByTaskType ",self.__config_task.scene_id)
        --采集  
        CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_GATHER,self.__config_task.scene_id)
    elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.BATTLE_MONSTER then
        --清空打怪
        CallModuleFunc(ModuleType.TEAM, "SetTeamDungeonFightMonsterNum",true)
        --打怪
        CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_NORMAL_ENCOUNTER,self.__config_task.scene_id)
    elseif self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.QUESTION then
        --答题
        self:__InitQuestion()
    end
end

TeamWorldMapBG = TeamWorldMapBG or BaseClass(UILayer)

--[[
@ params：
    必须：
    is_image_path          [string]是否有贴图路径
    image_anchor        [cocopoint]背景图锚点
    func                [function]点击回调
--]]
function TeamWorldMapBG:__init(params)
    self.image_anchor = params.image_anchor or COCOPoint(0.5, 0)

    self:SetAnchorPoint(0, 0)   
    if params.func then
        self:SetTouchEnabled(true)
        self:SetTouchFun(nil, nil, params.func)
    end

    --贴图路径
    local image_node
    if params.is_image_path then        
        for i=1,8 do
            image_node = Util:Sprite(Resource.PathTool.GetUIResPath("bg_team_worldmap/bg_sp_team_worldmap_"..i))
            image_node:SetAnchorPoint(self.image_anchor.x, self.image_anchor.y)
            self:AddChild(image_node) 
            image_node:SetPosition(((i-1)%4)*512,i > 4 and 480 or 0)
        end             
    end
    if image_node then
        self:SetContentSize(1704, 960)    
    end
end

--------------------------------------------------------------------------------------------
--[[
    快捷聊天条目
]]
Modules = Modules or {}
Modules.TemplQuickChatCell = Modules.TemplQuickChatCell or BaseClass(GUI.ScrollItem)

function Modules.TemplQuickChatCell:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplQuickChatCell:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(224,33)
  self.__layout:SetTouchEnabled(false)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplQuickChatCell:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    --创建背景
    if self.__data.index%2 == 0 then
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_WORLD_MAP, "bg_quick_chat2")
        local sprite = Util:Sprite9(sp)
        self.__layout:AddChild(sprite)
        PosTool.Center(sprite)
        sprite:SetContentSize(222,33)       
    end  
    --文字
    local label = Util:Label(self.__data.txt, Macros.TypefaceSize.normal, Macros.Color.btn,210,23)
    label:SetAnchorPoint(0,0.5)
    self.__layout:AddChild(label)
    PosTool.LeftCenter(label,6,0)
end
