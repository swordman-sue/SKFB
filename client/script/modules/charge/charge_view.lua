--
-- @brief 充值
-- @author: yjg
-- @date: 2017年6月16日10:59:50
--

--特殊充值类型
local SP_RECHARGE_TYPE = {
	GOOGLE = 1,
	MYCARD = 2,
}

Modules = Modules or {}

Modules.ChargeView = Modules.ChargeView or BaseClass(BaseView)

function Modules.ChargeView:__init()
	self.__layout_name = "recharge"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("recharge"),
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
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHARGE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHARGE, true)
    self:__AddPreloadTemplate( GUI.TemplateRegistry.TEMPL_RECHARGE_ITEM )
end

function Modules.ChargeView:__OpenCallback()  
	self:__Getwidget()
	self:__ShowUI()
	self:__Server()
end

function Modules.ChargeView:__RegistAllEvents()
    --月卡刷新
    self:BindGlobalEvent(Macros.Event.ChargeModule.CHARGE_DATA_RESP,function (_, data)
		self:__SetShowUI()
    end)

    --月卡购买获得
    self:BindGlobalEvent(Macros.Event.ChargeModule.RECHARGE_SUCCESS,function ( _ , recharge_type , recharge_id )
	    local cnf = CPPGameLib.GetConfig( "MonthCard", recharge_id , false)
	    if cnf then
		    local t1 = {}
		    table.insert(t1,{ type = Macros.Game.RolePropType.DIAMOND , num = cnf.buy_reward_diamond })
		    if cnf.reward_item_id then
			    table.insert(t1,{id = cnf.reward_item_id , num = cnf.reward_item_num or 1 })
			end
		    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
		end

    end)
 end

function Modules.ChargeView:__Dispose()
end

function Modules.ChargeView:__Server()
	local type_ = true
	if not CallModuleFunc(ModuleType.CHARGE , "GetMonthCard") then
		CallModuleFunc(ModuleType.CHARGE , "MonthCardDataReq")
		type_ = false
	end

	if not CallModuleFunc(ModuleType.CHARGE , "GetRechargeData") then
		CallModuleFunc(ModuleType.CHARGE , "RechargeDataReq")
		type_ = false
	end

	if type_ == true then
		self:__SetShowUI()
	end
end

function Modules.ChargeView:__Getwidget()
	--红条
	self.Image_red = self:GetWidget("Image_red")
	
	--进度条
	self.Panel_schedule = self:GetWidget("Panel_schedule")
	--vip图标
	self.Panel_vip = self:GetWidget("Panel_vip")
	--特权按钮
	self.Button_privilege = Util:WidgetButton( 
											self:GetWidget("Button_privilege") , 
											"" , 
											CPPGameLib.Handler(self,self.OnButtonPrivilege) , 
											Macros.TypefaceSize.popup , nil , 
											Macros.Color.button_yellow)
    self.Button_privilege:SetVisible(not CPPSdkTools.IsInExamine())

	--内容框
	self.Image_content = self:GetWidget("Image_content")
	--缎带
	self.Image_streamer = self:GetWidget("Image_streamer")
	--充值
	self.Image_3 = self:GetWidget("Image_3")
	--google按钮
	self.__btn_google = self:GetWidget("Button_google")
	Util:WidgetButton(self.__btn_google, "", function ()
		--特殊处理
		self:SpHandleRecharge(SP_RECHARGE_TYPE.GOOGLE)
	end)
	self.__btn_google:SetVisible(false)
	--mycard按钮
	self.__btn_mycard = self:GetWidget("Button_mycard")
	self.__btn_mycard:SetVisible(false)

	--界面
	self.Image_bg = self:GetWidget("Image_bg")
    --按钮
    local btn_btn_bag_add = Resource.PathTool.GetSystemEntryIconPath("btn_cumulative_recharge")
    self.addBtn = Util:Button( btn_btn_bag_add , function ( ... )
        CallModuleFunc(ModuleType.DIRECT_BUY_SHOP, "OpenView", BaseViewType.DIRECT_BUY_SHOP )
    end )
    self.addBtn:SetAnchorPoint( 0 , 0.5 )
    self.addBtn:SetPosition( self.Image_bg:GetContentWidth() , self.Image_bg:GetContentHeight()/2 + 95 )
    self.addBtn:SetVisible(false) 
    self.Image_bg:AddChild( self.addBtn )
end

function Modules.ChargeView:__ShowUI()
    local params =  
    {
        item_class = Modules.ChargeItem,
        item_width = 463.13 ,
        item_height = 130.00 ,
        row = 4 ,
        col = 2 ,
        view_height = self.Image_content:GetContentHeight() - 5,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())

	PosTool.Center(self.lst_list:GetNode() , 0 , 0 )

    local charge_list = self:CallModuleFunc("GetRechargeLst")  
	self.lst_list:SetDataList( charge_list )

   	--进度条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")
    --进度条
    self.LoadingBar_loadingBar = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Panel_schedule:GetContentWidth() - 4 , 20 , self.Panel_schedule:GetContentWidth() , 24 , 0, 100)
    self.LoadingBar_loadingBar:SetAnchorPoint(0.5, 0.5)
    self.Panel_schedule:AddChild(self.LoadingBar_loadingBar:GetNode())
    PosTool.Center(self.LoadingBar_loadingBar)

    --VIP经验
    self.vipExp = Util:Name("00/00",Macros.TypefaceSize.normal)
    self.vipExp:SetPosition(self.LoadingBar_loadingBar:GetNode():GetPositionX() , self.LoadingBar_loadingBar:GetNode():GetPositionY() )
    self.vipExp:SetAnchorPoint(0.5,0.5)
    self.Panel_schedule:AddChild(self.vipExp)

    --vip图样
    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true )
    local vip_title_icon = Util:Sprite(vip_title_icon_sp)
    vip_title_icon:SetPosition( 0 , self.Panel_vip:GetContentHeight()/2 )
    vip_title_icon:SetAnchorPoint( 0 , 0.5 )
    self.Panel_vip:AddChild( vip_title_icon )

   --VIP等级
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    self.viplv = GUI.ImgFont.New(vipnum)
    self.viplv:SetText(tostring(0))
    PosTool.RightTo( vip_title_icon , self.viplv , 5 )
    self.Panel_vip:AddChild(self.viplv:GetNode())  

end

function Modules.ChargeView:__SetShowUI()
    --经验
    local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp")
    --进度
    self.LoadingBar_loadingBar:SetProgress( nowExp/upExp * 100 )
    --经验
    self.vipExp:SetString(nowExp.."/"..upExp)

    if self.headHint then
    	self.headHint:RemoveSelf(true)
    	self.headHint = nil
    end

    if self.nextLv then
    	self.nextLv:RemoveSelf(true)
    	self.nextLv = nil
    end

	local lv , maxLv = CallModuleFunc(ModuleType.VIP , "GetVipLV")
	if maxLv ~= lv then
		nextLv = lv + 1
	    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true )
		--再充值
		self.headHint = Util:LabCenter({
		                            { zi = CPPGameLib.GetString("vip_continue"),dx = Macros.TypefaceSize.normal},
		                            { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND), dx = 0.7 , jg = 5},
		                            { zi = upExp - nowExp ,ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal, jg = 5},
		                            { zi = CPPGameLib.GetString("vip_can") ,ys = Macros.Color.golden , dx = Macros.TypefaceSize.normal, jg = 5},
									{ tp = vip_title_icon_sp, dx = 0.7 , jg = 5}
		                          })
		self.headHint:SetPosition( self.Panel_schedule:GetPositionX() , self.Panel_schedule:GetPositionY() - self.headHint:GetContentHeight() - 5 )
		self.headHint:SetAnchorPoint( 0.5 , 0.5 )
		self.Image_red:AddChild(self.headHint)

		--数值
		local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
		self.nextLv = GUI.ImgFont.New(vipnum)
		self.nextLv:SetText(tostring(nextLv))
		self.nextLv:SetPosition( 5 + self.headHint:GetPositionX() + (self.headHint:GetContentWidth()/2) , self.headHint:GetPositionY() )
		self.nextLv:SetAnchorPoint( 0 , 0.5 )
		self.Image_red:AddChild(self.nextLv :GetNode()) 
	end

    --等级
    self.viplv:SetText(tostring( CallModuleFunc(ModuleType.VIP , "GetVipLV") ))

    --刷新列表
	self.lst_list:ItemDataIter(function(index, item_data , item )
		if item then
			item:__SetShowUI()
		end
	end)
end

--特权按钮
function Modules.ChargeView:OnButtonPrivilege()
	CallModuleFunc( ModuleType.VIP , "OpenView" , BaseViewType.VIP )
end

--充值特殊处理
function Modules.ChargeView:SpHandleRecharge(index)
	if self.__selected_sp_index and self.__selected_sp_index == index then
		return 
	end
	self.__selected_sp_index = index
	--按钮资源
	if not self.__sp_recharge_btnres_list then
		self.__sp_recharge_btnres_list = {}
		local btn_res_st_list = {"selected","unselected"}
		for i,res_str in ipairs(btn_res_st_list) do
			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, res_str)
			table.insert(self.__sp_recharge_btnres_list,sp)
		end
	end
	--按钮显示
	self.__btn_google:SetNormalImage(self.__sp_recharge_btnres_list[index == SP_RECHARGE_TYPE.GOOGLE and 1 or 2])
	self.__btn_mycard:SetNormalImage(self.__sp_recharge_btnres_list[index == SP_RECHARGE_TYPE.MYCARD and 1 or 2])
	--google
	if index == SP_RECHARGE_TYPE.GOOGLE then
		if self.__normal_datalist then
			self.lst_list:SetDataList(self.__normal_datalist)
		end
	else
		--mycard
        if not self.__mycard_datalist then
            self.__mycard_datalist = {}       
            local upper_limit_recharge = self:CallModuleFunc("GetUpperLimitRecharge")        
    	    local rechargeCnf = CPPGameLib.GetConfig("RechargeMyCard", nil, nil , true ) 
    	    for i , v in pairs( rechargeCnf ) do
    	        if v.money <= upper_limit_recharge then
                    local mycard_info = CPPGameLib.CopyTblShallow(v)
    	        	mycard_info.type = Macros.Game.RechargeType.COMMON
    	        	mycard_info.id = i
                    mycard_info.is_mycard = true
    	        	table.insert(self.__mycard_datalist, mycard_info)
    	        end
    	    end

    	    table.sort( self.__mycard_datalist , function (  a , b )
    	    	return a.id < b .id
    	    end)
        end
        self.lst_list:SetDataList(self.__mycard_datalist)
	end
end

--充值条目
Modules = Modules or {}

Modules.ChargeItem = Modules.ChargeItem or BaseClass(GUI.ScrollItem)

function Modules.ChargeItem:__init()
	self:InitTemplate( GUI.TemplateRegistry.TEMPL_RECHARGE_ITEM )	
	self:__Getwidget()
end

function Modules.ChargeItem:__delete()
	if self.diamond then
		self.diamond:RemoveSelf(true)
		self.diamond = nil
	end
end

function Modules.ChargeItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__ShowUI(data)
end

function Modules.ChargeItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--名字
	self.Image_name = self:GetWidget("Image_name")
	--描述
	self.Panel_describe = self:GetWidget("Panel_describe")
	--充值
	self.Button_price = Util:WidgetButton( 
											self:GetWidget("Button_price") , 
											"" , 
											CPPGameLib.Handler(self,self.OnButtonPrice) , 
											Macros.TypefaceSize.popup , nil , 
											Macros.Color.button_yellow)
end

function Modules.ChargeItem:__ShowUI( data )

	--名字
	--图标
	local pedestalSp = nil
	local icon = nil
	local icon_ = nil
	if data.type == Macros.Game.RechargeType.MONTH_CARD then
		local sp = nil
		if data.valid_time ~= 0 then
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_platinum" , true )
			pedestalSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_red_light" )
		else
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_extreme" , true )
			pedestalSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_blue_light" )

			local sp_month_bubbling = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_month_bubbling" )
			self.sp_month_bubbling = Util:Sprite(sp_month_bubbling)
			self.Image_name:AddChild(self.sp_month_bubbling)
			PosTool.Center( self.sp_month_bubbling , 70 , 12 )
			self.sp_month_bubbling:SetVisible( true )

			local text = Util:Label( CPPGameLib.GetString("monthCard_give") , Macros.TypefaceSize.largish , Macros.Color.headline )
			self.sp_month_bubbling:AddChild( text )
			PosTool.Center( text , 0 , 0 )

			ActionManager.GetInstance():LeftRightMoveAnimation( self.sp_month_bubbling , 0.5 , 5)
		end

		local name = Util:Sprite( sp )
		name:SetAnchorPoint( 0 , 0.5 )
		name:SetPosition( 0 , self.Image_name:GetContentHeight()/2 )
		self.Image_name:AddChild( name )

		icon_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, data.icon , true )
	else
		local name = Util:MoneyControl( Macros.Game.RolePropType.DIAMOND )
		name:SetAnchorPoint( 0 , 0.5 )
		name:SetPosition( 0 , self.Image_name:GetContentHeight()/2 )
		self.Image_name:AddChild( name )

	    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
	    local item_num = GUI.ImgFont.New(vipnum)
	    item_num:SetText(tostring( data.frecharge_diamond ))
	    PosTool.RightTo( name , item_num , 5 )
	    self.Image_name:AddChild(item_num:GetNode()) 

	    pedestalSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_yellow_light" )

	    icon_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, data.icon )
	end

	--图标底座
	local pedestal = Util:Sprite( pedestalSp )
	self.Panel_icon:AddChild( pedestal )
	PosTool.Center( pedestal )

	self.icon = Util:Sprite( icon_ )
	self.Panel_icon:AddChild( self.icon )
	PosTool.Center( self.icon )

	--描述
	self.describe = Util:Label( data.describe[1] , Macros.TypefaceSize.normal , Macros.Color.content , self.Panel_describe:GetContentWidth() - 5 )
	self.describe:SetAnchorPoint( 0 , 1 )
	self.describe:SetPosition( 2.5 , self.Panel_describe:GetContentHeight() )
	self.Panel_describe:AddChild( self.describe )

	-- 价格
	self:Currency( data.id )
	-- self:SetBuyBtn( self.Button_price , Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2") )

	--活动
	self.recharge_label = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_label" ) )
	self.recharge_label:SetAnchorPoint( 0 , 1 )
	self.recharge_label:SetPosition( 0 , self.Image_item:GetContentHeight() )
	self.Image_item:AddChild( self.recharge_label )
	self.recharge_label:SetVisible(false)

	local recharge_activity = Util:Label( CPPGameLib.GetString("recharge_activity") , Macros.TypefaceSize.slightly , Macros.Color.Special )
	recharge_activity:SetAnchorPoint( 0.5 , 0.5 )
	self.recharge_label:AddChild(recharge_activity)
	PosTool.Center( recharge_activity , -10 , 10 )
	CPPActionManager.RotateTo(recharge_activity , 0 , -45 )

	--首充
	self.recharge_first = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE, "recharge_first" , true ) )
	self.recharge_first:SetAnchorPoint( 0.5 , 0.5 )
	self.recharge_first:SetPosition( 0 , self.Panel_icon:GetContentHeight() + 10 )
	self.Panel_icon:AddChild( self.recharge_first )
	self.recharge_first:SetVisible(false)

	if CallModuleFunc(ModuleType.CHARGE , "GetMonthCard") then
		self:__SetShowUI()
	end
end

function Modules.ChargeItem:__SetShowUI(  )
	--月卡
	if self.__data.type == Macros.Game.RechargeType.MONTH_CARD then
		self.recharge_label:SetVisible(true)
        
		local lst = CallModuleFunc(ModuleType.CHARGE , "GetMonthCard" )
		local t1 = {}
		for i , v in ipairs( lst.month_card_list ) do
			t1[v.card_id] = v
		end

		if lst.month_card_list and t1[self.__data.id] then
			if self.__data.valid_time ~= 0 then
				self:SetPlatinumRecharge( t1[self.__data.id] )
			else
				self:SetExtremeRecharge( t1[self.__data.id] )
			end
		end
		--特效
	    if self.__data.effect then
	    	self.icon:SetVisible(false)
	        local btnEffect_id , btnEffect_con = self:__CreateEffect( { res_name = self.__data.effect } , self.Panel_icon )
	        btnEffect_con:SetAnchorPoint( 0.5 , 0.5 )
	        PosTool.Center( btnEffect_con )
	    end

		--月卡
		local lst = CallModuleFunc(ModuleType.CHARGE , "GetMonthCard" )
		local t1 = {}
		local type_ = true
		for i , v in ipairs( lst.month_card_list ) do
			local cnf = CPPGameLib.GetConfig( "MonthCard", v.card_id )
			if cnf.type == 2 and self.__data.cardType == 2 then
				type_ = false
			end
		end
		if self.sp_month_bubbling then
			self.sp_month_bubbling:SetVisible( type_ )
		end

	else	
		if not CallModuleFunc(ModuleType.CHARGE , "GetRechargeData" ) then
			return
		end
		local type_ = CallModuleFunc(ModuleType.CHARGE , "GetRechargeRecord" , self.__data.id )

		if not type_ then
			self.recharge_first:SetVisible(true)
		else
			self.recharge_first:SetVisible(false)

			local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , self.__data.id,self.__data.is_mycard)
			self.describe :SetString( string.format( self.__data.describe[2] or "%s" , text ) )
		end

		--特效
	    if self.__data.effect then
	        local btnEffect_id , btnEffect_con = self:__CreateEffect( { res_name = self.__data.effect } , self.Panel_icon )
	        PosTool.Center( btnEffect_con , 5 , -20 )
	    end
	end
end

--充值
function Modules.ChargeItem:OnButtonPrice()
    CallModuleFunc(ModuleType.CHARGE , "RechargeOrderIDReq" , self.__data.type , {recharge_id = self.__data.id, is_mycard = self.__data.is_mycard} )
end

--设置屌丝卡
function Modules.ChargeItem:SetPlatinumRecharge(data)
	--结束时间
	local endTime = data.buy_time + ( ConfigAdapter.Common.GetMonthCardContinue() * 24 * 60 * 60 ) 
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--剩余时间
	local residueTime = endTime - serverTime
	if residueTime <= 0 then
		--月卡表
		self.describe:SetString( self.__data.describe[1] )

	    --充值按钮
	    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
	    self.Button_price:SetNormalImage( imgdata )
	    self:Currency( CPPGameLib.GetString("activity_vipDraw") )

		WidgetUnity.SetWidgetListener(self.Button_price , nil, nil, function()
			self:OnButtonPrice()
		end, nil, 0.5 * 1000)

		self.Button_price:SetEnabled(true)

		return
	end

    --领取按钮
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.Button_price:SetNormalImage( imgdata )
    self:Currency( CPPGameLib.GetString("activity_vipRecharge") )

	WidgetUnity.SetWidgetListener(self.Button_price , nil, nil, function()
		-- self:OnButtonPlatinumDraw()
		CallModuleFunc(ModuleType.ACTIVITY, "OpenView", BaseViewType.ACTIVITY , Macros.Game.SystemID.LUNA_WELFARE )
	end, nil, 0.5 * 1000)

    --上次使用的时间
	local __, __, day1, __, __, __, __ = TimeTool.GetDateFromTimeStamp( data.last_take_reward_time )
	--今天
	local __, __, day2, __, __, __, __ = TimeTool.GetDateFromTimeStamp( serverTime )

    if data.last_take_reward_time ~= 0 and day1 == day2 then
	   	-- self.Button_price:SetText( CPPGameLib.GetString("rebel_getWin") )
    	self:Currency( CPPGameLib.GetString("rebel_getWin") )
	   	self.Button_price:SetEnabled(false)
	end

    --倒计时
    local func = nil
    local function moveAnimate( ... )
		local endTime = data.buy_time + ( ConfigAdapter.Common.GetMonthCardContinue() * 24 * 60 * 60 ) 
		local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		local residueTime = endTime - serverTime
    	if residueTime <= 0 then
    		CPPActionManager.StopAllActions(self.describe)
    		CallModuleFunc(ModuleType.CHARGE , "MonthCardDataReq")
    	else
            CPPActionManager.DelayTo( self.describe , 1 , func )
            self.describe:SetString( TimeTool.TransTimeStamp( CPPGameLib.GetString("time_fate") , residueTime ) )
        end
    end

    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

--设置土豪卡
function Modules.ChargeItem:SetExtremeRecharge(data)
	-- self.Button_extreme_recharge:SetEnabled( false )
	--结束时间
	local endTime = data.buy_time + ( data.buy_time * 24 * 60 * 60 ) 
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--剩余时间
	local residueTime = endTime - serverTime

	if residueTime <= 0 then
		--土豪卡
		local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , self.__data.id,self.__data.is_mycard)
		self.describe :SetString( string.format( self.__data.describe[2] or "%s" , text ) )

	    --充值按钮
	    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
	    self.Button_price:SetNormalImage( imgdata )
	    self:Currency( CPPGameLib.GetString("activity_vipDraw") )
	    
		WidgetUnity.SetWidgetListener(self.Button_price , nil, nil, function()
			self:OnButtonPrice()
		end, nil, 0.5 * 1000)

		self.Button_price:SetEnabled(true)
		return
	end

    --按钮
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.Button_price:SetNormalImage( imgdata )
    self:Currency( CPPGameLib.GetString("activity_vipRecharge") )
	WidgetUnity.SetWidgetListener(self.Button_price , nil, nil, function()
		CallModuleFunc(ModuleType.ACTIVITY, "OpenView", BaseViewType.ACTIVITY,Macros.Game.SystemID.LUNA_WELFARE)
	end, nil, 0.5 * 1000)

    --上次使用的时间
	local year1, month1 , day1 , hour1 , minute1, second1 , week1 = TimeTool.GetDateFromTimeStamp( data.last_take_reward_time )
	local time_1 = year1 .. month1 .. day1
	--今天
	local year2, month2 , day2 , hour2 , minute2 , second2 , week2 = TimeTool.GetDateFromTimeStamp( serverTime )
	local time_2 = year2 .. month2 .. day2

    if data.last_take_reward_time ~= 0  and time_1 == time_2 then
	   	self:Currency( CPPGameLib.GetString("rebel_getWin") )
	   	self.Button_price:SetEnabled(false)
	end
end

function Modules.ChargeItem:Currency( info )
	self.Button_price:SetText(" ")
	self.Button_price:SetEnabled( true )
	if self.diamond then
		self.diamond:RemoveSelf(true)
		self.diamond = nil
	end

	if type( info ) == "number" then
		local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , info , self.__data.is_mycard)
	   	self.diamond = Util:Label( 
				                    text , 
				                    Macros.TypefaceSize.normal , 
				                    Macros.Color.btn , nil , nil ,
				                    Macros.Color.button_yellow 
				                )
	else
	    --钻石
   		self.diamond = Util:Label( 
   									info , 
   									Macros.TypefaceSize.normal , 
   									Macros.Color.btn , nil , nil ,
   									Macros.Color.button_yellow 
   								)
	end
		
	self.diamond:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_item:AddChild( self.diamond )
	self.diamond:SetPosition( self.Button_price:GetPositionX() , self.Button_price:GetPositionY() )
end
