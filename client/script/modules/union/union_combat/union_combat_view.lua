--
-- @brief 公会主界面
-- @author: yjg
-- @date: 2016年10月14日10:39:01
--

Modules = Modules or {}

Modules.UnionCombatView = Modules.UnionCombatView or BaseClass(BaseView)

function Modules.UnionCombatView:__init()
    self.__layout_name = ""  
    self.__layout_mode = BaseView.LayoutMode.CODE 
	self.__bg_params = {type = BaseView.BGType.SOLID,color = {0,0,0,100}}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("union_combat"),
		close_func = function()
			self:CloseManual()
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION_STAR)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION_COMBAT)
end

function Modules.UnionCombatView:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.widget
end

function Modules.UnionCombatView:__OpenCallback()  
	self:__Getwidget3D()
	self:__ShowUI() 
	self:__Server() 
	self:__Effects()
end

function Modules.UnionCombatView:__Dispose()
end

--数据
function Modules.UnionCombatView:__Server()
	--请求角色数据
	CallModuleFunc(ModuleType.UNION , "GuildWarRoleDataReq")
	--请求宣战对象
	CallModuleFunc(ModuleType.UNION , "GuildWarDeclareWarStarListReq")
	--请求星球列表响应
	CallModuleFunc(ModuleType.UNION , "GuildWarStarListReq")
	
	--刷新左边UI
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_OCCUPY , function (_, data)
		self:SetShowLeftUI()
		self:__ShowTime()
    end)

	--刷新星球
    self:BindGlobalEvent( Macros.Event.UnionModule.UNION_STAR_LST , function (_, data)
    	for i , v in ipairs( self.star ) do
    		v:SetUnionName( )
    		v:SetCombatType( )
    	end
    end)

    --奖励列表刷新
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_OCCUPY_AWARD , function (_, data)
    	if self.lst_list then
			self.lst_list:ItemIter(function (index, item)
				if item then
					item:__SetRed()
				end
			end)
		end
    end) 
end

--3D状态
function Modules.UnionCombatView:__Getwidget3D( ... )
	--背景
	local degree = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 14 )
	local scale = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 19 )

	local bg = Util:Sprite(Resource.PathTool.GetUIResPath("bigbg/bg_sp_union_combat"))
	bg:SetAnchorPoint(0.5, 0.5)
	bg:setRotation3D(COCOPoint3D(-degree, 0, 0))
	self.widget:AddChild(bg)
	PosTool.Center(bg)
	bg:SetScaleFactor( scale )

	self.wrench = Util:Layout( bg:GetContentWidth() , bg:GetContentHeight() )
	self.wrench:SetAnchorPoint( 0.5 , 0.5 )
	self.wrench:setRotation3D(COCOPoint3D( degree , 0, 0))
	bg:AddChild(self.wrench)
	PosTool.Center(self.wrench)

	--创建星球
	self:__ShowUIStar()

	local org_pos = bg:GetPosition3D()
	bg:SetPosition3D(COCOPoint3D(org_pos.x, org_pos.y, org_pos.z))

	local org_pos_1 = self.wrench:GetPosition3D()
	self.wrench:SetPosition3D(COCOPoint3D(org_pos_1.x, org_pos_1.y, org_pos_1.z))

	local layout_touch = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	layout_touch:SetTouchEnabled(true)
	layout_touch:SetTouchSwallow(false)
	layout_touch:SetAnchorPoint(0.5, 0.5)
	self.widget:AddChild(layout_touch)
	PosTool.Center(layout_touch)	

	local offset_x_alias = 0
	local offset_y_alias = 0

	local up = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 17 )
	local below = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 18 )
	local left = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 15 )
	local right = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 16 )

	local radians = (90 - degree)/180*math.pi
	local move_vec = COCOPoint3D(0, math.sin(radians), -math.cos(radians))

	local posSign = false

	local touch_begin_x, touch_begin_y 

		WidgetUnity.SetWidgetListener(layout_touch, 
			function(x, y)
			touch_begin_x = x
			touch_begin_y = y
		end,
		function(x, y)
			---------------------------------------
			-- 拖动
			---------------------------------------
			local pos , offset_pos , touch_pos = CallModuleFunc(ModuleType.UNION , "GetSaveLetGoPos" )
			if pos and posSign == false then
				posSign = true
				offset_x_alias = offset_pos.x
				offset_y_alias = offset_pos.y
				touch_begin_x = touch_pos.x
				touch_begin_y = touch_pos.y
			end

			offset_x_alias = offset_x_alias + (x - touch_begin_x)
			offset_y_alias = offset_y_alias + (y - touch_begin_y)

			if offset_x_alias >= (left * scale) then
				offset_x_alias = (left * scale)
			elseif offset_x_alias <= ((right) * scale) then
				offset_x_alias = ((right) * scale)
			end

			if offset_y_alias <= ((up) * scale) then
				offset_y_alias  = ((up) * scale)
			elseif offset_y_alias >= below * scale then
				offset_y_alias = below * scale
			end

			touch_begin_x = x
			touch_begin_y = y

			local now_pos = COCOPoint3D(0, 0, 0)
			now_pos.x = org_pos.x + offset_x_alias
			now_pos.y = org_pos.y + offset_y_alias * move_vec.y
			now_pos.z = org_pos.z + offset_y_alias * move_vec.z

			local offset = {}
			offset.x = offset_x_alias
			offset.y = offset_y_alias
			local touch = {}
			touch.x = touch_begin_x
			touch.y = touch_begin_y

			CallModuleFunc(ModuleType.UNION , "SetSaveLetGoPos" , now_pos , offset , touch )
			local pos , offset_pos , touch_pos = CallModuleFunc(ModuleType.UNION , "GetSaveLetGoPos" )
			bg:SetPosition3D( pos )
			CallModuleFunc(ModuleType.UNION , "SetIsCanTouchStar" , false )
			-- ---------------------------------------
			-- --位置重设
			-- ---------------------------------------
			self:__SetShowUIStar()
		end,
		function( x , y )			
			CPPActionManager.DelayTo(self.wrench , 0.02 , GlobalCallbackMgr:AddCallBackFunc( function( ... )
				CallModuleFunc(ModuleType.UNION , "SetIsCanTouchStar" , true )
			end ) )
		end)

	--设置初始位置
	local now_pos = CallModuleFunc(ModuleType.UNION , "GetSaveLetGoPos" )
	if unpack(self.__open_data) and now_pos then
		CPPGameLib.PrintTable("xy" , now_pos , "now_pos")  
		bg:SetPosition3D(now_pos)
	end

	--设置左边UI
	self:SetLeftUI()
end

function Modules.UnionCombatView:__ShowUI( ... )

	--商店
	local sp = Resource.PathTool.GetSystemEntryIconPath("btn_store")
	local shop = Util:Button( sp , function( ... )
		CallModuleFunc(ModuleType.RESOURCE_WAR, "OpenView", BaseViewType.RESOURCE_SHOP )
	end )
	shop:SetAnchorPoint( 0.5 , 0.5 )
	shop:SetPosition( self.widget:GetContentWidth() - 80 , 80 )
	self.widget:AddChild( shop )
	shop:SetScaleFactor( 0.9 )

	local union_combat_shop = Util:Name( CPPGameLib.GetString("union_combat_shop") )
	union_combat_shop:SetPosition( shop:GetContentWidth()/2 , 10 )
	union_combat_shop:SetAnchorPoint( 0.5 , 0.5 )
	shop:AddChild( union_combat_shop )

	--一键领取
	local sp = Resource.PathTool.GetSystemEntryIconPath("btn_cumulative_recharge")
	local oneKeyGet = Util:Button( sp , function( ... )
	    local timeCnf = CallModuleFunc( ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )
		for i , v in ipairs( timeCnf ) do
			if v == "duiZhan" then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_unableGetAward") )
				return 
			end
		end

		local lst = CallModuleFunc(ModuleType.UNION , "GetUnionCombatOccupy")
		local info = {}
		info.take_type = 3
		info.take_star_list = {}
		for i , v in ipairs( lst ) do
			if v.is_have_reward == 1 then
				table.insert( info.take_star_list , { star_id = v.id } )
			end
		end

		if #info.take_star_list <= 0 then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_notGetAward") )
			return 
		end
		CallModuleFunc(ModuleType.UNION , "GuildWarTakeStarRewardReq" , info )
	end )
	oneKeyGet:SetAnchorPoint( 0.5 , 0.5 )
	oneKeyGet:SetPosition( shop:GetPositionX() - 100 , 80 )
	self.widget:AddChild( oneKeyGet )
	oneKeyGet:SetScaleFactor( 0.9 )

	local union_combat_oneKeyGet = Util:Name( CPPGameLib.GetString("union_combat_oneKeyGet") )
	union_combat_oneKeyGet:SetPosition( oneKeyGet:GetContentWidth()/2 , 10 )
	union_combat_oneKeyGet:SetAnchorPoint( 0.5 , 0.5 )
	oneKeyGet:AddChild( union_combat_oneKeyGet )

	--规则
	--一键领取
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_rule")
	local btn_help = Util:Button( sp , function( ... )
        local help = LogicGUI.TemplRule.New()
        help:SetLoadedCallback( function ()
            help:SetCell("Rule_UnionCombat")
        end )
	end )
	btn_help:SetAnchorPoint( 0.5 , 0.5 )
	btn_help:SetPosition( oneKeyGet:GetPositionX() - 100 , 80 )
	self.widget:AddChild( btn_help )
	btn_help:SetScaleFactor( 0.9 )

	local label = Util:Name( CPPGameLib.GetString("team_btn_help") )
	label:SetPosition( oneKeyGet:GetContentWidth()/2 , 10 )
	label:SetAnchorPoint( 0.5 , 0.5 )
	btn_help:AddChild( label )


	local sp =  Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_check_big")
	local shop = Util:Button( sp , function( ... )
		LogicGUI.TempUnionCombatMyDeclarewar.New()
	end )
	shop:SetAnchorPoint( 0.5 , 0.5 )
	shop:SetPosition( 80 , 70 )
	self.widget:AddChild( shop )
	shop:SetScaleFactor( 0.9 )

	local union_combat_declareWarCheck = Util:Label( CPPGameLib.GetString("union_combat_declareWarCheck") , nil , Macros.Color.lilyWhite )
	union_combat_declareWarCheck:SetAnchorPoint( 0.5 , 0.5 )
	union_combat_declareWarCheck:SetPosition( 46 , shop:GetContentHeight()/2 )
	shop:AddChild( union_combat_declareWarCheck )
end

--星球设置
function Modules.UnionCombatView:__ShowUIStar( ... )
	--锚点
    self.anchor = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star2") )
    self.anchor:SetAnchorPoint( 0.5 , 0 )
	self.anchor:SetVisible( false )
	self.wrench:AddChild( self.anchor , 999 )
	local widget = NodeUnity.ConvertPosBetweenTwoContainer( self:GetNode() , self.wrench , COCOPoint( Macros.Global.DesignSize.width/2 , 0 ))
	self.anchor:SetPosition( widget.x , widget.y )

	local lst = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , nil , true )
	self.star = {}
	for i , v in ipairs( lst ) do
		self.star[i] = LogicGUI.TempUnionCombatStar.New( v )
		self.star[i]:SetAnchorPoint( 0.5 , 0.5 )

		local zorder = 1
		if v.sp_zorder[1] then
			zorder = v.sp_zorder[1]
		end

		self.wrench:AddChild( self.star[i]:GetNode() , zorder )
		if v.sp_pos[1] then
			PosTool.Center( self.star[i] , tonumber(v.sp_pos[1][1]) , tonumber(v.sp_pos[1][2]) )
		end
	end

	self:__SetShowUIStar()
end

--星球设置
function Modules.UnionCombatView:__SetShowUIStar( ... )
	local widget = NodeUnity.ConvertPosBetweenTwoContainer( self:GetNode() , self.wrench , COCOPoint( Macros.Global.DesignSize.width/2 , 0 ))
	self.anchor:SetPosition( widget.x , widget.y )

	for i , v in ipairs(self.star) do
		local x = math.abs( self.anchor:GetPositionX() - v:GetPositionX() )
		local y = math.abs( self.anchor:GetPositionY() - v:GetPositionY() )
		self.star[i]:SetScaleFactor( 1 +  math.min( ((1/(x + y) *20)) , 0.2 ) )

	end
end

--特效
function Modules.UnionCombatView:__Effects( ... )
end

--设置左边UI
function Modules.UnionCombatView:SetLeftUI( ... )
	--占领信息
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_label_up")
	local sp_label_up = Util:Sprite( sp )
	sp_label_up:SetAnchorPoint( 0 , 0.5 )
	sp_label_up:SetPosition( 0 , self.widget:GetContentHeight() - 115 )
	self.widget:AddChild( sp_label_up )

	local union_combat_occupy_info = Util:Name( CPPGameLib.GetString("union_combat_occupy_info") )
	union_combat_occupy_info:SetAnchorPoint( 0 , 0.5 )
	union_combat_occupy_info:SetPosition( sp_label_up:GetContentWidth()/2 - 10 , 25 )
	sp_label_up:AddChild( union_combat_occupy_info )

	--遮罩
	self.shade = Util:Layout( sp_label_up:GetContentWidth() , sp_label_up:GetContentHeight() + 500 )
	self.shade:SetAnchorPoint( 0.5 , 1 )
	self.shade:SetPosition( sp_label_up:GetContentWidth()/2 , 0 )
	sp_label_up:AddChild( self.shade )
	self.shade:SetClippingEnable( true )
	self.shade:SetClippingType( 0 )

	--列表
	self.lstBg = Util:Layout( sp_label_up:GetContentWidth() , sp_label_up:GetContentHeight() + 50 , function ( ... )
		-- body
	end)
	self.lstBg:SetAnchorPoint( 0.5 , 1 )
	self.lstBg:SetPosition( self.shade:GetContentWidth()/2 , self.shade:GetContentHeight() )
	self.shade:AddChild( self.lstBg )
    self.lstBg:SetBackGroundColor(0, 0 , 0)
    self.lstBg:SetBackGroundColorType(1)
    self.lstBg:SetBackGroundColorOpacity(120)

	--箭头
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_below")
	self.btn_below = Util:Button( sp , CPPGameLib.Handler(self,self.OnBtnBelow) )
	self.btn_below:SetAnchorPoint( 0.5 , 0.5 )
	self.btn_below:SetPosition( self.lstBg:GetContentWidth()/2 , 25 )
	self.lstBg:AddChild( self.btn_below )

	--下拉菜单
	self.belowKey = false
end

--刷新左边UI
function Modules.UnionCombatView:SetShowLeftUI( ... )
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionCombatOccupy")

	if lst then
		local length = #lst
		if #lst >= 5 then
			length = 5
		end
		self.lstBg:SetContentSize( self.lstBg:GetContentWidth() , 50 + (length * 70) )
		if self.belowKey == false then
			self.lstBg:SetPosition( self.shade:GetContentWidth()/2 , self.shade:GetContentHeight() + (length * 70) )
		else
			self.lstBg:SetPosition( self.shade:GetContentWidth()/2 , self.shade:GetContentHeight() )
		end
	end

	if not self.occupy and #lst ~= 0 then
		local length = #lst
		if #lst >= 5 then
			length = 5
		end

	    --图标列表
	    local params =  
	    {
	        item_class = Modules.UnionCombatItem,
	        item_width = self.lstBg:GetContentWidth(),
	        item_height = 70,
	        row = length ,
	        col = 1 ,
            item_space_r = 5 ,
	        view_width = self.lstBg:GetContentWidth(),
	        view_height = self.lstBg:GetContentHeight() - 50 ,
	    }
	    -- 创建参数
	    self.lst_list = GUI.ScrollView.New(params)
	    self.lstBg:AddChild(self.lst_list:GetNode())
	    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
	    self.lst_list:SetPosition( self.lstBg:GetContentWidth()/2 , self.lstBg:GetContentHeight()/2 + (50/2) )
	end

	if self.lst_list then
		self.lst_list:SetDataList( lst )
	end

	if #lst ~= 0 then
		print("xy" , "==================OnBtnBelow")
		self:OnBtnBelow()
	end
end

--下拉框
function Modules.UnionCombatView:OnBtnBelow( ... )
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionCombatOccupy")
	if not lst or #lst <= 0 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_notOccupy") )
		return 
	end

	--关着
	if self.belowKey == false then
		CPPActionManager.MoveTo( self.lstBg , 0.2 , self.shade:GetContentWidth()/2 , self.shade:GetContentHeight() )
		self.btn_below:SetRotation( 180 )
		self.belowKey = true
	--打开
	else
		local length = #lst
		if #lst >= 5 then
			length = 5
		end
		CPPActionManager.MoveTo( self.lstBg , 0.2 , self.shade:GetContentWidth()/2 , self.shade:GetContentHeight() + (length * 70) )
		self.btn_below:SetRotation( 0 )
		self.belowKey = false
	end
end


--时间设置
function Modules.UnionCombatView:__ShowTime( ... )

	if self.sp_black then
		CPPActionManager.StopAllActions(self.union_combat_type)
		self.sp_black:RemoveSelf(true)
		self.sp_black = nil
	end

	local timeCnf , endTime = CallModuleFunc(ModuleType.UNION , "GetGuildWarVSActivityTimeCnf")
	local t1 = {
			["xuanZhan"] = "union_combat_type1" ,
			["duiZhan"] = "union_combat_type2" ,
	}

	if not t1[timeCnf[1]] then
		return 
	end

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_black")
	self.sp_black = Util:Sprite9( sp )
	self.sp_black:SetContentSize( self.sp_black:GetContentWidth() + 50 , 40 )
	self.sp_black:SetAnchorPoint( 0.5 , 0.5 )
	self.sp_black:SetPosition( self.widget:GetContentWidth()/2 , self.widget:GetContentHeight() - 50 )
	self.widget:AddChild( self.sp_black )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_time")
	local sp_time = Util:Sprite( sp )
	sp_time:SetAnchorPoint( 0 , 0.5 )
	sp_time:SetPosition( 0 , self.sp_black:GetContentHeight()/2 )
	self.sp_black:AddChild( sp_time )

	self.union_combat_type = Util:RichText( " " , Macros.TypefaceSize.largish )
	self.union_combat_type:SetAnchorPoint( 0 , 0.5 )
	self.union_combat_type:SetPosition( 40 , self.sp_black:GetContentHeight()/2 )
	self.sp_black:AddChild( self.union_combat_type )

    local func = nil
    local function moveAnimate()
		local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		if endTime - time ~= 0 then
			local lab = TimeTool.TransTimeStamp( CPPGameLib.GetString("time_fate") , endTime - time )
			self.union_combat_type:SetText( CPPGameLib.GetString( t1[timeCnf[1]] , lab ) )
			CPPActionManager.DelayTo(self.union_combat_type , 1 , func)
		else
			CPPActionManager.DelayTo(self.union_combat_type , 0.02 , GlobalCallbackMgr:AddCallBackFunc(function( ... )
				self:__ShowTime()
			end) )
		end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end




Modules = Modules or {}
Modules.UnionCombatItem = Modules.UnionCombatItem or BaseClass(GUI.ScrollItem)

function Modules.UnionCombatItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.UnionCombatItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(180,70)
    return self.layout
end

function Modules.UnionCombatItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)  
    self:__ShowUI( data )
end

function Modules.UnionCombatItem:__ShowUI( data )
    --底
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_label_below")
	local sp_label_up = Util:Sprite( sp )
	sp_label_up:SetAnchorPoint( 0.5 , 0 )
	sp_label_up:SetPosition( self.layout:GetContentWidth()/2 , 0 )
	self.layout:AddChild( sp_label_up )
    
    --星球
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_STAR , data.cnf.sp[1] )
	local star = Util:Sprite( sp )
	star:SetAnchorPoint( 0.5 , 0.5 )
	star:SetPosition( self.layout:GetContentWidth()/2 - 50 , self.layout:GetContentHeight()/2 + 5 )
	self.layout:AddChild( star )
	star:SetScaleFactor( 0.15 )

	--名字
	local name = Util:Name( data.cnf.name , Macros.TypefaceSize.normal , data.cnf.star_type )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( self.layout:GetContentWidth()/2 - 5 , self.layout:GetContentHeight()/2 + 20 )
	self.layout:AddChild( name )

	--前往
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_little")
   	local btn_gather_little = Util:Button( sp , function( ... )
   		LogicGUI.TempUnionCombatStarInfo.New( data.id )
   	end , CPPGameLib.GetString("polytropic_maincity_go_for") , Macros.TypefaceSize.slightly , Macros.Color.btn , Macros.Color.blue_stroke )
   	btn_gather_little:SetAnchorPoint( 0.5 , 0.5 )
   	btn_gather_little:SetPosition( self.layout:GetContentWidth()/2 + 30 , 25 )
   	self.layout:AddChild( btn_gather_little )

   	--红点
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
   	self.red_point = Util:Sprite( sp )
   	self.red_point:SetAnchorPoint( 0.5 , 0.5 )
   	self.red_point:SetPosition( btn_gather_little:GetContentWidth() , btn_gather_little:GetContentHeight() )
   	btn_gather_little:AddChild( self.red_point )
   	self.red_point:SetScaleFactor( 0.8 )
   	self.red_point:SetVisible( false )

   	self:__SetRed()
end

--设置红点
function Modules.UnionCombatItem:__SetRed( )
	local info = CallModuleFunc( ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , self.__data.id )
    local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , self.__data.id )

    local timeCnf = CallModuleFunc( ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )
	for i , v in ipairs( timeCnf ) do
		if v == "duiZhan" then
			return 
		end
	end
	if info.is_have_reward ~= 0 then
		self.red_point:SetVisible( true )
	else
		self.red_point:SetVisible( false )
	end
end