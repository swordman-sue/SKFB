LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldHeartLightUpView = LogicGUI.TemplWorldHeartLightUpView or BaseClass(GUI.Template)

function LogicGUI.TemplWorldHeartLightUpView:__init(cur_stage,is_fulllevel)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.IMAGE,image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_worldheart")}
	self.__open_callback = function ()
		self:initUI(cur_stage,is_fulllevel)
	end
	self:InitTemplate(GUI.TemplateRegistry.WORLD_HEART_LIGHTUP)	
end

function LogicGUI.TemplWorldHeartLightUpView:initUI(cur_stage,is_fulllevel)
	local spirit_info = CPPGameLib.GetConfig("HeartOfWorld",cur_stage)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_lightup"),CPPGameLib.GetString("spirit_lightup_title",spirit_info.elves_list[#spirit_info.elves_list].name), 36, Macros.Color.Special)
	--特效
	local panel_effect = self:GetWidget("Panel_effect")
	local effectid,effect = self:__CreateEffect({id = 100038},panel_effect,4)
	PosTool.Center(effect)
	--属性标题
	Util:WidgetLabel(self:GetWidget("Text_addPropertyTitle"), CPPGameLib.GetString("spirit_lightup_addproperty_title"), Macros.TypefaceSize.headline, Macros.Color.btn,0,0,Macros.Color.common_stroke)
	--点击继续游戏
	self.__panel_lead = self:GetWidget("Panel_lead")
	self.__text_continute = self:GetWidget("Text_continute")
	Util:WidgetLabel(self.__text_continute, CPPGameLib.GetString("plunderedtreasures_TouchScreenTip"), Macros.TypefaceSize.popup,Macros.Color.btn,0,0,Macros.Color.common_stroke)
	--属性初始化
	CPPActionManager.DelayTo(self:GetWidget("Text_addPropertyTitle"),1, GlobalCallbackMgr:AddCallBackFunc(function()					
		for i,add_pro_type in ipairs(spirit_info.elves_list[#spirit_info.elves_list].add_pro_type) do
			local Text_property = self:GetWidget("Text_property"..i)
			Text_property:SetVisible(true)
			local str = Util:Nature(add_pro_type,spirit_info.elves_list[#spirit_info.elves_list].add_pro_value[i])
			Util:WidgetLabel(Text_property, str, 26,Macros.Color.green)
			local effectid,effect = self:__CreateEffect({id = 100045,loop = false},Text_property,4)
			PosTool.Center(effect)		
		end		
		self.__text_continute:SetVisible(true)	
		--关闭
		WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
			self:DeleteMe()
			self:FireNextFrame(Macros.Event.WorldHeartModule.WORLD_HEART_LIGHTUP,is_fulllevel)
		end)	
		self:__CheckSectionOperateSP()		
	end))
		
end

--新手指引特殊操作检测
function LogicGUI.TemplWorldHeartLightUpView:__CheckSectionOperateSP()				
	--召唤英雄展示点击继续
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.WORLD_HEART_TALENT_CONTINUTE,self.__panel_lead)				
end
