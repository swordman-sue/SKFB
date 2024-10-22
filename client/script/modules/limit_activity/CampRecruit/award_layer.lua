LogicGUI = LogicGUI or {}

LogicGUI.TemplCampRecruitAwardView = LogicGUI.TemplCampRecruitAwardView or BaseClass(GUI.Template)

function LogicGUI.TemplCampRecruitAwardView:__init(info)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self:InitUI(info)
    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplCampRecruitAwardView:InitUI(info)
    local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",info.cur_data_id)

    --奖励列表
    local widget = self:GetWidget("ScrollView_Rewards")
    local item_w = 84
    local item_space = 15
    local offset_x = item_w / 2
      
    local icon_layer = LogicGUI.IconLayer.New()
    icon_layer:SetData({id = config_camp_recruit.recruit_data_list[info.camp].score_task_reward_id_list[info.index],num = config_camp_recruit.recruit_data_list[info.camp].score_task_reward_num_list[info.index]})
    icon_layer:SetAnchorPoint(0.5, 1)
    icon_layer:ShowItemName(true)
    icon_layer:ShowEquipName(true)
    icon_layer:ShowHeroName(true)
    widget:AddChild(icon_layer:GetNode())
    PosTool.LeftTop(icon_layer, offset_x)
    offset_x = offset_x + item_w + item_space
    offset_x = offset_x - item_w / 2 - item_space
    widget:SetInnerContainerSize(offset_x + 4, 0)

    --标题
    Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("award_box_Title"), Macros.TypefaceSize.popup, Macros.Color.btn )
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_Close"), "", function()
        self:DeleteMe()
    end)                

    --领取按钮
    local stroke_color = info.isHaveGet and Macros.Color.forbidden_stroke or (info.isCouldGet and Macros.Color.button_yellow or Macros.Color.button_red)
    local btn_receive = Util:WidgetButton(self:GetWidget("Button_Receive"),nil, function()
        --领取副本宝箱
        if info.isCouldGet then
            local cur_info = {}
            cur_info.camp = info.camp
            cur_info.reward_idx = info.index - 1
            CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"CampRecruitScoreTaskRewardReq",cur_info)                
        end
        self:DeleteMe()  
    end,Macros.TypefaceSize.largish,Macros.Color.btn,stroke_color)
    btn_receive:SetBright(not info.isHaveGet)
    btn_receive:SetText(info.isHaveGet and CPPGameLib.GetString("dungeon_have_receive") or (info.isCouldGet and CPPGameLib.GetString("dungeon_receive") or CPPGameLib.GetString("common_close")))   
    if info.isCouldGet then
        btn_receive:SetNormalImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green"))
    end
    if info.isCouldGet and not info.isHaveGet then
        local effect
        self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},btn_receive)
        PosTool.Center(effect)
    end   
    --领取条件   
    self:GetWidget("Panel_StarCondition"):SetVisible(false)
    Util:WidgetLabel(self:GetWidget("Text_GameLevelCondition"), CPPGameLib.GetString("award_point_could_get",info.score), Macros.TypefaceSize.normal, Macros.Color.content )
    self:GetWidget("Text_GameLevelCondition"):SetVisible(not info.isCouldGet)
end

function LogicGUI.TemplCampRecruitAwardView:__delete()
    if self.__button_zoubian_effect then
        self:__DelEffect(self.__button_zoubian_effect)
        self.__button_zoubian_effect = nil
    end
end