
--[[
%% @module: 组队采集和打怪界面
%% @author: ldx
%% @created: 2017-10-30
--]]

Modules = Modules or {}

Modules.TeamBossBattleView = Modules.TeamBossBattleView or BaseClass(BaseView)

function Modules.TeamBossBattleView:__init()
    self.__layout_name = "team_boss_view" 

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM_BOSS_BATTLE)
end

function Modules.TeamBossBattleView:__RegistAllEvents()
	--血量改变
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_HP_CHANGE, function(_,hp,max_hp)
        self.__boss_battle_data.vo.hp = hp
        self.__boss_battle_data.vo.max_hp = max_hp
        self:SetTreasureBoxOpenStatus()
    end)
end

function Modules.TeamBossBattleView:__OpenCallback()   
    self:__ShowUI()    
    CallModuleFunc(ModuleType.TEAM,"ShowBossRewardView")
end

function Modules.TeamBossBattleView:__Dispose()
    if self.__chat then
        self.__chat:DeleteMe()
        self.__chat = nil
    end

    --定时器取消
    if self.__boss_retain_count_down_timer then
        GlobalTimerQuest:CancelQuest(self.__boss_retain_count_down_timer)
        self.__boss_retain_count_down_timer = nil
    end 
end

function Modules.TeamBossBattleView:__ShowUI()
	--队伍任务数据
    self.__team_dungeon_task_data = self:CallModuleFunc("GetTeamDungeonTaskData")
	--获取当前任务数据
    self.__new_task_data = self:CallModuleFunc("GetCurTaskData")
    self.__boss_battle_data = CallModuleFunc(ModuleType.TEAM,"GetTeamDungeonBossBattleStartData")
    if not self.__team_dungeon_task_data or not self.__new_task_data.task_id then
        return
    end
    --任务链配置
    self.__config_tasklink = CPPGameLib.GetConfig("TeamDungeonTaskLink",self.__team_dungeon_task_data.task_link_id) 
    --副本配置
    self.__config_boss_reward = CPPGameLib.GetConfig("TeamDungeonBossReward",self.__team_dungeon_task_data.dungeon_id) 

    --任务链内容
    if not self.__config_tasklink then
        return
    end
    --邀请背景
    self.__image_invitebg = self:GetWidget("Image_InviteBg")
    --特效   
    local effect_id,effect = self:__CreateEffect({res_name = "UI_zudui_mubiao",loop = true},self.__image_invitebg)
    PosTool.Center(effect)
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
    self:SetNewTaskContent()    
    --倒计时
    self.__txt_countdowntime = self:GetWidget("Text_CountDown")   
    Util:WidgetLabel(self.__txt_countdowntime, "", Macros.TypefaceSize.normal, Macros.Color.golden) 
    --BOSS
    self.__config_tasklink = CPPGameLib.GetConfig("TeamDungeonTaskLink",self.__team_dungeon_task_data.task_link_id)
    --倒计时
    if self.__boss_battle_data.vo.hp > 0 then
        self.__boss_retain_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            --倒计时
            local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
            local remain_time = (self.__boss_battle_data.start_battle_time + ConfigAdapter.Common.GetTeamDungeonBattleTime()) - server_time       
            self:TaskRemainCountDown(remain_time) 
        end), 1, -1)
        local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")    
        local remain_time = (self.__boss_battle_data.start_battle_time + ConfigAdapter.Common.GetTeamDungeonBattleTime()) - server_time
        self:TaskRemainCountDown(remain_time) 
    else
        self:GetWidget("Image_CountDown"):SetVisible(false)
    end
    --聊天窗口(特殊,需要跟俊广沟通)
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
end

--设置新任务内容
function Modules.TeamBossBattleView:SetNewTaskContent()
    CPPGameLib.PrintTable("ldx",self.__boss_battle_data,"self.__boss_battle_data  ")
    --任务配置
    self.__config_task = CPPGameLib.GetConfig("TeamDungeonTask",self.__new_task_data.task_id)
    if not self.__config_task then
        return
    end
    --任务链内容
    local lab = GUI.RichText.GetColorWord(self.__config_tasklink.task_content, Macros.Color.golden_hex)
    self.__task_list_content:SetText(lab)
    --任务内容
    local lab = GUI.RichText.GetColorWord(self.__config_tasklink.boss_task_content, Macros.Color.btn_hex)
    self.__text_curtask:SetText(lab)
    --怪物Boss信息
    self.__panel_enemy = self:GetWidget("Panel_Enemy")
    --怪物Boss名字
    local panel_enemyname = self:GetWidget("Panel_EnemyName")  
    self.__monster_name = Util:RichText(self.__boss_battle_data.vo.name or "", Macros.TypefaceSize.largish,0,0,Macros.Color.orange_hex, Macros.Color.orange_stroke_hex)
    panel_enemyname:AddChild(self.__monster_name)
    self.__monster_name:SetAnchorPoint(1,0.5)
    PosTool.RightCenter(self.__monster_name)
    --怪物Boss头像
    self.__panel_enemyhead = self:GetWidget("Panel_EnemyHead")
    local monster_config = CPPGameLib.GetConfig("Monster",self.__boss_battle_data.vo.info_id)
    local icon = LogicGUI.IconLayer.New(false,false)
    self.__panel_enemyhead:AddChild(icon:GetNode(),-1)
    icon:SetData({id = monster_config.head_icon})
    PosTool.Center(icon)   
    --怪物Boss血量
    self.__text_enemyhp = self:GetWidget("Text_EnemyHP")
    self.__text_enemyhp:SetZOrder(2)
    Util:WidgetLabel(self.__text_enemyhp,"", Macros.TypefaceSize.slightly, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --怪物Boss血条
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM_BOSS_BATTLE, "prog_hp_hero_big")
    self.__monster_hp_bar = GUI.ProgressBar9Layer.New(imgdata_bar, nil, 302, 19)
    self.__monster_hp_bar:SetAnchorPoint(0.5, 0.5)
    self.__monster_hp_bar:SetFlipX(true)    
    self:GetWidget("Panel_EnemyHP"):AddChild(self.__monster_hp_bar:GetNode())   
    PosTool.Center(self.__monster_hp_bar,4,0)

    --宝箱资源文字
    self.__box_res_list = {[1] = "green",[2] = "blue",[3] = "purple",[4] = "orange"}
    
    --宝箱
    for i=1,4 do
        self["__image_box"..i] = self:GetWidget("Image_box"..i)        
    end
    self:SetTreasureBoxOpenStatus()
end

--任务剩余时间倒计时
function Modules.TeamBossBattleView:TaskRemainCountDown(remain_time)   
    --倒计时
    if remain_time > 0 then
        local show_txt = TimeTool.TransTimeStamp("MM:SS",remain_time)
        self.__txt_countdowntime:SetText(CPPGameLib.GetString("team_task_retain_time",show_txt))
    else        
        self:GetWidget("Image_CountDown"):SetVisible(false)
        --定时器取消
        if self.__boss_retain_count_down_timer then
            GlobalTimerQuest:CancelQuest(self.__boss_retain_count_down_timer)
            self.__boss_retain_count_down_timer = nil
        end        
    end  
end

--设置宝箱打开程度
function Modules.TeamBossBattleView:SetTreasureBoxOpenStatus()  
    --血量
    self.__monster_hp_bar:SetProgressMax(self.__boss_battle_data.vo.max_hp)
    self.__monster_hp_bar:SetProgress(self.__boss_battle_data.vo.hp)
    print("ldx","self.__boss_battle_data.vo.hp: ",self.__boss_battle_data.vo.hp)
    --血量文字
    self.__text_enemyhp:SetText(string.format("%.2f%%",100*self.__boss_battle_data.vo.hp/self.__boss_battle_data.vo.max_hp))
    --宝箱状态    
    local k = 0
    for i,dungeon_info in ipairs(self.__config_boss_reward.energy_list) do
        if dungeon_info.damage_rate_min ~= 0 then
            k = k + 1
            local damage_percent = 100 * (self.__boss_battle_data.vo.max_hp - self.__boss_battle_data.vo.hp)/self.__boss_battle_data.vo.max_hp
            local box_str = damage_percent >= dungeon_info.damage_rate_min and "_box_open" or "_box_close"
            if self.__box_res_list[k] then
                self["__image_box"..k]:SetPosition(-189.50 + 311 * (dungeon_info.damage_rate_min/100),118)
                local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__box_res_list[k]..box_str)
                self["__image_box"..k]:SetImage(sp)
            end
        end
    end  
end
