
--[[
%% @module: 月卡
%% @author: yjg
%% @created: 2017年6月14日21:07:44
--]]

--不用

Modules = Modules or {}

Modules.TemplMember = Modules.TemplMember or BaseClass(BaseTabView)

function Modules.TemplMember:__init()
	-- self.__tab_params_ex = {15 , true}
end

--选项卡界面创建完成回调
function Modules.TemplMember:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplMember:__OpenCallback()
	if not CallModuleFunc(ModuleType.CHARGE , "GetMonthCard") then
		CallModuleFunc(ModuleType.CHARGE , "MonthCardDataReq")
	else
		self:__SetShowUI()
	end

    self:BindGlobalEvent(Macros.Event.ChargeModule.CHARGE_DATA_RESP,function (_, data)
		self:__SetShowUI()
    end)
end

--选项卡界面关闭回调
function Modules.TemplMember:__CloseCallback()
    if self.platinumTips then
    	self.platinumTips:RemoveSelf( true )
    	self.platinumTips = nil
    end

    if self.extremeTips then
    	self.extremeTips:RemoveSelf( true )
    	self.extremeTips = nil
    end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplMember:__Dispose()
    if self.platinumTips then
    	self.platinumTips:RemoveSelf( true )
    	self.platinumTips = nil
    end

    if self.extremeTips then
    	self.extremeTips:RemoveSelf( true )
    	self.extremeTips = nil
    end
end

--获得组件
function Modules.TemplMember:__Getwidget()
	-- 背景
	self.Image_contentLst = self:GetWidget("Image_contentLst") 
    local sp = Resource.PathTool.GetUIResPath("citycommonslices/sp_activity_member")
    self.Image_contentLst:SetImage(sp , TextureResType.LOCAL )
    
	--标题
	self.Image_label = self:GetWidget("Image_label") 
	--标题字
	self.Text_label = self:GetWidget("Text_label") 

	--白金卡
	self.Image_platinum = self:GetWidget("Image_platinum") 
	--白金卡字样
	self.Image_platinum_lab = self:GetWidget("Image_platinum_lab") 
	self.Image_platinum_lab:IgnoreContentAdaptWithSize(true)

	--白金卡提示
	self.Image_platinum_tab = self:GetWidget("Image_platinum_tab") 
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_platinum_tab" ,true)
    self.Image_platinum_tab:SetImage(sp)
    self.Image_platinum_tab:IgnoreContentAdaptWithSize(true)

	--白金卡图标
	self.Panel_platinum_icon = self:GetWidget("Panel_platinum_icon") 
	--白金卡购买标题
	self.Text_platinum_type = self:GetWidget("Text_platinum_type") 
	--白金卡时间
	self.Text_platinum_num = self:GetWidget("Text_platinum_num") 
	--白金卡充值
	self.Button_platinum_recharge = Util:WidgetButton(
														self:GetWidget("Button_platinum_recharge"),
				                                        CPPGameLib.GetString("activity_vipDraw") , 
				                                        CPPGameLib.Handler(self,self.OnButtonPlatinumRecharge),
				                                        Macros.TypefaceSize.popup, nil , Macros.Color.button_red  
				                                       )

	--至尊卡
	self.Image_extreme = self:GetWidget("Image_extreme") 
	if CPPSdkTools.IsInExamine() then
		self.Image_extreme:SetVisible(false)
	end

	--至尊卡字样
	self.Image_extreme_lab = self:GetWidget("Image_extreme_lab") 
	self.Image_extreme_lab:IgnoreContentAdaptWithSize(true)

	--至尊卡提示
	self.Image_extreme_tab = self:GetWidget("Image_extreme_tab") 
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_extreme_lab" ,true)
    self.Image_extreme_tab:SetImage(sp)
    self.Image_extreme_tab:IgnoreContentAdaptWithSize(true)

	--至尊卡图标
	self.Panel_extreme_icon = self:GetWidget("Panel_extreme_icon") 
	--至尊卡购买标题
	self.Text_extreme_type = self:GetWidget("Text_extreme_type") 
	--至尊卡时间
	self.Text_extreme_num = self:GetWidget("Text_extreme_num") 
	--至尊卡充值
	self.Button_extreme_recharge = Util:WidgetButton(
														self:GetWidget("Button_extreme_recharge"),
				                                        CPPGameLib.GetString("activity_vipDraw") , 
				                                        CPPGameLib.Handler(self,self.OnButtonExtremeRecharge),
				                                        Macros.TypefaceSize.popup, nil , Macros.Color.button_red
				                                       )
end

--创建组件
function Modules.TemplMember:__ShowUI()
	--标题
	local monthCard_buyTips = Util:Label( CPPGameLib.GetString("monthCard_buyTips_") , Macros.TypefaceSize.largish , Macros.Color.vipgift )
	self.Text_label:AddChild(monthCard_buyTips)
	PosTool.Center( monthCard_buyTips )

	--标题
	local monthCard_buyTips = Util:Label( CPPGameLib.GetString("monthCard_buyTips") , Macros.TypefaceSize.largish , Macros.Color.white )
	self.Text_label:AddChild(monthCard_buyTips)
	PosTool.Center( monthCard_buyTips , 0 , -450 )

	--月卡表
	local cnf = CPPGameLib.GetConfig("MonthCard", nil, nil , true )
	if not cnf then
		return
	end
	local t1 = {}
	for i , v in pairs( cnf ) do
		v.id = i
		table.insert( t1 , v )
	end

	table.sort( t1, function ( a , b )
		return a.id < b.id
	end )

	--屌丝卡
    local buy_reward_diamond = Modules.ChargeModule.GetMoney(t1[1], "sell_price")

	local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , buy_reward_diamond )

	local tab = {
			        {zi = CPPGameLib.GetString("monthCard_rechargeTips_1") , ys = 0 , dx = Macros.TypefaceSize.largish , mb = true },
			        {zi = text , ys = 1, dx = Macros.TypefaceSize.largish , jg = 4 , mb = true },
			        {zi = CPPGameLib.GetString("monthCard_rechargeTips_2") , ys = 0 , dx = Macros.TypefaceSize.largish , jg = 4 , mb = true },
			    }
    self.platinumTips = Util:LabCenter( tab )  
	self.Text_platinum_type:AddChild( self.platinumTips )
	PosTool.Center( self.platinumTips )

	--持续
	self.platinumContinueTime = Util:Label( CPPGameLib.GetString("monthCard_continueTime" , ConfigAdapter.Common.GetMonthCardContinue() ) , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	self.Text_platinum_num:AddChild( self.platinumContinueTime )
	PosTool.Center( self.platinumTips )

	--icon
	local sp_activity_platina_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_platina_icon" ,true)
	local icon = Util:Sprite(sp_activity_platina_icon)
	self.Panel_platinum_icon:AddChild(icon)
	PosTool.Center( icon )


	local sp_activity_extreme_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_extreme_icon" ,true)
	local icon = Util:Sprite(sp_activity_extreme_icon)
	self.Panel_extreme_icon:AddChild(icon)
	PosTool.Center( icon )

	--土豪卡
    buy_reward_diamond = Modules.ChargeModule.GetMoney(t1[2], "sell_price")

	text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , buy_reward_diamond )

	local tab = {
			        {zi = CPPGameLib.GetString("monthCard_rechargeTips_1") , ys = 0 , dx = Macros.TypefaceSize.largish , mb = true },
			        {zi = text , ys = 1, dx = Macros.TypefaceSize.largish , jg = 4 , mb = true },
			        {zi = CPPGameLib.GetString("monthCard_rechargeTips_2") , ys = 0 , dx = Macros.TypefaceSize.largish , jg = 4 , mb = true },
			    }
    self.extremeTips = Util:LabCenter( tab )  
	self.Text_extreme_type:AddChild( self.extremeTips )
	PosTool.Center( self.extremeTips )

	--持续
	self.extremeContinueTime = Util:Label( CPPGameLib.GetString("monthCard_forever") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	self.Text_extreme_num:AddChild( self.extremeContinueTime )
	PosTool.Center( self.extremeContinueTime )

	
	local sp_month_bubbling = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_month_bubbling" )
	self.sp_month_bubbling = Util:Sprite(sp_month_bubbling)
	icon:AddChild(self.sp_month_bubbling)
	PosTool.Center( self.sp_month_bubbling , 120 , 60 )

-- yangshuo(杨硕) 10-19 20:03:41
-- 送英雄
-- yuanjunguang(元俊广) 10-19 20:10:08
-- 就三个字？
-- yangshuo(杨硕) 10-19 20:10:28
-- 对
	local text = Util:Label( CPPGameLib.GetString("monthCard_give") , Macros.TypefaceSize.largish , Macros.Color.headline )
	self.sp_month_bubbling:AddChild( text )
	PosTool.Center( text , 0 , 5 )

	ActionManager.GetInstance():UpDownMoveAnimation( self.sp_month_bubbling , 0.5 , 5)

end


function Modules.TemplMember:__SetShowUI()
	print("xy" , "Modules.TemplMember:__SetShowUI()")
	local lst = CallModuleFunc(ModuleType.CHARGE , "GetMonthCard" )
	if lst.month_card_list then
		for i , v in ipairs( lst.month_card_list ) do
			local cnf = CPPGameLib.GetConfig( "MonthCard" , v.card_id , false )
			if cnf and cnf.valid_time ~= 0 then
				-- 白金卡设置
				self:SetPlatinumRecharge(v)
			else
				-- 土豪卡设置
				self:SetExtremeRecharge(v)
			end
		end
	end
end

--设置屌丝卡
function Modules.TemplMember:SetPlatinumRecharge(data)
    if self.platinumTips then
    	self.platinumTips:RemoveSelf( true )
    	self.platinumTips = nil
    end

	-- self.Button_platinum_recharge:SetEnabled( false )
	local cnf = CPPGameLib.GetConfig( "MonthCard", data.card_id )
	--结束时间
	local endTime = data.buy_time + ( cnf.valid_time * 24 * 60 * 60 ) 
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--剩余时间
	local residueTime = endTime - serverTime
	if residueTime <= 0 then
		--月卡表
		local cnf = CPPGameLib.GetConfig("MonthCard", nil, nil , true )
		if not cnf then return end

	    self.platinumTips = Util:LabCenter({
	        {zi = CPPGameLib.GetString("monthCard_rechargeTips" , cnf[1].sell_price ) , ys = 1 , dx = Macros.TypefaceSize.largish , mb = true },
	    })  
		self.Text_platinum_type:AddChild( self.platinumTips )
		PosTool.Center( self.platinumTips )

		self.platinumContinueTime:SetString( CPPGameLib.GetString("monthCard_continueTime" , cnf[1].valid_time ) )

		print("xy" , "===============")
	    --充值按钮
	    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
	    self.Button_platinum_recharge:SetNormalImage( imgdata )
	    self.Button_platinum_recharge:SetText( CPPGameLib.GetString("activity_vipDraw") )
	    local r, g, b, a = unpack(Macros.Color.button_red)
	    self.Button_platinum_recharge:SetOutline(r, g, b, a, 2)
		WidgetUnity.SetWidgetListener(self.Button_platinum_recharge , nil, nil, function()
			self:OnButtonPlatinumRecharge()
		end, nil, 0.5 * 1000)

		self.Button_platinum_recharge:SetEnabled(true)

		return
	end

    self.platinumTips = Util:LabCenter({
        {zi = CPPGameLib.GetString("activity_activate") , ys = 1 , dx = Macros.TypefaceSize.largish , mb = true },
    })  
	self.Text_platinum_type:AddChild( self.platinumTips )
	PosTool.Center( self.platinumTips )

    --领取按钮
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.Button_platinum_recharge:SetNormalImage( imgdata )
    self.Button_platinum_recharge:SetText( CPPGameLib.GetString("activity_vipRecharge") )
    local r, g, b, a = unpack(Macros.Color.button_yellow)
    self.Button_platinum_recharge:SetOutline(r, g, b, a, 2)
	WidgetUnity.SetWidgetListener(self.Button_platinum_recharge , nil, nil, function()
		self:OnButtonPlatinumDraw( data )
	end, nil, 0.5 * 1000)

    --上次使用的时间
	local year1, month1 , day1 , hour1 , minute1, second1 , week1 = TimeTool.GetDateFromTimeStamp( data.last_take_reward_time )
	local time_1 = year1 .. month1 .. day1
	--今天
	local year2, month2 , day2 , hour2 , minute2 , second2 , week2 = TimeTool.GetDateFromTimeStamp( serverTime )
	local time_2 = year2 .. month2 .. day2

    if data.last_take_reward_time ~= 0  and time_1 == time_2 then
	   	self.Button_platinum_recharge:SetText( CPPGameLib.GetString("rebel_getWin") )
	   	self.Button_platinum_recharge:SetEnabled(false)
	end

    --倒计时
    local func = nil
    local function moveAnimate( ... )
		local endTime = data.buy_time + ( cnf.valid_time * 24 * 60 * 60 ) 
		local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		local residueTime = endTime - serverTime
    	if residueTime <= 0 then
    		CPPActionManager.StopAllActions(self.platinumContinueTime)
    		CallModuleFunc(ModuleType.CHARGE , "MonthCardDataReq")
    	else
            CPPActionManager.DelayTo( self.platinumContinueTime , 1 , func )
            self.platinumContinueTime:SetString( TimeTool.TransTimeStamp( CPPGameLib.GetString("time_fate") , residueTime ) )
        end
    end

    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

--设置土豪卡
function Modules.TemplMember:SetExtremeRecharge(data)
	-- self.Button_extreme_recharge:SetEnabled( false )
	print("xy" , "SetExtremeRecharge" , data )

	--结束时间
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--剩余时间
	if data.buy_time == 0 then
		self.sp_month_bubbling:SetVisible(true)
		return 
	end
	

	self.sp_month_bubbling:SetVisible(false)
    --按钮
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.Button_extreme_recharge:SetNormalImage( imgdata )
    self.Button_extreme_recharge:SetText( CPPGameLib.GetString("activity_vipRecharge") )
    local r, g, b, a = unpack(Macros.Color.button_yellow)
    self.Button_extreme_recharge:SetOutline(r, g, b, a, 2)
	WidgetUnity.SetWidgetListener(self.Button_extreme_recharge , nil, nil, function()
		self:OnButtonExtremeDraw( data )
	end, nil, 0.5 * 1000)

    --上次使用的时间
	local year1, month1 , day1 , hour1 , minute1, second1 , week1 = TimeTool.GetDateFromTimeStamp( data.last_take_reward_time )
	local time_1 = year1 .. month1 .. day1
	--今天
	local year2, month2 , day2 , hour2 , minute2 , second2 , week2 = TimeTool.GetDateFromTimeStamp( serverTime )
	local time_2 = year2 .. month2 .. day2

    if data.last_take_reward_time ~= 0  and time_1 == time_2 then
	   	self.Button_extreme_recharge:SetText( CPPGameLib.GetString("rebel_getWin") )
	   	self.Button_extreme_recharge:SetEnabled(false)
	end
	
    --已购买
    if self.extremeTips then
    	self.extremeTips:RemoveSelf( true )
    	self.extremeTips = nil
    end

    self.extremeTips = Util:LabCenter({
        {zi = CPPGameLib.GetString("activity_activate") , ys = 1 , dx = Macros.TypefaceSize.largish , mb = true },
    })  
	self.Text_extreme_type:AddChild( self.extremeTips )
	PosTool.Center( self.extremeTips )

    self.extremeContinueTime:SetString( CPPGameLib.GetString("monthCard_forever") )
end

--屌丝卡充值
function Modules.TemplMember:OnButtonPlatinumRecharge()
	CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
end

--屌丝卡领取
function Modules.TemplMember:OnButtonPlatinumDraw( data )
	local info = {}
	info.card_id = data.card_id
	CallModuleFunc(ModuleType.CHARGE , "TakeMonthCardRewardReq" , info )
end

--土豪卡充值
function Modules.TemplMember:OnButtonExtremeRecharge()
	CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
end

--土豪卡领取
function Modules.TemplMember:OnButtonExtremeDraw( data )
	local info = {}
	info.card_id = data.card_id
	CallModuleFunc(ModuleType.CHARGE , "TakeMonthCardRewardReq" , info )
end

