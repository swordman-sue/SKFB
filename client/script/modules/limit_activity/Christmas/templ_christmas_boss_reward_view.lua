LogicGUI =LogicGUI or {}

LogicGUI.TemplChristmasBossRewardView = LogicGUI.TemplChristmasBossRewardView or BaseClass(GUI.Template)

function LogicGUI.TemplChristmasBossRewardView:__init(boss_type,boss_level,boss_reward_level)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()		
		self:CreateUI(boss_type,boss_level,boss_reward_level)
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_CHRISTMAS_BOSSREWARD_VIEW)	
end

function LogicGUI.TemplChristmasBossRewardView:__RegistAllEvents()
	--领取奖励响应
	self:BindGlobalEvent(Macros.Event.LimitActivityModule.GET_BOSS_REWARD_RESP, function()
		--获取奖励
		local getReward = {}
	    for i, rData in ipairs(self.__data_list2) do
	        table.insert(getReward,{id = rData.id, num = rData.num})
	    end
	    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )
	    --更新
		self.__boss_reward_level = self.__boss_reward_level + 1
		local panel_allscrollView = self:GetWidget("Panel_AllScrollView")
		self:CreateScrollView(panel_allscrollView,2)
	end)
end

function LogicGUI.TemplChristmasBossRewardView:CreateUI(boss_type,boss_level,boss_reward_level)	
	self.__boss_level = boss_level
	self.__boss_reward_level = boss_reward_level
	self.__boss_type = boss_type
	--标题
	Util:WidgetLabel(self:GetWidget("Text_Title"),CPPGameLib.GetString("dragonboat_boss_reward_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--个人击杀标题
	Util:WidgetLabel(self:GetWidget("Text_CurPersonReward"), CPPGameLib.GetString("dragonboat_boss_reward_celltitle1"), Macros.TypefaceSize.largish, Macros.Color.headline )
	--全服击杀标题
	self.__txt_allreward = self:GetWidget("Text_AllReward")
	Util:WidgetLabel(self.__txt_allreward,"", Macros.TypefaceSize.largish, Macros.Color.headline )
	--可领取按钮
	self.__btn_getreward = self:GetWidget("Button_GetReward")
	Util:WidgetButton(self.__btn_getreward, CPPGameLib.GetString("tower_drawAward"), function ()
		--判断活动是否已经结束
		if not CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"GetActivityOpen",Macros.Game.SystemID.CHRISTMAS_ACTIVITY) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_activity_over"))
			if not self.__is_check_open then
				CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.CHRISTMAS_ACTIVITY)
				self.__is_check_open = true
			end
			return
		end
		--领取奖励
		CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"ChristmasTakeBossRewardReq",self.__boss_type)
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--列表容器
	local panel_personscrollView = self:GetWidget("Panel_PersonScrollView")
	self:CreateScrollView(panel_personscrollView,1)
	local panel_allscrollView = self:GetWidget("Panel_AllScrollView")
	self:CreateScrollView(panel_allscrollView,2)
end

function LogicGUI.TemplChristmasBossRewardView:__delete()
	for index=1,2 do
		if self["__sp_scrollView"..index] then
			self["__sp_scrollView"..index]:DeleteMe()
			self["__sp_scrollView"..index] = nil
		end
	end
	--走边
	if self.__button_zoubian_effect then
		self:__DelEffect(self.__button_zoubian_effect)
		self.__button_zoubian_effect = nil
	end
end

--创建滚动容器
function LogicGUI.TemplChristmasBossRewardView:CreateScrollView(cur_panel,index)
	local config_boss = CPPGameLib.GetConfig("ChristmasBossLevel",self.__boss_type)
	--创建滚动容器
	local params = 
    {
        item_class = Modules.ChristmasRewardItem,
        item_width = 80,
        item_height = 80,
        row = 1,
        col = index == 2 and 4 or 6,
        item_space_c = 8,
        view_width = index == 2 and 344 or 520,
        view_height = 80,       
    }
    self["__sp_scrollView"..index] = GUI.ScrollView.New(params)
    cur_panel:AddChild(self["__sp_scrollView"..index]:GetNode())
    PosTool.Center(self["__sp_scrollView"..index])
    self["__data_list"..index] = {}
    --击杀类型
    if index == 1 then
    	--个人击杀BOSS奖励
    	for i,reward_id in ipairs(config_boss.boss_level_list[self.__boss_level].kill_reward_id_list) do
    		local data = {}
    		data.id = reward_id
    		data.num = config_boss.boss_level_list[self.__boss_level].kill_reward_num_list[i]
    		table.insert(self["__data_list"..index],data)
    	end	
    else
    	--全服击杀BOSS奖励
    	for i,reward_id in ipairs(config_boss.boss_level_list[self.__boss_reward_level+1].boss_reward_id_list) do
    		local data = {}
    		data.id = reward_id
    		data.num = config_boss.boss_level_list[self.__boss_reward_level+1].boss_reward_num_list[i]
    		table.insert(self["__data_list"..index],data)
    	end	
    end
    self["__sp_scrollView"..index]:SetDataList(self["__data_list"..index])
    self.__btn_getreward:SetEnabled((self.__boss_level - self.__boss_reward_level) >= 2)
    --不满足条件
	if (self.__boss_level - self.__boss_reward_level) < 2 then
		self.__btn_getreward:SetTextColor(unpack(Macros.Color.forbidden))	
		local r, g, b, a = unpack(Macros.Color.forbidden_stroke)
		stroke_width = stroke_width or 2
		self.__btn_getreward:SetOutline(r, g, b, a, stroke_width)
		--走边
		if self.__button_zoubian_effect then
			self:__DelEffect(self.__button_zoubian_effect)
			self.__button_zoubian_effect = nil
		end
	else
		if not self.__button_zoubian_effect then
			local effect
			self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__btn_getreward)
			PosTool.Center(effect)
		end
	end
    local boss_name_config = CPPGameLib.GetConfig("ChristmasBoss",self.__boss_type)
    self.__txt_allreward:SetText(CPPGameLib.GetString("dragonboat_boss_reward_celltitle2",boss_name_config.name or "",self.__boss_reward_level+1))   
end
--------------------------------------------------------------------------------------------
--圣诞奖励条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.ChristmasRewardItem = Modules.ChristmasRewardItem or BaseClass(GUI.ScrollItem)

function Modules.ChristmasRewardItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.ChristmasRewardItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,80)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.ChristmasRewardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--
	local icon = LogicGUI.IconLayer.New()
	self.layout:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = self.__data.id,num = self.__data.num})
end