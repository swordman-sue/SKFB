--
-- @brief 卡牌获得
-- @author: yjg
-- @date: 2017年12月4日20:11:10
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationCollect = LogicGUI.TemplDivinationCollect or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationCollect:__init( id , star , func )

    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
        
    self.__open_callback = function ()
        self.id = id
        self.star = star
        self.func = func
        self:__Getwidget()
        self:__ShowUI()
    end 

    self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_COLLECT)
end

function LogicGUI.TemplDivinationCollect:__delete()

end

function LogicGUI.TemplDivinationCollect:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --确定
    self.Button_confirm =  Util:WidgetButton( 
                                                self:GetWidget("Button_confirm") , 
                                                CPPGameLib.GetString("common_confirm") , 
                                                CPPGameLib.Handler(self,self.OnButtonConfirm) , 
                                                nil,nil,Macros.Color.button_yellow
                                            )
    
    self.Panel_star = self:GetWidget("Panel_star") 

    self.Panel_icon = self:GetWidget("Panel_icon") 

    self.Panel_text = self:GetWidget("Panel_text") 

    self.Panel_headline = self:GetWidget("Panel_headline") 

end

function LogicGUI.TemplDivinationCollect:__ShowUI()
    if self.star <= 1 then
        local effectid, effect = self:__CreateEffect({res_name = "UI_zhanbujihuo_language_chs",loop = true}, self.Panel_headline,2)
        PosTool.Center(effect)
    else
        local effectid, effect = self:__CreateEffect({res_name = "UI_zhanbushengxing_language_chs",loop = true}, self.Panel_headline,2)
        PosTool.Center(effect)
    end


    local cnf = CPPGameLib.GetConfig("CollectionGroup", self.id )
    --名字
    local name = Util:Label( cnf.name ..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.largish , Macros.Color.golden )
    name:SetAnchorPoint( 1 , 0.5 )
    name:SetPosition( 0 , self.Panel_star:GetContentHeight()/2 )
    self.Panel_star:AddChild( name )

    local index = 1
    if self.star ~= 0 then
        index = self.star 
    end
    --星
    for i = 1 , 7 do
        --星星
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_sky_star" )
        local write = Util:Sprite( sp )
        write:SetAnchorPoint( 0.5 , 0.5 )
        write:SetPosition( 30 + ( (i-1) * 30 ) , self.Panel_star:GetContentHeight()/2 )
        self.Panel_star:AddChild( write , 1 )
        
        if i <= index then
            local effectid, effect = self:__CreateEffect({res_name = "UI_zhanbudaxing",loop = true}, write,2)
            PosTool.Center(effect)
            
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "bigstar" )
            local star = Util:Sprite( sp )
            star:SetAnchorPoint( 0.5 , 0.5 )
            write:AddChild( star , 1 )
            PosTool.Center( star )
        end
    end

    --图标
    self.Panel_icon:SetContentSize( #cnf.cost_card_list * 110 , 90 )
    local interval = self.Panel_icon:GetContentWidth() / #cnf.cost_card_list

    for i,v in ipairs( cnf.cost_card_list ) do
        local icon = LogicGUI.IconLayer.New(false)
        icon:SetPosition( 55 + interval * (i- 1)  , self.Panel_icon:GetContentHeight()/2 )
        icon:SetData({ id = v })
        icon:SetAnchorPoint(0.5,0.5)
        self.Panel_icon:AddChild(icon:GetNode())
    end

    local agoPro = CallModuleFunc(ModuleType.DIVINATION,"ToGroupIdGetProLst" , nil , self.id , self.star - 1 )
    local nowPro = CallModuleFunc(ModuleType.DIVINATION,"ToGroupIdGetProLst" , nil , self.id , self.star )

    --计算长度
    local labelW = 0
    for i , v in ipairs( nowPro ) do
        local type_ , value_ = Util:Nature( v.type , v.value , true , true )
        local divination_cardType = Util:Label( type_ ..  CPPGameLib.GetString("common_colon") .. value_ , Macros.TypefaceSize.normal )
        if labelW < divination_cardType:GetContentWidth() then
            labelW = divination_cardType:GetContentWidth() + 30
        end
    end

    --属性
    for i , v in ipairs( nowPro ) do
        local type_ , value_ = Util:Nature( v.type , v.value , true , true )
        
        --箭头
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "smallarrow" )
        local smallarrow = Util:Sprite( sp )
        smallarrow:SetAnchorPoint( 0.5 , 0.5 )
        smallarrow:SetPosition( (self.Panel_text:GetContentWidth()/2) + 10 , self.Panel_text:GetContentHeight() - 10 - ( ( i-1 ) * 25 ) )
        self.Panel_text:AddChild(smallarrow)

        --类型
        local divination_cardType = Util:Label( type_ ..  CPPGameLib.GetString("common_colon"), Macros.TypefaceSize.normal )
        divination_cardType:SetAnchorPoint( 0 , 0.5 )
        divination_cardType:SetPosition( smallarrow:GetPositionX() - labelW , smallarrow:GetPositionY() )
        self.Panel_text:AddChild( divination_cardType )

        print("xy" , "===self.star===" , self.star )
        --上次的属性
        if self.star == 0 then
            local type_ , value_ = Util:Nature( v.type , 0 , true , true )
            local value = Util:Label( value_ , Macros.TypefaceSize.normal )
            self.Panel_text:AddChild( value )
            PosTool.RightTo( divination_cardType , value )
        else
            local type_ , value_ = Util:Nature( v.type , agoPro[i].value , true , true )
            local value = Util:Label( value_ , Macros.TypefaceSize.normal )
            self.Panel_text:AddChild( value )
            PosTool.RightTo( divination_cardType , value )
        end

        --当前属性
        local value = Util:Label( value_ , Macros.TypefaceSize.normal , Macros.Color.green)
        self.Panel_text:AddChild( value )
        PosTool.RightTo( smallarrow , value , 10 )
    end
end

function LogicGUI.TemplDivinationCollect:OnButtonClose()
    self:DeleteMe()
end

function LogicGUI.TemplDivinationCollect:OnButtonConfirm()
    if self.func then
        self.func( self.onCbx )
    end
    self:DeleteMe()
end




