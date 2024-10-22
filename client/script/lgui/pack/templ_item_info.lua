
--
-- @brief 物品信息
-- @author: ldx
-- @date: 2016年11月7日11:19:29
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplItemInfo = LogicGUI.TemplItemInfo or BaseClass(GUI.Template)

function LogicGUI.TemplItemInfo:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self:__ShowUI(data)
    end
	self:InitTemplate(GUI.TemplateRegistry.ITEM_INFO)
end

function LogicGUI.TemplItemInfo:__delete()

end

function LogicGUI.TemplItemInfo:__ShowUI( data )
    local type,info = ObjIDTool.GetInfo(data.id)
    --点击屏幕其他地方关闭界面
    WidgetUnity.SetWidgetListener(self:GetNode(),nil,nil,function ()
        self:DeleteMe()
    end,nil)

    --头像 
    local node_head = self:GetWidget("Panel_headIcon")
    local icon_layer = LogicGUI.IconLayer.New(false,false)
    icon_layer:SetData({id = data.id})
    node_head:AddChild(icon_layer:GetNode())
    -- icon_layer:adaptParentContentSize()
    PosTool.Center(icon_layer)            

    --名字
 	local Panel_name = self:GetWidget("Panel_name")
    local lab_name = Util:Name(info.name,20,info.quality)
    lab_name:SetAnchorPoint(0,0.5)
    Panel_name:AddChild(lab_name) 
    PosTool.LeftCenter(lab_name)

    --数量文字
    Util:WidgetLabel(self:GetWidget("Text_num"),CPPGameLib.GetString("pack_num"), Macros.TypefaceSize.slightly ,Macros.Color.lilyWhite )
    --拥有数量
    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", data.id)

    if info.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
        item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , info.param1 )
    end
    Util:WidgetLabel(self:GetWidget("Text_count"),item_num, Macros.TypefaceSize.slightly ,Macros.Color.lilyWhite)

    --说明
    local Image_bg = self:GetWidget("Image_bg")
    local lab_explain = Util:RichText(info.description , Macros.TypefaceSize.slightly , Image_bg:GetContentWidth() - 30 ,0)
    lab_explain:SetPosition( icon_layer:GetNode():GetPositionX() - 8 , Image_bg:GetContentHeight()/2 - 15 )
    lab_explain:SetAnchorPoint(0,1)
    Image_bg:AddChild(lab_explain)
end




--
-- @brief 物品信息
-- @author: yjg
-- @date: 2018年5月29日10:39:04
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplGiftInfo = LogicGUI.TemplGiftInfo or BaseClass(GUI.Template)

function LogicGUI.TemplGiftInfo:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self:__ShowUI(data)
    end
    self:InitTemplate(GUI.TemplateRegistry.GIFT_INFO)
end

function LogicGUI.TemplGiftInfo:__delete()
end

function LogicGUI.TemplGiftInfo:__ShowUI( data )
    self.layout = Util:Layout( Macros.Global.DesignSize.width , Macros.Global.DesignSize.height ,function ( ... )
        self:DeleteMe()
    end)
    self.layout:SetAnchorPoint(0.5,0.5)
    self.__root_widget:AddChild( self.layout )
    PosTool.Center( self.layout )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_pack_tips_2")
    local bg = Util:Sprite9( sp )
    bg:SetContentSize( 512 , 320 )
    bg:SetAnchorPoint( 0.5 , 1 )
    bg:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 + (bg:GetContentHeight()/2) )
    self.layout:AddChild( bg )

    local type,info = ObjIDTool.GetInfo(data.id)
    --礼包
    if info.item_id_list and #info.item_id_list > 4 then
        bg:SetContentSize( 512 , 370 )
    end

    self.pack_tips = Util:Layout( bg:GetContentWidth() , bg:GetContentHeight() ,function ( ... )

    end)
    self.pack_tips:SetAnchorPoint(0.5,1)
    self.pack_tips:SetPosition( bg:GetPositionX() , bg:GetPositionY() )
    self.layout:AddChild( self.pack_tips )

    --头像 
    local icon_layer = LogicGUI.IconLayer.New( false )
    icon_layer:SetData({id = data.id})
    self.pack_tips:AddChild(icon_layer:GetNode())
    icon_layer:SetAnchorPoint( 0 , 1 )
    icon_layer:SetPosition( 40 , self.pack_tips:GetContentHeight() - 20 )
    icon_layer:SetScaleFactor( 0.9 )

    --名字
    local lab_name = Util:Name(info.name,20,info.quality)
    lab_name:SetAnchorPoint( 0 , 1 )
    lab_name:SetPosition( icon_layer:GetNode():GetPositionX() + icon_layer:GetNode():GetContentWidth() + 20 , icon_layer:GetNode():GetPositionY() )
    self.pack_tips:AddChild(lab_name) 

    --拥有数量
    local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", data.id)
    if info.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
        item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , info.param1 )
    end

    local lab_name = Util:Label( CPPGameLib.GetString("pack_num") .. item_num , 20 )
    lab_name:SetAnchorPoint( 0 , 0 )
    lab_name:SetPosition( icon_layer:GetNode():GetPositionX() + icon_layer:GetNode():GetContentWidth() + 20 , icon_layer:GetNode():GetPositionY() - icon_layer:GetNode():GetContentHeight() + 15 )
    self.pack_tips:AddChild(lab_name) 

    --说明
    local lab_explain = Util:RichText(info.description , Macros.TypefaceSize.slightly , self.pack_tips:GetContentWidth() - 30 ,0)
    lab_explain:SetPosition( 15 , self.pack_tips:GetContentHeight() - 100 )
    lab_explain:SetAnchorPoint(0,1)
    self.pack_tips:AddChild(lab_explain)

    --内容
    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
    local black1_bg = Util:Sprite9(black1_bg_sp)
    black1_bg:SetAnchorPoint( 0.5 , 1 )
    black1_bg:SetPosition( self.pack_tips:GetContentWidth()/2  , self.pack_tips:GetContentHeight() - 170 )
    black1_bg:SetContentSize( self.pack_tips:GetContentWidth() - 30 , 135 )
    self.pack_tips:AddChild( black1_bg , 20 )

    --礼包
    if info.item_id_list and #info.item_id_list > 4 then
        black1_bg:SetContentSize( black1_bg:GetContentWidth() , 185 )
    end

    local t1 = {}
    for i , v in ipairs( info.item_id_list ) do
        table.insert( t1 , { id = v , num = info.item_min_num_list[i] , max = info.item_max_num_list[i] } )
    end

    local params =  
    {
        item_class = Modules.TemplGiftInfoItem,
        item_width = black1_bg:GetContentWidth()/4 ,
        item_height = 130,
        row = 1,
        col = 4,
        view_height = black1_bg:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    black1_bg:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
    self.lst_list:SetDataList( t1 )
end



Modules = Modules or {}
Modules.TemplGiftInfoItem = Modules.TemplGiftInfoItem or BaseClass(GUI.ScrollItem)

function Modules.TemplGiftInfoItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplGiftInfoItem:__CreateWidgetFromLuaCode()  
    self.layout = Util:Layout(120.5 , 130)
    return self.layout
end

function Modules.TemplGiftInfoItem:__delete()
end

function Modules.TemplGiftInfoItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    --头像 
    local icon_layer = LogicGUI.IconLayer.New()
    icon_layer:SetData( {id = data.id , num = "" } )

    if data.max and data.num ~= data.max then
        icon_layer:SetNum( data.num.."~" .. data.max )
    elseif data.num <= 1 then
    else
        icon_layer:SetNum( data.num )
    end

    self.layout:AddChild(icon_layer:GetNode())
    icon_layer:SetAnchorPoint( 0.5 , 0.5 )
    icon_layer:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 + 23 )
    icon_layer:SetScaleFactor( 0.9 )

    local type,info = ObjIDTool.GetInfo(data.id)
    local name = Util:Name( info.name , nil , info.init_quality or info.quality , nil , self.layout:GetContentWidth() )
    name:SetAnchorPoint( 0.5 , 1 )
    name:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 - 17 )
    self.layout:AddChild( name )
end