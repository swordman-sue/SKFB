
--[[
%% @module: 领地城市设置
%% @author: yjg
%% @created: 2017年3月6日15:24:17
--]]

LogicGUI = LogicGUI or {}

LogicGUI.ManorCity = LogicGUI.ManorCity or BaseClass()

function LogicGUI.ManorCity:__init( id , bg , centre , dark , ui_txt_name )

	self.id = id
	self.bg = bg
	self.centre = centre
	self.centre:SetTouchEnabled(false)
	self.dark = dark
	self.ui_txt_name = ui_txt_name
	self:SetShowUi()
end

function LogicGUI.ManorCity:__OpenCallback()
end

function LogicGUI.ManorCity:__Dispose()
	self:RemoveType()
	print("xy" , "__Dispose")
	if self.OnAddLayer then
		print("xy" , "self.OnAddLayer")
		self.OnAddLayer:DeleteMe()
		self.OnAddLayer = nil
	end
end

function LogicGUI.ManorCity:SetShowUi()

	--当前是谁的id
    self.roleTargetId = CallModuleFunc(ModuleType.MANOR, "GetRoleInfo").target_id
    --我的id
    self.myId =  CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id

	--获得城市的状态列表
	local lst = CallModuleFunc(ModuleType.MANOR, "GetCityType", self.id )
	--删除所有的状态
	self:RemoveType()

	if lst then
		if lst.patrol.type == "NotPatrol" then 		--没在巡逻
			
			if lst.award.type == "Award" then 			--有礼包
				--礼包
				self:SetAward()
			else  									--添加
				self:SetAdd()
			end

			WidgetUnity.SetWidgetListener(self.bg, nil, nil, function ( ... )
			end)

		else 										--在巡逻	
			if lst.riot.type == "Riot" then	--暴动			
				--暴动
				self:SetRiot()
			end

			--倒计时
			self:SetTime( lst.patrol.value )

			--设置城市巡逻中点击
			self.bg:SetTouchEnabled(true)
			WidgetUnity.SetWidgetListener(self.bg, nil, nil, function ( ... )
				if self.roleTargetId ~= self.myId then return end
				self:OnPatrol()
			end)
		end

	else 				--如果没有的话
		local lst = CallModuleFunc(ModuleType.MANOR, "GetCityType", self.id - 1 )

		if lst then 			--有没有他的前置
			self:SetSword()
		elseif self.id - 1 == 0 then 	--锁上
			self:SetSword()
		else
			self:SetLock( )
		end
		WidgetUnity.SetWidgetListener(self.bg, nil, nil, function ( ... )
		end)
	end
end


--加号
function LogicGUI.ManorCity:SetAdd( )

	if self.roleTargetId ~= self.myId then return end

	local ld_tj = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "ld_tj")
	self.add = Util:Button(ld_tj,function ( ... )
		self:OnAdd()
	end)
	self.centre:AddChild( self.add )
	PosTool.Center( self.add )
end

--锁
function LogicGUI.ManorCity:SetLock( )

	if self.roleTargetId ~= self.myId then return end
	self.dark:SetVisible(true)
	--触摸层
	self.layer = Util:Layout( self.bg:GetContentWidth() , self.bg:GetContentHeight() , function()
		self:OnLock( )
	end )
	self.layer:SetAnchorPoint(0.5,0.5)
	self.centre:AddChild(self.layer , 5)
	PosTool.Center(self.layer)

	local ld_s = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "ld_s")
	self.lock = Util:Button( ld_s , function ( ... )
		self:OnLock( )
	end )
	self.layer:AddChild( self.lock )
	PosTool.Center(self.lock)
end

--剑
function LogicGUI.ManorCity:SetSword()
	if self.roleTargetId ~= self.myId then return end
	--刀子
	self.effect_id , self.effect_con = BaseView.__CreateEffect(self, {res_name = "UI_xiaodao"}, self.centre)
	self.effect_con:SetScaleFactor(0.6)
	PosTool.Center(self.effect_con)

	--触摸层
	self.layer = Util:Layout( self.bg:GetContentWidth() , self.bg:GetContentHeight() , function()
		self:OnEffect()
	end )
	self.layer:SetAnchorPoint(0.5,0.5)
	self.centre:AddChild(self.layer)
	PosTool.Center(self.layer)
end

--暴动
function LogicGUI.ManorCity:SetRiot()
	--暴动
	self.layer = Util:Layout( self.bg:GetContentWidth() , self.bg:GetContentHeight() ,function ( ... )
		if self.roleTargetId ~= self.myId then 
			self:OnRiot()
		else
			self:OnPatrol()
		end
	end )
	self.layer:SetAnchorPoint(0.5,0.5)
	self.bg:AddChild( self.layer )
	PosTool.Center( self.layer )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "happen",true)
	self.riot = Util:Sprite(sp)
	self.riot:SetAnchorPoint(0.5,0.5)
	self.riot:SetPosition(self.layer:GetContentWidth()/2 , self.layer:GetContentHeight()/2 )
	self.layer:AddChild(self.riot)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "tips")
	local head = Util:Sprite(sp)
	head:SetAnchorPoint(0,0)
	head:SetPosition( self.riot:GetContentWidth() + 10 , self.riot:GetContentHeight() + 10 )
	self.riot:AddChild(head)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "monster_head")
	local btn = Util:Button(sp , function ( ... )
		if self.roleTargetId ~= self.myId then 
			self:OnRiot()
		else
			self:OnPatrol()
		end
	end)
	btn:SetPosition(head:GetContentWidth()/2 , head:GetContentHeight()/2)
	head:AddChild(btn)
	ActionManager.GetInstance():AboutRotateAnimation(btn , 0.1 , 15)
end

--奖励
function LogicGUI.ManorCity:SetAward()
	if self.roleTargetId ~= self.myId then return end

	self.layer = Util:Layout( self.bg:GetContentWidth() , self.bg:GetContentHeight() ,function ( ... )
		self:OnAward()
	end )
	self.layer:SetAnchorPoint(0.5,0.5)
	self.bg:AddChild( self.layer )
	PosTool.Center( self.layer )

	--礼包
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "receive",true)
	self.award = Util:Sprite(sp)
	self.award:SetAnchorPoint(0.5,0.5)
	self.award:SetPosition(self.layer:GetContentWidth()/2 , self.layer:GetContentHeight()/2 )
	self.layer:AddChild(self.award)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "tips")
	local head = Util:Sprite(sp)
	head:SetAnchorPoint(0,0)
	head:SetPosition( self.award:GetContentWidth() + 10 , self.award:GetContentHeight() + 10 )
	self.award:AddChild(head)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_dungeon_box")
	local btn = Util:Button(sp , function ( ... )
		self:OnAward()
	end)
	btn:SetScaleFactor(0.75)
	btn:SetPosition(head:GetContentWidth()/2 , head:GetContentHeight()/2)
	head:AddChild(btn)
	ActionManager.GetInstance():AboutRotateAnimation(btn , 0.1 , 15)

end

--倒计时
function LogicGUI.ManorCity:SetTime( endTime )
	local info = CallModuleFunc(ModuleType.MANOR, "GetCityType", self.id )
	--时间显示
    self.timeLab = Util:Label(vipnum , Macros.TypefaceSize.normal , Macros.Color.golden , nil , nil , Macros.Color.white_stroke)
    self.timeLab:SetAnchorPoint(0.5,1)
    self.ui_txt_name:AddChild(self.timeLab)
    PosTool.CenterBottom(self.timeLab, 0, 4)

	local time = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	if time <= 0 then
		return
	end

	self.time = GlobalTimerQuest:AddPeriodQuest(
	    GlobalCallbackMgr:AddCallBackFunc(function()
	    	--回复时间
	    	local time = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	        if time <= 0 then
	        	GlobalTimerQuest:CancelQuest(self.time)
	            self.time = nil
	           	CallModuleFunc(ModuleType.MANOR, "PlayerLandDataReq" , self.roleTargetId )
	        else
			    self.timeLab:SetString( TimeTool.TransTimeStamp("HH:MM:SS",time) )
	        end
	end),1,-1)
end

--删除所有特效
function LogicGUI.ManorCity:RemoveType( )
	if self.add then
		self.add:RemoveSelf(true)
		self.add = nil
	end
	if self.lock then
		self.lock:RemoveSelf(true)
		self.lock = nil
	end
	if self.effect_id then
		BaseView.__DelEffect(self, self.effect_id)
		self.effect_id = nil
		self.effect_con = nil
	end	
	if self.riot then
		self.riot:RemoveSelf(true)
		self.riot = nil
	end
	if self.timeLab then
		self.timeLab:RemoveSelf(true)
		self.timeLab = nil
	end
	if self.time then
		GlobalTimerQuest:CancelQuest(self.time)
	    self.time = nil
	end
	if self.award then
		self.award:RemoveSelf(true)
		self.award = nil
	end
	if self.layer then
		self.layer:RemoveSelf(true)
		self.layer = nil 
	end
	if self.dark then
		self.dark:SetVisible(false)
	end
end

--添加回调
function LogicGUI.ManorCity:OnAdd()
	print("xy" , self.id , "添加回调" )
	self.OnAddLayer = Modules.TempManorParticulars.New( self.id )
end

--锁回调
function LogicGUI.ManorCity:OnLock()
	print("xy" , self.id , "锁回调" )
	local lst = CallModuleFunc(ModuleType.MANOR, "GetCityType", self.id - 1 )
	local id = self.id - 1
	local city = CPPGameLib.GetConfig("LandCity", id )
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("manor_conquer" , city.name ) )
end

--剑回调
function LogicGUI.ManorCity:OnEffect()
	print("xy" , self.id , "剑回调" )
	Modules.TempManorChallenge.New( self.id )
end

--暴动回调
function LogicGUI.ManorCity:OnRiot()
	print("xy" , self.id , "暴动回调" )

	if CallModuleFunc(ModuleType.MANOR, "RiotSuppressTimes") <= 0 then
		
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("manor_noHelpTime" ) )
		return
	end

	local info = {}
	info.city_id = self.id
	info.friend_id = CallModuleFunc(ModuleType.MANOR , "GetRoleInfo").target_id

	CallModuleFunc(ModuleType.MANOR, "LandSuppressRiotReq", info )
end

--奖励回调
function LogicGUI.ManorCity:OnAward()
	print("xy" , self.id , "奖励回调" )
	Modules.TempManorObserve.New( self.id )
end

--巡逻中回调
function LogicGUI.ManorCity:OnPatrol()
	print("xy" , self.id , "巡逻中回调" )
	Modules.TempManorObserve.New( self.id )
end

--挑战界面
function LogicGUI.ManorCity:SetChallenge( data )
	print("xy" , self.id , "挑战界面" )
    --当前是谁的id
    local role = CallModuleFunc(ModuleType.MANOR, "GetRoleInfo")
    --我的id
    local id =  CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id

    if role.target_id ~= id then return end
    
	if data[2] then
		Modules.TemplManorPatrol.New( data )
	else
		self:OnAdd()
	end
end
