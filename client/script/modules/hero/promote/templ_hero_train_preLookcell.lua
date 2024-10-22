Modules = Modules or {}

Modules.TemplHeroTrainPreLookCell = Modules.TemplHeroTrainPreLookCell or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroTrainPreLookCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_TRAIN_PRE_LOOK_CELL)
end

function Modules.TemplHeroTrainPreLookCell:SetData(data)
	local config = CPPGameLib.GetConfig("HeroTrain",1)
	--特效名字table
	self.special_effect = {[1] = "UI_peiyanggongji",[2] = "UI_peiyangwufang",[3] = "UI_peiyangfafang",[4] = "UI_peiyangshengming"}
	--标题
	local lab = CPPGameLib.GetString("hero_train_need_conditional",data.pro_level,config.pro_level_list[math.max(data.pro_level - 1,0)].need_hero_level)
	Util:WidgetLabel(self:GetWidget("Text_title"),lab,Macros.TypefaceSize.normal,Macros.Color.btn, 0, 0,nil,Macros.Color.shadow_color)
	--设置各样东西
	for i=1,4 do
		local text = ""
		Util:WidgetLabel(self:GetWidget("Text_fullCouldGet"..i),CPPGameLib.GetString("hero_train_preLook_fullexaget"),Macros.TypefaceSize.slightly,Macros.Color.content)
		--添加属性
		local config = CPPGameLib.GetConfig("HeroTrain",i)
		for i,pro_type in ipairs(config.pro_level_list[data.pro_level].level_up_pro_type_list) do
			text = text..CPPGameLib.GetString("nature"..pro_type).." + "..MathTool.TransProbCoeffPercentage(config.pro_level_list[data.pro_level].level_up_pro_value_list[i]).."%"
			if i ~= #config.pro_level_list[data.pro_level].level_up_pro_type_list then
				text = text.." "
			end
		end		
		Util:WidgetLabel(self:GetWidget("Text_fullCouldGetProperty"..i),text,Macros.TypefaceSize.slightly,Macros.Color.content)
		--值
		local Panel_ballSpecialEffect = self:GetWidget("Panel_ballSpecialEffect"..i)
		self["clipping_node_"..i] = Util:CircleClippingNode(0,0,37)
		Panel_ballSpecialEffect:AddChild(self["clipping_node_"..i])
		PosTool.Center(self["clipping_node_"..i])	
		self["effect_id_"..i],self["special_effect_"..i] = self:__CreateEffect({res_name = self.special_effect[i]},self["clipping_node_"..i])
		PosTool.CenterBottom(self["special_effect_"..i],0,-40 + 60 * math.min(1,(data.pro_valueData[i]/config.pro_level_list[data.pro_level].pro_value_max)))
		Util:WidgetLabel(self:GetWidget("Text_propertyProgress"..i),data.pro_valueData[i].."/"..config.pro_level_list[data.pro_level].pro_value_max,Macros.TypefaceSize.slightly,Macros.Color.btn,0,0,Macros.Color.common_stroke)
	end
end

function Modules.TemplHeroTrainPreLookCell:__delete()
	for i=1,4 do
		self:__DelEffect(self["effect_id_"..i])	
	end
end