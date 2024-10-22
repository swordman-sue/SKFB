Modules = Modules or {}

Modules.GloryArenaView = Modules.GloryArenaView or BaseClass(BaseView)

function Modules.GloryArenaView:__init()
	self.__layout_name = "glory_arena_view"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_union_dungeon")}  
    self.__title_bar_params = {
        title = CPPGameLib.GetString("glory_arena_title"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function ( )                     
            self:CloseManual()
        end
    }
    self.__open_callback = function()        
    	--积分赛数据请求
    	self:CallModuleFunc("ScoreMatchDataReq")
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.INTEGRAL_RACE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.INTEGRAL_RACE,true)
end

function Modules.GloryArenaView:__RegistAllEvents()
    --积分赛数据响应
    self:BindGlobalEvent(Macros.Event.IntegralModule.SCORE_MATCHDATA_RESP, function(_,score_match_data,is_refresh,is_buy_battle_times)
        self.__score_match_data = score_match_data       
        self:__OpenCallback()
    end)
end

function Modules.GloryArenaView:__Dispose()
    if self.__activity_open_timer then
        GlobalTimerQuest:CancelQuest(self.__activity_open_timer)
        self.__activity_open_timer = nil
    end
end

function Modules.GloryArenaView:__OpenCallback()
    if not self.__score_match_data then
        return
    end

    local is_open = self:CallModuleFunc("GetIsCurActivityOpen")
    --积分赛  
    self.__img_integralrace = self:GetWidget("Image_IntegralRace")
    self.__img_integralrace:SetColor(unpack(is_open and Macros.Color.lilyWhite or Macros.Color.Gray))
    WidgetUnity.SetWidgetListener(self.__img_integralrace,nil,nil,function ()
        --进去
        local is_open = self:CallModuleFunc("GetIsCurActivityOpen")
        if is_open then
            --打开积分赛
            GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.INTEGRAL_RACE)
        else
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("glory_unopen_tips"))
        end
    end)
    --敬请期待
    self.__txt_waitforhopeintegral = self:GetWidget("Text_WaitForHopeIntegral")
    self.__txt_waitforhopeintegral:SetVisible(not is_open)
    Util:WidgetLabel(self.__txt_waitforhopeintegral,CPPGameLib.GetString("spirit_look_forward_to"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --积分赛结束时间
    self.__img_integralracetime = self:GetWidget("Image_IntegralRaceTime")
    self.__img_integralracetime:SetVisible(is_open)
    --积分赛文字
    self.__img_3 = self:GetWidget("Image_3")
    self.__img_3:IgnoreContentAdaptWithSize(true)
    self.__img_3:SetColor(unpack(is_open and Macros.Color.lilyWhite or Macros.Color.Gray))
    --结束日期
    self.__txt_integralracetime = self:GetWidget("Text_IntegralRaceTime")
    Util:WidgetLabel(self.__txt_integralracetime,"", Macros.TypefaceSize.largish, Macros.Color.golden)
    --赛季未开始
    self.__txt_integralraceunopen = self:GetWidget("Text_IntegralRaceUnOpen")
    self.__txt_integralraceunopen:SetVisible(not is_open)
    Util:WidgetLabel(self.__txt_integralraceunopen,CPPGameLib.GetString("glory_unopen_tips2"), Macros.TypefaceSize.largish, Macros.Color.red, 0, 0, Macros.Color.red_stroke)

    --默认争霸赛未开启
    local is_sp_open = false
    --争霸赛
    self.__img_championship = self:GetWidget("Image_ChampionShip")
    self.__img_championship:SetColor(unpack(is_sp_open and Macros.Color.lilyWhite or Macros.Color.Gray))
    WidgetUnity.SetWidgetListener(self.__img_championship,nil,nil,function ()
        --进去
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("glory_unopen_tips"))
    end)
    --敬请期待
    self.__txt_waitforhopechampionship = self:GetWidget("Text_WaitForHopeChampionShip")
    self.__txt_waitforhopechampionship:SetVisible(not is_sp_open)
    Util:WidgetLabel(self.__txt_waitforhopechampionship,CPPGameLib.GetString("spirit_look_forward_to"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
    --争霸赛结束时间
    self.__img_championshiptime = self:GetWidget("Image_ChampionShipTime")
    self.__img_championshiptime:SetVisible(is_sp_open)
    --争霸赛文字
    self.__img_3_0 = self:GetWidget("Image_3_0")
    self.__img_3_0:IgnoreContentAdaptWithSize(true)
    self.__img_3_0:SetColor(unpack(is_sp_open and Macros.Color.lilyWhite or Macros.Color.Gray))
    --争霸赛结束日期
    self.__txt_championshiptime = self:GetWidget("Text_ChampionShipTime")
    Util:WidgetLabel(self.__txt_championshiptime,"", Macros.TypefaceSize.largish, Macros.Color.golden)
    --争霸赛赛季未开始
    self.__txt_championshipunopen = self:GetWidget("Text_ChampionShipUnOpen")
    self.__txt_championshipunopen:SetVisible(not is_sp_open)
    Util:WidgetLabel(self.__txt_championshipunopen,CPPGameLib.GetString("glory_unopen_tips2"), Macros.TypefaceSize.largish, Macros.Color.red, 0, 0, Macros.Color.red_stroke)
    --开始了就倒计时结束时间
    if is_open then
        if not self.__activity_open_timer then
            self.__activity_open_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
            function()
                self:HeartBeatrecruitTimer()
            end), 1, -1)
        end 
        self:HeartBeatrecruitTimer()
    end
end

function Modules.GloryArenaView:HeartBeatrecruitTimer()
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local is_open,start_time,over_time = self:CallModuleFunc("GetIsCurActivityOpen")
    local retainTime = over_time - severTime
    if retainTime > 0 then
        self.__txt_integralracetime:SetText(CPPGameLib.GetString("glory_end_txt",TimeTool.TransTimeStamp(CPPGameLib.GetString("time_23"),retainTime) or ""))
    else
        --积分赛  
        self.__img_integralrace:SetColor(unpack(is_open and Macros.Color.lilyWhite or Macros.Color.Gray))
        --敬请期待
        self.__txt_waitforhopeintegral:SetVisible(not is_open)      
        --积分赛结束时间
        self.__img_integralracetime:SetVisible(is_open)
        --积分赛文字
        self.__img_3:SetColor(unpack(is_open and Macros.Color.lilyWhite or Macros.Color.Gray))
        --赛季未开始
        self.__txt_integralraceunopen:SetVisible(not is_open)
        --结束倒计时
        GlobalTimerQuest:CancelQuest(self.__activity_open_timer)
        self.__activity_open_timer = nil
    end
end