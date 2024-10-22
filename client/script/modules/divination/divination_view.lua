--[[
%% @module: 占卜主界面
%% @author: yjg
%% @created: 2017年11月14日11:32:03
--]]

Modules = Modules or {} 
Modules.DivinationView = Modules.DivinationView or BaseClass(BaseView)

function Modules.DivinationView:__init()
    self.__layout_name = "divination"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_divination") , image_size = COCOSize(1136,640) }
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination"), 
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

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIVINATION)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIVINATION , true)
end

function Modules.DivinationView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.DivinationView:__Dispose()
end

function Modules.DivinationView:__Server( )
    CallModuleFunc(ModuleType.DIVINATION , "PlayerZhanbuDataReq" )

    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DEAL , function ( ... )
        self:__SetShowUI()
    end )


    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE , function ( _ , system_id )
        if system_id == Macros.Game.SystemID.DIVINATION then
            --设置红点
            self:SetRed()
        end
    end )
end

function Modules.DivinationView:__Getwidget( )
    
    self.Panel_24 = self:GetWidget("Panel_24")

    --收藏
    self.Panel_collect = self:GetWidget("Panel_collect")
    WidgetUnity.SetWidgetListener( self.Panel_collect , nil, nil, function ( ... )
      self:OnButtonCollect()
    end)
    -- 闪光特效
    self.Image_collect = self:GetWidget("Image_collect")
    local effectid,effect = self:__CreateEffect({res_name = "UI_zhanbu_men"} , self.Image_collect , -1 )
    PosTool.Center(effect , -8 , 210 )

    --命运商店
    self.Panel_shop1 = self:GetWidget("Panel_shop1")
    WidgetUnity.SetWidgetListener( self.Panel_shop1 , nil, nil, function ( ... )
      self:OnButtonShop1()
    end)

    -- 水晶骨骼
    local effectid,effect = self:__CreateEffect({res_name = "UI_zhanbu_xiaoqiu" ,  type = Effect.EffectType.SKELETON } , self.Panel_shop1 , 1 )
    effect:SetAnchorPoint( 0.5 , 0.5 )
    effect:SetPosition( self.Panel_shop1:GetContentWidth()/2 - 5  , 150 )

    --占卜
    self.Panel_divination = self:GetWidget("Panel_divination")
    WidgetUnity.SetWidgetListener( self.Panel_divination , nil, nil, function ( ... )
      self:OnButtonDivination()
    end)
    -- 闪光特效
    local effectid,effect = self:__CreateEffect({res_name = "UI_zhanbu_huo"} , self.Panel_divination , 99 )
    PosTool.Center(effect , -3 , 92)

    -- 水晶骨骼
    local effectid,effect = self:__CreateEffect({res_name = "UI_zhanbu_daqiu" ,  type = Effect.EffectType.SKELETON } , self.Panel_divination , 9 )
    effect:SetAnchorPoint( 0.5 , 0.5 )
    effect:SetPosition( self.Panel_divination:GetContentWidth()/2 , 120 )

    --星魂商店
    self.Panel_shop2 = self:GetWidget("Panel_shop2")
    WidgetUnity.SetWidgetListener( self.Panel_shop2 , nil, nil, function ( ... )
      self:OnButtonShop2()
    end)
    -- 闪光特效
    local effectid,effect = self:__CreateEffect({res_name = "UI_zhanbu_shuijing"} , self.Panel_shop2 , 99 )
    PosTool.Center(effect , 0 , -50 )

    -- 水晶骨骼
    local effectid,effect = self:__CreateEffect({res_name = "UI_zhanbu_shuijin" ,  type = Effect.EffectType.SKELETON } , self.Panel_shop2  )
    effect:SetAnchorPoint( 0.5 , 0 )
    effect:SetPosition( self.Panel_shop2:GetContentWidth()/2 , -20 )

    --背包
    self.Button_pack = Util:WidgetButton( 
                                              self:GetWidget("Button_pack") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonPack) ,
                                              nil , nil , Macros.Color.button_yellow 
                                          )
    self.Button_pack:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_bag"), TextureResType.LOCAL )

    self.divination_pack = Util:Name( CPPGameLib.GetString("divination_pack") )
    self.divination_pack:SetPosition( self.Button_pack:GetContentWidth()/2 , 0 )
    self.divination_pack:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_pack:AddChild( self.divination_pack )

    --预览

    self.Button_preview = Util:WidgetButton( 
                                              self:GetWidget("Button_preview") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonPreview) ,
                                              nil , nil , Macros.Color.button_yellow 
                                          )

    self.Button_preview:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_divine"), TextureResType.LOCAL )

    self.divination_preview = Util:Name( CPPGameLib.GetString("divination_preview") )
    self.divination_preview:SetPosition( self.Button_preview:GetContentWidth()/2 , 0 )
    self.divination_preview:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_preview:AddChild( self.divination_preview )

    --帮助
    self.Button_help = Util:WidgetButton( 
                                              self:GetWidget("Button_help") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonHelp) ,
                                              nil , nil , Macros.Color.button_yellow 
                                          )
    self.Button_help:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_daily"), TextureResType.LOCAL )

    self.divination_help = Util:Name( CPPGameLib.GetString("divination_help") )
    self.divination_help:SetPosition( self.Button_help:GetContentWidth()/2 , 0 )
    self.divination_help:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_help:AddChild( self.divination_help )

    --免费
    self.Image_divination_text = self:GetWidget("Image_divination_text")
end 

function Modules.DivinationView:__ShowUI( )
    self.divination_gratis = Util:Label( CPPGameLib.GetString("divination_gratis") .. "0/0" )
    self.divination_gratis:SetAnchorPoint( 0.5 , 0.5 )
    self.divination_gratis:SetPosition( self.Image_divination_text:GetContentWidth()/2 , self.Image_divination_text:GetContentHeight()/2 )
    self.Image_divination_text:AddChild( self.divination_gratis )

    --收藏值
    local divination_collectValue = Util:Name( CPPGameLib.GetString("divination_collectValue") ..  CPPGameLib.GetString("common_colon") )
    divination_collectValue:SetAnchorPoint( 0 , 0.5 )
    divination_collectValue:SetPosition( 10 , self.Panel_24:GetContentHeight()/2 + 240 )
    self.Panel_24:AddChild( divination_collectValue )

    --历史最高
    local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
    self.history = Util:Name( max .. "  (".. CPPGameLib.GetString("tower_history") ..")" )
    self.Panel_24:AddChild( self.history )
    PosTool.RightTo( divination_collectValue , self.history )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite( icon_red_point )
    self.icon_red_point:SetAnchorPoint( 1 , 1 )
    self.icon_red_point:SetPosition( self.Image_collect:GetContentWidth() , self.Image_collect:GetContentHeight() )
    self.Image_collect:AddChild( self.icon_red_point )
    self:SetRed()
end

function Modules.DivinationView:__SetShowUI( )
    local card = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
    local shengyu = ConfigAdapter.Common.GetLowBDivinationFree() - card.today_low_card_group_open_card_times
    if shengyu <= 0 then
        shengyu = 0
    end
    self.divination_gratis:SetString( CPPGameLib.GetString("divination_gratis") .. shengyu .."/" .. ConfigAdapter.Common.GetLowBDivinationFree() )
end

-- 收藏
function Modules.DivinationView:OnButtonCollect( )
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_COLLECT )
end

-- 命运商店
function Modules.DivinationView:OnButtonShop1( )
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_DESTINY_SHOP )
end

-- 占卜
function Modules.DivinationView:OnButtonDivination( )
    local layer = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    layer:SetAnchorPoint( 0.5 , 0.5 )
    self.__root_widget:AddChild( layer )
    PosTool.Center(layer)
    layer:SetVisible(false)

    GUI.MultiResolutionAdapter.AdaptForBG( layer , self.__root_widget , self.__is_force_stretch, self.__is_ignore_iphone_safe_area )

    layer:SetBackGroundColor(0, 0, 0)
    layer:SetBackGroundColorType(1)

    CPPActionManager.FadeOut(layer , 0 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
        layer:SetVisible(true)
        CPPActionManager.FadeIn( layer , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
            CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_SCRYER )
        end) )
    end) )
end

-- 星魂商店
function Modules.DivinationView:OnButtonShop2( )
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_SHOP )
end

--背包
function Modules.DivinationView:OnButtonPack()
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_PACK )
end

--预览
function Modules.DivinationView:OnButtonPreview()
    LogicGUI.TemplDivinationRewardPrelook.New( )
end

--帮助
function Modules.DivinationView:OnButtonHelp()
    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
    help:SetLoadedCallback( function ( ... )
       help:SetCell("Rule_Divination")
    end )
end

--红点
function Modules.DivinationView:SetRed()
    local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.DIVINATION, Macros.Game.SystemID.MAX)
    if red_num <= 0 then
        self.icon_red_point:SetVisible(false)
    else
        self.icon_red_point:SetVisible(true)
    end
end
