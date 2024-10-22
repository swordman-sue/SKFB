--
-- @brief 公会副本次数购买
-- @author: yjg
-- @date: 2016年11月22日11:00:17
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionBuytime = LogicGUI.TemplUnionBuytime or BaseClass(GUI.Template)

function LogicGUI.TemplUnionBuytime:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.labCenter = nil
		self.num = 0
		self:__Getwidget()
		self:__ShowUI()
	end

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_BUYTIME)	
end

function LogicGUI.TemplUnionBuytime:__delete()
end

function LogicGUI.TemplUnionBuytime:__Getwidget()
	--大背景
	self.Panel_storeBuy = self:GetWidget("Panel_storeBuy") 
	--框
	self.Image_1 = self:GetWidget("Image_1") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--内容列表
	self.Image_contentBg = self:GetWidget("Image_contentBg") 
	--添加按钮
	self.Button_add = Util:WidgetButton( self:GetWidget("Button_add") , nil , function ( ... )
		self:OnButtonAdd(1)
	end,nil,nil,nil,nil,0)
	self.Image_20 = self:GetWidget("Image_20") 
	--减少按钮
	self.Button_sub = Util:WidgetButton( self:GetWidget("Button_sub") , nil , function ( ... )
		self:OnButtonSub(1)
	end,nil,nil,nil,nil,0)
	self.Image_21 = self:GetWidget("Image_21") 
	--添加10
	self.Button_add10 = Util:WidgetButton( self:GetWidget("Button_add10") , nil , function ( ... )
		self:OnButtonAdd(10)
	end,nil,nil,nil,nil,0)
	self.Image_22 = self:GetWidget("Image_22") 
	--减少10
	self.Button_sub10 = Util:WidgetButton( self:GetWidget("Button_sub10") , nil , function ( ... )
		self:OnButtonSub(10)
	end,nil,nil,nil,nil,0)
	self.Image_23 = self:GetWidget("Image_23") 
	--选择
	self.Image_select = self:GetWidget("Image_select") 
	--取消
	self.Button_cancel = Util:WidgetButton( 
											self:GetWidget("Button_cancel") , 
											CPPGameLib.GetString("common_cancel") , 
											CPPGameLib.Handler(self,self.OnButtonClose) ,
											Macros.TypefaceSize.largish , nil , Macros.Color.button_red)
	--确定
	self.Button_buy = Util:WidgetButton( 
											self:GetWidget("Button_buy") , 
											CPPGameLib.GetString("common_confirm") , 
											CPPGameLib.Handler(self,self.OnButtonBuy),
											Macros.TypefaceSize.largish , nil , Macros.Color.button_yellow)
end

function LogicGUI.TemplUnionBuytime:__ShowUI()
	--标题
	local light = Util:Label(CPPGameLib.GetString("worldBoss_buyTime") , Macros.TypefaceSize.popup , Macros.Color.white)
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

	--当前次数
	local nowTime = Util:LabCenter( {
									{ zi = CPPGameLib.GetString("worldBoss_nowTime") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.largish},
									{ zi = CallModuleFunc(ModuleType.UNION, "GetNowChallengeTime") , dx = Macros.TypefaceSize.largish , ys = Macros.Color.content },
								} )
	nowTime:SetAnchorPoint(0.5,1)
	nowTime:SetPosition(self.Image_contentBg:GetContentWidth()/2 , self.Image_contentBg:GetContentHeight() - nowTime:GetContentHeight() )
	self.Image_contentBg:AddChild(nowTime)

	--还可购买
	local mayBuyTime = Util:LabCenter( {
									{ zi = CPPGameLib.GetString("worldBoss_mayBuyTime") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.largish},
									{ zi = CallModuleFunc(ModuleType.UNION, "GetUnionTranscriptBuyTime") , dx = Macros.TypefaceSize.largish , ys = Macros.Color.content },
								} )
	mayBuyTime:SetAnchorPoint(0.5,1)
	mayBuyTime:SetPosition(self.Image_contentBg:GetContentWidth()/2 ,nowTime:GetPositionY() - nowTime:GetContentHeight() - 11 )
	self.Image_contentBg:AddChild(mayBuyTime)

	self.pitchNum = 0
	self.lab_setnum = Util:Label(self.pitchNum , nil , Macros.Color.content )
	self.lab_setnum:SetPosition(self.Image_select:GetContentWidth()/2 ,self.Image_select:GetContentHeight()/2 )
	self.Image_select:AddChild(self.lab_setnum)

	self.cnf = ConfigAdapter.Common.GetUnionEctypeTime()
	self:OnButtonAdd(1)
end

--关闭按钮
function LogicGUI.TemplUnionBuytime:OnButtonClose()
	self:DeleteMe()
end

--购买按钮
function LogicGUI.TemplUnionBuytime:OnButtonBuy()
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	self.num = self.num or 0
	if userDate.diamond >= self.num and self.num ~= 0 then
		local info = {}
		info.buy_times = self.pitchNum
		CallModuleFunc(ModuleType.UNION, "GuildFbBattleTimesBuyReq",info)
		self:DeleteMe()	

	elseif self.num <= 0 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_today_can_notbuy") )
	else
		local txt = "currency_insufficient_"..Macros.Game.RolePropType.DIAMOND
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
	end
end

--添加按钮
function LogicGUI.TemplUnionBuytime:OnButtonAdd(index)
	local max = CallModuleFunc(ModuleType.UNION, "GetUnionTranscriptBuyTime")
	print("添加按钮",index)
	if self.pitchNum >= max then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_today_can_notbuy") )
		return
	elseif self.pitchNum + index >= max then
		self.pitchNum = max
		self.lab_setnum:SetString(self.pitchNum)
	else
		self.pitchNum = self.pitchNum + index
		self.lab_setnum:SetString(self.pitchNum)
	end
	self:SetPrice( true )
end

--减少按钮
function LogicGUI.TemplUnionBuytime:OnButtonSub(index)
	print("减少按钮",index)
	if self.pitchNum <= 1 then
		return 
	elseif self.pitchNum - index <= 1  then
		self.pitchNum = 1
		self.lab_setnum:SetString(1)
	else 
		self.pitchNum = self.pitchNum - index
		self.lab_setnum:SetString(self.pitchNum)
	end
	self:SetPrice()
end

--价格
function LogicGUI.TemplUnionBuytime:SetPrice( type_ )
	local used = CallModuleFunc(ModuleType.UNION, "GetUnionUsedUpBuyTime")
	local now = self.pitchNum + used
	self.num = 0
	if self.pitchNum <= 1 then
		if self.cnf[now] then
			self.num = self.cnf[now]
		else
			self.num = self.cnf[#self.cnf]
		end	
	else
		for i = now , used + 1  ,-1 do
			if self.cnf[i] then
				self.num = self.num + self.cnf[i]
			else
				self.num = self.num + self.cnf[#self.cnf]
			end
		end
	end

	if self.labCenter then
		self.labCenter:RemoveSelf(true)
		self.labCenter = nil
	end
	self.labCenter = Util:LabCenter({ 
									{zi = CPPGameLib.GetString("shop_total_price_Text") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.largish },
									{tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.65 , jg = 5 },
									{zi = self.num , dx = Macros.TypefaceSize.largish , jg = 5 ,ys = Macros.Color.content },
								 })
	self.labCenter:SetPosition(self.Image_contentBg:GetContentWidth()/2 , 20 )
	self.Image_contentBg:AddChild(self.labCenter)
end