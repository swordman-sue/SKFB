LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresOneKeyTips = LogicGUI.TemplPlunderedTreasuresOneKeyTips or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresOneKeyTips:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitMainUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_ONEKEY_TIPS)
end

function LogicGUI.TemplPlunderedTreasuresOneKeyTips:InitMainUI(data)
	self.__data = data
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_tipsTitle"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--内容
	local image_bg = self:GetWidget("Image_bg")
	local item_type,config_info = ObjIDTool.GetInfo(self.__data.treasure_id)	
	local treasure_name = string.format(Macros.Quality_RT[config_info.quality or 1],config_info.name or "")
	print("ldx","treasure_nametreasure_nametreasure_name: ",treasure_name,CPPGameLib.GetString("plunderedtreasures_onekey_plundered_content",treasure_name))
	local rich_content = Util:RichText(CPPGameLib.GetString("plunderedtreasures_onekey_plundered_content",treasure_name), Macros.TypefaceSize.largish, 0, 0, Macros.Color.content_hex)
	image_bg:AddChild(rich_content)
	PosTool.CenterBottom(rich_content,0,177)
	--提示
	Util:WidgetLabel(self:GetWidget("Text_tips"),CPPGameLib.GetString("plunderedtreasures_onekey_plundered_auto_use_tips"),Macros.TypefaceSize.largish,Macros.Color.red)
	--选中框
	local check_box = self:GetWidget("CheckBox")
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("common_cancel"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_red)
	--确定
	Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("common_confirm"),function ()
		--判断精力是否足够
		if ConfigAdapter.Common.GetPlunderedTreasureCostEnery() > CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY) then
			LogicGUI.TemplConsumeBuy.New(Macros.Game.RolePropType.ENERGY,function ()
				--判断精力是否足够
				if ConfigAdapter.Common.GetPlunderedTreasureCostEnery() < CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY) then
					LogicGUI.TemplPlunderedTreasuresOneKeyView.New({treasure_id = self.__data.treasure_id,is_auto_use = check_box:GetSelectedState()})
					self:DeleteMe()	
				end
			end)
		else
			LogicGUI.TemplPlunderedTreasuresOneKeyView.New({treasure_id = self.__data.treasure_id,is_auto_use = check_box:GetSelectedState()})
			self:DeleteMe()	
		end		
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
end

