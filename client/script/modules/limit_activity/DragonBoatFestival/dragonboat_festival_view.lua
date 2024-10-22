Modules = Modules or {}

Modules.DragonBoatFestivalView = Modules.DragonBoatFestivalView or BaseClass(BaseView)

local TAB_TYPE = {
	DRIVE_MONSTER = 1,	--驱赶年兽
	INTEGRAL_CONVERT = 2,	--积分兑换
	PROP_CONVERT = 3,	--道具兑换
}

local BOSS_TYPE = {
	LOW_BOSS = 1,	--低级boss
	HIGH_BOSS = 2,	--高级boss
}

local txt_move_distant = 100
local txt_move_speed = 30

function Modules.DragonBoatFestivalView:__init()
	self.__layout_name = "dragonboat_festival"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_christmas")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("dragonboat_festival_title"),
        close_func = function()
        	--请求离开场景       	
        	self:CallModuleFunc("ChristmasBattleLeaveReq")
        	--清空商店记录
        	self:CallModuleFunc("ClearShopData")
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
    	--请求圣诞节数据
    	self:CallModuleFunc("ChristmasDataReq")
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_SPRINGFESTIVAL_SHOP_CELL)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DRAGONBOAT_FESTIVAL)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DRAGONBOAT_FESTIVAL,true)
end

function Modules.DragonBoatFestivalView:__Dispose()
	for boss_type=1,2 do
		--取消定时器
		if self["__boss_resurgencetimer"..boss_type] then
			GlobalTimerQuest:CancelQuest(self["__boss_resurgencetimer"..boss_type])
			self["__boss_resurgencetimer"..boss_type] = nil
		end
	end	
	--定时器
	for _,timer_event in pairs(self.__timer_event_list) do
		if timer_event then
			GlobalTimerQuest:CancelQuest(timer_event)
		end
	end	
	--滚字
	if self.__face_effects then
		self.__face_effects:DeleteMe()
		self.__face_effects = nil
	end
	if self.__face_effects_cur then
		self.__face_effects_cur:DeleteMe()
		self.__face_effects_cur = nil
	end
	if self.__face_effects_total then
		self.__face_effects_total:DeleteMe()
		self.__face_effects_total = nil
	end
end

function Modules.DragonBoatFestivalView:__RegistAllEvents()
	--错误码
	self:BindGlobalEvent(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, function(_,error_code)
		--扔雪球错误码
		if error_code == 20017 then
			self.__is_on_sending = false
			--相同的func
			self.__same_act_func()
		end
	end)

	--请求圣诞节数据响应 
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.CHRISTMAS_DATA_RESP, function(_,christmas_data)
		self.__christmas_data = christmas_data
		self:__OpenCallback()
	end)

	-- --请求圣诞节数据响应
	-- self:BindGlobalEvent(Macros.Event.LimitActivityModule.CHRISTMAS_BATTLE_ENTER_RESP, function()		
		
	-- end)	

	--攻击Boss响应
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.CHRISTMAS_ATTACK_BOSS_RESP, function(_,attack_boss_data)
		--CPPGameLib.PrintTable("ldx",attack_boss_data,"566666666666666666666666666666 ")
		self.__is_on_sending = false
		local role_name = CallModuleFunc(ModuleType.ROLE,"GetName")
		if not self.__face_effects then
			self.__face_effects = TypefaceEffects.New()
		end	
		--名字	
		if attack_boss_data.attacker_name and attack_boss_data.attacker_name ~= role_name then				
			self:HandleAttackBossResp(attack_boss_data)
		else
			--雪球
			local snowball_config = CPPGameLib.GetConfig("ChristmasSnowBall",self.__cur_snowball_id)
			local snowball_data = {}
			snowball_data.smooth_elapse_time = 0
			snowball_data.random_beg_posx = MathTool.GetRandom(snowball_config.snowball_start_area[1], snowball_config.snowball_start_area[2])
			snowball_data.random_beg_posy = MathTool.GetRandom(snowball_config.snowball_start_area[3], snowball_config.snowball_start_area[4])
			snowball_data.random_tgt_posx = MathTool.GetRandom(snowball_config.snowball_over_area[1], snowball_config.snowball_over_area[2])
			snowball_data.random_tgt_posy = MathTool.GetRandom(snowball_config.snowball_over_area[3], snowball_config.snowball_over_area[4])
			snowball_data.random_fly_time = MathTool.GetRandom(300,500)/1000		
			--容器
			local panel = self:GetWidget(attack_boss_data.boss_type == BOSS_TYPE.LOW_BOSS and "Panel_LowBoss" or "Panel_HighBoss")
			--雪球
			local config = CPPGameLib.GetConfig("Item",self.__cur_snowball_id)
			local sp = Resource.PathTool.GetItemPath(config.res_id)
			snowball_data.snow_ball = Util:Sprite(sp)
			panel:AddChild(snowball_data.snow_ball)
			snowball_data.snow_ball:SetPosition(snowball_data.random_beg_posx,snowball_data.random_beg_posy)
			table.insert(self.__snow_ball_list,snowball_data)
			print("ldx","UI_firecrackerUI_firecrackerUI_firecracker UI_firecrackerUI_firecracker ")
			--延迟处理
			self.__timer_event_count_num = self.__timer_event_count_num or 0
			self.__timer_event_count_num = self.__timer_event_count_num + 1
			local timer_event_count_num = self.__timer_event_count_num
			self.__timer_event_list[timer_event_count_num] = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
				function()
				--雪球击中特效				
				local effect_id,effect = self:__CreateEffect({res_name = "YX033_heji1",loop = false},panel,10)
				effect:SetPosition(snowball_data.random_tgt_posx,snowball_data.random_tgt_posy)
				--自己扔
				self:HandleAttackBossResp(attack_boss_data,{x = snowball_data.random_tgt_posx,y = snowball_data.random_tgt_posy})
				--定时器
				if self.__timer_event_list[timer_event_count_num] then
					GlobalTimerQuest:CancelQuest(self.__timer_event_list[timer_event_count_num])
					self.__timer_event_list[timer_event_count_num] = nil
				end
			end), snowball_data.random_fly_time)	
			--连续扔
			if self.__is_on_touching then
				CPPActionManager.DelayTo(self.__txt_curfirecrackerintegralnum,0.1, GlobalCallbackMgr:AddCallBackFunc(function()
					self:CheckIsSatisfyConditional()
				end))
			end
		end
	end)

	--请求商店数据响应
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.GET_SHOPDATA_BY_SHOPTYPE, function(_,shop_data,integral)
		self:HandleChristmasShop(shop_data)
		if integral then
			local old_value = self.__christmas_data.cur_score
			--更新积分
			self.__christmas_data.cur_score = self.__christmas_data.cur_score - integral
			--当前积分
			if not self.__face_effects_cur then
				self.__face_effects_cur = TypefaceEffects.New()
			end
			self.__face_effects_cur:RollEx(self.__txt_curfirecrackerintegralnum,old_value,self.__christmas_data.cur_score,0.2,nil,function (cur_value)				
				return cur_value..CPPGameLib.GetString("christmas_cur_integral_roll")
			end)
		end
		--self.__txt_curfirecrackerintegralnum:SetText(CPPGameLib.GetString("christmas_cur_integral",self.__christmas_data.cur_score or 0))
	end)	

	--Boss死亡通知
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.CHRISTMAS_BOSSDEATH_NOTIFY, function(_,boss_type)
		--设置宝箱显隐
		self:SetBoxImageVisible()
		self["__is_boss_alive"..boss_type] = false
		self.__is_on_sending = false
		--相同的func
		self.__same_act_func()
		--处理BOSS死亡
		self.__is_handle_boss_dead = true
	end)

	--购买物品通知
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.CHRISTMAS_SHOP_BUY_NOTIFY, function()		
		local christmashat_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetChristmasHatId())
		self.__txt_lanternnum:SetText(christmashat_num)
	end)	

	--领取奖励响应
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.GET_BOSS_REWARD_RESP, function()
		--设置宝箱显隐
		self:SetBoxImageVisible()
	end)
end

function Modules.DragonBoatFestivalView:__OpenCallback()
	self.__snow_ball_list = {}
	self.__timer_event_list = {}
	--设置定时器
	self:SetUpdateFunc(function (elapse_time)
		local del_list
		for i,snowball_data in ipairs(self.__snow_ball_list) do
			print("ldx","snowball_data.effect_idsnowball_data.effect_idyyyyyyyyyyyyyyyyyyyyyy ")
			--时间进度
	        snowball_data.smooth_elapse_time = snowball_data.smooth_elapse_time + elapse_time
	        local x = (snowball_data.random_tgt_posx - snowball_data.random_beg_posx)*snowball_data.smooth_elapse_time/snowball_data.random_fly_time+snowball_data.random_beg_posx
	        local y = (snowball_data.random_beg_posy - snowball_data.random_tgt_posy)*snowball_data.smooth_elapse_time/snowball_data.random_fly_time*(snowball_data.smooth_elapse_time/snowball_data.random_fly_time - 2) + snowball_data.random_beg_posy
	        if snowball_data.snow_ball then
        		print("ldx","snowball_data.effect_idsnowball_data.effect_id111111111111111 ",2*(snowball_data.random_fly_time - snowball_data.smooth_elapse_time) + 0.5,x,y)
		        snowball_data.snow_ball:SetScaleFactor(2*(snowball_data.random_fly_time - snowball_data.smooth_elapse_time) + 0.5)
		       	print("ldx","snowball_data.effect_idsnowball_data.effect_idkkkkkkkkkkkkkk ")
		        snowball_data.snow_ball:SetPosition(x,y)
		    end
	        if snowball_data.smooth_elapse_time >= snowball_data.random_fly_time then
	        	print("ldx","snowball_data.effect_idsnowball_data.effect_idrrrrrrrrrrrrrrrrrr ")
	        	if snowball_data.snow_ball then
	        		snowball_data.snow_ball:RemoveSelf(true)
	        		snowball_data.snow_ball = nil
	        	end
	        	del_list = del_list or {}
	        	table.insert(del_list,i)
	        	-- self.__snow_ball_list[i] = nil
	        end
		end		
		if del_list then
			table.sort(del_list, SortTools.ItemUpperSorter())
			for _, i in ipairs(del_list) do
				table.remove(self.__snow_ball_list,i)
			end
		end
	end)
	--累计积分	
	--雪球积分
	Util:WidgetLabel(self:GetWidget("Text_FirecrackerIntegral"),CPPGameLib.GetString("dragonboat_festival_firecrackerintegral"), Macros.TypefaceSize.normal, Macros.Color.Special)
	--数字
	self.__txt_totalfirecrackerintegralnum = self:GetWidget("Text_TotalFirecrackerIntegralNum")
	Util:WidgetLabel(self.__txt_totalfirecrackerintegralnum, "", Macros.TypefaceSize.normal, Macros.Color.btn)

	--当前积分
	--数字
	self.__txt_curfirecrackerintegralnum = self:GetWidget("Text_CurFirecrackerIntegralNum")
	Util:WidgetLabel(self.__txt_curfirecrackerintegralnum, "", Macros.TypefaceSize.normal, Macros.Color.btn)

	--结束时间
	--文字
	self.__txt_endtime = self:GetWidget("Text_EndTime")
	Util:WidgetLabel(self.__txt_endtime,CPPGameLib.GetString("christmas_end_time_txt"), Macros.TypefaceSize.normal, Macros.Color.btn)
	--数字
	local is_over,text = self:CallModuleFunc("GetActivityOpen",Macros.Game.SystemID.CHRISTMAS_ACTIVITY)
	self.__txt_endtimenum = self:GetWidget("Text_EndTimeNum")
	Util:WidgetLabel(self.__txt_endtimenum, text, Macros.TypefaceSize.normal, Macros.Color.btn)
	
	--规则
	self.__btn_rule = self:GetWidget("Button_rule")
	Util:WidgetButton(self.__btn_rule,"",function ()
	    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
	    help:SetLoadedCallback( function ( ... )
	       help:SetCell("Rule_Christmas")
	    end )
	end)

	--遍历获取控件
	local panel_name_list = {"Panel_DriveMonster","Panel_IntegralConvert","Panel_PropConvert"}
	self.__panel_list = {}
	for _,panel_name in ipairs(panel_name_list) do
		local panel = self:GetWidget(panel_name)
		table.insert(self.__panel_list,panel)
	end

	--寻找对应的雪球
	self.__firecracker_id_list = {}
	for firecracker_id,_ in pairs(CPPGameLib.GetConfig("ChristmasSnowBall",nil,nil,true)) do
		table.insert(self.__firecracker_id_list,firecracker_id)
	end
	table.sort(self.__firecracker_id_list,function (firecracker_id1,firecracker_id2)
		return firecracker_id1 < firecracker_id2
	end)
	--滚动容器数据
	self.__scrollview_data_list = {[TAB_TYPE.INTEGRAL_CONVERT] = {name = "__integral_scrollview",shop_type = Macros.Game.SHOP.CHRISTMAS_INTEGRAL},[TAB_TYPE.PROP_CONVERT] = {name = "__prop_scrollview",shop_type = Macros.Game.SHOP.CHRISTMAS_PROP}}
	--雪球类型队列
	self.__snow_ball_type_list = {"low","middle","high"}
	--设置Tab
	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DRAGONBOAT_FESTIVAL, "btn_selected")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DRAGONBOAT_FESTIVAL, "btn_unselected")
	self.__tab_params = {
	    {sp_2,sp_1},
	    {  
	    	"",
            "",
            "",
		},
		20,
		CPPGameLib.Handler(self, self.ChangeTag),
		true
	}
	self.__tab = GUI.FeignCheckBoxBtn.New(unpack(self.__tab_params))
	--选项卡
    local panel_tab = self:GetWidget("Panel_tab")         
    panel_tab:AddChild(self.__tab:GetNode())
    self.__tab:SetDirection(true)
    PosTool.RightTop(self.__tab,0,0)
    --按钮特殊处理
    local tab_sp_name = {"zongzi","best_gift","prop_convert"}
    local tab_sp_txt = {"dragonboat","integral_convert","prop_convert"}
    for i=1,3 do
    	local btn = self.__tab:GetBtn(i)
    	if btn then
    		--图片
    		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DRAGONBOAT_FESTIVAL, tab_sp_name[i])
    		local sprite = Util:Sprite(sp)
    		sprite:SetAnchorPoint(0,0.5)
    		btn:AddChild(sprite)
    		PosTool.LeftCenter(sprite,-8,-3)
    		--文字
    		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DRAGONBOAT_FESTIVAL, tab_sp_txt[i],true)
    		sprite = Util:Sprite(sp)
    		sprite:SetAnchorPoint(1,0.5)
    		btn:AddChild(sprite)
    		PosTool.RightCenter(sprite,-27,-15)
    	end
    end
    --圣诞帽
    local image_lanternicon = self:GetWidget("Image_LanternIcon")
    --图片
    local type,config_item = ObjIDTool.GetInfo(ConfigAdapter.Common.GetChristmasHatId())
	local sp = Resource.PathTool.GetItemPath(config_item.res_id)
    image_lanternicon:IgnoreContentAdaptWithSize(true)
    image_lanternicon:SetImage(sp,TextureResType.LOCAL)
    image_lanternicon:SetScaleFactor(0.6)
    image_lanternicon:SetPosition(image_lanternicon:GetPositionX(),image_lanternicon:GetPositionY()+2)
    --圣诞帽数量
    self.__txt_lanternnum = self:GetWidget("Text_LanternNum")
    local christmashat_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetChristmasHatId())
	Util:WidgetLabel(self.__txt_lanternnum,christmashat_num, Macros.TypefaceSize.largish, Macros.Color.btn)
    --文字容器table
    self.__txt_panel_table1 = {}
    self.__txt_panel_table2 = {}
    self.__tab:On(1)      --默认显示第一个选项卡的
    --进入战斗请求
    self:CallModuleFunc("ChristmasBattleEnterReq")
end

function Modules.DragonBoatFestivalView:ChangeTag(index)
	--设置控件显隐
	for i,panel in ipairs(self.__panel_list) do
		panel:SetVisible(i == index)
	end
	self.__seleceted_index = index
	--根据类型选择界面
	if index == TAB_TYPE.DRIVE_MONSTER then
		--雪球大战
		self:HandleSnowBattle()
	elseif index == TAB_TYPE.INTEGRAL_CONVERT then
		--积分兑换		
		self:CallModuleFunc("GetShopDataByShopType",Macros.Game.SHOP.CHRISTMAS_INTEGRAL)
	elseif index == TAB_TYPE.PROP_CONVERT then
		--道具兑换
		self:CallModuleFunc("GetShopDataByShopType",Macros.Game.SHOP.CHRISTMAS_PROP)
	end
end

--设置攻击Boss后的数据显示
function Modules.DragonBoatFestivalView:HandleAfterAttackBossData(dead_boss_type)
	if self.__face_effects then
		self.__face_effects:StopAllRolling()
		--self.__face_effects = nil
	end
	--累计积分
	self.__txt_totalfirecrackerintegralnum:SetText(CPPGameLib.GetString("christmas_total_integral",self.__christmas_data.total_score or 0))
	--当前积分
	self.__txt_curfirecrackerintegralnum:SetText(CPPGameLib.GetString("christmas_cur_integral",self.__christmas_data.cur_score or 0))
	--BOSS相关信息
	for boss_type=1,2 do
		local txt_boss_name = boss_type == BOSS_TYPE.LOW_BOSS and self.__txt_lowbossname or self.__txt_highbossname
		local boss_death_time = boss_type == BOSS_TYPE.LOW_BOSS and self.__christmas_data.lower_boss_death_time or self.__christmas_data.high_boss_death_time
		local boss_level = boss_type == BOSS_TYPE.LOW_BOSS and self.__christmas_data.lower_boss_level or self.__christmas_data.high_boss_level
		--BOSS配置
		local boss_config = CPPGameLib.GetConfig("ChristmasBoss",boss_type)
		--BOSS等级配置
		local boss_level_config = CPPGameLib.GetConfig("ChristmasBossLevel",boss_type)
		--BOSS名字
		txt_boss_name:SetText(CPPGameLib.GetString("christmas_boss_name",boss_config.name or "",boss_level))
		local is_alive = true
		if boss_death_time > 0 and boss_config.revive_time > 0 then
			local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
			if (severTime - boss_death_time) < boss_config.revive_time then
				is_alive = false
				self:BossResurgenceCountDown(boss_type)
			end			
		end	
		if dead_boss_type and not is_alive and dead_boss_type == boss_type then
			--BOSS容器
			local panel_bossmodule = boss_type == BOSS_TYPE.LOW_BOSS and self:GetWidget("Panel_LowBossModule") or self:GetWidget("Panel_HighBossModule")
			--BOSS击杀
			local effect_id,effect = self:__CreateEffect({id = 100100},panel_bossmodule,500)
			effect:SetPosition(5,55)
			--播放死亡动作
			self["__boss_module"..boss_type]:PlayAnimation(RenderObj.ActionIndex.DEAD, false)
			--模型
			CPPActionManager.DelayTo(self.__btn_rule,1.7, GlobalCallbackMgr:AddCallBackFunc(function()
				self["__boss_module"..boss_type]:SetGray(not is_alive)
				self["__boss_module"..boss_type]:SetToPlayTime(not is_alive and 0 or nil)
			end))			
		else
			--模型
			self["__boss_module"..boss_type]:SetGray(not is_alive)
			self["__boss_module"..boss_type]:SetToPlayTime(not is_alive and 0 or nil)
		end		
		--BOSS血条
		local total_hp = boss_level_config.boss_level_list[boss_level] and boss_level_config.boss_level_list[boss_level].boss_hp or 1
		self.__christmas_data["total_hp_"..boss_type] = total_hp
		local cur_hp = boss_type == BOSS_TYPE.LOW_BOSS and self.__christmas_data.lower_boss_hp or self.__christmas_data.high_boss_hp		
		self["__loadingbar"..boss_type]:SetPercent(math.min(1,cur_hp/total_hp)*100)
		--BOSS血量进度
		self["__txt_progressnum"..boss_type]:SetText(CPPGameLib.GetString("spirit_numcost",cur_hp,total_hp))
		--BOSS血条
		self["__image_loadingbarbg"..boss_type]:SetVisible(is_alive)
		--复活倒计时
		self["__txt_resurgence"..boss_type]:SetVisible(not is_alive)
		--是否存活
		self["__is_boss_alive"..boss_type] = is_alive
	end
end

--冰雪大战
function Modules.DragonBoatFestivalView:HandleSnowBattle()
	if not self.__snow_battle_init then
		--相同的func
		self.__same_act_func = function ()		
			--设置各个雪球是否能点击
			self:SetSnowBallTouchEnabled()	
			CPPActionManager.StopAllActions(self.__txt_curfirecrackerintegralnum)
			if self.__delay_timer then
				GlobalTimerQuest:CancelQuest(self.__delay_timer)
				self.__delay_timer = nil
			end	
			self.__is_on_touching = false
		end
		self.__snow_battle_init = true
		--低级BOSS配置
		local low_config = CPPGameLib.GetConfig("ChristmasBoss",BOSS_TYPE.LOW_BOSS)
		local hero_config = CPPGameLib.GetConfig("Hero",low_config.model)
		--低级BOSS容器
		local panel_lowbossmodule = self:GetWidget("Panel_LowBossModule")
		--低级BOSS模型
		self.__boss_module1 = LogicGUI.Model.New({info_id = low_config.model, parent = panel_lowbossmodule})	
		self.__boss_module1:SetScaleFactor(low_config.scale_factor or 1)
		self.__boss_module1:SetPosition(5,0)
		--低级BOSS宝箱
		self.__btn_lowbox = self:GetWidget("Button_LowBox")
		Util:WidgetButton(self.__btn_lowbox,"",function ()
			--低级BOSS奖励
			LogicGUI.TemplChristmasBossRewardView.New(BOSS_TYPE.LOW_BOSS,
				self.__christmas_data.lower_boss_level,self.__christmas_data.already_take_lower_boss_reward_level)
		end)
		--宝箱
		local effect
		local panel_lowboss = self:GetWidget("Panel_LowBoss")
		self.__starbox_specialeffect_low, effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"}, panel_lowboss,10)
		effect:SetPosition(270,157)
		self.__starbox_receiveeffect_low, effect = self:__CreateEffect({res_name = "UI_silverybox", type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE2}, panel_lowboss,11)
		effect:SetPosition(270,130)
		--低级BOSS名字
		self.__txt_lowbossname = self:GetWidget("Text_LowBossName")
		Util:WidgetLabel(self.__txt_lowbossname,"", Macros.TypefaceSize.largish, Macros.Color.purple, 0, 0, Macros.Color.purple_stroke)
		--低级BOSS复活时间
		self.__txt_resurgence1 = self:GetWidget("Text_lowresurgence")
		Util:WidgetLabel(self.__txt_resurgence1, "", Macros.TypefaceSize.normal, Macros.Color.red, 0, 0, Macros.Color.red_stroke)	
		--低级进度背景
		self.__image_loadingbarbg1 = self:GetWidget("Image_lowloadingbarbg")
		--低级BOSS血条
		self.__loadingbar1 = self:GetWidget("LoadingBar_Low")
		--低级BOSS血量进度
		self.__txt_progressnum1 = self:GetWidget("Text_LowProgressNum")
		Util:WidgetLabel(self.__txt_progressnum1, "", Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.GrayBlack_stroke)
		--低级
		self.__panel_iconlow = self:GetWidget("Panel_IconLow")
		--中级
		self.__panel_iconmiddle = self:GetWidget("Panel_IconMiddle")
		---------------------------------------------------------------------------------------------------------
		--高级BOSS配置
		local high_config = CPPGameLib.GetConfig("ChristmasBoss",BOSS_TYPE.HIGH_BOSS)
		local hero_config = CPPGameLib.GetConfig("Hero",high_config.model)
		--高级BOSS容器
		local panel_highbossmodule = self:GetWidget("Panel_HighBossModule")
		--高级BOSS模型
		self.__boss_module2 = LogicGUI.Model.New({info_id = high_config.model, parent = panel_highbossmodule})	
		self.__boss_module2:SetScaleFactor(high_config.scale_factor or 0.7)
		self.__boss_module2:SetPosition(5,0)
		--高级BOSS宝箱
		self.__btn_highbox = self:GetWidget("Button_HighBox")
		Util:WidgetButton(self.__btn_highbox,"",function ()
			--高级BOSS奖励
			LogicGUI.TemplChristmasBossRewardView.New(BOSS_TYPE.HIGH_BOSS,
				self.__christmas_data.high_boss_level,self.__christmas_data.already_take_high_boss_reward_level)
		end)
		--高级BOSS名字
		self.__txt_highbossname = self:GetWidget("Text_HighBossName")
		Util:WidgetLabel(self.__txt_highbossname,"", Macros.TypefaceSize.largish, Macros.Quality[hero_config.init_quality], 0, 0, Macros.Quality_Stroke[hero_config.init_quality])
		--高级BOSS复活时间
		self.__txt_resurgence2 = self:GetWidget("Text_highresurgence")
		Util:WidgetLabel(self.__txt_resurgence2, "", Macros.TypefaceSize.normal, Macros.Color.red, 0, 0, Macros.Color.red_stroke)		
		--高级进度背景
		self.__image_loadingbarbg2 = self:GetWidget("Image_highloadingbarbg")
		--高级BOSS血条
		self.__loadingbar2 = self:GetWidget("LoadingBar_High")
		--高级BOSS血量进度
		self.__txt_progressnum2 = self:GetWidget("Text_HighProgressNum")
		Util:WidgetLabel(self.__txt_progressnum2, "", Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.GrayBlack_stroke)
		--宝箱
		local panel_highboss = self:GetWidget("Panel_HighBoss")
		self.__starbox_specialeffect_high, effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"}, panel_highboss,10)
		effect:SetPosition(270,157)
		self.__starbox_receiveeffect_high, effect = self:__CreateEffect({res_name = "UI_red_box", type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE2}, panel_highboss,11)
		effect:SetPosition(270,130)
		--消耗道具
		--高级
		self.__panel_iconhigh = self:GetWidget("Panel_IconHigh")		
		--设置扔雪球
		for i,name in ipairs(self.__snow_ball_type_list) do
			WidgetUnity.SetWidgetListener(self["__panel_icon"..name], 
			function(x,y,in_cd)			
				if not in_cd then
					--判断活动是否已经结束
					if not self:CallModuleFunc("GetActivityOpen",Macros.Game.SystemID.CHRISTMAS_ACTIVITY) then
						GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_activity_over"))
						if not self.__is_check_open then
							CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.DRAGONBOAT_FESTIVAL)
							self.__is_check_open = true
						end
						return
					end
					if in_cd then
						self.__same_act_func()
					end
					self.__is_on_touching = false 
					if self:CheckIsSatisfyConditional(i,i <= 2 and BOSS_TYPE.LOW_BOSS or BOSS_TYPE.HIGH_BOSS,self.__firecracker_id_list[i]) then
						self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
					    	self.__delay_timer = nil
					    	if self:CheckIsSatisfyConditional(i,i <= 2 and BOSS_TYPE.LOW_BOSS or BOSS_TYPE.HIGH_BOSS,self.__firecracker_id_list[i]) then
						    	self.__is_on_touching = true 				    	
						    end
					    end), 0.5)  
				    end		
			    end	
			end, nil, 
			function()
				self.__same_act_func()
			end,function()				
				self.__same_act_func()	
			end,100,0,true)
		end		
		--设置宝箱显隐
		self:SetBoxImageVisible()
		--设置各个雪球的数量
		self:SetSnowBallNum()
		--设置数据
		self:HandleAfterAttackBossData()
	end
end

--检测是否有足够的雪球消耗
function Modules.DragonBoatFestivalView:CheckIsSatisfyConditional(index,boss_type,snow_ball_id)
	boss_type = boss_type or self.__cur_boss_type
	snow_ball_id = snow_ball_id or self.__cur_snowball_id
	--是否存活		
	if not self["__is_boss_alive"..boss_type] then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_boss_resurgence_tips"))
		return
	end
	local snow_ball_num = ObjIDTool.GetOwnNum(snow_ball_id)
	if snow_ball_num <= 0 then
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, snow_ball_id)
		--GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_snowball_noenough_tips"))
		return 
	else
		if not self.__is_on_sending then
			--设置各个雪球是否能点击
			if index then
				self:SetSnowBallTouchEnabled(index)
			end
			self.__cur_boss_type = boss_type
	    	self.__cur_snowball_id = snow_ball_id
	    	self.__is_on_sending = true
	    	--请求消息
	    	self:CallModuleFunc("ChristmasAttackBossReq",{boss_type = self.__cur_boss_type,snowball_id = self.__cur_snowball_id})
		end
	end
	return true
end

--设置宝箱显隐
function Modules.DragonBoatFestivalView:SetBoxImageVisible()
	--低级宝箱
	local is_reward = (self.__christmas_data.lower_boss_level - self.__christmas_data.already_take_lower_boss_reward_level) >= 2
	self.__btn_lowbox:SetOpacity(is_reward and 0 or 255)
	local effect_root = GlobalEffectMgr:GetEffect(self.__starbox_specialeffect_low)
	if effect_root then
		effect_root:SetVisible(is_reward)
	end
	effect_root = GlobalEffectMgr:GetEffect(self.__starbox_receiveeffect_low)
	if effect_root then
		effect_root:SetVisible(is_reward)
	end
	--高级宝箱
	is_reward = (self.__christmas_data.high_boss_level - self.__christmas_data.already_take_high_boss_reward_level) >= 2
	self.__btn_highbox:SetOpacity(is_reward and 0 or 255)
	effect_root = GlobalEffectMgr:GetEffect(self.__starbox_specialeffect_high)
	if effect_root then
		effect_root:SetVisible(is_reward)
	end
	effect_root = GlobalEffectMgr:GetEffect(self.__starbox_receiveeffect_high)
	if effect_root then
		effect_root:SetVisible(is_reward)
	end
end

--设置各个雪球的数量
function Modules.DragonBoatFestivalView:SetSnowBallNum()
	--设置扔雪球
	for i,name in ipairs(self.__snow_ball_type_list) do
		--创建图标
		if not self["__icon_"..name] then
			self["__icon_"..name] = LogicGUI.IconLayer.New(false,false)
			self["__panel_icon"..name]:AddChild(self["__icon_"..name]:GetNode())
			PosTool.Center(self["__icon_"..name])	
			self["__icon_"..name]:ShowItemName(true)		
		end
		local snow_ball_num = ObjIDTool.GetOwnNum(self.__firecracker_id_list[i])
		self["__icon_"..name]:SetData({id = self.__firecracker_id_list[i],num = snow_ball_num})
		self["__icon_"..name]:ShowItemName(true)
	end
end

--设置各个雪球是否能点击
function Modules.DragonBoatFestivalView:SetSnowBallTouchEnabled(index)
	for i,name in ipairs(self.__snow_ball_type_list) do
		if index and index ~= i then
			self["__panel_icon"..name]:SetTouchEnabled(false)
		elseif index and index == i then
			self["__panel_icon"..name]:SetTouchEnabled(true)
		else
			self["__panel_icon"..name]:SetTouchEnabled(true)
		end
	end
end

--攻击Boss后的回调
function Modules.DragonBoatFestivalView:HandleAttackBossResp(attack_boss_data,pos_data)
	if not self.__is_handle_boss_dead and not self["__is_boss_alive"..attack_boss_data.boss_type] then
		return
	end
	local panel = self:GetWidget(attack_boss_data.boss_type == BOSS_TYPE.LOW_BOSS and "Panel_LowBoss" or "Panel_HighBoss") 
	local height = panel:GetContentHeight()/2 - 20
	local cur_txt_height = 30
	--func
	local sp_func = function ()
		--容器				
		local layout = Util:Layout(100,cur_txt_height)
		layout:SetAnchorPoint(0.5,0)
		panel:AddChild(layout,10)
		--名字
		local label = Util:Label(CPPGameLib.GetString(attack_boss_data.reward_item_id ~= 0 and "christmas_attack_tips_txt2" or "christmas_attack_tips_txt1",not pos_data and attack_boss_data.attacker_name or "",attack_boss_data.damage),Macros.TypefaceSize.normal, Macros.Color.red, 0, 0, Macros.Color.red_stroke)
		label:SetAnchorPoint(0,0.5)
		layout:AddChild(label)				
		--大小
		local width = label:GetContentWidth()
		local icon,label1
		if attack_boss_data.reward_item_id ~= 0 then
			icon = LogicGUI.IconLayer.New(false,false)
			icon:SetAnchorPoint(1,0.5)
			layout:AddChild(icon:GetNode())
			icon:SetData({id = attack_boss_data.reward_item_id})
			icon:SetScaleFactor(0.3)
			--名字
			label1 = Util:Label("X "..attack_boss_data.reward_item_num,Macros.TypefaceSize.normal, Macros.Color.red, 0, 0, Macros.Color.red_stroke)
			label1:SetAnchorPoint(1,0.5)
			layout:AddChild(label1)	
			width = width + icon:GetNode():GetContentWidth()*0.3 + label1:GetContentWidth() + 10
		end
		layout:SetContentWidth(width)
		--设置位置
		if pos_data then
			layout:SetPosition(pos_data.x,pos_data.y)
		else
			PosTool.CenterBottom(layout,0,height)	
		end			
		PosTool.LeftCenter(label)
		if icon then
			PosTool.RightCenter(icon,-label1:GetContentWidth()-5,0)
			PosTool.RightCenter(label1)
		end
		if not pos_data then
			table.insert(self["__txt_panel_table"..attack_boss_data.boss_type],1,layout)
			--全部移动
			local del_list
			for i,layout in ipairs(self["__txt_panel_table"..attack_boss_data.boss_type]) do
				local distant = txt_move_distant + height - layout:GetPositionY()
				ActionManager.GetInstance():RunFadeToAndMoveByEffect(layout, distant/txt_move_speed, 0, distant, 0,function ()
					layout:RemoveSelf(true)
					self["__txt_panel_table"..attack_boss_data.boss_type][i] = nil
					-- del_list = del_list or {}
					-- table.insert(del_list,i)
				end)
			end
			-- if del_list then
			-- 	table.sort(del_list, SortTools.ItemUpperSorter())
			-- 	for _, i in ipairs(del_list) do
			-- 		table.remove(self["__txt_panel_table"..attack_boss_data.boss_type],i)
			-- 	end
			-- end
		else
			ActionManager.GetInstance():RunFadeToAndMoveByEffect(layout, txt_move_distant/txt_move_speed, 0, txt_move_distant, 0,function ()
				layout:RemoveSelf(true)
			end)
		end
	end
	--飘字
	if #self["__txt_panel_table"..attack_boss_data.boss_type] > 0 and not pos_data then
		--先停止运动
		local spr_func = function ()
			local del_list
			for i,layout in ipairs(self["__txt_panel_table"..attack_boss_data.boss_type]) do
				local distant = txt_move_distant + height - layout:GetPositionY()
				if distant <= 0 then
					layout:RemoveSelf(true)
					del_list = del_list or {}
					table.insert(del_list,i)
				else
					CPPActionManager.StopAllActions(layout)
				end			
			end
			if del_list then
				table.sort(del_list, SortTools.ItemUpperSorter())
				for _, i in ipairs(del_list) do
					table.remove(self["__txt_panel_table"..attack_boss_data.boss_type],i)
				end
			end
		end
		spr_func()
		--再判断
		if #self["__txt_panel_table"..attack_boss_data.boss_type] > 0 then
			local layout = self["__txt_panel_table"..attack_boss_data.boss_type][1]
			if layout then
				local distant = cur_txt_height - ((layout:GetPositionY() - height)%cur_txt_height)
				--容器table
				for i,layout in ipairs(self["__txt_panel_table"..attack_boss_data.boss_type]) do
					if layout then
						local opacity = (1 - ((layout:GetPositionY() - height)/txt_move_distant))*255
						layout:SetPosition(layout:GetPositionX(),layout:GetPositionY()+distant)
						layout:SetOpacity(opacity)					
					end
				end
				spr_func()
				sp_func()
			else
				sp_func()
			end
		else
			sp_func()
		end
	else
		sp_func()
	end
	--BOSS血量进度
	local cur_hp 
	if not self.__is_handle_boss_dead then
		cur_hp = attack_boss_data.boss_type == BOSS_TYPE.LOW_BOSS and self.__christmas_data.lower_boss_hp or self.__christmas_data.high_boss_hp
	else
		cur_hp = attack_boss_data.boss_type == BOSS_TYPE.LOW_BOSS and self.__christmas_data.lower_before_boss_hp or self.__christmas_data.high_before_boss_hp
	end			
	local now_hp = math.max(cur_hp - attack_boss_data.damage,0)
	self.__face_effects:RollEx(self["__txt_progressnum"..attack_boss_data.boss_type],cur_hp,now_hp,0.2,nil,function (cur_value)
		return cur_value.."/"..self.__christmas_data["total_hp_"..attack_boss_data.boss_type]
	end)
	--类型
	if not self.__is_handle_boss_dead then
		if attack_boss_data.boss_type == BOSS_TYPE.LOW_BOSS then
			self.__christmas_data.lower_boss_hp = now_hp
		else
			self.__christmas_data.high_boss_hp = now_hp
		end
	end
	self["__loadingbar"..attack_boss_data.boss_type]:SetPercent(math.min(1,now_hp/self.__christmas_data["total_hp_"..attack_boss_data.boss_type])*100)	
	--更新积分
	if pos_data then
		self.__christmas_data.total_score = self.__christmas_data.total_score + attack_boss_data.damage
		self.__christmas_data.cur_score = self.__christmas_data.cur_score + attack_boss_data.damage
		--当前积分
		if not self.__face_effects_cur then
			self.__face_effects_cur = TypefaceEffects.New()
		end	
		self.__face_effects_cur:RollEx(self.__txt_curfirecrackerintegralnum,self.__christmas_data.cur_score - attack_boss_data.damage,self.__christmas_data.cur_score,0.2,nil,function (cur_value)
			return cur_value..CPPGameLib.GetString("christmas_cur_integral_roll")
		end)
		--总积分
		if not self.__face_effects_total then
			self.__face_effects_total = TypefaceEffects.New()
		end	
		self.__face_effects_total:RollEx(self.__txt_totalfirecrackerintegralnum,self.__christmas_data.total_score - attack_boss_data.damage,self.__christmas_data.total_score,0.2,nil,function (cur_value)
			return cur_value..CPPGameLib.GetString("christmas_total_integral_roll")
		end)	
		--圣诞帽
		local christmashat_num = ObjIDTool.GetOwnNum(ConfigAdapter.Common.GetChristmasHatId())
		self.__txt_lanternnum:SetText(christmashat_num)
		--设置各个雪球的数量
		self:SetSnowBallNum()
	end
	--处理BOSS死亡
	if self.__is_handle_boss_dead then
		self:HandleAfterAttackBossData(attack_boss_data.boss_type)		
		self.__is_handle_boss_dead = false
	end
end

--积分兑换
function Modules.DragonBoatFestivalView:HandleChristmasShop(shop_data)
	--积分滚动容器
	if not self[self.__scrollview_data_list[self.__seleceted_index].name] then
		local panel_scrollview = self:GetWidget(self.__seleceted_index == TAB_TYPE.INTEGRAL_CONVERT and "Panel_IntegralScrollView" or "Panel_PropScrollView")
		local params = 
	    {
	        item_class = Modules.TemplSpringFestivalShopItem,
	        item_width = 353,
	        item_height = 110,
	        row = 4,
	        col = 2,
	        item_space_r = 6,
	        item_space_c = 6,
	        --view_width = 712,
	        view_height = 418,       
	    }
	    self[self.__scrollview_data_list[self.__seleceted_index].name] = GUI.ScrollView.New(params)
	    panel_scrollview:AddChild(self[self.__scrollview_data_list[self.__seleceted_index].name]:GetNode())
	    PosTool.Center(self[self.__scrollview_data_list[self.__seleceted_index].name]:GetNode())
	end
	local shop_type = self.__seleceted_index == TAB_TYPE.INTEGRAL_CONVERT and Macros.Game.SHOP.CHRISTMAS_INTEGRAL or Macros.Game.SHOP.CHRISTMAS_PROP
	--添加数据
	for i,shop_info in ipairs(shop_data) do
		shop_info.total_score = self.__christmas_data.total_score
		shop_info.shop_type = shop_type
	end
	self[self.__scrollview_data_list[self.__seleceted_index].name]:SetDataList(shop_data)
end


--复活倒计时
function Modules.DragonBoatFestivalView:BossResurgenceCountDown(boss_type)
	if not self["__boss_resurgencetimer"..boss_type] then
		self["__boss_resurgencetimer"..boss_type] = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
			function()
			self:UpdateCountDownTimeTxt(boss_type)
			end), 1, -1)
	end
	self:UpdateCountDownTimeTxt(boss_type)
end	

--复活倒计时
function Modules.DragonBoatFestivalView:UpdateCountDownTimeTxt(boss_type)
	--BOSS配置
	local boss_config = CPPGameLib.GetConfig("ChristmasBoss",boss_type)
	local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local boss_death_time = boss_type == BOSS_TYPE.LOW_BOSS and self.__christmas_data.lower_boss_death_time or self.__christmas_data.high_boss_death_time
	--判断是否复活
	if severTime - boss_death_time >= boss_config.revive_time then
		self["__image_loadingbarbg"..boss_type]:SetVisible(true)
		self["__txt_resurgence"..boss_type]:SetVisible(false)
		self["__is_boss_alive"..boss_type] = true
		--模型
		self["__boss_module"..boss_type]:SetGray(false)
		self["__boss_module"..boss_type]:SetToPlayTime(nil)
		--取消定时器
		if self["__boss_resurgencetimer"..boss_type] then
			GlobalTimerQuest:CancelQuest(self["__boss_resurgencetimer"..boss_type])
			self["__boss_resurgencetimer"..boss_type] = nil
		end
	else
		self["__txt_resurgence"..boss_type]:SetText(CPPGameLib.GetString("christmas_resurgence_time",TimeTool.TransTimeStamp("HH:MM:SS",boss_config.revive_time - severTime + boss_death_time)))
	end
end
