
--[[
%% @module: 天梯赛王者段位战斗结算界面
%% @author: swordman sue
%% @created: 2017年10月31日
--]]

Modules = Modules or {}

Modules.LadderSeniorBattleResult = Modules.LadderSeniorBattleResult or BaseClass(BaseView)

function Modules.LadderSeniorBattleResult:__init()
	self.__layout_name = "ladder_senior_result"	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__loaded_callback = function()
        self:__LoadedCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, true)
end

function Modules.LadderSeniorBattleResult:__LoadedCallback()
    self.__image_blanner = self:GetWidget("Image_Blanner")
    self.__text_continue = Util:WidgetLabel(self:GetWidget("Text_Continue"), CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.headline, Macros.Color.blinkcontent, nil, nil, Macros.Color.blinkcontent_stroke)
    WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
        if self.__is_finished then
            self:CloseManual(true)
            self:CallModuleFunc("BattleEndReturnRelevantView")        
        end
    end)

    Util:WidgetLabel(self:GetWidget("Text_Result"), CPPGameLib.GetString("ladder_senior_battle_info1"), Macros.TypefaceSize.normal, Macros.Color.content, nil, nil, nil)
    Util:WidgetLabel(self:GetWidget("Text_PayerInfo"), CPPGameLib.GetString("ladder_senior_battle_info2"), Macros.TypefaceSize.normal, Macros.Color.content, nil, nil, nil)
    Util:WidgetLabel(self:GetWidget("Text_Score"), CPPGameLib.GetString("ladder_senior_battle_info3"), Macros.TypefaceSize.normal, Macros.Color.content, nil, nil, nil)
    Util:WidgetLabel(self:GetWidget("Text_Rank"), CPPGameLib.GetString("ladder_senior_battle_info4"), Macros.TypefaceSize.normal, Macros.Color.content, nil, nil, nil)

    self.__panel_bg = self:GetWidget("Panel_BG")
    self.__panel_myitem = self:GetWidget("Panel_MyItem")
    self.__panel_enemyitem = self:GetWidget("Panel_EnemyItem")
    self.__panel_title_effect = self:GetWidget("Panel_TitleEffect")

    self.__image_my = self:GetWidget("Image_My")
    self.__image_enemy = self:GetWidget("Image_Enemy")
    self.__panel_my_head = self:GetWidget("Panel_MyHead")
    self.__panel_enemy_head = self:GetWidget("Panel_EnemyHead")

    local my_key, my_widget
    local widget_name_tbl = {"Text_%sName", "Text_%sPower", "Text_%sServer", "Text_%sScore", "Text_%sRank"}
    for i, widget_name in ipairs(widget_name_tbl) do
        my_key = string.lower(string.format("__"..widget_name, "my_"))
        my_widget =  Util:WidgetLabel(self:GetWidget(string.format(widget_name, "My")), "", Macros.TypefaceSize.normal, Macros.Color.blinkcontent, nil, nil, Macros.Color.blinkcontent_stroke)
        if i >= 4 then
            local real_my_widget = Util:RichText()
            real_my_widget:SetAnchorPoint(0.5, 0.5)
            my_widget:AddChild(real_my_widget)
            PosTool.Center(real_my_widget)                        
            self[my_key] = real_my_widget
        else
            self[my_key] = my_widget
        end
        self[string.lower(string.format("__"..widget_name, "enemy_"))] =  Util:WidgetLabel(self:GetWidget(string.format(widget_name, "Enemy")), "", Macros.TypefaceSize.normal, Macros.Color.blinkcontent, nil, nil, Macros.Color.blinkcontent_stroke)
    end    
end

function Modules.LadderSeniorBattleResult:__OpenCallback()
    self.__is_finished = false

    local battle_info = self:CallModuleFunc("GetCurInfo")
    local is_win = battle_info.is_win == Macros.Global.TRUE and true or false

    self.__text_continue:SetVisible(false)

    --横幅
    self.__image_blanner:SetImage(is_win and
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_success_top") or
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_failed_top"))

    --背景框
    self.__panel_bg:SetBackGroundImage(is_win and
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_success") or
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_failed"))

    --我方结果
    self.__image_my:SetImage(is_win and
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "sp_success", true) or
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "sp_failed", true))

    --我方条目框
    self.__panel_myitem:SetBackGroundImage(is_win and
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_success_item") or
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_failed_item"))

    --角色信息
    local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
    local server_info = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", role_info.server_id)
    local my_rank, my_rank_score = CallModuleFunc(ModuleType.LADDER, "GetMyGrid")
    local my_new_rank = battle_info.end_params_list[1]
    local my_score_change = ConfigAdapter.Common.GetLadderSeniorBattleResultAddScore()
    self.__text_my_name:SetText(role_info.role_name)
    self.__text_my_server:SetText(CPPGameLib.GetString("ladder_senior_server_info", role_info.server_id, server_info.name))
    self.__text_my_power:SetText(CPPGameLib.GetString("common_power_prefix", role_info.battle_value))

    local my_role_icon = LogicGUI.RoleIcon.New(false)
    my_role_icon:SetAnchorPoint(0.5, 0.5)
    my_role_icon:SetData(role_info.head_icon)
    self.__panel_my_head:AddChild(my_role_icon:GetNode())
    PosTool.Center(my_role_icon)

    --积分、排名
    local my_new_rank_score, my_rank_change, key, key_ex
    local my_rank_change = math.abs(my_new_rank - my_rank)
    if is_win then
        key = "ladder_senior_rank_add_info"
        key_ex = "ladder_senior_rank_add_info_ex"
        my_new_rank_score = my_rank_score + my_score_change
    else
        key = "ladder_senior_rank_sub_info"
        key_ex = "ladder_senior_rank_sub_info_ex"
        my_new_rank_score = math.max(0, my_rank_score - my_score_change)
    end

    --先设置原值，滚到新值
    self.__text_my_score:SetText(my_new_rank_score ~= my_rank_score and
        CPPGameLib.GetString(key, my_rank_score, my_score_change) or 
        GUI.RichText.GetColorWord(my_rank_score, Macros.Color.blinkcontent_hex, Macros.Color.blinkcontent_stroke_hex))

    if my_new_rank == 0 then
        self.__text_my_rank:SetText(GUI.RichText.GetColorWord(CPPGameLib.GetString("rank_reflection"), Macros.Color.blinkcontent_hex, Macros.Color.blinkcontent_stroke_hex))
    else
        --先设置原值，滚到新值
        self.__text_my_rank:SetText(my_new_rank ~= my_rank and 
            CPPGameLib.GetString(key_ex, my_rank, my_rank_change) or GUI.RichText.GetColorWord(my_rank,  Macros.Color.blinkcontent_hex, Macros.Color.blinkcontent_stroke_hex))
    end

    --敌方结果
    self.__image_enemy:SetImage(is_win and
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "sp_failed", true) or
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "sp_success", true))

    --敌方条目框
    self.__panel_enemyitem:SetBackGroundImage(is_win and
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_success_item") or
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LADDER_SENIOR_RESULT, "bg_failed_item"))

    --角色信息
    local enemy_info = CallModuleFunc(ModuleType.LADDER, "GetMyOpponent")
    server_info = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", enemy_info.server)
    self.__text_enemy_name:SetText(enemy_info.role_name)
    self.__text_enemy_server:SetText(CPPGameLib.GetString("ladder_senior_server_info", enemy_info.server, server_info.name))
    self.__text_enemy_power:SetText(CPPGameLib.GetString("common_power_prefix", enemy_info.power))

    local enemy_role_icon = LogicGUI.RoleIcon.New(false)
    enemy_role_icon:SetAnchorPoint(0.5, 0.5)
    enemy_role_icon:SetData(enemy_info.head_img_id)
    self.__panel_enemy_head:AddChild(enemy_role_icon:GetNode())
    PosTool.Center(enemy_role_icon)

    --积分、排名
    self.__text_enemy_score:SetText(enemy_info.rank_value)
    self.__text_enemy_rank:SetText(enemy_info.rank <= 0 and CPPGameLib.GetString("rank_reflection") or enemy_info.rank)

    --打字特效
    local _, effect_node = self:__CreateEffect({id = is_win and 100089 or 100090 }, self.__panel_title_effect)
    PosTool.Center(effect_node)
    CPPActionManager.DelayTo(self.__panel_title_effect, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
        --积分、排名滚字特效
        self:__ShowRankValueEffect(my_rank, my_new_rank, my_rank_score, my_new_rank_score)
    end))    
end

function Modules.LadderSeniorBattleResult:__Dispose()
    if self.__roll_rank_effect then
        self.__roll_rank_effect:DeleteMe()
        self.__roll_rank_effect = nil
    end

    if self.__roll_rank_score_effect then
        self.__roll_rank_score_effect:DeleteMe()
        self.__roll_rank_score_effect = nil
    end
end

function Modules.LadderSeniorBattleResult:__ShowRankValueEffect(my_rank, my_new_rank, my_rank_score, my_new_rank_score)
    if not self.__roll_rank_effect then
        self.__roll_rank_effect = TypefaceEffects.New()
    end
    self.__roll_rank_effect:RollEx(self.__text_my_rank, my_rank, my_new_rank, 0.3, nil, 
    function(cur_value)
        local key = (my_rank > 0 and my_new_rank > my_rank) and "ladder_senior_rank_sub_info_ex" or "ladder_senior_rank_add_info_ex"
        return CPPGameLib.GetString(key, cur_value, math.abs(my_new_rank - my_rank))              
    end, 
    function()
        self.__is_finished = true
        self.__text_continue:SetVisible(true)
    end)

    if not self.__roll_rank_score_effect then
        self.__roll_rank_score_effect = TypefaceEffects.New()
    end
    self.__roll_rank_score_effect:RollEx(self.__text_my_score, my_rank_score, my_new_rank_score, 0.3, nil, 
    function(cur_value)      
        local key = my_new_rank_score - my_rank_score > 0 and "ladder_senior_rank_add_info" or "ladder_senior_rank_sub_info"
        return CPPGameLib.GetString(key, cur_value, math.abs(my_new_rank_score - my_rank_score))
    end)
end
