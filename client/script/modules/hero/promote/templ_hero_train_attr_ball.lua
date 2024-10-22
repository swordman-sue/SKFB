
Modules = Modules or {}

--英雄培养属性球
Modules.TemplHeroTrainAttrBall = Modules.TemplHeroTrainAttrBall or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroTrainAttrBall:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE	
	self:InitTemplate()
end

function Modules.TemplHeroTrainAttrBall:__CreateWidgetFromLuaCode(...)
	local layout = Util:Layout(50, 50)
	layout:SetAnchorPoint(0.5, 0.5)
	layout:SetTouchEnabled(true)
	layout:SetTouchSwallow(false)

	self.__img_attr_ball = Util:Sprite(
		Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_PROMOTE, "attribute_ball"))
	layout:AddChild(self.__img_attr_ball)
	PosTool.Center(self.__img_attr_ball)

	WidgetUnity.SetWidgetListener(layout, nil, nil,
		function()
			self:__ShowAttrInfo()
		end)

	return layout
end

function Modules.TemplHeroTrainAttrBall:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	local cur_train_data_index = self.__data.cur_train_data_index
	local config = self.__data.config

	self.__img_attr_ball:SetGray(cur_train_data_index + 1 < config.data_index)
	if cur_train_data_index >= config.data_index or cur_train_data_index + 1 < config.data_index then
		self.__img_attr_ball:SetColor(unpack(Macros.Color.lilyWhite))
	else
		self.__img_attr_ball:SetColor(unpack(Macros.Color.green))
	end
end

function Modules.TemplHeroTrainAttrBall:__ShowAttrInfo()
	self.__is_showing = not self.__is_showing

	if self.__is_showing then
		if not self.__panel_attr then
			self.__panel_attr = Util:Layout(98, 54)
			self.__panel_attr:SetAnchorPoint(0.5, 0)
			self.__root_widget:AddChild(self.__panel_attr)
			PosTool.CenterTop(self.__panel_attr)

			local img_line = Util:Sprite(
				Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_PROMOTE, "Attribute_line"))			
			img_line:SetAnchorPoint(0.5, 0)
			self.__panel_attr:AddChild(img_line)
			PosTool.CenterBottom(img_line)

			local config = self.__data.config
			local attr_str = Util:Nature(config.pro_type_list[1], config.pro_value_list[1], false, false, true)
			local lab_attr = Util:Label(attr_str, Macros.TypefaceSize.minimum, Macros.Color.white)
			lab_attr:SetAnchorPoint(0.5, 1)
			self.__panel_attr:AddChild(lab_attr)
			PosTool.CenterTop(lab_attr)
		end		
	end
	self.__panel_attr:SetVisible(self.__is_showing)
end
