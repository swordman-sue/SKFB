
LogicGUI = LogicGUI or {}

LogicGUI.TemplIntegralRaceRankView = LogicGUI.TemplIntegralRaceRankView or BaseClass(GUI.Template)

function LogicGUI.TemplIntegralRaceRankView:__init(camp)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.__cur_camp = camp or 0
		self:__ShowUI()
	end
	
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_INTEGRALRACE_RANK_CELL)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_INTEGRALRACE_RANK_SP_CELL)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_INTEGRALRACE_RANK_VIEW)	
end

function LogicGUI.TemplIntegralRaceRankView:__delete()
	for _,scrollview in ipairs(self.__scrollview_list) do
		scrollview:DeleteMe()
		scrollview = nil
	end
end

function LogicGUI.TemplIntegralRaceRankView:__RegistAllEvents()
    --阵容红点
    self:BindGlobalEvent(Macros.Event.RankModule.RANK_LIST_REQ, function(_,info)
        for index,rank_type in ipairs(self.__left_rank_type) do
        	if rank_type == info.type then
        		self:CreateScrollView(index,info)
        		break
        	end
        end
    end)
end

function LogicGUI.TemplIntegralRaceRankView:__ShowUI()
	--标题	
	Util:WidgetLabel(self:GetWidget("Text_title"), CPPGameLib.GetString("integral_rank_title"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--左边容器
	self.__panel_tab = self:GetWidget("Panel_tab")
	--关闭
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--我的排名
	self.__txt_myrank = self:GetWidget("Text_MyRank")
	Util:WidgetLabel(self.__txt_myrank,CPPGameLib.GetString("rank_myRank"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--我的排名数字
	self.__txt_myranknum = self:GetWidget("Text_MyRankNum")
	Util:WidgetLabel(self.__txt_myranknum, "", Macros.TypefaceSize.largish, Macros.Color.content)
	--排行容器
	self.__panel_encouragescrollview = self:GetWidget("Panel_encourageScrollView")
	--奖励排行榜
	--提示
	Util:WidgetLabel(self:GetWidget("Text_tip"),CPPGameLib.GetString("integral_rankencouragetip"),Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--排名区间
	Util:WidgetLabel(self:GetWidget("Text_zoom"), CPPGameLib.GetString("arena_rankZoom"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self:GetWidget("Text_zoom"):SetShadow(true)
	self:GetWidget("Text_zoom"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
	--排名奖励
	Util:WidgetLabel(self:GetWidget("Text_encourageTitle"), CPPGameLib.GetString("arena_rankEncourage"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self:GetWidget("Text_encourageTitle"):SetShadow(true)
	self:GetWidget("Text_encourageTitle"):SetShadowColor(unpack({0X00,0X00,0X00,0XFF}))
	--左边列表排行榜类型
	self.__left_rank_type = {Macros.Game.RankType.INTEGRAL_RACE_SUPER,Macros.Game.RankType.INTEGRAL_RACE_LEGEND,Macros.Game.RankType.INTEGRAL_RACE_WARCRAFT}
	--左边列表
    local btn_res = {"btn_btn_biggreen","btn_btn_bigblue"}
    self.__btn_res = {}
    for _,btn_res_name in ipairs(btn_res) do
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,btn_res_name)
        table.insert(self.__btn_res,sp)
    end 
    --初始化
    local left_namelist = {}
    local left_iconlist = {}
    for i=1,3 do
        local camp_name = CPPGameLib.GetString("hero_handbook_buttonType_"..(i-1))
        table.insert(left_namelist,camp_name)
        local camp_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"camp_icon_"..(i-1),true)
        table.insert(left_iconlist,camp_sp)
    end 
    table.insert(left_namelist,CPPGameLib.GetString("rebel_awardRank"))
    self.__btn_list = {}
    self.__left_panel_list = {}
    local offset_y = 0
    local interval = 20
    for i,name_str in ipairs(left_namelist) do
    	--左边容器列表
    	local panel_scrollview = self:GetWidget("Panel_Scrollview"..i)
    	table.insert(self.__left_panel_list,panel_scrollview)
        local sp_data = {}
        local btn = Util:Button(self.__btn_res[2],function ()
            self:SetSelectedCallBack(i)
        end)
        btn:SetAnchorPoint(0,1)
        self.__panel_tab:AddChild(btn)
        PosTool.LeftTop(btn,0,offset_y)
        sp_data.btn = btn
        table.insert(self.__btn_list,sp_data)
        offset_y = offset_y - btn:GetContentHeight() - interval
        if left_iconlist[i] then
	        --图标
	        local icon = Util:Sprite9(left_iconlist[i])
	        icon:SetAnchorPoint(0,0.5)
	        btn:AddChild(icon)
	        PosTool.LeftCenter(icon,5,2)
	        icon:SetScaleFactor(0.9)
	    end
        --文字
        sp_data.label = Util:Label(name_str, Macros.TypefaceSize.popup, Macros.Color.btn,0,0,Macros.Color.button_yellow)
        sp_data.label:SetAnchorPoint(0.5,0.5)
        btn:AddChild(sp_data.label)
        if left_iconlist[i] then
	        PosTool.Center(sp_data.label,35,0)
	    else
	    	PosTool.Center(sp_data.label)
	    end
    end
    self:SetSelectedCallBack(1)
end

function LogicGUI.TemplIntegralRaceRankView:SetSelectedCallBack(index)
	if self.__selected_index and self.__selected_index == index then
        return
    end
    self.__selected_index = index
    self.__txt_myrank:SetVisible(index == (self.__cur_camp + 1))
	self.__txt_myranknum:SetVisible(index == (self.__cur_camp + 1))
    for i,sp_data in ipairs(self.__btn_list) do
        sp_data.btn:SetNormalImage(index == i and self.__btn_res[1] or self.__btn_res[2])
    end 
	--容器显隐
	for i,panel in ipairs(self.__left_panel_list) do
		panel:SetVisible(i == index)
	end
	--创建滚动容器
	if not self["__rank_scrollview"..index] then
		if index <= 3 then
			CallModuleFunc(ModuleType.RANK,"RankListRequest",{type = self.__left_rank_type[index]})
		else
			self:CreateScrollView(index)
		end		
	end
end

function LogicGUI.TemplIntegralRaceRankView:CreateScrollView(index,info)
	self.__scrollview_list = self.__scrollview_list or {}
	local cur_panel = index <= 3 and self.__left_panel_list[index] or self.__panel_encouragescrollview
	local params = {
		item_class = index <= 3 and Modules.TemplIntegralRaceRankCell or Modules.TemplIntegralRaceRankSpCell,
	    item_width = 582,
	    item_height	= index <= 3 and 122 or 88,
	    row	= 4,
	    col	= 1,
	    item_space_r = index <= 3 and 8 or 0,
	    view_width = 582,
	    view_height = index <= 3 and 400 or 357,}
	self["__rank_scrollview"..index] = GUI.ScrollView.New(params)
	cur_panel:AddChild(self["__rank_scrollview"..index]:GetNode())
	PosTool.Center(self["__rank_scrollview"..index])
	table.insert(self.__scrollview_list,self["__rank_scrollview"..index])
	--检测索引
	if index <= 3 then		
		for i,cur_info in ipairs(info.rank_list) do
			cur_info.rank = i
			cur_info.rank_type = info.type
		end		
		self["__rank_scrollview"..index]:SetDataList(info.rank_list)
		if self.__cur_camp == (index - 1) then
			--排行信息
			self.__txt_myranknum:SetText(info.rank == 0 and CPPGameLib.GetString("rank_reflection") or info.rank)
		end
	else
		local cur_big_data = {}
		for sort_id,config_info in pairs(CPPGameLib.GetConfig("ScoreMatchRankReward",nil,nil,true)) do
			local data = CPPGameLib.CopyTblShallow(config_info)
			data.sort_id = sort_id
			table.insert(cur_big_data,data)
		end
		table.sort(cur_big_data,function (data1,data2)
			return data1.sort_id < data2.sort_id
		end)
		for i,v in ipairs(cur_big_data) do
			v.index = i
		end
		self["__rank_scrollview"..index]:SetDataList(cur_big_data)
	end
end


Modules = Modules or {}
Modules.TemplIntegralRaceRankSpCell = Modules.TemplIntegralRaceRankSpCell or BaseClass(GUI.ScrollItem)

function Modules.TemplIntegralRaceRankSpCell:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_INTEGRALRACE_RANK_SP_CELL) 
end

function Modules.TemplIntegralRaceRankSpCell:__delete()
end

function Modules.TemplIntegralRaceRankSpCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--背景
	self:GetWidget("Image_bg"):SetVisible(self.__data.index%2 == 1)
	--排名数字
	self.__txt_rank = self:GetWidget("Text_rank")
	self.__txt_rank:SetVisible(self.__data.rank_min > 3)
	Util:WidgetLabel(self.__txt_rank,"", Macros.TypefaceSize.tab, Macros.Color.keypoint)
	--排名图片
	self.__img_rank = self:GetWidget("Image_rank")
	self.__img_rank:SetVisible(self.__data.rank_min <= 3)
	--排名
	if self.__data.rank_min <=3 then 
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..self.__data.rank_min)
		self.__img_rank:SetImage(imageData)
	else
		self.__txt_rank:SetText(self.__data.rank_min ~= self.__data.rank_max and CPPGameLib.GetString("integral_rank_showrank",self.__data.rank_min,self.__data.rank_max) or self.__data.rank_max)
	end
	--容器
	local panel_scrollview = self:GetWidget("Panel_ScrollView")
	local interval = 0
	for i,reward_item_id in ipairs(self.__data.reward_item_id_list) do
		local icon = LogicGUI.IconLayer.New()
		icon:SetAnchorPoint(0,0.5)
		panel_scrollview:AddChild(icon:GetNode())
		PosTool.LeftCenter(icon,interval,0)
		icon:SetData({id = reward_item_id,num = self.__data.reward_item_num_list[i] or 0})
		interval = interval + 13 + icon:GetWidth()
	end
end