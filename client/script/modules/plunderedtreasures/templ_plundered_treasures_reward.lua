--
-- @brief 进度奖励
-- @author: ldx
-- @date: 2018年5月4日11:05:29
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresReward = LogicGUI.TemplPlunderedTreasuresReward or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresReward:__init()
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.REBEL_AWARD)	
end

function LogicGUI.TemplPlunderedTreasuresReward:__RegistAllEvents()
	--奖励记录
	self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_PROGRESS_REWARD,function ()
		print("ldx","!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 ")
		self:CreateProgressList()
	end)
end

function LogicGUI.TemplPlunderedTreasuresReward:__delete()
	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end
end

function LogicGUI.TemplPlunderedTreasuresReward:__Getwidget()
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
											nil,nil,Macros.Color.button_red
										)
	--提示
	self.lab_tips = self:GetWidget("lab_tips")
end	

function LogicGUI.TemplPlunderedTreasuresReward:__ShowUI()
	--标题
	local tips = Util:Label(CPPGameLib.GetString("plunderedtreasures_progress_reward_title"),Macros.TypefaceSize.popup,Macros.Color.btn)
    tips:SetAnchorPoint(0.5,0.5)
    tips:SetPosition(self.img_tou:GetContentWidth()/2,self.img_tou:GetContentHeight()/2)
    self.img_tou:AddChild(tips)
    --进度提示
	tips = Util:Label(CPPGameLib.GetString("plunderedtreasures_progress_reward_tips") , Macros.TypefaceSize.slightly , Macros.Color.keypoint,0)
    tips:SetAnchorPoint(0 , 0.5)
    tips:SetPosition(self.lab_tips:GetContentWidth()/2,self.lab_tips:GetContentHeight()/2)
    self.lab_tips:AddChild(tips)
    
	local params =  
	{
		item_class = Modules.TemplPlunderedTreasuresRewardItem,
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
	self:CreateProgressList()
end

--创建scrollview
function LogicGUI.TemplPlunderedTreasuresReward:CreateProgressList()
	--显示奖励队列
	self.__progress_reward_list,get_reward = CallModuleFunc(ModuleType.PLUNDEREDTREASURES , "GetProgressRewardList")
	if get_reward then		
		self.btn_allGet:SetEnabled(true)
		local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.btn_allGet:SetOutline(r, g, b, a, 2)
	else
		Util:GreyButton( self.btn_allGet )
	end
	self.lst_list:SetDataList(self.__progress_reward_list)
end

--一键领奖
function LogicGUI.TemplPlunderedTreasuresReward:OnBtnAllGet( ... )
	local pregross_id_list = {}
	for _,progress_info in ipairs(self.__progress_reward_list) do
		if progress_info.reward_status == 2 then
			table.insert(pregross_id_list,{id = progress_info.progress_id})
		end
	end
	if #pregross_id_list > 0 then
		CallModuleFunc(ModuleType.PLUNDEREDTREASURES , "TakeDuoBaoPregressRewardReq",pregross_id_list)
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rebel_noneCanGet"))
	end
end

--关闭按钮
function LogicGUI.TemplPlunderedTreasuresReward:OnBtnExit( ... )
	self:DeleteMe()
end


Modules = Modules or {}
Modules.TemplPlunderedTreasuresRewardItem = Modules.TemplPlunderedTreasuresRewardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPlunderedTreasuresRewardItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM) 
    self:__Getwidget()
end

function Modules.TemplPlunderedTreasuresRewardItem:__delete()
end

function Modules.TemplPlunderedTreasuresRewardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	
	data.type =  math.floor(data.reward_item_id_list[1] / 10000)
	self:__ShowUI(data)
end

function Modules.TemplPlunderedTreasuresRewardItem:__AddToParent()
end

function Modules.TemplPlunderedTreasuresRewardItem:__Getwidget( ... )
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

function Modules.TemplPlunderedTreasuresRewardItem:__ShowUI( data )
	local type,info = ObjIDTool.GetInfo(self.__data.reward_item_id_list[1])
	local numText = " "
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

		-- info.name = CPPGameLib.GetString("rebel_gold")

	    --图标数量
	    local icon_num = Util:Label(self.__data.reward_item_num_list[1],Macros.TypefaceSize.slightly,Macros.Color.white)
	    icon_num:SetAnchorPoint(1,0)
	    icon_num:SetPosition(icon_layer:GetContentWidth()- 5 ,2.5)
	    icon_layer:AddChild(icon_num)
	else
		--图标
		icon_layer = LogicGUI.IconLayer.New(true)
		icon_layer:SetData({id = self.__data.reward_item_id_list[1] , num = self.__data.reward_item_num_list[1]})
		self.Panel_icon:AddChild(icon_layer:GetNode())
		PosTool.Center(icon_layer)   
	 --    if data.award_item_num > 1 then
		--     --图标数量
		--     local icon_num = Util:Label(data.award_item_num,Macros.TypefaceSize.slightly,Macros.Color.white)
		--     icon_num:SetAnchorPoint(1,0)
		--     icon_num:SetPosition(icon_layer:GetNode():GetContentWidth()- 5 ,2.5)
		--     icon_layer:GetNode():AddChild(icon_num)
		-- end
	end

    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

   	self.Button_click:SetEnabled(data.type)

	--提示
	local tips = Util:Label(CPPGameLib.GetString("plunderedtreasures_progress_tips",data.times) , 
Macros.TypefaceSize.normal , Macros.Color.content ) 
    tips:SetAnchorPoint(0,0.5)
    tips:SetPosition(10,50.5)
    self.Image_content:AddChild(tips)

	--进度
	local schedule = Util:Label( CPPGameLib.GetString("rebel_when"), Macros.TypefaceSize.normal , Macros.Color.keypoint )
    schedule:SetAnchorPoint(0,0.5)
    schedule:SetPosition(10,20.5)
    self.Image_content:AddChild(schedule)   

    local schedule1 = Util:Label( string.format("%d/%d",self.__data.cur_progress,data.times)  , Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(schedule , schedule1)
    self.Image_content:AddChild(schedule1)  

	if self.__data.reward_status <= 0 then
		self.Button_click:SetVisible(false)
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
		self.receive_icon = Util:Sprite(sp)
		self.receive_icon:SetPosition(self.Button_click:GetPositionX() , self.Button_click:GetPositionY() )
		self.Image_bg:AddChild(self.receive_icon)
	elseif self.__data.reward_status == 1 then
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
end

function Modules.TemplPlunderedTreasuresRewardItem:OnButtonClick()
	local pregross_id_list = {{id = self.__data.progress_id}}
	CallModuleFunc(ModuleType.PLUNDEREDTREASURES , "TakeDuoBaoPregressRewardReq",pregross_id_list)
end


