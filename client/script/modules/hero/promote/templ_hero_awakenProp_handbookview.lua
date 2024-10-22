LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroAwakenPropHandBookView = LogicGUI.TemplHeroAwakenPropHandBookView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroAwakenPropHandBookView:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_HANDBOOK_CELL)
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_PRELOOK)
end

function LogicGUI.TemplHeroAwakenPropHandBookView:InitUI()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_awaken_PropPreLookTitle"),Macros.TypefaceSize.tab,Macros.Color.btn)
	--关闭按钮	
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ( )
		self:DeleteMe()
	end)
	--关闭界面事件
	self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	    self:DeleteMe()
	end)
	--隐藏控件
	self:GetWidget("Panel_scrollview"):SetVisible(false)
	--整理图鉴数据
	local qualityTable = {[0] = {},[1] = {},[2] = {},[3] = {},[4] = {},[5] = {}}
	for id,propData in pairs(CPPGameLib.GetConfig("AwakeningItem",nil,nil,true)) do
		local data = {}
		data.id = id
		data.name = propData.name
		data.quality = propData.quality
		table.insert(qualityTable[propData.quality],data)
	end
	--ScrollView Table
	self.__scrollviewtable = {}
	--预览容器
	local listView_panel = self:GetWidget("ListView_panel")
	listView_panel:SetVisible(true)
	for i=0,5 do		
		local templ_handbookCell = GUI.TemplHeroAwakenPropHandBookCell.New(qualityTable[i],i)
		listView_panel:PushBackCustomItem(templ_handbookCell:GetNode())
		table.insert(self.__scrollviewtable,templ_handbookCell)
	end
	
end

function LogicGUI.TemplHeroAwakenPropHandBookView:__delete()
	for i,panel in ipairs(self.__scrollviewtable) do
		if panel then
			panel:DeleteMe()
			panel = nil
		end
	end
end

