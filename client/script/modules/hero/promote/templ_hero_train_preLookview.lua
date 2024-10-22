LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroTrainPreLookView = LogicGUI.TemplHeroTrainPreLookView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroTrainPreLookView:__init(pro_level,pro_valueData)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(pro_level,pro_valueData)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_TRAIN_PRE_LOOK_CELL)
	self:InitTemplate(GUI.TemplateRegistry.HERO_TRAIN_PRE_LOOK_VIEW)
end

function LogicGUI.TemplHeroTrainPreLookView:InitUI(pro_level,pro_valueData)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_train_PreLook_title"),Macros.TypefaceSize.popup,Macros.Color.btn, 0, 0,nil,Macros.Color.shadow_color)
	--关闭按钮	
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ( )
		self:DeleteMe()
	end)
	--提示1
	Util:WidgetLabel(self:GetWidget("Text_tips_1"),CPPGameLib.GetString("hero_train_PreLook_tips_1"),Macros.TypefaceSize.slightly,Macros.Color.headline)
	--提示2
	Util:WidgetLabel(self:GetWidget("Text_tips_2"),CPPGameLib.GetString("hero_train_PreLook_tips_2"),Macros.TypefaceSize.slightly,Macros.Color.headline,544,42)
	--滚动容器
	local Panel_preLookScrollView = self:GetWidget("Panel_preLookScrollView")
	local params = {
		item_class = Modules.TemplHeroTrainPreLookCell,
	    item_width = 538,
	    item_height	= 220,
	    row	= 2,
	    col = 1,
	    item_space_r = 3,
	    view_width  = 538,
	    view_height = 385,
	}
	self.ScrollView = GUI.ScrollView.New(params)
	Panel_preLookScrollView:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)
	--数据处理
	local dataList = {}
	local config = CPPGameLib.GetConfig("HeroTrain",1)
	--判断是否满级
	if pro_level >= table.maxn(config.pro_level_list) then
		for i=5,1,-1 do
			local data = {}
			data.pro_level = pro_level - i + 1
			data.pro_valueData = pro_valueData
			table.insert(dataList,data)
		end
	else
		for i=1,5 do
			local data = {}
			data.pro_level = pro_level + i - 1
			data.pro_valueData = pro_valueData
			table.insert(dataList,data)
		end
	end	
	self.ScrollView:SetDataList(dataList)
end

function LogicGUI.TemplHeroTrainPreLookView:__delete()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end