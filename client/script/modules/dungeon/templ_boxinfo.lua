
Modules = Modules or {}

--副本宝箱信息模板
Modules.TemplDungeonBoxInfo = Modules.TemplDungeonBoxInfo or BaseClass(GUI.Template)

--[[
	@data
	title
	reward_list
	type 	 			[number]奖励类型(Macros.Game.FBRewardType)
	status				[number]宝箱状态(Modules.DungeonModule.BoxStatus)
	condition			[string]星级条件/通关条件
	chapter_index 		[number]章节索引
	index 				[number]上下文索引
	fb_type				[number]副本类型
--]]
function Modules.TemplDungeonBoxInfo:__init(data)
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	CPPGameLib.PrintTable(data,"heromoduleFule")
	self.__is_auto_load = true

	self.__open_callback = function()
		--标题
		Util:WidgetLabel(self:GetWidget("Text_Title"), data.title, Macros.TypefaceSize.popup, Macros.Color.btn)

		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_Close"), "", function()
			self:DeleteMe()
		end)				

		local can_receive = data.status and data.status == Modules.DungeonModule.BoxStatus.CAN_RECEIVE or false
		local already_received = data.status and data.status == Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE or false

		--领取按钮
		local stroke_color = already_received and Macros.Color.forbidden_stroke or (can_receive and Macros.Color.button_yellow or Macros.Color.button_red)
		local btn_receive = Util:WidgetButton(self:GetWidget("Button_Receive"), nil, function()
			if can_receive then
				--领取副本宝箱
				CallModuleFunc(ModuleType.DUNGEON, "FBAwardGetReq", data.type, data.chapter_index, data.index, data.fb_type)
			end
			self:DeleteMe()
		end,Macros.TypefaceSize.largish,Macros.Color.btn,stroke_color)
		btn_receive:SetBright(not already_received)
		btn_receive:SetText(already_received and CPPGameLib.GetString("dungeon_have_receive") or (can_receive and CPPGameLib.GetString("dungeon_receive") or CPPGameLib.GetString("common_close")))		
		if can_receive then
	        btn_receive:SetNormalImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green"))
	    end
		--可领取走边特效
		if can_receive then
			local effectId, effect = self:__CreateEffect({res_name = "ui_anniuzoubian"}, btn_receive,1)
			PosTool.Center(effect)
		end
		
		--领取条件
		Util:WidgetLabel(self:GetWidget("Text_arriveCondition"),CPPGameLib.GetString("dungeon_reach"),Macros.TypefaceSize.normal,  Macros.Color.content )
		local txt_gamelevel_condition = Util:WidgetLabel(self:GetWidget("Text_GameLevelCondition"), data.condition,Macros.TypefaceSize.normal, Macros.Color.content )
		local txt_star_condition = Util:WidgetLabel(self:GetWidget("Text_StarCondition"), data.condition..CPPGameLib.GetString("dungeon_acquire"), Macros.TypefaceSize.normal, Macros.Color.content )
		local panel_star_condition = self:GetWidget("Panel_StarCondition")
		txt_gamelevel_condition:SetVisible(data.type == Macros.Game.FBRewardType.GAMELEVEL)
		panel_star_condition:SetVisible(data.type ~= Macros.Game.FBRewardType.GAMELEVEL)
		local image_6 = WidgetUnity.GetWidgetByName(panel_star_condition, "Image_6")

		--奖励列表
		local widget = self:GetWidget("ScrollView_Rewards")
		local item_w = 84
		local item_space = 15
		local offset_x = item_w / 2
		for _, reward_info in ipairs(data.reward_list) do
			local icon_layer = LogicGUI.IconLayer.New()
			icon_layer:SetData(reward_info)
			icon_layer:SetAnchorPoint(0.5, 1)
			icon_layer:ShowItemName(true)
			icon_layer:ShowEquipName(true)
			icon_layer:ShowTreasureName(true)
			icon_layer:ShowHeroName(true)
			widget:AddChild(icon_layer:GetNode())
			PosTool.LeftTop(icon_layer, offset_x)
			offset_x = offset_x + item_w + item_space
		end	
		offset_x = offset_x - item_w / 2 - item_space
		widget:SetInnerContainerSize(offset_x + 4, 0)
	end

	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)		
end

function Modules.TemplDungeonBoxInfo:__delete()
end
