LogicGUI = LogicGUI or {}

LogicGUI.TemplArenaRankView = LogicGUI.TemplArenaRankView or BaseClass(GUI.Template)

function LogicGUI.TemplArenaRankView:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
	self.__tab_params = {
	    {sp_2,sp_1},
	    {  
	    	CPPGameLib.GetString("arena_rankTitle"),
            CPPGameLib.GetString("arena_rankRight"),
		},
		10,
		CPPGameLib.Handler(self, self.ChangeTag),
		true
	}

	self.__open_callback = function ()
		self:createUI(data)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_RANK_CELL)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_RANK_ENCOURAGE_CELL)
	self:InitTemplate(GUI.TemplateRegistry.ARENA_RANK_VIEW)	
end

function LogicGUI.TemplArenaRankView:createUI(data) 
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"), CPPGameLib.GetString("arena_rankTitle"), Macros.TypefaceSize.tab, Macros.Color.btn)
	self:GetWidget("Text_title"):SetShadow(true)
	self:GetWidget("Text_title"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ( )
		self:DeleteMe()
	end)
	--排行榜
	self.Panel_rank = self:GetWidget("Panel_rank")
	--奖励排行榜
	self.Panel_rankEncourage = self:GetWidget("Panel_rankEncourage")
	--页签
	local selected = self:GetWidget("Panel_yeqian")
	selected:SetZOrder(2)
    self.__tab:On(1)      --默认显示第一个选项卡的
    selected:AddChild(self.__tab:GetNode())
    self.__tab:SetDirection(true,0)
    PosTool.LeftTop(self.__tab,0,0)
	--排行榜
	Util:WidgetLabel(self:GetWidget("Text_myRank"),CPPGameLib.GetString("arena_myRank"),Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--排名数
	self.Text_myRankNum = self:GetWidget("Text_myRankNum")
	Util:WidgetLabel(self.Text_myRankNum,"",Macros.TypefaceSize.largish, Macros.Color.content)
	--排名奖励
	Util:WidgetLabel(self:GetWidget("Text_rankEncourage"),CPPGameLib.GetString("arena_rankEncourageText"),Macros.TypefaceSize.largish,Macros.Color.keypoint)
	--奖励容器
	self.Panel_encourage = self:GetWidget("Panel_encourage")	
	self.Text_reputationNum = self:GetWidget("Text_reputationNum")
	Util:WidgetLabel(self.Text_reputationNum,"", Macros.TypefaceSize.largish, Macros.Color.content)
	self.Text_goldNum = self:GetWidget("Text_goldNum")
	Util:WidgetLabel(self.Text_goldNum,"", Macros.TypefaceSize.largish, Macros.Color.content)
	--未入榜
	self.Text_weirubang = self:GetWidget("Text_weirubang")
	Util:WidgetLabel(self.Text_weirubang,CPPGameLib.GetString("arena_notInRankListText"), Macros.TypefaceSize.largish, Macros.Color.content)
	--排行榜容器
	local Panel_rankScrollView = self:GetWidget("Panel_rankScrollView")
	local params = {
		item_class = Modules.TemplArenaRankCell,
	    item_width = 584,
	    item_height	= 122,
	    row	= 4,
	    col	= 1,
	    item_space_r = 5,
	    view_width = 584,
	    view_height = 400,
	}
	self.RankScrollView = GUI.ScrollView.New(params)
	Panel_rankScrollView:AddChild(self.RankScrollView:GetNode())
	PosTool.Center(self.RankScrollView)
	for i,v in ipairs(data.rank_list) do
		local isRobot = ObjIDTool.GetArenaRobotData(v.role_id)
		if isRobot then				
			local robotData = CPPGameLib.GetConfig("ArenaRobot",v.role_id)
			local battleValue = 0
			for key,monsterId in ipairs(robotData.monster_list) do
				battleValue = battleValue + CallModuleFunc(ModuleType.POWER, "CalcMonsterPower", monsterId,robotData.add_pro_type_list,robotData.add_pro_value_list)
			end
			v.battle_value = battleValue
			v.role_name = robotData.name
			v.level = robotData.level				
			v.head_icon = robotData.head_img_id	
		end
		v.rank = i
	end
	self.RankScrollView:SetDataList(data.rank_list)
	self:RankChange(data.rank)
	--奖励排行榜
	--提示
	Util:WidgetLabel(self:GetWidget("Text_tip"),CPPGameLib.GetString("arena_rankEncourageTip"),Macros.TypefaceSize.largish, Macros.Color.keypoint )
	--排名区间
	Util:WidgetLabel(self:GetWidget("Text_zoom"), CPPGameLib.GetString("arena_rankZoom"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self:GetWidget("Text_zoom"):SetShadow(true)
	self:GetWidget("Text_zoom"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
	--排名奖励
	Util:WidgetLabel(self:GetWidget("Text_encourageTitle"), CPPGameLib.GetString("arena_rankEncourage"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self:GetWidget("Text_encourageTitle"):SetShadow(true)
	self:GetWidget("Text_encourageTitle"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
end

function LogicGUI.TemplArenaRankView:RankChange(rank)
	if rank == 0 then
		self.Text_myRankNum:SetText(CPPGameLib.GetString("arena_weirubang"))
		self.Panel_encourage:SetVisible(false)
	else
		self.Text_myRankNum:SetText(rank)
		self.Text_weirubang:SetVisible(false)
		local info = nil
		for i,config in pairs(CPPGameLib.GetConfig("Arena", nil, nil , true )) do
			if rank >= config.rank_min and rank <= config.rank_max then
				info = config
				break
			end
		end
		self.Text_reputationNum:SetText(info.award_prestige)
		self.Text_goldNum:SetText(info.award_gold)
	end	
end

function LogicGUI.TemplArenaRankView:ChangeTag(index)
	self.Panel_rank:SetVisible(false)
	self.Panel_rankEncourage:SetVisible(false)
	if index == 1 then
		self.Panel_rank:SetVisible(true)
	else
		self.Panel_rankEncourage:SetVisible(true)
		if not self.RankEncourageScrollView then
			local Panel_encourageScrollView = self:GetWidget("Panel_encourageScrollView")
			local params = {
				item_class = Modules.TemplArenaRankEncourageCell,
			    item_width = 583,
			    item_height	= 38,
			    row	= 9,
			    col	= 1,
			    item_space_r = 2,
			    view_width = 583,
			    view_height = 360,
			}
			self.RankEncourageScrollView = GUI.ScrollView.New(params)
			Panel_encourageScrollView:AddChild(self.RankEncourageScrollView:GetNode())
			PosTool.Center(self.RankEncourageScrollView)
			local dataList = {}
			for k,config in pairs(CPPGameLib.GetConfig("Arena", nil, nil , true )) do
				table.insert(dataList,config)
			end
			table.sort(dataList,function (configA,configB)
				return configA.rank_max < configB.rank_min
			end)
			for i,rankInfo in ipairs(dataList) do
				rankInfo.listNum = i
			end
			self.RankEncourageScrollView:SetDataList(dataList)
		end
	end
end

function LogicGUI.TemplArenaRankView:__delete()
	if self.RankScrollView then
		self.RankScrollView:DeleteMe()
		self.RankScrollView = nil
	end
	if self.RankEncourageScrollView then
		self.RankEncourageScrollView:DeleteMe()
		self.RankEncourageScrollView = nil
	end
end