
--[[
%% @module: 英雄培养界面
%% @author: swordman sue
%% @created: 2016-08-26
--]]

Modules = Modules or {}

Modules.HeroTrainTabView = Modules.HeroTrainTabView or BaseClass(BaseTabView)

function Modules.HeroTrainTabView:__init()
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_TRAIN)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_TRAIN,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_TRAIN_VIEW)
end

--选项卡界面创建完成回调
function Modules.HeroTrainTabView:__LoadedCallback()
	--加载模板
	LogicGUI.TemplHeroTrainView.New(self.__root_widget)	
	--箭头图片资源
	self.__RedArrowsData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "red_arrow")
	self.__GreenArrowsData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips")
	--按钮位置
	self.Button_Pos = {[1] = 26, [2] = 88}	
	--特效名字table
	self.special_effect = {[1] = "UI_peiyanggongji",[2] = "UI_peiyangwufang",[3] = "UI_peiyangfafang",[4] = "UI_peiyangshengming"}
	--培养石配置
	local train_prop_config = CPPGameLib.GetConfig("Item",ConfigAdapter.Common.GetHeroTrainPropId())
	--初始化遮罩
	for i=1,4 do
		self["Panel_ballSpecialEffect_"..i] = self:GetWidget("Panel_ballSpecialEffect"..i)
		self["clipping_node_"..i] = Util:CircleClippingNode(0,0,37)
		self["Panel_ballSpecialEffect_"..i]:AddChild(self["clipping_node_"..i])
		PosTool.Center(self["clipping_node_"..i])		
		--属性值与上限
		self["Text_propertyProgress"..i] = self:GetWidget("Text_propertyProgress"..i)
		Util:WidgetLabel(self["Text_propertyProgress"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn, 0, 0,Macros.Color.common_stroke)		
		--变化属性底框
		self["Image_arrowsframe"..i] = self:GetWidget("Image_arrowsframe"..i)		
		self["Image_arrowsframe"..i]:SetVisible(false)
		--变化属性箭头
		self["propertyArrows"..i] = self:GetWidget("propertyArrows"..i)	
		--变化属性等级
		self["text_propertylevel"..i] = self:GetWidget("Text_propertyLevel"..i)	
		Util:WidgetLabel(self["text_propertylevel"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn, 0, 0,Macros.Color.common_stroke)
		--领取奖励
		self["Image_getproperty"..i] = self:GetWidget("Image_getproperty"..i)	
		self["Image_getproperty"..i]:SetVisible(false)
		--UI适配		
		local adp_img = WidgetUnity.GetWidgetByName(self:GetWidget("Image_propertyType"..i),"Image_247")
		adp_img:IgnoreContentAdaptWithSize(true)	
				
		WidgetUnity.SetWidgetListener(self:GetWidget("Image_propertycircle"..i),nil, nil,function ()
			if self["pro_reward_flag"..i] == 1 and not self.__is_on_train_req then
				--请求领取
				self:CallModuleFunc("HeroTrainRewardGetReq",self.__hero_uid,i)	
				self.__is_on_train_req = true								
			end
		end)
		--培养石
		local Image_trainItem = self:GetWidget("Image_trainItem"..i)
		Image_trainItem:SetImage(Resource.PathTool.GetItemPath(train_prop_config.res_id),TextureResType.LOCAL)		
	end
	--拥有培养丹
	Util:WidgetLabel(self:GetWidget("Text_trainMedicine"),CPPGameLib.GetString("hero_train_haveMedicine"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--拥有培养丹数量
	self.Text_haveTrainMedicineNum = self:GetWidget("Text_haveTrainMedicineNum")
	Util:WidgetLabel(self.Text_haveTrainMedicineNum,"", Macros.TypefaceSize.largish, Macros.Color.content)
	--选中框
	for i=1,3 do
		self["CheckBox_train"..i] = self:GetWidget("CheckBox_train"..i)		
		WidgetUnity.SetCheckboxListener(self["CheckBox_train"..i],function ()
			self:__ChangeCostType(i)
		end, function ()
			self["CheckBox_train"..i]:SetSelectedState(true)
		end)	
	end
	--消耗列表
	self.CostTable = {}
	for i,costInfo in ipairs(CPPGameLib.GetConfig("HeroTrainCost",nil,nil,true)) do
		self.CostTable[i] = costInfo
	end
	--丹药培养
	Util:WidgetLabel(self:GetWidget("Text_medicineTrain"),CPPGameLib.GetString("hero_train_trainByMedicine"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--丹药培养所需丹药数量
	self.Text_medicineTrainNum = self:GetWidget("Text_medicineTrainNum")
	Util:WidgetLabel(self.Text_medicineTrainNum,"", Macros.TypefaceSize.normal, Macros.Color.content)
	--金币培养
	Util:WidgetLabel(self:GetWidget("Text_goldTrain"),CPPGameLib.GetString("hero_train_trainByGold"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--金币培养所需丹药数量
	self.Text_goldTrainMedicineNum = self:GetWidget("Text_goldTrainMedicineNum")
	Util:WidgetLabel(self.Text_goldTrainMedicineNum,"", Macros.TypefaceSize.normal, Macros.Color.content)
	--金币培养所需金币数量
	self.Text_goldTrainGoldNum = self:GetWidget("Text_goldTrainGoldNum")
	Util:WidgetLabel(self.Text_goldTrainGoldNum,"", Macros.TypefaceSize.normal, Macros.Color.content)
	--钻石培养
	Util:WidgetLabel(self:GetWidget("Text_diamondsTrain"),CPPGameLib.GetString("hero_train_trainByDiamonds"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--钻石培养所需丹药数量
	self.Text_diamondsTrainMedicineNum = self:GetWidget("Text_diamondsTrainMedicineNum")
	Util:WidgetLabel(self.Text_diamondsTrainMedicineNum,"", Macros.TypefaceSize.normal, Macros.Color.content)
	--钻石培养所需钻石数量
	self.Text_diamondsTrainDiamondNum = self:GetWidget("Text_diamondsTrainDiamondNum")
	Util:WidgetLabel(self.Text_diamondsTrainDiamondNum,"", Macros.TypefaceSize.normal, Macros.Color.content)
	
	--培养按钮
	self.Button_train = self:GetWidget("Button_train")
	Util:WidgetButton(self.Button_train,CPPGameLib.GetString("hero_train_ButtonTrain"),function ()
		--检查是否已经满了
		local IsAllFull = true
		local IsHaveGetAward = false
		local config = CPPGameLib.GetConfig("HeroTrain",1)	
		for pro_type=1,4 do
			if self["pro_reward_flag"..pro_type] == 1 then
				IsHaveGetAward = true
				break
			end
			if not self["train_pro_full_"..pro_type] then
				IsAllFull = false
			end			
		end	
		if IsHaveGetAward then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_train_fullGetAward_tips"))
			return
		end
		if IsAllFull and self.pro_level1 == table.maxn(config.pro_level_list) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_train_fulltips2"))
			return
		elseif IsAllFull then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_train_fulltips1"))
			return
		end
		local IsCouldTrain = self:__CheckIsCouldTrain()
		if IsCouldTrain then
			for pro_type=1,4 do
				if self["Text_changeProperty"..self.__windseffect_count..pro_type] then
					self["Text_changeProperty"..self.__windseffect_count..pro_type]:RemoveSelf(true)
					self["Text_changeProperty"..self.__windseffect_count..pro_type] = nil
				end
			end	
			self.Button_train:SetEnabled(false)		
			self.Button_replace:SetEnabled(false)			
			self:CallModuleFunc("HeroTrainReq",self.__hero_uid,self.__cost_type,self.__train_times)
		else
			if type(self.__conditional_str) == 'number' then
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.__conditional_str)
			else
				local str = (self.CostTable[self.__cost_type].cost_currency_type == Macros.Game.RolePropType.GOLD) and CPPGameLib.GetString("hero_train_gold_string") 
				or CPPGameLib.GetString("hero_train_diamonds_string")
				if str == CPPGameLib.GetString("hero_train_gold_string") then
					CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
				end
				GlobalTipMsg.GetInstance():Show(self.__conditional_str)
			end			
		end
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_yellow )
	--替换按钮
	self.Button_replace = self:GetWidget("Button_replace")
	Util:WidgetButton(self.Button_replace,CPPGameLib.GetString("hero_train_ButtonReplace"),function ()
		self.Button_train:SetEnabled(false)		
		self.Button_replace:SetEnabled(false)
		self.__cur_windseffect_count = self.__windseffect_count
		self:CallModuleFunc("HeroTrainReplaceReq",self.__hero_uid)		
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_red )
	--更改培养次数图片
	WidgetUnity.SetWidgetListener(self:GetWidget("Panel_TouchOpen"), nil, nil,function ()
		LogicGUI.TemplHeroTrainTimesChooseView.New()
	end)
	--培养次数
	self.Text_trainTimes = self:GetWidget("Text_trainTimes")
	Util:WidgetLabel(self.Text_trainTimes,"", Macros.TypefaceSize.normal, Macros.Color.content)
	
	--培养预览按钮
	Util:WidgetButton(self:GetWidget("Button_propertyPreLook"),"",function ()
		local least_pro_level = self.pro_level1
		local pro_valueData ={}
		for i=1,4 do
			if least_pro_level > self["pro_level"..i] then
				least_pro_level = self["pro_level"..i]
			end
			pro_valueData[i] = self["pro_value_"..i]
		end
		LogicGUI.TemplHeroTrainPreLookView.New(least_pro_level,pro_valueData)
	end)

	--培养所需VIP
	local times_list = {5,10}
	local limit_conditional_list = {}
	local conList = ConfigAdapter.Common.GetHeroTrainFiveTimesNeedVipLevelAndLevel()
	table.insert(limit_conditional_list,conList)
	conList = ConfigAdapter.Common.GetHeroTrainTenTimesNeedVipLevelAndLevel()
	table.insert(limit_conditional_list,conList)

	--培养VIP提示
	self.__txt_trainviptips = self:GetWidget("Text_TrainVipTips")
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local vip_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.VIP_LEVEL)
	local is_need_show = false
	local show_txt = ""
	for i=1,2 do
		if level < limit_conditional_list[i][2] and vip_level < limit_conditional_list[i][1] then
			is_need_show = true
			show_txt = CPPGameLib.GetString("hero_train_need_vip_tips2",limit_conditional_list[i][1],times_list[i])
			break
		end
	end
	self.__txt_trainviptips:SetVisible(is_need_show)
	Util:WidgetLabel(self.__txt_trainviptips, show_txt, Macros.TypefaceSize.minimum, Macros.Color.red)

end

--选项卡界面打开回调
function Modules.HeroTrainTabView:__OpenCallback()

	self.__hero_uid = self.__parent_view:GetCurHeroUID()

	--创建特效
	self:__createSpecialEffect()
	self:__Reset()

	--英雄详细信息反馈事件
	self.__detail_info_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_DETAIL_INFO_RESP, function(_,detail_info)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == detail_info.hero_uid then
			CPPGameLib.PrintTable(detail_info,"detail_info")
			self:__ResetDetail(detail_info)
		end
	end)
	--请求数据
	self:CallModuleFunc("HeroDetailInfoReq",self.__hero_uid)

	--英雄培养反馈事件
	self.__train_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_TRAIN_RESP, function(_,resp_info)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == resp_info.hero_uid then			
			self.Button_train:SetEnabled(true)		
			self.Button_replace:SetEnabled(true)	 	
			local itemNum = ObjIDTool.GetOwnNum(self.CostTable[1].cost_item_id)
			self.Text_haveTrainMedicineNum:SetText(itemNum)
	 		self:__showReplaceProperty(resp_info.pro_list)
	 	end
	end)

	--英雄培养次数选择
	self.__train_times_choose_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_TRAIN_TIMES_CHOOSE, function(_,times)
		self.__train_times = times
		self:__ResetTrainPrice()
	end)

	--英雄培养替换反馈
	self.__train_replace_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_TRAIN_REPLACE_RESP, function()
		self.Button_train:SetEnabled(true)		
		self.Button_replace:SetEnabled(true)
		self:__replaceProperty()
	end)

	--英雄培养领取奖励反馈
	self.__train_get_reward_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_TRAIN_GET_REWARD_RESP, function(_,value_pro_type)																
		--添加属性
		local text = ""	
		local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__hero_uid)		
		local config = CPPGameLib.GetConfig("HeroTrain",value_pro_type)
		for i,pro_type in ipairs(config.pro_level_list[self["pro_level"..value_pro_type]].level_up_pro_type_list) do
			text = text..CPPGameLib.GetString("nature"..pro_type).."+"..MathTool.TransProbCoeffPercentage(config.pro_level_list[self["pro_level"..i]].level_up_pro_value_list[i]).."% "
		end	
		local cur_lv = hero_info.level >= config.pro_level_list[self["pro_level"..value_pro_type]].need_hero_level and (self["pro_level"..value_pro_type]+1) or self["pro_level"..value_pro_type]
		local lv_str = CPPGameLib.GetString("nature"..value_pro_type).." "..CPPGameLib.GetString("common_lev9",cur_lv)
		LogicGUI.TemplHeroTrainBreakRewardView.New(text,lv_str,function ()
			self.__is_on_train_req = false
		end)
		--请求数据
		self:CallModuleFunc("HeroDetailInfoReq",self.__hero_uid)
		--GlobalTipMsg.GetInstance():Show(text)
		self["pro_reward_flag"..value_pro_type] = 0
		self:__DelEffect(self["hand_effect_id_"..value_pro_type])
		self["hand_effect_id_"..value_pro_type] = nil
		self["Image_getproperty"..value_pro_type]:SetVisible(false)
	end)
end

--选项卡界面关闭回调
function Modules.HeroTrainTabView:__CloseCallback()

	if self.__detail_info_resp_event then
		self:UnBind(self.__detail_info_resp_event)
		self.__detail_info_resp_event = nil
	end

	if self.__train_resp_event then
		self:UnBind(self.__train_resp_event)
		self.__train_resp_event = nil
	end

	if self.__train_replace_resp_event then
		self:UnBind(self.__train_replace_resp_event)
		self.__train_replace_resp_event = nil
	end

	if self.__train_get_reward_resp_event then
		self:UnBind(self.__train_get_reward_resp_event)
		self.__train_get_reward_resp_event = nil
	end

	if self.__train_times_choose_resp_event then
		self:UnBind(self.__train_times_choose_resp_event)
		self.__train_times_choose_resp_event = nil
	end

	for i=1,4 do
		self:__DelEffect(self["effect_id_"..i])		
		self["effect_id_"..i] = nil
		if self["hand_effect_id_"..i] then
			self:__DelEffect(self["hand_effect_id_"..i])
			self["hand_effect_id_"..i] = nil
		end
	end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroTrainTabView:__Dispose()
end

--创建特效
function Modules.HeroTrainTabView:__createSpecialEffect()
	for i=1,4 do
		self["effect_id_"..i],self["special_effect_"..i] = self:__CreateEffect({res_name = self.special_effect[i]},self["clipping_node_"..i])
		PosTool.CenterBottom(self["special_effect_"..i],0,-40)
	end
end

--刷新信息
function Modules.HeroTrainTabView:__Reset()
	--重置培养次数
	self.__train_times = 1
	self:__ResetTrainPrice()
	--重置预览的培养丹药数量
	local itemNum = ObjIDTool.GetOwnNum(self.CostTable[1].cost_item_id)
	self.Text_haveTrainMedicineNum:SetText(itemNum)
	--重置消耗类型
	self:__ChangeCostType(Macros.Game.HERO_TRAIN_COST_TYPE.ONLY_ITEM)
	--飘字计数
	self.__windseffect_count = 0
end

--刷新培养价格
function Modules.HeroTrainTabView:__ResetTrainPrice()
	--培养次数
	self.Text_trainTimes:SetText(CPPGameLib.GetString("hero_train_TrainTimes",self.__train_times))
	local cost_item_num_1 = 0

	local cost_item_num_2 = 0
	local cost_currency_value_2 = 0

	local cost_item_num_3 = 0
	local cost_currency_value_3 = 0

	local hero_info = CallModuleFunc (ModuleType.HERO,"GetHeroInfo" , self.__hero_uid )
	cost_item_num_1 = self.CostTable[1].cost_item_num
	cost_item_num_2 = self.CostTable[2].cost_item_num
	cost_currency_value_2 = self.CostTable[2].cost_currency_value
	cost_item_num_3 = self.CostTable[3].cost_item_num
	cost_currency_value_3 = self.CostTable[3].cost_currency_value

	--价格	
	self.Text_medicineTrainNum:SetText( cost_item_num_1 * self.__train_times)
	self.Text_goldTrainMedicineNum:SetText( cost_item_num_2 * self.__train_times)
	self.Text_goldTrainGoldNum:SetText( cost_currency_value_2 * self.__train_times)
	self.Text_diamondsTrainMedicineNum:SetText( cost_item_num_3 * self.__train_times)
	self.Text_diamondsTrainDiamondNum:SetText( cost_currency_value_3 * self.__train_times)
end

--检查是否满足培养条件
function Modules.HeroTrainTabView:__CheckIsCouldTrain()
	local IsCouldTrain = false

	--消耗材料、货币
	local cost_item_num = self.CostTable[self.__cost_type].cost_item_num
	local cost_currency_value = self.CostTable[self.__cost_type].cost_currency_value
	local cost_currency_type = self.CostTable[self.__cost_type].cost_currency_type

	--先检测培养材料是否充足
	local itemNum = ObjIDTool.GetOwnNum(self.CostTable[1].cost_item_id)
	if itemNum >= ( cost_item_num * self.__train_times ) then
		IsCouldTrain = true
	else
		local item_type,item_info = ObjIDTool.GetInfo(self.CostTable[1].cost_item_id)
		self.__conditional_str = self.CostTable[1].cost_item_id
	end

	--再检测培养所需货币是否充足
	if cost_currency_value then
		local cur_currency_value = CallModuleFunc( ModuleType.ROLE , "GetPropValue" , cost_currency_type )
		if cur_currency_value < cost_currency_value * self.__train_times then
			local str = cost_currency_type == Macros.Game.RolePropType.GOLD and 
			CPPGameLib.GetString("hero_train_gold_string") or CPPGameLib.GetString("hero_train_diamonds_string")
			self.__conditional_str = str..CPPGameLib.GetString("hero_train_no_enough_string")
		end		
	end
	
	return IsCouldTrain
end

--刷新详细信息
function Modules.HeroTrainTabView:__ResetDetail(reset_data)	
	self.train_pro_list = reset_data.train_pro_list
	local tmp_pro_value_List = {}
	local IsNeedReplace = false

	local hero_info = CallModuleFunc (ModuleType.HERO,"GetHeroInfo" , self.__hero_uid )
	local heroCnf = CPPGameLib.GetConfig( "Hero", hero_info.info_id)

	for i,info in ipairs(reset_data.train_pro_list) do
		local config = CPPGameLib.GetConfig("HeroTrain",info.pro_type)
		--记录当前的属性值，上限，还有等级
		self["pro_value_"..info.pro_type] = info.pro_value
		self["pro_reward_flag"..info.pro_type] = info.pro_reward_flag
		self["pro_level"..info.pro_type] = info.pro_level	
		self["text_propertylevel"..info.pro_type]:SetText(CPPGameLib.GetString("common_lev9",info.pro_level))	


		local pro_value_max = config.pro_level_list[self["pro_level"..info.pro_type]].pro_value_max
		self["pro_value_max"..info.pro_type] = pro_value_max

		print("xyz","pro_value_max---------------------", pro_value_max )		
		self["Text_propertyProgress"..info.pro_type]:SetText(info.pro_value.."/".. pro_value_max )		

		PosTool.CenterBottom(self["special_effect_"..i],0,-40 + 60 *(info.pro_value/ pro_value_max ))
		if info.pro_value == config.pro_level_list[info.pro_level].pro_value_max then
			self["train_pro_full_"..info.pro_type] = true
		else
			self["train_pro_full_"..info.pro_type] = false
		end
		if info.tmp_pro_value ~= 0 then
			IsNeedReplace = true
		end
		local tmp_Data = {type = info.pro_type, value = info.tmp_pro_value}
		table.insert(tmp_pro_value_List,tmp_Data)
	end
	--显示满了的特效
	self:__showHandSpecialEffect()
	if IsNeedReplace then
		self:__showReplaceProperty(tmp_pro_value_List)
	else
		self:__SetButtonPos(self.Button_Pos[1])
	end
end

--替换属性显示
function Modules.HeroTrainTabView:__showReplaceProperty(replace_data)			
	local IsNeedOnlyReplace = true
	local IsNeedOnlyTrain = true
	self.__replace_Pro_List = {}
	self.__windseffect_count = self.__windseffect_count + 1
	for i,info in ipairs(replace_data) do
		self.__replace_Pro_List[info.type] = info.value
		--添加属性文字
		self["Text_changeProperty"..self.__windseffect_count..info.type] = Util:Label("",Macros.TypefaceSize.normal,Macros.Color.btn)
		self:GetWidget("Image_propertyType"..info.type):AddChild(self["Text_changeProperty"..self.__windseffect_count..info.type])
		self["Text_changeProperty"..self.__windseffect_count..info.type]:SetAnchorPoint(0.5,0.5)
		self["Text_changeProperty"..self.__windseffect_count..info.type]:SetPosition(38,135)
		self["Text_changeProperty"..self.__windseffect_count..info.type]:SetVisible(not self["train_pro_full_"..info.type])
		self["Image_arrowsframe"..info.type]:SetVisible(not self["train_pro_full_"..info.type])	
		self["propertyArrows"..info.type]:SetVisible(true)		
		if not self["train_pro_full_"..info.type] then
			if info.value > 0 then
				IsNeedOnlyTrain = false
				self["Text_changeProperty"..self.__windseffect_count..info.type]:SetString("+"..info.value)
				self["Text_changeProperty"..self.__windseffect_count..info.type]:SetColor(unpack(Macros.Color.green))
				self["propertyArrows"..info.type]:SetImage(self.__GreenArrowsData)
			elseif info.value == 0 then
				self["Text_changeProperty"..self.__windseffect_count..info.type]:SetString(info.value)
				self["propertyArrows"..info.type]:SetVisible(false)	
			else
				IsNeedOnlyReplace = false
				self["Text_changeProperty"..self.__windseffect_count..info.type]:SetString(info.value)
				self["Text_changeProperty"..self.__windseffect_count..info.type]:SetColor(unpack(Macros.Color.red))
				self["propertyArrows"..info.type]:SetImage(self.__RedArrowsData)
			end
		end
	end	
	if IsNeedOnlyReplace then
		self:__SetButtonPos(nil,self.Button_Pos[1])
	elseif IsNeedOnlyTrain then
		self:__SetButtonPos(self.Button_Pos[1],nil)
	else
		self:__SetButtonPos(self.Button_Pos[2],self.Button_Pos[1])
	end
end

--显示抓手特效
function Modules.HeroTrainTabView:__showHandSpecialEffect()	
	for pro_type=1,4 do
		if self["pro_reward_flag"..pro_type] == 1 and not self["hand_effect_id_"..pro_type] then
			self["Image_getproperty"..pro_type]:SetVisible(true)
			self["hand_effect_id_"..pro_type],self["special_hand_effect_"..pro_type] = self:__CreateEffect({res_name = "UI_peiyangzoubian",scale = 1.1},self["Panel_ballSpecialEffect_"..pro_type])
			PosTool.Center(self["special_hand_effect_"..pro_type])
		end
	end
end

--属性替换
function Modules.HeroTrainTabView:__replaceProperty()	
	local hero_info = CallModuleFunc (ModuleType.HERO,"GetHeroInfo" , self.__hero_uid )
	local heroCnf = CPPGameLib.GetConfig( "Hero", hero_info.info_id)

	for i=1,4 do
		if not self["train_pro_full_"..i] and self.__replace_Pro_List and self.__replace_Pro_List[i] then
			self["Image_arrowsframe"..i]:SetVisible(self["train_pro_full_"..i])	
			local config = CPPGameLib.GetConfig("HeroTrain",i)
			local downLimit = 0

			local upLimit = config.pro_level_list[self["pro_level"..i]].pro_value_max

			if self["pro_level"..i] > 0 then
				downLimit = config.pro_level_list[self["pro_level"..i] - 1].pro_value_max
			end
			if self["pro_value_"..i] + self.__replace_Pro_List[i] <= downLimit then
				self["pro_value_"..i] = downLimit
			elseif self["pro_value_"..i] + self.__replace_Pro_List[i] >= upLimit then
				self["pro_value_"..i] = upLimit
				self["train_pro_full_"..i] = true
				self["pro_reward_flag"..i] = 1			
			else
				self["pro_value_"..i] = self["pro_value_"..i] + self.__replace_Pro_List[i]
			end
			
			print("xyz" , "===__replaceProperty" , self["pro_value_"..i].."/"..upLimit )
			self["Text_propertyProgress"..i]:SetText( self["pro_value_"..i].."/"..upLimit )
			PosTool.CenterBottom(self["special_effect_"..i],0,-40 + 60 *(self["pro_value_"..i]/upLimit))
		end
	end
	self:__SetButtonPos(self.Button_Pos[1])	
	for pro_type=1,4 do
		local effect_id,effect = self:__CreateEffect({id = 100026,loop = false,end_callback = function ()			
			--飘字
			ActionManager.GetInstance():RunFadeOutAndMoveToEffect(self["Text_changeProperty"..self.__cur_windseffect_count..pro_type],1,
				38,200,GlobalCallbackMgr:AddCallBackFunc(function ()
				if self["Text_changeProperty"..self.__cur_windseffect_count..pro_type] then
					self["Text_changeProperty"..self.__cur_windseffect_count..pro_type]:RemoveSelf(true)
					self["Text_changeProperty"..self.__cur_windseffect_count..pro_type] = nil
				end
			end))
			if pro_type == 4 then
				self:__showHandSpecialEffect()		
			end	
		end},self["Panel_ballSpecialEffect_"..pro_type])
		PosTool.Center(effect)
	end	
end

--按钮位置
function Modules.HeroTrainTabView:__SetButtonPos(pos1,pos2)
	if pos1 then
		self.Button_train:SetVisible(true)	
		self.Button_train:SetPosition(382,pos1)
	else
		self.Button_train:SetVisible(false)	
	end
	if pos2 then
		self.Button_replace:SetVisible(true)	
		self.Button_replace:SetPosition(382,pos2)
	else
		self.Button_replace:SetVisible(false)	
	end

	--英雄培养-培养
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_TRAIN_BTN_TRAIN, self.Button_train)                	

	--英雄培养-替换
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_TRAIN_BTN_REPLACE, self.Button_replace)                	
end

--切换消耗类型
function Modules.HeroTrainTabView:__ChangeCostType(type)
	self.__cost_type = type
	for i=1,3 do
		if i ~= self.__cost_type then
			self["CheckBox_train"..i]:SetSelectedState(false)
		else
			self["CheckBox_train"..i]:SetSelectedState(true)
		end
	end		
end

--切换英雄
function Modules.HeroTrainTabView:RefreshBySelectedHero()

	self.__hero_uid = self.__parent_view:GetCurHeroUID()	
	self.__is_on_train_req = false
	self:CallModuleFunc("HeroDetailInfoReq", self.__hero_uid)
	for pro_type=1,4 do
		if self["Text_changeProperty"..self.__windseffect_count..pro_type] then
			self["Text_changeProperty"..self.__windseffect_count..pro_type]:RemoveSelf(true)
			self["Text_changeProperty"..self.__windseffect_count..pro_type] = nil			
		end
		self["Image_arrowsframe"..pro_type]:SetVisible(false)
		if self["hand_effect_id_"..pro_type] then
			self:__DelEffect(self["hand_effect_id_"..pro_type])
			self["hand_effect_id_"..pro_type] = nil
		end
		self.Button_train:SetEnabled(true)		
		self.Button_replace:SetEnabled(true)
		self["Image_getproperty"..pro_type]:SetVisible(false)	
	end
	self.__windseffect_count = self.__windseffect_count + 1


	print("xyz" , "=======RefreshBySelectedHero=====")
	self:__ResetTrainPrice()
end
