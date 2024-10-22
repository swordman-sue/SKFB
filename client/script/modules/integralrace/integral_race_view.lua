Modules = Modules or {}

Modules.IntegralRaceView = Modules.IntegralRaceView or BaseClass(BaseView)

local COST_TYPE = {
    CHALLENGE_COST = 1,
    REFRESH_COST = 2,
}

function Modules.IntegralRaceView:__init()
	self.__layout_name = "integral_race"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_union_dungeon")}  
    self.__title_bar_params = {
        title = CPPGameLib.GetString("integral_race_title"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function ( )     
            --刷新中
            if self.__is_on_refrash then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("integral_wait_for_refresh"))
                return
            end                 
            self:CloseManual()
        end
    }
    self.__open_callback = function()
        self:__OpenCallback()
    	--积分赛数据请求
    	self:CallModuleFunc("ScoreMatchDataReq")
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_CAMPRECRUIT_RECORD_CELL)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.INTEGRAL_RACE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.INTEGRAL_RACE,true)
end

function Modules.IntegralRaceView:__Dispose()
    
end

function Modules.IntegralRaceView:__RegistAllEvents()
    --阵容红点
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
        if system_id == Macros.Game.SystemID.HERO_LINEUP then
            self:__UpdateLineupBtnRedDot()
        end
    end)

    --积分赛数据响应
    self:BindGlobalEvent(Macros.Event.IntegralModule.SCORE_MATCHDATA_RESP, function(_,score_match_data,is_refresh,is_buy_battle_times)
        self.__score_match_data = score_match_data        
        if is_buy_battle_times then
            --挑战次数
            self.__txt_challengetimes:SetText(CPPGameLib.GetString("integral_race_can_challenge_times",math.max(0,self.__cur_challenge_times + self.__score_match_data.buy_battle_times - self.__score_match_data.battle_times)))
        else
            --初始化挑战信息
            self:InitChallengeMessage(is_refresh)
            if is_refresh then
                self.__is_on_refrash = false
            end
        end
    end)
end

function Modules.IntegralRaceView:__OpenCallback()
    --增加今日可挑战次数
    local btn_addtimes = self:GetWidget("Button_AddTimes")
    Util:WidgetButton(btn_addtimes, "",function ()
        if not self.__score_match_data then
            return
        end
        self:OpenAffirmTipsView(COST_TYPE.CHALLENGE_COST)       
    end)
    --挑战次数
    self.__txt_challengetimes = self:GetWidget("Text_ChallengeTimes")
    Util:WidgetLabel(self.__txt_challengetimes,"", Macros.TypefaceSize.largish, Macros.Color.btn)
    --查询免费挑战次数
    self.__cur_challenge_times = ConfigAdapter.Common.GetIntegralRaceFreeChallengeTimes()
    -- local refresh_config = CPPGameLib.GetConfig("ScoreMatchTimesCost",COST_TYPE.CHALLENGE_COST)
    -- for _,refresh_info in ipairs(refresh_config.cost_diamond_list) do
    --     if refresh_info.cost_diamond == 0 then
    --         self.__cur_challenge_times = self.__cur_challenge_times + 1
    --     else
    --         break
    --     end
    -- end
    --刷新次数
    self.__txt_refrashtimes = self:GetWidget("Text_RefrashTimes")
    Util:WidgetLabel(self.__txt_refrashtimes,"", Macros.TypefaceSize.largish, Macros.Color.btn,0,0,Macros.Color.common_stroke)
    --查询免费刷新次数
    self.__cur_refresh_times = 0
    local refresh_config = CPPGameLib.GetConfig("ScoreMatchTimesCost",COST_TYPE.REFRESH_COST)
    for _,refresh_info in ipairs(refresh_config.cost_diamond_list) do
        if refresh_info.cost_diamond == 0 then
            self.__cur_refresh_times = self.__cur_refresh_times + 1
        else
            break
        end
    end
    --刷新
    local btn_refrash = self:GetWidget("Button_Refrash")
    btn_refrash:SetZOrder(2)
    Util:WidgetButton(btn_refrash, "",function ()
        if not self.__score_match_data then
            return
        end
        --刷新中
        if self.__is_on_refrash then
            return
        end
        local refresh_times = math.max(0,self.__cur_refresh_times - self.__score_match_data.refresh_times)
        if refresh_times > 0 then
            --刷新挑战对手
            self:CallModuleFunc("ScoreMatchRefreshTargetReq")
        else           
            self:OpenAffirmTipsView(COST_TYPE.REFRESH_COST)
        end
    end)   
    --浮台初始位置
    self.__panel_floatingplatform_info_list = {}
    --初始化浮台
    for i=1,3 do
        --浮台容器
        self.__panel_floatingplatform_info_list[i] = {}
        --平台
        self.__panel_floatingplatform_info_list[i].panel_floatingplatform = self:GetWidget("Panel_FloatingPlatform"..i)
        local cur_panel = self.__panel_floatingplatform_info_list[i].panel_floatingplatform
        WidgetUnity.SetWidgetListener(cur_panel, nil, nil,function ()
            --拒绝三连
            if not self.__score_match_data then
                return
            end
            if not self.__score_match_data.battle_target_list then
                return
            end
            if not self.__score_match_data.battle_target_list[i] then
                return
            end
            --挑战对手是否死亡
            if self.__score_match_data.battle_target_list[i].is_dead >= Macros.Global.TRUE then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("integral_have_battle"))
                return
            end
            --次数检测
            local cur_challenge_times = math.max(0,self.__cur_challenge_times + self.__score_match_data.buy_battle_times - self.__score_match_data.battle_times)
            if cur_challenge_times <= 0 then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("integral_race_challenge_no_enough"))
                return
            end
            --存储信息
            local challenge_info = self.__score_match_data.battle_target_list[i]
            self:CallModuleFunc("SetIntegralRaceBattleMessage",CPPGameLib.GetString("integral_player_name",challenge_info.server_id or 0,challenge_info.target_name or ""),challenge_info.head_img)
            --挑战对手
            CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.INTEGRAL_RACE,self.__score_match_data.battle_target_list[i].target_id)
        end)
        --位置
        self.__panel_floatingplatform_info_list[i].pos = cur_panel:GetPositionY()
        cur_panel:SetPosition(cur_panel:GetPositionX(),0)      
        --模型容器
        self.__panel_floatingplatform_info_list[i].panel_heromodule = WidgetUnity.GetWidgetByName(cur_panel, "Panel_HeroModule")
        --战力容器
        self.__panel_floatingplatform_info_list[i].panel_battlepower = WidgetUnity.GetWidgetByName(cur_panel, "Panel_BattlePower")
        --可获得荣耀
        self.__panel_floatingplatform_info_list[i].txt_rewardintegral = WidgetUnity.GetWidgetByName(cur_panel, "Text_RewardIntegral")
        Util:WidgetLabel(self.__panel_floatingplatform_info_list[i].txt_rewardintegral,CPPGameLib.GetString("integral_win_award"), Macros.TypefaceSize.slightly, Macros.Color.btn,0,0,Macros.Color.common_stroke)
        --可获得荣耀（数字）
        self.__panel_floatingplatform_info_list[i].txt_rewardintegral_num = WidgetUnity.GetWidgetByName(cur_panel, "Text_RewardIntegralNum")
        Util:WidgetLabel(self.__panel_floatingplatform_info_list[i].txt_rewardintegral_num,"50", Macros.TypefaceSize.slightly, Macros.Color.btn,0,0,Macros.Color.common_stroke)
        --荣耀图标
        --TODO 还没定货币
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..Macros.Game.RolePropType.SCORE_MATCH_HONOR)
        local img_moneyicon = WidgetUnity.GetWidgetByName(cur_panel, "Image_MoneyIcon")
        img_moneyicon:SetImage(sp)
    end
    --创建系统图标
    self:CreateSystemIconList()
end

--更新阵容按钮红点
function Modules.IntegralRaceView:__UpdateLineupBtnRedDot()
    if self.__entry_red_dot then
        local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.HERO_LINEUP, Macros.Game.SystemID.MAX)  
        self.__entry_red_dot:SetVisible(red_num > 0)
    end
end

--创建系统图标
function Modules.IntegralRaceView:CreateSystemIconList()
    local position_y = 10
     --荣耀排行
    self.__btn_glory_rank = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_rank"), function()
        LogicGUI.TemplIntegralRaceRankView.New(self.__score_match_data.camp)
    end)
    self:GetNode():AddChild(self.__btn_glory_rank)
    self.__btn_glory_rank:SetAnchorPoint(0.5,0)
    self.__btn_glory_rank:SetPosition(60,position_y)
    local label = Util:Label(CPPGameLib.GetString("integral_race_glory_rank_txt"), Macros.TypefaceSize.tab, Macros.Color.btn, 0, 0, Macros.Color.common_stroke,2)
    self.__btn_glory_rank:AddChild(label)
    PosTool.CenterBottom(label,0,12)
    --荣耀商店
    self.__btn_glory_shop = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_shop"), function()
        --请求购买记录
        CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",Macros.Game.SHOP.SHOP_SCORE_MATCH)
    end)
    self:GetNode():AddChild(self.__btn_glory_shop)
    self.__btn_glory_shop:SetAnchorPoint(0.5,0)
    self.__btn_glory_shop:SetPosition(180,position_y)
    local label = Util:Label(CPPGameLib.GetString("integral_race_glory_shop_txt"), Macros.TypefaceSize.tab, Macros.Color.btn, 0, 0, Macros.Color.common_stroke,2)
    self.__btn_glory_shop:AddChild(label)
    PosTool.CenterBottom(label,0,12)
    --挑战任务
    self.__btn_challenge_task = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_daily"), function()
        LogicGUI.TemplChallengeTask.New()
    end)
    self:GetNode():AddChild(self.__btn_challenge_task)
    self.__btn_challenge_task:SetAnchorPoint(0.5,0)
    self.__btn_challenge_task:SetPosition(300,position_y)
    local label = Util:Label(CPPGameLib.GetString("integral_race_challenge_task_txt"), Macros.TypefaceSize.tab, Macros.Color.btn, 0, 0, Macros.Color.common_stroke,2)
    self.__btn_challenge_task:AddChild(label)
    PosTool.CenterBottom(label,0,12)
    --右边区域容器
    local panel_rightregion = self:GetWidget("Panel_RightRegion")
    --阵容
    local city_icon_id = CallModuleFunc(ModuleType.MAIN_CITY,"TranslateSystemIDToEntryID",Macros.Game.SystemID.HERO_LINEUP)
    local entry_info = CPPGameLib.GetConfig("CityIcon",city_icon_id)
    self.__btn_lineup = Util:Button(Resource.PathTool.GetSystemEntryIconPath(entry_info.res_name), function()
        CallModuleFunc(ModuleType.HERO,"OpenView", BaseViewType.HERO_LINEUP)
    end)
    panel_rightregion:AddChild(self.__btn_lineup)
    self.__btn_lineup:SetAnchorPoint(0.5,0)
    self.__btn_lineup:SetPosition(150,position_y)
    local label = Util:Label(CPPGameLib.GetString("dungeon_lineup_btn"), Macros.TypefaceSize.tab, Macros.Color.btn, 0, 0, Macros.Color.common_stroke,2)
    self.__btn_lineup:AddChild(label)
    PosTool.CenterBottom(label,0,12)
    --红点
    local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
    self.__entry_red_dot:SetAnchorPoint(1, 1)
    self.__btn_lineup:AddChild(self.__entry_red_dot,1)
    PosTool.RightTop(self.__entry_red_dot)
    --更新
    self:__UpdateLineupBtnRedDot()
    --规则说明
    self.__btn_rule = Util:Button(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_rule"), function()
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_IntegralRace")
        end )
    end)
    panel_rightregion:AddChild(self.__btn_rule)
    self.__btn_rule:SetAnchorPoint(0.5,0)
    self.__btn_rule:SetPosition(50,position_y)
    local label = Util:Label(CPPGameLib.GetString("integral_race_rule_txt"), Macros.TypefaceSize.tab, Macros.Color.btn, 0, 0, Macros.Color.common_stroke,2)
    self.__btn_rule:AddChild(label)
    PosTool.CenterBottom(label,0,12)
end

--初始化挑战信息
function Modules.IntegralRaceView:InitChallengeMessage(is_refresh)
    if not self.__score_match_data then
        return
    end
    --挑战次数
    self.__txt_challengetimes:SetText(CPPGameLib.GetString("integral_race_can_challenge_times",math.max(0,self.__cur_challenge_times + self.__score_match_data.buy_battle_times - self.__score_match_data.battle_times)))
    --刷新次数
    self.__txt_refrashtimes:SetText(CPPGameLib.GetString("integral_race_free_refrash_times",math.max(0,self.__cur_refresh_times - self.__score_match_data.refresh_times)))
    --是否已经选了阵营
    if self.__score_match_data.camp ~= Macros.Game.FORCE.CAMP_INVALID then
        --悬浮台初始化
        self:InitPanelFloatingPlatform(is_refresh)
    else
        --打开选择阵营界面
        LogicGUI.TemplChooseCamp.New()
    end
end

--悬浮台初始化
function Modules.IntegralRaceView:InitPanelFloatingPlatform(is_refresh)
    if not self.__score_match_data then
        return
    end
    --挑战列表
    for list_index,challenge_info in ipairs(self.__score_match_data.battle_target_list) do
        --平台信息
        local cur_platform_info = self.__panel_floatingplatform_info_list[list_index]
        --设置平台位置为0
        cur_platform_info.panel_floatingplatform:SetPosition(cur_platform_info.panel_floatingplatform:GetPositionX(),is_refresh and 0 or cur_platform_info.pos)
        CPPActionManager.StopAllActions(cur_platform_info.panel_floatingplatform)
        --不是刷新
        if not is_refresh then
            --悬浮台循环悬浮
            --挑战对手死亡则不悬浮
            if challenge_info.is_dead < Macros.Global.TRUE then
                self:PanelFloatingPlatformLoopMoving(cur_platform_info.panel_floatingplatform,true)
            end
        end
        --模型
        local config_hero = CPPGameLib.GetConfig("Hero",challenge_info.head_img)
        if not cur_platform_info.hero_module then
            cur_platform_info.hero_module = LogicGUI.Model.New({info_id = config_hero.model_id, parent = cur_platform_info.panel_heromodule})               
            cur_platform_info.hero_module:SetPosition(5,0)
        else
            cur_platform_info.hero_module:SetInfoID(config_hero.model_id)
        end
        cur_platform_info.hero_module:SetScaleFactor(config_hero.city_scale)
        --设灰 
        cur_platform_info.hero_module:SetGray(challenge_info.is_dead >= Macros.Global.TRUE)        
        cur_platform_info.hero_module:SetToPlayTime(challenge_info.is_dead >= Macros.Global.TRUE and 0 or nil)
        --是否挑战过标记
        if challenge_info.is_dead >= Macros.Global.TRUE then
            local challenge_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.INTEGRAL_RACE,
                challenge_info.is_dead == 2 and "battle_lose" or "battle_win",true)
            if not challenge_info.challenge_sp then
                cur_platform_info.challenge_sp = Util:Sprite(challenge_sp)           
                cur_platform_info.panel_heromodule:AddChild(cur_platform_info.challenge_sp)
                cur_platform_info.challenge_sp:SetPosition(5,60)
                cur_platform_info.challenge_sp:SetScaleFactor(0.5)
            else
                cur_platform_info.challenge_sp:SetImage(challenge_sp)
            end 
        end
        if cur_platform_info.challenge_sp then
            cur_platform_info.challenge_sp:SetVisible(challenge_info.is_dead >= Macros.Global.TRUE)
        end
        --玩家名字
        if not cur_platform_info.player_name then
            cur_platform_info.player_name = Util:Label("",Macros.TypefaceSize.normal,Macros.Color.btn,0,0,Macros.Color.common_stroke)
            cur_platform_info.panel_floatingplatform:AddChild(cur_platform_info.player_name)
            -- cur_platform_info.player_name:SetPosition(137,289)
            PosTool.CenterBottom(cur_platform_info.player_name,0,289)
        end   
        cur_platform_info.player_name:SetString(CPPGameLib.GetString("integral_player_name",challenge_info.server_id or 0,challenge_info.target_name or "")) 
        --称号
        if challenge_info.title_id ~= 0 then
            local titlesp = Resource.PathTool.GetTitleIconPath("sp_title_"..challenge_info.title_id,true)
            if not cur_platform_info.title_sp then
                cur_platform_info.title_sp = Util:Sprite(titlesp)           
                cur_platform_info.panel_floatingplatform:AddChild(cur_platform_info.title_sp)
                cur_platform_info.title_sp:SetPosition(116.5,335.5)
            else
                cur_platform_info.title_sp:SetImage(titlesp)
            end           
        end
        if cur_platform_info.title_sp then
            cur_platform_info.title_sp:SetVisible(challenge_info.title_id ~= 0)
        end
        --战力
        if not cur_platform_info.power_font then
            cur_platform_info.power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
            cur_platform_info.power_font:SetAnchorPoint(0, 0.5)
            cur_platform_info.panel_battlepower:AddChild(cur_platform_info.power_font:GetNode())
            PosTool.LeftCenter(cur_platform_info.power_font,0,0)
        end
        local battle_value = challenge_info.battle_value
        if battle_value >= 1000000 then
            battle_value = math.floor( battle_value/10000)
            --万
            if not cur_platform_info.myriad_font then
                cur_platform_info.myriad_font = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"myriad",true))
                cur_platform_info.panel_battlepower:AddChild(cur_platform_info.myriad_font)
            end
        end
        cur_platform_info.power_font:SetText(tostring(battle_value))
        --万字
        if cur_platform_info.myriad_font then 
            PosTool.RightTo(cur_platform_info.power_font,cur_platform_info.myriad_font)
            cur_platform_info.myriad_font:SetVisible(challenge_info.battle_value >= 1000000)
        end
        --可获得荣耀
        local honor = self:CallModuleFunc("GetIntergralRaceHonor",challenge_info.battle_value)
        cur_platform_info.txt_rewardintegral_num:SetText(honor)
    end
    if is_refresh then
        --迷雾
        local effectid,effect = self:__CreateEffect({res_name = "UI_pata_miwu"}, self:GetNode(),1000)
        PosTool.Center(effect)   
        self.__panel_floatingplatform_action_tag = 1
        self:PanelFloatingPlatformPlayAction()
    end
end

--悬浮台悬浮
function Modules.IntegralRaceView:PanelFloatingPlatformPlayAction()
    --平台信息
    local cur_platform_info = self.__panel_floatingplatform_info_list[self.__panel_floatingplatform_action_tag]
    local cur_panel = cur_platform_info.panel_floatingplatform
    local cur_action_tag = self.__panel_floatingplatform_action_tag
    local roteateAnimate = function()   
        --悬浮台循环悬浮
        --挑战对手死亡则不悬浮
        local score_info = self.__score_match_data.battle_target_list[cur_action_tag]
        if score_info.is_dead < Macros.Global.TRUE then
            self:PanelFloatingPlatformLoopMoving(cur_panel,true)
        end      
    end  
    local func = GlobalCallbackMgr:AddCallBackFunc(roteateAnimate)   
    CPPActionManager.MoveBy(cur_panel,0.6,0,cur_platform_info.pos,func)
    print("skfb","PanelFloatingPlatformPlayActionPanelFloatingPlatformPlayAction ",self.__panel_floatingplatform_action_tag)
    --下一个悬浮台
    if self.__panel_floatingplatform_action_tag < 3 then
        CPPActionManager.DelayTo(cur_panel,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
            self.__panel_floatingplatform_action_tag = self.__panel_floatingplatform_action_tag + 1
            self:PanelFloatingPlatformPlayAction()
        end))
    end
end

--悬浮台循环悬浮
function Modules.IntegralRaceView:PanelFloatingPlatformLoopMoving(cur_platform,is_first)
    if not cur_platform then
        return
    end
    local roteateAnimate = function()       
        CPPActionManager.MoveBy(cur_platform,2,0,-20,GlobalCallbackMgr:AddCallBackFunc(function ()
            self:PanelFloatingPlatformLoopMoving(cur_platform)
        end))
    end  
    local func = GlobalCallbackMgr:AddCallBackFunc(roteateAnimate)   
    CPPActionManager.MoveBy(cur_platform,is_first and 1 or 2,0,is_first and 10 or 20,func)
end


function Modules.IntegralRaceView:OpenAffirmTipsView(cost_type)
    local score_match_config = CPPGameLib.GetConfig("ScoreMatchTimesCost",cost_type)
    local cur_use_times = cost_type == COST_TYPE.REFRESH_COST and self.__score_match_data.refresh_times or 
                            self.__score_match_data.buy_battle_times

    --购买挑战次数限制
    local buy_limit_times
    if cost_type == COST_TYPE.CHALLENGE_COST then
        buy_limit_times = CallModuleFunc(ModuleType.VIP,"GetPrivilege",Macros.Game.VIPType.INTEGRAL_RACE_BUY_TIME)               
        buy_limit_times = buy_limit_times or 0
        if self.__score_match_data.buy_battle_times >= buy_limit_times then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("integral_cost_limit_tips"))
            return
        end
    end
    local user_diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    local cost_diamond = score_match_config.cost_diamond_list[cur_use_times+1] and score_match_config.cost_diamond_list[cur_use_times+1].cost_diamond
                            or score_match_config.cost_diamond_list[#score_match_config.cost_diamond_list].cost_diamond
    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                    content = " ",
                                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                    confirm_func = function ( ... )
                                                        if cost_diamond <= user_diamond then
                                                            if cost_type == COST_TYPE.REFRESH_COST then
                                                                --刷新挑战对手
                                                                self:CallModuleFunc("ScoreMatchRefreshTargetReq")
                                                            else
                                                                --购买挑战次数请求
                                                                self:CallModuleFunc("ScoreMatchBuyBattleTimesReq")
                                                            end
                                                        else
                                                            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("currency_insufficient_"..Macros.Game.RolePropType.DIAMOND))
                                                        end
                                                    end,
                                                } )
    tips:SetLoadedCallback( function ( ... )
        local text = 
            {
                { zi = CPPGameLib.GetString(cost_type == COST_TYPE.REFRESH_COST and "integral_refresh_cost" or "integral_add_challenge_cost",cost_diamond), ys = Macros.Color.content,dx = Macros.TypefaceSize.largish,jg = -150},
                { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND),dx = 0.8,jg = -185},
            }
        local Affirm = Util:LabCenter(text)
        Affirm:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 40 )
        tips:GetNode():AddChild(Affirm)

        --剩余次数
        if cost_type == COST_TYPE.CHALLENGE_COST then            
            local arena_today_buyTimes = Util:Label(CPPGameLib.GetString("integral_cost_limit",buy_limit_times - self.__score_match_data.buy_battle_times) , Macros.TypefaceSize.normal , Macros.Color.red )
            arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
            arena_today_buyTimes:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 40 )
            tips:GetNode():AddChild(arena_today_buyTimes)
        end       
    end)
end

