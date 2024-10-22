LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroTrainTimesChooseView = LogicGUI.TemplHeroTrainTimesChooseView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroTrainTimesChooseView:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.HERO_TRAIN_TIMES_CHOOSE_VIEW)
end

function LogicGUI.TemplHeroTrainTimesChooseView:InitUI()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_train_traintimes_title"),Macros.TypefaceSize.popup,Macros.Color.btn, 0, 0,nil,Macros.Color.shadow_color)
	--关闭按钮	
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ( )
		self:DeleteMe()
	end)
	--关闭按钮	
	Util:WidgetButton(self:GetWidget("Button_closeToo"),CPPGameLib.GetString("common_close"),function ( )
		self:DeleteMe()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
	
	local times_list = {1,5,10}
	local limit_conditional_list = {{1,0}}
	local conList = ConfigAdapter.Common.GetHeroTrainFiveTimesNeedVipLevelAndLevel()
	table.insert(limit_conditional_list,conList)
	conList = ConfigAdapter.Common.GetHeroTrainTenTimesNeedVipLevelAndLevel()
	table.insert(limit_conditional_list,conList)
	--培养次数
	for i=1,3 do
		local Image_times = self:GetWidget("Image_times"..i)
		local satisfy = false
		local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		local vip_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)	
		if level >= limit_conditional_list[i][2] or vip_level >= limit_conditional_list[i][1] then
			satisfy = true
		end
		WidgetUnity.SetWidgetListener(Image_times, nil, nil,function ()					
			if satisfy then
				self:FireNextFrame(Macros.Event.HeroModule.HERO_TRAIN_TIMES_CHOOSE,times_list[i])
				self:DeleteMe()
			else
				LogicGUI.TemplAffirmTips.New({content = CPPGameLib.GetString("hero_train_need_vip_tips"),
											  confirm_text = CPPGameLib.GetString("common_confirm"),
		                                        confirm_func = function ()
		                                        	self:DeleteMe()
		                                            --前往充值界面
													GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.CHARGE)
		                                        end,cancel_text = CPPGameLib.GetString("common_cancel"),})
				-- GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_train_times_conditional",limit_conditional_list[i][1],limit_conditional_list[i][2]))
			end
		end)
		--培养次数
		Util:WidgetLabel(WidgetUnity.GetWidgetByName(Image_times,"Text_trainTimes"),CPPGameLib.GetString("hero_train_traintimes",times_list[i]),Macros.TypefaceSize.largish,Macros.Color.headline )
		--点击选择
		Util:WidgetLabel(WidgetUnity.GetWidgetByName(Image_times,"Text_clickchoose"),CPPGameLib.GetString("hero_train_click_choose"),Macros.TypefaceSize.largish,Macros.Color.content)
		--开启条件
		local Text_openCondition = WidgetUnity.GetWidgetByName(Image_times,"Text_openCondition")
		Text_openCondition:SetVisible(i ~= 1 and (not satisfy) or false)
		Util:WidgetLabel(Text_openCondition,CPPGameLib.GetString("hero_train_times_conditionaltext",limit_conditional_list[i][1],limit_conditional_list[i][2]),Macros.TypefaceSize.largish,Macros.Color.red,125,100)	
	end	
end