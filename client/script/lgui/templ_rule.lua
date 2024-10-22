--[[
%% @module: 规则说明
%% @author: yjg
%% @created: 2016年11月22日16:41:24
--]]


LogicGUI = LogicGUI or {}

LogicGUI.TemplRule = LogicGUI.TemplRule or BaseClass(GUI.Template)

function LogicGUI.TemplRule:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__Getwidget()
	    self:__ShowUI()
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_RULE) 
end

function LogicGUI.TemplRule:__delete()
end

function LogicGUI.TemplRule:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--内容背景
	self.Image_content = self:GetWidget("Image_content") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TemplRule:__ShowUI()
	--标题
	self.light = Util:Label( CPPGameLib.GetString("common_rule") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite  )
	self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	self.Image_light:AddChild(self.light)

	self.listView = UIListViewEx.Create()
	self.listView:SetDirection(UIListViewEx.ScrollDirVertical)
	self.listView:SetContentSize(self.Image_content:GetContentWidth() - 5 ,self.Image_content:GetContentHeight() - 5)
	self.listView:SetPosition(2.5,2.5)
	self.Image_content:AddChild(self.listView)
end

--设置名字
function LogicGUI.TemplRule:SetName( name )
	self.light:SetString(name)
end

function LogicGUI.TemplRule:OnButtonClose()
	self:DeleteMe()
end

function LogicGUI.TemplRule:SetCell(cnf)
	self.listView:RemoveAllItems()

	cnf = CPPGameLib.GetConfig( tostring(cnf) , nil, nil , true )
	if cnf then
	    for i,v in ipairs(cnf) do
	        local lst = CPPGameLib.SplitEx(v.describe , "<GH>")
	        self:Cell(v.title or nil , lst)
	    end
	end
end

function LogicGUI.TemplRule:Cell( headline , data )

	if headline then
	    local headlineLayout = Util:Layout(self.Image_content:GetContentWidth() , 30)
	    headlineLayout:SetAnchorPoint(0.5, 0.5)

        local privilegeTips = Util:Label(headline , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
        privilegeTips:SetPosition(headlineLayout:GetContentWidth()/2 , headlineLayout:GetContentHeight()/2)
        privilegeTips:SetAnchorPoint(0.5,0.5)
        headlineLayout:AddChild(privilegeTips)    

        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")
        local sp_1 = Util:Sprite9(sp)
        sp_1:SetContentSize( sp_1:GetContentWidth()/2 , sp_1:GetContentHeight() )
        sp_1:SetFlipX(true)
        sp_1:SetAnchorPoint(0.5,0.5)
        sp_1:SetPosition( headlineLayout:GetContentWidth()/2 + privilegeTips:GetContentWidth()/2 + sp_1:GetContentWidth()/2 + 5 , privilegeTips:GetPositionY() )
        headlineLayout:AddChild(sp_1)  

        local sp_1 = Util:Sprite9(sp)
        sp_1:SetContentSize( sp_1:GetContentWidth()/2 , sp_1:GetContentHeight() )
        sp_1:SetAnchorPoint(0.5,0.5)
        sp_1:SetPosition( headlineLayout:GetContentWidth()/2 - privilegeTips:GetContentWidth()/2 - sp_1:GetContentWidth()/2 - 5 , privilegeTips:GetPositionY() )
        headlineLayout:AddChild(sp_1)  
	    self.listView:PushBackCustomItem(headlineLayout)
	end

	for i,v in ipairs(data) do
	    local layout = UILayoutEx.Create()
	    layout:SetAnchorPoint(0.5, 0.5)

	    local lab = Util:RichText( v , Macros.TypefaceSize.normal ,  self.Image_content:GetContentWidth() -50  ,0)
	    layout:SetContentSize(self.Image_content:GetContentWidth(), lab:GetContentHeight() + 5 )

	    lab:SetAnchorPoint(0.5,0.5)
	    lab:SetPosition(layout:GetContentWidth()/2 , layout:GetContentHeight()/2)
	    layout:AddChild(lab)

	    self.listView:PushBackCustomItem(layout)
	end

end
