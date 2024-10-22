--
-- @brief 七天活动 
-- @author: yjg
-- @date: 2017年1月17日10:53:55
--

Modules = Modules or {}

Modules.SevenDaysView = Modules.SevenDaysView or BaseClass(BaseView)

function Modules.SevenDaysView:__init()
  	self.__layout_name = "activity_sevenDays"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_sevendays")}
    
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("activity_seven") ,
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

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_SEVEN_DAYS)    
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SEVEN_DAYS)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SEVEN_DAYS,true)
end

function Modules.SevenDaysView:__Dispose()   
    if self.sevenDays then
        self.sevenDays:DeleteMe()
        self.sevenDays = nil
    end

    local max , now , t1 = CallModuleFunc(ModuleType.SEVENDAYS, "GetProgressData")
    for i,v in ipairs(t1) do
        if self.effectsLst[v.target_num]then
            self:__DelEffect(self.effectsLst[v.target_num].id)
            self.effectsLst[v.target_num].id = nil
            self.effectsLst[v.target_num].con = nil
        end

        if self.effectsLst_[v.target_num]then
            self:__DelEffect(self.effectsLst_[v.target_num].id)
            self.effectsLst_[v.target_num].id = nil
            self.effectsLst_[v.target_num].con = nil
        end

    end
end

function Modules.SevenDaysView:__RegistAllEvents()    
    --签到事件
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY,function (_, data)
        self:__SetShowUI( data )
    end)

    --礼包事件
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_GIFT_BAG,function (_, data)
        self:__SetGiftBag( )
    end)

    --打折事件
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY_CHOOSE_BUY,function (_, data)
        self:OnButtonSky(  )
    end)

    --红点更新事件
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
        if system_id == Macros.Game.SystemID.SEVEN_DEYS then
            self:UpRed()
            self.sevenDays:SetRed()
        end
    end)
end

function Modules.SevenDaysView:__OpenCallback()
    self.icon_red_point = {}
    self.effectsLst = {}
    self.effectsLst_ = {}  

    --设置当前类型
    CallModuleFunc(ModuleType.SEVENDAYS, "SetCurType" , unpack(self.__open_data) )

    self.sevenLst = {}
    if unpack(self.__open_data) == Macros.Game.SEVEN_TYPE.EIGHT then
        self.sevenLst = CallModuleFunc(ModuleType.SEVENDAYS, "GetEighiDayCnf")
    elseif unpack(self.__open_data) == Macros.Game.SEVEN_TYPE.SEVEN then
        self.sevenLst = CallModuleFunc(ModuleType.SEVENDAYS, "GetSevenDayCnf")
    end

    self:__Getwidget()
    self:__ShowUI() 
    self:CountTime()
    self:UpRed()
    self.sevenDays:SetRed()

    CallModuleFunc(ModuleType.SEVENDAYS, "ShopBuyRecordRequest")
    CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayTargetListReq")    
end

function Modules.SevenDaysView:__Getwidget()
    --居中层
    self.Panel_interlayer = self:GetWidget("Panel_interlayer") 

    --下框
    self.Panel_interlayer = self:GetWidget("Panel_interlayer")

    --页签
    self.Image_selectLst = self:GetWidget("Image_selectLst")
    self.Image_selectLst :SetZOrder(20)

    --进度条
    self.Image_barBg = self:GetWidget("Image_barBg")
    self.Image_barBg:SetVisible(false)

    --结束时间
    self.Text_overTime = self:GetWidget("Text_overTime")
    
    --截止时间
    self.Text_abortTime = self:GetWidget("Text_abortTime")

    --子页面
    self.Panel_subpage = self:GetWidget("Panel_subpage")

    -- --背景
    -- self.Image_bg = self:GetWidget("Image_bg")

    --页眉容器
    self.Panel_brow_bg = self:GetWidget("Panel_brow_bg")

    --页眉按钮
    self.Button_sky = {}
    self.red_lst = {}

    local num = 0
    if unpack(self.__open_data) == Macros.Game.SEVEN_TYPE.EIGHT then
        num = 7 
    end

    for i = 1 ,  7 do
        self.Button_sky[i] = self:GetWidget("Button_sky"..i)
        self.Button_sky[i]:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "sp_day_" .. i + num , true ))

        self.Button_sky[i]:SetTouchEnabled(true)
        WidgetUnity.SetWidgetListener(self.Button_sky[i], nil, nil, function ( ... )
            self:OnButtonSky(i)
        end)

        self.red_lst[i] =  self:GetWidget("Image_red_"..i)
        self.red_lst[i]:SetZOrder(10)
        self.red_lst[i]:SetVisible(false)
    end
end


function Modules.SevenDaysView:__ShowUI()

    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")
    --进度条
    self.LoadingBar_loadingBar = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Image_barBg:GetContentWidth() - 4 , 20 , self.Image_barBg:GetContentWidth() , 24 , 0, 100)
    self.LoadingBar_loadingBar:SetAnchorPoint(0.5, 0.5)
    self.Image_barBg:AddChild(self.LoadingBar_loadingBar:GetNode())
    PosTool.Center(self.LoadingBar_loadingBar)
    self.Image_barBg:GetContentSize( self.LoadingBar_loadingBar:GetNode():GetContentWidth() , self.LoadingBar_loadingBar:GetNode():GetContentHeight() )

    local start = {}
    if unpack(self.__open_data) == Macros.Game.SEVEN_TYPE.EIGHT then
       start = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.SEVEN_DEYS_II )
    elseif unpack(self.__open_data) == Macros.Game.SEVEN_TYPE.SEVEN then
       start = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.SEVEN_DEYS )
    end

    local overTime = Util:Name( CPPGameLib.GetString("activity_overTime", "" ) , Macros.TypefaceSize.minimum , 100 )
    overTime:SetAnchorPoint(0,0.5)
    overTime:SetPosition( 0 , self.Text_overTime:GetContentHeight()/2 )
    self.Text_overTime:AddChild(overTime)

    local overTime_ = Util:Name( TimeTool.GetTimeFormat(start.over, TimeTool.defaul_format[19] ) , Macros.TypefaceSize.minimum )
    overTime_:SetAnchorPoint(0,0.5)
    self.Text_overTime:AddChild(overTime_)    
    PosTool.RightTo( overTime , overTime_ )

    local drawTime = Util:Name( CPPGameLib.GetString("activity_drawTime" , "" ) , Macros.TypefaceSize.minimum , 100 )
    drawTime:SetAnchorPoint(0,0.5)
    overTime:SetPosition( 0 , self.Text_abortTime:GetContentHeight()/2 )
    self.Text_abortTime:AddChild(drawTime)

    local drawTime_ = Util:Name( TimeTool.GetTimeFormat(start.reward, TimeTool.defaul_format[19] ) , Macros.TypefaceSize.minimum )
    drawTime_:SetAnchorPoint(0,0.5)
    self.Text_abortTime:AddChild(drawTime_)    
    PosTool.RightTo( drawTime , drawTime_ )

    local max , now , t1 = CallModuleFunc(ModuleType.SEVENDAYS, "GetProgressData")

    self.icon_path = {
        [1] = { 
                    ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "green_box_close"),
                    op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "green_box_open"),
                    effects = "UI_wood_box"
                },
        [2] = { 
                    ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_close"),
                    op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_open"),
                    effects = "UI_silverybox"
                },                
        [3] = { 
                    ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "purple_box_close"),
                    op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "purple_box_open"),
                    effects = "UI_blue_box"
                },
        [4] = { 
                    ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "orange_box_close"),
                    op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "orange_box_open"),
                    effects = "UI_red_box"
                },
    }

    self.daily = {}
    self.daily_bg = {}
    for i,v in ipairs( t1 ) do

        self.daily[v.target_num] = Util:Sprite( self.icon_path[i].ed or self.icon_path[1].ed )
        self.daily[v.target_num]:SetPosition( self.Image_barBg:GetContentWidth()/(max / v.target_num ) , self.Image_barBg:GetContentHeight() + 20 )
        self.Image_barBg:AddChild(self.daily[v.target_num])

        self.daily_bg[v.target_num] = Util:Layout( self.daily[v.target_num]:GetContentWidth() , self.daily[v.target_num]:GetContentHeight() , function ( ... )
            self:OnDaily( v )
        end )
        self.daily_bg[v.target_num]:SetAnchorPoint(0.5,0.5)
        self.daily_bg[v.target_num]:SetPosition( self.Image_barBg:GetContentWidth()/(max / v.target_num ) , self.Image_barBg:GetContentHeight() + 20 )
        self.Image_barBg:AddChild(self.daily_bg[v.target_num])

        local target_num = Util:Name(v.target_num)
        target_num:SetPosition( self.daily_bg[v.target_num]:GetContentWidth()/2 , -16 )
        self.daily_bg[v.target_num]:AddChild(target_num)
        
    end

    self.activity_PlanTips = Util:RichText( CPPGameLib.GetString("activity_PlanTips",0) )
    self.activity_PlanTips:SetAnchorPoint(0.5 , 1)
    self.Image_barBg:AddChild(self.activity_PlanTips)
    PosTool.Center(self.activity_PlanTips , 0 , 0 - (self.activity_PlanTips:GetContentHeight() ) - 3  )

    -- 子页面
    self.sevenDays = LogicGUI.TemplActivitySevenDays.New( self.Image_selectLst  , self.Panel_subpage)
end

function Modules.SevenDaysView:OnButtonSky( index )
    index = index or self.index
    self.index = index

    local bg_sp9_selected = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "sp_day_"..index)
    if self.selected then
        self.selected:RemoveSelf(false)
        self.selected = nil
    end

    self.selected = Util:Sprite9( bg_sp9_selected )
    self.Button_sky[index]:AddChild( self.selected )
    PosTool.Center( self.selected )

    self.sevenDays:__SetShowUI(self.sevenLst[index] , index )
    self.sevenDays:SetRed()
end

function Modules.SevenDaysView:__SetShowUI()
    self:OnButtonSky(1)

    self:__SetGiftBag()
end

--礼包
function Modules.SevenDaysView:__SetGiftBag()
    local max , now , t1 = CallModuleFunc(ModuleType.SEVENDAYS, "GetProgressData")
    self.LoadingBar_loadingBar:SetProgress( now/max * 100 )
    for i,v in ipairs( t1 ) do

        local type_ = CallModuleFunc(ModuleType.SEVENDAYS, "GetBoxType" , v.target_num )
        if type_ then

            self.daily[v.target_num]:SetImage( self.icon_path[i].op )
            self.daily[v.target_num]:SetPosition( self.daily[v.target_num]:GetPositionX() , 52.5 )

            if self.effectsLst[v.target_num] then
                self:__DelEffect(self.effectsLst[v.target_num].id)
                self.effectsLst[v.target_num].id = nil
                self.effectsLst[v.target_num].con = nil
            end

            if self.effectsLst_[v.target_num] then
                self:__DelEffect(self.effectsLst_[v.target_num].id)
                self.effectsLst_[v.target_num].id = nil
                self.effectsLst_[v.target_num].con = nil
            end

            self.daily[v.target_num]:SetVisible(true)

        elseif v.target_num <= now then

            if self.effectsLst[v.target_num] then
                self:__DelEffect(self.effectsLst[v.target_num].id)
                self.effectsLst[v.target_num].id = nil
                self.effectsLst[v.target_num].con = nil
            end

            if self.effectsLst_[v.target_num] then
                self:__DelEffect(self.effectsLst_[v.target_num].id)
                self.effectsLst_[v.target_num].id = nil
                self.effectsLst_[v.target_num].con = nil
            end


            self.effectsLst_[v.target_num] = {}
            self.effectsLst_[v.target_num].id , self.effectsLst_[v.target_num].con = self:__CreateEffect({res_name = "UI_xingjibaoxiang" }, self.daily_bg[v.target_num] )
            self.effectsLst_[v.target_num].con:SetAnchorPoint(0.5 , 0.5 )
            self.effectsLst_[v.target_num].con:SetPosition(  self.daily_bg[v.target_num]:GetContentWidth()/2 + 10 ,  self.daily_bg[v.target_num]:GetContentHeight()/2 )


            self.effectsLst[v.target_num] = {}
            self.effectsLst[v.target_num].id , self.effectsLst[v.target_num].con = self:__CreateEffect( {res_name = self.icon_path[i].effects , type = Effect.EffectType.SKELETON , act_index = RenderObj.ActionIndex.IDLE2 } , self.daily_bg[v.target_num] )
            self.effectsLst[v.target_num].con:SetAnchorPoint(0.5 , 0.5 )
            self.effectsLst[v.target_num].con:SetPosition(  self.daily_bg[v.target_num]:GetContentWidth()/2 + 10 ,  0 )
        
            self.daily[v.target_num]:SetVisible(false)

        end
    end

    self.activity_PlanTips:SetText( CPPGameLib.GetString("activity_PlanTips" , now .. "/" .. max ) )
end

--刷新红点
function Modules.SevenDaysView:UpRed()
    local cur_type = self:CallModuleFunc("GetCurType")

    --列表相关调整
    for i = 1, #self.Button_sky do
        local num = CallModuleFunc(ModuleType.SEVENDAYS, "GetRedDotNumByDay", i)
        if num ~= 0 then
            self.red_lst[i]:SetVisible(true)
        else
            self.red_lst[i]:SetVisible(false)
        end
    end
end

function Modules.SevenDaysView:CountTime()
    local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
    local num = 0
    if unpack(self.__open_data) == Macros.Game.SEVEN_TYPE.EIGHT then
        num = 7 
    end
    
    --列表相关调整
    for i = 1 , #self.Button_sky do
        if i + num > time  then
            WidgetUnity.SetWidgetListener( self.Button_sky[i] , nil, nil, function()
                local lst = CallModuleFunc(ModuleType.SEVENDAYS, "GetAwardPreview",i)
                LogicGUI.TemplSevenAwardPreview.New( lst )
            end )
        end
    end
end

function Modules.SevenDaysView:OnDaily( data )

    local type_ = CallModuleFunc(ModuleType.SEVENDAYS, "GetBoxType" , data.target_num )
    if type_ then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("rebel_getWin") )
    else
        LogicGUI.TemplChestContent.New( data )
    end
end



-- @brief 预览宝箱 
-- @author: yjg
-- @date: 2016年8月8日15:53:48
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplChestContent = LogicGUI.TemplChestContent or BaseClass(GUI.Template)

function LogicGUI.TemplChestContent:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self.data = data
        self:__Getwidget()
        self:__ShowUI(data)

    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplChestContent:__delete()

end

function LogicGUI.TemplChestContent:__Getwidget()
    --居中层
    self.Panel_boxInfo = self:GetWidget("Panel_boxInfo") 
    --背景
    self.Image_6 = self:GetWidget("Image_6") 
    --标题背景
    self.Image_4 = self:GetWidget("Image_4") 
    --内容
    self.Image_9 = self:GetWidget("Image_9") 
    --标题
    self.Text_Title = Util:WidgetLabel( self:GetWidget("Text_Title") , CPPGameLib.GetString("award_box_Title") , Macros.TypefaceSize.popup )
    --提示
    self.Text_GameLevelCondition = self:GetWidget("Text_GameLevelCondition") 

    self.Panel_StarCondition = self:GetWidget("Panel_StarCondition") 
    self.Panel_StarCondition:SetVisible(false)

    self.ScrollView_Rewards = self:GetWidget("ScrollView_Rewards") 
    self.ScrollView_Rewards:SetVisible(false)

    --关闭
    self.Button_Close =  Util:WidgetButton( self:GetWidget("Button_Close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
    --确定
    self.Button_Receive = Util:WidgetButton( self:GetWidget("Button_Receive") , " " , CPPGameLib.Handler(self,self.Button_Receive) )
end

function LogicGUI.TemplChestContent:__ShowUI(data)

    Util:WidgetLabel( self.Text_GameLevelCondition , CPPGameLib.GetString("activity_target" , data.target_num ) , Macros.TypefaceSize.normal , Macros.Color.content )

    local interval = self.Image_9:GetContentWidth() / 4
    for i,v in ipairs(data.reward_item_id_list) do
        local layer = LogicGUI.IconLayer.New(true)
        layer:SetData({ id = v , num = data.reward_item_num_list[i] } )
        layer:SetPosition( ( interval / 2 ) + interval * (i - 1) , self.Image_9:GetContentHeight() - ( ( self.Image_9:GetContentHeight()/2 ) / 2 ) - 20 )
        layer:SetAnchorPoint(0.5,0.5)
        layer:ShowItemName(true)
        layer:SetTouchSwallow(false)
        self.Image_9:AddChild(layer:GetNode())
    end

    local max , now , t1 = CallModuleFunc(ModuleType.SEVENDAYS, "GetProgressData")

    local type_ = CallModuleFunc(ModuleType.SEVENDAYS, "GetBoxType" , data.target_num )

    if data.target_num > now or type_ then
        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
        self.Button_Receive:SetNormalImage(imgdata)
        self.Button_Receive:SetText(CPPGameLib.GetString("common_close"))

        local r, g, b, a = unpack(Macros.Color.button_yellow)
        self.Button_Receive:SetOutline(r, g, b, a, 2)

    else
        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
        self.Button_Receive:SetNormalImage(imgdata)
        self.Button_Receive:SetText(CPPGameLib.GetString("get_Award_Text"))

        local r, g, b, a = unpack(Macros.Color.button_red)
        self.Button_Receive:SetOutline(r, g, b, a, 2)

        self.btnEffect_id , self.BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.Button_Receive )
        self.BtnEffect_con:SetPosition(self.Button_Receive:GetContentWidth()/2 , self.Button_Receive:GetContentHeight()/2 )
    end

end

function LogicGUI.TemplChestContent:OnBtnClose()
    self:DeleteMe()
end

function LogicGUI.TemplChestContent:Button_Receive()
    local max , now , t1 = CallModuleFunc(ModuleType.SEVENDAYS, "GetProgressData")
    if self.data.target_num > now then
        self:DeleteMe()
    else
        local info = {}
        info.completion_num = self.data.target_num
        CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayCompletionRewardReq",info)
    end
    self:DeleteMe()
end


-- @brief 还未开启的预览 
-- @author: yjg
-- @date: 2016年8月8日15:53:48
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplSevenAwardPreview = LogicGUI.TemplSevenAwardPreview or BaseClass(GUI.Template)

function LogicGUI.TemplSevenAwardPreview:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self.data = data
        self:__Getwidget()
        self:__ShowUI(data)
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_SEVEN_DAYS_PREVIEW_ITEM)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_SEVEN_DAYS_PREVIEW)
end

function LogicGUI.TemplSevenAwardPreview:__delete()
    if self.lst_list then 
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function LogicGUI.TemplSevenAwardPreview:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --内容
    self.Image_content = self:GetWidget("Image_content") 
    --关闭
    self.Button_Close =  Util:WidgetButton( self:GetWidget("Button_Close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
end

function LogicGUI.TemplSevenAwardPreview:__ShowUI(data)

    local activity_previewName = Util:Label( CPPGameLib.GetString("activity_previewName") , Macros.TypefaceSize.popup , Macros.Color.white )
    activity_previewName:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_light:AddChild( activity_previewName )
    PosTool.Center( activity_previewName )

    local params =  
    {
        item_class = Modules.TemplSevenAwardPreviewItem,
        item_width = self.Image_content:GetContentWidth() ,
        item_height = 135 ,
        row = 3,
        col = 1,
        item_space_r = 5,
        view_height = self.Image_content:GetContentHeight() - 5  ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
    self.lst_list:SetDataList( data )

end

function LogicGUI.TemplSevenAwardPreview:OnBtnClose()
    self:DeleteMe()
end



Modules = Modules or {}
Modules.TemplSevenAwardPreviewItem = Modules.TemplSevenAwardPreviewItem or BaseClass(GUI.ScrollItem)

function Modules.TemplSevenAwardPreviewItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_SEVEN_DAYS_PREVIEW_ITEM)   
    self:__Getwidget()
end

function Modules.TemplSevenAwardPreviewItem:__delete()
    if self.lst_list then
        self.lst_list:RemoveSelf(true)
        self.lst_list = nil
    end
end

function Modules.TemplSevenAwardPreviewItem:__Getwidget()
    --条目
    self.Image_item = self:GetWidget("Image_item") 
    --内容
    self.Image_content = self:GetWidget("Image_content") 
end

function Modules.TemplSevenAwardPreviewItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TemplSevenAwardPreviewItem:__ShowUI(data)   
    self.text = Util:Label( data.name , Macros.TypefaceSize.largish , Macros.Color.keypoint )
    self.text:SetAnchorPoint( 0 , 1 )
    self.text:SetPosition( (self.Image_item:GetContentWidth()/2) - (self.Image_content:GetContentWidth()/2) + 5 , self.Image_item:GetContentHeight() - (self.text:GetContentHeight()/2) )
    self.Image_item:AddChild( self.text )

    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetAnchorPoint(0,0)
    self.lst_list:SetContentSize( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight() )
    self.lst_list:SetDirection(UIListViewEx.ScrollDirHorizontal)
    self.lst_list:SetPosition(0 , 0 )
    self.lst_list:SetTouchSwallow(false)

    self.Image_content:AddChild(self.lst_list)

    for i,v in ipairs( data.lst ) do
        local layou = Util:Layout( self.Image_content:GetContentWidth()/4 , self.Image_content:GetContentHeight() )
        layou:SetAnchorPoint(0.5,0.5)
        -- layou:SetTouchEnabled(true)

        local icon = LogicGUI.IconLayer.New(true)
        icon:SetPosition(layou:GetContentWidth()/2 , layou:GetContentHeight()/2) 
        icon:SetData({id = v })
        icon:SetTouchSwallow(false)
        layou:AddChild(icon:GetNode())
        PosTool.Center(icon:GetNode())

        self.lst_list:PushBackCustomItem(layou)
    end

end