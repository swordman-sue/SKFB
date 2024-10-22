Modules = Modules or {}

Modules.TemplHeroAwakenPropPreLookCell = Modules.TemplHeroAwakenPropPreLookCell or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroAwakenPropPreLookCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_PRELOOK_CELL)
end

function Modules.TemplHeroAwakenPropPreLookCell:SetData(data)
	--标题	
	Util:WidgetLabel(self:GetWidget("Text_stage"),CPPGameLib.GetString("hero_awaken_propertyTitleSp",data.awaken_star,data.awaken_level),Macros.TypefaceSize.tab, Macros.Color.headline )
	--获取配置表
	local awaken_config = CPPGameLib.GetConfig("HeroAwakening", data.specialId)
	awaken_config = awaken_config[data.awaken_star][data.awaken_level]
	for i,itemId in ipairs(awaken_config.cost_item_list) do
		local panel_pos = self:GetWidget("Panel_pos"..i)
		panel_pos:SetVisible(true)
		panel_pos:SetTouchSwallow(false)
		local prop_config = CPPGameLib.GetConfig("AwakeningItem",itemId)
		--道具
		local icon = LogicGUI.IconLayer.New(false,false)
		panel_pos:AddChild(icon:GetNode())
		PosTool.Center(icon)
		icon:SetData({id = itemId})
		if prop_config.quality == Macros.Game.QualityType.WHITE then
			WidgetUnity.SetWidgetListener(panel_pos, nil, nil,function ()
				LogicGUI.TemplHeroAwakenPropView.New(itemId,2)
			end)
		else
			WidgetUnity.SetWidgetListener(panel_pos, nil, nil,function ()
				LogicGUI.TemplHeroAwakenPropView.New(itemId,4)
			end)
		end	
		--拥有
		Util:WidgetLabel(self:GetWidget("Text_haved"..i),CPPGameLib.GetString("approach_have"),Macros.TypefaceSize.largish,Macros.Color.content)
		--数量
		local haveNum = ObjIDTool.GetOwnNum(itemId)
		Util:WidgetLabel(self:GetWidget("Text_havedNum"..i),haveNum,Macros.TypefaceSize.largish,haveNum < 1 and Macros.Color.content or Macros.Color.green,0,0,haveNum >= 1 and Macros.Color.green_stroke or nil)	
	end
end