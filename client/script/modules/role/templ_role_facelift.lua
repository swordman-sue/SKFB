--
-- @brief 切换头像 
-- @author: yjg
-- @date: 2016年12月13日10:07:10
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplRoleFacelift = LogicGUI.TemplRoleFacelift or BaseClass(GUI.Template)

function LogicGUI.TemplRoleFacelift:__init(id,direct,itemResolveId)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
		self:__SettleData()
	end
	--装备更新
	self.__hero_gather = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_GATHER, function(_,data)
		self:__UpdateHero()
	end)

	self.__change_succeed = self:BindGlobalEvent(Macros.Event.RoleModule.HEAD_CHANGE, function(_,data)
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("role_changeSucceed") )
		self:OnBtnClose()
	end)

	self:InitTemplate(GUI.TemplateRegistry.ROLE_FACELIFT)	
end

function LogicGUI.TemplRoleFacelift:__delete()
	if self.__hero_gather then
		CPPGameLib.UnbindEvent(self.__hero_gather)
		self.__hero_gather = nil
	end
	if self.__change_succeed then
		CPPGameLib.UnbindEvent(self.__change_succeed)
		self.__change_succeed = nil
	end
end

function LogicGUI.TemplRoleFacelift:__SettleData()
	CallModuleFunc(ModuleType.HERO, "HeroCollectionReq")
end

function LogicGUI.TemplRoleFacelift:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--内容背景
	self.Image_content = self:GetWidget("Image_content") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") ,nil,CPPGameLib.Handler(self,self.OnBtnClose) )
end

function LogicGUI.TemplRoleFacelift:__ShowUI()
	self.listView = UIListViewEx.Create()
	self.listView:SetDirection(UIListViewEx.ScrollDirVertical)
	self.listView:SetContentSize(self.Image_content:GetContentWidth() - 5 ,self.Image_content:GetContentHeight() - 5)
	self.listView:SetPosition(2.5,2.5)
	self.Image_content:AddChild(self.listView)

	--标题
	local light = Util:Label( 
								CPPGameLib.GetString("role_info_change_headicon") , 
								Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

end

--更新数据
function LogicGUI.TemplRoleFacelift:__UpdateHero()
	self.facelift = CallModuleFunc(ModuleType.HERO,"GetFacelift")
	CPPGameLib.PrintTable(data,"物品合成响应") 

	
	local getLst = Util:SegmentationTab(self.facelift.get , 5 ) 
	local noLst = Util:SegmentationTab(self.facelift.no , 5 )

	self:Cell(getLst,noLst)
end

--取消
function LogicGUI.TemplRoleFacelift:OnBtnClose()
	self:DeleteMe()
end


--列表细胞
function LogicGUI.TemplRoleFacelift:Cell(getLst,noLst)

	for i,v in ipairs(getLst) do
		if i == 1 then
			self:SetHeadline( CPPGameLib.GetString("role_getDeblocking"))
		end
		self:SetLstIcon(v)
	end

	for i,v in ipairs(noLst) do
		if i == 1 then
			self:SetHeadline( CPPGameLib.GetString("role_didNotGetn"))
		end
		self:SetLstIcon(v)	
	end

end

--设置列表标题
function LogicGUI.TemplRoleFacelift:SetHeadline(text)
    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_property_tag")
	local sp = Util:Sprite9(path)
	sp:SetContentSize(self.listView:GetContentWidth() , 33.00)
	
	local layout = Util:Layout(sp:GetContentWidth(), sp:GetContentHeight() )

	sp:SetPosition(layout:GetContentWidth()/2 , layout:GetContentHeight()/2 )
	sp:SetAnchorPoint(0.5,0.5)
	layout:AddChild(sp)

	--标题
	local light = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	light:SetPosition(sp:GetContentWidth()/2 , sp:GetContentHeight()/2 )
	sp:AddChild(light)

	self.listView:PushBackCustomItem(layout)
end

--设置列表
function LogicGUI.TemplRoleFacelift:SetLstIcon(data)
	local layou = Util:Layout()
	layou:SetContentSize(self.listView:GetContentWidth() , 90 )
	local interval = layou:GetContentWidth() / 5
	for i,v in ipairs(data) do
		local layer = LogicGUI.IconLayer.New(true)
		layer:SetData({ id = v.id } )
		layer:SetPosition( ( (layer:GetNode():GetContentWidth()/2) + ( (interval -layer:GetNode():GetContentWidth() ) /2 ) )  + interval * (i - 1)  , layou:GetContentHeight()/2)
		layer:SetAnchorPoint(0.5,0.5)
		layer:ShowIconQuality(false)
		layou:AddChild(layer:GetNode())

		local sp = nil
		--如果是持有的
		if v.type_ == true then
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW, "icon_sp_role_Lock")

			layer:SetGray(false)
			layer:SetTouchCallback(function ( ... )
				CallModuleFunc(ModuleType.ROLE,"HeadImageReplaceReq",v.id)
			end)
		else
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW, "icon_sp_role_notLock")

			layer:SetGray(true)
			layer:SetTouchCallback(function ( ... )
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("role_didNotGetnTips"))
			end)
		end

		local sp_1 = Util:Sprite(sp)
		sp_1:SetAnchorPoint(0.5,0.5)
		sp_1:SetScaleFactor(0.88)
		sp_1:SetPosition( layer:GetNode():GetContentWidth()/2 , layer:GetNode():GetContentHeight()/2 )
		layer:GetNode():AddChild(sp_1)  

	end
	self.listView:PushBackCustomItem(layou)
end

