--[[
%% @module: 商店整合
%% @author: yjg
%% @created: 2017年6月13日10:52:20
--]]

Modules = Modules or {} 
Modules.ShopFailarmyView = Modules.ShopFailarmyView or BaseClass(BaseView)

function Modules.ShopFailarmyView:__init()
    self.__layout_name = "shop_failarmy"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_challenge")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("shop_shop"), 
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

    --公会信息
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_DATE , function (_, data)
        self:__ShowUI()
    end)

    --英雄商店信息
    self:BindGlobalEvent(Macros.Event.HeroShopModule.HERO_SHOP , function (_, data)
        self:__ShowUI()
    end)  

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SHOP_FAILARMY)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SHOP_FAILARMY,true) 
end

function Modules.ShopFailarmyView:__OpenCallback( )


    self.Key = true

    self:__Getwidget()

    local unionData = CallModuleFunc(ModuleType.UNION , "UnionData")
    if not unionData then
        CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "shop" )
    end

    local hero_shopData = CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopList")
    if not hero_shopData then
         CallModuleFunc(ModuleType.HERO_SHOP,"HeroShopDataReq" )
    end

    if unionData and hero_shopData then
        self:__ShowUI()
    end
end	

function Modules.ShopFailarmyView:__Dispose()   
end

function Modules.ShopFailarmyView:__Getwidget( )
    --大背景
    self.Image_bg = self:GetWidget("Image_bg")
    --二背景
    self.Image_bg2 = self:GetWidget("Image_bg2")
    --内容
    self.Image_content = self:GetWidget("Image_content")
    self.Image_2 = self:GetWidget("Image_2")

    --横幅
    self.Image_streamer = self:GetWidget("Image_streamer")
    self.Image_3 = self:GetWidget("Image_3")

end 

function Modules.ShopFailarmyView:__ShowUI( )
    local tab = {
            [1] = { effect = "UI_shangdian1" , x = 124 , y = 102 , id = "商城" , name = "store_shop" , system = Macros.Game.SystemID.SHOP },
            [2] = { effect = "UI_shangdian2" , x = 138 , y = 110 , id = "声望商店" , name = "arena_shop" , system = Macros.Game.SystemID.ARENA_REPUTATIONSHOP },
            [3] = { effect = "UI_shangdian3" , x = 128 , y = 100 , id = "英雄商店" , name = "hero_shop" , system = Macros.Game.SystemID.HERO_SHOP },
            [4] = { effect = "UI_shangdian4" , x = 121 , y = 105 , id = "神装商店" , name = "tower_shop" , system = Macros.Game.SystemID.TOWER_SHOP },
            [5] = { effect = "UI_shangdian5" , x = 182 , y = 60 , id = "公会商店" , name = "union_shop" , system = Macros.Game.SystemID.UNION_SHOP },
            [6] = { id = "战功商店" , name = "rebel_shop" , system = Macros.Game.SystemID.REBEL_SHOP },
            [7] = { id = "深渊商店" , name = "abyss_shop" , system = Macros.Game.SystemID.CHASM },
            [8] = { id = "星魂商店" , name = "divination_shop" , system = Macros.Game.SystemID.DIVINATION_SHOP },
            [9] = { id = "原石商店" , name = "raw_gemstone_shop" , system = Macros.Game.SystemID.RAW_GEMSTONE_SHOP },
            [10] = { id = "积分赛商店" , name = "integral_glory_shop" , system = Macros.Game.SystemID.INTEGRAL_GLORY_SHOP },
    }

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local t1 = {}
    for i , v in ipairs( tab ) do
        --开启等级
        local system = v.system
        if v.system == Macros.Game.SystemID.UNION_SHOP then
            system = Macros.Game.SystemID.UNION
        end

        local module_info = CPPGameLib.GetConfig("SystemEntry" , system )
        if module_info.prepare_level <= role_level then
            v.level = module_info.level
            table.insert( t1 , v )
        end
    end
    tab = t1

    table.sort( tab, function ( a , b )
        if a.level < b.level then
            return a.level < b.level
        elseif a.level == b.level then
            return a.system < b.system
        end
    end )
    
    local params =  
    {
        item_class = LogicGUI.TemplShopFailarmyItem ,
        item_width = self.Image_content:GetContentWidth() / 3,
        item_height = self.Image_content:GetContentHeight() / 2 - 10,
        row = 2,
        col = 3,
        item_space_r = 2,
        item_space_c = 2,
        selectable = true,
        select_callback = function (item_index,item_data)
            if self.Key == true then
                self.Key = false
            else
                return
            end

            --开启等级
            local system = item_data.system
            if item_data.system == Macros.Game.SystemID.UNION_SHOP then
                system = Macros.Game.SystemID.UNION
            end

            --判断
            local is_open = GlobalModulesMgr:IsOpen( system , true )
            if is_open ~= true then
                self.Key = true
                return
            end

            --声望
            if item_data.system == Macros.Game.SystemID.ARENA_REPUTATIONSHOP then
                GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.ARENA_REPUTATIONSHOP)
            
            -- 公会
            elseif item_data.system == Macros.Game.SystemID.UNION_SHOP then
                local unionData = CallModuleFunc(ModuleType.UNION , "UnionData")
                if not unionData then
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_NoJoin" ))
                    self.Key = true
                else
                    CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_SHOP)
                end

            -- 无尽
            elseif item_data.system == Macros.Game.SystemID.CHASM then
                CallModuleFunc(ModuleType.CHASM , "EndlessLandDataReq") 
                CallModuleFunc(ModuleType.CHASM, "OpenView", BaseViewType.CHASM_SHOP)

            -- 积分赛商店
            elseif item_data.system == Macros.Game.SystemID.INTEGRAL_GLORY_SHOP then
                CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",Macros.Game.SHOP.SHOP_SCORE_MATCH)

            else

                local viewInfo = GlobalViewMgr:GetCurViewInfo()
                local spirit_system_info = CPPGameLib.GetConfig("SystemEntry" , item_data.system )
                if spirit_system_info.module_type ~= viewInfo.view_type then
                    GlobalModulesMgr:OpenSystemModule(item_data.system, nil, false)
                end
            end
        end,
        view_height = self.Image_content:GetContentHeight() - 5,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    self.lst_list:SetDataList( tab )
end 


--[[
%% @module: 商店整合条目
%% @author: yjg
%% @created: 2017年10月14日10:03:05
--]]

LogicGUI = LogicGUI or {}
LogicGUI.TemplShopFailarmyItem = LogicGUI.TemplShopFailarmyItem or BaseClass(GUI.ScrollItem)

function LogicGUI.TemplShopFailarmyItem:__init( data )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function LogicGUI.TemplShopFailarmyItem:__delete()
end

function LogicGUI.TemplShopFailarmyItem:__AddToParent()
end

function LogicGUI.TemplShopFailarmyItem:__CreateWidgetFromLuaCode()

    local sp_shop_btn = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SHOP_FAILARMY, "sp_shop_btn")
    local shop_btn = Util:Sprite(sp_shop_btn)
    self.layout = Util:Layout( shop_btn:GetContentWidth() , shop_btn:GetContentHeight() )
    return self.layout
end

function LogicGUI.TemplShopFailarmyItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function LogicGUI.TemplShopFailarmyItem:__ShowUI( data )
    --开启等级
    local system = data.system
    if data.system == Macros.Game.SystemID.UNION_SHOP then
        system = Macros.Game.SystemID.UNION
    end

    --背景
    local sp_shop_btn = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SHOP_FAILARMY, "sp_shop_btn")
    local shop_btn = Util:Sprite(sp_shop_btn)
    self.layout:AddChild( shop_btn )
    PosTool.Center( shop_btn )

    local sp_name = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SHOP_FAILARMY, data.name )
    local icon = Util:Sprite( sp_name)
    icon:SetPosition( shop_btn:GetContentWidth()/2 , shop_btn:GetContentHeight()/2 )
    shop_btn:AddChild( icon )

    --名字
    local sp_name = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SHOP_FAILARMY, data.name , true )
    local name = Util:Sprite( sp_name )
    name:SetPosition( shop_btn:GetContentWidth()/2 , shop_btn:GetContentHeight() - name:GetContentHeight() )
    shop_btn:AddChild( name )

    local is_open = GlobalModulesMgr:IsOpen( system )
    if data.system == Macros.Game.SystemID.UNION_SHOP then
        local unionData = CallModuleFunc(ModuleType.UNION , "UnionData")
        is_open = is_open and not unionData
    end

    --等级判断
    if is_open ~= true then
        --背景置灰
        shop_btn:SetGray(true)
        --名字置灰
        name:SetGray(true)
        --图标置灰
        icon:SetGray(true)
        
        local bg_hero_name3 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_name3" )
        local hero_name = Util:Sprite( bg_hero_name3 )
        hero_name:SetPosition( shop_btn:GetContentWidth()/2 , shop_btn:GetContentHeight()/2 )
        shop_btn:AddChild( hero_name )    
        
        local module_info = CPPGameLib.GetConfig("SystemEntry" , system )
        local text = CPPGameLib.GetString("system_foreshow_openlevel_text", module_info.level)
        if data.system == Macros.Game.SystemID.UNION_SHOP then
            text = CPPGameLib.GetString("union_NoJoin")
        end

        local system_foreshow_openlevel_text = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.Special )
        system_foreshow_openlevel_text:SetAnchorPoint( 0.5 , 0.5 )
        shop_btn:AddChild( system_foreshow_openlevel_text )
        PosTool.Center(system_foreshow_openlevel_text) 
        
    else
        self:__UpdateReddot(shop_btn)
        
        --特效
        if data.effect then
            local btnEffect_id , btnEffect_con = self:__CreateEffect( { res_name = data.effect } , icon )
            btnEffect_con:SetPosition( data.x , data.y )
        end
    end
end

function LogicGUI.TemplShopFailarmyItem:__UpdateReddot(shop_btn)
   local num = 0
    if system == Macros.Game.SystemID.ARENA_REPUTATIONSHOP then
        system = Macros.Game.SystemID.ARENA
    end
    num = CallModuleFunc(ModuleType.RED_MGR , "GetRed" , system )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    local icon_red_point = Util:Sprite(sp)
    icon_red_point:SetPosition(shop_btn:GetContentWidth() - 5 , shop_btn:GetContentHeight() - 5 )
    shop_btn:AddChild(icon_red_point)
    icon_red_point:SetVisible(false)

    if system == Macros.Game.SystemID.UNION and ( num == 0 or not num ) then
        icon_red_point:SetVisible(false)
    elseif system == Macros.Game.SystemID.HERO_SHOP then
        if CallModuleFunc(ModuleType.HERO_SHOP,"GetHeroShopFreeRefresh") == ConfigAdapter.Common.GetHeroShopFreeNum() then
            icon_red_point:SetVisible(true)
        end

    elseif system == Macros.Game.SystemID.SHOP then
        local num = CallModuleFunc(ModuleType.SHOP , "GetPurchasableGiftList" )
        icon_red_point:SetVisible(#num > 0)

    elseif system == Macros.Game.SystemID.SHOP then
        local num = CallModuleFunc(ModuleType.SHOP , "GetPurchasableGiftList" )
        icon_red_point:SetVisible(#num > 0)

    elseif system == Macros.Game.SystemID.TOWER_SHOP then
        local index = CallModuleFunc(ModuleType.TOWER,"GetNowBuyNum")
        if index ~= 0 then
            icon_red_point:SetVisible(true)
        else
            icon_red_point:SetVisible(false)
        end

    elseif system == Macros.Game.SystemID.REBEL_SHOP then
        local index = CallModuleFunc(ModuleType.REBEL, "GetNowBuyNum")
        if index ~= 0 then
            icon_red_point:SetVisible(true)
        else
            icon_red_point:SetVisible(false)
        end

    else 
        icon_red_point:SetVisible(false)
    end
end