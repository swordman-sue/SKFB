--
-- @brief 冲榜活动 
-- @author: yjg
-- @date: 2017年9月6日18:00:33
--

Modules = Modules or {}

Modules.RankActivityView = Modules.RankActivityView or BaseClass(BaseView)

function Modules.RankActivityView:__init()
  	self.__layout_name = "rank_activity"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_rank_activity")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("rank_activity") ,
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            if self.endTou then
                self.endTou:RemoveSelf(true)
                self.endTou = nil 
            end

            if self.Selected == true then
                self:InitialShow()
            else
                self:CloseManual()
            end
        end,
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RANK_ACTIVITY,true)
end

function Modules.RankActivityView:__Dispose()   

end

function Modules.RankActivityView:__OpenCallback()
    self:__Getwidget()

    self:__ShowUI()
end

function Modules.RankActivityView:__Getwidget()
    --居中层
    self.Panel_center = self:GetWidget("Panel_center")
    
    --三个旗帜 
    self.Panel_rank = {}
    for i = 1 , 3 do
        self.Panel_rank[i] = self:GetWidget("Panel_rank_"..i)
        self.Panel_rank[i]:SetScaleFactor(0.8)
        WidgetUnity.SetWidgetListener( self.Panel_rank[i] , nil, nil, function ( ... )
            --旗帜消失
            self:PlayActionTimeline("vanish", false , function( ... )
                self:SetSelectedFlag(i)
            end)
        end)
    end
    --选中旗帜
    self.Panel_option = self:GetWidget("Panel_option")
    --按钮层
    self.Panel_button = self:GetWidget("Panel_button")

    --向左
    self.Button_left = Util:WidgetButton( self:GetWidget("Button_left") , nil , function ( ... )
        self:OnButtonDirection( -1 )
    end ) 
    
    --向右
    self.Button_right = Util:WidgetButton( self:GetWidget("Button_right") , nil , function ( ... )
        self:OnButtonDirection( 1 )
    end ) 

    --排行榜
    self.Panel_rank = self:GetWidget("Panel_rank")
    --排行榜背景
    self.Image_ranklst = self:GetWidget("Image_ranklst")
    --区间
    self.Image_section = self:GetWidget("Image_section")
    --奖励
    self.Image_award = self:GetWidget("Image_award")
    --排行榜列表
    self.Panel_lst = self:GetWidget("Panel_lst")

    --查看排行榜
    self.Button_check = Util:WidgetButton( 
                                            self:GetWidget("Button_check"), 
                                            CPPGameLib.GetString("rank_activity_check") , 
                                            CPPGameLib.Handler( self,self.OnButtonCheck ) , 
                                            nil,nil,Macros.Color.button_yellow 
                                        )  


    --提示
    self.Text_tips = self:GetWidget("Text_tips")  
    
    --我的排行
    self.Text_myRank = self:GetWidget("Text_myRank") 

    --结算时间
    self.Text_Settlement_time = self:GetWidget("Text_Settlement_time")  

end

function Modules.RankActivityView:__ShowUI()
    local effectid, effect = self:__CreateEffect({res_name = "UI_chongbang",loop = true}, self.__root_widget )
    PosTool.Center(effect)

    --进入界面时
    if not unpack(self.__open_data) then
        --播放动画
        self:PlayActionTimeline("appear", false,nil)
    end

    --列表
    local params =  
    {
        item_class = Modules.TemplRankActivityItem,
        item_width = self.Panel_lst:GetContentWidth() ,
        item_height = 85,
        row = 4,
        col = 1,
        view_height = self.Panel_lst:GetContentHeight() ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    --排名区间
    local arena_rankZoom = Util:Label( CPPGameLib.GetString("arena_rankZoom") , Macros.TypefaceSize.popup, Macros.Color.btn )
    self.Image_section:AddChild( arena_rankZoom )
    PosTool.Center( arena_rankZoom )

    --排名奖励
    local arena_rankEncourage = Util:Label( CPPGameLib.GetString("arena_rankEncourage") , Macros.TypefaceSize.popup, Macros.Color.btn )
    self.Image_award:AddChild( arena_rankEncourage )
    PosTool.Center( arena_rankEncourage )

    --我的排名
    Util:WidgetLabel(self.Text_myRank ,CPPGameLib.GetString("arena_myRank"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
    self.myRank = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.content )
    self.myRank:SetAnchorPoint(0,0.5)
    self.Panel_rank:AddChild(self.myRank)    
    PosTool.RightTo( self.Text_myRank , self.myRank )

    --榜单结算时间
    Util:WidgetLabel( self.Text_Settlement_time ,CPPGameLib.GetString("rank_settlement_time"),Macros.TypefaceSize.normal, Macros.Color.keypoint )  

    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , 97 )
    local overTime_ = Util:Label( TimeTool.GetTimeFormat(info.over, TimeTool.defaul_format[21] ) , Macros.TypefaceSize.normal , Macros.Color.content )
    overTime_:SetAnchorPoint(0,0.5)
    self.Panel_rank:AddChild(overTime_)    
    PosTool.RightTo( self.Text_Settlement_time , overTime_ )

    --我的排名
    Util:WidgetLabel(self.Text_tips ,CPPGameLib.GetString("arena_myRank"),Macros.TypefaceSize.normal, Macros.Color.content )

    if unpack(self.__open_data) then
        self:SetSelectedFlag( unpack(self.__open_data) )
    end
end

--选中旗帜
function Modules.RankActivityView:SetSelectedFlag( index )
    CallModuleFunc(ModuleType.RANK_ACTIVITY , "MYRankReq", index )

    self.endTou = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height ,function ( ... )
        -- body
    end)
    self.endTou:SetAnchorPoint( 0.5 , 0.5 )
    self:AddChild( self.endTou )
    PosTool.Center( self.endTou )

    self.Selected = true
    self.index = index
    local lst = {
              [1] = { sp = "athletics_rank" , lst = "GetAthleticsCnf" ,txt = "rank_activity_tips1" },
              [2] = { sp = "mainline_rank" , lst = "GetMainlinesCnf" ,txt = "rank_activity_tips2" },
              [3] = { sp = "power_rank" , lst = "GetPowerCnf" ,txt = "rank_activity_tips3" },
    }
    --设置选中
    self.Panel_option:SetBackGroundImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RANK_ACTIVITY , lst[index].sp , true) )
    --设置列表
    self.lst_list:SetDataList( CallModuleFunc(ModuleType.RANK_ACTIVITY, lst[index].lst ) )
    --提示
    self.Text_tips:SetText( CPPGameLib.GetString( lst[index].txt ) )

    self:PlayActionTimeline("merge", false , function ( ... )
        if self.endTou then
            self.endTou:RemoveSelf(true)
            self.endTou = nil 
        end
    end )
end

--左右按钮
function Modules.RankActivityView:OnButtonDirection( index )
    if self.index + index > 3 then
        self.index = 1
    elseif self.index + index < 1 then
        self.index = 3
    else
        self.index = self.index + index
    end
    self:SetSelectedFlag( self.index ) 
end

--散开
function Modules.RankActivityView:InitialShow( index )
    self.Selected = false
    --散开
    self:PlayActionTimeline("separate", false , function ( ... )
        self:PlayActionTimeline("appear", false,nil)
    end)
end

--查看排行
function Modules.RankActivityView:OnButtonCheck()
    if self.index == 1 then
        CallModuleFunc(ModuleType.RANK,"OpenView", BaseViewType.RANK , 5 )
    elseif self.index == 2 then
        CallModuleFunc(ModuleType.RANK,"OpenView", BaseViewType.RANK , 3 )
    elseif self.index == 3 then
        CallModuleFunc(ModuleType.RANK,"OpenView", BaseViewType.RANK , 1 )
    end
end

--获取打开参数
function Modules.RankActivityView:GetOpenParams()
    if self.index then
        return {self.index}
    end
end

--设置我的排位
function Modules.RankActivityView:SetMyRank( num )
    local text = CPPGameLib.GetString( "tower_noranking" )
    if num ~= 0 then
       text = CPPGameLib.GetString( "rebel_mingci" , num )
    end
    self.myRank:SetString( text )
end

Modules = Modules or {}
Modules.TemplRankActivityItem = Modules.TemplRankActivityItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRankActivityItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplRankActivityItem:__CreateWidgetFromLuaCode()  
    self.layout = Util:Layout(666 , 85)
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.TemplRankActivityItem:__delete()
end

function Modules.TemplRankActivityItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TemplRankActivityItem:__ShowUI( data )
    local sp = nil
    if data.show == true then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
        local sp_2 = Util:Sprite9(sp)
        sp_2:SetContentSize(self.layout:GetContentWidth()  , self.layout:GetContentHeight() )
        sp_2:SetPosition(self.layout:GetContentWidth() / 2 , self.layout:GetContentHeight() /2)
        self.layout:AddChild(sp_2)
    end

    if not data.rank then
        return
    end

    --区间
    if data.rank <= 3 then
        local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
        local icon = Util:Sprite(imageData)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetPosition( 90 , self.layout:GetContentHeight() / 2)
        self.layout:AddChild(icon)
    else
        local iconValue = Util:Label(data.section,nil,Macros.Color.headline)
        iconValue:SetPosition( 90 , self.layout:GetContentHeight() / 2)
        self.layout:AddChild(iconValue)  
    end

    --奖励
    for i , v in ipairs( data.award ) do
        --图标
        local icon_layer = LogicGUI.IconLayer.New(true)
        icon_layer:SetData({id = v.id , num = v.num })
        icon_layer:SetPosition( 210 + (90 * (i - 1))  , self.layout:GetContentHeight()/2)
        icon_layer:SetAnchorPoint(0,0.5)
        icon_layer:SetScaleFactor(0.95)
        icon_layer:SetTouchSwallow(false)
        self.layout:AddChild(icon_layer:GetNode())  
    end

end

