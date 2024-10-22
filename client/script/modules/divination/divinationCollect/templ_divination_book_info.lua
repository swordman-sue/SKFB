--[[
%% @module: 卡牌条目详情
%% @author: yjg
%% @created: 2017年11月27日11:22:27
--]]


LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationBookInfo = LogicGUI.TemplDivinationBookInfo or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationBookInfo:__init( )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__Getwidget()
	    self:__ShowUI()
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_BOOK_INFO) 
end

function LogicGUI.TemplDivinationBookInfo:__delete()
end

function LogicGUI.TemplDivinationBookInfo:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--内容背景
	self.Image_content = self:GetWidget("Image_content") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TemplDivinationBookInfo:__ShowUI()
	--标题
	self.light = Util:Label( CPPGameLib.GetString("divination_starAdd") , Macros.TypefaceSize.popup  )
	self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 + 3)
	self.Image_light:AddChild(self.light)

	self.listView = UIListViewEx.Create()
	self.listView:SetDirection(UIListViewEx.ScrollDirVertical)
	self.listView:SetContentSize(self.Image_content:GetContentWidth() - 5 ,self.Image_content:GetContentHeight() - 5)
	self.listView:SetPosition(0 ,2.5)
	self.Image_content:AddChild(self.listView)
end

--设置名字
function LogicGUI.TemplDivinationBookInfo:SetName( name )
	-- self.light:SetString(name)
end

function LogicGUI.TemplDivinationBookInfo:OnButtonClose()
	self:DeleteMe()
end

function LogicGUI.TemplDivinationBookInfo:SetCell(cnf)
	if cnf then
	    for i,v in ipairs(cnf) do
	        self:Cell( v )
	    end
	end
end

function LogicGUI.TemplDivinationBookInfo:Cell( data )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_label_frame")
    local sp_1 = Util:Sprite9(sp)

	--收藏详情
    local headlineLayout = Util:Layout(self.Image_content:GetContentWidth() ,  sp_1:GetContentHeight() + 5 )
    headlineLayout:SetAnchorPoint(0.5, 0.5)

    sp_1:SetContentSize( headlineLayout:GetContentWidth() - 10 , sp_1:GetContentHeight() )
    sp_1:SetAnchorPoint(0.5,0.5)
    sp_1:SetPosition( headlineLayout:GetContentWidth()/2 , headlineLayout:GetContentHeight()/2 )
    headlineLayout:AddChild(sp_1)  


    local attribute = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_equip_attribute")
    local content = Util:Sprite9(attribute)
    content:SetAnchorPoint( 0.5 , 0.5 )
    content:SetContentSize( sp_1:GetContentWidth() - 20 , sp_1:GetContentHeight() - 45 )
    content:SetPosition( sp_1:GetContentWidth()/2 , sp_1:GetContentHeight()/2 - 13 )
    sp_1:AddChild( content )
    
    local divination_starValue = Util:Label( CPPGameLib.GetString("divination_starValue" , data.star ) , Macros.TypefaceSize.largish , Macros.Color.headline )
    divination_starValue:SetAnchorPoint( 0 , 1 )
    divination_starValue:SetPosition( 20 , headlineLayout:GetContentHeight() - 10 )
    headlineLayout:AddChild(divination_starValue)  


    local divination_CollectValue = Util:Label( CPPGameLib.GetString("divination_CollectValue" , data.collectionValue ) , Macros.TypefaceSize.normal , Macros.Color.headline )
    divination_CollectValue:SetAnchorPoint( 1 , 1 )
    divination_CollectValue:SetPosition( headlineLayout:GetContentWidth() - 20 , headlineLayout:GetContentHeight() - 10 )
    headlineLayout:AddChild(divination_CollectValue)  

    for i , v in ipairs( data.pro ) do
    	local type_ , value_ = Util:Nature( v.type , v.value , true )
	    local pro = Util:Label( type_ .. CPPGameLib.GetString("common_colon") .. value_ , Macros.TypefaceSize.normal , Macros.Color.content )
	    pro:SetAnchorPoint( 0 , 0.5 )
	    pro:SetPosition( 10 + ( (content:GetContentWidth()/2) * (i-1) ) , content:GetContentHeight()/2 )
	    content:AddChild(pro)  
    end





    self.listView:PushBackCustomItem(headlineLayout)


    -- local divination_starCollectValue = Util:Label( CPPGameLib.GetString("divination_starCollectValue" , data.star , data.collectionValue 

	-- local headlineLayout = Util:Layout(self.Image_content:GetContentWidth() , 110 )
	-- headlineLayout:SetAnchorPoint(0.5, 0.5)





 --    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp9_division")
 --    local sp_1 = Util:Sprite9(sp)
 --    sp_1:SetContentSize( headlineLayout:GetContentWidth() - 10, sp_1:GetContentHeight() )
 --    sp_1:SetAnchorPoint(0.5,0.5)
 --    sp_1:SetPosition( headlineLayout:GetContentWidth()/2 , 2 )
 --    headlineLayout:AddChild(sp_1)  

 --    self.listView:PushBackCustomItem(headlineLayout)





end
