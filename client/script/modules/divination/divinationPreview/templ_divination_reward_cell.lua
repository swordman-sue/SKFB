--[[
%% @module: 占卜抽卡界面预览
%% @author: yjg
%% @created: 2017年12月4日11:15:02
--]]

GUI = GUI or {}
GUI.TemplDivinationRewardCell = GUI.TemplDivinationRewardCell or BaseClass(GUI.Template)

function GUI.TemplDivinationRewardCell:__init( lst )
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TEAM_LOTTERY_REWARD_CELL)
	self:CreateMainUI(lst)
end

function GUI.TemplDivinationRewardCell:__AddToParent()

end

function GUI.TemplDivinationRewardCell:CreateMainUI(lst)	
	if not lst then
		return
	end

	CPPGameLib.PrintTable("xy" , lst , "请求物品列表")  
	--奖励类型
	Util:WidgetLabel(self:GetWidget("Text_stage"), lst.title ,Macros.TypefaceSize.tab,Macros.Color.headline )
	--计算行数
	local rowNum = 0
	local type_num = 0
	self.__data_list = {}


	for i , v in ipairs( lst.card_list ) do
		type_num = type_num + 1
		table.insert(self.__data_list,{ item_id = v })
	end

	rowNum = math.ceil(type_num/5)
	local scrollviewparams = {
		item_class = Modules.IconItem,
    	item_width = 80,
    	item_height	= 80,
    	item_space_c = 44,
    	item_space_r = 10,
    	row	= rowNum,
    	col	= 5,
	}

	--顶部控件
	local panel_top = self:GetWidget("Panel_top")
	--背景图
	local image_bg = self:GetWidget("Image_bg")
	--滚动容器
	local panel_scrollview = self:GetWidget("Panel_ScrollView")
	self.__reward_scrollview = GUI.ScrollView.New(scrollviewparams)					
	panel_scrollview:AddChild(self.__reward_scrollview:GetNode())			
	self.__reward_scrollview:SetDataList(self.__data_list)
	--根据高度适配
	local height = self.__reward_scrollview:GetViewHeight()
	panel_scrollview:SetContentSize(576,height)	
	PosTool.Center(self.__reward_scrollview)	
	--根节点设置大小
	self:GetNode():SetContentSize(626,height+82)
	--背景图设置大小
	image_bg:SetContentSize(603,height+20)
	PosTool.CenterTop(image_bg,0,-51)
	--设置位置
	PosTool.LeftTop(panel_scrollview,25,-61)
	PosTool.CenterTop(panel_top)	
end

function GUI.TemplDivinationRewardCell:__delete()
	if self.__reward_scrollview then
		self.__reward_scrollview:DeleteMe()
		self.__reward_scrollview = nil
	end
end
