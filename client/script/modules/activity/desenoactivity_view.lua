--
-- @brief 限时活动
-- @author: yjg
-- @date: 2017年1月14日11:39:01
--

Modules = Modules or {}

Modules.DesenoActivityView = Modules.DesenoActivityView or BaseClass(BaseView)

function Modules.DesenoActivityView:__init()
  	self.__layout_name = "activity_signIn"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("desenoActivity"),
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
        {Modules.TemplOneRecharge, "Panel_oneRecharge"},
        {Modules.TemplGrandTotalRecharge, "Panel_grandTotalRecharge"},
        {Modules.TemplSummon, "Panel_summon"},
        {Modules.TemplJustFull, "Panel_just_full"},
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ACTIVITY)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ACTIVITY,true)
end

function Modules.DesenoActivityView:__OpenCallback()
end

function Modules.DesenoActivityView:__Dispose()    
end

function Modules.DesenoActivityView:__RegistAllEvents()
    --单充
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE,function (_, data)
        self:__UpdateRedDot( Macros.Game.SystemID.ONE_RECHARGE )
        self:__UpdateRedDot( Macros.Game.SystemID.GRAND_TOTAL_RECHARGE )
    end)

    --招募
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_RECRUIT,function (_, data)
        self:__UpdateRedDot( Macros.Game.SystemID.RECRUIT )
    end)

    --零点刷新
    self:BindGlobalEvent(Macros.Event.SundryModule.NEW_DAY_NOTIFY,function (_, data)
        self:__ShowUI( )
    end)

    --红点更新
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function (_, system_id)
        for _, v in pairs(self.text) do
            if v.value == system_id then
                self:__UpdateRedDot(system_id)
                break
            end
        end
    end)
end

function Modules.DesenoActivityView:__Getwidget()
    --居中层
    self.Panel_center = self:GetWidget("Panel_center") 

    --内容框
    self.Image_1 = self:GetWidget("Image_1")

    --页签
    self.Image_selectLst = self:GetWidget("Image_selectLst")
end

function Modules.DesenoActivityView:__ShowUI()

    self.text = {
            [1] = {name = CPPGameLib.GetString("activity_one_recharge") , value = Macros.Game.SystemID.ONE_RECHARGE },
            [2] = {name = CPPGameLib.GetString("activity_grand_total_recharge") , value = Macros.Game.SystemID.GRAND_TOTAL_RECHARGE },
            [3] = {name = CPPGameLib.GetString("activity_summon") , value = Macros.Game.SystemID.RECRUIT },
            -- [4] = {name = CPPGameLib.GetString("direct_buy_direct") , value = Macros.Game.SystemID.DIRECT_BUY },
    }

    local t1 = {}
    for i , v in ipairs( self.text ) do
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , v.value )
        if info.judge and info.judge == "open" and info.show == "open" then
            local iconSp = CPPGameLib.GetConfig("SystemEntry", v.value )
            if not iconSp then return end
            v.sort = iconSp.order or  0
            table.insert( t1 , v )
        end
    end
    self.text = t1
    table.sort( self.text, function ( a , b )
        return a.sort < b.sort
    end )

    self.__system_id_to_tab_index_map = {}
    for i,v in ipairs(self.text) do
        self.__system_id_to_tab_index_map[v.value] = i
    end

    self.__tab:SetContentSize( self.Image_selectLst:GetContentWidth() + 20 , self.Image_selectLst:GetContentHeight() - 10 )
    self.__tab:SetData( self.text )
    self.__tab:SetItemAlign( 0 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_activity_on")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_activity_no")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition(self.Image_selectLst:GetContentWidth()/2 + 15 , self.Image_selectLst:GetContentHeight()/2 )
    self.Image_selectLst:AddChild(self.__tab:GetNode())
    self.__tab:SetTouchEnabled(true)

    local lst = {}
    lst[Macros.Game.SystemID.ONE_RECHARGE] = 1
    lst[Macros.Game.SystemID.GRAND_TOTAL_RECHARGE] = 2
    lst[Macros.Game.SystemID.RECRUIT] = 3
    -- lst[Macros.Game.SystemID.DIRECT_BUY] = 4

    local index = nil
    for i , v in ipairs( self.text ) do
        if v.value == unpack(self.__open_data) then
            index = i
        end
        self:__UpdateRedDot(v.value)
    end
    self.__tab:On( index or 1 )
end

function Modules.DesenoActivityView:__UpdateRedDot( type )
    local num = 0
    if type == Macros.Game.SystemID.ONE_RECHARGE then
        num = CallModuleFunc(ModuleType.ACTIVITY, "GetOneRechargeNum")
    elseif type == Macros.Game.SystemID.GRAND_TOTAL_RECHARGE then
        num = CallModuleFunc(ModuleType.ACTIVITY, "GetGrandTotalRechargeNum")
    elseif type == Macros.Game.SystemID.RECRUIT  then
        num = CallModuleFunc(ModuleType.ACTIVITY, "GetLimitRecruitNum")
    end

    local index = self.__system_id_to_tab_index_map[type]
    local item = self.__tab:GetBtn_(index)
    local onItem , onIndex = self.__tab:GetOnBtn_()
    if num > 0 and not self.icon_red_point[index] and item then
        self.icon_red_point[index] = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point"))
        if index == onIndex then
            self.icon_red_point[index]:SetPosition( item:GetContentWidth() - 15 , item:GetContentHeight() - 10 )
        else
            self.icon_red_point[index]:SetPosition( item:GetContentWidth() - 2 , item:GetContentHeight() - 9 )
        end
        item:AddChild(self.icon_red_point[index] , 20)
    end
    if self.icon_red_point[index] then
        self.icon_red_point[index]:SetVisible(num > 0)
    end
end

function Modules.DesenoActivityView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_activity_on")
    local sizeSp = Util:Sprite( on )

    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    if data.value then
        local iconSp = CPPGameLib.GetConfig("SystemEntry", data.value )
        if not iconSp then return end
        local icon = Util:Sprite( Resource.PathTool.GetSystemEntryIconPath( iconSp.team_icon ) )
        icon:SetAnchorPoint( 0.5 , 0.5 )
        icon:SetScaleFactor( 0.8 )
        icon:SetPosition( (icon:GetContentWidth()/2) + 5  , icon:GetContentHeight()/2 )
        layer:AddChild(icon)

        local name = Util:Label( data.name , Macros.TypefaceSize.popup , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_yellow )
        name:SetAnchorPoint( 0.5 , 0.5 )
        name:SetPosition( icon:GetContentWidth() + icon:GetPositionX() , layer:GetContentHeight()/2 )
        layer:AddChild( name )
    else
        local name = Util:Label( data.name , Macros.TypefaceSize.popup , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_yellow )
        name:SetAnchorPoint( 0.5 , 0.5 )
        name:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight()/2 )
        layer:AddChild( name )
    end

    return layer
end

function Modules.DesenoActivityView:OnCallBack( index , data )

    local lst = {}
    lst[Macros.Game.SystemID.ONE_RECHARGE] = 1
    lst[Macros.Game.SystemID.GRAND_TOTAL_RECHARGE] = 2
    lst[Macros.Game.SystemID.RECRUIT] = 3
    -- lst[Macros.Game.SystemID.DIRECT_BUY] = 4

    self:__TabChange( lst[self.text[index].value] )

    self.index = index
    local item = self.__tab:GetBtn_( index )
    for i = 1 , #self.text do
      if self.icon_red_point[i] then
         self.icon_red_point[i]:SetPosition( item:GetContentWidth() - 30 , item:GetContentHeight() - 10 )
      end
    end
    if self.icon_red_point[index] then
        self.icon_red_point[index]:SetPosition( item:GetContentWidth() - 15 , item:GetContentHeight() - 10 )
    end
end

--获取打开参数
function Modules.DesenoActivityView:GetOpenParams()
    if self.index then
        return {self.index}
    end
end
