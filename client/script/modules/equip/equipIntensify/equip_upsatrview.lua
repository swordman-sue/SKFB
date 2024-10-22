--[[
%% @module: 装备升星
%% @author: yjg
%% @created: 2018年7月30日09:44:12
--]]

Modules = Modules or {} 
Modules.EquipUpSatrView = Modules.EquipUpSatrView or BaseClass(BaseView)

function Modules.EquipUpSatrView:__init()
    self.__layout_name = ""  
    self.__layout_mode = BaseView.LayoutMode.CODE 
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_upstar"), image_size = COCOSize(1136,640) }
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("equip_upstar"),
        show_power = true,
        show_diamond = true,
        show_gold = true,
        show_level = true,
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EQUIPPACK,true)
end

function Modules.EquipUpSatrView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.EquipUpSatrView:__Dispose()
end

function Modules.EquipUpSatrView:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.widget
end

function Modules.EquipUpSatrView:__Server()
    CallModuleFunc(ModuleType.EQUIP, "SettleEquipmentStarUpCnf")

    self.adornOrderLst = CallModuleFunc( ModuleType.EQUIP , "GetEquipAdornOrderLst" )
    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

    self.heroIndex = 0
    self.equipIndex = 0
    --寻找当前的位置
    for i,v in ipairs( self.adornOrderLst ) do
        for j , vv in ipairs( v ) do
            if vv.uid == equipInfo.uid then
                self.heroIndex = i
                self.equipIndex = j
                break
            end
        end
    end

    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_UPSTAR,function ( ... )

        local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
        if equipInfo.star_level ~= 0 then
            Modules.TemplEquipUpSatrLevel.New( function ( ... )
                self:__SetShowUI( true )
            end )
        else
            LogicGUI.TemplEquipUpSatr.New()
            self:__SetShowUI()
        end
    end )

    self:__SetShowUI()
end

function Modules.EquipUpSatrView:__Getwidget()
    self.layer = Util:Layout( self.widget:GetContentWidth() , self.widget:GetContentHeight() )
    self.widget:AddChild( self.layer )
    self.layer:SetAnchorPoint( 0.5 , 0.5 )
    PosTool.Center( self.layer )

    --名字
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_name")
    self.bg_hero_name = Util:Sprite9( sp )
    self.bg_hero_name:SetContentSize( 285 , 40 )
    self.bg_hero_name:SetAnchorPoint( 0.5 , 0.5 )
    self.layer:AddChild( self.bg_hero_name )
    PosTool.Center( self.bg_hero_name , 0 , 235 )

    --星星
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_star_bg")
    self.sp_star_bg = Util:Sprite( sp )
    self.sp_star_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.layer:AddChild( self.sp_star_bg )
    PosTool.Center( self.sp_star_bg , 0 , 170 )

    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_now_upstar")
    local btn_select_tab = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_select_tab")
    local t1 = {}
    local x = { 
                [1] = { x = self.layer:GetContentWidth()/2 - 300 , btn_x = -200 },
                [2] = { x = self.layer:GetContentWidth()/2 + 300 , btn_x = 200 }
             }
    for i = 1 , 2 do
        t1[i] = Util:Sprite( sp )
        t1[i]:SetAnchorPoint( 0.5 , 0.5 )
        t1[i]:SetPosition( x[i].x , self.layer:GetContentHeight()/2 + 30 )
        self.layer:AddChild( t1[i] )

        local select_tab = Util:Button( btn_select_tab , function( ... )
            self:SelectTab(i)
        end )
        select_tab:SetAnchorPoint( 0.5 , 0.5 )
        t1[i]:AddChild(select_tab) 
        PosTool.Center( select_tab , x[i].btn_x )
        if i == 1 then
            select_tab:SetFlipX( true )
        end

    end
    self.nowbg = t1[1]
    self.nexbg = t1[2]

    --消耗
    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
    self.consume = Util:Sprite9(black1_bg_sp)
    self.consume:SetContentSize( 270 , 130 )
    self.consume:SetAnchorPoint( 0.5 , 0.5 )
    self.layer:AddChild( self.consume )
    PosTool.Center( self.consume , 0 , -145 )

    --金钱消耗
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_consume")
    self.sp_consume = Util:Sprite(sp)
    self.sp_consume:SetAnchorPoint( 0.5 , 0.5 )
    self.consume:AddChild( self.sp_consume )
    PosTool.Center( self.sp_consume , 0 , -40 )

    --升星
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred" )
    self.btn_up_star = Util:Button( sp , function( ... )
        self:OnBtnUpStar()
    end ,CPPGameLib.GetString("equip_upstar") , Macros.TypefaceSize.headline , Macros.Color.btn , Macros.Color.button_red )
    self.btn_up_star:SetAnchorPoint( 0.5 , 0.5 )
    self.layer:AddChild(self.btn_up_star) 
    PosTool.Center( self.btn_up_star , 0 , -260 )

end

function Modules.EquipUpSatrView:__ShowUI()
    --名字
    self.name = Util:Name( " " )
    self.bg_hero_name:AddChild( self.name )
    PosTool.Center( self.name )

    -- 图标
    local path = Resource.PathTool.GetBigIconPath(20101)
    self.icon = Util:Sprite(path)
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.icon:SetVisible(false)
    self.layer:AddChild(self.icon)
    PosTool.Center( self.icon , 0 , 20 )
    self.icon:SetScaleFactor( 0.9 )
    ActionManager.GetInstance():UpDownMoveAnimation(self.icon , 1 , 15)

    --消耗背景
    self.prop_bg = Util:Layout( 0 , 0 )
    self.prop_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.consume:AddChild( self.prop_bg )
    PosTool.Center( self.prop_bg , 0 , 25 )

    --装备英雄
    self.equip_upstartabhero_1 = Util:RichText( CPPGameLib.GetString("equip_upstartabhero_1" , " ") )
    self.equip_upstartabhero_1:SetAnchorPoint( 0 , 0.5 )
    self.equip_upstartabhero_1:SetPosition( 80 , self.layer:GetContentHeight() - 80 )
    self.equip_upstartabhero_1:SetVisible( false )
    self.layer:AddChild(self.equip_upstartabhero_1)

    --星
    local layer = Util:Layout( 100 , 100 )
    layer:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_star_bg:AddChild( layer )
    PosTool.Center( layer , 0 , 0 )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star2")
    local size = Util:Sprite( sp )

    layer:SetContentSize( 5 * ( size:GetContentWidth() + 10 ) , size:GetContentHeight() )

    self.star = {}
    for i = 1 , 5 do
        self.star[i] = Util:Sprite( sp )
        self.star[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.star[i]:SetPosition( (size:GetContentWidth()/2+2.5) + ( (i-1) * (size:GetContentWidth()+10) ) , layer:GetContentHeight()/2 )
        self.star[i]:SetGray( true )
        layer:AddChild( self.star[i] )
    end

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "level_max",true)
    self.level_max = Util:Sprite( sp )
    self.level_max:SetAnchorPoint( 0.5 , 0.5 )
    self.level_max:SetVisible( false )
    self.nexbg:AddChild( self.level_max )
    PosTool.Center( self.level_max )
end

function Modules.EquipUpSatrView:SelectTab( index )
    local sIndex = 0
    if index == 1 then
        sIndex = -1
    elseif index == 2 then
        sIndex = 1
    end

    --切换英雄提示
    local function ShowTabHeroTips( ... )
        local data = self.adornOrderLst[ self.heroIndex ][ self.equipIndex ]
        if not data then
            return 
        end
        local cnf = CPPGameLib.GetConfig("Hero", data.equip_heroid )
        if self.nowHero ~= cnf.name then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("equip_upstartabhero",cnf.name) )
        end
    end

    if self.adornOrderLst[ self.heroIndex ][ self.equipIndex + sIndex ] then
        self.equipIndex = self.equipIndex + sIndex
    else
        if self.adornOrderLst[ self.heroIndex + sIndex ] and #self.adornOrderLst[ self.heroIndex + sIndex ] ~= 0 then
            self.heroIndex = self.heroIndex + sIndex
            if sIndex == -1 then
                self.equipIndex = #self.adornOrderLst[ self.heroIndex ]
            else
                self.equipIndex = 1
            end
            ShowTabHeroTips()
        else
            if sIndex == -1 then
                local max = 0
                for i , v in ipairs( self.adornOrderLst ) do
                    if #v ~= 0 then
                        max = max + 1
                    end
                end
                self.heroIndex = max
                self.equipIndex = #self.adornOrderLst[ self.heroIndex ]
                ShowTabHeroTips()
            else
                self.heroIndex = 1
                self.equipIndex = 1
                ShowTabHeroTips()
            end
        end
    end

    local data = self.adornOrderLst[ self.heroIndex ][ self.equipIndex ]
    if not data then
        return 
    end
    CallModuleFunc(ModuleType.EQUIP, "SetOnEquipUid" , data.uid )

    self:__SetShowUI()
end  

function Modules.EquipUpSatrView:__SetShowUI( roll )
    local data = self.adornOrderLst[ self.heroIndex ][ self.equipIndex ]

    --佩戴于
    local cnf = CPPGameLib.GetConfig("Hero", data.equip_heroid )
    self.nowHero = cnf.name
    self.equip_upstartabhero_1:SetText( CPPGameLib.GetString("equip_upstartabhero_1" , string.format(Macros.Quality_Stroke_RT[cnf.init_quality],cnf.name) ) )
    self.equip_upstartabhero_1:SetVisible( true )

    --名字
    local text = ""
    if data.refine_level ~= 0 then
        text = " +" .. data.refine_level
    end

    self.name:SetString( data.name .. text )    
    self.name:SetColor(unpack(Macros.Quality[data.quality]))
    self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[data.quality]))

    --图标
    local equipinfo = CPPGameLib.GetConfig("Equipment", data.info_id )
    self.icon:SetImage(Resource.PathTool.GetBigIconPath( equipinfo.res_id ))
    self.icon:SetVisible( true )

    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )
    --星级
    for i , v in ipairs( self.star ) do
        if i > equipInfo.star then
            v:SetGray( true )
        else
            v:SetGray( false )
        end
    end

    self:SetNowEquip( info.nowInfo , roll )
    self:SetNexEquip( info.nexInfo , roll )
    self:SetExpend( info.expend )
end

--当前装备信息
function Modules.EquipUpSatrView:SetNowEquip( info , roll )
    if not self.nowEquip then
        self.nowEquip = {}
    end

    for i,v in ipairs( self.nowEquip ) do
        if v.type then
            v.type:RemoveSelf(true)
            v.type = nil
        end
        if v.value then
            v.value:RemoveSelf(true)
            v.value = nil
        end
    end

    if not info.name then
        return
    end

    local t1 = { 
                [1] = { type = CPPGameLib.GetString("equip_nowStartName") , value = info.name }
             }
    for i , v in ipairs( info.info ) do
        local type_ , value_ = Util:Nature( v.type , v.value , true  , true )
        table.insert( t1 , { type = type_ , value = value_ } )
    end

    if not roll then
        self.nowfirst = t1
        self.nowEquip = self:ShowEquipInfo( t1 , self.nowbg )
    else
        self.nowEquip = self:ShowEquipInfo( t1 , self.nowbg , nil , self.nowfirst )
        self.nowfirst = t1
    end

end

--升星装备信息
function Modules.EquipUpSatrView:SetNexEquip( info , roll )
    if not self.nexEquip then
        self.nexEquip = {}
    end

    for i,v in ipairs( self.nexEquip ) do
        if v.type then
            v.type:RemoveSelf(true)
            v.type = nil
        end
        if v.value then
            v.value:RemoveSelf(true)
            v.value = nil
        end
    end

    if not info.name then
        self.level_max:SetVisible( true )
        return
    else
        self.level_max:SetVisible( false )
    end

    local t1 = { 
                [1] = { type = CPPGameLib.GetString("equip_nowStartName") , value = info.name }
             }
    for i , v in ipairs( info.info ) do
        local type_ , value_ = Util:Nature( v.type , v.value , true )
        table.insert( t1 , { type = type_ , value = value_ } )
    end

    if not roll then
        self.nexfirst = t1
        self.nexEquip = self:ShowEquipInfo( t1 , self.nexbg )
    else
        self.nexEquip = self:ShowEquipInfo( t1 , self.nexbg )
        self.nexfirst = t1
    end
end

--升星消耗信息
function Modules.EquipUpSatrView:SetExpend( info )
    CPPGameLib.PrintTable("xy" , info , "升星消耗信息") 
    self.prop_bg:SetContentSize( #info.prop * 100 , self.consume:GetContentHeight()/2 )

    if not self.prop then
        self.prop = {}
    end

    for i , v in ipairs( self.prop ) do
        v:RemoveSelf(true)
        v = nil
    end

    if self.money then
        self.money:RemoveSelf(true)
        self.money = nil
    end

    if not info then
        return
    end

    for i , v in ipairs( info.prop ) do
        self.prop[i] = LogicGUI.IconLayer.New()
        local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", v.item_id )
        self.prop[i]:SetData( {id = v.item_id , num = item_num .. "/".. v.item_num } )
        self.prop[i]:SetAnchorPoint( 0.5,0.5 )
        self.prop[i]:SetPosition( 50 + ((i-1)*100) , self.prop_bg:GetContentHeight()/2 )
        self.prop_bg:AddChild( self.prop[i]:GetNode() )
        self.prop[i]:SetScaleFactor( 0.8 )
    end


    local tab = {
                    { tp = Util:GetMoneyIconImgData( info.money.type ) , jg = 5 ,dx = 0.8 },
                    { zi = info.money.num , jg = 5 },
                }
    self.money = Util:LabCenter( tab )
    self.money:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_consume:AddChild( self.money )
    PosTool.Center( self.money )
end

--装备信息
function Modules.EquipUpSatrView:ShowEquipInfo( lst , bg , color , roll )
    local layer = Util:Layout( 0 , 0 )
    layer:SetAnchorPoint( 0.5 , 0.5 )
    bg:AddChild( layer )
    PosTool.Center( layer )

    layer:SetContentSize( bg:GetContentWidth() , #lst * 45 )

    local t1 = {}
    local func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)

    for i , v in ipairs( lst ) do
        t1[i] = {}
        t1[i].type = Util:Label( v.type .. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.largish )
        t1[i].type:SetAnchorPoint( 1 , 0.5 )
        t1[i].type:SetPosition( layer:GetContentWidth()/2 - 2 , layer:GetContentHeight() - ( 22.5 + ( (i-1)*45) ) )
        layer:AddChild( t1[i].type )

        if not roll then
            t1[i].value = Util:Label( v.value , Macros.TypefaceSize.largish , color )
            t1[i].value:SetAnchorPoint( 0 , 0.5 )
            t1[i].value:SetPosition( layer:GetContentWidth()/2 + 2 , t1[i].type:GetPositionY() )
            layer:AddChild( t1[i].value )
        else

            t1[i].value = Util:Label( roll[i].value , Macros.TypefaceSize.largish , color )
            t1[i].value:SetAnchorPoint( 0 , 0.5 )
            t1[i].value:SetPosition( layer:GetContentWidth()/2 + 2 , t1[i].type:GetPositionY() )
            layer:AddChild( t1[i].value )

            local func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
            CPPActionManager.ScaleTo( t1[i].value , 0.2 , 1.6 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                t1[i].value:SetString( v.value )
                CPPActionManager.ScaleTo( t1[i].value , 0.2 , 1 )
            end ) )
        end
    end

    return t1
end

--确定
function Modules.EquipUpSatrView:OnBtnUpStar()

    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )

    CPPGameLib.PrintTable("xy" , info , "请求物品列表----------")  


    if info.expend then
        for i , v in ipairs( info.expend.prop ) do
            local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", v.item_id )
            if item_num < v.item_num then
                local _ , info_ = ObjIDTool.GetInfo( v.item_id )
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2",info_.name) )
                GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, v.item_id)
                return
            end
        end

        local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , info.expend.money.type  )
        if num < info.expend.money.num then
            local text = "currency_insufficient_"..info.expend.money.type 
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( text ) )

            local config = CPPGameLib.GetConfig("HuoBi", info.expend.money.type )
            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config.big_icon)
            return
        end

        if not info.nexInfo.name then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("spirit_intensify_fullstar") )
            return 
        end
    end

    CallModuleFunc(ModuleType.EQUIP, "EquipmentStarUpReq", equipInfo)
end



------------------------------------------------------------------------------------------------------------------------------------------------------------------
--星级
------------------------------------------------------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.TemplEquipUpSatrLevel = Modules.TemplEquipUpSatrLevel or BaseClass(GUI.Template)

function Modules.TemplEquipUpSatrLevel:__init(fuc)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 
    self.fuc = fuc
    self:__ShowUI(data)
end

function Modules.TemplEquipUpSatrLevel:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height ,function ( ... )
        -- self:DeleteMe()
    end )
    return self.layout
end

-- function Modules.TemplEquipUpSatrLevel:__AddToParent()
-- end

function Modules.TemplEquipUpSatrLevel:__delete()
    -- upstar
end

function Modules.TemplEquipUpSatrLevel:__ShowUI()

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_upstar_win")
    local sp_upstar_win = Util:Sprite( sp )

    local layer = Util:Layout( sp_upstar_win:GetContentWidth() , sp_upstar_win:GetContentHeight() ,function ( ... )
    end )
    layer:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( layer )
    PosTool.Center( layer )  


    local sp_upstar_win = Util:Layout( sp_upstar_win:GetContentWidth() , sp_upstar_win:GetContentHeight() ,function ( ... )
    end )
    sp_upstar_win:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( sp_upstar_win )
    PosTool.Center( sp_upstar_win )

    --升星成功
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.EQUIPPACK, "upstar",true)
    local upstar = Util:Sprite( sp )
    upstar:SetAnchorPoint( 0.5 , 0.5 )
    upstar:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight() - upstar:GetContentHeight() )
    layer:AddChild( upstar )
    
    CPPActionManager.MoveBy(upstar, 3 , 0 , 30 )
    CPPActionManager.FadeOut(upstar,3)


    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

    local equip_upstarTo = Util:Name( CPPGameLib.GetString("equip_upstarTo" , CPPGameLib.GetString("equip_StartLevel" , equipInfo.star , equipInfo.star_level) ) , Macros.TypefaceSize.popup  )
    equip_upstarTo:SetAnchorPoint( 0.5 , 0.5 )
    equip_upstarTo:SetPosition( sp_upstar_win:GetContentWidth()/2 , upstar:GetPositionY() - 50 )
    sp_upstar_win:AddChild( equip_upstarTo )

    local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )
    local lst = info.nowInfo.info

    local layer = Util:Layout( 0 , 0 )
    layer:SetAnchorPoint( 0.5 , 1 )
    sp_upstar_win:AddChild( layer )
    PosTool.Center( layer , 0 , -10 )

    layer:SetContentSize( sp_upstar_win:GetContentWidth() , #lst * 45 )

    local t1 = {}
    for i , v in ipairs( lst ) do
        local type_ , value_ = Util:Nature( v.type , v.value , true  , true )
        t1[i] = {}
        t1[i].type = Util:Label( type_ .. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.largish )
        t1[i].type:SetAnchorPoint( 1 , 0.5 )
        t1[i].type:SetPosition( layer:GetContentWidth()/2 - 2 , layer:GetContentHeight() - ( 22.5 + ( (i-1)*45) ) )
        layer:AddChild( t1[i].type )

        t1[i].value = Util:Label( value_ , Macros.TypefaceSize.largish , color )
        t1[i].value:SetAnchorPoint( 0 , 0.5 )
        t1[i].value:SetPosition( layer:GetContentWidth()/2 + 2 , t1[i].type:GetPositionY() )
        layer:AddChild( t1[i].value )
    end

    local function moveAnimate( ... )
        self:DeleteMe()
        if self.fuc then
            self.fuc()
        end
    end     

    local func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)

    CPPActionManager.ScaleTo( sp_upstar_win ,0.3,1.1,GlobalCallbackMgr:AddCallBackFunc(function ()
        CPPActionManager.ScaleTo( sp_upstar_win ,0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
            CPPActionManager.DelayTo( sp_upstar_win ,1,GlobalCallbackMgr:AddCallBackFunc(function ()
                CPPActionManager.MoveBy(sp_upstar_win , 0.2 , -300 , 20 ,func)
                CPPActionManager.ScaleTo(sp_upstar_win , 0.2 , 0)
                CPPActionManager.FadeOut(sp_upstar_win , 0.2 )
            end))
        end))
    end))
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------
--星数
------------------------------------------------------------------------------------------------------------------------------------------------------------------

LogicGUI = LogicGUI or {}

LogicGUI.TemplEquipUpSatr = LogicGUI.TemplEquipUpSatr or BaseClass(GUI.Template)

function LogicGUI.TemplEquipUpSatr:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID , color = {0,0,0,125}} 
    self.__open_callback = function ()
        self:__Getwidget()
        self:__ShowUI()
    end 
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_UP_STAR)
end

function LogicGUI.TemplEquipUpSatr:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg")

    --上级
    self.before_type1 = self:GetWidget("Text_CurPropTitle22")
    self.before_type1:SetText("")
    self.before_value1 = self:GetWidget("Text_CurProp22")
    self.before_value1:SetText("")

    self.before_type2 = self:GetWidget("Text_CurPropTitle24")
    self.before_type2:SetText("")
    self.before_value2 = self:GetWidget("Text_CurProp24")
    self.before_value2:SetText("")

    self.before_type3 = self:GetWidget("Text_CurPropTitle25")
    self.before_type3:SetText("")
    self.before_value3 = self:GetWidget("Text_CurProp25")
    self.before_value3:SetText("")

    self.before_type4 = self:GetWidget("Text_CurPropTitle23")
    self.before_type4:SetText("")
    self.before_value4 = self:GetWidget("Text_CurProp23")
    self.before_value4:SetText("")

    --当前
    self.now_type1 = self:GetWidget("Text_NextPropTitle22")
    self.now_type1:SetText("")
    self.now_value1 = self:GetWidget("Text_NextProp22")
    self.now_value1:SetText("")

    self.now_type2 = self:GetWidget("Text_NextPropTitle24")
    self.now_type2:SetText("")
    self.now_value2 = self:GetWidget("Text_NextProp24")
    self.now_value2:SetText("")

    self.now_type3 = self:GetWidget("Text_NextPropTitle25")
    self.now_type3:SetText("")
    self.now_value3 = self:GetWidget("Text_NextProp25")
    self.now_value3:SetText("")

    self.now_type4 = self:GetWidget("Text_NextPropTitle23")
    self.now_type4:SetText("")
    self.now_value4 = self:GetWidget("Text_NextProp23")
    self.now_value4:SetText("")

    --箭头
    self.Image_arrows1 = self:GetWidget("Image_27")
    self.Image_arrows1:SetVisible(false)
    self.Image_arrows2 = self:GetWidget("Image_1")
    self.Image_arrows2:SetVisible(false)
    self.Image_arrows3 = self:GetWidget("Image_1_0")
    self.Image_arrows3:SetVisible(false)
    self.Image_arrows4 = self:GetWidget("Image_27_0")
    self.Image_arrows4:SetVisible(false)

    -- 点击继续
    self.Text_Continue = self:GetWidget("Text_Continue")
    self.Text_Continue:SetText("")
end

function LogicGUI.TemplEquipUpSatr:__ShowUI()

    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    --上次属性
    local beforeInfo = CallModuleFunc(ModuleType.EQUIP, "GetStarUpCnf" ,  equipInfo.type , equipInfo.quality , equipInfo.star-1 , 10  )
    local beforeLst = { 
                [1] = { type = CPPGameLib.GetString("equip_star") , value = CPPGameLib.GetString("equip_star_",equipInfo.star - 1 ) }
             }
    for i , v in ipairs( beforeInfo.pro_type_list ) do
        local type_ , value_ = Util:Nature( v , beforeInfo.pro_value_list[i] , true  , true )
        table.insert( beforeLst , { type = type_ , value = value_ } )
    end

    for i , v in ipairs( beforeLst ) do
        self["before_type"..i]:SetText( v.type .. CPPGameLib.GetString("common_colon") )
        self["before_value"..i]:SetText( v.value )
        self["Image_arrows"..i]:SetVisible( true )
    end

    --上次图标
    local beforeIcon = LogicGUI.IconLayer.New()
    beforeIcon:SetData( {id = equipInfo.info_id } )
    beforeIcon:SetAnchorPoint( 0.5,0.5 )
    beforeIcon:SetPosition( self.Image_bg:GetContentWidth()/2 - 125 , self.Image_bg:GetContentHeight()/2 + 70 )
    self.Image_bg:AddChild( beforeIcon:GetNode() )
    beforeIcon:SetEquipStar( equipInfo.star - 1  )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "team_updatearrow")
    local team_updatearrow = Util:Sprite( sp )
    self.Image_bg:AddChild( team_updatearrow )  
    team_updatearrow:SetPosition( self.Image_bg:GetContentWidth()/2 , self.Image_bg:GetContentHeight()/2 + 70 )


    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    --当前属性
    local nowInfo = CallModuleFunc(ModuleType.EQUIP, "GetStarUpCnf" ,  equipInfo.type , equipInfo.quality , equipInfo.star , equipInfo.star_level  )
   CPPGameLib.PrintTable("xy" , nowInfo , "nowInfo")  
    local nowLst = { 
                [1] = { type = CPPGameLib.GetString("equip_star") , value = CPPGameLib.GetString("equip_star_",equipInfo.star) }
             }
    for i , v in ipairs( nowInfo.pro_type_list ) do
        local type_ , value_ = Util:Nature( v , nowInfo.pro_value_list[i] , true  , true )
        table.insert( nowLst , { type = type_ , value = value_ } )
    end
    for i , v in ipairs( nowLst ) do
        self["now_type"..i]:SetText( v.type .. CPPGameLib.GetString("common_colon") )
        self["now_value"..i]:SetText( v.value )
    end

    --当前图标
    local nowIcon = LogicGUI.IconLayer.New()
    nowIcon:SetData( {id = equipInfo.info_id } )
    nowIcon:SetAnchorPoint( 0.5,0.5 )
    nowIcon:SetPosition( self.Image_bg:GetContentWidth()/2 + 125 , self.Image_bg:GetContentHeight()/2 + 70 )
    self.Image_bg:AddChild( nowIcon:GetNode() )
    nowIcon:SetEquipStar( equipInfo.star )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.EQUIPPACK, "upstar_activate",true)
    if nowInfo.desc then
        local tab = {
                        { tp = sp , jg = 5 ,dx = 0.8 },
                        { zi = nowInfo.desc or " " , jg = 5 , ys = Macros.Color.green },
                    }
        self.money = Util:LabCenter( tab )
        self.money:SetAnchorPoint( 0.5 , 0.5 )
        self.Image_bg:AddChild( self.money )
        self.money:SetPosition( self.Image_bg:GetContentWidth()/2 , 20 )
    end

    self:PlayActionTimeline("animation", false,function ( ... )
        local layout = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height ,function ( ... )
            self:DeleteMe()
        end )
        layout:SetAnchorPoint( 0.5 , 0.5 )
        self.__root_widget:AddChild( layout )
        PosTool.Center( layout )

        self.Text_Continue:SetText( CPPGameLib.GetString("hero_recruit_touchscreen_game") )
    end)

end