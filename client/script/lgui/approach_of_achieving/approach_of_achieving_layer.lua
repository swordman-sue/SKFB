--
-- @brief 获取途径
-- @author: ldx
-- @date: 2016年10月31日15:17:54
--
LogicGUI = LogicGUI or {}

LogicGUI.ApproachOfAchievingLayer = LogicGUI.ApproachOfAchievingLayer or BaseClass(GUI.Template)

function LogicGUI.ApproachOfAchievingLayer:__init(info_id,is_stronger,func)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self.func = func
		self:createMainUI(info_id,is_stronger)
		self:FireNextFrame(Macros.Event.ApproachOfAchievingModule.OPEN_APPROACHVIEW, info_id)		
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGCELL)
	self:InitTemplate(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW)
end

function LogicGUI.ApproachOfAchievingLayer:createMainUI(info_id,is_stronger)
	--获取物品类型和信息表
	local item_type,item_info = ObjIDTool.GetInfo(info_id)	
	--是否是寻找更强的装备	
	if is_stronger and item_type == Macros.Game.ObjType.EQUIP then
		info_id = item_info.equipid[1] and item_info.equipid[1] or info_id
		item_type,item_info = ObjIDTool.GetInfo(info_id)	
	end
	local function closeView()
		self:FireNextFrame(Macros.Event.HeroModule.HERO_HAND_BOOK_ADJUST_OPACITY)
		if self.func then
			self.func()
		end
		self:DeleteMe()
	end
	--关闭界面事件
	self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
    		if self.func then
				self.func()
			end
	        self:DeleteMe()
	    end)
	local buttonClose = self:GetWidget("Button_close")
	WidgetUnity.SetWidgetListener(buttonClose, nil, nil, closeView, nil)	
	--途径列表
	local approach_list = {}
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"), CPPGameLib.GetString("approach_titleText"), Macros.TypefaceSize.popup, Macros.Color.btn)
	--物品头像
	local IconPanel = self:GetWidget("Panel_ItemIcon")
	local icon = LogicGUI.IconLayer.New(false,false)
	IconPanel:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = info_id})
	--物品名字
	local NamePanel = self:GetWidget("Panel_name")
	local labelName = nil
	if item_type == Macros.Game.ObjType.HERO then
		labelName = Util:Name(item_info.name,20,item_info.init_quality)
	else
		labelName = Util:Name(item_info.name,20,item_info.quality)
	end
	labelName:SetAnchorPoint(0,0.5)
	NamePanel:AddChild(labelName)
	PosTool.LeftCenter(labelName)
	--拥有数量
	Util:WidgetLabel(self:GetWidget("Text_have"), CPPGameLib.GetString("approach_have"), Macros.TypefaceSize.normal, Macros.Color.keypoint )
	local HaveItemNum
	if item_type == Macros.Game.ObjType.ITEM and item_info.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
        HaveItemNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , item_info.param1 )
    else
    	HaveItemNum = ObjIDTool.GetOwnNum(info_id)
    end
	Util:WidgetLabel(self:GetWidget("Text_haveNum"), HaveItemNum, Macros.TypefaceSize.normal, Macros.Color.content)
	--获取处理后的途径数据
	self.__approach_of_achieving_list = CallModuleFunc(ModuleType.APPROACH_OF_ACHIEVING,"HandleApproachOfAchievingData",info_id)
	--CPPGameLib.PrintTable("ldx",self.__approach_of_achieving_list,"self.__approach_of_achieving_list")
	--描述
	local item_des = Util:RichText(item_info.description or item_info.describe or item_info.des or "", Macros.TypefaceSize.normal, 423, 45, Macros.Color.content_hex )
	item_des:SetAnchorPoint(0,0.5)
	self:GetWidget("Panel_main"):AddChild(item_des)
	item_des:SetPosition(118,342)
	--创建滚动容器
	self:CreateScrollView()
end

function LogicGUI.ApproachOfAchievingLayer:CreateScrollView()
	local scrollview_panel = self:GetWidget("Panel_scrollview")
	local params = {
		item_class	= Modules.TemplApproachOfAchievingCell,
	    item_width	= 530,
	    item_height	= 85,
	    row	= 3,
	    col	= 1,
	    item_space_r = 5,
	    view_width = 530,
	    view_height = 258,
	}
	self.__scrollview = GUI.ScrollView.New(params)
	scrollview_panel:AddChild(self.__scrollview:GetNode())
	PosTool.Center(self.__scrollview)	
	self.__scrollview:SetDataList(self.__approach_of_achieving_list)
	if #self.__approach_of_achieving_list == 0 then
		local label = Util:Label(CPPGameLib.GetString("approach_title_tips"), Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
		scrollview_panel:AddChild(label)
		PosTool.Center(label)
	end
end

function LogicGUI.ApproachOfAchievingLayer:__delete()
	if self.__scrollview then
		self.__scrollview:DeleteMe()
		self.__scrollview = nil
	end
end

