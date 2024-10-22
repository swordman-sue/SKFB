
--[[
%% @module: 领取体力
%% @author: yjg
%% @created: 2017年1月14日15:17:21
--]]

--不用

Modules = Modules or {}

Modules.TemplMedicine = Modules.TemplMedicine or BaseClass(BaseTabView)

function Modules.TemplMedicine:__init()
end

--选项卡界面创建完成回调
function Modules.TemplMedicine:__LoadedCallback()
	self.sp_activity_medicine = {}
	self.sp_activity_medicine.vase = {}
	self.sp_activity_medicine.type_ = {}
	self.sp_activity_medicine.getType = {}
	self.sp_activity_medicine.time = {}
	self.sp_activity_medicine.showType = {}

	self.effect_id = {}
	self.effect_con = {}

	CallModuleFunc(ModuleType.ACTIVITY, "TqtDdataReq")
end

--选项卡界面打开回调
function Modules.TemplMedicine:__OpenCallback()
	self.index = nil

	self:__Getwidget()
	self:__ShowUI()
    self:__SetShowUI()
    --体力事件
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_MEDICINE,function ( _ , key )
        self:__SetShowUI()
        if key then
        	self:Stamp()
        end

        self.countdownType = false
    end)
end

--选项卡界面关闭回调
function Modules.TemplMedicine:__CloseCallback()
	if self.medicineBg then
		self.medicineBg:RemoveSelf(true)
		self.medicineBg = nil
	end
	self:RemoveEff()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplMedicine:__Dispose()
end

--获得所有组件
function Modules.TemplMedicine:__Getwidget()
	--背景图
    self.Image_MedicineBg = self:GetWidget("Image_MedicineBg")
    local sp = Resource.PathTool.GetUIResPath("citycommonslices/sp_activity_medicine_bg")
    self.Image_MedicineBg:SetImage(sp , TextureResType.LOCAL )

    --药瓶
    self.Image_medicine = self:GetWidget("Image_medicine")

    --对话框
	self.Image_bubble = self:GetWidget("Image_bubble")

	--确定按钮
    self.Button_ok = Util:WidgetButton( 
    										self:GetWidget("Button_ok") , 
    										CPPGameLib.GetString("activity_take") , 
    										CPPGameLib.Handler(self,self.OnButtonOk) ,
    										nil,nil,Macros.Color.button_yellow
    									)
    self.Panel_face = {}
    self.Image_icon = {}
    for i = 1 , 3 do
    	self.Panel_face[i] = self:GetWidget("Panel_face"..i)
    	self.Panel_face[i]:SetVisible(false)
    	self.Image_icon[i] = self:GetWidget("Image_icon"..i)
    	self.Image_icon[i]:SetVisible(false)
    end
end

function Modules.TemplMedicine:__ShowUI()

	self.medicineBg = Util:Layout( self.__root_widget:GetContentWidth() , self.__root_widget:GetContentHeight() )
	self.medicineBg:SetAnchorPoint(0.5,0.5)
	self.Image_MedicineBg:AddChild( self.medicineBg )
	PosTool.Center( self.medicineBg )

	--居中层
	local layou = Util:Layout()
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetPosition(self.medicineBg:GetContentWidth()/2 ,self.medicineBg:GetContentHeight()/2 + 70)
	self.medicineBg:AddChild(layou)

	local BtnEffect_con = nil
	self.btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.Button_ok , 999)
	BtnEffect_con:SetAnchorPoint(0.5,0.5)
	PosTool.Center( BtnEffect_con )

	local posY = self.Image_bubble:GetContentHeight() + 25
	--表
	local cnf = CallModuleFunc(ModuleType.ACTIVITY, "GetTQTcnf")

	for i,v in ipairs( cnf ) do
		--时间
		local start_time = string.sub(v.start_time , 0 , 2) .. ":" .. string.sub(v.start_time , -2)
		local over_time = string.sub(v.over_time , 0 , 2) .. ":" .. string.sub(v.over_time , -2)

		self.sp_activity_medicine.time[i] = Util:Name( v.name.. CPPGameLib.GetString("common_colon") .. string.format("%s - %s",start_time,over_time) )
		self.sp_activity_medicine.time[i]:SetAnchorPoint( 0 , 1 )
		posY = posY - (self.sp_activity_medicine.time[i]:GetContentHeight() + 10)
		self.sp_activity_medicine.time[i]:SetPosition( 30 , posY )
		self.Image_bubble:AddChild(self.sp_activity_medicine.time[i])
	end

	-- 药瓶子
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_bottle")
	self.vase = Util:Sprite(sp)
	self.vase:SetAnchorPoint( 0.5 , 0 )
	self.vase:SetPosition( self.Image_medicine:GetContentWidth()/2 , 0 )
	self.Image_medicine:AddChild( self.vase )
	self.vase:SetVisible(false)


	self.effect_id , effect_root = self:__CreateEffect({res_name = "UI_pingzi" , type = Effect.EffectType.SKELETON}, self.Image_medicine)
	PosTool.CenterBottom(effect_root)
	GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
	
	self.tili_effect_id , tili_effect_root = self:__CreateEffect({res_name = "UI_huodongtili"}, effect_root )
	PosTool.Center(tili_effect_root , 0 , 50 )
	GlobalEffectMgr:SetEffectVisible(self.tili_effect_id , false)

	local text = {  
		{zi = ConfigAdapter.Common.GetManual() * #cnf , ys = Macros.Color.green },
	}

	local Affirm = Util:Echo( CPPGameLib.GetString("activity_takeTips") , text )
	Affirm:SetPosition( self.Button_ok:GetPositionX()  , self.medicineBg:GetContentHeight()/2 - 225 )
	self.medicineBg:AddChild(Affirm)
end


function Modules.TemplMedicine:__SetShowUI()
	-- --表
	local cnf = CallModuleFunc(ModuleType.ACTIVITY, "GetTQTcnf")
	for i,v in ipairs(cnf) do
		local text = CallModuleFunc(ModuleType.ACTIVITY, "GetTQTIsTakeTime" , v.start_time , v.over_time )

		self:SetIcon( i , text )
		local type_ = CallModuleFunc(ModuleType.ACTIVITY, "GetTQTRecordTidy" , i )
		if text ~= "kaizhe" or type_ then
			self.sp_activity_medicine.time[i]:SetColor(unpack(Macros.Quality[Macros.Game.QualityType.WHITE]))
			self.sp_activity_medicine.time[i]:SetStrokeColor(unpack(Macros.Quality_Stroke[Macros.Game.QualityType.WHITE]))
			Util:GreyButton( self.Button_ok )

			GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
			GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
			GlobalEffectMgr:SetEffectVisible(self.tili_effect_id , false)

			GlobalEffectMgr:SetEffectVisible(self.effect_id , false)
			self.vase:SetVisible(true)
		else
			self.index = i

			self.sp_activity_medicine.time[i]:SetColor(unpack(Macros.Quality[Macros.Game.QualityType.GREEN]))
			self.sp_activity_medicine.time[i]:SetStrokeColor(unpack(Macros.Quality_Stroke[Macros.Game.QualityType.GREEN]))

			self.Button_ok:SetEnabled(true)
			local r, g, b, a = unpack(Macros.Color.button_yellow)
			self.Button_ok:SetOutline(r, g, b, a, 2)

			GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
			GlobalEffectMgr:SetEffectVisible(self.effect_id , true)
			GlobalEffectMgr:SetEffectVisible(self.tili_effect_id , true)

			self:Countdown( i )
			self.vase:SetVisible(false)
			break
		end
	end
end

function Modules.TemplMedicine:OnButtonOk()
	if CallModuleFunc(ModuleType.ACTIVITY, "CheckTQT") then
		CallModuleFunc(ModuleType.ACTIVITY, "TqtRewardReq")
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("activity_noTakeTips") )
	end
end

function Modules.TemplMedicine:RemoveEff()
	if self.effect_id then
		self:__DelEffect(self.effect_id)
		self.effect_id = nil
	end
	if self.btnEffect_id then
		self:__DelEffect(self.btnEffect_id)
		self.btnEffect_id = nil
	end
end

function Modules.TemplMedicine:Stamp()
	if self.index ~= 0 then
	   self.Panel_face[ self.index ]:SetVisible(true)

	   self:SetIcon( self.index , "dianle" )

		self.__parent_view:PlayActionTimeline("animation" .. (self.index - 1) , false , function()
			self.Image_icon[self.index]:SetVisible(true)
			self.Panel_face[ self.index ]:SetVisible(false)
	    end)
	end
end

function Modules.TemplMedicine:SetIcon( id , key )
	local x_05 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "x_05")
	local x_13 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "x_13")

	local sp = nil
	if key == "kaizhe" then	
		sp = nil
	elseif key == "guole" then
		sp = x_05
	elseif key == "meidao" then
		sp = nil
	elseif key == "dianle" then
		sp = x_13
	end

	local type_ = CallModuleFunc(ModuleType.ACTIVITY, "GetTQTRecordTidy" , id )
	if type_ then
		sp = x_13
	end

	if sp then
		self.Image_icon[ id ]:SetImage(sp)
		self.Image_icon[id]:SetVisible(true)
	end
end

--倒计时
function Modules.TemplMedicine:Countdown( index )
	if self.countdownType then
		return
	end
	self.countdownType = true

	local cnf = CallModuleFunc(ModuleType.ACTIVITY, "GetTQTcnf")
	local func = nil
	local function moveAnimate( ... )
		--当前时间 
		local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)
		local n_time = n_shi .. n_fen
		if tonumber( n_time ) > tonumber( cnf[index].over_time ) then
			CallModuleFunc(ModuleType.ACTIVITY, "TqtDdataReq")
		else
			CPPActionManager.DelayTo( self.medicineBg , 1 , func)
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()
end