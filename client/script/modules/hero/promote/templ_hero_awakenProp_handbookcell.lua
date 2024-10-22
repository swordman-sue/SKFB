GUI = GUI or {}
GUI.TemplHeroAwakenPropHandBookCell = GUI.TemplHeroAwakenPropHandBookCell or BaseClass(GUI.Template)

function GUI.TemplHeroAwakenPropHandBookCell:__init(data,quality)
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_HANDBOOK_CELL)
	self:createMainUI(data,quality)
end

function GUI.TemplHeroAwakenPropHandBookCell:__AddToParent()

end
function GUI.TemplHeroAwakenPropHandBookCell:createMainUI(data,quality)	
	--标题
	Util:WidgetLabel(self:GetWidget("Text_stage"),CPPGameLib.GetString("hero_awaken_PropHandBookCellTitle"..quality),Macros.TypefaceSize.tab,Macros.Quality[quality],0,0,Macros.Quality_Stroke[quality])
	
	--计算行数
	local rowNum = 0
	if #data%5 == 0 then
		rowNum = math.floor(#data/5)
	else
		rowNum = math.ceil(#data/5)
	end
	local params = {
		item_class = Modules.TemplHeroAwakenPropHandBookCell,
    	item_width = 80,
    	item_height	= 125,
    	item_space_c = 35,
    	item_space_r = 10,
    	row	= rowNum,
    	col	= 5,
    	touchable = true,
    	touch_callback = function (index,data)
    		if data.quality == Macros.Game.QualityType.WHITE then
    			LogicGUI.TemplHeroAwakenPropView.New(data.id,2)
    		else
    			LogicGUI.TemplHeroAwakenPropView.New(data.id,4)
    		end
    	end
	}
	local scrollviewpanel = self:GetWidget("Panel_ScrollView")
	self.__scrollview = GUI.ScrollView.New(params)					
	scrollviewpanel:AddChild(self.__scrollview:GetNode())		
	self.__scrollview:SetDataList(data)
	--根据高度适配
	local height = self.__scrollview:GetViewHeight()	
	self:GetNode():SetContentSize(626,height+77)
	self:GetWidget("Image_bg"):SetContentSize(603,height+15)
	scrollviewpanel:SetContentSize(540,height)	
	PosTool.LeftTop(self:GetWidget("Panel_top"))
	PosTool.CenterBottom(self:GetWidget("Image_bg"),0,11)
	PosTool.CenterBottom(scrollviewpanel,0,19)
	PosTool.Center(self.__scrollview)	
	
end

function GUI.TemplHeroAwakenPropHandBookCell:__delete()
	if self.__scrollview then
		self.__scrollview:DeleteMe()
		self.__scrollview = nil
	end
end

-------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}

Modules.TemplHeroAwakenPropHandBookCell = Modules.TemplHeroAwakenPropHandBookCell or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroAwakenPropHandBookCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 
end

function Modules.TemplHeroAwakenPropHandBookCell:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,125)
  self.layout:SetAnchorPoint(0,0)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(255, 0, 0)
  return self.layout
end

function Modules.TemplHeroAwakenPropHandBookCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	 --头像
    self.__icon = LogicGUI.IconLayer.New(false,false)
    self.__icon:SetAnchorPoint(0.5,1)  
    self.layout:AddChild(self.__icon:GetNode())
    PosTool.CenterTop(self.__icon)
    self.__icon:SetData({id = data.id})
    self.__icon:ShowAwakenToolName(true,85,0)
end