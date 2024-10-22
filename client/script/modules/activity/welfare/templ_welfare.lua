
--[[
%% @module: vip福利
%% @author: yjg
%% @created: 2017年5月11日10:21:46
--]]

--不用

Modules = Modules or {}

Modules.TemplWelfare = Modules.TemplWelfare or BaseClass(BaseTabView)

function Modules.TemplWelfare:__init()
	self.__tab_params_ex = {15 , true}
end

--选项卡界面创建完成回调
function Modules.TemplWelfare:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplWelfare:__OpenCallback()
	if not CallModuleFunc(ModuleType.ACTIVITY, "GetVipDrawLevel") then
		CallModuleFunc(ModuleType.ACTIVITY, "VipGiftDataReq")
	else
		self:__SetShowUI()
	end
	--每日礼包
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_VIP_GIFT , function ( _ , key )
    	self:__SetShowUI()
    end)

	--设置推荐
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_RECOMMEND , function ( _ , key )
    	self:SetRecommend()
    end)

end

--选项卡界面关闭回调
function Modules.TemplWelfare:__CloseCallback()
	if self.diamond then
		self.diamond:RemoveSelf(true)
		self.diamond = nil
	end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplWelfare:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil 
    end
    if self.draw_list then
        self.draw_list:DeleteMe()
        self.draw_list = nil 
    end
    if self.recharge_list then
        self.recharge_list:DeleteMe()
        self.recharge_list = nil 
    end
end

--获得组件
function Modules.TemplWelfare:__Getwidget()
	--页签
	self.Panel_2 = self:GetWidget("Panel_2") 

	--每天页面
	self.Image_everyday = self:GetWidget("Image_everyday")
	self.Image_everyday:SetVisible( false )
		--背景
	    local sp = Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_activity_welfare")
	    self.Image_everyday:SetImage(sp , TextureResType.LOCAL )

	    --今日推荐
	    self.Image_award_item = self:GetWidget("Image_award_item")
	    --黑色底框
	    self.Image_award_black_item = self:GetWidget("Image_award_black_item")
	    --领取
	    self.Button_award = Util:WidgetButton( 
	    										self:GetWidget("Button_award") , 
	    										"" , 
	    										CPPGameLib.Handler(self,self.OnButtonAward),
	    										nil,nil , Macros.Color.button_yellow
	    										)
	    --标签
	    self.Image_award = self:GetWidget("Image_award")

	    local BtnEffect_con = nil
		self.btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.Button_award , 999)
		BtnEffect_con:SetAnchorPoint(0.5,0.5)
		PosTool.Center( BtnEffect_con )
		GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)


	    --领取条目
	    self.Image_draw_item = self:GetWidget("Image_draw_item")
	    self.Image_draw_item:SetVisible(true)
	    --黑色底框
	    self.Image_draw_black_item = self:GetWidget("Image_draw_black_item")
	    --领取
	    self.Button_draw = Util:WidgetButton( 
	    										self:GetWidget("Button_draw") , 
	    										CPPGameLib.GetString("activity_vipRecharge") , 
	    										CPPGameLib.Handler(self,self.OnButtonDraw),
	    										nil,nil , Macros.Color.button_yellow
	    										)
	    --标签
	    self.Image_label = self:GetWidget("Image_label")

		--充值条目
	    self.Image_recharge_item = self:GetWidget("Image_recharge_item")
	    self.Image_recharge_item:SetVisible(true)
	    --黑色底框
	    self.Image_recharge_black_item = self:GetWidget("Image_recharge_black_item")
	    --领取
	    self.Button_recharge = Util:WidgetButton( 
	    										self:GetWidget("Button_recharge") , 
	    										CPPGameLib.GetString("activity_vipDraw") , 
	    										CPPGameLib.Handler(self,self.OnButtonRecharge),
	    										nil,nil , Macros.Color.button_red
	    										)
	    --标签
	    self.Text_hint = self:GetWidget("Text_hint")   

	    --横幅
	    -- self.Image_streamer = self:GetWidget("Image_streamer")
	    --字样1
	    self.Text_streamer_1 = self:GetWidget("Text_streamer_1")
	    --字样2
	    self.Text_streamer_2 = self:GetWidget("Text_streamer_2")

	--每周页面
	self.Image_weekly = self:GetWidget("Image_weekly")
	self.Image_weekly:SetVisible( false )
		--提示
		self.Text_weekly_tips = self:GetWidget("Text_weekly_tips")
		--滚动层
		self.Panel_roll = self:GetWidget("Panel_roll")
end

--创建组件
function Modules.TemplWelfare:__ShowUI()

	--页眉
  	local text = {
            [1] = {name = CPPGameLib.GetString("activity_everyday") },
            [2] = {name = CPPGameLib.GetString("activity_weekly") },
    }

    self.icon_red_point = {}
    self.__tab:SetContentSize( self.Panel_2:GetContentWidth() , self.Panel_2:GetContentHeight() + 15 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0.5 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse_1")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no_1")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition(self.Panel_2:GetContentWidth()/2 + 20 , 23 )
    self.Panel_2:AddChild(self.__tab:GetNode())

	--每日礼包
    local activity_vipWelfareTips1 = Util:Label( CPPGameLib.GetString("activity_vipWelfareTips1") , Macros.TypefaceSize.largish , Macros.Color.vipgift )
    activity_vipWelfareTips1:SetAnchorPoint( 0 , 0.5 )
    activity_vipWelfareTips1:SetPosition( 0 , self.Text_streamer_1:GetContentHeight()/2 )
    self.Text_streamer_1:AddChild( activity_vipWelfareTips1 )

    local activity_vipWelfareTips2 = Util:Label( CPPGameLib.GetString("activity_vipWelfareTips2") , Macros.TypefaceSize.largish , Macros.Color.vipgift )
    activity_vipWelfareTips2:SetAnchorPoint( 0 , 0.5 )
    activity_vipWelfareTips2:SetPosition( 0 , self.Text_streamer_2:GetContentHeight()/2 )
    self.Text_streamer_2:AddChild( activity_vipWelfareTips2 )

    --今日推荐
    self.activity_recommend = Util:Label( CPPGameLib.GetString("activity_recommend") , Macros.TypefaceSize.largish , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_red )
    self.activity_recommend:SetAnchorPoint( 0 , 0.5 )
    self.activity_recommend:SetPosition( 10 , self.Image_award:GetContentHeight()/2 )
    self.Image_award:AddChild( self.activity_recommend )

    --当前专属福利
    self.draw_Name = Util:Label( "" , Macros.TypefaceSize.largish , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_red )
    self.draw_Name:SetAnchorPoint( 0 , 0.5 )
    self.draw_Name:SetPosition( 10 , self.Image_label:GetContentHeight()/2 )
    self.Image_label:AddChild( self.draw_Name )

    --下级专属福利
    self.recharge_Name = Util:Label( "" , Macros.TypefaceSize.largish , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_red )
    self.recharge_Name:SetAnchorPoint( 0 , 0.5 )
    self.recharge_Name:SetPosition( 10 , self.Text_hint:GetContentHeight()/2 )
    self.Text_hint:AddChild( self.recharge_Name )

    --每周礼包
    self.Text_weekly_tips = Util:WidgetLabel( self.Text_weekly_tips , CPPGameLib.GetString("activity_vipUp") , Macros.TypefaceSize.normal , nil )

    --每周礼包列表
    local params =  
    {
        item_class = Modules.TemplWelfareItem,
        item_width = 660.00 ,
        item_height = 145.00 ,
        row = 4 ,
        col = 1 ,
        view_height = self.Panel_roll:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_roll:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
end

function Modules.TemplWelfare:__SetShowUI()
	--礼包状态
	local now , nex = CallModuleFunc(ModuleType.ACTIVITY, "GetVipTodayWelfare")

	--今日推荐
	self:SetRecommend()

	--没现在
	if not now then
		--藏起领取
		self.Image_draw_item:SetVisible(false)
		--放开充值
		self.Image_recharge_item:SetVisible(true)
		-- self.Image_recharge_item:SetPosition( self.Image_draw_item:GetPositionX() , self.Image_draw_item:GetPositionY() )
		
		--设置充值
		self:SetRecharge()
	--没下次
	elseif not nex then
		--藏起充值
		self.Image_recharge_item:SetVisible(false)
		--放开领取
		self.Image_draw_item:SetVisible(true)
		-- self.Image_draw_item:SetPosition( self.Image_draw_item:GetPositionX() , self.Image_draw_item:GetPositionY() )

		--设置充值
		self:SetDraw()
	else
		--放开充值
		self.Image_recharge_item:SetVisible(true)
		--放开领取
		self.Image_draw_item:SetVisible(true)

		--设置领取
		self:SetDraw()
		--设置充值
		self:SetRecharge()
	end

	--每周礼包
	local lst = CallModuleFunc(ModuleType.ACTIVITY, "GetVipWeekGiftList")
	self.lst_list:SetDataList( lst ) 


	local _ , num_1 , num_2 = CallModuleFunc(ModuleType.ACTIVITY, "GetVipWeekGiftNum")
	for i,v in ipairs(self.icon_red_point) do
		self.icon_red_point[i]:SetVisible(false)
	end
	if num_1 ~= 0 then
		self.icon_red_point[1]:SetVisible(true)
	end
	if num_2 ~= 0 then
		self.icon_red_point[2]:SetVisible(true)
	end
end

--今日推荐
function Modules.TemplWelfare:SetRecommend()
		
	if not self.recommend_list then
	    --每周礼包列表
	    local params =  
	    {
	        item_class = Modules.IconItem,
	        item_width = 80.00 ,
	        item_height = 80.00 ,
	        horizon = true ,
	        row = 1 ,
	        col = 3 ,
	        item_space_c = 5 ,
	        view_width = self.Image_award_black_item:GetContentWidth() - 5 ,
	    }
	    -- 创建参数
	    self.recommend_list = GUI.ScrollView.New(params)
	    self.Image_award_black_item:AddChild(self.recommend_list:GetNode())
	    PosTool.Center(self.recommend_list:GetNode())
	end

	local lst , money , recharge_id = CallModuleFunc(ModuleType.ACTIVITY, "GetVipWeekRecommend")
	self.recommend_list:SetDataList( lst )

	local type_ = CallModuleFunc(ModuleType.ACTIVITY, "GetVipWeekRecommendBuyType")

	--按钮设置
	--可以领取
	if type_ == 1 then
		self:Currency( CPPGameLib.GetString("activity_vipRecharge") )
		WidgetUnity.SetWidgetListener( self.Button_award , nil, nil, function ( ... )
			CallModuleFunc(ModuleType.ACTIVITY, "TakeTodayRecommendGiftReq")
		end, nil, 0.5*1000)
		GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
	elseif type_ == 0 then
		self:Currency( recharge_id )
		WidgetUnity.SetWidgetListener( self.Button_award , nil, nil, function ( ... )
			print("xy" , "购买")
			CallModuleFunc(ModuleType.CHARGE , "RechargeOrderIDReq" , Macros.Game.RechargeType.RECOMMEND_BUY , {recharge_id = recharge_id} )
		end, nil, 0.5*1000)
		GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
	else
		self:Currency( CPPGameLib.GetString("rebel_getWin") )
		Util:GreyButton( self.Button_award )
		GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
	end

end

--领取设置
function Modules.TemplWelfare:SetDraw()
	local now , nex = CallModuleFunc(ModuleType.ACTIVITY, "GetVipTodayWelfare")
    --vip专属福利
	self.draw_Name:SetString( CPPGameLib.GetString("activity_vipExclusive" , now.level) )

	if not self.draw_list then
	    --每周礼包列表
	    local params =  
	    {
	        item_class = Modules.IconItem,
	        item_width = 80.00 ,
	        item_height = 80.00 ,
	        horizon = true ,
	        row = 1 ,
	        col = 3 ,
	        item_space_c = 5 ,
	        view_width = self.Image_draw_black_item:GetContentWidth() - 5 ,
	    }
	    -- 创建参数
	    self.draw_list = GUI.ScrollView.New(params)
	    self.Image_draw_black_item:AddChild(self.draw_list:GetNode())
	    PosTool.Center(self.draw_list:GetNode())
	end
	self.draw_list:SetDataList( now.lst )

	--按钮设置
	local drawLevel = CallModuleFunc(ModuleType.ACTIVITY, "GetVipDrawLevel")
	local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")
	if drawLevel and drawLevel == vipLevel then
		self.Button_draw:SetText( CPPGameLib.GetString("rebel_getWin") )
		Util:GreyButton( self.Button_draw )
	end
end

--充值设置
function Modules.TemplWelfare:SetRecharge()
	local now , nex = CallModuleFunc(ModuleType.ACTIVITY, "GetVipTodayWelfare")

    --下级vip专属福利
    local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp")
	self.recharge_Name:SetString( CPPGameLib.GetString("activity_vipExclusive_" , nex.level , upExp - nowExp ) )

	if not self.recharge_list then
	    local params =  
	    {
	        item_class = Modules.IconItem,
	        item_width = 80.00 ,
	        item_height = 80.00 ,
	        horizon = true ,
	        row = 1 ,
	        col = 3 ,
	        item_space_c = 5 ,
	        view_width = self.Image_recharge_black_item:GetContentWidth() - 5 ,
	    }
	    -- 创建参数
	    self.recharge_list = GUI.ScrollView.New(params)
	    self.Image_recharge_black_item:AddChild(self.recharge_list:GetNode())
	    PosTool.Center(self.recharge_list:GetNode())
	end
	self.recharge_list:SetDataList( nex.lst )
end

function Modules.TemplWelfare:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no_1")
    local sizeSp = Util:Sprite( on )
	local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

	local name = Util:Label( data.name , Macros.TypefaceSize.popup , Macros.Color.blinkcontent  , nil , nil , Macros.Color.button_yellow )
	name:SetAnchorPoint( 0.5 , 0.5 )
	layer:AddChild(name)
	PosTool.Center( name )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point[index] = Util:Sprite(icon_red_point)
    self.icon_red_point[index]:SetPosition( layer:GetContentWidth() - 5  , layer:GetContentHeight() - 10 )
    layer:AddChild( self.icon_red_point[index] , 20 )
    self.icon_red_point[index]:SetVisible(false)

	return layer
end

function Modules.TemplWelfare:OnCallBack( index , data )
	self.Image_everyday:SetVisible(false)
	self.Image_weekly:SetVisible(false)
	if index == 1 then
		self.Image_everyday:SetVisible(true)
	else
		self.Image_weekly:SetVisible(true)
	end
end

--领取
function Modules.TemplWelfare:OnButtonDraw()
    if not CallModuleFunc(ModuleType.ACTIVITY, "GetVipGiftData") then
        return
    end

	print("xy" , "OnButtonDraw")
	local info = {}
	info.vip_level = CallModuleFunc(ModuleType.ACTIVITY, "GetVipDrawLevel") + 1
	CallModuleFunc(ModuleType.ACTIVITY, "VipDailyGiftGetReq" , info )
end

--充值
function Modules.TemplWelfare:OnButtonRecharge()
	print("xy" , "OnButtonRecharge")
	CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
end


function Modules.TemplWelfare:__GetShowType()
	return Macros.Game.SystemID.VIP_WELFARE
end


function Modules.TemplWelfare:Currency( info )
	if self.diamond then
		self.diamond:RemoveSelf(true)
		self.diamond = nil
	end

	self.Button_award:SetText("")
	print("xy" , "===============info" , info)

	if type( info ) == "number" then
		local text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , info )
		self.diamond = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.btn , nil , nil , Macros.Color.button_yellow )
	else
	    --钻石
		self.diamond = Util:Label( info , Macros.TypefaceSize.normal , Macros.Color.btn , nil , nil , Macros.Color.button_yellow )
	end

	self.diamond:SetAnchorPoint( 0.5 , 0.5 )
	self.Button_award:AddChild( self.diamond )
	PosTool.Center( self.diamond )
end



--每周礼包
Modules = Modules or {}

Modules.TemplWelfareItem = Modules.TemplWelfareItem or BaseClass(GUI.ScrollItem)

function Modules.TemplWelfareItem:__init()
	self:InitTemplate( GUI.TemplateRegistry.TEMPL_ACTIVITY_WELFARE_ITEM )	
	self:__Getwidget()
end

function Modules.TemplWelfareItem:__delete()
    if self.recharge_list then
        self.recharge_list:DeleteMe()
        self.recharge_list = nil 
    end

    if self.discount then
    	self.discount:DeleteMe()
    	self.discount = nil
    end
end

function Modules.TemplWelfareItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__ShowUI()
end

function Modules.TemplWelfareItem:__Getwidget(data)
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--图标框
	self.Image_content = self:GetWidget("Image_content")
	--名字
	self.Text_name = Util:WidgetLabel( self:GetWidget("Text_name") , "" , Macros.TypefaceSize.largish , Macros.Color.keypoint )
	--购买按钮
	self.Button_purchase = Util:WidgetButton( 
												self:GetWidget("Button_purchase") , 
												CPPGameLib.GetString("activity_vipBuy") , 
												CPPGameLib.Handler(self,self.OnButtonPurchase) , 
												nil , nil , Macros.Color.button_yellow 
											)
end

function Modules.TemplWelfareItem:__ShowUI(data)
	-- CPPGameLib.PrintTable("xy",self.__data,"self.__data-------")
	--名字
	self.Text_name:SetText(self.__data.name)
	--价钱
	local tab = {
					{ tp = Util:GetMoneyIconImgData( self.__data.consumeLst[1].type ) , dx = 0.6 },
					{ zi = self.__data.consumeLst[1].num , ys = Macros.Color.content , jg = 5 , dx = Macros.TypefaceSize.normal },
				}
	local price = Util:LabCenter(tab)
	price:SetAnchorPoint( 0.5 , 0.5 )
	price:SetPosition( self.Button_purchase:GetPositionX() , self.Button_purchase:GetPositionY() - ( self.Button_purchase:GetContentHeight()/2 ) - price:GetContentHeight() + 10 )
	self.Image_item:AddChild( price )

	--道具列表
    local params =  
    {
        item_class = Modules.IconItem,
        item_width = 80.00 ,
        item_height = 80.00 ,
        horizon = true ,
        row = 1 ,
        col = 3 ,
        item_space_c = 10 ,
        view_width = self.Image_content:GetContentWidth() - 5 ,
    }
    -- 创建参数
    self.recharge_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.recharge_list:GetNode())
    PosTool.Center(self.recharge_list:GetNode())

	self.recharge_list:SetDataList( self.__data.itemLst )

	--打折
	self.discount = LogicGUI.LconDiscount.New( self.__data.discount )
	self.discount:SetAnchorPoint( 0.5 , 0.5 )
	self.discount:SetPosition( 12 , self.Image_content:GetContentHeight() + 17 )
	self.Image_content:AddChild( self.discount:GetNode() )

	--可购买次数
	local shop_can_buymax = Util:Label( CPPGameLib.GetString("shop_can_buymax" , self.__data.max - self.__data.num ) , Macros.TypefaceSize.normal , Macros.Color.red )
	shop_can_buymax:SetPosition(self.Button_purchase:GetPositionX() , self.Button_purchase:GetPositionY() + shop_can_buymax:GetContentHeight() + 20 )
	self.Image_item:AddChild( shop_can_buymax )

	--点击设置
	local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")
	if self.__data.buyLevel > vipLevel or (self.__data.max - self.__data.num) <= 0 then
		if (self.__data.max - self.__data.num) <= 0 then
			self.Button_purchase:SetText( CPPGameLib.GetString("shop_albuy") )
		end
		Util:GreyButton( self.Button_purchase )
	end

	--(
	local tips_1 = Util:Label( "(" , Macros.TypefaceSize.largish , Macros.Color.keypoint )
	self.Image_item:AddChild( tips_1 )
	PosTool.RightTo( self.Text_name , tips_1 , 3 )

    --vip图样
    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true )
    local tips_2 = Util:Sprite(vip_title_icon_sp)
    tips_2:SetScaleFactor( 0.8 )
    PosTool.RightTo( tips_1 , tips_2 , 5)
    self.Image_item:AddChild(tips_2) 
    
    --数值
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    local tips_3 = GUI.ImgFont.New(vipnum)
    -- tips_3:SetScaleFactor( 0.8 )
    tips_3:SetText(tostring( self.__data.buyLevel ))
    PosTool.RightTo( tips_2 , tips_3 , -5 )
    self.Image_item:AddChild(tips_3:GetNode()) 

   --可购买
	local tips_4 = Util:Label( CPPGameLib.GetString("activity_Purchasability") .. " )" , Macros.TypefaceSize.largish , Macros.Color.keypoint )
	self.Image_item:AddChild( tips_4 )
	PosTool.RightTo( tips_3 , tips_4 )


end

function Modules.TemplWelfareItem:OnButtonPurchase(data)

	local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", self.__data.consumeLst[1].type )

	if num < self.__data.consumeLst[1].num then
		local txt = "currency_insufficient_"..self.__data.consumeLst[1].type 
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
		return 
	end

	local info = {}
	info.gift_id = self.__data.id
	CallModuleFunc(ModuleType.ACTIVITY, "VipWeekGiftBuyReq" , info)

end