LogicGUI =LogicGUI or {}

LogicGUI.TemplArenaFightReportView = LogicGUI.TemplArenaFightReportView or BaseClass(GUI.Template)

function LogicGUI.TemplArenaFightReportView:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:createUI(data)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_FIGHTREPORT_CELL)
	self:InitTemplate(GUI.TemplateRegistry.ARENA_FIGHTREPORT_VIEW)	
end

function LogicGUI.TemplArenaFightReportView:createUI(data) 
	--标题	
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("arena_fightReportTitle"), Macros.TypefaceSize.popup, Macros.Color.btn)
	--关闭按钮	
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--战报
	if data and #data.record_list > 0 then
		local Panel_scrollview = self:GetWidget("Panel_scrollview")
		local params = {
			item_class	= Modules.TemplArenaFightReportCell,
		    item_width	= 625,
		    item_height = 105,
		    item_space_r = 7,
		    row	= 4,
		    col	= 1,
		    view_width = 625,
		    view_height = 436,
		}
		self.ScrollView = GUI.ScrollView.New(params)
		Panel_scrollview:AddChild(self.ScrollView:GetNode())
		PosTool.Center(self.ScrollView)
		local sortTable = {}
		for i,v in ipairs(data.record_list) do
			local isRobot = ObjIDTool.GetArenaRobotData(v.target_id)
			if isRobot then				
				local robotData = CPPGameLib.GetConfig("ArenaRobot",v.target_id)
				v.target_name = robotData.name
				v.target_level = robotData.level
			end
			table.insert(sortTable,1,v)
		end
		self.ScrollView:SetDataList(sortTable)
		--数量
		Util:WidgetLabel(self:GetWidget("Text_reportNum"),#data.record_list.."/10",Macros.TypefaceSize.largish,Macros.Color.content)
	else
		Util:WidgetLabel(self:GetWidget("Text_reportNum"),"0/10",Macros.TypefaceSize.largish,Macros.Color.content)
	end	
	--数量
	Util:WidgetLabel(self:GetWidget("Text_num"),CPPGameLib.GetString("arena_numText"),Macros.TypefaceSize.largish,Macros.Color.keypoint)
	
end

function LogicGUI.TemplArenaFightReportView:__delete()
	if self.ScrollView then
		self.ScrollView:DeleteMe()
		self.ScrollView = nil
	end
end