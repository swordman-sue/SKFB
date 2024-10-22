 --
-- @brief vip内容
-- @author: yjg
-- @date: 2016年8月17日21:38:39
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplVipItem = LogicGUI.TemplVipItem or BaseClass(GUI.Template)

function LogicGUI.TemplVipItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_VIP_ITEM)  
    self:__Getwidget()

end

function LogicGUI.TemplVipItem:__RegistAllEvents()
    -- 礼包刷新
    self:BindGlobalEvent(Macros.Event.VIPModule.VIP_BUY, function(_, obj_id)
        self:OnButtonType()
    end)

    --零点刷新
    self:BindGlobalEvent(Macros.Event.ShopModule.BUY_SUCCESS, function( _,data )
        if self.__good_data.goods_id == data.param1 then
            local info = CallModuleFunc(ModuleType.VIP,"GetShopBuy")
            table.insert( info , { goods_id = data.param1 , buy_num = data.param2 } )
            CallModuleFunc(ModuleType.VIP,"ShopBuyRecordRequestRqsp",info)
            self:OnButtonType()  
        end
    end)
end

function LogicGUI.TemplVipItem:__AddToParent()
    
end

function LogicGUI.TemplVipItem:__delete()
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function LogicGUI.TemplVipItem:SetData(data)
    GUI.Template.SetData(self , data)
    self:__ShowUI()
end

function LogicGUI.TemplVipItem:__Getwidget( ... )
    --标题
    self.Image_1 = self:GetWidget("Image_1") 
    self.Image_1:IgnoreContentAdaptWithSize(true)

    --背景容器
    self.Panel_15 = self:GetWidget("Panel_15") 
    --特权列表
    self.Image_content = self:GetWidget("Image_content") 
    --丝带
    self.Image_sash = self:GetWidget("Image_sash") 
    --礼包
    self.Image_content_1 = self:GetWidget("Image_content_1") 
    --丝带
    -- self.Image_sash_1 = self:GetWidget("Image_sash_1") 
    --去获取
    self.Button_goGet = Util:WidgetButton( 
                                        self:GetWidget("Button_goGet") , 
                                        CPPGameLib.GetString("vip_get") , 
                                        CPPGameLib.Handler(self,self.OnButtonGoGet),
                                        nil,Macros.Color.btn,Macros.Color.button_red
                                    )
    --原价
    self.Text_raw = self:GetWidget("Text_raw") 
    --现价
    self.Text_now = self:GetWidget("Text_now") 
end

function LogicGUI.TemplVipItem:__ShowUI( ... )

    self:SetLv( self.Image_sash )

    -- self:SetLv( self.Image_sash_1 , true )

    --特权文字描述
    local lst_list = UIListViewEx.Create()
    lst_list:SetContentSize(self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight() - 75 )
    lst_list:SetBounceEnabled(true)
    lst_list:SetTouchSwallow(false)
    lst_list:SetPosition( 0 , 5 )
    self.Image_content:AddChild(lst_list)

    for i,v in ipairs(self.__data.lst) do
        local item = Modules.TempVIPViewItem.New(v)
        lst_list:PushBackCustomItem(item:GetNode())
    end

    local packLst = CallModuleFunc(ModuleType.VIP , "GetVipPackCnf", self.__data.level)

    --特权礼包
    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80 ,
        item_height = 80 ,
        row = 1,
        col = 4,
        item_space_r = 5,
        item_space_c = 0,
        horizon = true,
        -- view_height = 80
        view_width = ( 80 + 5 ) * 4
    }
    -- 创建参数
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.__zones_scrollview:GetNode():SetPosition(self.Image_content_1:GetContentWidth()/2 ,self.Image_content_1:GetContentHeight()/2 + 55 )
    self.__zones_scrollview:SetTouchSwallow(true)
    self.Image_content_1:AddChild(self.__zones_scrollview:GetNode())

    if not GlobalCustomizedLogicAdapterMgr:Call("CheckPrivilegedGiftOfVIP") then
        self.Image_content_1:SetVisible(false)
    end

    self.__zones_scrollview:SetDataList( packLst )

    self:SetConsume()
end

--设置彩带
function LogicGUI.TemplVipItem:SetLv( node , bag )
    self.vipExp = Util:Label( CPPGameLib.GetString("vip_levelRights" , self.__data.level ) ,Macros.TypefaceSize.normal , Macros.Color.golden )
    self.vipExp:SetPosition( node:GetContentWidth()/2 , node:GetContentHeight()/2 + 8 )
    self.vipExp:SetAnchorPoint(0.5,0.5)
    node:AddChild(self.vipExp)
end

--设置价钱
function LogicGUI.TemplVipItem:SetConsume( )
    --得到礼包id
    local itemId = CPPGameLib.GetConfig("Vip", self.__data.level).getgoods

    --找到礼包表里的这个玩意
    self.__good_data = nil
    for i,v in pairs( CPPGameLib.GetConfig("GiftShop",nil,nil,true ) ) do
        if itemId == v.item_id then
            self.__good_data = CallModuleFunc(ModuleType.SHOP, "ParseGood", Macros.Game.SHOP.STOREOEM, v)
            break
        end
    end

    if self.__good_data and self.__good_data.pur_price ~= 0 then
        --原价
        local tab = {
                    { zi = CPPGameLib.GetString("shop_before_price"),dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint },
                    { tp = Util:GetMoneyIconImgData(self.__good_data.cost_type) , dx = 0.6},
                    { zi = self.__good_data.pur_price , ys = Macros.Color.content ,jg = 5 },
                  }
        local text_raw = Util:LabCenter( tab )
        text_raw:SetPosition(0 , self.Text_raw:GetContentHeight()/2)
        text_raw:SetAnchorPoint(0,0.5)
        self.Text_raw:AddChild(text_raw)  

        --红线
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "delete_line")
        local delete_line = Util:Sprite(sp)
        delete_line:SetPosition(text_raw:GetContentWidth()/2 ,text_raw:GetContentHeight()/2 )
        delete_line:SetAnchorPoint(0.5,0.5)
        text_raw:AddChild(delete_line)

        local tab = {
                        { zi = CPPGameLib.GetString("info_cash") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal  },
                        { tp = Util:GetMoneyIconImgData(self.__good_data.cost_type) , dx = 0.6 },
                        { zi = self.__good_data.cost_value , ys = Macros.Color.content ,jg = 5 },
                    }
        --现价
        local text_now = Util:LabCenter( tab )
        text_now:SetPosition( 0 , self.Text_now:GetContentHeight()/2 )
        text_now:SetAnchorPoint(0,0.5)
        self.Text_now:AddChild(text_now)  
    end
end

--设置价钱
function LogicGUI.TemplVipItem:OnButtonGoGet( )
    self.__good_data.shop_type = Macros.Game.SHOP.STOREOEM
    LogicGUI.TemplShopBuyCountView.New(self.__good_data)
end

--设置按钮状态
function LogicGUI.TemplVipItem:OnButtonType( )
    local bool = CallModuleFunc(ModuleType.VIP,"GetShopBuyRecordRequest",self.__data.level)
    if bool == true then
        self.Button_goGet:SetVisible(false)
        self.getIcon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1",true))
        self.getIcon:SetPosition(self.Button_goGet:GetPositionX() , self.Button_goGet:GetPositionY())
        self.Image_content_1:AddChild(self.getIcon)
    end
end


--------------------------------------------------------------------------------------------------------------------------------
--字体条目
--------------------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}

Modules.TempVIPViewItem = Modules.TempVIPViewItem or BaseClass(GUI.ScrollItem)

function Modules.TempVIPViewItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.data = data
    self:InitTemplate(nil)
end

function Modules.TempVIPViewItem:__CreateWidgetFromLuaCode()

    local lab = Util:RichText( string.format( Macros.Color.content_rt , self.data )  , Macros.TypefaceSize.normal , 380.00 , 0)

    self.layout = Util:Layout()
    self.layout:SetAnchorPoint(0.5, 0.5)
    self.layout:SetContentSize( 431.00 , lab:GetContentHeight() + 20 )

    lab:SetAnchorPoint( 0 , 0.5)
    lab:SetPosition( 25.5 , self.layout:GetContentHeight()/2 )
    self.layout:AddChild(lab)

    local vip_headline = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_headline"))
    vip_headline:SetPosition( 15 , self.layout:GetContentHeight()/2 )
    self.layout:AddChild(vip_headline)    

    local vip_headline = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_segmentation"))
    vip_headline:SetPosition( self.layout:GetContentWidth()/2 , 0 )
    self.layout:AddChild(vip_headline)   
    
    return self.layout
end

function Modules.TempVIPViewItem:__delete()
end

function Modules.TempVIPViewItem:SetData(data)
end

function Modules.TempVIPViewItem:__AddToParent()
end
