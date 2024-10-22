--
-- @brief 世界boss奖励 
-- @author: yjg
-- @date: 2016年11月18日17:32:25
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldbossAward = LogicGUI.TemplWorldbossAward or BaseClass(GUI.Template)

function LogicGUI.TemplWorldbossAward:__init(id,direct,itemResolveId)
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
			CPPGameLib.GetString("worldBoss_honorAward"),
			CPPGameLib.GetString("worldBoss_killAward"),
		},
		10,CPPGameLib.Handler(self, self.OnYeMei)
	}
	
	self.event_award = self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_AWARD,function (_,type,data)
		self:ToTypeUpLst(type,data)
	end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD)	
end

function LogicGUI.TemplWorldbossAward:__delete()
	if self.honor_lst then
		self.honor_lst:DeleteMe()
		self.honor_lst = nil 
	end
	if self.kill_lst then
		self.kill_lst:DeleteMe()
		self.kill_lst = nil 
	end
end

function LogicGUI.TemplWorldbossAward:__Getwidget()
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --内容背景
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
    --获取按钮
    self.Button_get = Util:WidgetButton( 
    										self:GetWidget("Button_get") , 
    										CPPGameLib.GetString("friend_Button_oneUseGet") , 
    										CPPGameLib.Handler(self,self.OnButtonGet) ,
    										Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow
    									 )
end

function LogicGUI.TemplWorldbossAward:__ShowUI()
	--奖励
	local award = Util:Label( CPPGameLib.GetString("worldBoss_award") , Macros.TypefaceSize.largish, Macros.Color.white )
	award:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	award:SetAnchorPoint(0.5,0.5)
	self.Image_light:AddChild(award)

    --条目
    self.params =  
    {
        item_class = Modules.TemplWorldbossAwardItem,
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

function LogicGUI.TemplWorldbossAward:OnButtonClose()
	self:DeleteMe()
end

--页签回调
function LogicGUI.TemplWorldbossAward:OnYeMei(index)
	self.index = index
	if self.honor_lst then
	    self.honor_lst:SetVisible(false)
	end

	if self.kill_lst then
	    self.kill_lst:SetVisible(false)
	end

	local type = nil
	if index == 1 then
		if not self.honor_lst then
			self:EstablishHonorLst()
		else
			self.honor_lst:SetVisible(true)
		end
		type = Macros.Game.RankType.WORLDBOSS_HONOR
	elseif index == 2 then
		if not self.kill_lst then
			self:EstablishKillLst()
		else
			self.kill_lst:SetVisible(true)
		end
		type = Macros.Game.RankType.WORLDBOSS_HARM
	end

	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "AutoGet", index )
	if lst and #lst.reward_list ~= 0 then
		self.Button_get:SetEnabled(true)
    	local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.Button_get:SetOutline(r, g, b, a, 2)
	else
		Util:GreyButton( self.Button_get )
	end

end

--刷新列表
function LogicGUI.TemplWorldbossAward:ToTypeUpLst( type , data )
	local lst = nil
	--荣誉
	if type == Modules.WorldBossModule.REWARD_TYPE.HONOR then
		lst = self.honor_lst
		self.honor_lst:SetDataList( CallModuleFunc(ModuleType.WORLDBOSS, "GetHonor") )
	elseif type == Modules.WorldBossModule.REWARD_TYPE.KILL then
		lst = self.kill_lst
		self.kill_lst:SetDataList( CallModuleFunc(ModuleType.WORLDBOSS, "GetKill") )
	end

	--列表相关调整
	local item_data
	lst:ItemDataIter(function(i, item_data , item )
		if data[item_data.id] then
			item_data.getType = true
			if item then
				-- item:SetType( item_data )
			end
		end
	end)

	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "AutoGet", self.index )
	if lst and #lst.reward_list ~= 0 then
		self.Button_get:SetEnabled(true)
    	local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.Button_get:SetOutline(r, g, b, a, 2)
	else
		Util:GreyButton( self.Button_get )
	end

	self:SetRed()
end

--创建荣誉表
function LogicGUI.TemplWorldbossAward:EstablishHonorLst( ... )
	self.honor_lst = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.honor_lst:GetNode())
	PosTool.Center(self.honor_lst:GetNode())
	self.honor_lst:SetDataList( CallModuleFunc(ModuleType.WORLDBOSS, "GetHonor") )

	local data = CallModuleFunc(ModuleType.WORLDBOSS, "GetRecordDraw" , Modules.WorldBossModule.REWARD_TYPE.HONOR )
	self:ToTypeUpLst(Modules.WorldBossModule.REWARD_TYPE.HONOR , data )
end

--创建击杀表
function LogicGUI.TemplWorldbossAward:EstablishKillLst( ... )
    self.kill_lst = GUI.ScrollView.New(self.params)
    self.Image_lstBg:AddChild(self.kill_lst:GetNode())
    PosTool.Center(self.kill_lst:GetNode())
    self.kill_lst:SetDataList( CallModuleFunc(ModuleType.WORLDBOSS, "GetKill") )

	local data = CallModuleFunc(ModuleType.WORLDBOSS, "GetRecordDraw" , Modules.WorldBossModule.REWARD_TYPE.KILL )
	self:ToTypeUpLst(Modules.WorldBossModule.REWARD_TYPE.KILL , data )
end

function LogicGUI.TemplWorldbossAward:OnButtonGet()
	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "AutoGet",self.index)
	if lst and #lst.reward_list ~= 0 then
		CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossRewardReq",lst)
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("rebel_noneCanGet") )
	end
end

function LogicGUI.TemplWorldbossAward:SetRed()
	local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")

	local data = CallModuleFunc(ModuleType.WORLDBOSS, "GetRecordDraw" , Modules.WorldBossModule.REWARD_TYPE.HONOR )
	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetHonor")
	local honorIndex = 0
	for i,v in ipairs(lst) do
		if v.type == true and not data[v.id] then
			honorIndex = honorIndex + 1 
		end
	end

	if self.honor_red then
		self.honor_red:RemoveSelf(true)
		self.honor_red = nil
	end

	if honorIndex ~= 0 then
		self.honor_red = Util:Sprite(icon_red_point)
		self.honor_red:SetPosition( self.__tab.btn_list[1]:GetContentWidth() - 5 , self.__tab.btn_list[1]:GetContentHeight() - 5 )
		self.__tab.btn_list[1] :AddChild(self.honor_red , 20)
	end

	local data = CallModuleFunc(ModuleType.WORLDBOSS, "GetRecordDraw" , Modules.WorldBossModule.REWARD_TYPE.KILL )
	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetKill")
	local recordIndex = 0
	for i,v in ipairs(lst) do
		if v.type == true and not data[v.id] then
			recordIndex = recordIndex + 1 
		end
	end

	if self.record_red then
		self.record_red:RemoveSelf(true)
		self.record_red = nil
	end

	if recordIndex ~= 0 then
		self.record_red = Util:Sprite(icon_red_point)
		self.record_red:SetPosition( self.__tab.btn_list[2]:GetContentWidth() - 5 , self.__tab.btn_list[2]:GetContentHeight() - 5 )
		self.__tab.btn_list[2] :AddChild(self.record_red , 20)
	end

end









Modules = Modules or {}
Modules.TemplWorldbossAwardItem = Modules.TemplWorldbossAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplWorldbossAwardItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_AWARD_ITEM) 
    self:__Getwidget()
end

function Modules.TemplWorldbossAwardItem:__delete()
end

function Modules.TemplWorldbossAwardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	self:__ShowUI(data)
end

function Modules.TemplWorldbossAwardItem:__AddToParent()
end

function Modules.TemplWorldbossAwardItem:__Getwidget( ... )
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--内容背景
	self.Image_content = self:GetWidget("Image_content")
	--名称
	self.Text_name = self:GetWidget("Text_name")
	--按钮
	self.Button_click = Util:WidgetButton( 
											self:GetWidget("Button_click") , 
											CPPGameLib.GetString("worldBoss_receive") , 
											CPPGameLib.Handler(self,self.OnButtonClick) , 
											Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow
										)
	self.Button_click:SetZOrder(10)
	self.Button_click:SetVisible(false)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
	self.receive_icon = Util:Sprite(sp)
	self.receive_icon:SetAnchorPoint(0.5,0.5)
	self.receive_icon:SetPosition( self.Button_click:GetPositionX() , self.Button_click:GetPositionY() )
	self.Image_bg:AddChild( self.receive_icon )
	self.receive_icon:SetVisible(false)

	-- local effect
	-- self.effect_id,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.Button_click)
	-- effect:SetScaleFactor( 1.2 )
	-- PosTool.Center(effect)
end

function Modules.TemplWorldbossAwardItem:__ShowUI( data )
	--图标
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = data.reward_item_id , num = data.reward_item_num})
    self.Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)     

    local type_ , cnf = ObjIDTool.GetInfo(data.reward_item_id)

    --名字
    local name = Util:Name(cnf.name,nil,cnf.quality)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

    --文本
    local dice_label = Util:Label(data.lab , Macros.TypefaceSize.normal , Macros.Color.keypoint, nil, self.Image_content:GetContentWidth() - 10)
    dice_label:SetPosition( 10 , self.Image_content:GetContentHeight() - 10 )
    dice_label:SetAnchorPoint(0,1)
    self.Image_content:AddChild(dice_label)

    --进度
   	local worldBoss_schedule = Util:Label( CPPGameLib.GetString("worldBoss_schedule") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
   	worldBoss_schedule:SetAnchorPoint(0,0.5)
   	worldBoss_schedule:SetPosition( 10 , worldBoss_schedule:GetContentHeight() )
   	self.Image_content:AddChild(worldBoss_schedule)

   	local my_Value = Util:Label( data.my_value , Macros.TypefaceSize.normal , Macros.Color.content )
   	self.Image_content:AddChild(my_Value)
   	PosTool.RightTo(worldBoss_schedule , my_Value)

    self:SetType( data )
end

function Modules.TemplWorldbossAwardItem:OnButtonClick()
	local info = {}
	info.reward_type = self.__data.enum
	info.reward_list = {}
	info.reward_list[1] = {}
	info.reward_list[1].reward_id = self.__data.id
	CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossRewardReq",info)
end

function Modules.TemplWorldbossAwardItem:SetType( data )
    if data.type == false then
    	Util:GreyButton( self.Button_click )
    else
    	self.Button_click:SetEnabled(true)
    end

    if data.getType then
		self.Button_click:SetVisible(false)
		self.receive_icon:SetVisible(true)
	else
		self.Button_click:SetVisible(true)
		self.receive_icon:SetVisible(false)
    end

end