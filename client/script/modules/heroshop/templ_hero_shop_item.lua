--
-- @brief 英雄商店条目 
-- @author: yjg
-- @date: 2016年8月20日18:31:32
--


Modules = Modules or {}

Modules.TemplHeroShopItem = Modules.TemplHeroShopItem or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroShopItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_HERO_SHOP_ITEM) 
    self:__Getwidget()
end

function Modules.TemplHeroShopItem:__delete()
end

function Modules.TemplHeroShopItem:SetData(data)
     GUI.ScrollItem.SetData(self, data)   
    self.data = data
    self:__ShowUI(self.data)
end

function Modules.TemplHeroShopItem:SetViewIndex(var)
    GUI.ScrollItem.SetViewIndex(self, var)

    --新手指引特殊操作检测(英雄商店-购买商品（商品索引，从1开始）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_SHOP_BUY, self.Button_buy, self:GetViewIndex())    
end

function Modules.TemplHeroShopItem:__AddToParent()
end

function Modules.TemplHeroShopItem:__Getwidget( ... )
    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --名称
    self.Text_name = self:GetWidget("Text_name")
    --购买
    self.Button_buy = Util:WidgetButton( 
                                            self:GetWidget("Button_buy") , 
                                            CPPGameLib.GetString("tower_shop") ,
                                            CPPGameLib.Handler(self,self.OnButtonGain) ,
                                            nil , Macros.Color.btn , Macros.Color.button_yellow 
                                            )
    --价钱
    self.Panel_2 = self:GetWidget("Panel_2")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    self.Panel_icon:SetPosition(self.Panel_icon:GetPositionX() ,self.Panel_icon:GetPositionY() )
end

function Modules.TemplHeroShopItem:__ShowUI( data )

    local cnf = nil
    if data.type == 1 then
        cnf = CPPGameLib.GetConfig("HeroShop", data.goods_id)       
    else
        cnf = CPPGameLib.GetConfig("AwakenShop", data.goods_id)      
    end

    --图标    
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = cnf.item_id , num = cnf.item_num})
    self.Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)

    local type_ , info = ObjIDTool.GetInfo(cnf.item_id)
    if type_ == Macros.Game.ObjType.HERO and info.type ~= Macros.Game.HeroType.EXP then 
        local effectId, effect = self:__CreateEffect({res_name = "UI_panjuntouxiang"}, self:GetWidget("Panel_icon"),1)
        PosTool.Center(effect)
    end

    --名字   
    local type_ , info = ObjIDTool.GetInfo(cnf.item_id)
    self.name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality or info.init_quality)
    self.name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    self.name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(self.name)

    local sp = nil
    --上阵
    local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "ToIdGetBattle" , cnf.item_id)
    if type_ then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "played" , true )
    end

    --宿命
    local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "ToIdGetFate" , cnf.item_id)
    if type_ then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "fate" , true )
    end

    --合击
    local type_ = CallModuleFunc(ModuleType.HERO_SHOP , "ToIdGetPunch" , cnf.item_id)
    if type_ then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "footing" , true )
    end

    if sp then
        self.tobattle = Util:Sprite( sp )
        self.tobattle:SetPosition( 23 , self.icon_layer:GetNode():GetContentHeight() - 5 )
        self.icon_layer:GetNode():AddChild(self.tobattle,999)
    end

    --消耗
    self.consumeType = cnf.cost_type_list[1]
    self.consumeNum = cnf.cost_value_list[1]

    self.consume = Util:MoneyControl(cnf.cost_type_list[1] , 0.8 )
    self.consume:SetPosition( 0 , self.Panel_2:GetContentHeight()/2)
    self.consume:SetAnchorPoint(0,0.5)
    self.Panel_2:AddChild(self.consume)

    self.consumeValue = Util:Label( cnf.cost_value_list[1] , Macros.TypefaceSize.slightly , Macros.Color.lilyWhite )
    self.consumeValue:SetAnchorPoint( 1 , 0.5 )
    self.consumeValue:SetPosition( self.Panel_2:GetContentWidth() - 5 , self.Panel_2:GetContentHeight()/2 )
    self.Panel_2:AddChild(self.consumeValue)

    --按钮
    if data.buy_num ~= 0 then
        -- self.Button_buy:SetEnabled(false)
        self.Button_buy:SetText(CPPGameLib.GetString("shop_albuy"))
        Util:GreyButton( self.Button_buy )
    else
        self.Button_buy:SetEnabled(true)
        self.Button_buy:SetText(CPPGameLib.GetString("shop_sure_Buy"))     
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        local stroke_width = 2
        self.Button_buy:SetOutline(r, g, b, a, stroke_width) 
    end
end

--使用
function Modules.TemplHeroShopItem:OnButtonGain()
    local haveConsumeNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.consumeType)
    self.data.consumeType = self.consumeType
    self.data.haveConsumeNum = haveConsumeNum
    self.data.consumeNum = self.consumeNum


    local cnf = nil
    if self.data.type == 1 then
        cnf = CPPGameLib.GetConfig("HeroShop", self.data.goods_id)       
    else
        cnf = CPPGameLib.GetConfig("AwakenShop", self.data.goods_id)      
    end

    -- 判断列表背包上限
    local t1 = { 
         [1] = { id = cnf.item_id , num = cnf.item_num },
    }
    CallModuleFunc(ModuleType.SUNDRY, "LstPackVacancy", t1 , true , function ( text )
        if #text == 0 then
            if self.consumeType == Macros.Game.RolePropType.DIAMOND then
                Modules.TemplHeroShopTips.New( self.data )
            else
                if haveConsumeNum < self.consumeNum then
                    -- local txt = "currency_insufficient_"..self.consumeType
                    -- GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
                    local cnf = CPPGameLib.GetConfig("HuoBi", self.consumeType)  
                    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, cnf.big_icon)
                    return
                end   

                local info = {}
                if self.data.type == 1 then
                    info.type = Macros.Game.SHOP.SOUL
                else
                    info.type = Macros.Game.SHOP.AWAKEN_SHOP
                end   
                info.param1 = self.data.goods_id
                info.param2 = 1
                CallModuleFunc(ModuleType.HERO_SHOP , "HeroShopBuyReq",info)
            end
        end
    end)
end


Modules = Modules or {}
Modules.TemplHeroShopTips = Modules.TemplHeroShopTips or BaseClass(GUI.Template)

function Modules.TemplHeroShopTips:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self.data = data
        self:__ShowUI(data)
    end
    self:InitTemplate(GUI.TemplateRegistry.TOWER_TREASURE) 
end

function Modules.TemplHeroShopTips:__delete()
end

function Modules.TemplHeroShopTips:__Getwidget()
    --背景
    self.img_selectBG = self:GetWidget("Image_bg")
    --标题
    self.img_headline = self:GetWidget("Image_light")
    --关闭按钮
    self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , "  " , CPPGameLib.Handler(self,self.OnBtnclose) )
    --列表容器
    self.img_dataBG = self:GetWidget("Image_content")
    --确定按钮
    self.btn_ok = Util:WidgetButton( 
                                        self:GetWidget("Button_confirm") , 
                                        CPPGameLib.GetString("common_confirm") , 
                                        CPPGameLib.Handler(self,self.OnBtn_ok) ,
                                        nil,nil, Macros.Color.button_red
                                     )
    --取消按钮
    self.btn_no = Util:WidgetButton( 
                                        self:GetWidget("Button_cancel") , 
                                        CPPGameLib.GetString("common_cancel") , 
                                        CPPGameLib.Handler(self,self.OnBtnclose) ,
                                        nil,nil, Macros.Color.button_yellow
                                     )
    --提示
    self.Text_tips = self:GetWidget("Text_tips")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --名称
    self.Text_name = self:GetWidget("Text_name") 
    --现价
    self.Text_now = self:GetWidget("Text_now") 
end

function Modules.TemplHeroShopTips:__ShowUI(data)
    --标题
    self.headline = Util:Label( CPPGameLib.GetString("heroShop_buyOk") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
    self.headline:SetPosition(self.img_headline:GetContentWidth()/2 , self.img_headline:GetContentHeight()/2 )
    self.img_headline:AddChild(self.headline)

    local cnf = nil
    if data.type == 1 then
        cnf = CPPGameLib.GetConfig("HeroShop", data.goods_id)       
    else
        cnf = CPPGameLib.GetConfig("AwakenShop", data.goods_id)      
    end
    --图标    
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = cnf.item_id , num = cnf.item_num})
    self.icon_layer:SetPosition( self.Panel_icon:GetContentWidth()/2 , self.Panel_icon:GetContentHeight()/2 )
    self.Panel_icon:AddChild(self.icon_layer:GetNode())

    --名字   
    local type_ , info = ObjIDTool.GetInfo(cnf.item_id)
    self.name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality or info.init_quality)
    self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
    self.name:SetAnchorPoint(0.5,0.5)
    self.Text_name:AddChild(self.name)

    --图标
    local sp = Util:GetMoneyIconImgData(data.consumeType)

    local tab = { 
                 {zi = CPPGameLib.GetString("plunderedtreasures_AvoidWarText_8") , ys = Macros.Color.keypoint },
                 {zi = data.consumeNum , ys = Macros.Color.content },
                 {tp = sp , dx = 0.85 },
                }
    self.AvoidWarText = Util:LabCenter( tab )
    self.AvoidWarText:SetPosition( self.Text_now:GetContentWidth()/2 , self.Text_now:GetContentHeight()/2 )
    self.AvoidWarText:SetAnchorPoint(0.5 , 0.5 )
    self.Text_now:AddChild(self.AvoidWarText)

end

function Modules.TemplHeroShopTips:OnBtn_ok(data)
    if self.data.haveConsumeNum < self.data.consumeNum then
        local txt = "currency_insufficient_"..self.data.consumeType
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
        return
    end   

    local info = {}
    if self.data.type == 1 then
        info.type = Macros.Game.SHOP.SOUL
    else
        info.type = Macros.Game.SHOP.AWAKEN_SHOP
    end   
    info.param1 = self.data.goods_id
    info.param2 = 1
    CallModuleFunc(ModuleType.HERO_SHOP , "HeroShopBuyReq",info)

    self:DeleteMe()
end

function Modules.TemplHeroShopTips:OnBtnclose(data)
    self:DeleteMe()
end
