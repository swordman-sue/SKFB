LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresDetailMessageView  = LogicGUI.TemplPlunderedTreasuresDetailMessageView or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresDetailMessageView:__init(info_id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initUI(info_id)
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROCHIPVIEW)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGCELL)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_TREASUREMESSAGE)
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_DETAILMESSAGE)	
end

function LogicGUI.TemplPlunderedTreasuresDetailMessageView:initUI(info_id)
	--关闭界面事件
	self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	        self:DeleteMe()
	    end)

	--关闭
	Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
		self:DeleteMe()
	end)
	local pieceId = nil
	local treasureId = nil
	local type,config = ObjIDTool.GetInfo(info_id)	
	if type == Macros.Game.ObjType.ITEM then
		pieceId = info_id
		treasureId = config.target_id
	elseif type == Macros.Game.ObjType.TREASURE then
		local composeInfo = CPPGameLib.GetConfig("TreasureCompose",info_id)
		pieceId = composeInfo.cost_item_list[1]
		treasureId = info_id
	else
		return
	end
	--翻页容器
	self.PageView_treasureDetailMessage = self:GetWidget("PageView_treasureDetailMessage")	
	--碎片信息
	self.ChipView = GUI.TemplPlunderedTreasureChipView.New(pieceId)
	self.PageView_treasureDetailMessage:AddPage(self.ChipView:GetNode())	
	--装备信息
	self.MessageView = GUI.TemplPlunderedTreasuresDetailMessageView.New(treasureId)
	self.PageView_treasureDetailMessage:AddPage(self.MessageView:GetNode())
	--设置点击监听
	self:SetPanelListener()
	--翻页监听	
	local function pageViewEvent()
        local index = self.PageView_treasureDetailMessage:GetCurPageIndex()
        self:ChangePage(index+1)      
    end
	WidgetUnity.SetPageViewListener(self.PageView_treasureDetailMessage,pageViewEvent)
	--翻页到
	self.PageView_treasureDetailMessage:ForceDoLayout()
	if type == Macros.Game.ObjType.ITEM then
		self:ChangePage(1,true)
	else
		self:ChangePage(2,true)
	end	

    -- --左箭头
    self.Image_left = self:GetWidget("Image_left")
    -- --右箭头
    self.Image_right = self:GetWidget("Image_right")

    local effect = nil
    self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_left , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_left:GetContentWidth()/2 , self.Image_left:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )

    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_right , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_right:GetContentWidth()/2 , self.Image_right:GetContentHeight()/2 )
end

--设置监听绑定
function LogicGUI.TemplPlunderedTreasuresDetailMessageView:SetPanelListener()
	for i=1,2 do
		local panel = self:GetWidget("Panel_messageType_"..i)
		--初始化切换标签文字
		local tagName = WidgetUnity.GetWidgetByName(panel,"Text_2")  
		Util:WidgetLabel(tagName,CPPGameLib.GetString("plunderedtreasures_treasure_messageType_"..i), Macros.TypefaceSize.normal,Macros.Color.content)
		WidgetUnity.SetWidgetListener(panel,nil,nil,function ()
			self:ChangePage(i,true)
		end,nil)
	end
end

--换页
function LogicGUI.TemplPlunderedTreasuresDetailMessageView:ChangePage(messageType,IsNeedTurnPage)
	if self.curPageType ~= messageType then
		self.curPageType = messageType
	else
		return
	end
	for i=1,2 do 
		local panel = self:GetWidget("Panel_messageType_"..i)
		--选中状态
		local tagButton = WidgetUnity.GetWidgetByName(panel,"Image_4")  
		--文字
		local tagName = WidgetUnity.GetWidgetByName(panel,"Text_2")  
		if i == messageType then
			tagButton:SetVisible(true)
			tagName:SetColor(unpack(Macros.Color.headline))
		else
			tagButton:SetVisible(false)
			tagName:SetColor(unpack(Macros.Color.content))			
		end				
	end	
	if IsNeedTurnPage then
		self.PageView_treasureDetailMessage:ScrollToPage(messageType-1)
	end	
end

function LogicGUI.TemplPlunderedTreasuresDetailMessageView:__delete()
	if self.ChipView then
		self.ChipView:DeleteMe()
		self.ChipView = nil
	end
	if self.MessageView then
		self.MessageView:DeleteMe()
		self.MessageView = nil
	end
end