
--[[
%% @module: 组队采集和打怪界面
%% @author: ldx
%% @created: 2017-10-30
--]]

Modules = Modules or {}

Modules.TeamGatherAndBattleView = Modules.TeamGatherAndBattleView or BaseClass(BaseView)

function Modules.TeamGatherAndBattleView:__init()
    self.__layout_name = "team_gather_and_battle_view" 

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TEAM_GATHER_AND_BATTLE)
end

function Modules.TeamGatherAndBattleView:__RegistAllEvents()
    --采集进度
    self:BindGlobalEvent(Macros.Event.SceneObj.MT_ITEM_PICK_UP, function()
        if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.GATHER then
        	self:UpdateTaskCompleteValue()
        end
    end)

    --打怪进度
    self:BindGlobalEvent(Macros.Event.SceneObj.MT_MONSTER_KILLED, function()
        if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.BATTLE_MONSTER then
        	self:UpdateTaskCompleteValue()
        end
    end)

    --任务剩余时间
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_REMAIN_TIME, function(_,remain_time)
        self:TaskRemainCountDown(remain_time)
    end) 
end

function Modules.TeamGatherAndBattleView:__OpenCallback()   
    self:__ShowUI()    
end

function Modules.TeamGatherAndBattleView:__ShowUI()
    print("ldx","Modules.TeamGatherAndBattleViewModules.TeamGatherAndBattleViewModules.TeamGatherAndBattleViewModules.TeamGatherAndBattleView: ")
	--队伍任务数据
    self.__team_dungeon_task_data = self:CallModuleFunc("GetTeamDungeonTaskData")
	--获取当前任务数据
    self.__new_task_data = self:CallModuleFunc("GetCurTaskData")
    if not self.__team_dungeon_task_data or not self.__new_task_data.task_id then
        return
    end
    --任务链配置
    self.__config_tasklink = CPPGameLib.GetConfig("TeamDungeonTaskLink",self.__team_dungeon_task_data.task_link_id) 
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
    self.__txt_countdown = self:GetWidget("Text_CountDown")   
    Util:WidgetLabel(self.__txt_countdown, "", Macros.TypefaceSize.normal, Macros.Color.golden) 
    --BOSS
    self.__config_tasklink = CPPGameLib.GetConfig("TeamDungeonTaskLink",self.__team_dungeon_task_data.task_link_id)
    --倒计时
    local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local remain_time = (self.__team_dungeon_task_data.task_start_time + self.__config_tasklink.task_time) - server_time
    self:TaskRemainCountDown(remain_time) 
end

--设置新任务内容
function Modules.TeamGatherAndBattleView:SetNewTaskContent()
    --任务配置
    self.__config_task = CPPGameLib.GetConfig("TeamDungeonTask",self.__new_task_data.task_id)
    if not self.__config_task then
        return
    end
    --任务链内容
    local lab = GUI.RichText.GetColorWord(self.__config_tasklink.task_content..CPPGameLib.GetString("tower_amount",self.__new_task_data.complete_task_num or 1,self.__config_tasklink.task_num), Macros.Color.golden_hex)
    self.__task_list_content:SetText(lab)
    --根据任务类型来获取总数量
    self.__total_task_num = 0
    self.__cur_task_complete_value = 0
    if self.__config_task.type == Macros.Game.TEAM_TASK_TYPE.BATTLE_MONSTER then
        self.__cur_task_complete_value = CallModuleFunc(ModuleType.TEAM,"GetTeamDungeonFightMonsterNum") 
    end   
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
    --任务内容
    self:UpdateTaskCompleteValue(true)
end

--更新当前任务进度
function Modules.TeamGatherAndBattleView:UpdateTaskCompleteValue(init_task)   
    if not init_task then
        self.__cur_task_complete_value = self.__cur_task_complete_value + 1
    end
    --任务内容
    local lab = GUI.RichText.GetColorWord(self.__config_task.desc2..CPPGameLib.GetString("team_task_num",self.__cur_task_complete_value,self.__total_task_num), Macros.Color.btn_hex)
    self.__text_curtask:SetText(lab)
end

--任务剩余时间倒计时
function Modules.TeamGatherAndBattleView:TaskRemainCountDown(remain_time)   
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
