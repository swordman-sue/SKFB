--
-- @brief PVP奖励 
-- @author: yjg
-- @date: 2017年6月21日16:16:37
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPvpAward = LogicGUI.TemplPvpAward or BaseClass(GUI.Template)

function LogicGUI.TemplPvpAward:__init( id )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self.__open_callback = function()
		self.id = id
		self:__Getwidget()
		self:__ShowUI()
	end

    self:BindGlobalEvent(Macros.Event.PVPModule.PVP_AWARD,function (_, data)
        self:__SetShowUI()
    end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_PVP_AWARD_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_PVP_AWARD)	
end

function LogicGUI.TemplPvpAward:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplPvpAward:__Getwidget()
	--居中
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--内容层
	self.Image_lstBg = self:GetWidget("Image_lstBg")
	--关闭按钮
	self.Button_close = Util:WidgetButton( 
											self:GetWidget("Button_close") , 
											nil ,
											CPPGameLib.Handler(self,self.OnBtnClose) 
										)
	--一键按钮
	self.Button_oneKey = Util:WidgetButton( 
											self:GetWidget("Button_oneKey") , 
											CPPGameLib.GetString("pvp_oneKeyGet") ,
											CPPGameLib.Handler(self,self.OnButtonOneKey) ,
											Macros.TypefaceSize.largish,nil,
											Macros.Color.button_red
										)
end

function LogicGUI.TemplPvpAward:__ShowUI()

	local text = ""
	if self.id == Macros.Game.PVP_REWARD_TYPE.DAY then
		text = CPPGameLib.GetString("pvp_everyday")
	elseif self.id == Macros.Game.PVP_REWARD_TYPE.WEEK then
		text = CPPGameLib.GetString("pvp_weekly")
	end		
	--标题
	local rank_title = Util:Label( text , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	rank_title:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( rank_title )
	PosTool.Center( rank_title )

    --排行榜
    local params =  
    {
        item_class = Modules.TemplPvpAwardItem,
        item_width = self.Image_lstBg:GetContentWidth() ,
        item_height = 130,
        row = 4,
        col = 1,
        -- view_width = 925.00,
        view_height = self.Image_lstBg:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.rank_list = GUI.ScrollView.New(params)
    self.Image_lstBg:AddChild(self.rank_list:GetNode())
    PosTool.Center(self.rank_list:GetNode())

    local t1 = CallModuleFunc( ModuleType.PVP , "GetBattleTimesReward" , self.id )
    self.rank_list:SetDataList( t1 )
end

function LogicGUI.TemplPvpAward:__SetShowUI()
    local t1 = CallModuleFunc( ModuleType.PVP , "GetBattleTimesReward" , self.id )
    self.rank_list:SetDataList( t1 )
    
  --   self.rank_list:ItemDataIter(function(i, item_data)
  --   	local item = self.rank_list:GetItem(i)
		-- if item then
		-- 	item:__SetShowUI(item_data)
		-- end 	     
  --   end)
end

function LogicGUI.TemplPvpAward:OnBtnClose()
	self:DeleteMe()
end

function LogicGUI.TemplPvpAward:OnButtonOneKey()
	-- self:DeleteMe()
	local t1 = CallModuleFunc( ModuleType.PVP , "GetOneKeyLst" , self.id )
	CPPGameLib.PrintTable("xy" , t1 ,"物品合成响应")   

	if #t1 ~= 0 then
		local info = {}
		info.type = self.id
		info.take_reward_list = t1
		CallModuleFunc( ModuleType.PVP , "PVPTakeBattleRewardReq" , info )
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rebel_noneCanGet"))
	end
end


--奖励条目
Modules = Modules or {}
Modules.TemplPvpAwardItem = Modules.TemplPvpAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPvpAwardItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_PVP_AWARD_ITEM)   
end

function Modules.TemplPvpAwardItem:__delete()
end

function Modules.TemplPvpAwardItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI(data)
end

function Modules.TemplPvpAwardItem:__Getwidget( data )
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--按钮占位
	self.Panel_btn = self:GetWidget("Panel_btn")
	--提示
	self.Text_tips = self:GetWidget("Text_tips")
	--进度
	self.Text_schedule = self:GetWidget("Text_schedule")
end

function Modules.TemplPvpAwardItem:__ShowUI(data)
    --头像
    local icon = LogicGUI.IconLayer.New(true)
    self.Panel_icon:AddChild(icon:GetNode())
    icon:SetData({id = data.item[1].id , num = data.item[1].num })
    PosTool.Center(icon)

    --物品信息
    local type_ , info_ = ObjIDTool.GetInfo( data.item[1].id )
    local name = Util:Name( info_.name , Macros.TypefaceSize.normal , info_.quality or info_.init_quality )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild( name )

    --提示
    local text = ""
    if data.type == Macros.Game.PVP_REWARD_TYPE.DAY then
    	text = CPPGameLib.GetString( "pvp_getDayTips" , data.times )
    elseif data.type == Macros.Game.PVP_REWARD_TYPE.WEEK then
		text = CPPGameLib.GetString( "pvp_getWeeklyTips" , data.times )
    end
    local tips = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.content )
    tips:SetAnchorPoint( 0 , 0.5 )
    tips:SetPosition( 0 , self.Text_tips:GetContentHeight()/2 )
    self.Text_tips:AddChild( tips )

    --进度
    local pvp_schedule = Util:Label( CPPGameLib.GetString("pvp_schedule") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    pvp_schedule:SetAnchorPoint( 0 , 0.5 )
    pvp_schedule:SetPosition( 0 , self.Text_schedule:GetContentHeight()/2 )
    self.Text_schedule:AddChild( pvp_schedule )

    --进度
    self.schedule = Util:Label( CPPGameLib.GetString("pvp_schedule") , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Text_schedule:AddChild( self.schedule )
    PosTool.RightTo( pvp_schedule , self.schedule )

    self:__SetShowUI( data )
end

function Modules.TemplPvpAwardItem:__SetShowUI(data)

	--数量
	local num = nil
    if data.type == Macros.Game.PVP_REWARD_TYPE.DAY then
    	num = CallModuleFunc( ModuleType.PVP , "GetDayBattleTimes" )
    elseif data.type == Macros.Game.PVP_REWARD_TYPE.WEEK then
    	num = CallModuleFunc( ModuleType.PVP , "GetWeekBattleTimes" )
    end
    if num > data.times then
    	num = data.times
    end
   	self.schedule:SetString( (num or 0) ..  "/" .. data.times )

   	if self.btn then
   		self.btn:RemoveSelf(true)
   		self.btn = nil
   	end

	--图标
	local record = CallModuleFunc( ModuleType.PVP , "GetRecord" , data.type , data.times )

	--可领取
	if num >= data.times and not record then
		local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
		self.btn = Util:Button( sp_1 , function ( ... )

			local info = {}
			info.type = data.type

			local t1 = {}
			table.insert( t1 , { times = data.times } )
			info.take_reward_list = t1
			CallModuleFunc( ModuleType.PVP , "PVPTakeBattleRewardReq" , info )

		end , CPPGameLib.GetString("pvp_receive") , Macros.TypefaceSize.normal , nil , Macros.Color.button_red )
		self.btn:SetAnchorPoint( 0.5 , 0.5 )
		self.Panel_btn:AddChild( self.btn )
		PosTool.Center( self.btn )

		self.Panel_btn:SetTouchSwallow(false)
		self.btn:SetTouchSwallow(true)

	--不可领取
	elseif num < data.times and not record then

		local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
		self.btn = Util:Button( sp_1 , function ( ... )
		end , CPPGameLib.GetString("pvp_receive") , Macros.TypefaceSize.normal , nil ,  Macros.Color.button_red )
		self.btn:SetAnchorPoint( 0.5 , 0.5 )
		self.Panel_btn:AddChild( self.btn )
		PosTool.Center( self.btn )
		Util:GreyButton( self.btn )

		self.Panel_btn:SetTouchSwallow(false)
		self.btn:SetTouchSwallow(false)

	--有记录
	elseif record then

		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
		self.btn = Util:Sprite(sp)
		self.Panel_btn:AddChild( self.btn )
		PosTool.Center( self.btn )

		self.Panel_btn:SetTouchSwallow(false)
	end

end

