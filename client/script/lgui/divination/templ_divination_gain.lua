--
-- @brief 卡牌获得
-- @author: yjg
-- @date: 2017年12月4日20:11:10
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationGain = LogicGUI.TemplDivinationGain or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationGain:__init( id , func )

    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
        
    self.__open_callback = function ()
        self.id = id
        self.func = func
        self:__Getwidget()
        self:__ShowUI()
    end 

    self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_GAIN)
end

function LogicGUI.TemplDivinationGain:__delete()

end

function LogicGUI.TemplDivinationGain:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --内容框
    self.Image_gxhd = self:GetWidget("Image_gxhd") 
    --确定
    self.Button_confirm =  Util:WidgetButton( 
                                                self:GetWidget("Button_confirm") , 
                                                CPPGameLib.GetString("common_confirm") , 
                                                CPPGameLib.Handler(self,self.OnButtonConfirm) , 
                                                nil,nil,Macros.Color.button_yellow
                                            )

end

function LogicGUI.TemplDivinationGain:__ShowUI()
    self.Image_gxhd:SetVisible(false)

    
    local effect_id, effect = self:__CreateEffect({id = 100033}, self.Image_bg , - 2 )
    effect:SetPosition( self.Image_gxhd:GetPositionX() , self.Image_gxhd:GetPositionY() )

    local gxhd_1 = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "gxhd_1" , true ) )
    gxhd_1:SetPosition( self.Image_gxhd:GetPositionX() , self.Image_gxhd:GetPositionY() )
    gxhd_1:SetAnchorPoint( 0.5,0.5 )
    self.Image_bg:AddChild( gxhd_1 , 99 )


    --图标
    local _ , cnf = ObjIDTool.GetInfo( self.id  )
    local path = Resource.PathTool.GetBigIconPath( cnf.bigicon )
    local icon = Util:Sprite( path )
    icon:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_bg:AddChild( icon )
    PosTool.Center( icon , -180 , 0 )
    icon:SetScaleFactor( 0.5 )

    --品质框    
    local frame = Util:Sprite( Resource.PathTool.GetUIResPath("divinationbig/icon_sp_card_".. cnf.quality ) )
        -- Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_card_" .. cnf.quality ) )
    frame:SetAnchorPoint( 0.5 , 0.5 )
    icon:AddChild( frame , 2 )
    PosTool.Center( frame )

    --名字
    local name = Util:Name( cnf.name , Macros.TypefaceSize.tab , cnf.quality )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( 60 + 70 + 13 , 35 )
    frame:AddChild( name )

    --背光
    local iconId , iconLed = TypefaceEffects.New():Spotlight( self , icon, cnf.quality )
    iconLed:SetPosition( icon:GetContentWidth()/2 , 0 )
    iconLed:SetScaleFactor(1.8)

    --名字
    local divination_cardName = Util:Label( CPPGameLib.GetString("divination_cardName") , Macros.TypefaceSize.largish , Macros.Color.golden )
    divination_cardName:SetAnchorPoint( 0 , 0.5 )
    divination_cardName:SetPosition( self.Image_bg:GetContentWidth()/2 , self.Image_bg:GetContentHeight() - 50 )
    self.Image_bg:AddChild( divination_cardName )

    --名字
    local cardName = Util:Name( cnf.name , Macros.TypefaceSize.largish , cnf.quality )
    self.Image_bg:AddChild( cardName )
    PosTool.RightTo( divination_cardName , cardName )

    --类型
    local divination_cardType = Util:Label( CPPGameLib.GetString("divination_cardType") , Macros.TypefaceSize.largish , Macros.Color.golden )
    divination_cardType:SetAnchorPoint( 0 , 0.5 )
    divination_cardType:SetPosition( divination_cardName:GetPositionX() , divination_cardName:GetPositionY() - 40 )
    self.Image_bg:AddChild( divination_cardType )

    --类型
    local cardType = Util:Name( CPPGameLib.GetString("divination_cardType_" .. cnf.type2 )  , Macros.TypefaceSize.largish )
    self.Image_bg:AddChild( cardType )
    PosTool.RightTo( divination_cardType , cardType )

    --属性
    local divination_cardProperty = Util:Label( CPPGameLib.GetString("divination_cardProperty") , Macros.TypefaceSize.largish , Macros.Color.golden )
    divination_cardProperty:SetAnchorPoint( 0 , 0.5 )
    divination_cardProperty:SetPosition( divination_cardType:GetPositionX() , divination_cardType:GetPositionY() - 40 )
    self.Image_bg:AddChild( divination_cardProperty )


    local text = ""
    for i , v in ipairs( cnf.base_pro_type_list ) do
        local type_ , value_ =  Util:Nature( v , cnf.base_pro_value_list[i] , true )
        text = text .. type_ .. value_ .. "  "
    end
    --属性
    local cardProperty = Util:Name( text , Macros.TypefaceSize.largish )
    self.Image_bg:AddChild( cardProperty )
    PosTool.RightTo( divination_cardProperty , cardProperty )

    --描述
    local divination_cardDescribe = Util:Label( CPPGameLib.GetString("divination_cardDescribe") , Macros.TypefaceSize.largish , Macros.Color.golden )
    divination_cardDescribe:SetAnchorPoint( 0 , 0.5 )
    divination_cardDescribe:SetPosition( divination_cardProperty:GetPositionX() , divination_cardProperty:GetPositionY() - 40 )
    self.Image_bg:AddChild( divination_cardDescribe )

    --属性
    local cardDescribe = Util:Name( cnf.description , Macros.TypefaceSize.largish , nil , nil , 300 )
    cardDescribe:SetAnchorPoint( 0 , 1 )
    cardDescribe:SetPosition( cardProperty:GetPositionX() , cardProperty:GetPositionY() - 25  )
    self.Image_bg:AddChild( cardDescribe )
    cardDescribe:SetTextHorizontalAlign(LabelEx.ALIGIN_LEFT)

end

function LogicGUI.TemplDivinationGain:OnButtonClose()
    self:DeleteMe()
end

function LogicGUI.TemplDivinationGain:OnButtonCancel()
    self:DeleteMe()
end

function LogicGUI.TemplDivinationGain:OnButtonConfirm()
    if self.func then
        self.func( self.onCbx )
    end
    self:DeleteMe()
end




