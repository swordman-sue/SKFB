LogicGUI = LogicGUI or {}

LogicGUI.TemplTeamWorldMapRewardView = LogicGUI.TemplTeamWorldMapRewardView or BaseClass(GUI.Template)

function LogicGUI.TemplTeamWorldMapRewardView:__init(show_reward_list)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(show_reward_list)
	end
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplTeamWorldMapRewardView:InitUI(show_reward_list)

    --奖励列表
    local widget = self:GetWidget("ScrollView_Rewards")
    local item_w = 84
    local item_space = 15
    local offset_x = item_w / 2
      
    for i, reward_id in ipairs(show_reward_list) do
        local icon_layer = LogicGUI.IconLayer.New()
        icon_layer:SetData({id = reward_id})
        icon_layer:SetAnchorPoint(0.5, 1)
        icon_layer:ShowItemName(true)
        icon_layer:ShowEquipName(true)
        icon_layer:ShowHeroName(true)
        icon_layer:ShowGemName(true)
        widget:AddChild(icon_layer:GetNode())
        PosTool.LeftTop(icon_layer, offset_x)
        offset_x = offset_x + item_w + item_space
    end 
    offset_x = offset_x - item_w / 2 - item_space
    widget:SetInnerContainerSize(offset_x + 4, 0)

    --标题
    Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("team_boss_reward_prelook"), Macros.TypefaceSize.tab, Macros.Color.btn)
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_Close"), "", function()
        self:DeleteMe()
    end)                

    --关闭按钮
    local btn_receive = Util:WidgetButton(self:GetWidget("Button_Receive"),CPPGameLib.GetString("common_close"), function()
        self:DeleteMe()  
    end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_red) 
    --领取条件   
    self:GetWidget("Panel_StarCondition"):SetVisible(false)
    self:GetWidget("Text_GameLevelCondition"):SetVisible(false)
end

function LogicGUI.TemplTeamWorldMapRewardView:__delete()
end