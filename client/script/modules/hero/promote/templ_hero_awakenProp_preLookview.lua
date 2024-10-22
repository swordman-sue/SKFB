LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroAwakenPropPreLookView = LogicGUI.TemplHeroAwakenPropPreLookView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroAwakenPropPreLookView:__init(specialid,star,level)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(specialid,star,level)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_PRELOOK_CELL)
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_PRELOOK)
end

function LogicGUI.TemplHeroAwakenPropPreLookView:InitUI(specialid,star,level)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_awaken_PropPreLookTitle"),Macros.TypefaceSize.tab,Macros.Color.btn)
	--关闭按钮	
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ( )
		self:DeleteMe()
	end)
	--插入预览数据	
	local finalData = {}
	for i=1,5 do
		local smallData = {}
		if star >= Macros.Game.HERO_AWAKE_STAR_MAX then
			if i == 1 then
				smallData = {specialId = specialid,awaken_star = star,awaken_level = level}
			else
				smallData = {specialId = specialid,awaken_star = star-1,awaken_level = level +11 - i}
			end				
			table.insert(finalData,1,smallData)
		else
			if level + i > Macros.Game.HERO_AWAKE_LEVEL_MAX then
				if star+1 >= Macros.Game.HERO_AWAKE_STAR_MAX then
					smallData = {specialId = specialid,awaken_star = star+1,awaken_level = 0}	
					table.insert(finalData,smallData)
					break
				else
					smallData = {specialId = specialid,awaken_star = star+1,awaken_level = level + i - 10}	
				end							
			else
				smallData = {specialId = specialid,awaken_star = star,awaken_level = level + i}	
			end
			table.insert(finalData,smallData)
		end
	end
	--预览容器
	local panel_scrollview = self:GetWidget("Panel_scrollview")
	local params = {
		item_class	= Modules.TemplHeroAwakenPropPreLookCell,
	    item_width	= 626,
	    item_height	= 185,
	    row	= 3,
	    col	= 1,
	    item_space_r = 5,
	    view_width 	= 626,
	    view_height = 486,
	}
	self.__scrollview = GUI.ScrollView.New(params)
	panel_scrollview:AddChild(self.__scrollview:GetNode())
	PosTool.Center(self.__scrollview)
	self.__scrollview:SetDataList(finalData)
end

function LogicGUI.TemplHeroAwakenPropPreLookView:__delete()
	if self.__scrollview then
		self.__scrollview:DeleteMe()
		self.__scrollview = nil
	end
end