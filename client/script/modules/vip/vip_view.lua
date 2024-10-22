--[[
%% @module: vip
%% @author: yjg
%% @created: 2016年10月25日09:33:22
--]]

Modules = Modules or {} 
Modules.VIPView = Modules.VIPView or BaseClass(BaseView)

function Modules.VIPView:__init()
    self.__layout_name = "vip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("vip"), 
      show_diamond = true,
      show_gold = true,
      show_stamina = true,
      show_energy = true,
      close_func = function()
        self:CloseManual()
      end,
    }

    self.__open_callback = function()
      self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_VIP_ITEM)  

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.VIP)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.VIP,true)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
end

function Modules.VIPView:__OpenCallback( )
    -- self.lv = nil
    self.lst = {}
    self:__Getwidget()
    self:__ShowUI()
    CallModuleFunc(ModuleType.VIP, "ShopBuyRecordRequest")
end 

function Modules.VIPView:__Dispose()
    for i=1,#self.lst do
        self.lst[i]:DeleteMe()
        self.lst[i] = nil
    end

    if self.PageView_1 then 
        self.PageView_1:DeleteMe()
        self.PageView_1 = nil
    end
end

function Modules.VIPView:__Getwidget( )

    --居中层
    self.Panel_1 = self:GetWidget("Panel_1") 
    --三段式背景
    self.Image_content_bg = self:GetWidget("Image_content_bg") 
    self.Image_content_bg_1 = self:GetWidget("Image_content_bg_1") 
    -- self.Image_content = self:GetWidget("Image_content") 

    -- --彩带
    -- self.Image_ribbon = self:GetWidget("Image_ribbon") 
    --标题
    self.Image_title = self:GetWidget("Image_title") 
    --vip等级
    self.Text_viplv = self:GetWidget("Text_viplv") 
    --再充值
    self.Text_carryOn = self:GetWidget("Text_carryOn") 
    --即可
    self.Text_shbcco = self:GetWidget("Text_shbcco") 
    --进度条
    self.Panel_schedule = self:GetWidget("Panel_schedule") 
    --快速充值
    self.Button_8 = Util:WidgetButton( 
                                        self:GetWidget("Button_8") , 
                                        " " , 
                                        CPPGameLib.Handler(self,self.OnVipPay),
                                        nil,nil,Macros.Color.button_yellow
                                    )

    --翻页容器
    local panel_pageview = self:GetWidget("PageView_1") 
    panel_pageview:SetVisible(false)
    local panel_layout = Util:Layout(panel_pageview:GetContentWidth(), panel_pageview:GetContentHeight())
    panel_layout:SetAnchorPoint(0.5,1)   
    panel_layout:SetPosition(panel_pageview:GetPositionX(),panel_pageview:GetPositionY())
    self.Panel_1:AddChild(panel_layout)
    self.__panel_pageview = panel_layout

    --后退
    WidgetUnity.SetWidgetListener(self:GetWidget("Image_back"), nil, nil,function ()
        self:OnButtonLeft()
    end)

    local effect = nil
    self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self:GetWidget("Image_back") , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self:GetWidget("Image_back"):GetContentWidth()/2 , self:GetWidget("Image_back"):GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )

    --前进
    WidgetUnity.SetWidgetListener(self:GetWidget("Image_front"), nil, nil,function ()
        self:OnButtonRight()
    end)

    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self:GetWidget("Image_front") , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self:GetWidget("Image_front"):GetContentWidth()/2 , self:GetWidget("Image_front"):GetContentHeight()/2 )

    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(1)VIP 小图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_VIP")
        end )
    end )
    Button_rule:SetZOrder(100)
end 

function Modules.VIPView:__ShowUI( )

    -- --vip图样
    -- local vip_privilege_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_privilege" , true )
    -- local vip_privilege = Util:Sprite(vip_privilege_sp)
    -- self.Image_ribbon:AddChild( vip_privilege )
    -- PosTool.Center( vip_privilege , 0 , 38 )

    --当前等级是
    local vip_lvTips = Util:Label( "" , Macros.TypefaceSize.largish , Macros.Color.white )
    vip_lvTips:SetAnchorPoint( 0 , 0.5 )
    vip_lvTips:SetPosition( self.Text_viplv:GetContentWidth()/2 , self.Text_viplv:GetContentHeight()/2 )
    self.Text_viplv:AddChild( vip_lvTips )

    --vip图样
    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_big" , true )
    local vip_title_icon = Util:Sprite(vip_title_icon_sp)
    PosTool.RightTo( vip_lvTips , vip_title_icon )
    self.Text_viplv:AddChild( vip_title_icon )

    --VIP等级
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_num", true)
    self.viplv = GUI.ImgFont.New(vipnum)
    self.viplv:SetText(tostring(0))
    PosTool.RightTo( vip_title_icon , self.viplv , 5 )
    self.Text_viplv:AddChild(self.viplv:GetNode()) 
    
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "schedule_bar")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "schedule_bg")
    --进度条
    self.LoadingBar_loadingBar = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Panel_schedule:GetContentWidth() - 5 ,22, self.Panel_schedule:GetContentWidth() , 32, 100, 100)
    self.LoadingBar_loadingBar:SetAnchorPoint(0.5, 0.5)
    self.Panel_schedule:AddChild(self.LoadingBar_loadingBar:GetNode())
    PosTool.Center(self.LoadingBar_loadingBar)

    --VIP经验
    self.vipExp = Util:Label("00/00",Macros.TypefaceSize.normal)
    self.vipExp:SetPosition(self.LoadingBar_loadingBar:GetNode():GetPositionX() , self.LoadingBar_loadingBar:GetNode():GetPositionY() )
    self.vipExp:SetAnchorPoint(0.5,0.5)
    self.Panel_schedule:AddChild(self.vipExp)

    self:SetPageViewCell()
    self:SetHeadHint()
    self:SetShowUI()
end

function Modules.VIPView:SetPageViewCell( ... )
    local vipCnf =  CallModuleFunc(ModuleType.VIP, "GetVipTipsCnf")
    local vipClv =  CallModuleFunc(ModuleType.VIP, "GetVipLV")
    if not self.PageView_1 then
        self.PageView_1 = GUI.PageView.New({view_width = self.__panel_pageview:GetContentWidth(),
            view_height = self.__panel_pageview:GetContentHeight(),first_scroll_page = vipClv})
        self.__panel_pageview:AddChild(self.PageView_1:GetNode())
        PosTool.Center(self.PageView_1)
    end
    for i,v in ipairs(vipCnf) do
        v.page_class = LogicGUI.TemplVipItem       
    end
    self.PageView_1:SetDataList(vipCnf)
end

--左
function Modules.VIPView:OnButtonLeft()
    if self.PageView_1:GetCurPageIndex() ~= 0 then
        self.PageView_1:ScrollToPage(self.PageView_1:GetCurPageIndex() - 1 )
    end
end

--右
function Modules.VIPView:OnButtonRight()
    local vipCnf =  CallModuleFunc(ModuleType.VIP, "GetVipTipsCnf")
    if self.PageView_1:GetCurPageIndex() < #vipCnf - 1 then
        self.PageView_1:ScrollToPage(self.PageView_1:GetCurPageIndex() + 1 )
    end
end

function Modules.VIPView:SetShowUI( ... )
    --等级
    self.viplv:SetText(tostring( CallModuleFunc(ModuleType.VIP , "GetVipLV") ))
    --礼包提示
    local lv = CallModuleFunc(ModuleType.VIP , "GetVipLV")
    if CallModuleFunc(ModuleType.VIP , "GetVipLV") == 0 then
        lv = 1
    end
    --经验
    local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp")
    self.vipExp:SetString(nowExp.."/"..upExp)
    self.LoadingBar_loadingBar:SetProgress( nowExp/upExp * 100 )
end

--设置标题
function Modules.VIPView:SetHeadHint( ... )
    --再充值
    if self.headHint and self.headHint.recharge then
        self.headHint.recharge:RemoveSelf(true)
        self.headHint.recharge = nil
    end
    --其他
    for i=1,10 do
        if self.headHint and self.headHint["intensify_"..i] then
            self.headHint["intensify_"..i]:RemoveSelf(true)
            self.headHint["intensify_"..i] = nil
        end
    end

    self.headHint = {}

    local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp")
    local lv , maxLv = CallModuleFunc(ModuleType.VIP , "GetVipLV")
    
    if maxLv ~= lv then
        nextLv = lv + 1
        --再充值
        self.headHint.recharge = Util:LabCenter({
                                    { zi = CPPGameLib.GetString("vip_continue"),dx = Macros.TypefaceSize.normal},
                                    { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND), dx = 0.85 , jg = 5},
                                    { zi = upExp - nowExp ,ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal, jg = 5}
                                  })
        self.headHint.recharge:SetPosition(0 , self.Text_carryOn:GetContentHeight()/2)
        self.headHint.recharge:SetAnchorPoint(0,0.5)
        self.Text_carryOn:AddChild(self.headHint.recharge)

        --即可成为
        self.headHint.intensify_1 = Util:Label(  CPPGameLib.GetString("vip_can") , Macros.TypefaceSize.normal )
        self.headHint.intensify_1:SetPosition(0 , self.Text_shbcco:GetContentHeight()/2)
        self.headHint.intensify_1:SetAnchorPoint(0,0.5)
        self.Text_shbcco:AddChild(self.headHint.intensify_1) 

        --vip图样
        local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true )
        self.headHint.intensify_2 = Util:Sprite(vip_title_icon_sp)
        PosTool.RightTo( self.headHint.intensify_1 , self.headHint.intensify_2 , 5 )
        self.Text_shbcco:AddChild(self.headHint.intensify_2) 

        --VIP等级
        local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
        self.headHint.intensify_3 = GUI.ImgFont.New(vipnum)
        self.headHint.intensify_3:SetText(tostring(nextLv))
        PosTool.RightTo( self.headHint.intensify_2 , self.headHint.intensify_3 , 5 )
        self.Text_shbcco:AddChild(self.headHint.intensify_3:GetNode()) 

    end
end

--跳到指定界面
function Modules.VIPView:SetSkip()
    
end

--快速充值
function Modules.VIPView:OnVipPay()
    CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
end