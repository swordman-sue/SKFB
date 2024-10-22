--
-- @brief 叛军奖励
-- @author: yjg
-- @date: 2016年9月19日18:26:29
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplRebelAward = LogicGUI.TemplRebelAward or BaseClass(GUI.Template)

function LogicGUI.TemplRebelAward:__init( fun )
	self.fun = fun
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
		self:UpItem(CallModuleFunc(ModuleType.REBEL , "GetAwardRecord"))
	end
	
	--奖励记录
	self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_AWARD_RECORD,function (_, data)
		self:UpItem(CallModuleFunc(ModuleType.REBEL , "GetAwardRecord"))
	end)

	--叛軍信息
	self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_DATA_RESP,function (_, data)
		self.data = CallModuleFunc(ModuleType.REBEL , "GetRankDrawType")
		self.lst_list:SetDataList(self.data)
	end)
	

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.REBEL_AWARD)	
end

function LogicGUI.TemplRebelAward:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end

	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end
end

function LogicGUI.TemplRebelAward:__Getwidget()
	--背景
	self.img_bg = self:GetWidget("img_bg")
	--关闭界面
	self.btn_exit = Util:WidgetButton( self:GetWidget("btn_exit") ,nil, CPPGameLib.Handler( self,self.OnBtnExit ) )
	--标题
	self.img_tou = self:GetWidget("img_tou")
	--属性背景
	self.img_lst = self:GetWidget("img_lst")
	--一键领奖
	self.btn_allGet = Util:WidgetButton( 
											self:GetWidget("btn_allGet") , 
											CPPGameLib.GetString("rebel_all_get") , 
											CPPGameLib.Handler( self,self.OnBtnAllGet ) ,
											nil , nil , Macros.Color.button_red
										)
	--提示
	self.lab_tips = self:GetWidget("lab_tips")
end	

function LogicGUI.TemplRebelAward:__ShowUI()
	local tips = Util:Label(CPPGameLib.GetString("rebel_work_award"),Macros.TypefaceSize.popup)
    tips:SetAnchorPoint(0.5,0.5)
    tips:SetPosition(self.img_tou:GetContentWidth()/2,self.img_tou:GetContentHeight()/2)
    self.img_tou:AddChild(tips)

	local tips = Util:Label(CPPGameLib.GetString("rebel_rank_award") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    tips:SetAnchorPoint(0 , 0.5)
    tips:SetPosition(self.lab_tips:GetContentWidth()/2,self.lab_tips:GetContentHeight()/2)
    self.lab_tips:AddChild(tips)
    
	local params =  
	{
		item_class = Modules.TemplRebelAwardItem,
		item_width = self.img_lst:GetContentWidth() - 5,
		item_height = 125.00,
		row = 5,
		col = 1,
		view_width = self.img_lst:GetContentWidth() - 5 ,
		view_height = self.img_lst:GetContentHeight() - 5,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.img_lst:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

	self.data = CallModuleFunc(ModuleType.REBEL , "GetRankDrawType")
	self.lst_list:SetDataList(self.data)
end

function LogicGUI.TemplRebelAward:UpItem( info )

	print("xy" , " LogicGUI.TemplRebelAward:UpItem( info )" )

	self.data = CallModuleFunc(ModuleType.REBEL , "GetRankDrawType")
	self.lst_list:SetDataList(self.data)
	local awardNum = CallModuleFunc(ModuleType.REBEL , "GetNoeAwardRed")
	if awardNum == 0 then
		self.btn_allGet:SetEnabled(false)
		Util:GreyButton( self.btn_allGet )
	end
end

--一键领奖
function LogicGUI.TemplRebelAward:OnBtnAllGet( ... )

	local sum = CallModuleFunc(ModuleType.REBEL , "RebelSum")
	local info = {}
	info.award_list = {}

	local awardRecord = CallModuleFunc(ModuleType.REBEL , "GetAwardRecord")
	for i,v in ipairs(self.data) do
	    if sum.today_exploit > v.need_exploit and not awardRecord[v.award_id] then
	    	table.insert(info.award_list,{award_id = v.award_id})
	    end
	end

	if #info.award_list ~= 0 then
		CallModuleFunc(ModuleType.REBEL , "RebelExploitAwardReq",info)
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rebel_noneCanGet"))
	end
end

--关闭按钮
function LogicGUI.TemplRebelAward:OnBtnExit( ... )
	if self.fun then
		self.fun()
	end
	self:DeleteMe()
end


Modules = Modules or {}
Modules.TemplRebelAwardItem = Modules.TemplRebelAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRebelAwardItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM) 
    self:__Getwidget()
end

function Modules.TemplRebelAwardItem:__delete()
end

function Modules.TemplRebelAwardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	
	data.type =  math.floor(data.award_item_id / 10000)
	self:__ShowUI(data)
end

function Modules.TemplRebelAwardItem:__AddToParent()
end

function Modules.TemplRebelAwardItem:__Getwidget( ... )
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--内容背景
	self.Image_content = self:GetWidget("Image_content")
	--名称
	self.Text_name = self:GetWidget("Text_name")
	--按钮
	self.Button_click = Util:WidgetButton( self:GetWidget("Button_click") , nil , CPPGameLib.Handler(self,self.OnButtonClick))
end

function Modules.TemplRebelAwardItem:__ShowUI( data )
	local info = {}
	local numText = " "
	if data.type == Macros.Game.ObjType.ITEM then
		info = CPPGameLib.GetConfig("Item", data.award_item_id )

	elseif data.type == Macros.Game.ObjType.EQUIP then
		info = CPPGameLib.GetConfig("Equipment", data.award_item_id )

	elseif data.type == Macros.Game.ObjType.HERO then
		info = CPPGameLib.GetConfig("Hero", data.award_item_id )

	elseif data.type == Macros.Game.ObjType.TREASURE then
		info = CPPGameLib.GetConfig("Treasure", data.award_item_id )
	end

	local icon_layer = nil

	if data.type == 0 then
	    --价格图标
		local img_diamond = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_5")
		local icon_layer = Scale9SpriteEx.CreateWithImageFile(img_diamond)
		icon_layer:SetAnchorPoint(0,0.5)
		icon_layer:SetContentSize(88,88)
		-- icon_layer:SetScaleFactor(0.85)
		self.Panel_icon:AddChild(icon_layer)	
		PosTool.Center(icon_layer) 	

		info.name = CPPGameLib.GetString("rebel_gold")

	    --图标数量
	    local icon_num = Util:Label(data.award_item_num,Macros.TypefaceSize.slightly,Macros.Color.white)
	    icon_num:SetAnchorPoint(1,0)
	    icon_num:SetPosition(icon_layer:GetContentWidth()- 5 ,2.5)
	    icon_layer:AddChild(icon_num)
	else
		--图标
		icon_layer = LogicGUI.IconLayer.New(true)
		icon_layer:SetData({id = data.award_item_id , num = data.award_item_num})
		self.Panel_icon:AddChild(icon_layer:GetNode())
		PosTool.Center(icon_layer)   
	    if data.award_item_num > 1 then
		    --图标数量
		    local icon_num = Util:Label(data.award_item_num,Macros.TypefaceSize.slightly,Macros.Color.white)
		    icon_num:SetAnchorPoint(1,0)
		    icon_num:SetPosition(icon_layer:GetNode():GetContentWidth()- 5 ,2.5)
		    icon_layer:GetNode():AddChild(icon_num)
		end
	end

	local type_ , cnf = ObjIDTool.GetInfo(data.award_item_id)

    --名字
    local name = Util:Name(cnf.name , Macros.TypefaceSize.normal , cnf.quality)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

   	self.Button_click:SetEnabled(data.type)

	--玩家信息
	local sum = CallModuleFunc(ModuleType.REBEL , "RebelSum")

	--提示
	local tips = Util:Label(CPPGameLib.GetString("rebel_getOk",data.need_exploit) , Macros.TypefaceSize.normal , Macros.Color.content ) 
    tips:SetAnchorPoint(0,0.5)
    tips:SetPosition(10,50.5)
    self.Image_content:AddChild(tips)

	--进度
	local schedule = Util:Label( CPPGameLib.GetString("rebel_when"), Macros.TypefaceSize.normal , Macros.Color.keypoint )
    schedule:SetAnchorPoint(0,0.5)
    schedule:SetPosition(10,20.5)
    self.Image_content:AddChild(schedule)   

    local schedule1 = Util:Label( string.format("%d/%d", sum.today_exploit ,data.need_exploit )  , Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(schedule , schedule1)
    self.Image_content:AddChild(schedule1)   

    --不能领
    if sum.today_exploit < data.need_exploit then
    	self.Button_click:SetEnabled(false)
		self.Button_click:SetText( CPPGameLib.GetString("rebel_noMeet") )
		self.Button_click:SetTextColor(unpack(Macros.Color.forbidden))
		local r, g, b, a = unpack( Macros.Color.forbidden_stroke )
		self.Button_click:SetOutline(r, g, b, a, 2)
	else
    	self.Button_click:SetEnabled(true)
		self.Button_click:SetText( CPPGameLib.GetString("manor_get") )
		self.Button_click:SetTextColor(unpack(Macros.Color.btn))
		local r, g, b, a = unpack( Macros.Color.button_red )
		self.Button_click:SetOutline(r, g, b, a, 2)
    end


    local award = CallModuleFunc(ModuleType.REBEL , "GetAwardRecord")
	if award[data.award_id] then
		self.Button_click:SetVisible(false)

		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
		self.receive_icon = Util:Sprite(sp)
		self.receive_icon:SetPosition(self.Button_click:GetPositionX() , self.Button_click:GetPositionY() )
		self.Image_bg:AddChild(self.receive_icon)
	end  
end

function Modules.TemplRebelAwardItem:OnButtonClick()
	local info = {}
	info.award_list = {}
	info.award_list[1] = {}
	info.award_list[1].award_id = self.__data.award_id
	CallModuleFunc(ModuleType.REBEL , "RebelExploitAwardReq",info)
end


