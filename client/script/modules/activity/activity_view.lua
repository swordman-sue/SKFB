--
-- @brief 活动主界面
--

Modules = Modules or {}

Modules.ActivityView = Modules.ActivityView or BaseClass(BaseView)

function Modules.ActivityView:__init()
    self.__layout_name = "activity_signIn"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("everydayActivity"),
      show_diamond = true,
      show_gold = true,
      show_stamina = true,
      show_energy = true,
      close_func = function()
          self:CloseManual()
      end,
    }

    self.__tab_params_ex = {6}

    self.__tab_view_params = 
    {
        {Modules.TemplDailySignin, "Panel_Registration"},
        {Modules.TemplMedicine, "Panel_Medicine"},
        {Modules.TemplFairyMoney, "Panel_Money"},
        {Modules.TemplLevelShop, "Panel_LevelShop"},
        {Modules.TemplLevelGiftBag, "Panel_LevelGiftBag"},
        {Modules.TemplCdkey, "Panel_CDKEY"},
        {Modules.TemplWelfare, "Panel_Welfare"},
        {Modules.TemplMember, "Panel_Member"},
        {Modules.TemplFund, "Panel_Fund"},
        {Modules.TemplRechargeWelfare, "Panel_RechargeWelfare"},
        {Modules.TemplLoginReward, "Panel_registerAward"},
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ACTIVITY)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ACTIVITY,true)
    
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_LEVEL_ITEM)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_LEVEL_GIFTBAG_ITEM)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_WELFARE_ITEM)
end

function Modules.ActivityView:__OpenCallback()
    self.icon_red_point = {}
    self:__Getwidget()
    self:__ShowUI() 
end

function Modules.ActivityView:__Dispose()    
end

function Modules.ActivityView:__RegistAllEvents()
    --红点更新事件
    self:BindGlobalEvent( Macros.Event.RedModule.RED_UPDATE , function ( _ , system_id , sub_system_id )
        if system_id == Macros.Game.SystemID.ACTIVITY then
            self:__UpdateRedDot( sub_system_id )
        end
    end )
end

function Modules.ActivityView:__Getwidget()
    --居中层
    self.Panel_center = self:GetWidget("Panel_center") 

    --内容框
    self.Image_1 = self:GetWidget("Image_1")

    --页签
    self.Image_selectLst = self:GetWidget("Image_selectLst")
end

function Modules.ActivityView:__ShowUI()
    self.__tab_list = {
                    [1] = { 
                                system_id = Macros.Game.SystemID.DAILY_SIGNIN , 
                                system_tab = 1 
                            },
                    [2] = { 
                                system_id = Macros.Game.SystemID.TQT , 
                                system_tab = 2  
                            },
                    [3] = { 
                                system_id = Macros.Game.SystemID.YCS , 
                                system_tab = 3  
                            },
                    [4] = { 
                                system_id = Macros.Game.SystemID.LEVEL_SHOP , 
                                system_tab = 4 
                            },
                    [5] = { 
                                system_id = Macros.Game.SystemID.LEVEL_GIFTBAG , 
                                system_tab = 5 
                            },
                    [6] = { 
                                system_id = Macros.Game.SystemID.CDKEY  , 
                                system_tab = 6 
                            },
                    [7] = { 
                                system_id = Macros.Game.SystemID.VIP_WELFARE , 
                                system_tab = 7 
                            },
                    [8] = { 
                                system_id = Macros.Game.SystemID.LUNA_WELFARE , 
                                system_tab = 8 
                            },
                    [9] = { 
                                system_id = Macros.Game.SystemID.FUND , 
                                system_tab = 10 
                            },
                    [10] = { 
                                system_id = Macros.Game.SystemID.REBATE , 
                                system_tab = 9 
                            },
                    [11] = { 
                                system_id = Macros.Game.SystemID.REGISTER_AWARD , 
                                system_tab = 11 
                            },
    }

    --检测开启的活动
    local t1 = {}
    for i , v in ipairs( self.__tab_list ) do
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , v.system_id )
        if not info or info.judge then
            local IsOpen = GlobalModulesMgr:IsOpen( v.system_id )
            if IsOpen then
                local config_system = CPPGameLib.GetConfig("SystemEntry", v.system_id )
                v.system_name = config_system.name
                v.sort = config_system.order or  0
                table.insert( t1 , v )
            end
        end
    end
    --对活动进行排序
    self.__tab_list = t1
    table.sort( self.__tab_list, function ( a , b )
        return a.sort < b.sort
    end )

    self.__system_id_to_tab_index = {}
    for i,v in ipairs( self.__tab_list ) do
        self.__system_id_to_tab_index[ v.system_id ] = i
    end

    --创建活动列表
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_activity_on")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_activity_no")
    self.__tab:SetContentSize( self.Image_selectLst:GetContentWidth() + 20 , self.Image_selectLst:GetContentHeight() - 10 )
    self.__tab:SetItemAlign( 0 )
    self.__tab:SetData( self.__tab_list )
    self.__tab:SetOnCeil( on , no )  
    self.__tab:SetPosition(self.Image_selectLst:GetContentWidth()/2 + 15 , self.Image_selectLst:GetContentHeight()/2 )
    self.__tab:SetTouchEnabled(true)
    self.__tab.lst_list:ForceDoLayout() 
    self.Image_selectLst:AddChild(self.__tab:GetNode())

    local init_tab_index = 1
    local tgt_system_id = unpack(self.__open_data)
    --指定跳转到某个活动
    if tgt_system_id then
        init_tab_index = self.__system_id_to_tab_index[tgt_system_id]

    --跳转到有红点的活动
    elseif not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
        local sprite_reddot
        for tab_index, v in ipairs(self.__tab_list) do
            local red_num = CallModuleFunc( ModuleType.RED_MGR , "GetRed", Macros.Game.SystemID.ACTIVITY , v.system_id )
            if red_num > 0 then
                init_tab_index = tab_index
                break
            end
        end
    end
    self.__tab:On(init_tab_index, nil)
    self.__tab:LocateToItemByIndex(init_tab_index)
end

function Modules.ActivityView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_activity_on")
    local sizeSp = Util:Sprite( on )

    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local config_system = CPPGameLib.GetConfig("SystemEntry", data.system_id )
    if not config_system then return end
    
    local icon = Util:Sprite( Resource.PathTool.GetSystemEntryIconPath( config_system.team_icon ) )
    icon:SetAnchorPoint( 0.5 , 0.5 )
    icon:SetScaleFactor( 0.8 )
    icon:SetPosition( (icon:GetContentWidth()/2) + 5  , icon:GetContentHeight()/2 )
    layer:AddChild(icon)

    local name = Util:Label( data.system_name , Macros.TypefaceSize.popup , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_yellow )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( icon:GetContentWidth() + icon:GetPositionX() , layer:GetContentHeight()/2 )
    layer:AddChild( name )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point[index] = Util:Sprite(icon_red_point)
    self.icon_red_point[index]:SetPosition( layer:GetContentWidth()  , layer:GetContentHeight() - 10 )
    layer:AddChild( self.icon_red_point[index] , 20 )
    self.icon_red_point[index]:SetVisible(false)
    self:__UpdateRedDot(data.system_id)

    return layer
end

function Modules.ActivityView:OnCallBack( index , data )
    self:__TabChange( self.__tab_list[index].system_tab )

    self.__cur_system_id = self.__tab_list[index].system_id
    
    local item = self.__tab:GetBtn_( index )
    for i = 1 , #self.__tab_list do
        if self.icon_red_point[i] then
            self.icon_red_point[i]:SetPosition( item:GetContentWidth() - 30 , item:GetContentHeight() - 10 )
        end
    end
    if self.icon_red_point[index] then
        self.icon_red_point[index]:SetPosition( item:GetContentWidth() - 15 , item:GetContentHeight() - 10 )
    end
end

function Modules.ActivityView:__UpdateRedDot( sub_system_id )
    local index = self.__system_id_to_tab_index[ sub_system_id ]
    local red_num = CallModuleFunc( ModuleType.RED_MGR , "GetRed", Macros.Game.SystemID.ACTIVITY , sub_system_id )
    if self.icon_red_point[index] then
        self.icon_red_point[index]:SetVisible(red_num > 0)
    end
end

--获取打开参数
function Modules.ActivityView:GetOpenParams()
    if self.__cur_system_id then
        return {self.__cur_system_id}
    end
end
