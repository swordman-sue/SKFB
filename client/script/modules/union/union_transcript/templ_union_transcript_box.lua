
--
-- @brief 公会副本宝箱
-- @author: yjg
-- @date: 2017年5月25日17:21:50
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionTranscriptBox = LogicGUI.TemplUnionTranscriptBox or BaseClass(GUI.Template)

function LogicGUI.TemplUnionTranscriptBox:__init( id )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__is_auto_load = true
	self.__open_callback = function()

		self.id = id
		self:__Getwidget()
		self:__ShowUI( )
	end

	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_REWARD_RECORD , function ( _ , data )
		self:__SetShowUI()
		self:__SetShowLst()
	end )

	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_GET_RECORD , function ( _ , data )
		self:__SetShowUI()
		self:__UpShowLst( data )
	end )

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_BOX)	
end

function LogicGUI.TemplUnionTranscriptBox:__Getwidget()
	--居中层
	self.Image_center = self:GetWidget("Image_center")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--关闭
	self.Button_close = Util:WidgetButton( 
											self:GetWidget("Button_close") ,
											"" , 
											CPPGameLib.Handler(self,self.OnBtnClose) 
										)
	--内容框
	self.Image_content = self:GetWidget("Image_content")
	--滚动层
	self.Panel_slide = self:GetWidget("Panel_slide")

	--条
	self.Image_headline = {}
	--图标
	self.Panel_icon = {}
	--名字
	self.Text_name = {}
	--状态
	self.Text_state = {}
	for i = 1 , 3 do
		self.Image_headline[i] = self:GetWidget("Image_headline"..i)		
		self.Panel_icon[i] = self:GetWidget("Panel_icon"..i)
		self.Text_name[i] = self:GetWidget("Text_name"..i)
		self.Text_state[i] = self:GetWidget("Text_state"..i)

		local layou = Util:Layout( self.Image_headline[i]:GetContentWidth() , self.Image_headline[i]:GetContentHeight() ,function ( ... )
			self:OnLayer(i , true )
		end )
		layou:SetAnchorPoint( 0.5 , 0.5 )
		self.Image_headline[i]:AddChild( layou )
		PosTool.Center( layou )
	end
	self:OnLayer(1 , true )
end

function LogicGUI.TemplUnionTranscriptBox:__ShowUI( date )
	--标题
	self.lab = Util:Label(CPPGameLib.GetString("union_box"),Macros.TypefaceSize.popup,Macros.Color.lilyWhite)
	self.Image_light:AddChild(self.lab)
	PosTool.Center(self.lab)

    --副本
    local FBcnf = CPPGameLib.GetConfig("GuildFB", self.id )
    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")

    self.getType = {}
    self.icon_red_point = {}
    for i,v in ipairs(FBcnf.gamelevel_list) do
	    --设置模型
    	local level = CPPGameLib.GetConfig("GameLevel" , v )
        local hero = LogicGUI.Model.New({info_id = level.icon_id , model_type = LogicGUI.ModelType.MONSTER , parent = self.Panel_icon[i] })
        hero:SetScaleFactor(0.7)
        hero:SetToPlayTime(0)
        hero:SetPosition(self.Panel_icon[i]:GetContentWidth()/2  ,0  )

		--怪物表
		local monsterCnf = CPPGameLib.GetConfig("Monster" , level.icon_id )
		--设置
		local monsterName = Util:Name( monsterCnf.name , Macros.TypefaceSize.normal , monsterCnf.quality )
		monsterName:SetAnchorPoint( 0 , 0.5 )
		monsterName:SetPosition( 0 , self.Text_name[i] )
		self.Text_name[i]:AddChild( monsterName )

		--领取状态
		self.getType[i] = Util:Label( "" )
		self.getType[i]:SetAnchorPoint( 0 , 0.5 )
		self.getType[i]:SetPosition( 0 , self.Text_state[i]:GetContentHeight()/2 )
		self.Text_state[i]:AddChild( self.getType[i] )

		self.icon_red_point[i] = Util:Sprite(icon_red_point)
		self.icon_red_point[i]:SetPosition( self.Image_headline[i]:GetContentWidth() - 5 , self.Image_headline[i]:GetContentHeight() - 5 )
		self.Image_headline[i]:AddChild(self.icon_red_point[i] , 20)
		self.icon_red_point[i]:SetVisible( false )
    end

    --内容
	local params =  
	{
	    item_class = Modules.TemplUnionTranscriptBoxItem,
	    item_width = 135.2 ,
	    item_height = 128.00,
	    row = 3,
	    col = 5,
	    view_height = self.Image_content:GetContentHeight() - 10 ,
		touchable = true,
		touch_callback = function(item_index, item_data)
			-- self:AppointResult(item_index, item_data)
		end,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

end

--选中框
function LogicGUI.TemplUnionTranscriptBox:OnLayer( index , server )
    local selected = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
    if self.selected then
        self.selected:RemoveSelf(true)
        self.selected = nil
    end  
    self.selected = Util:Sprite9(selected)
    self.selected:SetContentSize( self.Image_headline[index]:GetContentWidth() + 5 , self.Image_headline[index]:GetContentHeight() + 5)
    self.selected:SetPosition(self.Image_headline[index]:GetContentWidth()/2 , self.Image_headline[index]:GetContentHeight()/2 )
    self.Image_headline[index]:AddChild(self.selected , 10)

    if server then
	    self:__Server( index )
	end
end

--协议处理
function LogicGUI.TemplUnionTranscriptBox:__Server( index )
	self.index = index
	if not CallModuleFunc(ModuleType.UNION , "GetInitialBegItemBox") then
		for i = 3 , 1 ,-1 do
			local info = {}
			info.chapter = self.id
			info.camp = i - 1
			CallModuleFunc(ModuleType.UNION , "GuildFbMapRewardRecordReq" , info )
		end
	else
		local info = {}
		info.chapter = self.id
		info.camp = index - 1
		CallModuleFunc(ModuleType.UNION , "GuildFbMapRewardRecordReq" , info )
	end
end

--设置UI
function LogicGUI.TemplUnionTranscriptBox:__SetShowUI(  )

	local lst = CallModuleFunc(ModuleType.UNION, "GetMyMapReward" , self.id )

	for i = 1 , 3 do
		--领取状态
		local isGet = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptSectionDraw" , self.id , i )
		if isGet == "linquguo" then
			self.getType[i]:SetString( CPPGameLib.GetString("union_yetGet") )
			self.getType[i]:SetColor(unpack( Macros.Color.front_color ))

			self.icon_red_point[i]:SetVisible(false)
		elseif isGet == "buxing" then
			self.getType[i]:SetString( CPPGameLib.GetString("union_notGet_") )
			self.getType[i]:SetColor(unpack(Macros.Color.GrayBlack))

			self.icon_red_point[i]:SetVisible(false)

		elseif isGet == "xing" then
			self.getType[i]:SetString( CPPGameLib.GetString("union_notGet") )	
			self.getType[i]:SetColor(unpack(Macros.Color.orange))

			self.icon_red_point[i]:SetVisible(true)
		end
	end




















end

--设置列表
function LogicGUI.TemplUnionTranscriptBox:__SetShowLst( data )
	local lst = CallModuleFunc(ModuleType.UNION , "GetSettleRecordList" , self.id , self.index )
	self.lst_list:SetDataList( lst )
end

--刷新列表
function LogicGUI.TemplUnionTranscriptBox:__UpShowLst( data )
    self.lst_list:ItemDataIter(function(i, item_data)
    	--位置索引
    	if item_data.index == data.box_idx then
			local fbCnf = CPPGameLib.GetConfig("GuildFB", item_data.section )
			local info = item_data
			info.id = fbCnf.map_reward_id_list[  data.reward_idx + 1 ] 
			info.num = fbCnf.map_reward_num_list[  data.reward_idx + 1]
			info.reward_idx = data.reward_idx
			info.box_idx = data.box_idx
			item_data = info
    		local item = self.lst_list:GetItem(i)
			if item then
				item:__SetShowUI(item_data)
			end  
    	end
    end)
end

function LogicGUI.TemplUnionTranscriptBox:OnBtnClose()
	self:DeleteMe()
end


Modules = Modules or {}
Modules.TemplUnionTranscriptBoxItem = Modules.TemplUnionTranscriptBoxItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionTranscriptBoxItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplUnionTranscriptBoxItem:__CreateWidgetFromLuaCode()	
    self.layout = Util:Layout(135.2 , 128 )
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.TemplUnionTranscriptBoxItem:__delete()
end

function Modules.TemplUnionTranscriptBoxItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

-- ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_close"),
-- op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_open"),
-- effects = "UI_silverybox"
function Modules.TemplUnionTranscriptBoxItem:__ShowUI( data )
	self.name = Util:Label( data.role_name , Macros.TypefaceSize.normal, Macros.Color.orange  )
	self.name:SetAnchorPoint( 0.5 , 0.5 )
	self.name:SetPosition( self.layout:GetContentWidth()/2 , self.name:GetContentHeight() )
	self.layout:AddChild( self.name )

	self:__SetShowUI( data )
end

function Modules.TemplUnionTranscriptBoxItem:__SetShowUI( data )
	if self.icon then
		self.icon:RemoveSelf(true)
		self.icon = nil
	end

	if data.box_idx then
	    self.icon = LogicGUI.IconLayer.New( true )
	    self.icon:SetData( { id = data.id , num = data.num } )
	    self.layout:AddChild(self.icon:GetNode())
	    self.icon:SetTouchSwallow(false)
	    PosTool.Center( self.icon , 0 , 20 )
	else                                                                                                                                                                                                                                                                                                                                                        
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_close")
		self.icon = Util:Sprite( sp , function ( ... )
			local isGet = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptSectionDraw" , data.section , data.level )
			if isGet == "xing" then
				local info = {}
				info.chapter = data.section
				info.camp = data.level
				info.reward_idx = data.index
				CallModuleFunc(ModuleType.UNION , "GuildFbMapRewardReq" , info )
			elseif isGet == "buxing" then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_notGet_1") )
			else
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_NotGetType") )
			end			
		end)
		self.icon:SetAnchorPoint( 0.5 , 0.5 )
		self.layout:AddChild( self.icon )
		PosTool.Center( self.icon , 0 , 20 )
	end

	self.name:SetString( data.role_name or "" )
end
