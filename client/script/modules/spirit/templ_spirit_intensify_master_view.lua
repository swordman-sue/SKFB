--[[
--精灵图鉴大师
]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplSpiritIntensifyMasterView = LogicGUI.TemplSpiritIntensifyMasterView or BaseClass(GUI.Template)

function LogicGUI.TemplSpiritIntensifyMasterView:__init(pokedex_id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitMainUI(pokedex_id)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_INTENSIFY_MASTER_CELL)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_INTENSIFY_MASTER_VIEW)
end

function LogicGUI.TemplSpiritIntensifyMasterView:InitMainUI(pokedex_id)
	--标题
   Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("spirit_handbook_property_prelook_title"),Macros.TypefaceSize.popup,Macros.Color.btn ) 
   --关闭按钮  
   Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
        self:DeleteMe()
   end)
   --滚动容器
   local panel_scrollview = self:GetWidget("Panel_ScrollView")
   local params = {
		item_class = Modules.TemplSpiritIntensifyMasterCell,
	    item_width = 482,
	    item_height	= 38,
	    row = 10,
	    col	= 1,
	    view_width = 482,
	    view_height = 380,
	}
	self.__handbook_scrollview = GUI.ScrollView.New(params)
	panel_scrollview:AddChild(self.__handbook_scrollview:GetNode())
	PosTool.Center(self.__handbook_scrollview)
	--构造
	local total_data = {}
	local handbook_info = CPPGameLib.GetConfig("SpiritPokedex",pokedex_id)
	local pokedex_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritHandBookInfoByID",pokedex_id)
	for i,pokedex_info in ipairs(handbook_info.pokedex_list) do
		local data = {}
		data.need_level = pokedex_info.need_enhance_level
		data.lightup_level = pokedex_info and pokedex_info.level or 0
		data.index = i
		data.str = Util:Nature(pokedex_info.pro_type,pokedex_info.pro_value, false, false,true)
		table.insert(total_data,data)
	end
	self.__handbook_scrollview:SetDataList(total_data)
end

function LogicGUI.TemplSpiritIntensifyMasterView:__delete()
	if self.__handbook_scrollview then
		self.__handbook_scrollview:DeleteMe()
		self.__handbook_scrollview = nil
	end
end

--图鉴大师
Modules = Modules or {}
Modules.TemplSpiritIntensifyMasterCell = Modules.TemplSpiritIntensifyMasterCell or BaseClass(GUI.ScrollItem)


function Modules.TemplSpiritIntensifyMasterCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_INTENSIFY_MASTER_CELL)
end

function Modules.TemplSpiritIntensifyMasterCell:__delete()

end

function Modules.TemplSpiritIntensifyMasterCell:SetData(data)	
	GUI.ScrollItem.SetData(self, data)
	--背景
	self:GetWidget("Image_bg"):SetVisible(self.__data.index%2 == 1)
	--属性
	Util:WidgetLabel(self:GetWidget("Text_property"),CPPGameLib.GetString("spirit_handbook_property_txt",self.__data.need_level,self.__data.str)
		,Macros.TypefaceSize.largish,self.__data.lightup_level >= self.__data.index and Macros.Color.green or Macros.Color.content) 
	
end