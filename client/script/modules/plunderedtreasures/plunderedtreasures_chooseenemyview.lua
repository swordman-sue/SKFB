Modules = Modules or {}
Modules.PlunderedTreasuresChooseenemyView = Modules.PlunderedTreasuresChooseenemyView or BaseClass(BaseView)

function Modules.PlunderedTreasuresChooseenemyView:__init()
	self.__layout_name = "plundered_treasures_fightenemyview"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = {
		close_func = function ()
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
			self:CloseManual()
		end,
		title = CPPGameLib.GetString("plunderedtreasures"),
		show_diamond = true,
		show_gold = true,
		show_energy	= true,
		show_level	= true,
	}
	self:__AddPreloadTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_FIGHTENEMY)
	self.__open_callback = function ()		
		self:__OpenCallback()
	end
end

function Modules.PlunderedTreasuresChooseenemyView:__OpenCallback()
	local data = unpack(self.__open_data)
	--当前精力	
	Util:WidgetLabel(self:GetWidget("Text_curEnery"),CPPGameLib.GetString("plunderedtreasures_curEnery"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--当前精力数量
	self.Text_curEneryNum = self:GetWidget("Text_curEneryNum")
	Util:WidgetLabel(self.Text_curEneryNum,"", Macros.TypefaceSize.normal, Macros.Color.content)
	self:UpdateEnery()
	--抢夺消耗	
	Util:WidgetLabel(self:GetWidget("Text_plunderCost"),CPPGameLib.GetString("plunderedtreasures_plunderCost"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--抢夺消耗数量
	Util:WidgetLabel(self:GetWidget("Text_plunderCostNum"),ConfigAdapter.Common.GetPlunderedTreasureCostEnery(), Macros.TypefaceSize.normal, Macros.Color.content)
	--免战剩余时间文字
	self.Text_retainAvoidWarTime = self:GetWidget("Text_retainAvoidWarTime")
	Util:WidgetLabel(self.Text_retainAvoidWarTime,CPPGameLib.GetString("plunderedtreasures_retainAvoidWarTime"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--免战剩余时间
	self.Text_retainAvoidWarTimeNum = self:GetWidget("Text_retainAvoidWarTimeNum")
	Util:WidgetLabel(self.Text_retainAvoidWarTimeNum,"", Macros.TypefaceSize.normal, Macros.Color.front_color)
	--免战时间
	self:refrashAvoidWarTime()
	--刷新倒计时
	self.funId = GlobalCallbackMgr:AddCallBackFunc(function ()
		self:UpdateCountTime()
	end)
	self.Text_timePlay = self:GetWidget("Text_timePlay")
	Util:WidgetLabel(self.Text_timePlay,"", Macros.TypefaceSize.normal, Macros.Color.red,0,0,Macros.Color.common_stroke)
	self.Text_timePlay:SetVisible(false)
	--刷新	
	Util:WidgetButton(self:GetWidget("Button_refrash"),CPPGameLib.GetString("plunderedtreasures_buttonRefrash"),function ()
		self.CountTime = 6
		self:GetWidget("Button_refrash"):SetVisible(false)
		self.Text_timePlay:SetVisible(true)
		self:UpdateCountTime()
		CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresTargetListReq",self.treasure_info_id,self.treasure_item_id)
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_yellow )
	--免战监听
	self.__plunderedTreasuresRefrashTimeEvent = self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHAVOIDWARTIME, function()
      	self:refrashAvoidWarTime()
    end)
    --精力改变
	self.__EneryChangeEvent = self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE, function()
      	self:UpdateEnery()
    end)
    --关闭界面
    self.__CloseViewEvent = self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_CLOSE_FIGHTENEMYVIEW, function(_,is_no_close,is_open_close)
      	--夺五次界面
      	if not is_no_close and self.__report_view then
      		self.__report_view:DeleteMe()
      		self.__report_view = nil
      	end
      	if not is_open_close then
      		self:CloseManual(true)
      	else
      		self:CloseManual()
      	end     	
    end)
	--容器
	local Panel_chooseEnemyScrollview = self:GetWidget("Panel_chooseEnemyScrollview")
	self.treasure_info_id = data.treasure_info_id
	self.treasure_item_id = data.treasure_item_id
	local params = {
		item_class = Modules.TemplPlunderedTreasuresChooseenemyCell,
	    item_width	= 740,
	    item_height	= 134,
	    row	= 3,
	    col	= 1,
	    item_space_r = 10,
	    view_width = 740,
	    view_height = 442,
	}
	self.ScrollView = GUI.ScrollView.New(params)
	Panel_chooseEnemyScrollview:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)	
	local resetData = self:resetDataTable(data)
	self:__SetEnemyList(resetData)
	self:__ShowAction()
	--刷新对手列表
	self.__plunderedTreasuresRefreshTargetEvent = self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASH_TARGET_LIST, function( _ , data)
        local resetData = self:resetDataTable(data)
		self:__SetEnemyList(resetData)
		self:__ShowAction()
    end) 
    --夺宝五次
    self.__plunderedTreasuresFiveTimesEvent = self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_FIVETIMES, function(_ , data)
        self.__report_view = LogicGUI.TemplPlunderedTreasuresReportView.New(data)  
    end)
end

--刷新倒计时
function Modules.PlunderedTreasuresChooseenemyView:UpdateCountTime()
	self.CountTime = self.CountTime - 1
	if self.CountTime ~= 0 then
		local str = "00:00:0"..self.CountTime
		self.Text_timePlay:SetText(str)
		CPPActionManager.DelayTo(self.Text_timePlay,1,self.funId)	
	else
		self:GetWidget("Button_refrash"):SetVisible(true)
		self.Text_timePlay:SetVisible(false)
	end	
end

--更新精力
function Modules.PlunderedTreasuresChooseenemyView:UpdateEnery()
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	self.Text_curEneryNum:SetText(role_info.energy.."/"..role_info.energy_max)
end

function Modules.PlunderedTreasuresChooseenemyView:__ShowAction()
	self.ScrollView:PlayFlyInAction(nil, nil, nil, nil, function()
		local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
		if operate_id == Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_ENEMYLIST_GRAB then
			--新手指引特殊操作检测（圣器争夺-敌人列表-抢夺（第一个机器人））
			local locate_index
			self.ScrollView:ItemDataIter(function(item_index, item_data)
				if item_data and item_data.target_type == Modules.PlunderedTreasuresModule.TargetType.MONSTER then
					locate_index = item_index
					return true
				end
			end)
			if locate_index then
				self.ScrollView:LocateItem(locate_index, true)
				local item = self.ScrollView:GetItem(locate_index)
				if item then
					item:CheckSectionOperateSP()				
				end
			end
		end
	end)
end

--整理数据
function Modules.PlunderedTreasuresChooseenemyView:resetDataTable(data)
	local resetDataTableList = {}
	for k,v in pairs(data.player_list) do
		v.target_type = 1
		v.treasure_info_id = data.treasure_info_id
		v.treasure_item_id = data.treasure_item_id
	end
	resetDataTableList = CPPGameLib.CopyTbl(data.player_list)
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	for i,v in ipairs(data.monster_list) do		
		local info = CPPGameLib.GetConfig("GameLevel",v.map_id)
		local monstertable = {}
		monstertable.level = roleLevel + MathTool.GetRandom(-8,0)
		monstertable.role_name = info.name
		monstertable.map_id = v.map_id		
		monstertable.hero_list = {}
		local battleInfo = CPPGameLib.GetConfig("Battle",info.battle_list[1])
		monstertable.fighting_power = 0
		for s=1,5 do
			if battleInfo["moster_"..s] ~= 0 then
				monstertable.fighting_power = monstertable.fighting_power + CallModuleFunc(ModuleType.POWER, "CalcMonsterPower", battleInfo["moster_"..s])
				local monster_Info = CPPGameLib.GetConfig("Monster",battleInfo["moster_"..s])
				table.insert(monstertable.hero_list,monster_Info.hero_id)
			end
		end
		monstertable.target_type = 2
		monstertable.treasure_info_id = data.treasure_info_id
		monstertable.treasure_item_id = data.treasure_item_id
		table.insert(resetDataTableList,monstertable)
	end
	CPPGameLib.PrintTable("ldx" , resetDataTableList , "resetDataTableListresetDataTableList ")
	return resetDataTableList
end

--刷新免战时间
function Modules.PlunderedTreasuresChooseenemyView:updateAvoidWarTime()
	local overTime = self:CallModuleFunc("GetAvoidWarOverTime")
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if severTime >=  overTime then
    	self.Text_retainAvoidWarTime:SetVisible(false)
    	self.Text_retainAvoidWarTimeNum:SetVisible(false)
		if self.__RefrashAvoidWarTimeEvent then
			GlobalTimerQuest:CancelQuest(self.__RefrashAvoidWarTimeEvent)
			self.__RefrashAvoidWarTimeEvent = nil
		end
    else
    	self.Text_retainAvoidWarTimeNum:SetText(TimeTool.TransTimeStamp("HH:MM:SS",overTime - severTime))
    end
end

--刷新免战时间
function Modules.PlunderedTreasuresChooseenemyView:refrashAvoidWarTime()
	local overTime = self:CallModuleFunc("GetAvoidWarOverTime")
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if severTime >=  overTime then
    	self.Text_retainAvoidWarTime:SetVisible(false)
    	self.Text_retainAvoidWarTimeNum:SetVisible(false)
    else
    	self.Text_retainAvoidWarTime:SetVisible(true)
    	self.Text_retainAvoidWarTimeNum:SetVisible(true)
    	self.Text_retainAvoidWarTimeNum:SetText(TimeTool.TransTimeStamp("HH:MM:SS",overTime - severTime))
    	if not self.__RefrashAvoidWarTimeEvent then
			self.__RefrashAvoidWarTimeEvent = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
				function()
				self:updateAvoidWarTime()
				end), 1, -1)
		end
		self:updateAvoidWarTime()
    end
end

function Modules.PlunderedTreasuresChooseenemyView:__SetEnemyList(data_list)
	self.ScrollView:SetDataList(data_list)	
end

function Modules.PlunderedTreasuresChooseenemyView:__Dispose()
	--动作
	if self.__PlunderedTreasures_runActionTimer then
		GlobalTimerQuest:CancelQuest(self.__PlunderedTreasures_runActionTimer)
		self.__PlunderedTreasures_runActionTimer = nil
	end
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
	--免战倒计时
	if self.__RefrashAvoidWarTimeEvent then
		GlobalTimerQuest:CancelQuest(self.__RefrashAvoidWarTimeEvent)
		self.__RefrashAvoidWarTimeEvent = nil
	end
end