--[[
%% @module: 扭蛋界面
%% @author: ldx
%% @created: 2018-02-24
--]]

Modules = Modules or {}

Modules.GashaponMachineView = Modules.GashaponMachineView or BaseClass(BaseView)

function Modules.GashaponMachineView:__init()
    self.__layout_name = "gashaponmachine" 
    self.__bg_params = {type = BaseView.BGType.SOLID}

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.GASHAPON_MACHINE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.GASHAPON_MACHINE,true)
end

function Modules.GashaponMachineView:__Dispose()
    --定时器取消
    if self.__lottery_req_count_down_timer then                
        GlobalTimerQuest:CancelQuest(self.__lottery_req_count_down_timer)
        self.__lottery_req_count_down_timer = nil
    end
    --特效
    if self.__gashaponmachine_ui_effect_id then
        self:__DelEffect(self.__gashaponmachine_ui_effect_id)
        self.__gashaponmachine_ui_effect_id = nil
    end
end

function Modules.GashaponMachineView:__OpenCallback()    
    WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
        --关闭扭蛋机         
        --是否正在请求 
        if not self.__is_on_lottery_req then
            self:CloseManual()
        end
    end)
    --列表容器
    self.__listview_rewardprelook = self:GetWidget("ListView_RewardPreLook")
    --扭蛋机容器
    self.__panel_gashaponmachine = self:GetWidget("Panel_gashaponMachine")   
    --开放时间
    local st_time_lab,ov_time_lab = self:CallModuleFunc("GetActivityOpenSp",Macros.Game.SystemID.GASHAPONMACHINE)
    local txt_time1 = self:GetWidget("Text_time1")     
    Util:WidgetLabel(txt_time1, st_time_lab, Macros.TypefaceSize.normal, Macros.Color.btn)
    local txt_time2 = self:GetWidget("Text_time2")     
    Util:WidgetLabel(txt_time2, ov_time_lab, Macros.TypefaceSize.normal, Macros.Color.btn)
    self:CallModuleFunc("NDLotteryRecordReq")
end

function Modules.GashaponMachineView:__RegistAllEvents()
    --抽奖记录响应
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.ND_LOTTERYRECORD_RESP, function(_,lottery_record_list)
        self:__OpenGashaponMachine(lottery_record_list)
    end)

    --抽奖响应
    self:BindGlobalEvent(Macros.Event.LimitActivityModule.ND_LOTTERY_RESP, function()
        self.__is_on_lottery_req = false
        if self.__btn_gashapon then
            self.__btn_gashapon:SetTouchEnabled(true)
        end
        --更新货币总类
        self:UpdateGashaponCoinType()
    end)
end

--打开扭蛋机
function Modules.GashaponMachineView:__OpenGashaponMachine(lottery_record_list) 
    if not lottery_record_list then
        return
    end
    local tmp_x = 25
    local tmp_y = -11
    --特效
    if self.__gashaponmachine_ui_effect_id then
        local ui_effect = GlobalEffectMgr:GetEffect(self.__gashaponmachine_ui_effect_id)
        if ui_effect then
            ui_effect:SetVisible(true)
        end
    else
        self.__gashaponmachine_ui_effect_id,effect = self:__CreateEffect({res_name = "UI_xinniudanji",loop = true},self.__panel_gashaponmachine,3)
        effect:SetAnchorPoint(0.5,0.5)
        effect:SetZOrder(2)
        PosTool.Center(effect,tmp_x-22,tmp_y+13)
    end
    --创建扭蛋机
    if not self.__gashaponmachine_effect_id then       
        --扭蛋机
        local effect
        self.__gashaponmachine_effect_id, effect = self:__CreateEffect({res_name = "UI_sp_gashaponmachine", type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE},self.__panel_gashaponmachine,3)
        effect:SetAnchorPoint(0.5,0)
        effect:SetZOrder(1)
        PosTool.CenterBottom(effect,tmp_x,tmp_y)
        --屏蔽层
        local shield_layout = Util:Layout(400, 640, function ()
            -- 
        end)
        shield_layout:SetAnchorPoint(0.5,0)
        self.__panel_gashaponmachine:AddChild(shield_layout,0)
        PosTool.CenterBottom(shield_layout,tmp_x,tmp_y)
        --扭蛋按钮
        self.__btn_gashapon = self:GetWidget("Button_gashapon")
        self.__btn_gashapon:SetZOrder(2)
        Util:WidgetButton(self.__btn_gashapon,"",function ()
            --是否已经过期
            local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.GASHAPONMACHINE )
            if info.show ~= "open" then
                --提示
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gashaponmachine_end"))  
                return
            end
            --数据消耗id
            local config_cost = CPPGameLib.GetConfig("NDLotteryCost",info.subtype)
            --是否扭10次
            local is_ten_times = self.__checkbox_coin:GetSelectedState()
            local gashapon_times = is_ten_times and 10 or 1
            local is_cost_item = 1
            local coin_num = self:CallModuleFunc("GetNDLotteryCoinNum")
            local is_buy_pass = false
            local judge_func = function (diamonds,cost_diamonds,coin_req)
                if not coin_req then
                    --判断钻石是否足够
                    if diamonds >= cost_diamonds then
                        is_buy_pass = true
                        is_cost_item = 0
                    else
                        LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("team_gashapon_no_diamonds_tips")
                            ,confirm_func = function ()
                                --判断钻石是否足够
                                GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.CHARGE)
                            end,})
                    end
                    --提示
                    if not is_buy_pass then                                
                        return 
                    end
                end
                self.__is_on_lottery_req = true
                self.__btn_gashapon:SetVisible(false)
                self.__btn_gashapon:SetTouchEnabled(false)
                --扭蛋机待机特效
                if self.__gashaponmachine_ui_effect_id then
                    local ui_effect = GlobalEffectMgr:GetEffect(self.__gashaponmachine_ui_effect_id)
                    if ui_effect then
                        ui_effect:SetVisible(false)
                    end
                end
                --动作
                GlobalEffectMgr:SetEffectAnim(self.__gashaponmachine_effect_id,is_ten_times and RenderObj.ActionIndex.SP_SKILL or RenderObj.ActionIndex.ATTACK)
                --延时播放
                CPPActionManager.DelayTo(self.__btn_gashapon,1.5,GlobalCallbackMgr:AddCallBackFunc(function()
                    GlobalEffectMgr:SetEffectAnim(self.__gashaponmachine_effect_id,RenderObj.ActionIndex.IDLE)
                    --扭蛋机待机特效
                    if self.__gashaponmachine_ui_effect_id then
                        local ui_effect = GlobalEffectMgr:GetEffect(self.__gashaponmachine_ui_effect_id)
                        if ui_effect then
                            ui_effect:SetVisible(true)
                        end                    
                    end
                    self.__btn_gashapon:SetVisible(true)
                    --请求扭蛋
                    self:CallModuleFunc("NDLotteryReq",{times = gashapon_times,is_cost_item = is_cost_item})
                end)) 
            end
            if gashapon_times > coin_num then   
                if config_cost.nd_onetime_cost then                           
                    --消耗钻石
                    local cost_diamonds = is_ten_times and config_cost.nd_tentime_cost or config_cost.nd_onetime_cost
                    --当前钻石
                    local diamonds = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
                    --获取是否显示提示
                    local is_show = self:CallModuleFunc("GetIsShowGashaponCostDiamondsTips")
                    if is_show then
                        --扭蛋币不足则消耗钻石
                        LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("team_gashapon_cost_diamonds_tips",cost_diamonds)
                            ,confirm_func = function (is_hide)
                                self:CallModuleFunc("SetIsShowGashaponCostDiamondsTips",is_hide)
                                judge_func(diamonds,cost_diamonds)
                        end,show_checkbox_content = CPPGameLib.GetString("common_no_show")})
                    else
                        judge_func(diamonds,cost_diamonds)
                    end  
                else
                    --提示
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_gashapon_coins_no_enough"))                
                end
            else
                judge_func(nil,nil,true)
            end                              
        end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.gashaponmachine_stroke )   
        --扭蛋提示文字
        self:GetWidget("Image_coinTxt"):SetZOrder(2)
        --复选框       
        self.__checkbox_coin = self:GetWidget("CheckBox_coin")
        self.__checkbox_coin:SetSelectedState(false)
        self.__checkbox_coin:SetZOrder(2)
        WidgetUnity.SetCheckboxListener(self.__checkbox_coin,function ()
            -- 10次 self.__text_diamondnum:SetText(10)
        end, function ()
            -- 1次  self.__text_diamondnum:SetText(1)
        end)
        --投10个币
        self.__text_coinnum = self:GetWidget("Text_coinNum")
        self.__text_coinnum:SetZOrder(2)
        Util:WidgetLabel(self.__text_coinnum,CPPGameLib.GetString("team_gashapon_ten_times"), Macros.TypefaceSize.normal, Macros.Color.btn)
        --货币类型
        self.__image_type = self:GetWidget("Image_type")  
        self.__image_type:IgnoreContentAdaptWithSize(true)
        self.__image_type:SetZOrder(2)
        self.__image_type:SetScaleFactor(0.6)
        --货币数量        
        self.__text_diamondnum = self:GetWidget("Text_diamondNum")
        self.__text_diamondnum:SetZOrder(2)
        Util:WidgetLabel(self.__text_diamondnum,"", Macros.TypefaceSize.normal, Macros.Color.btn)     
        --帮助
        Util:WidgetButton(self:GetWidget("Button_help"),"",function ()
            local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
            help:SetLoadedCallback( function ( ... )
               help:SetCell("Rule_GashaponMachine")
            end )
        end)
        --帮助文字
        Util:WidgetLabel(self:GetWidget("Text_help"),CPPGameLib.GetString("team_btn_help"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
        --奖励预览
        local btn_rewardpreLook = self:GetWidget("Button_rewardPreLook")
        btn_rewardpreLook:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_cumulative_recharge"),TextureResType.LOCAL)
        Util:WidgetButton(btn_rewardpreLook,"",function ()
            local coin_num,coin_id,coin_type = self:CallModuleFunc("GetNDLotteryCoinNum")
            if coin_type then
                --打开预览奖励
                LogicGUI.TemplTeamLotteryRewardPreLook.New(coin_type)
            else
                --提示
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gashaponmachine_end"))
            end
        end)
        --奖励预览文字
        Util:WidgetLabel(self:GetWidget("Text_rewardPreLook"),CPPGameLib.GetString("team_btn_rewardpreLook"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
    end   
    --先移除所有所有控件
    self.__listview_rewardprelook:RemoveAllItems()
    --抽奖记录
    for i,lottery_record_info in ipairs(lottery_record_list) do
        --富文本
        local item_type,item_info = ObjIDTool.GetInfo(lottery_record_info.item_id)
        if not item_info then
            return
        end
        local quality_str = string.format(Macros.Quality_RT[item_info.quality or 0],item_info.name)
        local rt_str = CPPGameLib.GetString("team_lottery_record_rt",CPPGameLib.GetString("team_lottery_record_player_name",lottery_record_info.server_id,
                lottery_record_info.role_name),quality_str,lottery_record_info.item_num)
        local rich_txt = Util:RichText(rt_str, Macros.TypefaceSize.slightly, 280, 0, Macros.Color.btn_hex)
        rich_txt:SetAnchorPoint(0,1)
        --控件
        local layout = Util:Layout(rich_txt:GetContentWidth(), rich_txt:GetContentHeight())
        layout:AddChild(rich_txt)
        PosTool.LeftTop(rich_txt)
        --插入条目
        self.__listview_rewardprelook:PushBackCustomItem(layout)
    end
    --更新货币总类
    self:UpdateGashaponCoinType()
    if not self.__lottery_req_count_down_timer then
        --延时请求
        self.__lottery_req_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
            self.__is_count_down_lottery_req = true
            self:CallModuleFunc("NDLotteryRecordReq")
        end), 3, -1)
    end
    --先整理
    self.__listview_rewardprelook:ForceDoLayout()
    if not self.__is_count_down_lottery_req then
        --移动
        local inner_posx,inner_posy = self.__listview_rewardprelook:GetInnerContainerPos()
        self.__listview_rewardprelook:StartAutoScrollToDestination(COCOPoint(inner_posx,0),3,false)
    else
        self.__listview_rewardprelook:ScrollToBottom()
    end
    self.__is_count_down_lottery_req = false
end

--更新货币总类
function Modules.GashaponMachineView:UpdateGashaponCoinType()
    --货币数量 
    local coin_num,coin_id,data_id = self:CallModuleFunc("GetNDLotteryCoinNum")
    --数据消耗id
    local config_cost = CPPGameLib.GetConfig("NDLotteryCost",data_id)
    if not coin_id then
        for _,coin_info in pairs(CPPGameLib.GetConfig("NDLotteryCost"),nil,nil,true) do
            coin_id = coin_info.cost_item_id
            break
        end           
    end
    local type,config
    --货币类型
    if coin_num <= 0 and config_cost.nd_onetime_cost then
        local huobi_info = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.DIAMOND)
        type,config = ObjIDTool.GetInfo(huobi_info.big_icon)
    end 
    local path = Resource.PathTool.GetItemPath(coin_num <= 0 and config and config.res_id or coin_id)       
    self.__image_type:SetImage(path,TextureResType.LOCAL)
    if coin_num <= 0 and config_cost.nd_onetime_cost then
        coin_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    end
    self.__text_diamondnum:SetText(string.format("X%d",coin_num))
end