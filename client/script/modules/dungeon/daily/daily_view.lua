
--[[
%% @module: 日常副本界面
%% @author: swordman sue
%% @created: 2016-10-31
--]]

Modules = Modules or {}

Modules.DailyDungeonView = Modules.DailyDungeonView or BaseClass(BaseView)

function Modules.DailyDungeonView:__init()
    self.__layout_name = "dungeon_daily"    
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_daliydungeon")}
    
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("everydayectype"), 
        show_bg = false,
        lucency_bg = true,
        show_diamond = true,
        show_power = true,
        show_stamina = true,
        show_energy = true,
        res_beg_offset_x = -90,
        res_unit_bg_imageinfo = {Resource.UITextureAtlasType.CITY, "bg_resource"},
        res_unit_title_offset_x = -16,
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_DAILY_DIFFICULTLEVEL)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_DAILY_ITEM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_DAILY)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_DAILY,true)
end

function Modules.DailyDungeonView:__OpenCallback()    
    self:__ShowUI()
    CallModuleFunc(ModuleType.DUNGEON, "FBDailyDataReq")
end

function Modules.DailyDungeonView:__RegistAllEvents()
    --日常副本数据
    self:BindGlobalEvent(Macros.Event.DungeonModule.DAILY_DUNGEON_DATA_RESP, function() 
        self.__org_dungeon_status_lst = CallModuleFunc(ModuleType.DUNGEON, "GetAllDailyChapterType")
        self:__Getwidget()
    end)  
       
    --零点刷新
    self:BindGlobalEvent(Macros.Event.SundryModule.NEW_DAY_NOTIFY, function() 
        CPPActionManager.DelayTo(self.__panel_center,2,GlobalCallbackMgr:AddCallBackFunc(function ()
            self.__is_zero_refrash = true
            CallModuleFunc(ModuleType.DUNGEON, "FBDailyDataReq")
        end))        
    end)
end

function Modules.DailyDungeonView:__Dispose()
    if self.__cur_page then
        self.__cur_page:DeleteMe()
        self.__cur_page = nil
    end
    if self.__dungeon_type_scrollview then
        self.__dungeon_type_scrollview:DeleteMe()
        self.__dungeon_type_scrollview = nil
    end
end

function Modules.DailyDungeonView:__Getwidget()
    --活动类型容器
    if not self.__dungeon_type_scrollview then
        local Panel_scrollview = self:GetWidget("Panel_scrollview")
        local params = {
            item_class = Modules.TemplDailyDungeonItem,
            item_width = 160,
            item_height = 130,
            row  = 1,
            col  = 6,
            horizon = true,
            view_width = 960,
            view_height = 130,
            selectable = true,
            select_callback = function (index,data)
                self:__SelectedCallBack(index)
            end,
            select_filter_func = function (index,data)
                if self.__dungeon_info_lst[index].__status == "notLevel" then
                    return false
                elseif self.__dungeon_info_lst[index].__status == "notTime" then
                    return true                  
                else
                    return true
                end
            end,
            select_filter_func_callback = function (index,data)
                if self.__dungeon_info_lst[index].__status == "notLevel" then
                    local fb_type_info = CPPGameLib.GetConfig("DailyChapter",data.__index)
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("daliy_dungeon_openLv_tips",fb_type_info.gamelevel_limit_list[1]))                
                end
            end,
        }    
        self.__dungeon_type_scrollview = GUI.ScrollView.New(params)
        Panel_scrollview:AddChild(self.__dungeon_type_scrollview:GetNode())
        PosTool.Center(self.__dungeon_type_scrollview)
    end
    self.__dungeon_info_lst = {}
    local count_num = 0
    for index,daliy_info in ipairs(CPPGameLib.GetConfig("DailyChapter",nil,nil,true)) do
        local data = {}
        data.__index = index
        data.__status = self.__org_dungeon_status_lst[index]
        if self.__org_dungeon_status_lst[index] ~= "notLevel" and self.__org_dungeon_status_lst[index] ~= "notTime" then
            count_num = count_num + 1
            table.insert(self.__dungeon_info_lst,count_num,data)
        else
            table.insert(self.__dungeon_info_lst,data)
        end       
    end
    self.__dungeon_type_scrollview:SetDataList(self.__dungeon_info_lst)
    local open_index = unpack(self.__open_data)  
    if self.__is_zero_refrash then
        self.__is_zero_refrash = false
        open_index = self.__cur_actual_fb_type
    else
        open_index = open_index or 1
    end  
    local selected_index
    for i,data_info in ipairs(self.__dungeon_info_lst) do
        if data_info.__index == open_index then
            selected_index = i
            break
        end
    end
    self.__dungeon_type_scrollview:SelectItem(selected_index)   
end 

function Modules.DailyDungeonView:__ShowUI()
    --居中父容器
    self.__panel_center = self:GetWidget("Panel_center")
    --向后箭头
    self.__image_back = self:GetWidget("Image_back")
    WidgetUnity.SetWidgetListener(self.__image_back, nil, nil,function ()
        self.__cur_selected_page = self.__cur_selected_page - 1
        self:__HandleChangePage(false)
    end)
    --向后文字
    self.__text_backdifficultlevel = self:GetWidget("Text_backdifficultlevel")
    Util:WidgetLabel(self.__text_backdifficultlevel, "", Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.GrayBlack_stroke)
    --向前箭头
    self.__image_front = self:GetWidget("Image_front")
    WidgetUnity.SetWidgetListener(self.__image_front, nil, nil,function ()
        self.__cur_selected_page = self.__cur_selected_page + 1
        self:__HandleChangePage(true)
    end)
    --向前文字
    self.__text_frontdifficultlevel = self:GetWidget("Text_frontdifficultlevel")
    Util:WidgetLabel(self.__text_frontdifficultlevel, "", Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.GrayBlack_stroke)
    --TODO 做完灰黑给美术君看
    --今日挑战次数
    self.__text_changetimes = self:GetWidget("Text_changetimes")
    Util:WidgetLabel(self.__text_changetimes, "", Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)   
    --挑战
    self.__button_challenge = self:GetWidget("Button_challenge")
    Util:WidgetButton(self.__button_challenge,CPPGameLib.GetString("dungeon_daily_challenge"),function ()
        if self.__dungeon_info_lst[self.__cur_selected_index].__status == "has_battled" then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("everydayectype_use_out")) 
            return
        end
        --记录进度
        CallModuleFunc(ModuleType.DUNGEON, "SetDaliyDungeonDifficultRecord")
        CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelChallenge", self.__cur_actual_fb_type,self.__cur_selected_page,Macros.Game.FBType.DAILY_DUNGEON)
    end, Macros.TypefaceSize.popup, Macros.Color.btn, Macros.Color.button_red)
    --扫荡
    self.__button_clean = self:GetWidget("Button_clean")
    Util:WidgetButton(self.__button_clean,CPPGameLib.GetString("dungeon_clean_title"),function ()
        --是否通关
        if not self.__button_clean:IsBright() then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("everydayectype_pass_could_clean")) 
            return
        end
        CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelClean", self.__cur_actual_fb_type,self.__cur_selected_page,Macros.Game.FBType.DAILY_DUNGEON)
    end, Macros.TypefaceSize.popup, Macros.Color.btn, Macros.Color.button_yellow)
    --奖励框
    self.__image_bestencourageframe = self:GetWidget("Image_bestEncourageframe")
    --奖励容器
    self.__panel_encourageitem1 = self:GetWidget("Panel_encourageItem1")
    self.__panel_encourageitem2 = self:GetWidget("Panel_encourageItem2")
    --拖动区域
    self.__panel_touch = self:GetWidget("Panel_touch")
    WidgetUnity.SetWidgetListener(self.__panel_touch,function (x,y)
        self.__began_x = x
    end, nil,function (x,y)
        local fb_info = CPPGameLib.GetConfig("DailyChapter",self.__cur_actual_fb_type)        
        if x - self.__began_x >= 100 then
            self.__panel_touch:SetTouchEnabled(false)
            if self.__cur_selected_page <= 1 then                
                CPPActionManager.JumpBy(self.__cur_page:GetNode(),0.3,50,0,0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
                    CPPActionManager.JumpBy(self.__cur_page:GetNode(),0.3,-50,0,0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
                        self.__panel_touch:SetTouchEnabled(true)
                    end))
                end))
            else
                self.__cur_selected_page = self.__cur_selected_page - 1
                self:__HandleChangePage(false)
            end
        elseif x - self.__began_x <= -100 then
            self.__panel_touch:SetTouchEnabled(false)
            if self.__cur_selected_page >= #fb_info.gamelevel_list then
                CPPActionManager.JumpBy(self.__cur_page:GetNode(),0.3,-50,0,0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
                    CPPActionManager.JumpBy(self.__cur_page:GetNode(),0.3,50,0,0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
                        self.__panel_touch:SetTouchEnabled(true)
                    end))
                end))
            else
                self.__cur_selected_page = self.__cur_selected_page + 1
                self:__HandleChangePage(true)
            end
        end
    end)
    --气泡
    self.__image_bubble = self:GetWidget("Image_bubble")
    self.__image_bubble:SetZOrder(2)
    --气泡文字
    self:GetWidget("Text_bubble"):SetVisible(false)
    -- Util:WidgetLabel(self.__text_bubble, "", Macros.TypefaceSize.normal, Macros.Color.content ,164, 67)
    self.__text_bubble = Util:Label("", Macros.TypefaceSize.normal, Macros.Color.content ,164, 67)
    self.__text_bubble:SetAnchorPoint(0,1)
    self.__image_bubble:AddChild(self.__text_bubble)
    self.__text_bubble:SetPosition(9,76)
    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(4)日常副本，小图标（所有标签页下共用）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Everyday")
        end )
    end )
    Button_rule:SetZOrder(100)
end

--滚动容器回调
function Modules.DailyDungeonView:__SelectedCallBack(index)   
    self.__cur_selected_index = index
    self.__cur_actual_fb_type = self.__dungeon_info_lst[self.__cur_selected_index].__index
    --跳转到当前选中副本可以挑战到的难度
    local chapter_info = CallModuleFunc(ModuleType.DUNGEON,"GetChapterInfo",self.__cur_actual_fb_type,Macros.Game.FBType.DAILY_DUNGEON)
    local fb_info = CPPGameLib.GetConfig("DailyChapter",self.__cur_actual_fb_type)
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    self.__cur_level_limit = 0
    for page_index,limit_level in ipairs(fb_info.gamelevel_limit_list) do     
        if role_level < limit_level then
            break
        end
        self.__cur_level_limit = page_index
    end     
    --今日挑战次数
    self.__text_changetimes:SetText(CPPGameLib.GetString("everydayectype_residue",chapter_info.__remanent_battle_times,fb_info.daily_battle_times))  
    local is_change = false
    --弹首次通关弹窗
    if not self.__first_passed_dungeon then
        self.__first_passed_dungeon = true
        local open_index = unpack(self.__open_data) 
        if open_index then
            local org_record = CallModuleFunc(ModuleType.DUNGEON,"GetDaliyDungeonDifficultRecordByFbType",self.__cur_actual_fb_type)           
            if chapter_info.__map_idx_record > org_record and chapter_info.__map_idx_record < #fb_info.gamelevel_limit_list then
                if org_record > 0 then
                    is_change = true
                end
                --弹窗
                LogicGUI.TemplDailyDungeonPassedNewDifficultLevel.New(self.__cur_actual_fb_type,chapter_info.__map_idx_record,function ()
                    if org_record > 0 then
                        self.__cur_selected_page = self.__cur_selected_page + 1
                        self:__HandleChangePage(true)
                    end
                end)
            end
        end
    end
    --当前选中副本可以挑战到的难度
    self.__cur_selected_page = (self.__cur_level_limit > chapter_info.__map_idx_record) and (chapter_info.__map_idx_record + 1) or chapter_info.__map_idx_record
    if is_change then
        self.__cur_selected_page = self.__cur_selected_page - 1
    end
    --数据变化
    self:__HandleChangePage(nil,true)
end

function Modules.DailyDungeonView:__HandleChangePage(is_front,is_selected_callback)
    --初始化条目
    if not self.__cur_page then
        self.__cur_page = GUI.TemplDailyDungeonPage.New(self.__cur_actual_fb_type,self.__cur_selected_page)
        self.__cur_page:GetNode():SetCascadeOpacityEnabled(true)
        self.__cur_page:SetAnchorPoint(0.5,0)
        self.__panel_center:AddChild(self.__cur_page:GetNode())
        PosTool.CenterBottom(self.__cur_page,0,120)
        --控件数据变化
        self:__RunActionChangeData()
        return 
    end
    --隐藏需要隐藏的控件
    self:__VisiblePanel(false)
    --停止所有动作
    CPPActionManager.StopAllActions(self.__cur_page:GetNode())  
    if is_selected_callback then
        PosTool.CenterBottom(self.__cur_page,0,120)
        self.__cur_page:SetOpacity(255)
        self.__cur_page:ResetData(self.__cur_actual_fb_type,self.__cur_selected_page)
        --控件数据变化
        self:__RunActionChangeData()       
    else        
        local effect_time = 0.3
        CPPActionManager.MoveBy(self.__cur_page:GetNode(),effect_time,is_front and -300 or 300,0)
        CPPActionManager.FadeOut(self.__cur_page:GetNode(),effect_time,GlobalCallbackMgr:AddCallBackFunc(function ()
            self.__cur_page:SetPosition(self.__panel_center:GetContentWidth()/2 + (is_front and 300 or -300),120)
            self.__cur_page:ResetData(self.__cur_actual_fb_type,self.__cur_selected_page)
            CPPActionManager.MoveBy(self.__cur_page:GetNode(),effect_time,is_front and -300 or 300,0)
            CPPActionManager.FadeIn(self.__cur_page:GetNode(),effect_time,GlobalCallbackMgr:AddCallBackFunc(function ()
                --控件数据变化
                self:__RunActionChangeData()  
            end))
        end))        
    end
    
end

--做完动作后的回调
function Modules.DailyDungeonView:__RunActionChangeData()
    --可获取物品
    local fb_info = CPPGameLib.GetConfig("DailyChapter",self.__cur_actual_fb_type)
    -- local gamelevel_info = CPPGameLib.GetConfig("GameLevel",fb_info.gamelevel_list[self.__cur_selected_page])
    local chapter_info = CallModuleFunc(ModuleType.DUNGEON,"GetChapterInfo",self.__cur_actual_fb_type,Macros.Game.FBType.DAILY_DUNGEON)
    --奖励容器
    self.__panel_encourageitem1:SetVisible(false)
    self.__panel_encourageitem2:SetVisible(false)
    for i,icon_id in ipairs(fb_info.reward_item_id[self.__cur_selected_page]) do
        self["__panel_encourageitem"..i]:SetVisible(true)
        if not self["__encourageitem_icon"..i] then
            self["__encourageitem_icon"..i] = LogicGUI.IconLayer.New()
            self["__panel_encourageitem"..i]:AddChild(self["__encourageitem_icon"..i]:GetNode())
            PosTool.Center(self["__encourageitem_icon"..i])
        end
        local numStr = fb_info.reward_item_num[self.__cur_selected_page][i]
        self["__encourageitem_icon"..i]:SetData({id = icon_id, num = numStr})
    end
    --向后箭头   
    if self.__cur_selected_page > 1 then
       self.__text_backdifficultlevel:SetText(CPPGameLib.GetString("daliy_dungeon_diffcultlevel_"..(self.__cur_selected_page-1)))
    end
    self.__text_backdifficultlevel:SetVisible(self.__cur_selected_page > 1)
    self.__image_back:SetVisible(self.__cur_selected_page > 1)
    --向前箭头   
    if self.__cur_selected_page < #fb_info.gamelevel_list then
       self.__text_frontdifficultlevel:SetText(CPPGameLib.GetString("daliy_dungeon_diffcultlevel_"..(self.__cur_selected_page+1)))
    end
    self.__text_frontdifficultlevel:SetVisible(self.__cur_selected_page < #fb_info.gamelevel_list)
    self.__image_front:SetVisible(self.__cur_selected_page < #fb_info.gamelevel_list)
    --气泡
    self.__text_bubble:SetString(fb_info.chat)
    self.__image_bubble:SetScaleFactor(0.1)
    self.__image_bubble:SetVisible(true)   
    CPPActionManager.ScaleTo(self.__image_bubble,0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
            self.__panel_touch:SetTouchEnabled(true)
        end))
    --挑战按钮
    self.__button_challenge:SetEnabled(true)
    if self.__dungeon_info_lst[self.__cur_selected_index].__status ~= "notTime" then
        self.__text_changetimes:SetVisible(true)   
        self.__button_challenge:SetVisible(chapter_info.__remanent_battle_times > 0)
        self.__button_clean:SetVisible(chapter_info.__remanent_battle_times > 0)
        local record_idx = (self.__cur_level_limit > chapter_info.__map_idx_record) and (chapter_info.__map_idx_record + 1) or chapter_info.__map_idx_record
        self.__button_challenge:SetEnabled(self.__cur_selected_page <= record_idx)
        self.__button_clean:SetBright(chapter_info.__map_idx_record >= self.__cur_selected_page)
        print("skfb", "wwwwwwwwwwwwwwwwwwwwwwww ", self.__cur_actual_fb_type, chapter_info.__remanent_battle_times)
    end
    --显示需要的控件
    self:__VisiblePanel(true)
end

--显隐某些控件
function Modules.DailyDungeonView:__VisiblePanel(is_visible)
    if is_visible then
        self.__image_bestencourageframe:SetVisible(is_visible)
    else
        self.__image_bestencourageframe:SetVisible(is_visible)
        self.__text_backdifficultlevel:SetVisible(is_visible)
        self.__image_back:SetVisible(is_visible)
        self.__image_front:SetVisible(is_visible)
        self.__text_frontdifficultlevel:SetVisible(is_visible)
        self.__image_bubble:SetVisible(is_visible)
        self.__button_challenge:SetVisible(is_visible)
        self.__text_changetimes:SetVisible(is_visible)
        self.__button_clean:SetVisible(is_visible)
    end
end

------------------------------------------------------------------------------------------------------------------
--日常副本类型条目
------------------------------------------------------------------------------------------------------------------

Modules = Modules or {}

Modules.TemplDailyDungeonItem = Modules.TemplDailyDungeonItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDailyDungeonItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_DAILY_ITEM)
end 

function Modules.TemplDailyDungeonItem:SetData(data)
    GUI.ScrollItem.SetData(self,data)
    local fb_type_info = CPPGameLib.GetConfig("DailyChapter",self.__data.__index)

    local Image_frame = self:GetWidget("Image_frame")
    Image_frame:IgnoreContentAdaptWithSize( true )

    local Image_ribbon = self:GetWidget("Image_ribbon")
    Image_ribbon:IgnoreContentAdaptWithSize( true )

    --头像
    local Panel_icon = self:GetWidget("Panel_icon")

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_DAILY, "bg_icon_bg")
    local bg_icon_bg = Util:Sprite(sp)       
    Panel_icon:AddChild(bg_icon_bg)
    PosTool.Center(bg_icon_bg)

    local icon = LogicGUI.IconLayer.New(false,false)
    Panel_icon:AddChild(icon:GetNode())
    icon:SetData({id = fb_type_info.icon_id })
    PosTool.Center(icon)
    icon:adaptParentContentSize()

    --文字
    Util:WidgetLabel(self:GetWidget("Text_dungeonName"), fb_type_info.name, Macros.TypefaceSize.normal, Macros.Color.btn,0,0,Macros.Color.GrayBlack_stroke)

    --状态
    if self.__data.__status ~= "ok" then        
        if self.__data.__status == "notLevel" then
            icon:SetGray(true)
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon")
            local lock_img = Util:Sprite(sp)       
            self:GetNode():AddChild(lock_img,1)
            PosTool.Center(lock_img,0,15)
            local label = Util:Label(CPPGameLib.GetString("dungeon_daily_open_level",fb_type_info.gamelevel_limit_list[1]), Macros.TypefaceSize.normal, Macros.Color.red, 0, 0,Macros.Color.red_stroke)
            self:GetNode():AddChild(label,2)
            PosTool.Center(label,0,-8)
        elseif self.__data.__status == "notTime" then
            icon:SetGray(true)
            self.__data.__str = self:ContentShow(self.__data.__index)
            local label = Util:Label(self.__data.__str, Macros.TypefaceSize.slightly, Macros.Color.btn,130,0,Macros.Color.common_stroke)
            label:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
            self:GetNode():AddChild(label,3)
            PosTool.Center(label,0,10)

        else
            print("skfb", "ffffffffffffffffffffffffffffff ", self.__data.__index, self.__data.__status)
        end
    else
        icon:SetGray(false)
        if self.__icon_red_point then
            self.__icon_red_point:RemoveSelf(true)
            self.__icon_red_point = nil
        end  
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
        self.__icon_red_point = Util:Sprite(sp)       
        self:GetNode():AddChild(self.__icon_red_point , 2)
        PosTool.RightTop(self.__icon_red_point,-35,-15)
    end
end

--开启时间
function Modules.TemplDailyDungeonItem:ContentShow(index)
    local cnf = CPPGameLib.GetConfig("DailyChapter", index)
    local lab = ""
    for i,day in ipairs(cnf.week_day_list) do
        local h = i == #cnf.week_day_list and CPPGameLib.GetString("dungeon_daily_open_text") or CPPGameLib.GetString("common_Comma")
        lab = lab..CPPGameLib.GetString("num_"..day)..h
    end
    return lab
end

--选中框
function Modules.TemplDailyDungeonItem:SetSelected(var)
    if var then
        if not self.__icon_frame then
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_DAILY, "selected_frame")
            self.__icon_frame = Util:Sprite(sp)    
            self.__icon_frame:SetAnchorPoint(0.5,1)   
            self:GetNode():AddChild(self.__icon_frame,1)
            -- PosTool.Center(self.__icon_frame)
            PosTool.CenterTop(self.__icon_frame,0,2)
        end
    else
        if self.__icon_frame then
            self.__icon_frame:RemoveSelf(true)
            self.__icon_frame = nil
        end
    end
end


