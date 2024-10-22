LogicGUI = LogicGUI or {}

LogicGUI.TemplVipRecruitPrelook = LogicGUI.TemplVipRecruitPrelook or BaseClass(GUI.Template)

function LogicGUI.TemplVipRecruitPrelook:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initBaseUI()
	end
	self:__AddTemplate(GUI.TemplateRegistry.VIP_RECRUIT_PRELOOK_CELL)
	self:InitTemplate(GUI.TemplateRegistry.VIP_RECRUIT_PRELOOK)
end

function LogicGUI.TemplVipRecruitPrelook:__delete()
	if self.__del_panel_lst then
		for _,panel in ipairs(self.__del_panel_lst) do
			panel:DeleteMe()
		end
		self.__del_panel_lst = nil
	end	
end

function LogicGUI.TemplVipRecruitPrelook:initBaseUI()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_viprecruitPreLookTitle"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, nil, Macros.Color.shadow_color)
	--需要删除容器列表
	self.__del_panel_lst = {}
	--预览
	self.__listview = self:GetWidget("ListView")
	local config_camp
	for camp = 0,2 do
		config_camp = CPPGameLib.GetConfig("HeroVipRecruit",camp)
		local tmp_list = {}
		for hero_id,_ in pairs(config_camp.hero_list) do
			table.insert(tmp_list,hero_id)
		end
		table.sort(tmp_list,function (id1,id2)
			return id1 < id2
		end)
		local panel = GUI.TemplVipRecruitPrelookCell.New(camp,tmp_list)
		table.insert(self.__del_panel_lst,panel)
		self.__listview:PushBackCustomItem(panel:GetNode())
	end

	--关闭按钮
	local buttonClose = self:GetWidget("Button_close")
	WidgetUnity.SetWidgetListener(buttonClose, nil, nil, function()
		self:DeleteMe()
	end)

	
end


--------------------------------------------------------------------------------------------
--[[
	头像条目
]]
GUI = GUI or {}
GUI.TemplVipRecruitPrelookCell = GUI.TemplVipRecruitPrelookCell or BaseClass(GUI.Template)

function GUI.TemplVipRecruitPrelookCell:__init(camp, hero_list)
	self:InitTemplate(GUI.TemplateRegistry.VIP_RECRUIT_PRELOOK_CELL)
	self:CreateUI(camp,hero_list)
end

function GUI.TemplVipRecruitPrelookCell:__AddToParent()

end

function GUI.TemplVipRecruitPrelookCell:CreateUI(camp, hero_list)

	--阵营
	local txt_Camp = self:GetWidget("Text_Camp")
	Util:WidgetLabel(txt_Camp, CPPGameLib.GetString("hero_vip_camp_recruit_prelook_"..camp), Macros.TypefaceSize.popup, Macros.Color.Special, 0, 0, Macros.Color.common_stroke)
	
	--队列容器
	local panel_scrollview = self:GetWidget("Panel_ScrollView")

	--图标长度
	local item_width = 80
	local item_space_c = 50
	local item_space_r = 50
	local panel_space_r = 10
	--设置容器大小
	panel_scrollview:SetContentSize(panel_scrollview:GetContentWidth(),math.ceil(#hero_list/5) * (item_width + item_space_r)+panel_space_r)
	self:GetNode():SetContentSize(self:GetNode():GetContentWidth(),panel_scrollview:GetContentHeight()+55+panel_space_r)
	PosTool.LeftTop(panel_scrollview,0,-55)
	PosTool.CenterTop(self:GetWidget("Image_9"))
	
	--队列图标
	local hero_count = 0
	local pos_x,pos_y
	for _,hero_id in ipairs(hero_list) do
		hero_count = hero_count + 1
		pos_x = ((hero_count%5 == 0 and 5 or hero_count%5) - 1)*(item_width + item_space_c)
		pos_y = -(math.ceil(hero_count/5) - 1)*(item_width + item_space_c)
		--头像
	    local icon = LogicGUI.IconLayer.New()
	    icon:SetAnchorPoint(0,1)  
	    panel_scrollview:AddChild(icon:GetNode())
	    PosTool.LeftTop(icon,pos_x,pos_y)  
	    icon:SetData({id = hero_id})
	    icon:ShowHeroName(true)
	    icon:ShowItemName(true)
	end
end

