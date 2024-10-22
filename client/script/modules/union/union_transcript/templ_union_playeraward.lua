
--
-- @brief 通关奖励
-- @author: yjg
-- @date: 2017年5月25日17:21:50
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionPlayerAward = LogicGUI.TemplUnionPlayerAward or BaseClass(GUI.Template)

function LogicGUI.TemplUnionPlayerAward:__init( )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI	
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_AWARD_GET , function (_, data)
    	self:__SetShowUI()
    end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_UNION_PLAYER_AWARD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_PLAYER_AWARD)	
end

function LogicGUI.TemplUnionPlayerAward:__Getwidget()
	--居中层
	-- self.Image_center = self:GetWidget("Image_center")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--关闭
	self.Button_close =  Util:WidgetButton( 
											self:GetWidget("Button_close") , 
											" " , 
											CPPGameLib.Handler(self,self.OnButtonClose) 
										)
	--内容
	self.Image_content = self:GetWidget("Image_content")
end

function LogicGUI.TemplUnionPlayerAward:__ShowUI( date )
	--通关奖励
	local tower_congratsGetAward = Util:Label( CPPGameLib.GetString("tower_congratsGetAward") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	tower_congratsGetAward:SetAnchorPoint(0.5,0.5)
	tower_congratsGetAward:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(tower_congratsGetAward)

	--进度
	local params = 
	{
	    item_class = Modules.TemplUnionPlayerAwardItem,
	    item_width = 442.00,
	    item_height = 133.00,
	    row = 3,
	    col = 1,
	    item_space_r = 5,
	    view_height = self.Image_content:GetContentHeight() - 5 ,
	}
	self.__recommend_scrollview = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.__recommend_scrollview:GetNode())
	PosTool.Center(self.__recommend_scrollview)

	local cnf = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptAwardLst")
	self.__recommend_scrollview:SetDataList( cnf )
end

function LogicGUI.TemplUnionPlayerAward:__SetShowUI( date )
	local cnf = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptAwardLst")
	self.__recommend_scrollview:SetDataList( cnf )
end

function LogicGUI.TemplUnionPlayerAward:OnButtonClose( date )
	self:DeleteMe()
end






Modules = Modules or {}

Modules.TemplUnionPlayerAwardItem = Modules.TemplUnionPlayerAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionPlayerAwardItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_PLAYER_AWARD_ITEM)	
	self:__Getwidget()
end

function Modules.TemplUnionPlayerAwardItem:__delete()

end

function Modules.TemplUnionPlayerAwardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)	
	self:__ShowUI(data)
end

function Modules.TemplUnionPlayerAwardItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--按钮占位
	self.Panel_btn = self:GetWidget("Panel_btn")
end

function Modules.TemplUnionPlayerAwardItem:__ShowUI(data)
	--名字
	local name = Util:Label( data.name , Macros.TypefaceSize.largish ,Macros.Color.keypoint )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
	self.Text_name:AddChild( name )

	--图标
	for i , v in ipairs( data.chapter_reward_list ) do
		local icon = LogicGUI.IconLayer.New(false)
		icon:SetData({ id = v[1] , num = v[2] })
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetTouchSwallow(false)
		icon:SetPosition( (icon:GetNode():GetContentWidth()/2) + ( icon:GetNode():GetContentWidth() + 5 ) * (i-1) , self.Panel_icon:GetContentHeight()/2 )
		self.Panel_icon:AddChild( icon:GetNode() )
	end

	self:SetShowUI( data )
end

function Modules.TemplUnionPlayerAwardItem:SetShowUI(data)
	if self.btn then
		self.btn:RemoveSelf(true)
		self.btn = nil
	end

	local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
	local awardlst = CallModuleFunc(ModuleType.UNION , "GetAwardGetRecord" , data.id )
		
	if data.id >= section then
		local btn_btn_green = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green")
		self.btn = Util:Button( 
								btn_btn_green , 
								function ( ... )
									print("xy","未通过")
								end , 
								CPPGameLib.GetString("union_noPass") , 
								Macros.TypefaceSize.button , 
								nil , Macros.Color.button_yellow  
							)
		self.btn:SetTouchEnabled(false)
	elseif awardlst then
		local receive_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"receive_icon",true)
		self.btn = Util:Sprite( receive_icon )
	else
		local btn_btn_blue = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_blue")
		self.btn = Util:Button( 
								btn_btn_blue , 
								function ( ... )
									print("xy","可领")
									local info = {}
									info.chapter = data.id
									CallModuleFunc(ModuleType.UNION , "GuildFbChapterRewardReq" , info )
								end , 
								CPPGameLib.GetString("union_Pass") , 
								Macros.TypefaceSize.button , 
								nil , Macros.Color.button_red 
							)
	    local effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian"}, self.btn )
		effect:SetPosition( self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 + 2 )
	end
	self.btn:SetAnchorPoint( 0.5,0.5 )
	self.Panel_btn:AddChild( self.btn )
	PosTool.Center( self.btn )
end


