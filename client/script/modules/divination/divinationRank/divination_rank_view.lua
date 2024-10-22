--[[
%% @module: 占卜排行榜
%% @author: yjg
%% @created: 2017年11月24日11:29:11
--]]


Modules = Modules or {} 

Modules.DivinationRankView = Modules.DivinationRankView or BaseClass(BaseView)

function Modules.DivinationRankView:__init()
	self.__layout_name = "divination_rank"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("rank_title"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			self:CloseManual()
		end,
	}

	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
  	local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
  	self.__tab_params = {
  		{sp_2,sp_1},
  		{	
  			CPPGameLib.GetString("divination_collectValue"),
	    },
	    10,
	    CPPGameLib.Handler(self, self.changeTag),
	    false
	}

	self.__open_callback = function()   	
    	self:__OpenCallback()
    end
 --    --请求排行榜数据回调事件
	-- self.__getRankBattleListReqEvent = self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_RANK, function(data)
	-- 		-- self.__rank_data_list[self.__selected_index] = data
	-- 		-- self:__SetRankList()
 --    end)
	-- --请求等级榜排行榜数据回调事件
	-- self.__getRankLevelListReqEvent = self:BindGlobalEvent(Macros.Event.RankModule.RANK_LEVEL_LIST_REQ, function(data)
	--         self.levelRankDataList = data
	--         self:__SetRankLevelList()
	--     end)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.RANK_VIEW_CELL)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RANK)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RANK,true)
end

function Modules.DivinationRankView:__OpenCallback()
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.DivinationRankView:__Dispose()

end

function Modules.DivinationRankView:__Getwidget()
 	--等级榜
 	self.Panel_level = self:GetWidget("Panel_level")	

	--头像
	self.Panel_headIcon = self:GetWidget("Panel_headIcon")

	--名字
	self.Text_playerName = self:GetWidget("Text_playerName")
	self.Text_playerName:SetText("")

	--等级
	self.Text_level = self:GetWidget("Text_level")
	self.Text_level:SetText("")

	--公会
	self.Text_union = self:GetWidget("Text_union")
	self.Text_union:SetText("")

	--等级数值
	self.Text_levelNum = self:GetWidget("Text_levelNum")
	self.Text_levelNum:SetText("")

	--所在公会
	self.Text_unionName = self:GetWidget("Text_unionName")
	self.Text_unionName:SetText("")

	--星图片
	self.Image_star = self:GetWidget("Image_star")
	self.Image_star:SetVisible(false)

	--查看详细资料
	self.Button_watchDetail = Util:WidgetButton( 
													self:GetWidget("Button_watchDetail") , 
													CPPGameLib.GetString("rank_watchDetail") , 
													CPPGameLib.Handler(self,self.OnButtonWatchDetail),
													Macros.TypefaceSize.popup,
													Macros.Color.btn,
													Macros.Color.button_yellow
												)

	--我的排名	
	self.Text_myRank = self:GetWidget("Text_myRank")
	self.Text_myRank:SetText("")

	--我的等级
	self.Text_myLevel = self:GetWidget("Text_myLevel")
	self.Text_myLevel:SetText("")

	--我的排名数值	
	self.Text_myRankNum = self:GetWidget("Text_myRankNum")
	self.Text_myRankNum:SetText("")

	--我的等级数值
	self.Text_myLevelNum = self:GetWidget("Text_myLevelNum")
	self.Text_myLevelNum:SetText("")

  	--选项卡
  	--选项卡选中标记
  	self.Image_selectedtab1 = self:GetWidget("Image_selectedtab1")
  	self.Image_selectedtab1:SetVisible(true)

	local Panel_tab = self:GetWidget("Panel_tab")

  	self.__tab:On(1)      --默认显示第一个选项卡的
  	Panel_tab:AddChild(self.__tab:GetNode())
end

function Modules.DivinationRankView:__ShowUI()

	--头像切圆
	local headIconBg = Util:CircleClippingNode(0,0,40)
	self.Panel_headIcon:AddChild(headIconBg)
	PosTool.Center(headIconBg)
	--头像
	self.headIcon = Util:Sprite(" ")
	headIconBg:AddChild(self.headIcon)
	self.headIcon:SetScaleFactor(1.2)
	PosTool.Center(self.headIcon)	

	--收藏值
	Util:WidgetLabel(self.Text_level , CPPGameLib.GetString("divination_collectValue").. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal,Macros.Color.tangerine_color)
	self.Text_level:SetAnchorPoint( 0 , 0.5 )
	self.Text_level:SetPosition( 60 , self.Text_level:GetPositionY() )

	self.Text_levelNum:SetAnchorPoint( 0 , 0.5 )
	PosTool.RightTo( self.Text_level , self.Text_levelNum )

	--公会
	Util:WidgetLabel(self.Text_union , CPPGameLib.GetString("rank_union") , Macros.TypefaceSize.normal,Macros.Color.tangerine_color)
	self.Text_union:SetAnchorPoint( 0 , 0.5 )
	self.Text_union:SetPosition( 60 , self.Text_union:GetPositionY() )

	self.Text_unionName:SetAnchorPoint( 0 , 0.5 )
	PosTool.RightTo( self.Text_union , self.Text_unionName )

	--我的排名
	Util:WidgetLabel(self.Text_myRank , CPPGameLib.GetString("rank_myRank") , Macros.TypefaceSize.normal,Macros.Color.keypoint)
	self.Text_myRank:SetAnchorPoint( 0 , 0.5 )

	self.Text_myRankNum:SetAnchorPoint( 0 , 0.5 )
	PosTool.RightTo( self.Text_myRank , self.Text_myRankNum )

	--我的收藏值
	Util:WidgetLabel(self.Text_myLevel , CPPGameLib.GetString("divination_myCollectionValue") , Macros.TypefaceSize.normal,Macros.Color.keypoint)
	self.Text_myLevel:SetAnchorPoint( 0 , 0.5 )
	self.Text_myLevel:SetPosition( self.Text_myLevel:GetPositionX() - 60 , self.Text_myLevel:GetPositionY() )

	self.Text_myLevelNum:SetAnchorPoint( 0 , 0.5 )
	PosTool.RightTo( self.Text_myLevel , self.Text_myLevelNum )

	local params = {
  		item_class	= Modules.TemplDivinationRankCell,
	    item_width	= 616,
	    item_height	= 122,
	    row	= 4,
	    col	= 1,
	    item_space_r = 5,
	    view_width = 616,
	    view_height = 430,
  	}
  	self.__panel_list = GUI.ScrollView.New(params)
  	self.Panel_level:AddChild( self.__panel_list:GetNode() )
  	PosTool.Center( self.__panel_list )
end	

--数据
function Modules.DivinationRankView:__Server()
	CallModuleFunc(ModuleType.DIVINATION , "DivinationRankListRequest" )
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_RANK , function ( _ , data)
    	self:__SetShowUI()
    end)
end

function Modules.DivinationRankView:__SetShowUI()
	--获得排行榜第一信息
	local no1Info = CallModuleFunc(ModuleType.DIVINATION , "GetRankNO1" )
	CPPGameLib.PrintTable("xy" , no1Info , "no1Info")  
	if no1Info.head_icon then
		--设置头像
		self.headIcon:SetImage( Resource.PathTool.GetRoleHeadPath( no1Info.head_icon ) )
		--设置名字
		Util:WidgetLabel(self.Text_playerName, no1Info.role_name ,Macros.TypefaceSize.normal,Macros.Color.golden)
		--收藏值
		Util:WidgetLabel(self.Text_levelNum, no1Info.context )
		--公会
		local guild_name = CPPGameLib.GetString("rank_haveNoUnion")
		if no1Info.guild_name ~= "" then
			guild_name = no1Info.guild_name
		end

		Util:WidgetLabel(self.Text_unionName, guild_name )
	end
	
	--获得我的排行榜信息
	local myInfo = CallModuleFunc(ModuleType.DIVINATION , "GetMyRankInfo" )
	if myInfo.rank and myInfo.rank ~= 0 then
		--排名
		Util:WidgetLabel(self.Text_myRankNum, myInfo.rank , Macros.TypefaceSize.normal,Macros.Color.content)
	else
		--排名
		Util:WidgetLabel(self.Text_myRankNum, CPPGameLib.GetString("rank_reflection") , Macros.TypefaceSize.normal,Macros.Color.content)
	end

	--收藏值
	Util:WidgetLabel(self.Text_myLevelNum, myInfo.rank_value , Macros.TypefaceSize.normal,Macros.Color.content)

	--设置列表
	local rankLst = CallModuleFunc(ModuleType.DIVINATION , "GetRankLst" )
	CPPGameLib.PrintTable("xy" , rankLst , "rankLst") 
	self.__panel_list:SetDataList( rankLst )
end

--详细信息
function Modules.DivinationRankView:OnButtonWatchDetail()
	--获得排行榜第一信息
	local no1Info = CallModuleFunc(ModuleType.DIVINATION , "GetRankNO1" )
	if no1Info.role_id then
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , no1Info )
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_notNo1"))
	end
end





Modules = Modules or {}

Modules.TemplDivinationRankCell = Modules.TemplDivinationRankCell or BaseClass(GUI.ScrollItem)

function Modules.TemplDivinationRankCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.RANK_VIEW_CELL)
end

function Modules.TemplDivinationRankCell:__delete()

end

function Modules.TemplDivinationRankCell:SetData(data)	
	GUI.ScrollItem.SetData(self, data)
	--排名图标
	local Image_rank = self:GetWidget("Image_rank")
	--星星图标
	local Image_star = self:GetWidget("Image_star")
	Image_star:SetVisible(self.__data.rankType == Macros.Game.RankType.MAIN_DUNGEON_STAR)
	--角色图标
	local Panel_roleicon = self:GetWidget("Panel_roleicon")
	local icon = LogicGUI.RoleIcon.New(true)
	Panel_roleicon:AddChild(icon:GetNode())
	icon:SetTouchCallback(function()
		-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
    end)
	icon:SetData(data.head_icon)
	icon:adaptParentContentSize()
	PosTool.Center(icon)

	--v字图标
	if self.__data.vip_level > 0 then 
	    self.v_icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true ))
	    self.v_icon:SetScaleFactor(0.8)
	   	self.v_icon:SetAnchorPoint(0,0)
	   	icon:GetNode():AddChild(self.v_icon,100)
	   	self.v_icon:SetPosition(-2,-6)
	    --vip等级
	    self.power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true))
	    self.power_font:SetSpan(-3)
	    self.power_font:SetText(tostring(self.__data.vip_level))
	    self.power_font:SetAnchorPoint(0,0)
	   	icon:GetNode():AddChild(self.power_font:GetNode(),100)
	   	self.power_font:SetPosition(50,-6)
   end

--排名数字
	local Text_rank = self:GetWidget("Text_rank")
	Util:WidgetLabel(Text_rank,"", 26, Macros.Color.headline )
	if data.rank <=3 then 
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		Image_rank:SetImage(imageData)
		Text_rank:SetVisible(false)
	else
		Image_rank:SetVisible(false)
		Text_rank:SetText(data.rank)
	end
	--公会
	local str = data.rankType ~= Macros.Game.RankType.MAIN_DUNGEON_STAR and CPPGameLib.GetString("rank_union") or CPPGameLib.GetString("rank_typeText1")
	local text_union = self:GetWidget("Text_union")
	Util:WidgetLabel(text_union,str, Macros.TypefaceSize.normal , Macros.Color.keypoint)
	--公会名字
	str = data.rankType ~= Macros.Game.RankType.MAIN_DUNGEON_STAR and (data.guild_name ~= "" and data.guild_name or CPPGameLib.GetString("rank_haveNoUnion")) or data.battle_value
	local text_unionname = self:GetWidget("Text_unionname")
	Util:WidgetLabel(text_unionname,str, Macros.TypefaceSize.normal , Macros.Color.content)
	--玩家名字
	Util:WidgetLabel(self:GetWidget("Text_playerName"), data.role_name, Macros.TypefaceSize.normal , Macros.Color.headline )
	--类型
	Util:WidgetLabel(self:GetWidget("Text_protype"), CPPGameLib.GetString("divination_collectValue"), Macros.TypefaceSize.normal , Macros.Color.headline )	
	
	--当前类型数值
	local pro_value = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true))
    pro_value:SetAnchorPoint( 0.5 , 0.5 )  
    self:GetWidget("Image_5"):AddChild(pro_value:GetNode())	
    PosTool.Center( pro_value )
    pro_value:SetText(tostring(data.context))
    
	--另一种类型属性
	Util:WidgetLabel(self:GetWidget("Text_anotherprotype"), CPPGameLib.GetString("rank_typeText2"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
	--另一种类型数值
	Util:WidgetLabel(self:GetWidget("Text_protypevalue"), data.level, Macros.TypefaceSize.normal , Macros.Color.content)		
end

