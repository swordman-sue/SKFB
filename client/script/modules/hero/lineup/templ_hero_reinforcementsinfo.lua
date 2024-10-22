GUI = GUI or {}
GUI.TemplHeroReinforcementsInfo = GUI.TemplHeroReinforcementsInfo or BaseClass(GUI.Template)

function GUI.TemplHeroReinforcementsInfo:__init(name,heroList,des,IsMakeWork,equipTable,divination_type_list,recommend_card_list)
	self:InitTemplate(GUI.TemplateRegistry.HERO_REINFORCEMENTS_INFO)
	self:createMainUI(name,heroList,des,IsMakeWork,equipTable,divination_type_list,recommend_card_list)	
end

function GUI.TemplHeroReinforcementsInfo:__AddToParent()

end

function GUI.TemplHeroReinforcementsInfo:createMainUI(name,heroList,des,IsMakeWork,equipTable,divination_type_list,recommend_card_list)
	--宿命名字 
	Util:WidgetLabel(self:GetWidget("Text_reinforcementName"),name, Macros.TypefaceSize.largish, Macros.Color.content)
	--宿命队列
	local listView_reinforcementList = self:GetWidget("ListView_reinforcementList")
	listView_reinforcementList:SetTouchSwallow(false)
	local OnFightHeroList = CallModuleFunc(ModuleType.HERO,"GetAllLineupHeroList")	
	for i,info in ipairs(heroList) do
		local is_Gray = true
		if info ~= "text" then
			if OnFightHeroList[info] then
				is_Gray = false
			else
				if equipTable[info] then
					is_Gray = false
				elseif divination_type_list and divination_type_list[info] then
					is_Gray = false
				else 
					is_Gray = true
				end
			end
		end
		if recommend_card_list and i ~= 1 then
			--获取英雄信息
			local card_id = recommend_card_list[i-1]
			if card_id then
				info = card_id
			else
				return
			end
		end
		local layout = Util:Layout(info ~= "text" and 80 or 15,80)
		layout:SetBackGroundColor(0, 0, 0)
		if info ~= "text" then
			local icon = LogicGUI.IconLayer.New()
			icon:SetAnchorPoint(0.5,0.5)  
			layout:AddChild(icon:GetNode())
			PosTool.Center(icon)  
			icon:SetData({id = info})
			icon:SetGray(is_Gray)
	   	else
	   		local lable = Util:Label(CPPGameLib.GetString("manor_dispatch3"), Macros.TypefaceSize.largish, Macros.Color.blinkcontent,0,0,Macros.Color.common_stroke)
	   		layout:AddChild(lable)
	   		PosTool.Center(lable)
	   	end
	   	listView_reinforcementList:PushBackCustomItem(layout)
	end
	--宿命描述
	Util:WidgetLabel(self:GetWidget("Text_reinforcementDes"),des,Macros.TypefaceSize.normal, Macros.Color.content,470,50)
	--变绿
	if IsMakeWork then
		self:GetWidget("Text_reinforcementName"):SetColor(unpack(Macros.Color.front_color))
		self:GetWidget("Text_reinforcementDes"):SetColor(unpack(Macros.Color.front_color))
	end
	
end

function GUI.TemplHeroReinforcementsInfo:__delete()
	
end

--[[
	头像条目
]]
-- Modules = Modules or {}
-- Modules.ReinforcementsItem = Modules.ReinforcementsItem or BaseClass(GUI.ScrollItem)

-- function Modules.ReinforcementsItem:__init()
-- 	self.__layout_mode = GUI.Template.LayoutMode.CODE
--     self:InitTemplate(nil)  
-- end

-- function Modules.ReinforcementsItem:__CreateWidgetFromLuaCode()
--   self.layout = Util:Layout(80,80)
--   self.layout:SetTouchEnabled(true)
--   self.layout:SetClippingEnable(false)
--   self.layout:SetBackGroundColor(0, 0, 0)
--   return self.layout
-- end

-- function Modules.ReinforcementsItem:SetData(data)
-- 	GUI.ScrollItem.SetData(self, data)
-- 	 --头像
-- 	if data and data.info_id then
-- 		self.icon = LogicGUI.IconLayer.New(false,false)
-- 		self.icon:SetAnchorPoint(0.5,0.5)  
-- 		self.layout:AddChild(self.icon:GetNode())
-- 		PosTool.Center(self.icon)  
-- 		self.icon:SetData({id = data.info_id})
-- 		self.icon:SetGray(data.IsGray)
--    	else
--    		local lable = Util:Label(data.text, Macros.TypefaceSize.normal, Macros.Color.content)
--    		self.layout:AddChild(lable)
--    		PosTool.Center(lable)
--    	end
-- end