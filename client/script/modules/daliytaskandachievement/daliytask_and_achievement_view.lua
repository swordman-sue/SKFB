Modules = Modules or {}

Modules.DaliytaskAndAchievementView = Modules.DaliytaskAndAchievementView or BaseClass(BaseView)
local TagViewType = {
	DALIYTASK_VIEW = 1,
	ACHIEVEMENT_VIEW = 2,
}

local starbox_effect_map = {"UI_wood_box", "UI_silverybox","UI_blue_box","UI_red_box"}
function Modules.DaliytaskAndAchievementView:__init()
	self.__layout_name = "daliy_task_and_achievement"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = {
		close_func = function ()
		    self:CloseManual()	
		end,
		title = CPPGameLib.GetString("daliyMission_TopTitle"),
		show_level	= true,
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
	},

	--功能开启等级	
	self:__CountLockLevel()
	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigyellow")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigwhite")
    self.__tab_params = {
	    {sp_2,sp_1},
	    {  
	    	CPPGameLib.GetString("daliyMission_Title"),
            CPPGameLib.GetString("achievement_Title"),
		},
		10,
		CPPGameLib.Handler(self, self.changeTag),
		true,
		true,
		self.__prepare_level_list,
		self.__lock_level_list
	}

	self:__AddPreloadTemplate(GUI.TemplateRegistry.DALIYTASK_AND_ACHIEVEMENT_CELL)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DALIY_AND_ACHIEVEMENT,true)
	self.__open_callback = function ()
		self:__OpenCallback()
	end
end

function Modules.DaliytaskAndAchievementView:__OpenCallback()
	--日常任务条目刷新
	self.__RefrashDaliyMissionListEvent = self:BindGlobalEvent(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_DALIYMISSION_LIST, function(_ , index,missionId)
        self.daliyScrollView:MoveItemToLast(index,true)
        self:refrashPoint(missionId)
        --红点
		self:__CountTabRedDotNum()
    end)
    --日常任务积分奖励刷新
    self.__RefrashDaliyMissionPointEvent = self:BindGlobalEvent(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_DALIYMISSION_POINT, function(_ , index)
    	self["HaveGetTreasuresEncourage"..index] = true
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__box_res_list[index].."_box_open")
    	self["treasures"..index]:SetNormalImage(sp)
    	--删除特效
		self["treasures_specialeffect"..index]:SetVisible(false)
		self["treasures"..index]:SetVisible(true)
		if self["__button_zoubian_effect"..index] then
			self:__DelEffect(self["__button_zoubian_effect"..index])
			self["__button_zoubian_effect"..index] = nil
			self:__DelEffect(self["__button_receive_effect"..index])
			self["__button_receive_effect"..index] = nil			
		end
		--圣器红点计算
		self.__could_get_treasure_reddot_num = self.__could_get_treasure_reddot_num - 1
		--红点
		self:__CountTabRedDotNum()
    end)    
    --刷新成就条目位置
    self:BindGlobalEvent(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_ACHIEVEMENT_LIST, function(_ , index,isMoveToLast)
     	self.achievementFrontCount = self.achievementFrontCount - 1   
     	if isMoveToLast then
     		self.achievementScrollView:MoveItemToLast(index, true)
     	else
     		self.achievementScrollView:MoveItemTo(index,self.achievementFrontCount+2,true)
     	end	   
     	--红点
		self:__CountTabRedDotNum() 	
    end)  
    --关闭界面事件
	self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	        self:CloseManual()
	    end)

	--刷新成就条目表现
	self:BindGlobalEvent(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_ACHIEVEMENT_CELL, function(_ , index)
	        local item = self.achievementScrollView:GetItem(index)
	        if item then
	        	item:RefrashData()
	        end
	    end)

	--刷新成就条目表现
	self:BindGlobalEvent(Macros.Event.SundryModule.NEW_DAY_NOTIFY, function()
	        self:CallModuleFunc("DaliyMissionListReq")
	    end)

	--升级刷新条目
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function(_ , old_level,new_level)
	        self.__is_not_need_animation = true
	        self:CallModuleFunc("DaliyMissionListReq")
	    end)

    --日常任务界面
    self.DaliyMissionPanel = self:GetWidget("Panel_daliy")
    --成就界面
	self.AchievementPanel = self:GetWidget("Panel_achievement")

	--圣器领取红点数量计算
    self.__could_get_treasure_reddot_num = 0

    --宝箱资源文字
    self.__box_res_list = {[1] = "green",[2] = "blue",[3] = "purple",[4] = "orange"}
	    
	--解析成就数据
	local achievementData = self:CallModuleFunc("GetAchievementList")
	self:ParseAchievementData(achievementData)

	--选项卡
	local panel_Selected = self:GetWidget("Panel_tab")	
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local init_type = self.__lock_level_list[1] <= role_level and TagViewType.DALIYTASK_VIEW or TagViewType.ACHIEVEMENT_VIEW
    
    self.__tab:SetDirection(true,0)
    panel_Selected:AddChild(self.__tab:GetNode())
    PosTool.LeftTop(self.__tab,0,0)

    self.__tab:SetFontSize( Macros.TypefaceSize.tab )
    self.__tab:On(init_type)      --默认显示第一个选项卡的
	--红点
	self.__tab:HideUnOpenTab(role_level)	
	self:__CountTabRedDotNum()
end

--开放等级
function Modules.DaliytaskAndAchievementView:__CountLockLevel()
	self.__lock_level_list = {}
	self.__prepare_level_list = {}
	local module_type_list = {Macros.Game.SystemID.DALIYTASK,Macros.Game.SystemID.ACHIEVEMENT}
	for i,module_id in ipairs(module_type_list) do
		local module_info = CPPGameLib.GetConfig("SystemEntry",module_id)
		table.insert(self.__lock_level_list,module_info.level)
		table.insert(self.__prepare_level_list,module_info.prepare_level)
	end
end

--初始化任务
function Modules.DaliytaskAndAchievementView:__InitTask()
	 --日常任务Panel
    local DaliyMissionList = self:CallModuleFunc("getDaliyMissionList")
    local roleLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.DAILY_INIT_LEVEL)
    local missionInfo = nil
    for i,missionData in pairs(CPPGameLib.GetConfig("TeamLevelDailyMission",nil,nil,true)) do
    	if roleLevel >= missionData.level_min and roleLevel <= missionData.level_max then
    		missionInfo = missionData
    		break
    	end
    end
    --初始化
    if not self.__is_init_task then
    	self.__is_init_task = true
	    --根据数据给宝箱设置状态和打开信息界面
	    for i=1,4 do
	    	if self["__button_zoubian_effect"..i] then
				self:__DelEffect(self["__button_zoubian_effect"..i])
				self["__button_zoubian_effect"..i] = nil
			end
			if self["__button_receive_effect"..i] then
				self:__DelEffect(self["__button_receive_effect"..i])
				self["__button_receive_effect"..i] = nil
			end
	    	local function openTreasuresInfoView()
	    		LogicGUI.TemplAwardView.New(i,self["CouldGetTreasuresEncourage"..i],self["HaveGetTreasuresEncourage"..i])
	    	end
	    	--宝箱
	    	self["treasures"..i] = self:GetWidget("Button_treasure"..i)
	    	self["treasures"..i]:SetZOrder(1)
	    	self["treasures"..i]:IgnoreContentAdaptWithSize(true)
	    	local pos_x = 47+(missionInfo.point_list[i]/250)*650
	    	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__box_res_list[i].."_box_close")   	
	    	self["treasures"..i]:SetNormalImage(sp)
	    	self["treasures"..i]:SetPosition(pos_x,self["treasures"..i]:GetPositionY())
	    	WidgetUnity.SetWidgetListener(self["treasures"..i],nil,nil,openTreasuresInfoView,nil)
	    	--特效容器
	    	self["treasures_specialeffect"..i] = self:GetWidget("Panel_specialEffect"..i)
	    	self["treasures_specialeffect"..i]:SetZOrder(1)
	    	self["treasures_specialeffect"..i]:SetVisible(false)
	    	self["treasures_specialeffect"..i]:SetPosition(pos_x,self["treasures_specialeffect"..i]:GetPositionY())
	    	WidgetUnity.SetWidgetListener(self["treasures_specialeffect"..i],nil,nil,openTreasuresInfoView,nil)
	    	--积分数字
	    	local pointNum = self:GetWidget("Text_progress"..i)
	    	pointNum:SetZOrder(1)
	    	pointNum:SetPosition(pos_x,pointNum:GetPositionY())
	    	Util:WidgetLabel(pointNum, missionInfo.point_list[i],Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0,Macros.Color.common_stroke)   	
	    	if DaliyMissionList.point >= missionInfo.point_list[i] then
	    		self["CouldGetTreasuresEncourage"..i] = true
	    	else
	    		self["CouldGetTreasuresEncourage"..i] = false
	    	end
	    	if self["CouldGetTreasuresEncourage"..i] then
	    		if #DaliyMissionList.point_reward_record == 0 then
		    		self["HaveGetTreasuresEncourage"..i] = false
		    	else
		    		local isHaveGet = false
		    		for k,v in ipairs(DaliyMissionList.point_reward_record) do
		    			if v.point == missionInfo.point_list[i] then
		    				isHaveGet = true
		    				break
		    			end
		    		end
		    		if isHaveGet then
		    			self["HaveGetTreasuresEncourage"..i] = true
		    		else
		    			self["HaveGetTreasuresEncourage"..i] = false
		    		end	
		    	end
	    	else
	    		self["HaveGetTreasuresEncourage"..i] = false
	    	end   	
	    	if self["CouldGetTreasuresEncourage"..i] then
	    		if self["HaveGetTreasuresEncourage"..i] then   			
	    			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__box_res_list[i].."_box_open")
	    			self["treasures"..i]:SetNormalImage(sp)
	    		else
	    			--加特效
	    			self["treasures_specialeffect"..i]:SetVisible(true)
	    			self["treasures"..i]:SetVisible(false)
	    			self.__could_get_treasure_reddot_num = self.__could_get_treasure_reddot_num + 1
	    			local effect
					self["__button_zoubian_effect"..i],effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"},self["treasures_specialeffect"..i])
					PosTool.Center(effect)
					self["__button_receive_effect"..i],effect = self:__CreateEffect({res_name = starbox_effect_map[i], type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE },self["treasures_specialeffect"..i],2)
					PosTool.Center(effect, 0, -30 - math.min(1,i-1) * 5)
	    		end
	    	end
	    end
	    --积分进度条
	    if not self.daliyMissionProgressBar then
		    local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview_bg")
			local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview")
			self.daliyMissionProgressBar = GUI.ProgressBar9Layer.New(progressBar,progressBar_bg,650,20,652,24) 
			self.daliyMissionProgressBar:SetPosition(47,452)
			self.daliyMissionProgressBar:SetZOrder(0)
			self.DaliyMissionPanel:AddChild(self.daliyMissionProgressBar:GetNode())  
		end
	    self.daliyMissionProgressBar:SetProgress(math.min(100,(DaliyMissionList.point/250)*100))
	    --当前积分
	    Util:WidgetLabel(self:GetWidget("Text_curPoint"), CPPGameLib.GetString("cur_getPoint"), Macros.TypefaceSize.slightly, Macros.Color.keypoint )  
	    --记录当前积分
	    self.curHavePoint = DaliyMissionList.point
	    self.curHavePointNumText = self:GetWidget("Text_curPointNum")
	    Util:WidgetLabel(self.curHavePointNumText,self.curHavePoint, Macros.TypefaceSize.slightly, Macros.Color.content )  
	    --重置提示
	    Util:WidgetLabel(self:GetWidget("Text_resetTip"), CPPGameLib.GetString("clearMission_Tip"), Macros.TypefaceSize.slightly, Macros.Color.headline  )
   	end
    --日常任务条目Scrollview
    if not self.daliyScrollView then
	    local daliyScrollViewPanel = self:GetWidget("Panel_daliyTaskScrollView")
	    local params = {
	    	item_class	= Modules.TemplDaliyTaskCell,
		    item_width	= 684,
		    item_height	= 119,
		    row	= 4,
		    col	= 1,
		    item_space_r = 8,
		    view_width = 684,
		    view_height = 395,
		}
		self.daliyScrollView = GUI.ScrollView.New(params)
		daliyScrollViewPanel:AddChild(self.daliyScrollView:GetNode())
		PosTool.Center(self.daliyScrollView)
	end
	self.missionTable = {}
	--排位置需要的累计
	local needBackCount = 0
	local needFrontCount = 0 
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	for i,v in ipairs(missionInfo.mission_list) do
		local data = {}
		data.mission_id = v
		data.is_get_reward = false
		data.value = 0
		local missionInfoTempl = CPPGameLib.GetConfig("DailyMission",v)		
		if role_level >= (missionInfoTempl.need_role_level - ConfigAdapter.Common.GetDaliyTaskShowLevelDifferenceValue()) then
			if #DaliyMissionList.mission_list ~= 0 then			
				for num,mission in pairs(DaliyMissionList.mission_list) do
					if v == mission.mission_id then
						if mission.is_get_reward == 1 then
							data.is_get_reward = true
							data.value = mission.value
							table.insert(self.missionTable,data)
							needBackCount = needBackCount + 1	
						elseif mission.value >= missionInfoTempl.value then	
							data.value = mission.value
							table.insert(self.missionTable,1,data)	
							needFrontCount = needFrontCount + 1			
						else
							data.value = mission.value
							table.insert(self.missionTable,1+needFrontCount,data)
						end
						break
					elseif num == #DaliyMissionList.mission_list then
						table.insert(self.missionTable,i-needBackCount,data)
					else
					end
				end
			else
				table.insert(self.missionTable,i-needBackCount,data)
			end
		end
	end	
	self.daliyScrollView:SetDataList(self.missionTable)
	if not self.__is_not_need_animation then
		self.daliyScrollView:PlayFlyInAction()
	end
	self.__is_not_need_animation = false
end

--刷新积分
function Modules.DaliytaskAndAchievementView:refrashPoint(missionId)
	local missionInfo = CPPGameLib.GetConfig("DailyMission",missionId)
	local roleLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.DAILY_INIT_LEVEL)
	self.curHavePoint = self.curHavePoint + missionInfo.point
	self.daliyMissionProgressBar:SetProgress(math.min(100,self.curHavePoint/250 * 100))
    self.curHavePointNumText:SetText(self.curHavePoint)
    for i,v in pairs(CPPGameLib.GetConfig("TeamLevelDailyMission",nil,nil,true)) do
    	if roleLevel >= v.level_min and roleLevel <= v.level_max then
    		missionInfo = v
    		break
    	end
    end
    for i=1,4 do
    	if self.curHavePoint >= missionInfo.point_list[i] then
    		if not self["CouldGetTreasuresEncourage"..i] then
    			self.__could_get_treasure_reddot_num = self.__could_get_treasure_reddot_num + 1
    			self["CouldGetTreasuresEncourage"..i] = true
    			--加特效
    			self["treasures_specialeffect"..i]:SetVisible(true)
    			self["treasures"..i]:SetVisible(false)
    			local effect
				self["__button_zoubian_effect"..i],effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"},self["treasures_specialeffect"..i])
				PosTool.Center(effect)
				self["__button_receive_effect"..i],effect = self:__CreateEffect({res_name = starbox_effect_map[i], type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE },self["treasures_specialeffect"..i],2)
				PosTool.Center(effect, 0, -30 - math.min(1,i-1) * 5)
    		end
    	end
    end
end

--切换页签
function Modules.DaliytaskAndAchievementView:changeTag(index)
	if index == TagViewType.DALIYTASK_VIEW then
		if not self.daliyScrollView then
			self:__InitTask()
		end
		self.DaliyMissionPanel:SetVisible(true)
		self.daliyScrollView:SetVisible(true)
		if self.achievementScrollView then
			self.achievementScrollView:SetVisible(false)
		end
		self.AchievementPanel:SetVisible(false)		
	else
		self.DaliyMissionPanel:SetVisible(false)
		if self.daliyScrollView then
			self.daliyScrollView:SetVisible(false)
		end
		self.AchievementPanel:SetVisible(true)
		if not self.achievementScrollView then
			self:createAchievementView()
		else
			self.achievementScrollView:SetVisible(true)
		end
	end
end

--解析成就数据
function Modules.DaliytaskAndAchievementView:ParseAchievementData(achievementData)
	self.achievementTable = {}
	print("ldx","achievementData: ",achievementData)
	--排位置需要的累计
	self.achievementFrontCount = 0 
	self.__achievement_reddot_num = 0
	local needBackCount = 0
	for i,achievementInfoTempl in ipairs(CPPGameLib.GetConfig("Achievement",nil,nil,true)) do
		local data = {}
		data.achievement_id = i
		data.level = 0
		data.value = 0
		data.CouldGetReward = false
		data.cur_reward_lv = 0
		data.HaveGetThrough = false	
		if achievementData and #achievementData.achievement_list > 0 then		
			for num,achievement in ipairs(achievementData.achievement_list) do
				if i == achievement.id then
					data.level = achievement.level
					data.value = achievement.value
					data.cur_reward_lv = achievement.cur_reward_lv
					if achievement.cur_reward_lv == achievement.level and #achievementInfoTempl.level_list <= achievement.cur_reward_lv then
						data.HaveGetThrough = true
						needBackCount = needBackCount + 1
						table.insert(self.achievementTable,data)
					elseif achievement.cur_reward_lv < achievement.level then
						data.CouldGetReward = true	
						self.__achievement_reddot_num = self.__achievement_reddot_num + 1				
						table.insert(self.achievementTable,1,data)	
						self.achievementFrontCount = self.achievementFrontCount + 1	
					elseif achievement.cur_reward_lv == achievement.level then
						table.insert(self.achievementTable,1+self.achievementFrontCount,data)	
					else

					end
					break 
				elseif num == #achievementData.achievement_list then
					table.insert(self.achievementTable,i - needBackCount,data)
				else
				end
			end
		else
			table.insert(self.achievementTable,i,data)
		end
	end
end

--创建成就界面
function Modules.DaliytaskAndAchievementView:createAchievementView()
	--成就条目Scrollview	
	if not self.achievementScrollView then		
		local achievementScrollViewPanel = self:GetWidget("Panel_achievementScrollView")
		local params = {
			item_class	= Modules.TemplAchievementCell,
			item_width	= 684,
		    item_height	= 119,
		    row	= 4,
		    col	= 1,
		    item_space_r = 8,
		    view_width = 684,
		    view_height = 482,
		}
		self.achievementScrollView = GUI.ScrollView.New(params)
		achievementScrollViewPanel:AddChild(self.achievementScrollView:GetNode())
		PosTool.Center(self.achievementScrollView) 		
		self.achievementScrollView:SetDataList(self.achievementTable)	
		self.achievementScrollView:PlayFlyInAction(nil, nil, nil, nil, function ()
			--新手指引特殊操作检测
			local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
			if operate_id == Macros.Game.LeadOperateSpecialID.ACHIEVEMENT_GOTO or operate_id == Macros.Game.LeadOperateSpecialID.ACHIEVEMENT_GET_REWARD then				
				local locate_index
				self.achievementScrollView:ItemDataIter(function(item_index, item_data)
					if item_data.achievement_id == data_id then
						locate_index = item_index
						return true
					end
				end)				
				if locate_index then
					self.achievementScrollView:LocateItem(locate_index, true)

					--列表相关调整
					local item = self.achievementScrollView:GetItem(locate_index)
					if item then
						item:CheckSectionOperateSP()
					end
				end
			end
		end)
	end
end

function Modules.DaliytaskAndAchievementView:__CountTabRedDotNum()
	local total_red_dot_num = self.__could_get_treasure_reddot_num
	if self.daliyScrollView then
		self.daliyScrollView:ItemDataIter(function(item_index, item_data)
			if item_data.CouldGetReward then
				total_red_dot_num = total_red_dot_num + 1
			end
		end)
	end
	if total_red_dot_num > 0 then
		self.__tab:AddRedDot(1,true)
	else
		self.__tab:HideRedDot(1)
	end
	total_red_dot_num = 0

	if self.achievementScrollView then
		self.achievementScrollView:ItemDataIter(function(item_index, item_data)
			if item_data.CouldGetReward then
				total_red_dot_num = total_red_dot_num + 1
			end
		end)
	else
		total_red_dot_num = total_red_dot_num + self.__achievement_reddot_num
	end
	if total_red_dot_num > 0 then
		self.__tab:AddRedDot(2,true)
	else
		self.__tab:HideRedDot(2)
	end
end

--界面销毁
function Modules.DaliytaskAndAchievementView:__Dispose()
	if self.daliyScrollView then
		self.daliyScrollView:DeleteMe()
		self.daliyScrollView = nil
	end
	if self.achievementScrollView then
		self.achievementScrollView:DeleteMe()
		self.achievementScrollView = nil		
	end

	if self.__DaliyMission_runActionTimer then
		GlobalTimerQuest:CancelQuest(self.__DaliyMission_runActionTimer)
		self.__DaliyMission_runActionTimer = nil
	end	
	
	if self.__Achievement_runActionTimer then
		GlobalTimerQuest:CancelQuest(self.__Achievement_runActionTimer)
		self.__Achievement_runActionTimer = nil
	end

	for i=1,4 do
		if self["__button_zoubian_effect"..i] then
			self:__DelEffect(self["__button_zoubian_effect"..i])
			self["__button_zoubian_effect"..i] = nil
		end
		if self["__button_receive_effect"..i] then
			self:__DelEffect(self["__button_receive_effect"..i])
			self["__button_receive_effect"..i] = nil
		end
	end
end