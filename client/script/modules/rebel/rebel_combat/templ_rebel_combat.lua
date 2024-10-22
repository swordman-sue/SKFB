--
-- @brief 叛军战斗入口
-- @author: yjg
-- @date: 2016年9月19日18:26:29
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplRebelCombat = LogicGUI.TemplRebelCombat or BaseClass(GUI.Template)

function LogicGUI.TemplRebelCombat:__init( data , fun)
	self.data = data
	self.fun = fun
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI(data)
	end

    self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE,function (_, data)
    	self:UpCrusade()
    end)

	self:InitTemplate(GUI.TemplateRegistry.REBEL_COMBAT)	
end

function LogicGUI.TemplRebelCombat:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
	if self.screen then
		GlobalTimerQuest:CancelQuest(self.screen)
		self.screen = nil
	end
	if self.modes then
		self.modes:DeleteMe()
		self.modes = nil
	end
end

function LogicGUI.TemplRebelCombat:__Getwidget()
	--背景
	self.Image_floor = self:GetWidget("Image_floor") 
	--上框
	self.Image_TopBG = self:GetWidget("Image_TopBG") 
	--副本图片
	self.Panel_ChallengeBG = self:GetWidget("Panel_ChallengeBG") 
	--框
	self.Image_frame = self:GetWidget("Image_frame") 
	--货币类型
	self.Panel_itemIcon = self:GetWidget("Panel_itemIcon") 
	--进度条
	self.Panel_progress = self:GetWidget("Panel_progress") 
	--添加按钮
	self.Button_addNum = Util:WidgetButton(
												self:GetWidget("Button_addNum") ,
												nil , CPPGameLib.Handler( self,self.OnButtonAddNum ) )
	--货币名
	self.Text_itemName = self:GetWidget("Text_itemName") 
	--货币数量
	self.Text_itemNum = self:GetWidget("Text_itemNum") 
	--下矿
	self.Image_bottom = self:GetWidget("Image_bottom") 
	--全力
	self.Button_exert = Util:WidgetButton(
											self:GetWidget("Button_exert"),
											CPPGameLib.GetString("rebel_quanli") , 
											CPPGameLib.Handler( self,self.OnBtnQuanli ) ,
											nil,nil,Macros.Color.button_yellow
										)
	--普通
	self.Button_common = Util:WidgetButton(
											self:GetWidget("Button_common"),
											CPPGameLib.GetString("rebel_putong") , 
											CPPGameLib.Handler( self,self.OnBtnPutong ) ,
											nil,nil,Macros.Color.button_red
										)
	
	--双倍
	self.Image_double = self:GetWidget("Image_double") 
	--标题
	self.Text_name = self:GetWidget("Text_name") 
	--提示1
	self.Image_tips1 = self:GetWidget("Image_tips1") 
	--提示2
	self.Image_tips2 = self:GetWidget("Image_tips2") 

	--关闭
	self.Button_close = Util:WidgetButton(
											self:GetWidget("Button_close"),nil , function ( ... )
												self:DeleteMe()
											end
										)	
end	

function LogicGUI.TemplRebelCombat:__ShowUI(data)

	local rebel_attack = Util:Label( CPPGameLib.GetString("rebel_assault") , Macros.TypefaceSize.popup , Macros.Color.headline )
	rebel_attack:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
	rebel_attack:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild( rebel_attack )

	local shortlineSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")
	--左边
    local shortline_1 = Util:Sprite9( shortlineSp , {50,10} )
    shortline_1:SetContentSize(shortline_1:GetContentWidth() , shortline_1:GetContentHeight())
    shortline_1:SetAnchorPoint( 1 , 0.5 )
    shortline_1:SetPosition( rebel_attack:GetPositionX() - (rebel_attack:GetContentWidth()/2) - 10 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(shortline_1)

    --左边
    local shortline_2 = Util:Sprite9( shortlineSp , {50,10})
    shortline_2:SetContentSize(shortline_2:GetContentWidth() , shortline_2:GetContentHeight())
    shortline_2:SetAnchorPoint( 1 , 0.5 )
    shortline_2:SetFlipX(true)
    shortline_2:SetPosition( rebel_attack:GetPositionX() + (rebel_attack:GetContentWidth()/2) + 10 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(shortline_2)

	--名字
	local name = Util:Name(data.name , Macros.TypefaceSize.popup , data.quality)
	name:SetPosition(10 , self.Image_frame:GetContentHeight()/2 - 65 )
	name:SetAnchorPoint(0,0.5)
	self.Image_frame:AddChild(name)

	local tp = CPPGameLib.GetConfig("GameLevel", data.map_id)
	local image_challenge_bg = Util:Sprite(Resource.PathTool.GetUIResPath("language/chs/rebel/1"))
	self.Panel_ChallengeBG:AddChild(image_challenge_bg,0)
	PosTool.Center(image_challenge_bg)

	--等级
	local lv = Util:Name( CPPGameLib.GetString("common_lev6",data.level) , Macros.TypefaceSize.popup )
	PosTool.RightTo(name,lv , 10)
	self.Image_frame:AddChild(lv)

	--逃走时间value
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local text = data.endTime - server_time
	if data.endTime - server_time <= 0 then
		text = 0
	end
	self.endTime = Util:Label(TimeTool.TransTimeStamp( "HH:MM:SS" , text ) , Macros.TypefaceSize.popup , Macros.Color.red ,nil,nil, Macros.Color.red_stroke )
	self.endTime:SetPosition(self.Image_frame:GetContentWidth() - 10 , self.Image_frame:GetContentHeight()/2 - 65 )
	self.endTime:SetAnchorPoint(1,0.5)
	self.Image_frame:AddChild(self.endTime)
	self:EndTime(data)

	--逃走时间txt
	local endTime = Util:Name( CPPGameLib.GetString("rebel_taozou") , Macros.TypefaceSize.popup )
	endTime:SetAnchorPoint(1,0.5)
	endTime:SetPosition(self.endTime:GetPositionX() - self.endTime:GetContentWidth() - 10 , self.Image_frame:GetContentHeight()/2 - 65 )
	self.Image_frame:AddChild(endTime)

	--模型
	self.modes = LogicGUI.Model.New({info_id = data.modes_id, parent = self.Image_frame , zorder = 99 })
    self.modes:SetPosition(80,51)


    local hb1 = Util:LabCenter( {
    								{ zi = CPPGameLib.GetString("rebel_consume") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
    								{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.WANTED) , dx = 0.8 },
    								{ zi = ConfigAdapter.Common.GetRebelConsume1() , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.content },
					    	} )
    hb1:SetPosition(self.Button_common:GetContentWidth()/2 , -20 )
    self.Button_common:AddChild(hb1)

    local hb2 , hb2_lst = Util:LabCenter( {
								{ zi = CPPGameLib.GetString("rebel_consume") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
								{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.WANTED) , dx = 0.8 },
								{ zi = ConfigAdapter.Common.GetRebelConsume2() , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.content },
				    		} )
    hb2:SetPosition(self.Button_exert:GetContentWidth()/2 , -20 )
    self.Button_exert:AddChild(hb2)

	local config = {}

	table.insert( config , { start_time = ConfigAdapter.Common.GetRebelConsumeHalve()[1] , over_time = ConfigAdapter.Common.GetRebelConsumeHalve()[2] } )
	table.insert( config , { start_time = ConfigAdapter.Common.GetRebelExploitDouble()[1] , over_time = ConfigAdapter.Common.GetRebelExploitDouble()[2] } )

	for i,v in ipairs(config) do
		local start = TimeTool.TransTimeStamp( "HH:MM" , (v.start_time * 0.01 * 60 * 60) )
		local over = TimeTool.TransTimeStamp( "HH:MM" , (v.over_time * 0.01 * 60 * 60) )

		local sp = nil
		local color = nil
		local bool = CallModuleFunc(ModuleType.REBEL, "GetActivityOpen", start , over )
		if bool then
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.REBEL, "opbg")
			color = Macros.Color.red_color
			if i == 1 then
				hb2_lst[3]:SetString(  ConfigAdapter.Common.GetRebelConsume2() / 2 )
			end
		else
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.REBEL, "nobg")
			color = Macros.Color.content
		end

		self["Image_tips"..i]:SetImage( sp )

		local time = Util:Label( start .." - ".. over , Macros.TypefaceSize.normal , color )
		time:SetPosition(self["Image_tips"..i]:GetContentWidth()/2 , self["Image_tips"..i]:GetContentHeight() - 20)
		self["Image_tips"..i]:AddChild(time)

		local rebel_tips = Util:Label( CPPGameLib.GetString("rebel_tips"..i) , Macros.TypefaceSize.slightly , color , self["Image_tips"..i]:GetContentWidth() - 20  )
		rebel_tips:SetTextHorizontalAlign(LabelEx.ALIGIN_MIDDLE)
		self["Image_tips"..i]:AddChild(rebel_tips)
		PosTool.Center( rebel_tips , 0 , -10 )

	end

	local hpBG = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")
	local hp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")

	--进度条
	self.bar = GUI.ProgressBar9Layer.New(hp, hpBG, self.Panel_progress:GetContentWidth() - 5 , 24 , self.Panel_progress:GetContentWidth(), self.Panel_progress:GetContentHeight(), 100, 100)
	self.bar:SetAnchorPoint(0.5, 0.5)
	self.bar:SetPosition(self.Panel_progress:GetContentWidth()/2 , self.Panel_progress:GetContentHeight()/2)
	self.Panel_progress:AddChild(self.bar:GetNode())

    --生命值
    self.hp = Util:LabCenter({
								{ zi = CPPGameLib.GetString("rebel_hp") , ys = 0 , mb = true , dx = Macros.TypefaceSize.normal },
								{ zi = data.sumHp - data.total_damage , ys = 0 , mb = true , dx = Macros.TypefaceSize.normal},
								{ zi = "/" , ys = 0 , mb = true , dx = Macros.TypefaceSize.normal},
								{ zi = data.sumHp , ys = 0 , mb = true , dx = Macros.TypefaceSize.normal},
							})
    self.hp:SetPosition(self.bar:GetNode():GetPositionX(),self.bar:GetNode():GetPositionY())
    self.hp:SetAnchorPoint(0.5,0.5)
    self.bar:AddChild(self.hp)

    if data.total_damage == 0  then
		self.bar:SetProgress(100)
	else
		self.bar:SetProgress(100 - ( data.total_damage / data.sumHp * 100 ) )
	end

	local now , max = CallModuleFunc(ModuleType.ROLE,"GetWanted")
	self.currency = Util:Label( now .. "/" .. max , Macros.TypefaceSize.normal , Macros.Color.content )
	self.currency:SetAnchorPoint(1,0.5)
	self.currency:SetPosition( self.Button_addNum:GetPositionX() - (self.Button_addNum:GetContentHeight()/2) - 5, self.Button_addNum:GetPositionY() )
	self.Image_floor:AddChild(self.currency)

	self.icon = Util:MoneyControl( Macros.Game.RolePropType.WANTED )
	self.icon:SetAnchorPoint(0.5,0.5)
	self.icon:SetPosition( self.currency:GetPositionX() - self.currency:GetContentWidth() - (self.icon:GetContentWidth()/2) - 5 , self.Button_addNum:GetPositionY() )
	self.Image_floor:AddChild(self.icon)	

	self.rebel_attack = Util:Label( CPPGameLib.GetString("rebel_attack") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	self.rebel_attack:SetAnchorPoint(1,0.5)
	self.rebel_attack:SetPosition( self.icon:GetPositionX() - (self.icon:GetContentWidth()/2) , self.Button_addNum:GetPositionY() )
	self.Image_floor:AddChild(self.rebel_attack)

end

--更新怪物的数据
function LogicGUI.TemplRebelCombat:SetData( data )
	if self.hp then
		self.hp:RemoveSelf(true)
		self.hp = nil
	end

    --生命值
    self.hp = Util:LabCenter({
								{ zi = CPPGameLib.GetString("rebel_hp") , ys = 0 , mb = true , dx = Macros.TypefaceSize.slightly },
								{ zi = data.sumHp - data.total_damage , ys = 0 , mb = true , dx = Macros.TypefaceSize.slightly},
								{ zi = "/" , ys = 0 , mb = true , dx = Macros.TypefaceSize.slightly},
								{ zi = data.sumHp , ys = 0 , mb = true , dx = Macros.TypefaceSize.slightly},
							})
    self.hp:SetPosition(self.LoadingBar_1:GetContentWidth()/2,self.LoadingBar_1:GetContentHeight()/2)
    self.hp:SetAnchorPoint(0.5,0.5)
    self.LoadingBar_1:AddChild(self.hp)

    if data.total_damage == 0  then
		self.LoadingBar_1:SetPercent(100)
	else
		self.LoadingBar_1:SetPercent(100 - ( data.total_damage / data.sumHp * 100 ) )
	end
end

--更新讨伐令数量
function LogicGUI.TemplRebelCombat:UpCrusade( data )
	local now , max = CallModuleFunc(ModuleType.ROLE,"GetWanted")
	self.currency:SetString( now .. "/" .. max )
	self.currency:SetPosition( self.Button_addNum:GetPositionX() - (self.Button_addNum:GetContentHeight()/2) - 5, self.Button_addNum:GetPositionY() )
	self.icon:SetPosition( self.currency:GetPositionX() - self.currency:GetContentWidth() - (self.icon:GetContentWidth()/2) - 5 , self.Button_addNum:GetPositionY() )
	self.rebel_attack:SetPosition( self.icon:GetPositionX() - (self.icon:GetContentWidth()/2) , self.Button_addNum:GetPositionY() )
end

--普通按钮
function LogicGUI.TemplRebelCombat:OnBtnPutong( ... )
	local checkBattle = CallModuleFunc(ModuleType.REBEL, "CheckBattleCondition", Macros.Game.RebelAttackType.NORMAL , self.data )
	if checkBattle then
		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.REBEL, self.data.uid, Macros.Game.RebelAttackType.NORMAL, self.data.level)
		self:OnBtnExit()
	else
		self:OnButtonAddNum()
	end
end

--全力按钮
function LogicGUI.TemplRebelCombat:OnBtnQuanli( ... )
	local checkBattle = CallModuleFunc(ModuleType.REBEL, "CheckBattleCondition", Macros.Game.RebelAttackType.FULL , self.data )
	if checkBattle then
		CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.REBEL, self.data.uid, Macros.Game.RebelAttackType.FULL, self.data.level)
		self:OnBtnExit()
	else
		self:OnButtonAddNum()
	end
end

--关闭按钮
function LogicGUI.TemplRebelCombat:OnBtnExit( ... )
	self.fun()
end

--逃走倒计时
function LogicGUI.TemplRebelCombat:EndTime( data )
	if self.screen then
		GlobalTimerQuest:CancelQuest(self.screen)
		self.screen = nil
	end

	self.screen = GlobalTimerQuest:AddPeriodQuest(
	GlobalCallbackMgr:AddCallBackFunc(function()
		local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		if data.endTime - server_time <= 0 then
			GlobalTimerQuest:CancelQuest(self.screen)
			self.screen = nil
			self:OnBtnExit()
			CallModuleFunc(ModuleType.REBEL, "RebelDataReq")
		else
			local time = data.endTime - server_time
			if data.endTime - server_time <= 0 then
				time = 0
			end
			self.endTime:SetString( TimeTool.TransTimeStamp( "HH:MM:SS" , time ) )
		end
	end),0.1,-1)
end

--添加按钮
function LogicGUI.TemplRebelCombat:OnButtonAddNum( ... )
	LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.WANTED ,function ( ... )
	end)
end

