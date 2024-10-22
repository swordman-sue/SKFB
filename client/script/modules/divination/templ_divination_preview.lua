
--[[
%% @module: 占卜预览
%% @author: yjg
%% @created: 2017年11月16日16:29:54
--]]

Modules = Modules or {}

Modules.TemplDivinationPreview = Modules.TemplDivinationPreview or BaseClass(GUI.Template)

function Modules.TemplDivinationPreview:__init( )
	self.__is_auto_load = true
		
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_PREVIEW)	

end

function Modules.TemplDivinationPreview:__delete()
end

function Modules.TemplDivinationPreview:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_1 = self:GetWidget("Image_1")
	--列表背景
	self.Image_lstBg = self:GetWidget("Image_lstBg")
	--关闭
	self.Button_close = Util:WidgetButton(self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
end

function Modules.TemplDivinationPreview:__ShowUI( )

	--标题
	local light = Util:Label( CPPGameLib.GetString("divination_preview") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_1:GetContentWidth()/2 , self.Image_1:GetContentHeight()/2 )
	self.Image_1:AddChild(light)

	self.listView = UIListViewEx.Create()
	self.listView:SetDirection(UIListViewEx.ScrollDirVertical)
	self.listView:SetContentSize(self.Image_lstBg:GetContentWidth() - 5 ,self.Image_lstBg:GetContentHeight() - 5)
	self.listView:SetPosition(2.5,2.5)
	self.Image_lstBg:AddChild(self.listView)

	local cnf = CPPGameLib.GetConfig("Card", nil , nil , true )
	local lst = {}
	for i,v in pairs(cnf) do
		if not lst[v.quality + 1 ] then
			lst[v.quality + 1 ] = {}
		end
		v.item_id = i
		table.insert( lst[v.quality + 1 ] , v )
	end

	for i , v in ipairs( lst ) do
		self:Setheadline( v )

		self:SetIconLst( v )
	end
end

--标题
function Modules.TemplDivinationPreview:Setheadline( data )
    local headlineLayout = Util:Layout(self.Image_lstBg:GetContentWidth() , 30)
    headlineLayout:SetAnchorPoint(0.5, 0.5)

    local divination_quality = Util:Label( CPPGameLib.GetString("divination_quality" .. data[1].quality + 1 )  , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
    divination_quality:SetPosition(headlineLayout:GetContentWidth()/2 , headlineLayout:GetContentHeight()/2)
    divination_quality:SetAnchorPoint(0.5,0.5)
    headlineLayout:AddChild(divination_quality)    

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")
    local sp_1 = Util:Sprite9(sp)
    sp_1:SetContentSize( sp_1:GetContentWidth() , sp_1:GetContentHeight() )
    sp_1:SetFlipX(true)
    sp_1:SetAnchorPoint(0.5,0.5)
    sp_1:SetPosition( headlineLayout:GetContentWidth()/2 + divination_quality:GetContentWidth()/2 + sp_1:GetContentWidth()/2 + 5 , divination_quality:GetPositionY() )
    headlineLayout:AddChild(sp_1)  

    local sp_1 = Util:Sprite9(sp)
    sp_1:SetContentSize( sp_1:GetContentWidth() , sp_1:GetContentHeight() )
    sp_1:SetAnchorPoint(0.5,0.5)
    sp_1:SetPosition( headlineLayout:GetContentWidth()/2 - divination_quality:GetContentWidth()/2 - sp_1:GetContentWidth()/2 - 5 , divination_quality:GetPositionY() )
    headlineLayout:AddChild(sp_1)  

	self.listView:PushBackCustomItem(headlineLayout)
end

--内容
function Modules.TemplDivinationPreview:SetIconLst( lst )
	local info = Util:SegmentationTab( lst , 6 )
	for i , v in ipairs( info ) do
		local headlineLayout = Util:Layout(self.Image_lstBg:GetContentWidth() , 100 )
	    headlineLayout:SetAnchorPoint(0.5, 0.5)
		for ii , vv in ipairs( v ) do
		    --图标
		    local icon_layer = LogicGUI.IconLayer.New(true)
		    icon_layer:SetData({id = vv.item_id })
		    icon_layer:SetPosition( 7 + ( (ii-1) * 85 ) , headlineLayout:GetContentHeight()/2 )
		    icon_layer:SetAnchorPoint( 0 , 0.5 )
		    headlineLayout:AddChild(icon_layer:GetNode() )
		end
	    self.listView:PushBackCustomItem(headlineLayout)
	end
end

function Modules.TemplDivinationPreview:OnButtonClose( )
	self:DeleteMe()
end