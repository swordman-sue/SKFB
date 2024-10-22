--
-- @brief 公会主界面
-- @author: yjg
-- @date: 2016年10月14日10:39:01
--

Modules = Modules or {}

Modules.UnionView = Modules.UnionView or BaseClass(BaseView)

function Modules.UnionView:__init()
	self.__layout_name = "union"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_union")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("union"),
		close_func = function()
			print("xy" , "=======function===111")
			self:CloseReturnCity()
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

-- Macros.Game
-- 	--公会红点
-- 	UNION_RED = {
-- 		AUDIT = 0,      	--入会申请列表有新的申请显示红点
-- 		DESENO_SHOP = 1 , 	--限时商店刷新后
-- 		AWARD = 2 ,			--奖励商店解锁新的物品购买时
-- 		CONSTRUCTION = 3 ,	--军团有可剩余建设次数时
-- 		MESSAGE = 4 , 		--留言板内有新的留言
-- 		KILLS = 5 , 		--军团技能建筑中

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION,true)


   	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION_COMBAT)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION_COMBAT_BATTLEGROUND)
end

function Modules.UnionView:__OpenCallback()  
	self:__Getwidget()
	self:__ShowUI() 
	self:__Server() 
	self:__Effects()
end


function Modules.UnionView:__Dispose()
	for i = 1 , #self.skeletons do
		if self.skeletons[i] then
			self:__DelEffect(self.skeletons[i])
			self.skeletons[i] = nil
		end
	end
end

--数据
function Modules.UnionView:__Server()
	
	CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "union" )
	--公会信息
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_DATE , function (_, data)
    	local unionData = CallModuleFunc(ModuleType.UNION , "UnionData")
    	if unionData then
	    	if self.unionJoin then
	    		self.unionJoin:DeleteMe()
	    		self.unionJoin = nil
	    	end
	    	self:__SetShowUI()
    	else
    		self.unionJoin = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.UNION_JOIN , false , function( ... )
    			self:CloseManual()
    		end )
    	end
    	self:SetRed()

		self:OpenLevel( )
    end)
end

function Modules.UnionView:__Getwidget( ... )
	
	self.Panel_tou = {}
	self.Image_icon = {}
	self.Image_noOpen = {}

	for i = 1 , 6 do
		self.Image_icon[i] = self:GetWidget("Image_icon"..i)
		self.Panel_tou[i] = self:GetWidget("Panel_tou"..i)
		-- self.Panel_tou[i]:SetTouchEnabled(false)

		self.Image_noOpen[i] = self:GetWidget("Image_noOpen"..i)
		self.Image_noOpen[i]:SetVisible(false)

		local cnf = CPPGameLib.GetConfig( "GuildBuilding", i )
		local union_openLevel = Util:Name( CPPGameLib.GetString("union_openLevel" , cnf.need_guild_level ) , Macros.TypefaceSize.slightly , Macros.Game.QualityType.RED )
		union_openLevel:SetAnchorPoint( 0 , 0.5 )
		self.Image_noOpen[i]:AddChild( union_openLevel )
		PosTool.Center( union_openLevel , -40 , - 2)
	end

	-- 黑底
	self.Image_property = self:GetWidget("Image_property")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--进度条
	self.Panel_bar = self:GetWidget("Panel_bar")
	--留言
	self.Button_leaveword = Util:WidgetButton( 
                                            self:GetWidget("Button_leaveword") , 
                                            "" , 
                                            CPPGameLib.Handler(self,self.OnButtonLeaveword) ,
                                            nil , nil , Macros.Color.button_yellow 
                                        )

	self.leavewordRed = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point") )
	self.leavewordRed:SetAnchorPoint( 0.5 , 0.5 )
	self.leavewordRed:SetPosition( self.Button_leaveword:GetContentWidth() - 10 , self.Button_leaveword:GetContentHeight() - 10 )
	self.leavewordRed:SetVisible( false )
	self.Button_leaveword:AddChild( self.leavewordRed )
	
	self.union_message = Util:Name( CPPGameLib.GetString("union_message") )
	self.union_message:SetPosition( self.Button_leaveword:GetContentWidth()/2 , 0 )
	self.union_message:SetAnchorPoint( 0.5 , 0.5 )
	self.Button_leaveword:AddChild( self.union_message )

	--排行榜
	self.Button_rank = Util:WidgetButton( 
                                            self:GetWidget("Button_rank") , 
                                            "" , 
                                            CPPGameLib.Handler(self,self.OnButtonRank) ,
                                            nil , nil , Macros.Color.button_yellow 
                                        )
	self.Button_rank:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_rank"), TextureResType.LOCAL )

	self.rank_union_rank = Util:Name( CPPGameLib.GetString("rank_union_rank") )
	self.rank_union_rank:SetPosition( self.Button_rank:GetContentWidth()/2 , 0 )
	self.rank_union_rank:SetAnchorPoint( 0.5 , 0.5 )
	self.Button_rank:AddChild( self.rank_union_rank )
end

function Modules.UnionView:__ShowUI( ... )
	self.__UNION_LST = {
			[1] = { pos = { x = 150 , y = 140 } , skeletons = "UI_union_icon_3" , icon = "icon_construction" , name = "union_construction" , func = CPPGameLib.Handler(self,self.OnConstruction) , type = Macros.Game.UNION_RED.CONSTRUCTION },
			[2] = { pos = { x = 150 , y = 140 } , skeletons = "UI_union_icon_4" , icon = "icon_applyFor" , name = "union_discuss" , func = CPPGameLib.Handler(self,self.OnDiscuss) , type = Macros.Game.UNION_RED.AUDIT },
			[3] = { pos = { x = 150 , y = 140 } , skeletons = "UI_union_icon_1" , icon = "icon_attack" , name = "union_dungeon" , func = CPPGameLib.Handler(self,self.OnDungeon) , type = Macros.Game.UNION_RED.TRANSCRIPT},
			[4] = { pos = { x = 150 , y = 150 } , skeletons = "UI_union_icon_5" , icon = "icon_award" , name = "union_shop" , func = CPPGameLib.Handler(self,self.OnShop) , type = Macros.Game.UNION_RED.AWARD },
			[5] = { pos = { x = 150 , y = 140 } , skeletons = "UI_union_icon_2" , icon = "icon_study" , name = "union_skill" , func = CPPGameLib.Handler(self,self.OnSkill) , type = Macros.Game.UNION_RED.KILLS },
			[6] = { pos = { x = 150 , y = 140 } , skeletons = "UI_union_icon_6" , icon = "icon_attack" , name = "union_transcript" , func = CPPGameLib.Handler(self,self.OnTranscript) , type = Macros.Game.UNION_RED.TRANSCRIPT },
	}

	self.redLst = {}
	self.redLst.red = {}
	self.skeletons = {}
	for i = 1 , #self.__UNION_LST do

		local effect_root = nil
		self.skeletons[i] , effect_root = self:__CreateEffect({res_name = self.__UNION_LST[i].skeletons , type = Effect.EffectType.SKELETON } , self.Image_icon[i] , -1 )
		PosTool.CenterBottom(effect_root)

		local cnf = CPPGameLib.GetConfig( "GuildBuilding", i , false )
		if cnf.is_show and cnf.is_show == 0 then
			self.Image_icon[i]:SetVisible(false)
			self.Panel_tou[i]:SetVisible(false)
		end

		--名字
		local name = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , self.__UNION_LST[i].name , true ) )
		name:SetAnchorPoint( 0.5 , 0.5 )
		name:SetPosition( self.Image_icon[i]:GetContentWidth()/2 , 20 )
		self.Image_icon[i]:AddChild( name )

		self.Image_noOpen[i]:SetPosition( name:GetPositionX(), self.Image_noOpen[i]:GetPositionY() )

		--触摸
		WidgetUnity.SetWidgetListener(self.Panel_tou[i], nil, nil, function ( ... )
			self.__UNION_LST[i].func()
		end)

		if self.__UNION_LST[i].type then
			self.redLst.red[self.__UNION_LST[i].type] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon_float") )
			self.redLst.red[self.__UNION_LST[i].type]:SetAnchorPoint( 0.5 , 0.5 )
			self.redLst.red[self.__UNION_LST[i].type]:SetPosition( self.__UNION_LST[i].pos.x , self.__UNION_LST[i].pos.y )
			self.redLst.red[self.__UNION_LST[i].type]:SetVisible( false )
			name:AddChild( self.redLst.red[self.__UNION_LST[i].type] )

			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, self.__UNION_LST[i].icon , true )
			local icon = Util:Sprite(sp)
			icon:SetAnchorPoint(0.5,0.5)
			self.redLst.red[self.__UNION_LST[i].type]:AddChild( icon )
			PosTool.Center(icon)

			ActionManager.GetInstance():UpDownMoveAnimation( self.redLst.red[self.__UNION_LST[i].type] , 1 , 15)
		end

		if self.__UNION_LST[i].type == Macros.Game.UNION_RED.DESENO_SHOP then
			-- self.redLst.red[Macros.Game.UNION_RED.AWARD] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon_float") )
			-- self.redLst.red[Macros.Game.UNION_RED.AWARD]:SetAnchorPoint( 0.5 , 0.5 )
			-- self.redLst.red[Macros.Game.UNION_RED.AWARD]:SetPosition( 225 , 90 )
			-- self.redLst.red[Macros.Game.UNION_RED.AWARD]:SetVisible( false )
			-- name:AddChild( self.redLst.red[Macros.Game.UNION_RED.AWARD] )

			-- local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "icon_award" , true )
			-- local icon = Util:Sprite(sp)
			-- icon:SetAnchorPoint(0.5,0.5)
			-- self.redLst.red[Macros.Game.UNION_RED.AWARD]:AddChild( icon )
			-- PosTool.Center(icon)

			-- ActionManager.GetInstance():UpDownMoveAnimation( self.redLst.red[Macros.Game.UNION_RED.AWARD] , 1 , 15)

		elseif self.__UNION_LST[i].type == Macros.Game.UNION_RED.TRANSCRIPT then

			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon_float") )
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD]:SetAnchorPoint( 0.5 , 0.5 )
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD]:SetPosition( 225 , 90 )
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD]:SetVisible( false )
			name:AddChild( self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD] )

			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "icon_award" , true )
			local icon = Util:Sprite(sp)
			icon:SetAnchorPoint(0.5,0.5)
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD]:AddChild( icon )
			PosTool.Center(icon)

			ActionManager.GetInstance():UpDownMoveAnimation( self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_AWARD] , 1 , 15)

		elseif self.__UNION_LST[i].type == Macros.Game.UNION_RED.CONSTRUCTION then

			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon_float") )
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD]:SetAnchorPoint( 0.5 , 0.5 )
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD]:SetPosition( 225 , 90 )
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD]:SetVisible( false )
			name:AddChild( self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD] )

			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "icon_award" , true )
			local icon = Util:Sprite(sp)
			icon:SetAnchorPoint(0.5,0.5)
			self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD]:AddChild( icon )
			PosTool.Center(icon)

			ActionManager.GetInstance():UpDownMoveAnimation( self.redLst.red[Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD] , 1 , 15)

		end
	end

	--经验
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_preview")
	local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_interior_bg1")
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Panel_bar:GetContentWidth() -5  , self.Panel_bar:GetContentHeight() - 5 , self.Panel_bar:GetContentWidth() , self.Panel_bar:GetContentHeight(), 100 , 100 )
    self.__bar_exp:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_bar:AddChild(self.__bar_exp:GetNode())
	PosTool.Center( self.__bar_exp )

	--进度
	self.plan = Util:Label( " " , Macros.TypefaceSize.normal , Macros.Color.white )
	self.plan:SetAnchorPoint( 0.5,0.5 )
	self.plan:SetPosition( self.Panel_bar:GetContentWidth()/2 , (self.Panel_bar:GetContentHeight()/2) - 1 )
	self.Panel_bar:AddChild(self.plan)

	--名字
	self.name = Util:Label(" " , Macros.TypefaceSize.normal , Macros.Color.white )
	self.name:SetAnchorPoint( 0 , 0.5 )
	self.name:SetPosition( self.Panel_bar:GetPositionX() , self.Image_property:GetContentHeight() - self.name:GetContentHeight() - 2 )
	self.Image_property:AddChild( self.name )

	--等级
	self.level = Util:Label(" " , Macros.TypefaceSize.normal , Macros.Color.white )
	self.level:SetAnchorPoint( 1 , 0.5 )
	self.level:SetPosition( self.Panel_bar:GetPositionX() + self.Panel_bar:GetContentWidth() , self.name:GetPositionY() )
	self.Image_property:AddChild( self.level )
end

--特效
function Modules.UnionView:__Effects( ... )
	--宝石
	self.UI_juntuanbaoshi , effect_root = self:__CreateEffect({res_name = "UI_juntuanbaoshi" } , self.Image_icon[4] , -1)
	PosTool.Center(effect_root , 66 , 9)

	--水池
	self.UI_juntuanshuichi , effect_root = self:__CreateEffect({res_name = "UI_juntuanshuichi" } , self.Image_icon[3] , -1)
	PosTool.Center(effect_root , 14 , -5 )

	--瀑布
	self.UI_juntuanshuijing , effect_root = self:__CreateEffect({res_name = "UI_juntuanshuijing" } , self.Image_icon[5] , -1)
	PosTool.Center(effect_root , 0 , 80 )

	--瀑布
	self.UI_juntuanchangjing , effect_root = self:__CreateEffect({res_name = "UI_juntuanchangjing" } , self.__root_widget , -1)
	PosTool.Center(effect_root , -513 , 95 )
end




function Modules.UnionView:__SetShowUI( ... )
	--图标
	local iconSp = "union_icon" .. CallModuleFunc(ModuleType.UNION , "GetUnionIcon")
	self.Panel_icon:SetBackGroundImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , iconSp) )
	self.Panel_icon:SetBackGroundImageScale9Enabled(true)
	self.Panel_icon:SetBackGroundImageCapInsets( 70 , 70 , 0 , 0 )

	--名字
	self.name:SetString( CallModuleFunc(ModuleType.UNION , "GetUnionName") )
	--等级
	self.level:SetString( CPPGameLib.GetString("common_lev9" , CallModuleFunc(ModuleType.UNION , "GetUnionLevel")))
	--个人贡献
	if self.union_myContribute then
		self.union_myContribute:RemoveSelf(true)
		self.union_myContribute = nil
	end

	local tab = {
					{ zi = CPPGameLib.GetString("union_myContribute") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
					{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.GUILD_CONTRIBUTION) , dx = 0.8 },
					{ zi = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GUILD_CONTRIBUTION) , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
				}
	self.union_myContribute = Util:LabCenter(tab)
	self.union_myContribute:SetAnchorPoint(0.5,0.5)
	self.union_myContribute:SetPosition( self.Panel_bar:GetPositionX() + (self.Panel_bar:GetContentWidth()/2) , 0 + self.union_myContribute:GetContentHeight() - 5 )
	self.Image_property:AddChild( self.union_myContribute )

	--进度条
    local exp , expMax = CallModuleFunc(ModuleType.UNION , "GetUnionExp")
    self.__bar_exp:SetProgress( (exp/expMax) * 100 )

    self.plan:SetString( exp .. "/" .. expMax )
end

--公会大厅
function Modules.UnionView:OnDiscuss( ... )
	print("xy" , "公会大厅" )
	CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_HALL)
end

--留言板
function Modules.UnionView:OnButtonLeaveword( ... )
	self.leavewordRed:SetVisible( false )
	LogicGUI.TemplUnionLeaveWord.New()
end

--训练所
function Modules.UnionView:OnSkill( ... )
	CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_SKILL)
end

--奇迹之泉
function Modules.UnionView:OnConstruction( ... )
	LogicGUI.TemplUnionContribute.New()
end

--公会商店
function Modules.UnionView:OnShop( ... )
	CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_SHOP)
end

--排行榜
function Modules.UnionView:OnButtonRank( ... )
	LogicGUI.TemplUnionRank.New()
end

--军团副本
function Modules.UnionView:OnDungeon( ... )
	CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_TRANSCRIPT)
end

--工会战
function Modules.UnionView:OnTranscript( ... )
	CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_COMBAT )
end

--红点设置
function Modules.UnionView:SetRed( ... )
	local lst = CallModuleFunc(ModuleType.UNION, "GetUnionRedLst" )
	--重置
	local t1 = {}
	for i = 1 , #self.__UNION_LST do
		if self.redLst.red[ self.__UNION_LST[i].type ] then
			self.redLst.red[ self.__UNION_LST[i].type ]:SetVisible( false )
		end
		t1[ self.__UNION_LST[i].type ] = i
	end
	

	local unionLevel = CallModuleFunc(ModuleType.UNION , "GetUnionLevel")

	--设置
	for k,v in pairs( lst ) do
		local cnf = CPPGameLib.GetConfig( "GuildBuilding", t1[ k ] , false )
		if self.redLst.red[k] then
			if cnf and cnf.need_guild_level > unionLevel then
				self.redLst.red[k]:SetVisible( false )
			else
				self.redLst.red[k]:SetVisible( true )
			end
		end
	end	

	--留言
	if lst[ Macros.Game.UNION_RED.MESSAGE ] then
		self.leavewordRed:SetVisible( true )
	end

	--商店的第二个红点 如果有限时
	-- if lst[Macros.Game.UNION_RED.DESENO_SHOP] then
	-- 	local cnf = CPPGameLib.GetConfig( "GuildBuilding", 4 , false )
	-- 	if cnf and cnf.need_guild_level <= unionLevel then
	-- 		self.redLst.red[ Macros.Game.UNION_RED.DESENO_SHOP ]:SetVisible( true )
	-- 	else
	-- 		self.redLst.red[ Macros.Game.UNION_RED.DESENO_SHOP ]:SetVisible( false )
	-- 	end
	-- end

	if lst[Macros.Game.UNION_RED.AWARD] then
		local cnf = CPPGameLib.GetConfig( "GuildBuilding", 4 , false )
		if cnf and cnf.need_guild_level <= unionLevel then
			self.redLst.red[ Macros.Game.UNION_RED.AWARD ]:SetVisible( true )
		else
			self.redLst.red[ Macros.Game.UNION_RED.AWARD ]:SetVisible( false )
		end
	end

	--公会副本的第二个红点 如果有奖励
    --时间类型
    local timeType = CallModuleFunc(ModuleType.UNION , "GetStartTime")
	if lst[Macros.Game.UNION_RED.TRANSCRIPT_AWARD] then
		if timeType ~= false then
			local cnf = CPPGameLib.GetConfig( "GuildBuilding", 3 , false )
			if cnf and cnf.need_guild_level <= unionLevel then
				self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_AWARD ]:SetVisible( true )
			else
				self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_AWARD ]:SetVisible( false )
			end
		else
			self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_AWARD ]:SetVisible( false )
		end
	elseif lst[Macros.Game.UNION_RED.TRANSCRIPT_LEVLE_AWARD] then
		if timeType ~= false then
			local cnf = CPPGameLib.GetConfig( "GuildBuilding", 3 , false )
			if cnf and cnf.need_guild_level <= unionLevel then
				self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_AWARD ]:SetVisible( true )
			else
				self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_AWARD ]:SetVisible( false )
			end
		else
			self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_AWARD ]:SetVisible( false )
		end
	elseif lst[Macros.Game.UNION_RED.TRANSCRIPT] then
        if timeType == false then
        	self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT ]:SetVisible( false )
        end
	end

	--公会建设的第二个红点 如果有奖励
	local BoxNum = 0
	local cnf = CallModuleFunc(ModuleType.UNION , "GetUnionScheduleCnf")
	local schedule = CallModuleFunc(ModuleType.UNION , "GetUnionSchedule")
	for i , v in ipairs( cnf ) do
		local type_ = CallModuleFunc(ModuleType.UNION, "GetBoxType" , v.qs )
		if not type_ and schedule >= v.qs then
			BoxNum = BoxNum + 1
		end
	end

	if BoxNum ~= 0 then
		self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD ]:SetVisible( true )
	else
		self.redLst.red[ Macros.Game.UNION_RED.TRANSCRIPT_CONSTRUCTION_AWARD ]:SetVisible( false )
	end

end


--开启等级
function Modules.UnionView:OpenLevel( )
	local unionLevel = CallModuleFunc(ModuleType.UNION , "GetUnionLevel")
	for i = 1 , #self.__UNION_LST do
		local cnf = CPPGameLib.GetConfig( "GuildBuilding", i )
		if unionLevel < cnf.need_guild_level then
			self.Image_noOpen[i]:SetVisible(true)
			self.Panel_tou[i]:SetTouchEnabled(false)
		else
			self.Image_noOpen[i]:SetVisible(false)
			self.Panel_tou[i]:SetTouchEnabled(true)
		end
	end
end
