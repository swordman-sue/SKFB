--
-- @brief 领奖中心
-- @auther: ldx
-- @date 2016年11月22日 11:48:36
--

Modules = Modules or {}
Modules.AcceptPrizeCenterView = Modules.AcceptPrizeCenterView or BaseClass(BaseView)

function Modules.AcceptPrizeCenterView:__init()
	self.__layout_name = "award_center"
	self.__bg_params = {type = BaseView.BGType.SOLID} 
	--todo
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACCEPT_PRIZE_CENTER_CELL)
	self.__open_callback = function ()
		self:__OpenCallback()
	end
end

function Modules.AcceptPrizeCenterView:__OpenCallback()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("accept_prize_center_title"),Macros.TypefaceSize.popup,Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
		self:CloseManual()
	end)
	--一键领取
	Util:WidgetButton(self:GetWidget("Button_oneKeyGet"),CPPGameLib.GetString("accept_prize_center_oneKey_get"),function ()
		local dataList = self:CallModuleFunc("GetRewardCenterList")
		local info = {}		
		for i,listData in ipairs(dataList) do
			local rewardData = {}
			rewardData.id = listData.id
			table.insert(info,rewardData)
		end
		CallModuleFunc(ModuleType.ACCEPT_PRIZE_CENTER,"RewardCenterGetReq",info,nil,true)			
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_red)

	--ScrollView 容器
	local dataList = self:CallModuleFunc("GetRewardCenterList")
	self.Panel_ScrollView = self:GetWidget("Panel_ScrollView")
	local params = {
		item_class	= Modules.TemplAcceptPrizeCenterCell,
	    item_width	= 509,
	    item_height	= 155,
	    row	= 2,
	    col = 1,
	    item_space_r = 8,
	    view_width = 509,
	    view_height = 313,
	}
	--预处理数据
	for i,cur_data in ipairs(dataList) do
		cur_data.dataList = {}
		--物品表
		for i,itemData in ipairs(cur_data.item_list) do
			local data = {}
			data.info_id = itemData.item_id
			data.item_num = itemData.item_num
			table.insert(cur_data.dataList, data)
		end
		--货币表
		for i,moneyData in ipairs(cur_data.currency_list) do
			local data = {}
			local money_Config = CPPGameLib.GetConfig("HuoBi",moneyData.type)
			data.info_id = money_Config.big_icon
			data.item_num = moneyData.value
			table.insert(cur_data.dataList, data)
		end
	end
	self.ScrollView = GUI.ScrollView.New(params)
	self.Panel_ScrollView:AddChild(self.ScrollView:GetNode())
	PosTool.Center(self.ScrollView)
	self.ScrollView:SetDataList(dataList)
end

function Modules.AcceptPrizeCenterView:__RegistAllEvents()
	--移除控件
	self:BindGlobalEvent(Macros.Event.AcceptPrizeCenterModule.REWARD_DELETE_ITEM, function(_, index,is_onekey_get) 
		--是否是一键领取
		if not is_onekey_get then 
			local item_data = self.ScrollView:GetItemData(index)
			local getReward = {}
	        for i, rData in ipairs(item_data.dataList) do
	            table.insert(getReward,{id = rData.info_id , num = rData.item_num})
	        end
	        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )

	        --列表相关调整
	        self.ScrollView:DetachItem(index)   
	        if self.ScrollView:GetItemCount() == 0 then
	        	self:CloseManual()
	        end     

        else
        	if self.ScrollView:GetItemCount() > 0 then
	        	local getReward = {}
	        	self.ScrollView:ItemDataIter(function(item_index, item_data)
	        		if item_data and item_data.dataList then
						for key, rData in ipairs(item_data.dataList) do
							table.insert(getReward, {id = rData.info_id , num = rData.item_num})
						end
					end
	        	end)
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )
				self.ScrollView:SetDataList(nil)
			end					
			self:CloseManual()
        end     
    end)
end

function Modules.AcceptPrizeCenterView:__Dispose()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end