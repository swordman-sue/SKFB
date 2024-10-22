LogicGUI = LogicGUI or {}

LogicGUI.TemplAwardView = LogicGUI.TemplAwardView or BaseClass(GUI.Template)

function LogicGUI.TemplAwardView:__init(i,isCouldGet,isHaveGet)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self:InitUI(i,isCouldGet,isHaveGet)
    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplAwardView:InitUI(i,isCouldGet,isHaveGet)
    self.OpenIndex = i
    --根据礼包来设置物品头像和名字
    local roleLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.DAILY_INIT_LEVEL)
    local missionInfo = nil
    for i,v in pairs(CPPGameLib.GetConfig("TeamLevelDailyMission",nil,nil,true)) do
        if roleLevel >= v.level_min and roleLevel <= v.level_max then
            missionInfo = v
            break
        end
    end

    --奖励列表
    local widget = self:GetWidget("ScrollView_Rewards")
    local item_w = 84
    local item_space = 15
    local offset_x = item_w / 2
      
    local itemInfo = CPPGameLib.GetConfig("Item",missionInfo.point_award_list[i])
    for i, reward_info in ipairs(itemInfo.item_id_list) do
        local icon_layer = LogicGUI.IconLayer.New()
        icon_layer:SetData({id = reward_info,num = itemInfo.item_max_num_list[i]})
        icon_layer:SetAnchorPoint(0.5, 1)
        icon_layer:ShowItemName(true)
        icon_layer:ShowEquipName(true)
        icon_layer:ShowHeroName(true)
        widget:AddChild(icon_layer:GetNode())
        PosTool.LeftTop(icon_layer, offset_x)
        offset_x = offset_x + item_w + item_space
    end 
    offset_x = offset_x - item_w / 2 - item_space
    widget:SetInnerContainerSize(offset_x + 4, 0)

    --标题
    Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("award_box_Title"), Macros.TypefaceSize.popup, Macros.Color.btn )
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_Close"), "", function()
        self:DeleteMe()
    end)                

    --领取按钮
    local stroke_color = isHaveGet and Macros.Color.forbidden_stroke or (isCouldGet and Macros.Color.button_yellow or Macros.Color.button_red)
    local btn_receive = Util:WidgetButton(self:GetWidget("Button_Receive"),nil, function()
        --领取副本宝箱
        if isCouldGet then
            CallModuleFunc(ModuleType.DALIYTASK_AND_ACHIEVEMENT,"DaliyMissionPointAwardReq",missionInfo.point_list[i])                
            self:FireNextFrame(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_DALIYMISSION_POINT,self.OpenIndex)
        end
        self:DeleteMe()  
    end,Macros.TypefaceSize.largish,Macros.Color.btn,stroke_color)
    btn_receive:SetBright(not isHaveGet)
    btn_receive:SetText(isHaveGet and CPPGameLib.GetString("dungeon_have_receive") or (isCouldGet and CPPGameLib.GetString("dungeon_receive") or CPPGameLib.GetString("common_close")))   
    if isCouldGet then
        btn_receive:SetNormalImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green"))
    end
    if isCouldGet and not isHaveGet then
        local effect
        self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},btn_receive)
        PosTool.Center(effect)
    end   
    --领取条件   
    self:GetWidget("Panel_StarCondition"):SetVisible(false)
    Util:WidgetLabel(self:GetWidget("Text_GameLevelCondition"), CPPGameLib.GetString("award_point_could_get",missionInfo.point_list[i]), Macros.TypefaceSize.normal, Macros.Color.content )
    self:GetWidget("Text_GameLevelCondition"):SetVisible(not isCouldGet)
end

function LogicGUI.TemplAwardView:__delete()
    if self.__button_zoubian_effect then
        self:__DelEffect(self.__button_zoubian_effect)
        self.__button_zoubian_effect = nil
    end
end