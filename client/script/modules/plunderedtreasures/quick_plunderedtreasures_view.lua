Modules = Modules or {}
Modules.QuickPlunderedTreasuresView = Modules.QuickPlunderedTreasuresView or BaseClass(BaseView)

function Modules.QuickPlunderedTreasuresView:__init()
	self.__layout_name = "quick_plundered_treasures_view"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_quick_plunder")}
	self.__title_bar_params = {
		close_func = function ()
			--是否正在夺宝
			if self.__is_on_animationing then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("quick_plunderedtreasures_on_animationing"))
				return
			end
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsOpenEnemyViewTag",not self.__is_return_to_enemyview)
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
			self:CloseManual()				
		end,
		title = CPPGameLib.GetString("plunderedtreasures"),
		show_diamond = true,
		show_gold = true,
		show_energy	= true,
		show_level	= true,
		is_plundering = true,
	}
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PLUNDEREDTREASURES)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.QUICK_PLUNDERED_TREASURES_CELL)
	self.__open_callback = function ()	
		self:__OpenCallback()
	end
end

function Modules.QuickPlunderedTreasuresView:GetOpenParams()
	return {self.__data,self.__scrollview_data}
end

function Modules.QuickPlunderedTreasuresView:__OpenCallback()
	self.__data,scrollview_data = unpack(self.__open_data)
	self.__count_time = 0
	local role_head_icon = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
	local module_list = {role_head_icon,self.__data.hero_list[1]}
	self.__panel_quickplunder = self:GetWidget("Panel_QuickPlunder")
	--模型容器
	for i=1,2 do
		self["__hero_module"..i] = LogicGUI.Model.New({info_id = module_list[i],scale = 0.6,dir = i == 1 and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT,parent = self.__panel_quickplunder,zorder = 10 - i})		
		self["__hero_module"..i]:SetPosition(i == 1 and 175 or 495,140)
	end
	--容器背景
	self.__image_listbg = self:GetWidget("Image_ListBg")
	--退出按钮
	self.__btn_exit = self:GetWidget("Button_exit")
	Util:WidgetButton(self.__btn_exit,CPPGameLib.GetString("quick_plunderedtreasures_exit"),function ()
		--是否正在夺宝
		if self.__is_on_animationing then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("quick_plunderedtreasures_on_animationing"))
			return
		end		
		CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsSelectedTag",true)
		CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "SetIsOpenEnemyViewTag",not self.__is_return_to_enemyview)
		self:CloseManual()	
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_red)
	--再抢一次
	self.__btn_again = self:GetWidget("Button_again")
	Util:WidgetButton(self.__btn_again,CPPGameLib.GetString("quick_plunderedtreasures_again"),function ()
		--判断精力是否足够
		if ConfigAdapter.Common.GetPlunderedTreasureCostEnery() > CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY) then						
			--用药
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_eneryNoEnoughTips"))
			LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.ENERGY )
			return
		end
		--请求信息
		self:__PlayAnimation()	
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)

	self.__animation_call = function ()
		if self.__quick_scrollview then
			local length = self.__quick_scrollview:GetItemCount()
			local item = self.__quick_scrollview:GetItem(length)
			if item then
				item:SetPosition(-item:GetWidth(), item:GetPositionY())
				CPPActionManager.MoveBy(item:GetNode(), 0.3, item:GetWidth(), 0)
			end
		end
	end	
	--处理
	self.__btn_again:SetVisible(not self.__data.flag)
	self.__is_return_to_enemyview = self.__data.flag
	if self.__data.flag then		
		self.__btn_exit:SetPosition(self.__image_listbg:GetPositionX(),self.__btn_exit:GetPositionY())		
		local effect
		self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__btn_exit)
		PosTool.Center(effect)
	end
	if scrollview_data and #scrollview_data > 0 then
		self:__InitScrollView(scrollview_data)
	else
		self:__PlayAnimation()
	end
end

function Modules.QuickPlunderedTreasuresView:__RegistAllEvents()
	--夺宝回调
	self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_FIVETIMES, function(_,info)
		--设置状态
		CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"SetIsOnPlunder",false)
		self.__is_on_animationing = false
		--处理
		self.__btn_exit:SetVisible(true)
		self.__btn_again:SetVisible(info.flag ~= 1)
		self.__is_return_to_enemyview = info.flag == 1
		self.__data.flag = info.flag == 1
		if info.flag == 1 then		
			self.__btn_exit:SetPosition(self.__image_listbg:GetPositionX(),self.__btn_exit:GetPositionY())		
			local effect
			self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__btn_exit)
			PosTool.Center(effect)
		end
		--还原位置
		for i=1,2 do
			if i == 2 then
				self["__hero_module"..i]:SetEndCallback(function ()
					local model = self["__hero_module"..i]:GetModel()	
					model:SetVisible(false)
				end)
			end
			if self["__hero_module"..i] then				
				self["__hero_module"..i]:PlayAnimation(i == 1 and RenderObj.ActionIndex.WIN or RenderObj.ActionIndex.DEAD, i == 1 and true or false)								
			end
		end
		--隐藏特效
		if self.__smog_effectid then
			self:__DelEffect(self.__smog_effectid)
			self.__smog_effectid = nil
		end
		--添加条目
		self.__count_time = self.__count_time + 1
		self.__times_data = {}
		self.__times_data.times = self.__count_time
		self.__times_data.item_id = info.item_list[1].item_id
		self.__times_data.item_num = info.item_list[1].item_num
		self.__times_data.treasure_item_id = self.__data.treasure_item_id
		self.__times_data.flag = info.flag
		if not self.__quick_scrollview then
			self:__InitScrollView()
		else
			table.insert(self.__scrollview_data,self.__times_data)
			self.__quick_scrollview:AttachItem(self.__times_data)
			self.__quick_scrollview:LocateToLast()
			self.__animation_call()
		end
	end)
end

function Modules.QuickPlunderedTreasuresView:__Dispose()
	for i=1,2 do
		if self["__hero_module"..i] then
			self["__hero_module"..i]:DeleteMe()
			self["__hero_module"..i] = nil
		end
	end
	if self.__quick_scrollview then
		self.__quick_scrollview:DeleteMe()
		self.__quick_scrollview = nil
	end
end

function Modules.QuickPlunderedTreasuresView:__PlayAnimation()
	self.__is_on_animationing = true
	self.__btn_exit:SetVisible(false)
	self.__btn_again:SetVisible(false)
	--设置状态
	CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"SetIsOnPlunder",true)
	for i=1,2 do
		if self["__hero_module"..i] then
			--还原位置			
		-- for i=1,2 do
		-- 	if self["__hero_module"..i] then		
		-- 		self["__hero_module"..i]:PlayAnimation(i == 1 and RenderObj.ActionIndex.WIN or RenderObj.ActionIndex.DEAD, true)	
		-- 		self["__hero_module"..i]:PlayAnimation(RenderObj.ActionIndex.IDLE, true)	
				
		-- 	end
		-- end		
			self["__hero_module"..i]:SetPosition(i == 1 and 175 or 495,140)		
			local model = self["__hero_module"..i]:GetModel()
			model:SetVisible(true)
			self["__hero_module"..i]:PlayAnimation(RenderObj.ActionIndex.RUN, true)
			CPPActionManager.MoveTo(model:GetNode(),0.6,i == 1 and 320 or 350,140,GlobalCallbackMgr:AddCallBackFunc(function ()
				self["__hero_module"..i]:PlayAnimation(RenderObj.ActionIndex.ATTACK, true)	
				--烟雾特效	
				if i == 1 then						
					if not self.__smog_effectid then
						local effect
						self.__smog_effectid,effect = self:__CreateEffect({id = 100095,loop = true},self.__panel_quickplunder,15)
						effect:SetPosition(335,215)
					end
				end
				--延时处理
				CPPActionManager.DelayTo(model:GetNode(),1,GlobalCallbackMgr:AddCallBackFunc(function ()
					if i == 1 then	
						CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresFiveTimesReq",{treasure_id = self.__data.treasure_id,treasure_piece_id = self.__data.treasure_item_id})	
					end						
				end))
			end))
		end
	end
end

function Modules.QuickPlunderedTreasuresView:__InitScrollView(scrollview_data)
	self.__scrollview_data = scrollview_data or {}	
	if not scrollview_data then
		table.insert(self.__scrollview_data,self.__times_data)
	else
		self.__count_time = #scrollview_data
	end
	local params = {
		item_class = Modules.TemplQuickPlunderedTreasuresCell,
	    item_width	= 423,
	    item_height	= 123,
	    row	= 3,
	    col	= 1,
	    item_space_r = 8,
	    view_width = 423,
	    view_height = 396,
	}
	local panel_scrollview = self:GetWidget("Panel_ScrollView")
	self.__quick_scrollview = GUI.ScrollView.New(params)
	panel_scrollview:AddChild(self.__quick_scrollview:GetNode())
	PosTool.Center(self.__quick_scrollview)	
	self.__quick_scrollview:SetDataList(self.__scrollview_data)
	self.__quick_scrollview:LocateToLast()
	self.__animation_call()
end