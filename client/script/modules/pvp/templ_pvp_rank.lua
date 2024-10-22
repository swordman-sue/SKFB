--
-- @brief PVP排行榜 
-- @author: yjg
-- @date: 2017年6月21日14:32:15
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPvpRank = LogicGUI.TemplPvpRank or BaseClass(GUI.Template)

function LogicGUI.TemplPvpRank:__init( )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
	self.__tab_params = {
		{sp_2,sp_1},
		{
			CPPGameLib.GetString("pvp_wholeServer") ,
			CPPGameLib.GetString("pvp_oneselfServer") ,
			CPPGameLib.GetString("pvp_award") ,
		},
		10,
		CPPGameLib.Handler(self, self.OnYeMei)	
	}

	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_PVP_RANK_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_PVP_RANK)	

	self:BindGlobalEvent(Macros.Event.PVPModule.PVP_RANK,function (_, data)
		local lst = {
				[Macros.Game.RankType.ONESELF_SERVER] = 2 , 
				[Macros.Game.RankType.ALL_SERVER] = 1 , 
		}
		if lst[data.type] == self.index then
			for i,v in ipairs(data.rank_list) do
				v.rank = i
			end
			self.rank_list:SetDataList( data.rank_list )
		end

		local text = CPPGameLib.GetString("worldBoss_NoRank")
		if data and data.rank ~= 0 then
			text = CPPGameLib.GetString( "worldBoss_Rank" , data.rank )
		end

		self.MyRank:SetString(text)
	end)

end

function LogicGUI.TemplPvpRank:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplPvpRank:__Getwidget()
	--居中
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--内容层
	self.Image_lstBg = self:GetWidget("Image_lstBg")
	--页签
	self.Panel_slide = self:GetWidget("Panel_slide")
	--关闭按钮
	self.Button_close = Util:WidgetButton( 
										self:GetWidget("Button_close") , 
										nil ,
										CPPGameLib.Handler(self,self.OnBtnClose) )
end

function LogicGUI.TemplPvpRank:__ShowUI()
	--标题
	local rank_title = Util:Label( CPPGameLib.GetString("rank_title"),Macros.TypefaceSize.popup,Macros.Color.lilyWhite )
	rank_title:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( rank_title )
	PosTool.Center( rank_title )

    --排行榜
    local params =  
    {
        item_class = Modules.TemplPVPRankItem,
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

    --奖励
    local params =  
    {
        item_class = Modules.TemplPVPRankAward,
        item_width = self.Image_lstBg:GetContentWidth() ,
        item_height = 65 ,
        row = 6,
        col = 1,
        -- view_width = 925.00,
        view_height = self.Image_lstBg:GetContentHeight() - 55 ,
    }
    -- 创建参数
    self.award_list = GUI.ScrollView.New(params)
	self.award_list:SetPosition( self.Image_lstBg:GetContentWidth()/2 , self.Image_lstBg:GetContentHeight()/2 - 22.5 )
    self.Image_lstBg:AddChild(self.award_list:GetNode())
    self.award_list:SetDataList( CallModuleFunc(ModuleType.PVP, "GetRankReward") )

    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_property_tag")
	local sp = Util:Sprite9(path)
	sp:SetContentSize( 400 , sp:GetContentHeight()  )
	sp:SetAnchorPoint( 0 , 1 )
	sp:SetPosition( 5 - self.Image_lstBg:GetContentWidth()/2 , self.Image_lstBg:GetContentHeight()/2 + 17.5 )
	self.award_list:GetNode():AddChild(sp)

	local pvp_goodsAward = Util:Label( CPPGameLib.GetString("pvp_goodsAward"),Macros.TypefaceSize.popup,Macros.Color.lilyWhite )
	pvp_goodsAward:SetAnchorPoint( 0.5 , 0.5 )
	sp:AddChild( pvp_goodsAward )
	PosTool.Center( pvp_goodsAward )

	local sp_ = Util:Sprite9(path)
	sp_:SetContentSize( 185 , sp:GetContentHeight()  )
	sp_:SetAnchorPoint( 1 , 1 )
	sp_:SetPosition( self.Image_lstBg:GetContentWidth()/2 - 5 , self.Image_lstBg:GetContentHeight()/2 + 17.5 )
	self.award_list:GetNode():AddChild(sp_)

	local pvp_titleAward = Util:Label( CPPGameLib.GetString("pvp_titleAward"),Macros.TypefaceSize.popup,Macros.Color.lilyWhite )
	pvp_titleAward:SetAnchorPoint( 0.5 , 0.5 )
	sp_:AddChild( pvp_titleAward )
	PosTool.Center( pvp_titleAward )

    --页签
    self.__tab:On(1)
    self.__tab:SetDirection(true , 0.5)
    self.__tab:SetPosition( self.Panel_slide:GetContentWidth()/2 , self.Panel_slide:GetContentHeight() )
    self.Panel_slide:AddChild(self.__tab:GetNode())  

    --我的排名
    local MyRank = Util:Label( CPPGameLib.GetString("worldBoss_MyRank") , nil , Macros.Color.keypoint  )
    MyRank:SetAnchorPoint( 0 , 0.5 )
    MyRank:SetPosition(  self.Image_lstBg:GetPositionX() + 10 , self.Image_lstBg:GetPositionY() - MyRank:GetContentHeight() + 5 )
    self.Image_bg:AddChild( MyRank )

    self.MyRank = Util:Label( "" , nil , Macros.Color.content  )
    PosTool.RightTo( MyRank , self.MyRank )
    self.Image_bg:AddChild( self.MyRank )
end

function LogicGUI.TemplPvpRank:OnYeMei( index )

	self.rank_list:SetVisible(false)
	self.award_list:SetVisible(false)

	self.index = index

	if index == 1 then
		self.rank_list:SetVisible(true)

		local data = {}
		data.type = Macros.Game.RankType.ALL_SERVER
		CallModuleFunc(ModuleType.RANK, "RankListRequest",data)

		-- local t1 = {}
		-- for i = 1 , 10 do
		-- 	table.insert( t1 , { id = i } )
		-- end
		-- self.rank_list:SetDataList( t1 )

	elseif index == 2 then
		self.rank_list:SetVisible(true)

		local data = {}
		data.type = Macros.Game.RankType.ONESELF_SERVER 
		CallModuleFunc(ModuleType.RANK, "RankListRequest",data)


		-- local t1 = {}
		-- for i = 1 , 10 do
		-- 	table.insert( t1 , { id = i } )
		-- end
		-- self.rank_list:SetDataList( t1 )

	elseif index == 3 then
		self.award_list:SetVisible(true)
	end
end

function LogicGUI.TemplPvpRank:OnBtnClose()
	self:DeleteMe()
end


--排行榜条目
Modules = Modules or {}
Modules.TemplPVPRankItem = Modules.TemplPVPRankItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPVPRankItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_PVP_RANK_ITEM)   
end

function Modules.TemplPVPRankItem:__delete()
end

function Modules.TemplPVPRankItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.TemplPVPRankItem:__Getwidget( data )
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--积分
	self.Text_integral = self:GetWidget("Text_integral")
	--胜局
	self.Text_victory = self:GetWidget("Text_victory")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--战力
	self.Text_power = self:GetWidget("Text_power")
	--区
	self.Text_district = self:GetWidget("Text_district")
end

function Modules.TemplPVPRankItem:__ShowUI(data)

	if self.__data.rank <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..self.__data.rank)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetPosition( 50 , self.Image_item:GetContentHeight() / 2)
		self.Image_item:AddChild(icon)
	else
		local iconValue = Util:Label(self.__data.rank,Macros.TypefaceSize.popup,Macros.Color.headline)
		iconValue:SetPosition( 50 , self.Image_item:GetContentHeight() / 2)
		self.Image_item:AddChild(iconValue)	
	end

    --头像
    local icon = LogicGUI.RoleIcon.New(false)
    self.Panel_icon:AddChild(icon:GetNode())
    icon:SetData(self.__data.head_icon)
    PosTool.Center(icon)

    self.Panel_icon:SetTouchSwallow(false)
    icon:GetNode():SetTouchSwallow(false)

    --名字
    local name = Util:Label( self.__data.role_name , Macros.TypefaceSize.normal , Macros.Color.headline )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild( name )

    --战力
    local zhandouli = Util:Label( CPPGameLib.GetString("zhandouli") .. self.__data.battle_value , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    zhandouli:SetAnchorPoint( 0 , 0.5 )
    zhandouli:SetPosition( 0 , self.Text_power:GetContentHeight()/2 )
    self.Text_power:AddChild( zhandouli )

    --区
    local lst = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", self.__data.server_id)
    if not lst then
	    local district = Util:Label( CPPGameLib.GetString("common_district" , self.__data.server_id ) .. " - " .. lst.name , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	    district:SetAnchorPoint( 0 , 0.5 )
	    district:SetPosition( 0 , self.Text_district:GetContentHeight()/2 )
	    self.Text_district:AddChild( district )
	end

    --积分
    local pvp_integral = Util:Label( CPPGameLib.GetString("pvp_integral" ) , Macros.TypefaceSize.normal , Macros.Color.content )
    pvp_integral:SetAnchorPoint( 0.5 , 0.5 )
    pvp_integral:SetPosition( self.Text_victory:GetPositionX() , self.Image_item:GetContentHeight()/2 + 28 )
    self.Image_item:AddChild(pvp_integral)

    --积分
    local pvp_integral = Util:Label( self.__data.context , Macros.TypefaceSize.normal , Macros.Color.content )
    pvp_integral:SetAnchorPoint( 0.5 , 0.5 )
    pvp_integral:SetPosition( self.Text_victory:GetPositionX() , self.Image_item:GetContentHeight()/2 )
    self.Image_item:AddChild(pvp_integral)

    --胜场
    local pvp_victory = Util:Label( CPPGameLib.GetString("pvp_victory" , self.__data.extend ) , Macros.TypefaceSize.normal , Macros.Color.headline )
    pvp_victory:SetAnchorPoint( 0.5 , 0.5 )
    pvp_victory:SetPosition( self.Text_victory:GetPositionX() , self.Image_item:GetContentHeight()/2 - 28 )
    self.Image_item:AddChild(pvp_victory)

end

--奖励条目
Modules = Modules or {}

Modules.TemplPVPRankAward = Modules.TemplPVPRankAward or BaseClass(GUI.ScrollItem)

function Modules.TemplPVPRankAward:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPVPRankAward:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout( 598 , 65 )
  self.layout:SetClippingEnable(false)
  self.layout:SetTouchEnabled(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TemplPVPRankAward:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	local sp = nil
	if data.show == true then
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
		local sp_2 = Util:Sprite9(sp)
		sp_2:SetContentSize(self.layout:GetContentWidth() - 10  , self.layout:GetContentHeight() )
		sp_2:SetPosition(self.layout:GetContentWidth() / 2 , self.layout:GetContentHeight() /2)
		self.layout:AddChild(sp_2)
	end

	if not data.text then return end
	-- 2326 pvp 头衔奖励歪了
	if data.sort <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.sort)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetPosition( 40 , self.layout:GetContentHeight() / 2)
		self.layout:AddChild(icon)

		local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "rank_"..data.title_id , true )
		local rank_1 = Util:Sprite( rankSp )
		rank_1:SetAnchorPoint( 0.5 , 0.5 )
		rank_1:SetPosition(self.layout:GetContentWidth() / 2 + 200 , self.layout:GetContentHeight() /2)
		self.layout:AddChild(rank_1)

		-- local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "rank_"..data.sort )
		-- local rank = Util:Sprite( rankSp )
		-- rank:SetAnchorPoint( 0.5 , 0.5 )
		-- rank:SetPosition( rank_1:GetPositionX() + 100 , self.layout:GetContentHeight() /2)
		-- self.layout:AddChild(rank)

	else
		-- local iconValue = Util:Label(data.sort,nil,Macros.Color.headline)
		-- iconValue:SetPosition( 40 , self.layout:GetContentHeight() / 2)
		-- self.layout:AddChild(iconValue)	
	end

	local text = Util:Label( CPPGameLib.GetString( "pvp_ranking" , data.text ) , Macros.TypefaceSize.normal , Macros.Color.headline )
	text:SetAnchorPoint( 1 , 0.5 )
	text:SetPosition( 150 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild( text )

	local x = 180
	for i , v in ipairs( data.item ) do
	    local icon = LogicGUI.IconLayer.New()
	    icon:SetPosition( x , self.layout:GetContentHeight()/2 )
	    icon:SetScaleFactor( 0.7 )
	    self.layout:AddChild(icon:GetNode())
	    icon:SetData({id = v.id, num = v.num})
	    x = x + 60 
	    icon:SetTouchSwallow( false )
	end
end