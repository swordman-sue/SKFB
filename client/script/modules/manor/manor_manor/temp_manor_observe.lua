
--[[
%% @module: 领地观察巡逻
%% @author: yjg
%% @created: 2016年10月8日20:29:00
--]]

Modules = Modules or {}

Modules.TempManorObserve = Modules.TempManorObserve or BaseClass(GUI.Template)

function Modules.TempManorObserve:__init(id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.id = id
		self.key = true

		self:__GetServe()
		self:__Getwidget()
		self:__ShowUI()
		self:RondaAction()


	    self:BindGlobalEvent(Macros.Event.ManorModule.MANOR_EVENT_UP, function (_, data)
	        --刷新事件
	        self:UpEvent( data.desc )
	        --下次领取时间
	        self:ShowNextTime( data.patrol_type , data.finally_time )
			--显示当前巡逻的时间
	        self:ShowContinueTime( data.patrol_type , data.time_type , data.start_patrol_time )
	        --显示获得的道具
	        self:ShowGetPack()
			--刷新UI
			self:__SetShowUi()
		end)
	end

	self:InitTemplate(GUI.TemplateRegistry.MANOR_CHALLENGE)	
end


function Modules.TempManorObserve:__delete()
	-- if self.lst_list then
	-- 	self.lst_list:DeleteMe()
	-- 	self.lst_list = nil
	-- end
	if self.modes then
		self.modes:DeleteMe()
		self.modes = nil
	end

    if self.btnEffect_id then
		self:__DelEffect(self.btnEffect_id)
		self.btnEffect_id = nil
	end
end

function Modules.TempManorObserve:__GetServe()
	local info = {}
	info.role_id = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id
	info.city_id = self.id
	CallModuleFunc(ModuleType.MANOR, "LandCityDetailsReq",info)
end

function Modules.TempManorObserve:__Getwidget()
	--居中
	self.Image_BG = self:GetWidget("Image_BG") 
	--标题
	self.Image_light = self:GetWidget("Image_light") 
	--背景
	self.Image_setting = self:GetWidget("Image_setting") 
	--内容框
	self.Image_controlPanel = self:GetWidget("Image_controlPanel") 
	--说明
	self.Image_award = self:GetWidget("Image_award") 
	self.Image_award:SetVisible(false)
	--条目
	self.Image_1 = self:GetWidget("Image_1") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--icon
	self.Image_iconBg = self:GetWidget("Image_iconBg") 
	--战斗力
	self.Text_power = self:GetWidget("Text_power") 
	--关闭
	self.Button_close = Util:WidgetButton(
											self:GetWidget("Button_close") , 
											nil , 
											CPPGameLib.Handler(self,self.OnButtonClose)
											)
	--挑战
	self.Button_challenge = Util:WidgetButton(
												self:GetWidget("Button_challenge") , 
												CPPGameLib.GetString("manor_get") , 
												CPPGameLib.Handler(self,self.OnButtonChallenge),
												nil,nil,Macros.Color.button_yellow )

	self.btnEffect_id , self.BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.Button_challenge,999)
	PosTool.Center( self.BtnEffect_con )
end

function Modules.TempManorObserve:__ShowUI()
	--领地表
	local landCityCnf = CPPGameLib.GetConfig("LandCity", self.id)
	--关卡表
	local gameLevelCnf = CPPGameLib.GetConfig("GameLevel", landCityCnf.map_id)
	--获得代表的怪物的id
	local battleCnf = CPPGameLib.GetConfig("Battle", gameLevelCnf.battle_list[1]).representativeId
	--获得怪物的信息
	local monsterCnf = CPPGameLib.GetConfig("Monster", battleCnf)

	--标题
	local light = Util:Label( landCityCnf.name , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

-----------
	self.Image_setting:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR_BG, landCityCnf.build_bg ) )
	
	--怪物地光
	self.bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
	self.bg_shadow:SetPosition( 100 , 45 )
	self.Image_setting:AddChild(self.bg_shadow)

	print( "xy" , "gameLevelCnf.icon_id" , gameLevelCnf.icon_id )

	--怪物模型
	self.modes = LogicGUI.Model.New({
										info_id = nil , 
										parent = self.bg_shadow ,
										model_type = LogicGUI.ModelType.MONSTER,
									})
	self.modes:SetPosition( self.bg_shadow:GetContentWidth()/2 , self.bg_shadow:GetContentHeight()/2 )

	--名字
	self.name = Util:Name(monsterCnf.name , Macros.TypefaceSize.normal , monsterCnf.quality)
	self.name:SetAnchorPoint(0.5,0.5)
	self.name:SetPosition(self.bg_shadow:GetContentWidth()/2 , 0 )
	self.bg_shadow:AddChild(self.name)

-----------
	--事件
    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetAnchorPoint(0.5,0.5)
    self.lst_list:SetContentSize(self.Image_controlPanel:GetContentWidth()-10 , (self.Image_controlPanel:GetContentHeight()/2) - 25)
    self.lst_list:SetPosition(self.Image_controlPanel:GetContentWidth()/2 , self.Image_controlPanel:GetContentHeight()/2 + 78 )
	self.Image_controlPanel:AddChild(self.lst_list)  

-----------
	--下次收益时间
	local time = Util:Label( CPPGameLib.GetString("manor_next") , Macros.TypefaceSize.slightly , Macros.Color.content )
	time:SetAnchorPoint(0,0.5)
	time:SetPosition(self.Image_1:GetPositionX() - self.Image_1:GetContentWidth()/2 + 5 , (self.Image_1:GetContentHeight()/2) + self.Image_1:GetPositionY() + (time:GetContentHeight()/2) )
	self.Image_controlPanel:AddChild(time)

	self.time = Util:Label( "00:00:00" , Macros.TypefaceSize.noslightlyrmal , Macros.Color.red_color )
	PosTool.RightTo( time , self.time)
	self.Image_controlPanel:AddChild(self.time)

-----------
	--巡逻持续时间
	self.patrolText = Util:Label( CPPGameLib.GetString("manor_underway") , Macros.TypefaceSize.slightly , Macros.Color.red_color )
	self.patrolText:SetPosition( self.Button_challenge:GetPositionX() , self.Button_challenge:GetPositionY() + (self.Button_challenge:GetContentHeight()/2) + self.patrolText:GetContentHeight() + 15 )
	self.patrolText:SetAnchorPoint(0.5,0.5)
	self.Image_1:AddChild(self.patrolText)

	self.patrolTime = Util:Label( "00:00:00" , Macros.TypefaceSize.slightly , Macros.Color.red_color )
	self.patrolTime:SetPosition( self.Button_challenge:GetPositionX() , self.patrolText:GetPositionY() - self.patrolText:GetContentHeight() - 5)
	self.patrolTime:SetAnchorPoint(0.5,0.5)
	self.Image_1:AddChild(self.patrolTime)

-----------
	--胜利奖励
	local manor_winAward = Util:Label( CPPGameLib.GetString("manor_accumulativeAward") , Macros.TypefaceSize.normal , Macros.Color.headline )
	manor_winAward:SetAnchorPoint(0,0.5)
	manor_winAward:SetPosition( 0 , self.Text_name:GetContentHeight()/2  )
	self.Text_name:AddChild(manor_winAward)
-----------
	--icon列表

    local common =  
    {
        item_class = Modules.IconItem,
        horizon = true,
        item_width = 90,
        item_height = 90,
        row = 1,
        col = 4,
        selectable = true,
        update_move_func = function ( data )
        end,
        view_width = self.Image_iconBg:GetContentWidth() - 10 ,
    }

    -- 普通列表
    self.common = GUI.ScrollView.New(common)
    self.Image_iconBg:AddChild(self.common:GetNode())
    PosTool.Center(self.common:GetNode())

end

function Modules.TempManorObserve:__SetShowUi( )
	self.roleInfo = CallModuleFunc(ModuleType.MANOR, "GetRoleInfo")
	if not self.roleInfo.city_list[self.id] then
		return 
	end
    local info_id = self.roleInfo.city_list[self.id].hero_info_id

    local cnf = CPPGameLib.GetConfig("Hero", info_id )
    CPPGameLib.PrintTable("xy", cnf.model_id ,"info_id")

    --模型
	self.modes:SetModelType( LogicGUI.ModelType.HERO )
	self.modes:SetInfoID(info_id)
	-- self.modes:PlayAnimation(RenderObj.ActionIndex.IDLE)
	print("xy","====-=-=-=")

	--名字 
	self.name:SetString( cnf.name )
	self.name:SetColor(unpack(Macros.Quality[cnf.init_quality]))
	self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[cnf.init_quality]))
end

function Modules.TempManorObserve:OnButtonClose( )
	self:DeleteMe()
end

function Modules.TempManorObserve:OnButtonChallenge( )
	-- local info = {}
	-- info.city_id = self.id
	-- CallModuleFunc(ModuleType.MANOR, "LandGetPatrolAwardReq",info)
	-- self:OnButtonClose()
	self:ShowGetItem()
end

--刷新事件
function Modules.TempManorObserve:UpEvent( lst )
	self.lst_list:RemoveAllItems()
	for i,v in ipairs(lst) do
		local layer = Util:Layout( self.Image_controlPanel:GetContentWidth()-10 , 0 )
		local text = Util:RichText( v , Macros.TypefaceSize.normal , layer:GetContentWidth()-10 ,0 )
		layer:SetContentSize( layer:GetContentWidth() , text:GetContentHeight() + 5 )
		text:SetAnchorPoint( 0 , 0.5)
		text:SetPosition( 5 , layer:GetContentHeight()/2 )
		layer:AddChild(text)
		self.lst_list:PushBackCustomItem(layer)
	end

	self.lst_list:ScrollToBottom()
end

--显示下次收益的时间
function Modules.TempManorObserve:ShowNextTime( type , item )
	if self.key ~= true then
		return 
	end

	local Land = CPPGameLib.GetConfig("LandPatrol", type)
    local func = nil
    local function moveAnimate( ... )
    	local time = item + (Land.interval_time * 60)
    	local endTime = time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if endTime <=0 then
            self:__GetServe()
	        self.time:SetString("00:00:00")
            CPPActionManager.StopAllActions(self.Image_BG)
        else
        	self.time:SetString( TimeTool.TransTimeStamp("HH:MM:SS", endTime) )
	        CPPActionManager.DelayTo(self.Image_BG,1,func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

--显示当前巡逻的时间
function Modules.TempManorObserve:ShowContinueTime( patroltype , type , beginTime )
	if self.key ~= true then 
		return 
	end

	local cnf = CPPGameLib.GetConfig("LandPatrol",patroltype)
	--持续的时间
    local func = nil
    local function moveAnimate( ... )
		local continue = cnf["time_type_data_"..type][1] * 60
		local endTime = beginTime + continue
		local time = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if time <=0 then
            CPPActionManager.StopAllActions(self.Image_BG)
            CPPActionManager.StopAllActions(self.bg_shadow)

            self.modes:PlayAnimation(RenderObj.ActionIndex.IDLE)

            self.key = false
            self.Button_challenge:SetEnabled(true)
            self:__GetServe()
            -- CallModuleFunc(ModuleType.MANOR, "PlayerLandDataReq")

            self.BtnEffect_con:SetVisible(true)
            
            self.patrolTime:SetString(" ")
            self.patrolText:SetString( CPPGameLib.GetString("manor_probeEnd") )
            self.patrolText:SetPosition( self.patrolText:GetPositionX() , self.patrolText:GetPositionY() - 20 )

        else

        	self.BtnEffect_con:SetVisible(false)
        	self.patrolTime:SetString( TimeTool.TransTimeStamp("HH:MM:SS", time) )
	        CPPActionManager.DelayTo(self.Image_BG,1,func)
	        self.Button_challenge:SetEnabled(false)

        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end


function Modules.TempManorObserve:ShowGetPack()
	local pack = CallModuleFunc(ModuleType.MANOR, "GetAwardLst",self.id)
	CPPGameLib.PrintTable("xy", pack ,"物品合成响应")

	local t1 = {}
	for i , v in ipairs( pack.all ) do
		local num = v.num
		if v.double == 1 then
			num = v.num * 2
		end
		v.num = num
		table.insert( t1 , v )
	end
    self.common:SetDataList( t1 )
end

--巡逻的动作
function Modules.TempManorObserve:RondaAction( ... )

	local is_left = "Right"
	local inteval_time = 3

	local func = nil

	local function Left( ... )
		
		CPPActionManager.MoveTo(self.bg_shadow, inteval_time, self.Image_setting:GetContentWidth() - 80 , self.bg_shadow:GetPositionY() , func)
		if self.modes:GetInfoId() then
			self.modes:SetDir(MathTool.HORZ_RIGHT)
			self.modes:PlayAnimation(RenderObj.ActionIndex.RUN)
			is_left = "Left"
		end
	end

	local function Right( ... )
		CPPActionManager.MoveTo(self.bg_shadow, inteval_time, 80 , self.bg_shadow:GetPositionY() ,func)
		if self.modes:GetInfoId() then
			self.modes:SetDir(MathTool.HORZ_LEFT)
			self.modes:PlayAnimation(RenderObj.ActionIndex.RUN)
			is_left = "Right"
		end
	end


	local function Delay( ... )
		if self.modes:GetInfoId() then
			self.modes:PlayAnimation(RenderObj.ActionIndex.IDLE)
		end
		if is_left == "Left" then
			CPPActionManager.DelayTo(self.Image_BG,1,GlobalCallbackMgr:AddCallBackFunc(Right))
		else
			CPPActionManager.DelayTo(self.Image_BG,1,GlobalCallbackMgr:AddCallBackFunc(Left))
		end
	end

	func = GlobalCallbackMgr:AddCallBackFunc(Delay)
	Delay()
end

function Modules.TempManorObserve:ShowGetItem()

	local t1 = {}
	local t2 = {}
	local lst = CallModuleFunc(ModuleType.MANOR, "GetAwardLst",self.id)
	t1 = lst.all
	t2 = lst.double
	-- for i,v in ipairs(lst) do
	-- 	table.insert(t1 , {id = v.id , num = v.num , double = v.double } )
	-- 	if v.double == 1 then
	-- 		table.insert(t2 , {id = v.id , num = v.num/2 , double = v.double } )
	-- 	end
	-- end

	if #t2 ~= 0 then
		--有双倍的
		print("有双倍的")
		Modules.TempManorDoubleAward.New( t1 , t2 ,function ( ... )
			local info = {}
			info.city_id = self.id
			CallModuleFunc(ModuleType.MANOR, "LandGetPatrolAwardReq",info)
		end)
	else
		print("没双倍的")
		--没双倍的
		Modules.TempManorAward.New( t1 ,function ( ... )
			local info = {}
			info.city_id = self.id
			CallModuleFunc(ModuleType.MANOR, "LandGetPatrolAwardReq",info)
		end)
	end

	self:OnButtonClose()
end
