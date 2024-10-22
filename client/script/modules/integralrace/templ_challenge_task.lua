
LogicGUI = LogicGUI or {}

LogicGUI.TemplChallengeTask = LogicGUI.TemplChallengeTask or BaseClass(GUI.Template)

function LogicGUI.TemplChallengeTask:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
    self.__tab_params = {
    	{sp_2,sp_1},
    	{
			CPPGameLib.GetString("integral_race_daliy_task_txt"),
		},
		10,CPPGameLib.Handler(self, self.OnYeMei)
	}
	
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD)	
end

function LogicGUI.TemplChallengeTask:__delete()
	if self.__daliy_task_list then
		self.__daliy_task_list:DeleteMe()
		self.__daliy_task_list = nil 
	end
end

function LogicGUI.TemplChallengeTask:__RegistAllEvents()
    --领取挑战任务奖励响应
    self:BindGlobalEvent(Macros.Event.IntegralModule.SCORE_MATCH_TAKE_TASKREWARD_RESP, function()
        self:CreateDaliyTaskList()
    end)
end

function LogicGUI.TemplChallengeTask:__Getwidget()
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭
    self.__btn_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --内容背景
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
    --获取按钮
    self.__btn_get = Util:WidgetButton( 
    										self:GetWidget("Button_get") , 
    										CPPGameLib.GetString("friend_Button_oneUseGet") , 
    										CPPGameLib.Handler(self,self.OnButtonGet) ,
    										Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow
    									 )
end

function LogicGUI.TemplChallengeTask:__ShowUI()
	--奖励
	local award = Util:Label( CPPGameLib.GetString("integral_race_challenge_task_txt") , Macros.TypefaceSize.largish, Macros.Color.btn)
	award:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	award:SetAnchorPoint(0.5,0.5)
	self.Image_light:AddChild(award)

    --条目
    self.params =  
    {
        item_class = Modules.TemplChallengeTaskItem,
        item_width = 598.00,
        item_height = 125.00,
        row = 3,
        col = 1,
        item_space_r = 3 , 
        view_width = self.Image_lstBg:GetContentWidth() ,
        view_height = self.Image_lstBg:GetContentHeight() - 5 ,
    }

	--页签
    self.__tab:On(1)
    self.__tab:SetPosition( 155 , 470 )
    self.Image_bg:AddChild(self.__tab:GetNode())
end

function LogicGUI.TemplChallengeTask:OnButtonClose()
	self:DeleteMe()
end

--页签回调
function LogicGUI.TemplChallengeTask:OnYeMei(index)
	self.__selected_index = index
	if self.__daliy_task_list then
	    self.__daliy_task_list:SetVisible(index == 1)
	end

	if index == 1 and not self.__daliy_task_list then
		self:CreateDaliyTaskList()
	end
end

--创建荣誉表
function LogicGUI.TemplChallengeTask:CreateDaliyTaskList()
	if not self.__daliy_task_list then
		self.__daliy_task_list = GUI.ScrollView.New(self.params)
		self.Image_lstBg:AddChild(self.__daliy_task_list:GetNode())
		PosTool.Center(self.__daliy_task_list:GetNode())
	end
	self.__score_match_task_list = CallModuleFunc(ModuleType.INTEGRAL_RACE,"GetClearUpTaskData")
	self.__daliy_task_list:SetDataList(self.__score_match_task_list)

	local is_gray = true
	for _,score_info in ipairs(self.__score_match_task_list) do
		if score_info.have_battle_times >= score_info.battle_times and not score_info.is_already_get then
			is_gray = false
			break
		end
	end
	--判断一键领取是否灰色
	self.__btn_get:SetEnabled(not is_gray)
	if not is_gray then		
    	local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.__btn_get:SetOutline(r, g, b, a, 2)
	else
		Util:GreyButton(self.__btn_get)
	end
end

--创建击杀表
function LogicGUI.TemplChallengeTask:CreateWeeklyTaskList()
	if not self.__weekly_task_list then
	    self.__weekly_task_list = GUI.ScrollView.New(self.params)
	    self.Image_lstBg:AddChild(self.__weekly_task_list:GetNode())
	    PosTool.Center(self.__weekly_task_list:GetNode())
	end
end

function LogicGUI.TemplChallengeTask:OnButtonGet()
	local req_info = {}
	for _,score_info in ipairs(self.__score_match_task_list) do
		if score_info.have_battle_times >= score_info.battle_times and not score_info.is_already_get then
			table.insert(req_info,{task_id = score_info.id})
		end
	end
	self.__btn_get:SetEnabled(false)
	CallModuleFunc(ModuleType.INTEGRAL_RACE,"ScoreMatchTakeTaskRewardReq",req_info)
end


Modules = Modules or {}
Modules.TemplChallengeTaskItem = Modules.TemplChallengeTaskItem or BaseClass(GUI.ScrollItem)

function Modules.TemplChallengeTaskItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM) 
    self:__Getwidget()
end

function Modules.TemplChallengeTaskItem:__delete()
end

function Modules.TemplChallengeTaskItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	self:__ShowUI(data)
end

function Modules.TemplChallengeTaskItem:__AddToParent()
end

function Modules.TemplChallengeTaskItem:__Getwidget( ... )
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--内容背景
	self.Image_content = self:GetWidget("Image_content")
	--名称
	self.Text_name = self:GetWidget("Text_name")
	--按钮
	self.__btn_click = Util:WidgetButton( 
											self:GetWidget("Button_click") , 
											CPPGameLib.GetString("worldBoss_receive") , 
											CPPGameLib.Handler(self,self.OnButtonClick) , 
											Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow
										)
	self.__btn_click:SetZOrder(10)
	self.__btn_click:SetVisible(false)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
	self.receive_icon = Util:Sprite(sp)
	self.receive_icon:SetAnchorPoint(0.5,0.5)
	self.receive_icon:SetPosition( self.__btn_click:GetPositionX() , self.__btn_click:GetPositionY() )
	self.Image_bg:AddChild( self.receive_icon )
	self.receive_icon:SetVisible(false)

	-- local effect
	-- self.effect_id,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.__btn_click)
	-- effect:SetScaleFactor( 1.2 )
	-- PosTool.Center(effect)
end

function Modules.TemplChallengeTaskItem:__ShowUI( data )
	--图标
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = data.item_id_list[1] , num = data.item_num_list[1]})
    self.Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)     

    local type_ , cnf = ObjIDTool.GetInfo(data.item_id_list[1])

    --名字
    local name = Util:Name(cnf.name,nil,cnf.quality)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

    --文本
    local dice_label = Util:Label(CPPGameLib.GetString("integral_task_win",data.battle_times), Macros.TypefaceSize.normal , Macros.Color.keypoint, nil, self.Image_content:GetContentWidth() - 10)
    dice_label:SetPosition( 10 , self.Image_content:GetContentHeight() - 10 )
    dice_label:SetAnchorPoint(0,1)
    self.Image_content:AddChild(dice_label)

    --进度
   	local worldBoss_schedule = Util:Label(CPPGameLib.GetString("worldBoss_schedule"),Macros.TypefaceSize.normal , Macros.Color.keypoint )
   	worldBoss_schedule:SetAnchorPoint(0,0.5)
   	worldBoss_schedule:SetPosition( 10 , worldBoss_schedule:GetContentHeight() )
   	self.Image_content:AddChild(worldBoss_schedule)

   	local my_Value = Util:Label(CPPGameLib.GetString("integral_schedule_sp_txt",data.have_battle_times,data.battle_times),Macros.TypefaceSize.normal , Macros.Color.content )
   	self.Image_content:AddChild(my_Value)
   	PosTool.RightTo(worldBoss_schedule , my_Value)

   	if data.have_battle_times < data.battle_times then
   		Util:GreyButton(self.__btn_click)
   		self.__btn_click:SetEnabled(false)
   	end  	
   	self.__btn_click:SetVisible(not data.is_already_get)
   	self.receive_icon:SetVisible(data.is_already_get)
 
end

function Modules.TemplChallengeTaskItem:OnButtonClick()
	local req_info = {{task_id = self.__data.id}}
	CallModuleFunc(ModuleType.INTEGRAL_RACE,"ScoreMatchTakeTaskRewardReq",req_info)
end