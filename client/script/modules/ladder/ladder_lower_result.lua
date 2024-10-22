
--[[
%% @module: 天梯赛低段位战斗结算界面
%% @author: swordman sue
%% @created: 2017年10月31日
--]]

Modules = Modules or {}

Modules.LadderLowerBattleResult = Modules.LadderLowerBattleResult or BaseClass(BaseView)

local ResultType = 
{
    FAILED = 0,
    SUCCESS = 1,
    PROMPT = 2,
}

function Modules.LadderLowerBattleResult:__init()
	self.__layout_name = "ladder_lower_result"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_ladder")}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__loaded_callback = function()
        self:__LoadedCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER_COMMON)    
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER_LOWER_RESULT)    
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER_LOWER_RESULT, true)   
end

function Modules.LadderLowerBattleResult:__LoadedCallback()
    self.__panel_result = self:GetWidget("Panel_Result")
    self.__panel_prompt = self:GetWidget("Panel_Prompt")
    self.__panel_promptreward = self:GetWidget("Panel_PromptReward")
    self.__panel_info = self:GetWidget("Panel_Info")
    self.__panel_star = self:GetWidget("Panel_Star")
    self.__panel_stage = self:GetWidget("Panel_Stage")
    self.__panel_title_effect = self:GetWidget("Panel_TitleEffect")
    self.__image_level = self:GetWidget("Image_Stage")
    self.__image_level_static = self:GetWidget("Image_Stage_Static")
    self.__image_levelbg_static = self:GetWidget("Image_StageBG_Static")
    self.__image_banner = self:GetWidget("Image_Banner")
    self.__text_continue = Util:WidgetLabel(self:GetWidget("Text_Continue"), CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.headline, Macros.Color.blinkcontent, nil, nil, Macros.Color.blinkcontent_stroke)

    self.__image_level:IgnoreContentAdaptWithSize(true)
end

function Modules.LadderLowerBattleResult:__OpenCallback()
    WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
        if self.__is_finished then
            self:__HandleContinue()
        end
    end)

    local battle_info = self:CallModuleFunc("GetCurInfo")
    self:__SetRetType(battle_info.is_win == Macros.Global.TRUE and ResultType.SUCCESS or ResultType.FAILED)
end

function Modules.LadderLowerBattleResult:__Dispose()
    if self.__prompt_rewards_timer then
        GlobalTimerQuest:CancelQuest(self.__prompt_rewards_timer)
        self.__prompt_rewards_timer = nil
    end
end

function Modules.LadderLowerBattleResult:__SetRetType(var)
    self.__is_finished = false

    self.__ret_type = var
    CPPGameLib.Switch(self.__ret_type)
    {
        [ResultType.FAILED] = function()
            self:__HandleFailed()
        end,
        [ResultType.SUCCESS] = function()
            self:__HandleSuccess()
        end,
        [ResultType.PROMPT] = function()
            self:__HandlePrompt()
        end,
    }
end

function Modules.LadderLowerBattleResult:__HandleFailed()
    self.__text_continue:SetVisible(false)
    self.__panel_result:SetVisible(true)
    self.__panel_prompt:SetVisible(false)

    self:__ShowLevelAndStar()

    --打字特效
    local _, effect_node = self:__CreateEffect({id = 100090}, self.__panel_title_effect)
    PosTool.Center(effect_node)
    CPPActionManager.DelayTo(self.__panel_title_effect, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
        --打星特效
        self:__ShowFailedStar(function()
            --失败描述
            local level, star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
            local config_level = CPPGameLib.GetConfig("LadderDan", level)
            if config_level then
                local lab_tips
                if star >= 1 then
                    --降星
                    if config_level.is_loss == Macros.Global.TRUE then
                        lab_tips = Util:LabCenter({
                            { zi = CPPGameLib.GetString("ladder_lower_battle_failed_tips1"), ys = Macros.Color.blinkcontent , dx = Macros.TypefaceSize.normal},
                            { zi = "-1", ys = Macros.Color.red , dx = Macros.TypefaceSize.normal, jg = 4}
                        })
                    else
                        lab_tips = Util:Label(CPPGameLib.GetString("ladder_lower_battle_failed_tips2"), Macros.TypefaceSize.normal, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)
                    end                    
                else
                    --降段
                    if config_level.degrade == Macros.Global.TRUE then
                        local new_level = level - 1
                        local lab_tbl = Util:DanTbl(new_level)
                        table.insert(lab_tbl, 1, {zi = CPPGameLib.GetString("ladder_lower_battle_failed_tips3"), ys = Macros.Color.blinkcontent , dx = Macros.TypefaceSize.normal})
                        lab_tips = Util:LabCenter(lab_tbl)
                    else
                        lab_tips = Util:Label(CPPGameLib.GetString("ladder_lower_battle_failed_tips4"), Macros.TypefaceSize.normal, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)            
                    end
                end
                lab_tips:SetAnchorPoint(0.5, 0.5)
                self.__panel_info:AddChild(lab_tips)            
                PosTool.Center(lab_tips)
            end

            self.__failed_star_finished = true
            self:__CheckFinished()            
        end)
    end))
end

function Modules.LadderLowerBattleResult:__HandleSuccess()
    self.__text_continue:SetVisible(false)
    self.__panel_result:SetVisible(true)
    self.__panel_prompt:SetVisible(false)

    self:__ShowLevelAndStar()

    --打字特效
    local _, effect_node = self:__CreateEffect({id = 100089}, self.__panel_title_effect)
    PosTool.Center(effect_node)
    CPPActionManager.DelayTo(self.__panel_title_effect, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
        --打星特效
        self:__ShowSuccessStar(function()
            local battle_info = self:CallModuleFunc("GetCurInfo")
            local is_first_win = battle_info.is_first_win == Macros.Global.TRUE

            --胜利描述
            local lab_tips
            if is_first_win then
                lab_tips = Util:LabCenter({
                    { zi = CPPGameLib.GetString("ladder_lower_battle_success_first_win_tips"), ys = Macros.Color.blinkcontent , dx = Macros.TypefaceSize.normal},
                    { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_star"), dx = 1, jg = 0},
                    { zi = CPPGameLib.GetString("ladder_lower_battle_success_tips"), ys = Macros.Color.blinkcontent, dx = Macros.TypefaceSize.normal, jg = 135},
                    { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_star"), dx = 1, jg = 0},
                })
            else
                lab_tips = Util:LabCenter({
                    { zi = CPPGameLib.GetString("ladder_lower_battle_success_tips"), ys = Macros.Color.blinkcontent, dx = Macros.TypefaceSize.normal},
                    { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_star"), dx = 1, jg = 0},
                })
            end
            lab_tips:SetAnchorPoint(0.5, 0.5)
            self.__panel_info:AddChild(lab_tips)            
            PosTool.Center(lab_tips)

            self.__success_star_finished = true
            self:__CheckFinished()            
        end)
    end))
end

function Modules.LadderLowerBattleResult:__HandlePrompt()
    self.__text_continue:SetVisible(false)
    self.__panel_result:SetVisible(false)
    self.__panel_prompt:SetVisible(true)
    self.__image_level_static:SetVisible(false)
    self.__image_levelbg_static:SetVisible(false)
    self.__text_prompt_reward = Util:WidgetLabel(self:GetWidget("Text_PromptReward"), CPPGameLib.GetString("ladder_lower_battle_prompt_tips"), Macros.TypefaceSize.normal, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)
    self.__text_prompt_reward:SetVisible(false)

    if self.__sp_level_name then
        self.__sp_level_name:RemoveSelf()
        self.__sp_level_name = nil
    end
    self.__panel_star:RemoveAllChild()

    local level, star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    local battle_info = self:CallModuleFunc("GetCurInfo")
    local add_star = battle_info.is_first_win == Macros.Global.TRUE and 2 or 1
    local new_max_star = star + add_star - config_level.promotion_aequum
    self:__ShowLevelAndStar(level + 1, new_max_star)
    self.__panel_star:SetVisible(false)
    self.__sp_level_name:SetVisible(false)

    self:PlayActionTimeline("animation0", false, function()
        self.__panel_star:SetVisible(true)
        self.__sp_level_name:SetVisible(true)

        --打星特效
        self:__ShowPromptStar(new_max_star, function()
            --晋级奖励
            self:__ShowPromptRewards()
        end)
    end)

    CPPActionManager.DelayTo(self.__panel_stage, 0.6, GlobalCallbackMgr:AddCallBackFunc(function()
        local _, effect_node = self:__CreateEffect({res_name = "UI_wangzheshengji"}, self.__panel_stage)
        PosTool.Center(effect_node, 0, -10)
    end))    
end

function Modules.LadderLowerBattleResult:__HandleContinue()
    local function handle_func()
        self:CloseManual(true)
        self:CallModuleFunc("BattleEndReturnRelevantView")
    end

    CPPGameLib.Switch(self.__ret_type)
    {
        [ResultType.FAILED] = function()
            handle_func()
        end,
        [ResultType.SUCCESS] = function()
            local battle_info = self:CallModuleFunc("GetCurInfo")
            local add_star = battle_info.is_first_win == Macros.Global.TRUE and 2 or 1
            local level, star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
            local config_level = CPPGameLib.GetConfig("LadderDan", level)
            if not config_level or (star + add_star < config_level.promotion_aequum) then
                handle_func()
                return
            end  
            self:__SetRetType(ResultType.PROMPT)          
        end,
        [ResultType.PROMPT] = function()
            handle_func()
        end,
    }
end

function Modules.LadderLowerBattleResult:__CheckFinished()
    local is_finished
    CPPGameLib.Switch(self.__ret_type)
    {
        [ResultType.FAILED] = function()
            is_finished = self.__failed_star_finished
        end,
        [ResultType.SUCCESS] = function()
            is_finished = self.__success_star_finished
        end,
        [ResultType.PROMPT] = function()
            is_finished = self.__prompt_finished
        end,
    }
    if is_finished then
        self.__is_finished = true
        self.__text_continue:SetVisible(true)
    end
end

--展示段位和星级
function Modules.LadderLowerBattleResult:__ShowLevelAndStar(level, star)
    local cur_level, cur_star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")    
    level = level or cur_level
    star = star or cur_star
   
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    if not config_level then
        return
    end

    --段位图标
    self.__image_level:SetImage(Resource.PathTool.GetLadderIconPath(config_level.icon), TextureResType.LOCAL)
    self.__image_level_static:SetImage(Resource.PathTool.GetLadderIconPath(config_level.icon), TextureResType.LOCAL)    

    --段位名称
    self:__UpdateLevelName(level, star)

    --星级
    self.__star_list = {}
    self.__panel_star:RemoveAllChild()    
    local radius = 100
    local beg_degree = 180 / (config_level.promotion_aequum + 1)
    local step_degree = 180 / (config_level.promotion_aequum + 1)
    local x, y, degree
    for i = 1, config_level.promotion_aequum do
        local sp_star = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_COMMON, "sp_star"))
        sp_star:SetGray(1 + config_level.promotion_aequum - i > star)
        degree = beg_degree + (i - 1) * step_degree
        x = math.cos(math.rad(degree)) * radius
        y = math.sin(math.rad(degree)) * radius
        sp_star:SetPosition(x, y)

        table.insert(self.__star_list, 1, sp_star)
        self.__panel_star:AddChild(sp_star)       
    end

    return true
end

--更新段位名
function Modules.LadderLowerBattleResult:__UpdateLevelName(level, star)
    local cur_level, cur_star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")    
    level = level or cur_level
    star = star or cur_star

    if self.__sp_level_name then
        self.__sp_level_name:RemoveSelf()
    end
    self.__sp_level_name = Util:Dan(level, star)
    self.__sp_level_name:SetAnchorPoint(0.5, 0)
    self.__panel_stage:AddChild(self.__sp_level_name)    
    PosTool.CenterBottom(self.__sp_level_name, 0, 20)
end

--展示胜利界面星星特效
function Modules.LadderLowerBattleResult:__ShowSuccessStar(finished_callback)
    local level, star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    if not config_level then
        finished_callback()
        return
    end

    local battle_info = self:CallModuleFunc("GetCurInfo")
    local add_star = battle_info.is_first_win == Macros.Global.TRUE and 2 or 1
    local max_star = (star + add_star) > config_level.promotion_aequum and config_level.promotion_aequum or (star + add_star)

    local function handle_effect_func()
        star = star + 1
        if star > max_star then
            finished_callback()
            return
        end

        local sp_star = self.__star_list[star]
        if sp_star then
            local _, effect_node = self:__CreateEffect({res_name = "UI_wangzhesxingxing1", loop = false}, sp_star)
            PosTool.Center(effect_node)
            CPPActionManager.DelayTo(sp_star, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
                sp_star:SetGray(false)
                handle_effect_func()
            end))
        end
    end
    handle_effect_func()
end

--展示失败界面星星特效
function Modules.LadderLowerBattleResult:__ShowFailedStar(finished_callback)
    local level, star = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    if not config_level then
        finished_callback()
        return
    end

    local function handle_effect_func(i, callback)
        local sp_star = self.__star_list[i]
        if sp_star then
            local _, effect_node = self:__CreateEffect({res_name = "UI_wangzhexingxing2", loop = false}, sp_star)
            PosTool.Center(effect_node)
            CPPActionManager.DelayTo(sp_star, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
                sp_star:SetGray(true)
                callback()
            end))
            return
        end        
    end

    if star >= 1 then
        --降星
        if config_level.is_loss == Macros.Global.TRUE then
            handle_effect_func(star, function()
                finished_callback()
            end)
            return
        end
    else
        --降段
        if config_level.degrade == Macros.Global.TRUE then
            local new_level = level - 1
            local config_new_level = CPPGameLib.GetConfig("LadderDan", new_level)
            local new_max_star = config_new_level and config_new_level.promotion_aequum or 0
            self:__ShowLevelAndStar(level - 1, new_max_star)
            if new_max_star > 0  then
                handle_effect_func(new_max_star, function()
                    finished_callback()
                end)
                return
            end
        end
    end
    finished_callback()
end

--展示晋级界面星星特效
function Modules.LadderLowerBattleResult:__ShowPromptStar(new_max_star, finished_callback)
    local new_star = 0
    local function handle_effect_func()
        new_star = new_star + 1
        if new_star > new_max_star then
            finished_callback()
            return
        end

        local sp_star = self.__star_list[new_star]
        if sp_star then
            local _, effect_node = self:__CreateEffect({res_name = "UI_wangzhesxingxing1", loop = false}, sp_star)
            PosTool.Center(effect_node)
            CPPActionManager.DelayTo(sp_star, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
                sp_star:SetGray(false)
                handle_effect_func()
            end))            
        end
    end
    handle_effect_func()
end

--晋级奖励
function Modules.LadderLowerBattleResult:__ShowPromptRewards()
    local panel_reward = self:GetWidget("Panel_PromptRewardItems")  
    self.__text_prompt_reward:SetVisible(true)  

    local function finished_callback(show_rewards)
        if show_rewards == false then
            panel_reward:SetVisible(false)
            self.__text_prompt_reward:SetVisible(false)
        end
        self.__prompt_finished = true
        self:__CheckFinished()            
    end

    local level = CallModuleFunc(ModuleType.LADDER, "GetMyRank")
    local history_level = CallModuleFunc(ModuleType.LADDER, "GetHistoryRank")
    local config_level = CPPGameLib.GetConfig("LadderDan", level)
    if not config_level or level + 1 <= history_level then
        finished_callback(false)
        return 
    end

    local icon_space = 10
    local reward_index = 1
    local reward_list = ConfigAdapter.Item.GetGiftPackRewardListII(config_level.promotion_award_id, config_level.promotion_award_num)
    if #reward_list <= 0 then
        finished_callback(false)
        return 
    end

    local function create_reward_icon()
        if not reward_list then
            return
        end
        local reward_info = reward_list[reward_index]
        if not reward_info then
            return
        end

        local icon = LogicGUI.IconLayer.New(false, false)
        icon:SetAnchorPoint(0, 0.5)
        icon:SetData(reward_info)
        icon:PlayAppearEffect()
        panel_reward:AddChild(icon:GetNode())
        PosTool.LeftCenter(icon, (icon:GetWidth() + icon_space) * (reward_index - 1))

        reward_index = reward_index + 1
    end

    self.__prompt_rewards_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            create_reward_icon()

            if not reward_list or reward_index > #reward_list then
                GlobalTimerQuest:CancelQuest(self.__prompt_rewards_timer)
                self.__prompt_rewards_timer = nil
                finished_callback()
            end     
        end), 0.2, -1) 
end