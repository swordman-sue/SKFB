
--[[
%% @module: 好友列表
%% @author: yjg
%% @created: 2016年10月8日09:52:19
--]]

Modules = Modules or {}

Modules.ManorFriendView = Modules.ManorFriendView or BaseClass(GUI.Template)

function Modules.ManorFriendView:__init(node)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
		CallModuleFunc(ModuleType.MANOR, "FriendLandListReq")
	end

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_MANOR_FRIEND_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_MANOR_FRIEND)

	self.__friend_event = self:BindGlobalEvent(Macros.Event.ManorModule.MANOR_PATROL_FRIEND,function (_,lst)
		self:SetLst(lst)
	end)	
end

function Modules.ManorFriendView:__Dispose()
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function Modules.ManorFriendView:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--关闭
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--内容背景
	self.Image_contentBg = self:GetWidget("Image_contentBg")
end

function Modules.ManorFriendView:__ShowUI()

	local manor_friendLst = Util:Label(CPPGameLib.GetString("manor_friendLst") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	manor_friendLst:SetAnchorPoint(0.5 , 0.5)
	manor_friendLst:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )         
	self.Image_light:AddChild(manor_friendLst)

	local params =  
	{
	  	item_class = Modules.ManorFriendViewItem,
	  	item_width = 536.00,
	  	item_height = 132.00,
	  	row = 4,
	  	col = 1,
	  	item_space_c = 5,
	  	view_height = self.Image_contentBg:GetContentHeight() - 10 ,
        selectable = true,
        select_callback = function(item_index, item_data)
        	CallModuleFunc(ModuleType.MANOR, "PlayerLandDataReq",item_data.role_id)
        	self:OnButtonClose()
        end,
	}

	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.Image_contentBg:AddChild(self.__zones_scrollview:GetNode())
	PosTool.Center(self.__zones_scrollview:GetNode())

	--触摸层
	self.touch = Util:Layout( 100 , 50 , function ( ... )
		CallModuleFunc( ModuleType.FRIEND , "OpenView" , BaseViewType.FRIEND , Macros.Game.FRIEND_TYPE.ADD_FRIEND )
		self:OnButtonClose()
	end)
	self.touch:SetAnchorPoint( 0.5,0.5 )
    self.Image_contentBg:AddChild( self.touch )
    PosTool.Center( self.touch )

    local tips_1 = Util:Label( CPPGameLib.GetString("manor_noFriend") , Macros.TypefaceSize.largish , Macros.Color.content )
    self.touch:AddChild( tips_1 )
	PosTool.Center( tips_1 , 0 , 20 )

    local tips_2 = Util:Label( CPPGameLib.GetString("manor_GoAdd") , Macros.TypefaceSize.largish , Macros.Color.content )
    self.touch:AddChild( tips_2 )
	PosTool.Center( tips_2 , 0 , -20 )

	self.touch:SetVisible(false)
end

function Modules.ManorFriendView:SetLst( lst )
	print("xy" , "==========SetLst")


	if #lst ~= 0 then
		self.__zones_scrollview:SetDataList(lst)
		self.touch:SetVisible(false)
	else
		self.touch:SetVisible(true)
	end
end

function Modules.ManorFriendView:OnButtonClose( info )
	self:DeleteMe()
end



Modules = Modules or {}
Modules.ManorFriendViewItem = Modules.ManorFriendViewItem or BaseClass(GUI.ScrollItem)

function Modules.ManorFriendViewItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_MANOR_FRIEND_ITEM) 
end

function Modules.ManorFriendViewItem:__delete()
end

function Modules.ManorFriendViewItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
	self:__ShowUI(data)
end

function Modules.ManorFriendViewItem:__AddToParent()
end

function Modules.ManorFriendViewItem:__Getwidget( data )
	--条目背景
	self.Image_itemBg = self:GetWidget("Image_itemBg")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--等级
	self.Text_lv = self:GetWidget("Text_lv")
	--拜访
	self.Button_Go = Util:WidgetButton( 
										self:GetWidget("Button_Go") , 
										CPPGameLib.GetString("manor_see") , 
										CPPGameLib.Handler(self,self.OnButtonGo) ,
										nil,nil,Macros.Color.button_yellow
										)
end

function Modules.ManorFriendViewItem:__ShowUI( data )
	--头像
    local img_head = LogicGUI.RoleIcon.New(true)
    img_head:SetData(data.head_icon)
    img_head:SetAnchorPoint(0.5,0.5)
    img_head:SetPosition(self.Panel_icon:GetContentWidth()/2 , self.Panel_icon:GetContentHeight()/2 )
    self.Panel_icon:AddChild(img_head:GetNode())

    --名字
    local name = Util:Label( data.name , Macros.TypefaceSize.normal , Macros.Color.headline )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(name)

	--等级
    local lv = Util:Label( CPPGameLib.GetString("common_lev9" , data.level) , Macros.TypefaceSize.normal , Macros.Color.headline )
    lv:SetAnchorPoint(0,0.5)
    lv:SetPosition( 0 , self.Text_lv:GetContentHeight()/2 )
    self.Text_lv:AddChild(lv)

    --巡逻中的遗迹
    local manor_friendUnderway = Util:Label( CPPGameLib.GetString("manor_friendUnderway") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    manor_friendUnderway:SetAnchorPoint( 0 , 0.5 )
    manor_friendUnderway:SetPosition( 5 , self.Image_content:GetContentHeight()/2 + 15 )
    self.Image_content:AddChild(manor_friendUnderway)

    --巡逻中的数量
    local value = Util:Label( data.patrol_city_num , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.Image_content:AddChild(value)
    PosTool.RightTo( manor_friendUnderway , value )

    --暴动中的遗迹
    local manor_friendRebel = Util:Label( CPPGameLib.GetString("manor_friendRebel") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    manor_friendRebel:SetAnchorPoint( 0 , 0.5 )
    manor_friendRebel:SetPosition( 5 , self.Image_content:GetContentHeight()/2 - 15 )
    self.Image_content:AddChild(manor_friendRebel)

    --暴动中的数量
    local value = Util:Label( data.riot_city_num , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.Image_content:AddChild(value)
    PosTool.RightTo( manor_friendRebel , value )

end

function Modules.ManorFriendViewItem:GetTouchNode()
    return self.Button_Go
end