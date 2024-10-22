--
-- @brief 节日活动
-- @author: yjg
-- @date: 2017年9月13日17:02:30
--

Modules = Modules or {}

Modules.SpecialActivityView  = Modules.SpecialActivityView or BaseClass(BaseView)

function Modules.SpecialActivityView:__init()
    self.__layout_name = "activity_new"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_activity")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("Special"),
      show_diamond = true,
      show_gold = true,
      show_stamina = true,
      show_energy = true,
      close_func = function()
          self:CloseManual()
      end,
    }


    self.__tab_params_ex = {6 , true}

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ACTIVITY_NEW)
 end

function Modules.SpecialActivityView:__OpenCallback()
    self.tabIcon = {}
    self.tabName = {}
    self.tabRed = {}

    self:__Getwidget()
    self:__ShowUI() 
    self:__Server() 
end

function Modules.SpecialActivityView:__Dispose()    

end

function Modules.SpecialActivityView:__RegistAllEvents()

end

function Modules.SpecialActivityView:__Server()
    CallModuleFunc(ModuleType.ACTIVITY,"FestivalActivityRedPointReq")
        
    --获得装备
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_FESTIVAL,function (_, data)
        self:__SetShowUI( self.data )  
    end)

    --获得装备
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_FESTIVAL_RED,function (_, data)
        self:SetRed( )  
    end)

    --获得装备
    self:BindGlobalEvent(Macros.Event.SundryModule.NEW_DAY_NOTIFY,function (_, data)

        CallModuleFunc(ModuleType.ACTIVITY,"FestivalActivityRedPointReq")
        local lst = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityCnf")
        table.sort( lst, function ( a , b )
            return a.sort > b.sort
        end )
        self.__tab:On( 1 , lst[1] )
    end)
end

function Modules.SpecialActivityView:__Getwidget()
    --后台层
    self.Panel_background = self:GetWidget("Panel_background") 
    --模型层
    self.Panel_hero = self:GetWidget("Panel_hero")
    ActionManager.GetInstance():ZoomAnimation( self.Panel_hero , 1 )
    --吧台
    self.Image_counter = self:GetWidget("Image_counter")

    --货柜
    self.Image_container = self:GetWidget("Image_container")
    --货柜列表
    self.Panel_lst = self:GetWidget("Panel_lst")
    --列表选项
    self.Image_iconLst = self:GetWidget("Image_iconLst")
end

function Modules.SpecialActivityView:__ShowUI()

    local params =  
    {
        item_class = Modules.TemplSpecialItem,
        item_width = self.Panel_lst:GetContentWidth() ,
        item_height = 137,
        row = 4,
        col = 1,
        item_space_r = 5,
        view_height = self.Panel_lst:GetContentHeight() ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    local lst = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityCnf")
    table.sort( lst, function ( a , b )
        return a.sort > b.sort
    end )
    self.__tab:SetContentSize( self.Image_iconLst:GetContentWidth() , self.Image_iconLst:GetContentHeight() )
    self.__tab:SetData( lst )
    self.__tab:SetCeil()
    self.__tab:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_iconLst:AddChild( self.__tab:GetNode() )
    PosTool.Center( self.__tab )

    local index = nil
    local data = nil
    if self.__open_data then
        index = self.__open_data[1]
        data = lst[index] or self.__open_data[2]
    end

    --为打脸推送准备
    if data == "push" then
        for i , v in ipairs( lst ) do
            if v.activity_type == self.__open_data[3] then
                index = i
                data = v
            end
        end
    end
    if data == "push" then
        return 
    end
    self.__tab:On( index or 1 , data )
end

--页签
function Modules.SpecialActivityView:Ceil( index , data )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , data.icon )
    local sizeSp = Util:Sprite( on )

    local layer = Util:Layout( sizeSp:GetContentWidth() + 20 , sizeSp:GetContentHeight() + 10 )

    self.tabIcon[index] = Util:Sprite( Resource.PathTool.GetSystemEntryIconPath( data.icon ) )
    self.tabIcon[index]:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(  self.tabIcon[index] )
    PosTool.Center(  self.tabIcon[index] )

    local sp_activity_division = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , "sp_activity_division" ) )
    sp_activity_division:SetAnchorPoint( 0.5 , 0.5 )
    sp_activity_division:SetPosition( layer:GetContentWidth() , layer:GetContentHeight()/2 )
    layer:AddChild( sp_activity_division )

    self.tabName[index] = Util:Name( data.name , Macros.TypefaceSize.normal , 0 ) 
    self.tabName[index]:SetAnchorPoint( 0.5 , 0.5 )
    self.tabName[index]:SetPosition( layer:GetContentWidth()/2 , 18.5 )
    layer:AddChild( self.tabName[index] )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.tabRed[data.activity_type] = Util:Sprite( icon_red_point )
    self.tabRed[data.activity_type]:SetAnchorPoint(0.5,0.5)
    self.tabRed[data.activity_type]:SetPosition( layer:GetContentWidth() - 10 , layer:GetContentHeight() - 18 )
    layer:AddChild( self.tabRed[data.activity_type] )
    self.tabRed[data.activity_type]:SetVisible(false)

    return layer
end

--选中
function Modules.SpecialActivityView:OnCallBack( index , data )
    self.index = index
    --复原图标
    for i = 1 , 99 do
        local item , data = self.__tab:GetBtn(i)
        if item and i == index then
            self.tabIcon[i]:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW, data.icon ) )
            self.tabName[i]:SetColor(unpack(Macros.Quality[100]))
            self.tabName[i]:SetStrokeColor(unpack(Macros.Quality_Stroke[100]))
        elseif  item and i ~= index then
            self.tabIcon[i]:SetImage( Resource.PathTool.GetSystemEntryIconPath( data.icon ) )
            self.tabName[i]:SetColor(unpack(Macros.Quality[0]))
            self.tabName[i]:SetStrokeColor(unpack(Macros.Quality_Stroke[0]))
        elseif not item then
            break
        end
    end

    if data and data ~= "push" then
        CallModuleFunc(ModuleType.ACTIVITY,"FestivalActivityDataReq" , data )
        self.data = data
        self:__SetShowUI( data ) 
    end
    
end

--设置限时ui
function Modules.SpecialActivityView:__SetShowUI( data )

    local LALALAX = 0
    local LALALAY = 0

    --设置半身像
    if not self.head and data.head then
        self.head = Util:Sprite( Resource.PathTool.GetHalfHeadPath( data.head ) )
        self.head:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_hero:AddChild( self.head )
        self.head:SetVisible(true)
    else
        if data.head then
            self.head:SetImage( Resource.PathTool.GetHalfHeadPath( data.head ) )
            self.head:SetVisible(true)
        else
            if self.head then
                self.head:SetVisible(false)
            end
        end
    end
    if self.head then
        if data.excursion then
            self.head:SetPosition( data.excursion[1] + LALALAX , data.excursion[2] + LALALAY )
        end
        if data.scale then
            self.head:SetScaleFactor( data.scale )
        end
    end

    --设置装饰
    if not self.decorate then
        self.decorate = {}
    end

    for i = 1 , #self.decorate do
        self.decorate[i]:RemoveSelf(true)
        self.decorate[i] = nil
    end

    if data.decorate then
        for i = 1 , #data.decorate do
            self.decorate[i] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW, data.decorate[i] ) )
            self.decorate[i]:SetAnchorPoint( 0.5 , 0.5 )
            self.decorate[i]:SetPosition( data.decorate_x[i] , data.decorate_y[i] )   
            self.Image_counter:AddChild( self.decorate[i] )
        end
    end
    
    --设置字体
    if not self.test_res then
        self.test_res = {}
    end
    for i = 1 , #self.test_res do
        self.test_res[i]:RemoveSelf(true)
        self.test_res[i] = nil
    end 

    if data.test_res then
        for i = 1 , #data.test_res do
            local sp = Resource.PathTool.GetUIResPath("activitybig/" .. data.test_res[i] , true )
            self.test_res[i] = Util:Sprite( sp )
            self.test_res[i]:SetAnchorPoint( 0.5 , 0.5 )
            self.test_res[i]:SetPosition( data.test_res_x[i] , data.test_res_y[i] )   
            self.Image_counter:AddChild( self.test_res[i] )
        end
    end 

    --设置特效
    if not self.effect then
        self.effect = {}
    end

    for i = 1 , #self.effect do
        BaseView.__DelEffect(self, self.effect[i])
        self.effect[i] = nil
    end

    if data.effect then
        for i = 1 , #data.effect do
            local effect_con = nil
            self.effect[i] , effect_con = self:__CreateEffect({id = data.effect[i] }, self.Image_counter )
        end
    end

    --设置描述
    if not self.desc then
        self.desc = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.white , 370 )
        self.desc:SetAnchorPoint( 0.5 , 1 ) 
        self.desc:SetPosition( self.Image_counter:GetContentWidth()/2 + 5 , self.Image_counter:GetContentHeight()/2 + 60 )   
        self.Image_counter:AddChild( self.desc )
    end

    if data.desc then
        self.desc:SetString( data.desc )
    else
        self.desc:SetString( "" )
    end

    --设置开始时间
    local lst = CallModuleFunc(ModuleType.ACTIVITY,"ToTypeSettleActivityType" , data.time_type , data )
    if not self.star_date then
        self.star_date = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.white )
        self.star_date:SetAnchorPoint( 0.5 , 1 ) 
        self.star_date:SetPosition( self.Image_counter:GetContentWidth()/2  , self.Image_counter:GetContentHeight()/2 - 35 )   
        self.Image_counter:AddChild( self.star_date )
    end

    self.star_date:SetString( CPPGameLib.GetString("superDiscount_openTime" , TimeTool.GetTimeFormat( lst.star , TimeTool.defaul_format[21] ) ) )

    --设置结束时间
    local lst = CallModuleFunc(ModuleType.ACTIVITY,"ToTypeSettleActivityType" , data.time_type , data )
    if not self.over_date then
        self.over_date = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.white )
        self.over_date:SetAnchorPoint( 0.5 , 1 ) 
        self.over_date:SetPosition( self.Image_counter:GetContentWidth()/2  , self.Image_counter:GetContentHeight()/2 - 60 )   
        self.Image_counter:AddChild( self.over_date )
    end

    self.over_date:SetString( CPPGameLib.GetString("superDiscount_endTime" , TimeTool.GetTimeFormat( lst.over , TimeTool.defaul_format[21] ) ) )

    --设置列表
    local t1 = {}
    for i,v in ipairs( data.param_list ) do
        local cnf = CPPGameLib.GetConfig("FestivalActivityParam", v )
        if not cnf then 
            return 
        end
        table.insert( t1 , { cnf = cnf , id = v , type_ = data.activity_type } )
    end

    local diamond_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)

    local t4 = {}
    local t2 = {}
    local t3 = {}
    for i , v in ipairs( t1 ) do
        local cnf , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem", v.id , v.type_ )

        --单充
        if cnf.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ONCE_PAY then
            --可以领取
            if info and info.take_reward_times < cnf.reward_times and info.take_reward_times ~= info.context then
                table.insert( t4  , v )
            --领取完毕
            elseif info and info.take_reward_times >= cnf.reward_times  then
                table.insert( t2  , v )
            else
                table.insert( t3  , v )
            end
        --登录
        elseif cnf.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ENTER then

            --可以领取
            if info and info.context and info.take_reward_times < cnf.reward_times then
                table.insert( t4  , v )
            --领取完毕
            elseif info and info.take_reward_times >= cnf.reward_times  then
                table.insert( t2  , v )
            else
                table.insert( t3  , v )
            end
        --打折
        elseif cnf.logic_type == Macros.Game.ACTIVITY_FESTIVAL.SALE then

            if (not info and diamond_num >= cnf.x) or ( info and info.take_reward_times < cnf.reward_times and diamond_num >= cnf.x ) then
                table.insert( t4  , v )
            elseif (not info and diamond_num < cnf.x) or ( info and info.take_reward_times < cnf.reward_times and diamond_num < cnf.x ) then
                table.insert( t2  , v )
            else
                table.insert( t3  , v )
            end
        --兑换
        elseif cnf.logic_type == Macros.Game.ACTIVITY_FESTIVAL.CONVER then

            local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", cnf.x )
            --可以领取
            if ( not info and item_num >= cnf.y) or (info and info.take_reward_times < cnf.reward_times and item_num >= cnf.y ) then
                table.insert( t4  , v )
            --不可领取
            elseif item_num < cnf.y or (info and info.take_reward_times < cnf.reward_times) then
                table.insert( t2  , v )
            else
                table.insert( t3  , v )
            end
        --?
        elseif cnf.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OPEN_SERVICE then
            -- 可以领取
            if info and info.context and info.take_reward_times < cnf.reward_times then
                table.insert( t4  , v )
            --领取完毕
            elseif info and info.take_reward_times >= cnf.reward_times  then
                table.insert( t2  , v )
            else
                table.insert( t3  , v )
            end
        else
            --可以领取
            if info and info.context >= cnf.x and info.take_reward_times < cnf.reward_times then
                table.insert( t4  , v )
            --领取完毕
            elseif info and info.take_reward_times >= cnf.reward_times then
                table.insert( t2  , v )
            else
                table.insert( t3  , v )
            end
        end
    end

    for i,v in ipairs(t2) do
        table.insert( t3 , v )
    end

    for i,v in ipairs( t3 ) do
        table.insert( t4  , v )
    end

    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local t5 = {}
    for i , v in ipairs( t4 ) do
        if v.cnf.vip_precondition then
            if v.cnf.vip_precondition <= userDate.vip_exp then
                table.insert( t5 , v )
            end
        else
            table.insert( t5 , v )
        end
    end

    self.lst_list:SetDataList( t5 )
end

--红点
function Modules.SpecialActivityView:SetRed()
    local cnf = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityCnf" )
    for i , v in ipairs( cnf ) do
        local type_ = CallModuleFunc(ModuleType.ACTIVITY,"GetTypeFestivalActivityRed" , v.activity_type )
        if self.tabRed[v.activity_type] then
            if type_ == true then
                self.tabRed[v.activity_type]:SetVisible(true)
            else
                self.tabRed[v.activity_type]:SetVisible(false)
            end
        end
    end
end

--获取打开参数
function Modules.SpecialActivityView:GetOpenParams()
    if self.index then
        return {self.index , self.data }
    end
end
