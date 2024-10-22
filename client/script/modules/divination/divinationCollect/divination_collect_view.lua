--[[
%% @module: 占卜收藏室
%% @author: yjg
%% @created: 2017年11月22日12:01:07
--]]

Modules = Modules or {} 
Modules.DivinationCollectView = Modules.DivinationCollectView or BaseClass(BaseView)

local achievement_item_scale = 0.75

function Modules.DivinationCollectView:__init()
    self.__layout_name = "divination_collect"   
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_divination_book"), image_size = COCOSize(1136,640) }
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination_collect"), 
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__tab_params_ex = {6 , true}

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_HERO_SHOP_ITEM)   
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIVINATION_COLLECT)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIVINATION_COLLECT , true )
end

function Modules.DivinationCollectView:__Dispose()
end

function Modules.DivinationCollectView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end 

function Modules.DivinationCollectView:__Server( )
    CallModuleFunc(ModuleType.DIVINATION , "CollectionBookListReq" )

    --收藏室
    self:BindGlobalEvent(Macros.Event.DivinationModule.COLLECT_BOOK_UP , function ( ... )
        self:__SetCollectShowUI()
        self:__SetShowUI()
    end )

    --成就
    self:BindGlobalEvent(Macros.Event.DivinationModule.COLLECT_ACHIEVEMENT , function ( ... )
        local achievement_level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )

        self:__SetAchievementShowUI(achievement_level)
        
        local item = self.__collect_scrollview:GetItem(achievement_level)
        if item then
            item:UpdateAchievementStatus()
        end

        --效果表现
        self:AfterActivatedTheAchievement()

        self:__SetShowUI()
    end )

    --激活成就表现
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_COLLECT_ACHIEVEMENT_ACTIVATED, function (_,data,index)
        self:DivinationCollectAchievementActivated(data,index)
    end)

    --红点刷新
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function ( _ , system_id )
        if system_id == Macros.Game.SystemID.DIVINATION then
            self:__UpdateRed( )
        end
    end)
end

--获得组件
function Modules.DivinationCollectView:__Getwidget( )
    --显示信息1
    self.Image_ui_1 = self:GetWidget("Image_ui_1")
    --显示信息2
    self.Image_ui_2 = self:GetWidget("Image_ui_2")
    --列表容器
    self.Panel_lst = self:GetWidget("Panel_lst")

    --帮助
    self.Button_help = Util:WidgetButton( 
                                              self:GetWidget("Button_help") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonHelp)
                                          )
    local Button_help = Util:Name( CPPGameLib.GetString("divination_help") )
    Button_help:SetPosition( self.Button_help:GetContentWidth()/2 , 10 )
    Button_help:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_help:AddChild( Button_help )

    --排行
    self.Button_rank = Util:WidgetButton( 
                                              self:GetWidget("Button_rank") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonRank)
                                          )
    self.Button_rank:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_rank"), TextureResType.LOCAL )

    local Button_rank = Util:Name( CPPGameLib.GetString("divination_rank") )
    Button_rank:SetPosition( self.Button_rank:GetContentWidth()/2 , 10 )
    Button_rank:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_rank:AddChild( Button_rank )

    --背包
    self.Button_pack = Util:WidgetButton( 
                                              self:GetWidget("Button_pack") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonPack)
                                          )
    self.Button_pack:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_bag"), TextureResType.LOCAL )

    local Button_pack = Util:Name( CPPGameLib.GetString("divination_pack") )
    Button_pack:SetPosition( self.Button_pack:GetContentWidth()/2 , 10 )
    Button_pack:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_pack:AddChild( Button_pack )
end

--组件设置
function Modules.DivinationCollectView:__ShowUI( )
   if unpack( self.__open_data ) then
        if unpack( self.__open_data ) == 2 then
            self.cutIndex = 1
        else
            self.cutIndex = 2
        end
    else
        self.cutIndex = 1
    end

    --收藏值
    local divination_collectValue = Util:Name( CPPGameLib.GetString("divination_collectValue") ..  CPPGameLib.GetString("common_colon") )
    divination_collectValue:SetAnchorPoint( 0 , 0.5 )
    divination_collectValue:SetPosition( 10 , self.Image_ui_1:GetContentHeight()/2 + 15 )
    self.Image_ui_1:AddChild( divination_collectValue )

    --历史最高
    self.history = Util:Name( "0" .. "  (".. CPPGameLib.GetString("tower_history") ..") " )
    self.Image_ui_1:AddChild( self.history )
    PosTool.RightTo( divination_collectValue , self.history )

    --当前
    self.now = Util:Name( "0" .. "  (".. CPPGameLib.GetString("divination_collectNowValue") ..") " )
    self.Image_ui_1:AddChild( self.now )
    PosTool.RightTo( divination_collectValue , self.now , 0 , -30  )

    --收藏卡牌所提供的属性
    self.divination_collectPro = Util:Name(" ")
    self.divination_collectPro:SetAnchorPoint( 0 , 0.5 )
    self.divination_collectPro:SetPosition( 10 , self.Image_ui_2:GetContentHeight() - 25 )
    self.Image_ui_2:AddChild( self.divination_collectPro )

    self.divination_collectProTips = Util:Label( "" , Macros.TypefaceSize.slightly , nil , self.Image_ui_2:GetContentWidth() - 20 )
    self.divination_collectProTips:SetAnchorPoint( 0.5 , 0.5 )
    self.divination_collectProTips:SetPosition( self.Image_ui_2:GetContentWidth()/2 , 20 )
    self.Image_ui_2:AddChild( self.divination_collectProTips )

    if self.__collect_scrollview then
        return 
    end

    local params = 
    {
        item_class = Modules.TemplDivinationCollectItem,
        item_width = 320,
        item_height = 360.00,
        row = 1,
        col = 3,
        horizon = true,
        view_width = self.Panel_lst:GetContentWidth(), 
        selectable = true,
        select_callback = function(item_index, item_data)
            local item = self.__collect_scrollview:GetItem(item_index)
            if not item or item_data.itemType == "chengjiu" then
                return
            end
            local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
            if max >= item_data.collection_value then
                self.__tab_index = item_index
                CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_BOOK , item_data.id )
            else
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_notCollectorsValue"))
            end
        end,
    }
    self.__collect_scrollview = GUI.SpecialScrollView.New(params)    
    self.__collect_scrollview:SetScissorSize(nil, 640)
    self.Panel_lst:AddChild(self.__collect_scrollview:GetNode())
    PosTool.Center(self.__collect_scrollview)  

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION_COLLECT, "sp_divination_toAchievement")
    self.cut = Util:Button( sp , function ( ... )
        self:OnButtonCut(  )
    end ,"",Macros.TypefaceSize.button,Macros.Color.content)
    self.cut:SetAnchorPoint(0,0)
    self.cut:SetPosition( 950 , 42 )
    self.cut:IgnoreContentAdaptWithSize(true)
    self:AddChild(self.cut) 
    --切换提示文字名字
    self.__switch_txt = Util:Label(" ",Macros.TypefaceSize.largish,Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
    self.__switch_txt:SetAnchorPoint( 0.5 , 0.5 )
    self.__switch_txt:SetPosition(self.cut:GetPositionX() + self.cut:GetContentWidth()/2, self.cut:GetPositionY() - 10 )
    self:AddChild( self.__switch_txt )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_divination_price" )
    self.bag_numBg = Util:Sprite( sp )
    self.bag_numBg:SetAnchorPoint( 0.5 , 0.5 )
    self:AddChild( self.bag_numBg , 1 )
    PosTool.Center(self.bag_numBg , 280 , -290 )

    self.bag_num = Util:Label( "00/00" )
    self.bag_num:SetAnchorPoint( 0.5 , 0.5 )
    self.bag_numBg:AddChild( self.bag_num , 1 )
    PosTool.Center( self.bag_num )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite( icon_red_point )
    self.icon_red_point:SetAnchorPoint( 1 , 1 )
    self.icon_red_point:SetPosition( self.cut:GetContentWidth() , self.cut:GetContentHeight() )
    self.cut:AddChild( self.icon_red_point )
    self.icon_red_point:SetVisible(false)

    self:OnButtonCut()
end

--切换按钮
function Modules.DivinationCollectView:__SetShowUI( )
    --历史
    local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
    self.history:SetString( max .. "  (".. CPPGameLib.GetString("tower_history") ..") " )
    --现在
    local now = CallModuleFunc(ModuleType.DIVINATION , "GetNowCollectionValue" )
    self.now:SetString( now .. "  (".. CPPGameLib.GetString("divination_collectNowValue") ..") " )

    local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
    local cnf = CallModuleFunc(ModuleType.DIVINATION , "GetAchievementCnf" )
    self.bag_num:SetString( level .. "/" .. #cnf )
end

--帮助
function Modules.DivinationCollectView:OnButtonHelp( )
    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
    help:SetLoadedCallback( function ( ... )
       help:SetCell("Rule_Divination_Book")
    end )
end

--排行
function Modules.DivinationCollectView:OnButtonRank( )
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_RANK )     
end

--背包
function Modules.DivinationCollectView:OnButtonPack( )
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_PACK )
end

--切换按钮
function Modules.DivinationCollectView:OnButtonCut( )
    self.__collect_scrollview:SetIsNotUnconditionalSelected(self.cutIndex ~= 1)
    self.bag_numBg:SetVisible(false)

    --更新红点
    self:__UpdateRed()

    if self.cutIndex == 1 then
        self:CollectShowUI()
        self.cutIndex = 2
    else
        self.bag_numBg:SetVisible(true)
        self:AchievementShowUI()
        self.cutIndex = 1
    end 

    local sp = Resource.LoadTool.GetImageData(
        Resource.UITextureAtlasType.DIVINATION_COLLECT, self.cutIndex == 2 and "sp_divination_toAchievement" or "sp_divination_toCollect")
    self.cut:SetNormalImage(sp)
    --文字
    self.__switch_txt:SetString(CPPGameLib.GetString(self.cutIndex == 2 and "divination_collect_watch_achievement" or "divination_collect_watch_collect"))
end

------------------------------------------------------------------------------------------------------------------
--收藏室
------------------------------------------------------------------------------------------------------------------

--收藏室UI
function Modules.DivinationCollectView:CollectShowUI( )
    
    local cnf = CallModuleFunc(ModuleType.DIVINATION , "GetCollectionBookCnf" )
    self.__collect_scrollview:SetDataList( cnf )    

    self.__collect_scrollview:SetChangeData( 1 , 0.1 , 0.4)
    self.__collect_scrollview:SetNecessaryData(#cnf,3)
    self.__collect_scrollview:MoveItemToScreenCenterByIndex( self.__open_data[2] or 1)

    self.__collect_scrollview:SelectItem( self.__open_data[2] or  1)

    --设置收藏室属性
    self:__SetCollectShowUI()
end

--设置收藏室属性
function Modules.DivinationCollectView:__SetCollectShowUI( )
    self.divination_collectPro:SetString( CPPGameLib.GetString("divination_collectPro") )
    self.divination_collectProTips:SetString( CPPGameLib.GetString("divination_collectProTips") )

    --收藏卡牌所提供的属性
    if self.proLst then
        for i = 1 , #self.proLst.type do
            self.proLst.type[i]:RemoveSelf( true )
            self.proLst.type[i] = nil
            self.proLst.value[i]:RemoveSelf( true )
            self.proLst.value[i] = nil
        end
    end
    self.proLst = {}
    self.proLst.type = {}
    self.proLst.value = {}

    local proLst = CallModuleFunc(ModuleType.DIVINATION , "GetNowCollectProLst" )
    local index = 1
    for type_ , v in pairs( proLst ) do

        local type_ , value_ = Util:Nature( type_ , v , true )
        
        self.proLst.type[index] = Util:Name( type_ or "" )
        self.proLst.type[index]:SetAnchorPoint( 0 , 0.5 )
        self.proLst.type[index]:SetPosition( 10 , self.Image_ui_2:GetContentHeight() - 25 - ( (index) * 40 ) )
        self.Image_ui_2:AddChild( self.proLst.type[index] )

        self.proLst.value[index] = Util:Name( value_ or "" )
        self.proLst.value[index]:SetAnchorPoint( 1 , 0.5 )
        self.proLst.value[index]:SetPosition( self.Image_ui_2:GetContentWidth() - 10  , self.Image_ui_2:GetContentHeight() - 25 - ( (index) * 40 ) )
        self.Image_ui_2:AddChild( self.proLst.value[index] )

        index = index + 1
    end

    --开锁
    local value = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )

    local bookCnf = CallModuleFunc(ModuleType.DIVINATION , "GetCollectionBookCnf" )
    
    local index = 1
    for i , v in ipairs( bookCnf ) do
        if value >= v.collection_value then
            index = index + 1
        end
        return index
    end
    self.__collect_scrollview:SetNecessaryData( index )
end

------------------------------------------------------------------------------------------------------------------
--成就室
------------------------------------------------------------------------------------------------------------------

--成就室UI
function Modules.DivinationCollectView:AchievementShowUI( )
    local cnf = CallModuleFunc(ModuleType.DIVINATION , "GetAchievementCnf" )
    self.__collect_scrollview:SetDataList( cnf )    
    self.__collect_scrollview:SetChangeData( 1 , 0.1 , 0.4)

    local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
    self.__collect_scrollview:SetNecessaryData( level + 3 , 3 )
    self.__collect_scrollview:MoveItemToScreenCenterByIndex(level > 0 and level or 1)
    self.__collect_scrollview:SelectItem(level > 0 and level or 1)

    self:__SetAchievementShowUI()
end

--设置成就室属性
function Modules.DivinationCollectView:__SetAchievementShowUI(achievement_level)
    local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
    self.__collect_scrollview:SetNecessaryData( level + 3 , 3 )
    
    self.divination_collectPro:SetString( CPPGameLib.GetString("divination_achievementPro") )
    self.divination_collectProTips:SetString( CPPGameLib.GetString("divination_achievementProTips") )

    --收藏卡牌所提供的属性
    if self.proLst then
        for i = 1 , #self.proLst.type do
            self.proLst.type[i]:RemoveSelf( true )
            self.proLst.type[i] = nil
            self.proLst.value[i]:RemoveSelf( true )
            self.proLst.value[i] = nil
        end
    end

    self.proLst = {}
    self.proLst.type = {}
    self.proLst.value = {}

    local proLst = CallModuleFunc(ModuleType.DIVINATION , "GetAchievementPro" )
    local index = 1
    for type_ , v in pairs( proLst ) do
        local type_ , value_ = Util:Nature( type_ , v , true )
        
        self.proLst.type[index] = Util:Name( type_ or "" )
        self.proLst.type[index]:SetAnchorPoint( 0 , 0.5 )
        self.proLst.type[index]:SetPosition( 10 , self.Image_ui_2:GetContentHeight() - 25 - ( (index) * 40 ) )
        self.Image_ui_2:AddChild( self.proLst.type[index] )

        self.proLst.value[index] = Util:Name( value_ or "" )
        self.proLst.value[index]:SetAnchorPoint( 1 , 0.5 )
        self.proLst.value[index]:SetPosition( self.Image_ui_2:GetContentWidth() - 10  , self.Image_ui_2:GetContentHeight() - 25 - ( (index) * 40 ) )
        self.Image_ui_2:AddChild( self.proLst.value[index] )

        index = index + 1
    end

    if achievement_level then
        --刷新最新条目
        local item = self.__collect_scrollview:GetItem(achievement_level)
        if item then
            item:SetAchievement(item:GetData())
        end
    else
        --刷新所有条目
        self.__collect_scrollview:ItemIter( function( index , item , data )
            item:SetAchievement(data)
        end )
    end
end

--获取打开参数
function Modules.DivinationCollectView:GetOpenParams()
    if self.cutIndex then
        return {self.cutIndex , self.__tab_index }
    end
end

--激活卡牌表现
function Modules.DivinationCollectView:DivinationCollectAchievementActivated(data,index)
    local item = self.__collect_scrollview:GetItem(index) 
    if not item then
        return
    end
    local count_time = 0.2
    self.__cur_sp_index = index
    if not self.__activated_shield_layout then
        self.__activated_shield_layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function ()
            if self.__touch_screen_continute then
                --移动并旋转
                CPPActionManager.RotateTo(self.__templ_divinationcollect_item:GetNode(),count_time,-30)
                CPPActionManager.MoveTo(self.__templ_divinationcollect_item:GetNode(),count_time,self.__target_posx,self.__target_posy,GlobalCallbackMgr:AddCallBackFunc(function ()
                    self.__activated_shield_layout:SetVisible(false)
                    self.__touch_screen_continute = false
                    local item = self.__collect_scrollview:GetItem(self.__cur_sp_index) 
                    if item then
                        item:SetInActivatedEffect(false)
                        item:SetVisible(true)
                    end
                    if self.__templ_divinationcollect_item then
                        self.__templ_divinationcollect_item:DeleteMe()
                        self.__templ_divinationcollect_item = nil
                    end                   
                    local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
                    self.__collect_scrollview:SelectItem(level or 1)
                    -- self.__collect_scrollview:MoveItemToScreenCenterByIndex((level+1) or 1)                   
                    --延迟转换
                    CPPActionManager.DelayTo(self.__activated_shield_layout,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
                        self.__collect_scrollview:SelectItem((level+1) or 1)
                    end))
                end))                               
            end
        end)
        self:GetNode():AddChild(self.__activated_shield_layout,Macros.Global.z_order_top + 2)
        Util:SetLayoutBGColor(self.__activated_shield_layout, Macros.Color.lilyBlack, 204)
        --点击继续
        self.__continute_txt = Util:Label(CPPGameLib.GetString("plunderedtreasures_TouchScreenTip"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
        self.__activated_shield_layout:AddChild(self.__continute_txt)
        PosTool.CenterBottom(self.__continute_txt,0,80)
        self.__continute_txt:SetVisible(false)
    end
    self.__activated_shield_layout:SetVisible(true)
    self.__templ_divinationcollect_item = Modules.TemplDivinationCollectItem.New()
    self.__templ_divinationcollect_item:SetAnchorPoint(0.5,0.5)
    self.__activated_shield_layout:AddChild(self.__templ_divinationcollect_item:GetNode(),10)
    self.__templ_divinationcollect_item:GetNode():SetRotation(-30)
    self.__templ_divinationcollect_item:SetData(data)

    local posx = item:GetNode():GetContentWidth()/2
    local posy = item:GetNode():GetContentHeight()/2 + 30
    --转换坐标 
    local before_pos = NodeUnity.ConvertPosBetweenTwoContainer(item:GetNode(), self.__activated_shield_layout, COCOPoint(posx,posy))
    self.__target_posx = before_pos.x
    self.__target_posy = before_pos.y
    self.__templ_divinationcollect_item:SetPosition(before_pos.x,before_pos.y)
    --移动并旋转
    CPPActionManager.MoveTo(self.__templ_divinationcollect_item:GetNode(),count_time,Macros.Global.DesignSize.width/2,Macros.Global.DesignSize.height/2,GlobalCallbackMgr:AddCallBackFunc(function ()
        self.__templ_divinationcollect_item.__shield_sp:SetGray(false)
        self.__templ_divinationcollect_item.icon_frame:SetGray(false)

        self.__templ_divinationcollect_item.name:SetColor(unpack( Macros.Color.Special ))
        self.__templ_divinationcollect_item.name:SetStrokeColor(unpack( Macros.Color.content_stroke ))

        self.__templ_divinationcollect_item.pro:SetColor(unpack( Macros.Color.green ))
        self.__templ_divinationcollect_item.pro:SetStrokeColor(unpack( Macros.Color.green_stroke ))
        --特效
        local loop_effect_id,effect = self:__CreateEffect({res_name = "UI_zhanbuchengjiu",loop = true,scale = 1/achievement_item_scale},self.__templ_divinationcollect_item.icon_frame,15)
        effect:SetRotation(30)
        PosTool.Center(effect)
        --请求激活
        CallModuleFunc(ModuleType.DIVINATION , "ZhanBuAchievementAcivateReq")
    end))
    CPPActionManager.RotateTo(self.__templ_divinationcollect_item:GetNode(),count_time,0) 
end

--激活卡牌后的表现
function Modules.DivinationCollectView:AfterActivatedTheAchievement()
    local effect_id,effect = self:__CreateEffect({res_name = "UI_yijianqianghuasaoguang",loop = false},self.__templ_divinationcollect_item:GetNode(),100)
    effect:SetPosition(self.__templ_divinationcollect_item.__shield_sp:GetContentWidth()/2 + 20 , 60 )
    --激活成功
    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_activateSucceed"))
    --延迟出现点击继续
    CPPActionManager.DelayTo(self.__templ_divinationcollect_item:GetNode(),0.5,GlobalCallbackMgr:AddCallBackFunc(function ()
        self.__continute_txt:SetVisible(true)
        self.__touch_screen_continute = true
    end))
end

--更新红点
function Modules.DivinationCollectView:__UpdateRed()
    local redNum = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.DIVINATION, 
        self.cutIndex == 1 and Modules.RedMgr.SystemID.DIVINATION_BOOK or Modules.RedMgr.SystemID.DIVINATION_ACHIEVEMENT)

    if redNum > 0 then
        self.icon_red_point:SetVisible(true)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--[[
成就条目
--]]

Modules = Modules or {}

--等级礼包组件
Modules.TemplDivinationCollectItem = Modules.TemplDivinationCollectItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDivinationCollectItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplDivinationCollectItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(320.00 , 360.00)
    self.layout:SetAnchorPoint(0,0)
    return self.layout
end

function Modules.TemplDivinationCollectItem:__delete()
end

function Modules.TemplDivinationCollectItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    if data.itemType == "chengjiu" then
        self:__SetAchievement(data)
    else
        self:__SetCollect(data)
    end
end

function Modules.TemplDivinationCollectItem:SetSelected(var)
    self.__is_selected = var

    if var then
        self:UpdateAchievementStatus()

    elseif self.__sp_achievement_tips_bg then
        self.__sp_achievement_tips_bg:SetVisible(false)
    end
end

function Modules.TemplDivinationCollectItem:IsTouchEnable( data )
    if not GUI.ScrollItem.IsTouchEnable(self) then
        return false
    end
    
    local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )

    if self.__data.itemType == "chengjiu" then
        if self.__data.id > (level + 3) then
            return false
        else
            return true
        end
    else
        local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
        if max >= self.__data.collection_value then
            return true
        else
            return false
        end
    end
end


------------------------------------------------
--收藏
------------------------------------------------
function Modules.TemplDivinationCollectItem:__SetCollect(data)
    --遮罩容器
    self.__shield_sp = Util:Sprite( Resource.PathTool.GetUIResPath("divinationbig/sp_divination_collectLevel_"..data.id) )
    self.__shield_sp:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild(self.__shield_sp,1)
    PosTool.Center(self.__shield_sp)

    local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
    if max >= data.collection_value then
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION_COLLECT, "SP_divinationcollect_name_" .. data.id , true )
        self.name = Util:Sprite( sp )
        self.name:SetAnchorPoint( 0.5 , 0.5 )
        self.__shield_sp:AddChild( self.name , 1 )
        PosTool.Center(self.name , -30 , -90 )
        CPPActionManager.RotateTo( self.name , 0 , 30 )

        local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
        self.icon_red_point = Util:Sprite( icon_red_point )
        self.icon_red_point:SetAnchorPoint( 0.5 , 0.5 )
        self.icon_red_point:SetPosition( self.name:GetContentWidth() , self.name:GetContentHeight() )
        self.name:AddChild( self.icon_red_point )
        self.icon_red_point:SetVisible(false)

        for ii , vv in ipairs( data.collection_group_list ) do
            local type_ = CallModuleFunc(ModuleType.DIVINATION , "ToGroupIdGetUpStarType" , vv )
            if type_ then
                self.icon_red_point:SetVisible(true)
                break
            end
        end
        
    else

        self.name = Util:Label( CPPGameLib.GetString("divination_needValue" , data.collection_value ) , Macros.TypefaceSize.popup )
        self.name:SetAnchorPoint( 0.5 , 0.5 )
        self.__shield_sp:AddChild( self.name , 1 )
        PosTool.Center(self.name , -30 , -90 )
        -- self.name:SetScaleFactor( 1.4 )
        CPPActionManager.RotateTo( self.name , 0 , 30 )

        self.name:SetColor(unpack( Macros.Color.forbidden ))
        self.name:SetStrokeColor(unpack( Macros.Color.forbidden_stroke ))
        self.name:SetStrokeWidth(2)

        self.__shield_sp:SetGray(true)
    end

end





























------------------------------------------------
--成就
------------------------------------------------
function Modules.TemplDivinationCollectItem:__SetAchievement(data)
    --遮罩容器
    local path = Resource.PathTool.GetBigIconPath( data.icon )
    self.__shield_sp = Util:Sprite(path, function()
        self:OnAchievementActivate()
    end, false)
    self.__shield_sp:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild(self.__shield_sp,1)
    PosTool.Center(self.__shield_sp)
    self.__shield_sp:SetScaleFactor( achievement_item_scale )

    local imgdata = Resource.PathTool.GetUIResPath("divinationbig/icon_sp_achievement")
    -- Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_achievement" )
    
    --图标框
    self.icon_frame = Util:Sprite( imgdata )
    self.__shield_sp:AddChild(self.icon_frame)
    PosTool.Center( self.icon_frame )

    --名字
    self.name = Util:Label( data.name , Macros.TypefaceSize.popup , Macros.Color.Special )
    self.name:SetAnchorPoint( 0.5 , 1 )
    self.name:SetPosition( self.__shield_sp:GetContentWidth()/2 , self.__shield_sp:GetContentHeight() - 20  )
    self.__shield_sp:AddChild( self.name , 999 )
    self.name:SetScaleFactor( 1.25 )

    self.name:SetStrokeColor(unpack( Macros.Color.content_stroke ))
    self.name:SetStrokeWidth(2)

    -- local pro = Util:Nature( data.pro_type_list[1] , data.pro_value_list[1] )

    --属性
    self.pro = Util:Name( data.desc , Macros.TypefaceSize.normal , Macros.Game.QualityType.GREEN )
    self.pro:SetAnchorPoint( 0.5 , 1 )
    self.pro:SetPosition( self.__shield_sp:GetContentWidth()/2 , 50 )
    self.__shield_sp:AddChild( self.pro , 999 )
    self.pro:SetScaleFactor( 1.25 )

    --激活状态背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION_COLLECT, "sp_divination_bubble")
    self.__sp_achievement_tips_bg = Util:Sprite(sp)
    self.__sp_achievement_tips_bg:SetScaleFactor(1 / achievement_item_scale)
    self.__sp_achievement_tips_bg:SetVisible(false)
    self.__sp_achievement_tips_bg:SetRotation(30)
    self.__shield_sp:AddChild(self.__sp_achievement_tips_bg, Macros.Global.z_order_top)  
    PosTool.RightTop(self.__sp_achievement_tips_bg, 33, 15)

    --激活条件文本
    self.__txt_achievement_activate_condition = Util:Label(
        CPPGameLib.GetString("divination_collect_achievement_activated_condition"), 
        Macros.TypefaceSize.normal ,
        Macros.Color.content , nil, nil)
    self.__txt_achievement_activate_condition:SetAnchorPoint(0.5, 1)
    self.__sp_achievement_tips_bg:AddChild(self.__txt_achievement_activate_condition)
    PosTool.CenterTop(self.__txt_achievement_activate_condition, -10, -8)

    self.__txt_achievement_activate_condition2 = Util:Label(
        CPPGameLib.GetString("divination_collectValue")..self.__data.need_collection_value,
        Macros.TypefaceSize.normal, 
        Macros.Color.red, nil, nil)
    self.__txt_achievement_activate_condition2:SetAnchorPoint(0, 1)
    self.__txt_achievement_activate_condition2:SetPosition(0, -3)
    self.__txt_achievement_activate_condition:AddChild(self.__txt_achievement_activate_condition2)

    --点击激活文本
    self.__sp_achievement_to_activated = Util:Sprite( 
        Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION_COLLECT, "sp_can_activated_tips", true))
    self.__sp_achievement_tips_bg:AddChild(self.__sp_achievement_to_activated)
    PosTool.Center(self.__sp_achievement_to_activated, 0, 10)

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite( icon_red_point )
    self.icon_red_point:SetAnchorPoint( 1 , 1 )
    self.icon_red_point:SetPosition( self.icon_frame:GetContentWidth() , self.icon_frame:GetContentHeight() )
    self.icon_frame:AddChild( self.icon_red_point )
    self.icon_red_point:SetScaleFactor( 1.2 )
    self.icon_red_point:SetVisible(false)

    self:SetAchievement( data )
    self:UpdateAchievementStatus(true)
end

function Modules.TemplDivinationCollectItem:SetAchievement(data)
    local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
    if level >= data.level then
        self.__shield_sp:SetGray(false)
        self.icon_frame:SetGray(false)

        self.name:SetColor(unpack( Macros.Color.Special ))
        self.name:SetStrokeColor(unpack( Macros.Color.content_stroke ))

        self.pro:SetColor(unpack( Macros.Color.green ))
        self.pro:SetStrokeColor(unpack( Macros.Color.green_stroke ))
    else
        self.__shield_sp:SetGray(true)
        self.icon_frame:SetGray(true)

        self.name:SetColor(unpack( Macros.Color.forbidden ))
        self.name:SetStrokeColor(unpack( Macros.Color.forbidden_stroke ))

        self.pro:SetColor(unpack( Macros.Color.forbidden ))
        self.pro:SetStrokeColor(unpack( Macros.Color.forbidden_stroke ))
    end
end

--获取位置容器
function Modules.TemplDivinationCollectItem:GetPanelPos()
    return self.__shield_sp
end

--偏移容器
function Modules.TemplDivinationCollectItem:GetExcursionNode()
    return self.__shield_sp
end

--偏移量
function Modules.TemplDivinationCollectItem:GetExcursionPos()
    return 100
end

--最低位置偏移量
function Modules.TemplDivinationCollectItem:GetMinPos()
    return -70
end

--更新成就条目状态
function Modules.TemplDivinationCollectItem:UpdateAchievementStatus(ignore_tips_bg)
    if not self.__sp_achievement_tips_bg then
        return
    end

    self.__data.can_activated = nil
    self.__data.has_activated = nil    
    self.__data.cross_activated = nil
    self.__sp_achievement_tips_bg:SetVisible(ignore_tips_bg ~= true and true or false)
    self.__sp_achievement_to_activated:SetVisible(false)
    self.__txt_achievement_activate_condition:SetVisible(false)

    self.icon_red_point:SetVisible(false)

    local info = CallModuleFunc(ModuleType.DIVINATION , "GetAchievementCnf" , self.__data.level )
    if info.id then    
        local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
        if level >= info.level then
            --已激活
            self.__data.has_activated = true
            self.__sp_achievement_tips_bg:SetVisible(false)
            self:CreateAchievementActivedEffect()
        else
            --未激活
            local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
            if max >= info.need_collection_value then
                if info.level - level == 1 then
                    --可激活
                    self.__data.can_activated = true
                    self.__sp_achievement_to_activated:SetVisible(true)

                    self.icon_red_point:SetVisible(true)
                else
                    --跨等级
                    self.__data.cross_activated = true
                    self.__txt_achievement_activate_condition:SetVisible(true)
                end
            else
                --不可激活
                self.__txt_achievement_activate_condition:SetVisible(true)
            end
        end
    end
end

--激活成就
function Modules.TemplDivinationCollectItem:OnAchievementActivate()
    if not self.__is_selected then
        return
    end

    if self.__data.has_activated then
        return
    end

    if self.__data.cross_activated then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_crossActivateTips"))
        return
    end

    if not self.__data.can_activated then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_notCollectorsValue"))
        return
    end

    self:SetVisible(false)
    self:SetInActivatedEffect(true)

    --激活表现
    GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_COLLECT_ACHIEVEMENT_ACTIVATED,self.__data,self:GetIndex())
end

--创建成就已激活特效
function Modules.TemplDivinationCollectItem:CreateAchievementActivedEffect()
    if not self.__loop_effect_id then
        local effect
        self.__loop_effect_id, effect = self:__CreateEffect({res_name = "UI_zhanbuchengjiu",loop = true,scale = 1/achievement_item_scale},self.icon_frame,15)
        effect:SetRotation(30)
        PosTool.Center(effect)
    end    
end

--设置是否激活特效中
function Modules.TemplDivinationCollectItem:SetInActivatedEffect(var)
    self.__in_activated_effect = var
end

function Modules.TemplDivinationCollectItem:SetVisible(var)
    if self.__in_activated_effect then
        return
    end
    
    GUI.Template.SetVisible(self, var)
end
