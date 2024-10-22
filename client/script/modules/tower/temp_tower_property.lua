
--
-- @brief 爬塔属性界面
-- @author: yjg
-- @date: 2017年3月3日17:26:16
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerProperty = LogicGUI.TempTowerProperty or BaseClass(GUI.Template)

function LogicGUI.TempTowerProperty:__init(params)
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__InitWidget()
	end
	self.__bg_params.close_func = function ( ... )
		self:DeleteMe()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMP_TOWER_PROPERTY)	
end

function LogicGUI.TempTowerProperty:__InitWidget()

	local Image_1 = self:GetWidget("Image_1")
	local headline = Util:Label( CPPGameLib.GetString("tower_property") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	Image_1:AddChild( headline )
	PosTool.Center(headline)

	
	local Image_bg = self:GetWidget("Image_bg")
	local tower_propertyTips = Util:Label( CPPGameLib.GetString("tower_propertyTips") , Macros.TypefaceSize.normal , Macros.Color.darkOrange )
	tower_propertyTips:SetPosition( Image_bg:GetContentWidth()/2 , Image_bg:GetContentHeight()/2 + 75 )
	Image_bg:AddChild( tower_propertyTips )

	--累计属性
	local lst = CallModuleFunc(ModuleType.TOWER,"GetHoldBuff")
    -- CPPGameLib.PrintTable("xy",lst,"物品列表")
	lst = Util:SegmentationTab(lst , 2)

	local Image_lstBg = self:GetWidget("Image_lstBg")
	local y = Image_lstBg:GetContentHeight() - 25
	local lab = {}
	local interval = Image_lstBg:GetContentWidth() / 2
	for _,v in ipairs(lst) do
		for i,vv in ipairs(v) do
			local type_ , value = Util:Nature( vv.type , vv.value ,true )
			lab[i] = Util:LabCenter( { 
									{ zi = type_ , ys = Macros.Color.content },
									{ zi = value , ys = Macros.Color.front_color },
								} )
			lab[i]:SetPosition(  (interval/2) + interval * (i - 1) , y )
			Image_lstBg:AddChild( lab[i] )
		end
		y = y - 40 
	end

	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )

end


function LogicGUI.TempTowerProperty:OnBtnClose()
	self:DeleteMe()
end