Modules = Modules or {}

Modules.CampRecruitView = Modules.CampRecruitView or BaseClass(BaseView)

function Modules.CampRecruitView:__init()
	self.__layout_name = "camp_recruit"  
    self.__bg_params = {type = BaseView.BGType.SOLID,color = {0, 0, 0, 204}}   
    self.__open_callback = function()
    	--阵营招募数据请求
    	self:CallModuleFunc("CampRecruitDataReq")
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_CAMPRECRUIT_RECORD_CELL)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CAMP_RECRUIT)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CAMP_RECRUIT,true)
end

function Modules.CampRecruitView:__Dispose()
    if self.__record_scrollview then
        self.__record_scrollview:DeleteMe()
        self.__record_scrollview = nil
    end
    --删除特效
    self:__DeleteEffect()

    if self.__retain_recruit_timer then
        GlobalTimerQuest:CancelQuest(self.__retain_recruit_timer)
        self.__retain_recruit_timer = nil
    end

    if self.__record_req_count_down_timer then
        GlobalTimerQuest:CancelQuest(self.__record_req_count_down_timer)
        self.__record_req_count_down_timer = nil
    end
end

function Modules.CampRecruitView:__RegistAllEvents()
    --阵营招募数据响应
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.CAMP_RECRUIT_DATA_RESP, function(_,camp_recruit_data)
        self.__camp_recruit_data = camp_recruit_data       
        self:__OpenCallback()
    end)

    --阵营招募记录响应
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.CAMP_RECRUIT_RECORD_RESP, function(_,recruit_record_data)
        self:CreateRecordScrollView(recruit_record_data)
    end)

    --阵营招募响应
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.CAMP_RECRUIT_RESP, function(_,recruit_data)
        --免费文字
        self.__txt_freetime:SetVisible(self.__camp_recruit_data.today_already_recruit_times < 1)
        --钻石图标
        self.__img_one_diamond:SetVisible(self.__camp_recruit_data.today_already_recruit_times >= 1)
        --招募一次消耗钻石
        self.__txt_recruitonetime:SetVisible(self.__camp_recruit_data.today_already_recruit_times >= 1)
        --按钮设置
        self.__btn_recruit_onetime:SetEnabled(true)
        self.__btn_recruit_tentime:SetEnabled(true)
        local getReward = {}
        for i, rData in ipairs(recruit_data.hero_list) do
            table.insert(getReward,{id = rData.hero_id, num = rData.hero_num})
        end
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward,nil,nil,nil,true )
        --刷新箱子状态和积分
        self:RefrashBoxStatusAndScore()
        --刷新列表
        --阵营招募记录请求
        self:CallModuleFunc("CampRecruitRecordReq",self.__selected_index - 1)
    end)

    --阵营招募积分任务奖励响应
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.CAMP_RECRUIT_SCORE_TASK_REWARD_RESP, function(_,recruit_score_task_reward_data)
        local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",self.__camp_recruit_data.cur_data_id)
        local getReward = {}
        table.insert(getReward,{id = config_camp_recruit.recruit_data_list[recruit_score_task_reward_data.camp].score_task_reward_id_list[recruit_score_task_reward_data.reward_idx+1], num = config_camp_recruit.recruit_data_list[recruit_score_task_reward_data.camp].score_task_reward_num_list[recruit_score_task_reward_data.reward_idx+1]})
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )
        --刷新箱子状态和积分
        self:RefrashBoxStatusAndScore()
    end)
end

function Modules.CampRecruitView:__OpenCallback()
    --宝箱资源文字
    self.__box_res_list = {[1] = "green",[2] = "green",[3] = "blue",[4] = "purple",[5] = "orange"}
    --特效文件
    self.__starbox_effect_map = {"UI_wood_box","UI_wood_box", "UI_silverybox","UI_blue_box","UI_red_box"}

    local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",self.__camp_recruit_data.cur_data_id)
    self:CallModuleFunc("GetActivityOpen",Macros.Game.SystemID.CAMP_RECRUIT)
    if not config_camp_recruit then
        return
    end
    local open_camp = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.CampRecruit))
    open_camp = open_camp and tonumber(open_camp) or 1  
    open_camp = open_camp <= 1 and 1 or open_camp
    --阵营标记
    self.__img_camptag = self:GetWidget("Image_CampTag")
    --热点英雄半身像
    self.__img_halfhead = self:GetWidget("Image_HalfHead")
    self.__img_halfhead_pos_list = {self.__img_halfhead:GetPositionX(),self.__img_halfhead:GetPositionY()}
    self.__breathe_func = GlobalCallbackMgr:AddCallBackFunc(function ()       
        CPPActionManager.ScaleTo(self.__img_halfhead,1.5,1,GlobalCallbackMgr:AddCallBackFunc(function ()       
        self:ImgHalfHeadBreatheFunc()
        end))
    end)
    self:ImgHalfHeadBreatheFunc()
    --召唤预览
    self.__btn_prelook = self:GetWidget("Button_PreLook")
    self.__btn_prelook:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_handbook"),TextureResType.LOCAL)
    WidgetUnity.SetWidgetListener(self.__btn_prelook, nil, nil, function()
        --阵营招募预览模板
        Modules.TemplCampRecruitPreLook.New(self.__camp_recruit_data.cur_data_id,self.__selected_index)
    end, nil)
    --召唤预览标题
    self.__txt_recruit_prelook = self:GetWidget("Text_RecruitPreLook")
    Util:WidgetLabel(self.__txt_recruit_prelook,CPPGameLib.GetString("hero_recruitPreLook"), Macros.TypefaceSize.largish, Macros.Color.orange, 0, 0, Macros.Color.orange_stroke)
    --选中框
    self.__checkbox = self:GetWidget("CheckBox")
    WidgetUnity.SetCheckboxListener(self.__checkbox,function ()
        Cookies.Set(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.CampRecruit), tostring(self.__selected_index))
    end,function ()
        Cookies.Set(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.CampRecruit), "0")
    end)   
    --选中框提示
    self.__txt_checkboxtips = self:GetWidget("Text_CheckBoxTips")
    Util:WidgetLabel(self.__txt_checkboxtips,CPPGameLib.GetString("camp_recruit_checkbox_tips"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --tab容器
    self.__panel_tab = self:GetWidget("Panel_Tab")
    --进度条
    self.__loading_bar = self:GetWidget("LoadingBar")
    --热点英雄名字
    self.__txt_hothero = self:GetWidget("Text_HotHero")
    Util:WidgetLabel(self.__txt_hothero,"", Macros.TypefaceSize.headline, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --活动剩余时间
    self.__txt_act_retaintime = self:GetWidget("Text_ActRetainTime")
    Util:WidgetLabel(self.__txt_act_retaintime,CPPGameLib.GetString("monster_stormcastle_residuetimetxt"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --活动剩余时间数字显示
    self.__txt_act_retaintimenum = self:GetWidget("Text_ActRetainTimeNum")
    Util:WidgetLabel(self.__txt_act_retaintimenum,"", Macros.TypefaceSize.normal, Macros.Color.green, 0, 0, Macros.Color.green_stroke)
    --倒计时
    if not self.__retain_recruit_timer then
        self.__retain_recruit_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            self:HeartBeatHighrecruitTimer()
        end), 1, -1)
    end 
    self:HeartBeatHighrecruitTimer()
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_Close"),"",function ()
        self:CloseManual()
    end)
    --阵营底框
    self.__img_campframe = self:GetWidget("Image_CampFrame")
    --滚动容器容器
    self.__panel_recordscrollview = self:GetWidget("Panel_RecordScrollView")
    --免费文字
    self.__txt_freetime = self:GetWidget("Text_FreeTime")
    self.__txt_freetime:SetVisible(self.__camp_recruit_data.today_already_recruit_times < 1)
    Util:WidgetLabel(self.__txt_freetime,CPPGameLib.GetString("hero_recruitForFreeOnce"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
    --钻石图标
    self.__img_one_diamond = self:GetWidget("Image_6")
    self.__img_one_diamond:SetVisible(self.__camp_recruit_data.today_already_recruit_times >= 1)
    --招募一次消耗钻石
    self.__txt_recruitonetime = self:GetWidget("Text_RecruitOneTime")
    self.__txt_recruitonetime:SetVisible(self.__camp_recruit_data.today_already_recruit_times >= 1)
    Util:WidgetLabel(self.__txt_recruitonetime,config_camp_recruit.recruit_data_list[0].normal_recruit_cost_diamond, Macros.TypefaceSize.normal, Macros.Color.keypoint)
    --招募一次消耗按钮
    self.__btn_recruit_onetime = self:GetWidget("Button_RecruitOneTime")
    Util:WidgetButton(self.__btn_recruit_onetime,CPPGameLib.GetString("camp_recruit_recruit_times",1), function ()
        local heroNum,heroMaxNum = CallModuleFunc(ModuleType.HERO,"GetHeroSumAndMax")
        if heroNum > (heroMaxNum - 1) then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitFullHeroTip"))
            return
        end
        if not self.__txt_freetime:IsVisible() then
            local have_diamonds_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
            if have_diamonds_num < config_camp_recruit.recruit_data_list[0].normal_recruit_cost_diamond then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitDiamondNoEnough"))
                return 
            end
        end
        --是否已经过期
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime",Macros.Game.SystemID.CAMP_RECRUIT)
        if info.show ~= "open" then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gashaponmachine_end")) 
            --入口消失
            CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.CAMP_RECRUIT) 
            return
        end
        self.__btn_recruit_onetime:SetEnabled(false)
        self.__btn_recruit_tentime:SetEnabled(false)
        --阵营招募请求
        local info = {}
        info.recruit_type = 1
        info.camp = self.__selected_index - 1
        self:CallModuleFunc("CampRecruitReq",info)
    end, Macros.TypefaceSize.tab, Macros.Color.btn, Macros.Color.button_yellow, 2)
    --钻石图标
    self.__img_ten_diamond = self:GetWidget("Image_6")
    --招募十次消耗钻石
    self.__txt_recruittentime = self:GetWidget("Text_RecruitTenTime")
    Util:WidgetLabel(self.__txt_recruittentime,config_camp_recruit.recruit_data_list[0].one_key_recruit_cost_diamond, Macros.TypefaceSize.normal, Macros.Color.keypoint)
    --招募十次消耗按钮
    self.__btn_recruit_tentime = self:GetWidget("Button_RecruitTenTime")
    Util:WidgetButton(self.__btn_recruit_tentime,CPPGameLib.GetString("camp_recruit_recruit_times",10), function ()
        local heroNum,heroMaxNum = CallModuleFunc(ModuleType.HERO,"GetHeroSumAndMax")
        if heroNum > (heroMaxNum - 10) then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_morerecruitFullHeroTip"))
            return
        end
        local have_diamonds_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
        if have_diamonds_num < config_camp_recruit.recruit_data_list[0].one_key_recruit_cost_diamond then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_recruitDiamondNoEnough"))
            return 
        end
        --是否已经过期
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime",Macros.Game.SystemID.CAMP_RECRUIT)
        if info.show ~= "open" then
            --提示
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gashaponmachine_end"))  
            --入口消失
            CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.CAMP_RECRUIT)
            return
        end
        self.__btn_recruit_onetime:SetEnabled(false)
        self.__btn_recruit_tentime:SetEnabled(false)
        --阵营招募请求
        local info = {}
        info.recruit_type = 2
        info.camp = self.__selected_index - 1
        self:CallModuleFunc("CampRecruitReq",info)
    end, Macros.TypefaceSize.tab, Macros.Color.btn, Macros.Color.button_yellow, 2)
    --总积分
    self.__txt_totalintegral = self:GetWidget("Text_TotalIntegral")
    Util:WidgetLabel(self.__txt_totalintegral,"", Macros.TypefaceSize.largish, Macros.Color.golden)
    --积分进度
    local scroe_task_list = config_camp_recruit.recruit_data_list[0].scroe_task_list
    if scroe_task_list then
        local max_integral = scroe_task_list[#scroe_task_list]
        local open_box_func = function (index,score)
            local info = {}
            info.index = index
            info.camp = self.__selected_index - 1
            info.cur_data_id = self.__camp_recruit_data.cur_data_id
            info.score = score
            --总积分
            local cur_totalscore = self.__camp_recruit_data.camp_score_list[self.__selected_index].score or 0
            info.isCouldGet = cur_totalscore >= score
            local record_list = {}
            for i,score_info in ipairs(self.__camp_recruit_data.score_task_reward_record_list) do
                if score_info.camp == (self.__selected_index - 1) then
                    record_list = score_info.record_list
                end
            end
            info.isHaveGet = false
            for i,record_info in ipairs(record_list) do
                if record_info.reward_idx == (index-1) then
                    info.isHaveGet = true
                    break
                end
            end
            LogicGUI.TemplCampRecruitAwardView.New(info)
        end
        for i,cur_score in ipairs(scroe_task_list) do
            --积分进度箱子
            self["__btn_integralbox"..i] = self:GetWidget("Button_IntegralBox"..i)
            self["__btn_integralbox"..i]:SetScaleFactor(0.8)
            self["__btn_integralbox"..i]:SetPosition(255+580*(cur_score/max_integral),self["__btn_integralbox"..i]:GetPositionY())
            WidgetUnity.SetWidgetListener(self["__btn_integralbox"..i], nil, nil, function()
                open_box_func(i,cur_score)
            end, nil)
            --特效容器
            self["__panel_specialeffect"..i] = self:GetWidget("Panel_specialEffect"..i)    
            self["__panel_specialeffect"..i]:SetScaleFactor(0.8) 
            self["__panel_specialeffect"..i]:SetPosition(262+580*(cur_score/max_integral),self["__panel_specialeffect"..i]:GetPositionY())      
            WidgetUnity.SetWidgetListener(self["__panel_specialeffect"..i], nil, nil, function()
                open_box_func(i,cur_score)
            end, nil)
            --设置积分进度位置
            local bg_integral = self:GetWidget("bg_integral"..i)
            bg_integral:SetPosition(262+580*(cur_score/max_integral),bg_integral:GetPositionY())
            --积分进度
            local txt_integral = self:GetWidget("Text_integral"..i)
            Util:WidgetLabel(txt_integral,cur_score, Macros.TypefaceSize.slightly, Macros.Color.btn)            
        end
    end
    --左边列表
    local btn_res = {"btn_flag_selected","btn_flag_unselected"}
    self.__btn_res = {}
    for _,btn_res_name in ipairs(btn_res) do
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,btn_res_name)
        table.insert(self.__btn_res,sp)
    end 
    --初始化
    local left_namelist = {}
    local left_iconlist = {}
    for i=1,3 do
        local camp_name = CPPGameLib.GetString("hero_camp_"..(i-1))
        table.insert(left_namelist,camp_name)
        local camp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"camp_icon_"..(i-1),true)
        table.insert(left_iconlist,camp_sp)
    end 
    self.__btn_list = {}
    local offset_y = 0
    local interval = 20
    for i,name_str in ipairs(left_namelist) do
        local sp_data = {}
        local btn = Util:Button(self.__btn_res[2],function ()
            self:SetSelectedCallBack(i)
        end)
        btn:SetAnchorPoint(0,1)
        self.__panel_tab:AddChild(btn)
        PosTool.LeftTop(btn,0,offset_y)
        sp_data.btn = btn
        table.insert(self.__btn_list,sp_data)
        offset_y = offset_y - btn:GetContentHeight() - interval
        --图标
        local icon = Util:Sprite9(left_iconlist[i])
        icon:SetAnchorPoint(0,0.5)
        btn:AddChild(icon)
        PosTool.LeftCenter(icon,30,-5)
        icon:SetScaleFactor(0.7)
        --文字
        sp_data.label = Util:Label(name_str, Macros.TypefaceSize.largish, Macros.Color.btn)
        sp_data.label:SetAnchorPoint(0,0.5)
        btn:AddChild(sp_data.label)
        PosTool.LeftCenter(sp_data.label,93,0)
    end
    self:SetSelectedCallBack(open_camp)
end

function Modules.CampRecruitView:__DeleteEffect()
    for i=1,5 do
        if self["__button_zoubian_effect"..i] then
            self:__DelEffect(self["__button_zoubian_effect"..i])
            self["__button_zoubian_effect"..i] = nil
        end
        if self["__button_receive_effect"..i] then
            self:__DelEffect(self["__button_receive_effect"..i])
            self["__button_receive_effect"..i] = nil
        end
    end
end

function Modules.CampRecruitView:ImgHalfHeadBreatheFunc()
    CPPActionManager.ScaleTo(self.__img_halfhead,1.5,1.05,self.__breathe_func)
end

function Modules.CampRecruitView:SetSelectedCallBack(index)
    if self.__selected_index and self.__selected_index == index then
        return
    end
    local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",self.__camp_recruit_data.cur_data_id)
    if not config_camp_recruit then
        return
    end
    self.__selected_index = index
    for i,sp_data in ipairs(self.__btn_list) do
        sp_data.btn:SetNormalImage(index == i and self.__btn_res[1] or self.__btn_res[2])
        sp_data.label:SetColor(unpack(index == i and Macros.Color.golden or Macros.Color.content))
    end 
    --默认打开阵营
    local open_camp = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.CampRecruit))
    open_camp = open_camp and tonumber(open_camp) or 0
    self.__checkbox:SetSelectedState(self.__selected_index == open_camp)
    --阵营招募记录请求
    self:CallModuleFunc("CampRecruitRecordReq",index - 1)
    --阵营标记
    local camp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CAMP_RECRUIT,"bg_camp_bottom_"..(index-1),true)
    self.__img_camptag:SetImage(camp_sp)
    --热点英雄半身像
    local config_hero = CPPGameLib.GetConfig("Hero",config_camp_recruit.recruit_data_list[index-1].hot_hero)
    local res_name = Resource.PathTool.GetHalfHeadPath(config_hero and config_hero.half_head or config_camp_recruit.recruit_data_list[index-1].hot_hero)
    self.__img_halfhead:IgnoreContentAdaptWithSize(true)
    self.__img_halfhead:SetImage(res_name,TextureResType.LOCAL)
    self.__img_halfhead:SetPosition(self.__img_halfhead_pos_list[1]+config_camp_recruit.recruit_data_list[index-1].half_head_offset_x,self.__img_halfhead_pos_list[2]+config_camp_recruit.recruit_data_list[index-1].half_head_offset_y)
    --热点英雄
    self.__txt_hothero:SetText(config_hero and config_hero.name or "")
    self.__txt_hothero:SetColor(unpack(Macros.Quality[config_hero and config_hero.init_quality or 0]))
    self.__txt_hothero:SetStrokeColor(unpack(Macros.Quality_Stroke[config_hero and config_hero.init_quality or 0]))
    --阵营底框   
    camp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CAMP_RECRUIT,"camp_frame_"..(index-1))
    self.__img_campframe:SetImage(camp_sp)
    --刷新箱子状态和积分
    self:RefrashBoxStatusAndScore()
end

function Modules.CampRecruitView:CreateRecordScrollView(recruit_record_data)   
    if not self.__record_scrollview then
        local params = {
            item_class = Modules.TemplCampRecruitRecordCell,
            item_width = 377,
            item_height = 52,
            row = 4,
            col = 1,
            item_space_r = 3,
            view_width = 377,
            view_height = 200,
        }
        self.__record_scrollview = GUI.ScrollView.New(params)
        self.__panel_recordscrollview:AddChild(self.__record_scrollview:GetNode())
        PosTool.Center(self.__record_scrollview)
    end
    self.__record_scrollview:SetDataList(recruit_record_data.hero_list)
    self.__record_scrollview:LocateToLast()
    -- if not self.__record_req_count_down_timer then
    --     --延时请求
    --     self.__record_req_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
    --     function()
    --         self.__is_count_down_record_req = true
    --         self:CallModuleFunc("NDLotteryRecordReq")
    --     end), 10, -1)
    -- end
    --先整理
    -- if not self.__is_count_down_record_req then
    --     --移动
    --     local inner_posx,inner_posy = self.__record_scrollview:GetInnerContainerPosition()
    --     self.__record_scrollview:StartAutoScrollToDestination(COCOPoint(inner_posx,0),3,false)
    -- else
    --     self.__record_scrollview:ScrollToBottom()
    -- end
    self.__is_count_down_record_req = false
end

--刷新箱子状态和积分
function Modules.CampRecruitView:RefrashBoxStatusAndScore() 
    local config_camp_recruit = CPPGameLib.GetConfig("CampRecruit",self.__camp_recruit_data.cur_data_id)
    --总积分
    local cur_totalscore = self.__camp_recruit_data.camp_score_list[self.__selected_index].score or 0
    self.__txt_totalintegral:SetText(cur_totalscore)
    --积分进度
    local scroe_task_list = config_camp_recruit.recruit_data_list[0].scroe_task_list
    if scroe_task_list then
        --进度条
        local max_integral = scroe_task_list[#scroe_task_list]
        self.__loading_bar:SetPercent((cur_totalscore/max_integral)*100)
        --积分任务奖励记录列表
        local record_list = {}
        for i,score_info in ipairs(self.__camp_recruit_data.score_task_reward_record_list) do
            if score_info.camp == (self.__selected_index - 1) then
                record_list = score_info.record_list
            end
        end   
        --删除特效
        self:__DeleteEffect() 
        --箱子
        for i,cur_score in ipairs(scroe_task_list) do
            local isHaveGet = false
            for _,record_info in ipairs(record_list) do
                if record_info.reward_idx == (i-1) then
                    isHaveGet = true
                    break
                end
            end
            --积分进度箱子
            if cur_totalscore < cur_score or (cur_totalscore >= cur_score and isHaveGet) then
                local res_str = cur_totalscore >= cur_score and "_box_open" or "_box_close"
                local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, (self.__box_res_list[i] and self.__box_res_list[i] or self.__box_res_list[#self.__box_res_list])..res_str)
                self["__btn_integralbox"..i]:SetNormalImage(sp)
                self["__btn_integralbox"..i]:SetVisible(true)    
                self["__panel_specialeffect"..i]:SetVisible(false)  
            else               
                self["__btn_integralbox"..i]:SetVisible(false)   
                self["__panel_specialeffect"..i]:SetVisible(true)
                local effect
                self["__button_zoubian_effect"..i],effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"},self["__panel_specialeffect"..i])
                PosTool.Center(effect)
                self["__button_receive_effect"..i],effect = self:__CreateEffect({res_name = self.__starbox_effect_map and self.__starbox_effect_map[i] or self.__starbox_effect_map[#self.__starbox_effect_map], type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE },self["__panel_specialeffect"..i],2)
                if i <= 2 then
                    i = 1
                end
                PosTool.Center(effect, 0, -30 - math.min(1,i-1) * 5)  
            end
        end
    end
end

function Modules.CampRecruitView:HeartBeatHighrecruitTimer()
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local over_server_time = self:CallModuleFunc("GetActivityOverTime",Macros.Game.SystemID.CAMP_RECRUIT)
    local retainTime = over_server_time - severTime           
    if retainTime > 0 then
        local text = TimeTool.TransTimeStamp(CPPGameLib.GetString("time_23"),retainTime)
        self.__txt_act_retaintimenum:SetText(text)
    else
        GlobalTimerQuest:CancelQuest(self.__retain_recruit_timer)
        self.__retain_recruit_timer = nil
        --入口消失
        CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.CAMP_RECRUIT)
    end
end