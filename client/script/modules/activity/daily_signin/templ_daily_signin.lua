
--[[
%% @module: 每日签到
%% @author: yjg
%% @created: 2017年1月14日15:17:21
--]]
-- sp_activity_publicity
-- Image_headline

Modules = Modules or {}

Modules.TemplDailySignin = Modules.TemplDailySignin or BaseClass(BaseTabView)

function Modules.TemplDailySignin:__init()
end

--选项卡界面创建完成回调
function Modules.TemplDailySignin:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplDailySignin:__OpenCallback()
    --签到事件
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_REGISTRATION,function (_, data)
        self:__SetShowUI( data )
    end)
    
	if not CallModuleFunc(ModuleType.ACTIVITY, "GetCurSignInDay") then
		--只请求一次
		CallModuleFunc(ModuleType.ACTIVITY, "SignInDataReq")
	else
		self:__SetShowUI()
	end
end

--选项卡界面关闭回调
function Modules.TemplDailySignin:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplDailySignin:__Dispose()
	if self.__zones_scrollview then
		self.__zones_scrollview:DeleteMe()
		self.__zones_scrollview = nil
	end
end

--获得所有组件
function Modules.TemplDailySignin:__Getwidget()
	--内容列表
	self.Image_contentLst = self:GetWidget("Image_contentLst") 
	--标题
	self.Image_headline = self:GetWidget("Image_headline")
end

function Modules.TemplDailySignin:__ShowUI()
	local sp = Resource.PathTool.GetUIResPath("activitybig/sp_activity_publicity" , true )
	local sp_activity_publicity = Util:Sprite( sp )
	sp_activity_publicity:SetAnchorPoint(0.5,0.5)
	self.Image_headline:AddChild( sp_activity_publicity )
	PosTool.Center( sp_activity_publicity )
	
	--累计签到
	local activity_aggregateRegistration = Util:Label( CPPGameLib.GetString("activity_aggregateRegistration") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	activity_aggregateRegistration:SetAnchorPoint( 0 , 0.5 )
	activity_aggregateRegistration:SetPosition( self.Image_headline:GetContentWidth()/2 - 60 , self.Image_headline:GetContentHeight()/2 - 50 )
	self.Image_headline:AddChild( activity_aggregateRegistration )

	--已经签到
	self.registration = Util:Label( 10 , Macros.TypefaceSize.normal , Macros.Color.green )
	PosTool.RightTo( activity_aggregateRegistration , self.registration )
	self.Image_headline:AddChild( self.registration )

	--签到上限
	self.registrationMax = Util:Label( "/" .. 30 .. CPPGameLib.GetString("common_next") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	PosTool.RightTo( self.registration , self.registrationMax )
	self.Image_headline:AddChild( self.registrationMax )	

	self.activity_aggregateRegistration_ = Util:RichText( CPPGameLib.GetString("activity_aggregateRegistration_" , 2) , Macros.TypefaceSize.normal , nil , nil , Macros.Color.lilyWhite_hex )
	PosTool.RightTo( self.registrationMax , self.activity_aggregateRegistration_  , 15 )
	self.Image_headline:AddChild( self.activity_aggregateRegistration_ )	

	local params =  
	{
	    item_class = Modules.TemplDailySigninItem,
	    item_width = 106,
	    item_height = 106,
	    col = 6,
	    row = 5,
	    item_space_c = 4,
	    item_space_r = 5,
	    view_height = self.Image_contentLst:GetContentHeight() - 5 ,
	}
	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.Image_contentLst:AddChild(self.__zones_scrollview:GetNode())
	PosTool.Center(self.__zones_scrollview:GetNode())

	self.__zones_scrollview:SetDataList( CallModuleFunc(ModuleType.ACTIVITY, "GetSignInCnf") )
end

function Modules.TemplDailySignin:__SetShowUI()
	--更新签到状态
    self.__zones_scrollview:ItemIter(function(index, item, data)
    	item:UpdateSigninStatus(data)
    end)

    self.registration:SetString( CallModuleFunc(ModuleType.ACTIVITY, "GetCurSignInDay" ) )
    local cnf = CallModuleFunc(ModuleType.ACTIVITY, "GetSignInCnf")
    self.registrationMax:SetString(  "/" .. #cnf .. CPPGameLib.GetString("common_next") )
    PosTool.RightTo( self.registration , self.registrationMax )

    local day = CallModuleFunc(ModuleType.ACTIVITY, "GetSignInNexBigPrize")
    self.activity_aggregateRegistration_:SetText( CPPGameLib.GetString("activity_aggregateRegistration_" , day ) )
end

function Modules.TemplDailySignin:__GetShowType()
	return Macros.Game.SystemID.DAILY_SIGNIN
end


--
-- @brief 物品背包条目 
-- @author: yjg
-- @date: 2016年8月20日18:31:32
--

Modules = Modules or {}

Modules.TemplDailySigninItem = Modules.TemplDailySigninItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDailySigninItem:__init(data,direct)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplDailySigninItem:__delete()
end

function Modules.TemplDailySigninItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(80,80)
    self.layout:SetAnchorPoint(0.5,0.5)

    return self.layout
end

function Modules.TemplDailySigninItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TemplDailySigninItem:__AddToParent()
end

function Modules.TemplDailySigninItem:__ShowUI( data )
	--背景
	self.sp_activity_bg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_signin_nobg") )
	self.sp_activity_bg :SetAnchorPoint(0.5,0.5)
	self.layout:AddChild(self.sp_activity_bg)
	PosTool.Center(self.sp_activity_bg)

	--图标
    self.icon_layer = LogicGUI.IconLayer.New(false)
    self.icon_layer:SetData({id = data.reward_item_id , num = data.reward_item_num})
    self.sp_activity_bg:AddChild(self.icon_layer:GetNode())
    self.icon_layer:SetTouchSwallow(false)
	self.icon_layer:SetTouchCallback(function()
		local isGettype_ = CallModuleFunc(ModuleType.ACTIVITY, "GetSignInIsGet" , data.day )
		local is_last_signin_day_diff_today = CallModuleFunc(ModuleType.ACTIVITY, "IsLastSignInDayDiffToday" )

		if isGettype_ == true and is_last_signin_day_diff_today == true then
			CallModuleFunc(ModuleType.ACTIVITY, "SignInReq")
		else
			local data_ = {}
			data_.id = data.reward_item_id
			data_.num = data.reward_item_num
			data_.type = ObjIDTool.GetInfo( data.reward_item_id )

			local info = data_
			local obj_type = info.type
			--物品Tips
			if obj_type == Macros.Game.ObjType.ITEM then
				local __ , config_item = ObjIDTool.GetInfo(info.id)
				--装备碎片
				if config_item.type == Macros.Game.ObjType.ITEM and config_item.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS then
				    TemplEquipInfo.GetInstance(info)
				else
				    LogicGUI.TemplItemInfo.New(info)
				end
			end
		end
	end)
    PosTool.Center(self.icon_layer, 0 , -3 )    
    
    if data.double_reward_need_vip_level ~= 0 then
		--背景
		self.sp_activity_tips = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_tips") )
		self.sp_activity_tips:SetAnchorPoint(0 , 1 )
		self.sp_activity_tips:SetPosition( 0 , self.sp_activity_bg:GetContentHeight()  )
		self.sp_activity_bg:AddChild(self.sp_activity_tips , 999 )

		--vip		
		local tips_key = GlobalCustomizedLogicAdapterMgr:Call("GetDailySignDoubleRewardTipsKey") or "activity_VipTips"
		local vip = Util:Label( CPPGameLib.GetString( tips_key , data.double_reward_need_vip_level ) , Macros.TypefaceSize.minimum , Macros.Color.lilyWhite )
		vip:SetAnchorPoint( 0.5 , 0.5)
		self.sp_activity_tips:AddChild( vip )
		PosTool.Center(vip)
	end

	if data.big_prize then
		local sp_activity_particularly =  Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_particularly") )
		sp_activity_particularly:SetAnchorPoint(0.5 , 0.5)
		sp_activity_particularly:SetPosition( self.icon_layer:GetNode():GetContentWidth() - 4 , self.icon_layer:GetNode():GetContentHeight() - 4 )
		self.icon_layer:AddChild( sp_activity_particularly , 999 )

		local day = Util:Name( CPPGameLib.GetString("activity_day" , data.day) , Macros.TypefaceSize.minimum , 102 )
		sp_activity_particularly:AddChild( day , 999 )
		PosTool.Center( day , 0 , -2 )
	end

	--更新签到状态
	self:UpdateSigninStatus(data)
end

--更新签到状态
function Modules.TemplDailySigninItem:UpdateSigninStatus(data)
   	local cur_signin_day = CallModuleFunc(ModuleType.ACTIVITY, "GetCurSignInDay" )
    	local is_last_signin_day_diff_today = CallModuleFunc(ModuleType.ACTIVITY, "IsLastSignInDayDiffToday" )

    	--签到满后当前签到天数被重置为0，且最后一次签到是今天
    	if not is_last_signin_day_diff_today and cur_signin_day == 0 then
    		self:__SetEffType_3(data)
    	else
    		--是否已签到
			local is_day_has_signin = GlobalModulesMgr:Call (ModuleType.ACTIVITY , "CheckDayHasSignIn" , data.day)
			if is_day_has_signin then
				self:__SetEffType_1(data)
			else
				--是否可签到
				local is_day_has_signin = GlobalModulesMgr:Call( ModuleType.ACTIVITY , "CheckDayHasSignIn" , data.day , 1 )
				if is_day_has_signin and is_last_signin_day_diff_today then
					self:__SetEffType_2(data)
				end          	
			end
    	end
end

function Modules.TemplDailySigninItem:__SetEffType_1( data )
	self:__SetEffTypeRemove_1()
	self:__SetEffTypeRemove_2()

	local is_day_has_signin = CallModuleFunc(ModuleType.ACTIVITY, "CheckDayHasSignIn", data.day)
	if is_day_has_signin then
		self.sp_activity_bg:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_signin_bg") )

		--签到
		self.sp_activity_signin = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_activity_signin") )
		self.sp_activity_signin:SetAnchorPoint(0.5,0.5)
		self.icon_layer:AddChild( self.sp_activity_signin , 999 )
		PosTool.Center( self.sp_activity_signin )

	end
end

function Modules.TemplDailySigninItem:__SetEffTypeRemove_1( )
	self.icon_layer:ShowIconShade(false)
	if self.tips then
		self.tips:RemoveSelf(true)
		self.tips = nil
	end

	if self.sp_activity_signin then
		self.sp_activity_signin:RemoveSelf(true)
		self.sp_activity_signin = nil
	end
end

function Modules.TemplDailySigninItem:__SetEffType_2( data )
	self:__SetEffTypeRemove_1()
	self:__SetEffTypeRemove_2()

	local is_day_has_signin = CallModuleFunc(ModuleType.ACTIVITY , "CheckDayHasSignIn" , data.day , 1 )
	local is_last_signin_day_diff_today = CallModuleFunc(ModuleType.ACTIVITY, "IsLastSignInDayDiffToday" )
	if is_day_has_signin and is_last_signin_day_diff_today and not self.effectId then
		local effectCon = nil
		self.effectId , effectCon = self:__CreateEffect({res_name = "UI_qiandao"}, self.icon_layer:GetNode() , LogicGUI.IconLayer.InnerZOrder.ICON )
		PosTool.Center(effectCon , 0 , 0 )
	end    
end

function Modules.TemplDailySigninItem:__SetEffType_3( data )
	self:__SetEffTypeRemove_1()
	self:__SetEffTypeRemove_2()

	--签到
	self.sp_activity_signin = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_activity_signin") )
	self.sp_activity_signin:SetAnchorPoint(0.5,0.5)
	self.icon_layer:AddChild( self.sp_activity_signin , LogicGUI.IconLayer.InnerZOrder.ICON_QUALITY )
	PosTool.Center( self.sp_activity_signin)

end

function Modules.TemplDailySigninItem:__SetEffTypeRemove_2( )
	--背景特效
	if self.effectId then
		self:__DelEffect(self.effectId)
		self.effectId = nil
	end	
end