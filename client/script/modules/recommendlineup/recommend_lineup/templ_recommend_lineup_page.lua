GUI = GUI or {}

GUI.TemplRecommendLineupPage = GUI.TemplRecommendLineupPage or BaseClass(GUI.Template)

function GUI.TemplRecommendLineupPage:__init(hero_list)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_RECOMMENDLINEUP_PAGE)
	self:__CreateView(hero_list)
end

function GUI.TemplRecommendLineupPage:__AddToParent()
	
end

function GUI.TemplRecommendLineupPage:__CreateView(hero_list)
	self:SetData(hero_list)	

	for i,hero_id in ipairs(hero_list) do
		--监听容器
		local panel = self:GetWidget("Panel_pos"..i)
		WidgetUnity.SetWidgetListener(panel, nil, nil,function ()
			--CPPGameLib.PrintTable("ldx",self.__data,"self.__data: ")
			CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK_DETAIL,self.__data[i])
		end)
		local config = CPPGameLib.GetConfig("Hero",hero_id)
		if config then
			--英雄阴影
			local image_shadow = self:GetWidget("Image_shadow"..i)
			self["__ui_hero"..i] = LogicGUI.Model.New({info_id = hero_id, parent = image_shadow})	
			self["__ui_hero"..i]:SetScaleFactor(config.city_scale)
			self["__ui_hero"..i]:SetPosition(58,25)
			self["__ui_hero"..i]:SetZOrder(5 - i)
			--英雄名字		
			self["__hero_name"..i] = self:GetWidget("Text_heroname"..i)
			Util:WidgetLabel(self["__hero_name"..i], config.name, Macros.TypefaceSize.slightly, Macros.Quality[config.init_quality], 0, 0, Macros.Quality_Stroke[config.init_quality])		
		end
	end
end

function GUI.TemplRecommendLineupPage:__ChangeData(hero_list)
	self:SetData(hero_list)
	for i,hero_id in ipairs(hero_list) do
		--重设模型
		if self["__ui_hero"..i] then
			self["__ui_hero"..i]:SetInfoID(hero_id)
		end
		--英雄名字
		local config = CPPGameLib.GetConfig("Hero",hero_id)
		if config and self["__hero_name"..i] then
			self["__hero_name"..i]:SetText(config.name)
			self["__hero_name"..i]:SetColor(Macros.QualityColor(config.init_quality))
			self["__hero_name"..i]:SetStrokeColor(Macros.QualityColorStroke(config.init_quality))
		end
	end
end

function GUI.TemplRecommendLineupPage:__delete()
	for i=1,5 do
		if self["__ui_hero"..i] then
			self["__ui_hero"..i]:DeleteMe()
			self["__ui_hero"..i] = nil
		end
	end
end