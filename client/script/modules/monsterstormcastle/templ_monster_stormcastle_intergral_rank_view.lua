LogicGUI = LogicGUI or {}

LogicGUI.TemplMonsterStormCastleIntergralRankView = LogicGUI.TemplMonsterStormCastleIntergralRankView or BaseClass(GUI.Template)

function LogicGUI.TemplMonsterStormCastleIntergralRankView:__init(data) 
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
	self.__tab_params = {
	    {sp_2,sp_1},
	    {  
	    	CPPGameLib.GetString("monster_stormcastle_intergral_rank_title"),
            CPPGameLib.GetString("arena_rankRight"),
		},
		10,
		CPPGameLib.Handler(self, self.ChangeTag),
		true
	}

	self.__open_callback = function ()
		self:createUI(data)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_MONSTER_STROMCASTLE_RANK_CELL)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_RANK_ENCOURAGE_CELL)
	self:InitTemplate(GUI.TemplateRegistry.ARENA_RANK_VIEW)	
end

function LogicGUI.TemplMonsterStormCastleIntergralRankView:createUI(data) 
	self.__data = data
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"), CPPGameLib.GetString("monster_stormcastle_intergral_rank_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
	self:GetWidget("Text_title"):SetShadow(true)
	self:GetWidget("Text_title"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ( )
		self:DeleteMe()
	end)
	--排行榜
	self.__panel_rank = self:GetWidget("Panel_rank")
	--奖励排行榜
	self.__panel_rankEncourage = self:GetWidget("Panel_rankEncourage")
	--页签
	local selected = self:GetWidget("Panel_yeqian")

    self.__tab:On(1)      --默认显示第一个选项卡的
    selected:AddChild(self.__tab:GetNode())
    self.__tab:SetDirection(true,0)
    PosTool.LeftTop(self.__tab,0,0)
	--排行榜
	Util:WidgetLabel(self:GetWidget("Text_myRank"),CPPGameLib.GetString("arena_myRank"),Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--排名数
	self.__text_myranknum = self:GetWidget("Text_myRankNum")
	Util:WidgetLabel(self.__text_myranknum,"",Macros.TypefaceSize.largish, Macros.Color.content)
	--排名奖励
	Util:WidgetLabel(self:GetWidget("Text_rankEncourage"),CPPGameLib.GetString("arena_rankEncourageText"),Macros.TypefaceSize.largish, Macros.Color.keypoint)	
	--奖励容器
	self.__panel_encourage = self:GetWidget("Panel_encourage")	
	self.__text_reputationnum = self:GetWidget("Text_reputationNum")
	Util:WidgetLabel(self.__text_reputationnum,"", Macros.TypefaceSize.largish, Macros.Color.content)
	self.__text_goldnum = self:GetWidget("Text_goldNum")
	Util:WidgetLabel(self.__text_goldnum,"", Macros.TypefaceSize.largish, Macros.Color.content)
	--奖励货币
	local reward_info = CPPGameLib.GetConfig("AttackCityRankReward",1)
	for i=1,2 do
		local sp = Resource.PathTool.GetItemPath(reward_info.item_id_list[i]) 
		self:GetWidget("Image_type"..i):IgnoreContentAdaptWithSize(true)
		self:GetWidget("Image_type"..i):SetImage(sp,TextureResType.LOCAL)
		self:GetWidget("Image_type"..i):SetScaleFactor(i == 1 and 0.5 or 0.7)
	end	
	--未入榜
	self.__text_weirubang = self:GetWidget("Text_weirubang")
	Util:WidgetLabel(self.__text_weirubang,CPPGameLib.GetString("arena_notInRankListText"), Macros.TypefaceSize.largish,  Macros.Color.content )
	--暂无排名
	local text_noone_fornow = self:GetWidget("Text_noOneForNow")
	Util:WidgetLabel(text_noone_fornow,CPPGameLib.GetString("monster_stormcastle_intergral_noone_rightnow"), 50, Macros.Color.btn)
	text_noone_fornow:SetVisible(#self.__data.rank_list <= 0)
	--排行榜容器
	if #self.__data.rank_list > 0 then
		local panel_rankscrollview = self:GetWidget("Panel_rankScrollView")
		local params = {
			item_class = Modules.TemplMonsterStormCastleIntergralRankCell,
		    item_width = 584,
		    item_height	= 122,
		    row	= 4,
		    col	= 1,
		    item_space_r = 5,
		    view_width = 584,
		    view_height = 400,
		}
		self.__rank_scrollview = GUI.ScrollView.New(params)
		panel_rankscrollview:AddChild(self.__rank_scrollview:GetNode())
		PosTool.Center(self.__rank_scrollview)	
		--构造数据
		for i,rank_info in ipairs(self.__data.rank_list) do
			rank_info.rank = i
		end	
		self.__rank_scrollview:SetDataList(self.__data.rank_list)
	end
	self:RankChange()
	--奖励排行榜
	--提示
	Util:WidgetLabel(self:GetWidget("Text_tip"),CPPGameLib.GetString("monster_stormcastle_no_today_tips_4"),Macros.TypefaceSize.largish,Macros.Color.keypoint)
	--排名区间
	Util:WidgetLabel(self:GetWidget("Text_zoom"), CPPGameLib.GetString("arena_rankZoom"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self:GetWidget("Text_zoom"):SetShadow(true)
	self:GetWidget("Text_zoom"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
	--排名奖励
	Util:WidgetLabel(self:GetWidget("Text_encourageTitle"), CPPGameLib.GetString("arena_rankEncourage"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self:GetWidget("Text_encourageTitle"):SetShadow(true)
	self:GetWidget("Text_encourageTitle"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
end

function LogicGUI.TemplMonsterStormCastleIntergralRankView:RankChange()
	if self.__data.rank == 0 then
		self.__text_myranknum:SetText(CPPGameLib.GetString("arena_weirubang"))
		self.__panel_encourage:SetVisible(false)
	else
		self.__text_myranknum:SetText(self.__data.rank)
		self.__text_weirubang:SetVisible(false)
		local info
		for i,config in ipairs(CPPGameLib.GetConfig("AttackCityRankReward", nil, nil , true )) do
			if self.__data.rank >= config.rank_min and self.__data.rank <= config.rank_max then
				info = config
				break
			end
		end
		local iten_config = CPPGameLib.GetConfig("Item",info.item_id_list[1])
		self.__text_reputationnum:SetText(info.item_num_list[1])
		iten_config = CPPGameLib.GetConfig("Item",info.item_id_list[2])
		self.__text_goldnum:SetText(iten_config.param2 * info.item_num_list[2])
	end	
end

function LogicGUI.TemplMonsterStormCastleIntergralRankView:ChangeTag(index)
	self.__panel_rank:SetVisible(false)
	self.__panel_rankEncourage:SetVisible(false)
	if index == 1 then
		self.__panel_rank:SetVisible(true)
	else
		self.__panel_rankEncourage:SetVisible(true)
		if not self.__rank_encourage_scrollview then
			local panel_encouragescrollview = self:GetWidget("Panel_encourageScrollView")
			local params = {
				item_class = Modules.TemplMonsterStormCastleRankCell,
			    item_width = 583,
			    item_height	= 38,
			    row	= 9,
			    col	= 1,
			    item_space_r = 2,
			    view_width = 583,
			    view_height = 360,
			}
			self.__rank_encourage_scrollview = GUI.ScrollView.New(params)
			panel_encouragescrollview:AddChild(self.__rank_encourage_scrollview:GetNode())
			PosTool.Center(self.__rank_encourage_scrollview)
			local dataList = {}
			for i,config in ipairs(CPPGameLib.GetConfig("AttackCityRankReward", nil, nil , true )) do
				local data = {}
				data.rank = i
				table.insert(dataList,data)
			end
			self.__rank_encourage_scrollview:SetDataList(dataList)
		end
	end
end

function LogicGUI.TemplMonsterStormCastleIntergralRankView:__delete()
	if self.__rank_scrollview then
		self.__rank_scrollview:DeleteMe()
		self.__rank_scrollview = nil
	end
	if self.__rank_encourage_scrollview then
		self.__rank_encourage_scrollview:DeleteMe()
		self.__rank_encourage_scrollview = nil
	end
end