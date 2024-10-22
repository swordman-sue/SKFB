
--[[
%% @module: 摇钱树
%% @author: yjg
%% @created: 2017年1月19日18:34:20
--]]

--不用

Modules = Modules or {}

Modules.TemplFairyMoney = Modules.TemplFairyMoney or BaseClass(BaseTabView)

function Modules.TemplFairyMoney:__init()
end

--选项卡界面创建完成回调
function Modules.TemplFairyMoney:__LoadedCallback()
	self.opshop = false
	CallModuleFunc(ModuleType.ACTIVITY, "YcsDataReq")
end

--选项卡界面打开回调
function Modules.TemplFairyMoney:__OpenCallback()

	if self.opshop == false then
		self.opshop = true
		self:__Getwidget()
		self:__ShowUI()
	end
	
	self.key = true

    --摇钱树事件
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_MONEY,function (_, data)
        self:__SetShowUI( data )

    end)

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_GET_MONEY,function (_, data)
    	if self.Affirm then
			CPPActionManager.DelayTo( self.Affirm  , 1 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
		    	GlobalEffectMgr:SetEffectAnim( self.effect_id , RenderObj.ActionIndex.IDLE )
			end ) )
		end
    end)

    local data = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsBeforeTime")
    if data then
		self:__SetShowUI()
	end
end

--选项卡界面关闭回调
function Modules.TemplFairyMoney:__CloseCallback()

	self.Button_ok:SetEnabled(true)
	local r, g, b, a = unpack(Macros.Color.button_yellow)
	self.Button_ok:SetOutline(r, g, b, a, 2)

	--背景特效
	self:RemoveEffects()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplFairyMoney:__Dispose()
end

--获得所有组件
function Modules.TemplFairyMoney:__Getwidget()
	--背景图
    self.Panel_ImgBg = self:GetWidget("Panel_ImgBg")
    local sp = Resource.PathTool.GetUIResPath("citycommonslices/sp_activity_money")
    self.Panel_ImgBg:SetBackGroundImage(sp , TextureResType.LOCAL )
    
    --文字背景
	self.Image_textBg = self:GetWidget("Image_textBg")

	self.effect_id , effect_root = self:__CreateEffect({res_name = "UI_yaoqianshu" , type = Effect.EffectType.SKELETON}, self.Panel_ImgBg)
	PosTool.CenterBottom(effect_root)

	--确定按钮
    self.Button_ok = Util:WidgetButton( self:GetWidget("Button_ok") , CPPGameLib.GetString("activity_rock") , CPPGameLib.Handler(self,self.OnButtonOk) , nil , nil , nil  )
end

function Modules.TemplFairyMoney:__ShowUI()
	local btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "UI_yaoqianshujinbi_saoguang"},self.Panel_ImgBg,2)
	PosTool.Center(BtnEffect_con , -50 , 180 )

	--提示
	local mainLab = CPPGameLib.GetString("activity_tips")
	local tab = {  
			{zi = "0", ys = Macros.Color.Special },
			{zi = "0", ys = Macros.Color.Special },
		}

	self.Affirm = Util:Echo(mainLab , tab , Macros.Color.lilyWhite )
	self.Affirm:SetAnchorPoint(0.5, 0.5)
	self.Affirm:SetPosition(self.Image_textBg:GetContentWidth()/2 , self.Image_textBg:GetContentHeight() - self.Affirm:GetContentHeight() )
	self.Image_textBg:AddChild(self.Affirm)

	local num = ConfigAdapter.Common.GetAggregateMoneyNum()
	self.activity_tips_1 = Util:Label( CPPGameLib.GetString("activity_tips_1", num ) , Macros.TypefaceSize.slightly , Macros.Color.lilyWhite )
	self.activity_tips_1:SetAnchorPoint(0.5 , 0.5)
	self.activity_tips_1:SetPosition( self.Image_textBg:GetContentWidth()/2 , self.activity_tips_1:GetContentHeight() + 3 )
	self.Image_textBg:AddChild(self.activity_tips_1)

	--提示2 今日已摇：%s 次
	self.activity_already = Util:Label( CPPGameLib.GetString("activity_already") )
	self.activity_already:SetPosition( 
										self.Button_ok:GetPositionX() , 
										self.Button_ok:GetPositionY() - (self.Button_ok:GetContentHeight()/2) +  16
									)
	self.Panel_ImgBg:AddChild(self.activity_already)

	--倒计时
	self.time = Util:Label( "00:00:00" , Macros.TypefaceSize.normal , Macros.Color.Special  )
	PosTool.RightTo( self.activity_already , self.time , 0 - (self.time:GetContentWidth()/2) )
	self.Panel_ImgBg:AddChild(self.time)
end

function Modules.TemplFairyMoney:__SetShowUI()
	if self.Affirm then
		self.Affirm:RemoveSelf(true)
		self.Affirm = nil
	end

	--提示
	self.activity_already:SetString( CPPGameLib.GetString("activity_already",  CallModuleFunc(ModuleType.ACTIVITY, "GetYcsUsedNum") ) )

	--累计数量
	local aggregateNum = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsBigAggregateMoney")
	local mainLab = CPPGameLib.GetString("activity_tips")
	local tab = {  
			{zi = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsAggregateMoney") , ys = Macros.Color.Special },
			{zi = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsBigAggregateMoney") , ys = Macros.Color.Special },
		}

	if aggregateNum == 0 then
		mainLab = CPPGameLib.GetString("activity_tips_2")
		tab = {  
				{zi = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsAggregateMoney") , ys = Macros.Color.Special },
			}	
		self:SetEffects()
	else
		self:RemoveEffects()
	end 
	self.Affirm = Util:Echo(mainLab , tab , Macros.Color.YcsDataReq )
	self.Affirm:SetPosition(self.Image_textBg:GetContentWidth()/2 , self.Image_textBg:GetContentHeight() - self.Affirm:GetContentHeight() - 3 )
	self.Image_textBg:AddChild(self.Affirm)

	local money = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsAggregateMoney")
	-- if money == 0 then
	-- 	self.Affirm:SetVisible(false)
	-- 	self.activity_tips_1:SetVisible(false)
	-- else
	-- 	self.Affirm:SetVisible(true)
	-- 	self.activity_tips_1:SetVisible(true)	
	-- end
	self:Countdown()
end

--倒计时
function Modules.TemplFairyMoney:Countdown()
	local num = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsTodayGetTimes")
	if num >= ConfigAdapter.Common.GetAggregateMoneyMax() then
		self.time:SetString(" ")
		self.Button_ok:SetEnabled(false)
		Util:GreyButton( self.Button_ok )
		return
	end

    local func = nil
    local function moveAnimate( ... )
		--CD
		local time = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsBeforeTime")
		--现在时间
		local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		--CD时间
		time = time + ConfigAdapter.Common.GetAggregateMoneyTime()	
		local runTime = time - server_time
        if runTime <=0 then
        	local num = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsTodayGetTimes")
        	local max = ConfigAdapter.Common.GetAggregateMoneyMax()
        	if num < max then
	        	self.Button_ok:SetEnabled(true)
	        	local r, g, b, a = unpack(Macros.Color.button_yellow)
				self.Button_ok:SetOutline(r, g, b, a, 2)

	        else
	        	self.Button_ok:SetEnabled(false)
	        	Util:GreyButton( self.Button_ok )
	        end

	        local aggregateNum = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsBigAggregateMoney")
	        if aggregateNum == 0 then
	        	self.Button_ok:SetEnabled(false)
	        end
        	CPPActionManager.StopAllActions(self.Panel_ImgBg)
        	self.time:SetString( " " )
        else
			self.Button_ok:SetEnabled(false)
			Util:GreyButton( self.Button_ok )
        	self.time:SetString( TimeTool.TransTimeStamp( "HH:MM:SS" , runTime ) )
	        CPPActionManager.DelayTo(self.Panel_ImgBg , 1 , func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

function Modules.TemplFairyMoney:OnButtonOk()
	self.Button_ok:SetEnabled(false)

	GlobalEffectMgr:SetEffectAnim( self.effect_id , RenderObj.ActionIndex.RUN )
	
	local btnEffect_id , btnEffect_con = self:__CreateEffect({id = 100012, loop = false, end_callback = function ( ... )
	end },self.Panel_ImgBg , 2 )
	PosTool.Center(btnEffect_con , -50 , 180 )

	Util:GreyButton( self.Button_ok )
	CallModuleFunc(ModuleType.ACTIVITY, "YcsRewardReq")
end

--特效
function Modules.TemplFairyMoney:SetEffects()

	if self.key == false then
		return 
	end
	self.key = false

	if not self.effects then
		self.effects = Util:Layout( 100 , 100 , function( ... )
			LogicGUI.TemplActivityTips.New()
		end )
		self.effects:SetAnchorPoint(0.5,0.5)
		self.Panel_ImgBg:AddChild(self.effects)
		PosTool.Center(self.effects)

		self.btnEffect_id1 , BtnEffect_con = self:__CreateEffect({id = 100052}, self.Panel_ImgBg )
		PosTool.Center(BtnEffect_con , 0 , -50 )

		CPPActionManager.DelayTo( self.effects  , 0.4 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
			self.btnEffect_id2 , BtnEffect_con = self:__CreateEffect({res_name = "UI_yaoqianshuguang2"} , self.Panel_ImgBg )
			PosTool.Center(BtnEffect_con , 0 , -40 )
		end ) )

	else
		self.btnEffect_id2 , BtnEffect_con = self:__CreateEffect({res_name = "UI_yaoqianshuguang2"} , self.Panel_ImgBg )
		PosTool.Center(BtnEffect_con , 0 , -40 )
	end

end

--删除特效
function Modules.TemplFairyMoney:RemoveEffects()
	if self.btnEffect_id1 then
		self:__DelEffect(self.btnEffect_id1)
		self.btnEffect_id1 = nil
	end	

	if self.btnEffect_id2 then
		self:__DelEffect(self.btnEffect_id2)
		self.btnEffect_id2 = nil
	end	

	if self.effects then
		self.effects:RemoveSelf(true)
		self.effects = nil
	end

end

function Modules.TemplFairyMoney:__GetShowType()
	return Macros.Game.SystemID.YCS
end






--
-- @brief 领取
-- @author: yjg
-- @date: 2016年9月27日11:44:59
--
-- templ_activity_tips
LogicGUI = LogicGUI or {}

LogicGUI.TemplActivityTips = LogicGUI.TemplActivityTips or BaseClass(GUI.Template)

function LogicGUI.TemplActivityTips:__init(index)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_TIPS)	
end

function LogicGUI.TemplActivityTips:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplActivityTips:SetData(data)
	print("////")
end

function LogicGUI.TemplActivityTips:__Getwidget( ... )
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --条
    self.Image_1 = self:GetWidget("Image_1") 
    --内层容器
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
    --关闭按钮
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), nil , CPPGameLib.Handler( self,self.OnBtnExit ) )  
    --居中内容
    self.Text_content = self:GetWidget("Text_content") 
    --领取
    self.Button_no = Util:WidgetButton(
    										self:GetWidget("Button_no"), 
    										CPPGameLib.GetString("manor_get") , 
    										CPPGameLib.Handler( self,self.OnBtnBuy ) ,
    										nil,nil,Macros.Color.button_yellow
    										)  
end

function LogicGUI.TemplActivityTips:__ShowUI( ... )	
	--标题
	self.lab = Util:Label(CPPGameLib.GetString("activity_insurance") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	self.Image_1:AddChild(self.lab)
	PosTool.Center(self.lab)

	local config = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.GOLD )
    --头像
    local __icon = LogicGUI.IconLayer.New()
    self.Text_content:AddChild(__icon:GetNode())
    PosTool.Center(__icon:GetNode())
    local iconData = {id = config.big_icon , num = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsAggregateMoney") }
    __icon:SetData(iconData)

end

--购买
function LogicGUI.TemplActivityTips:OnBtnBuy( ... )
	CallModuleFunc(ModuleType.ACTIVITY, "YcsRewardReq")
	self:OnBtnExit()
end

function LogicGUI.TemplActivityTips:OnBtnExit( ... )
	self.__bg:SetVisible(false)
	--缩放效果
	local node = self:GetNode()
	CPPActionManager.MoveTo(node, 0.13 , node:GetPositionX() + 100  , node:GetPositionY() - 20 )
	CPPActionManager.ScaleTo(node, 0.13, 0.1, GlobalCallbackMgr:AddCallBackFunc( function ( ... )
		self:DeleteMe()
	end ))
end
