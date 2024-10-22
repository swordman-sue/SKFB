--[[
%% @module: 排行榜
%% @author: yj
%% @created: 2016-08-18
--]]


Modules = Modules or {} 


Modules.RankView = Modules.RankView or BaseClass(BaseView)

function Modules.RankView:__init()
	self.__layout_name = "rank"	
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

	self:HandleTabViewData()
	self.__tab_params_ex = {#self.__rank_type_data , true}

	self.__open_callback = function()   	
    	self:__OpenCallback()
    end
    --请求排行榜数据回调事件
	self.__getRankBattleListReqEvent = self:BindGlobalEvent(Macros.Event.RankModule.RANK_LIST_REQ, function(_,data)
		--竞技场特殊处理
		if self.__rank_type_data[self.__selected_index].rank_type == Macros.Game.RankType.ARENA then			
			for i,v in ipairs(data.rank_list) do
				local isRobot = ObjIDTool.GetArenaRobotData(v.role_id)
				if isRobot then				
					local robotData = CPPGameLib.GetConfig("ArenaRobot",v.role_id)
					local battleValue = 0
					for key,monsterId in ipairs(robotData.monster_list) do
						battleValue = battleValue + CallModuleFunc(ModuleType.POWER, "CalcMonsterPower", monsterId,robotData.add_pro_type_list,robotData.add_pro_value_list)
					end
					v.battle_value = battleValue
					v.role_name = robotData.name
					v.level = robotData.level				
					v.head_icon = robotData.head_img_id	
					v.vip_level = 0
					v.guild_name = ""
					v.is_robot = true
				end
			end			
		end
		self.__rank_data_list[self.__selected_index] = data
		self:__SetRankList()
	end)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.RANK_VIEW_CELL)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RANK)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RANK,true)
end

function Modules.RankView:__OpenCallback()
	local init_type = unpack(self.__open_data)   --获得传进来的参数
	init_type = init_type or Macros.Game.RankType.BATTLE
	self.__is_first_open_rank = true
	--容器列表
	self.__rank_data_list = {}
	self.__panel_total = self:GetWidget("Panel_total")	
	
	--设置第一名信息
	--类型属性
	self.pro_value_pos = {[Macros.Game.RankType.LEVEL] = {760,870},[Macros.Game.RankType.BATTLE] = {750,850},[Macros.Game.RankType.KRUUNU] = {735,857},
						  [Macros.Game.RankType.MAIN_DUNGEON_STAR] = {735,857},[Macros.Game.RankType.CHASM] = {735,870}}
	--头像
	self.__roleicon_path = {}
	local panel_headicon = self:GetWidget("Panel_headIcon")
	self.__clipping_node = Util:CircleClippingNode(0,0,40)
	panel_headicon:AddChild(self.__clipping_node)
	PosTool.Center(self.__clipping_node)
	
	--名字
	self.__text_playername = self:GetWidget("Text_playerName")
	Util:WidgetLabel(self.__text_playername,"",Macros.TypefaceSize.normal,Macros.Color.golden)
	--类型
	self.__text_type = self:GetWidget("Text_level")
	Util:WidgetLabel(self.__text_type,"",Macros.TypefaceSize.normal,Macros.Color.tangerine_color)
	--公会
	Util:WidgetLabel(self:GetWidget("Text_union"),CPPGameLib.GetString("rank_union"),Macros.TypefaceSize.normal,Macros.Color.tangerine_color)
	--冠军类型数值
	self.__champion_value = self:GetWidget("Text_levelNum")
	Util:WidgetLabel(self.__champion_value,"",Macros.TypefaceSize.normal,Macros.Color.btn)
	--所在公会
	self.__union_name = self:GetWidget("Text_unionName")
	Util:WidgetLabel(self.__union_name,"",Macros.TypefaceSize.normal,Macros.Color.btn)
	--星图片
	self.__Image_star = self:GetWidget("Image_star")
	self.__Image_star:SetVisible(false)
	--查看详细资料
	local button_watchdetail = self:GetWidget("Button_watchDetail")
	Util:WidgetButton(button_watchdetail,CPPGameLib.GetString("rank_watchDetail"),function ()
		--竞技场特殊处理
		if self.__rank_type_data[self.__selected_index].rank_type == Macros.Game.RankType.ARENA then
			local role_info = self.__rank_data_list[self.__selected_index].rank_list[1]
			if role_info.is_robot then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_rank_find_tips"))
				return
			end
		end
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , self.__rank_data_list[self.__selected_index].rank_list[1] )
		--CallModuleFunc(ModuleType.CHECKROLEINFO,"RoleDetailsReq",Macros.Game.HeroMessageReq_Type.RANK,self.__rank_data_list[self.__selected_index].rank_list[1].role_id)
	end,Macros.TypefaceSize.popup,Macros.Color.btn, Macros.Color.button_yellow )
	--我的排名	
	self.__txt_myrank = self:GetWidget("Text_myRank")
	Util:WidgetLabel(self.__txt_myrank,CPPGameLib.GetString("rank_myRank"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
	--我的类型
	self.__my_texttype = self:GetWidget("Text_myLevel")
	Util:WidgetLabel(self.__my_texttype,"",Macros.TypefaceSize.normal, Macros.Color.keypoint )
	--我的排名数值	
	self.__my_ranknum = self:GetWidget("Text_myRankNum")
	Util:WidgetLabel(self.__my_ranknum,"",Macros.TypefaceSize.normal,Macros.Color.content )
	--我的类型数值
	self.__my_typenum = self:GetWidget("Text_myLevelNum")
	Util:WidgetLabel(self.__my_typenum,"",Macros.TypefaceSize.normal,Macros.Color.content )		
	--处理选项卡
	local panel_tab = self:GetWidget("Panel_tab")
    self.__tab:SetContentSize(920,80)
    self.__tab:SetData(self.__rank_tab_name_list)
    self.__tab:SetItemAlign(0)

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__is_call_back_return = true
    self.__tab:SetOnCeil(on,no)  
    self.__is_call_back_return = false
    -- self.__tab:SetAnchorPoint(0.5,)
    self.__tab:SetPosition(450,25)
    panel_tab:AddChild(self.__tab:GetNode())

    self.__tab:SetTouchEnabled(true)
    self.__tab:SetHandleSpCallBack(function (index)
    	if index and self.__rank_data_list[index] and #self.__rank_data_list[index].rank_list <= 0 then
    		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rank_nil_tips"))
    		return true
    	end
    end)
    self.__tab:On(init_type)	
end	

function Modules.RankView:__Dispose()
	if self.__total_scrollview then
		self.__total_scrollview:DeleteMe()
		self.__total_scrollview = nil
	end
end

--处理选项叶卡数据
function Modules.RankView:HandleTabViewData()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	self.__rank_type_data = {} 
	self.__rank_tab_name_list = {}
	for i,system_info in ipairs(CPPGameLib.GetConfig("SystemEntry",nil,nil,true)) do
		--是否需要显示的排行榜
		if system_info.rank_list and role_level >= system_info.level then
			--遍历对应的排行榜列表
			for _,rank_type in ipairs(system_info.rank_list) do
				table.insert(self.__rank_type_data,{rank_type = rank_type})
				table.insert(self.__rank_tab_name_list,CPPGameLib.GetString("rank_title_type"..rank_type) or "")
			end			
		end
	end
end

function Modules.RankView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )
    layer:SetAnchorPoint( 0.5 , 0.5 )

    local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.white , nil , nil , Macros.Color.button_yellow )
    name:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(name)
    PosTool.Center( name )

    return layer
end

--选项卡回调
function Modules.RankView:OnCallBack(index)	
	if self.__is_call_back_return then
		return
	end
	--下划线
	if self.__onitemeff then
        self.__onitemeff:RemoveSelf(true)
        self.__onitemeff = nil
    end

    local item = self.__tab:GetBtn( index )

    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    self.__onitemeff = Util:Sprite( com_sp_cbox_wire )
    self.__onitemeff:SetAnchorPoint( 0.5 , 1 )
    self.__onitemeff:SetPosition( item:GetContentWidth()/2 , 3 )
    item:AddChild( self.__onitemeff )
    self.__before_selected_index = self.__selected_index or index
	self.__selected_index = index	
	if not self.__rank_data_list[index] then
		--对应排行榜数据请求 			
		self:CallModuleFunc("RankListRequest",{type = self.__rank_type_data[index].rank_type})
	else
		self:__SetRankList()
	end
	-- --是否刷新
	-- if self.__rank_data_list[index] and #self.__rank_data_list[self.__selected_index].rank_list > 0 then
	-- 	self:__RefreshViewMessage()	
	-- end
end

function Modules.RankView:__SetRankList()		
	if self.__rank_data_list[self.__selected_index] and #self.__rank_data_list[self.__selected_index].rank_list > 0 then
		if not self.__total_scrollview then
			local params = {
		  		item_class	= Modules.TemplRankCell,
			    item_width	= 616,
			    item_height	= 122,
			    row	= 4,
			    col	= 1,
			    item_space_r = 5,
			    view_width = 616,
			    view_height = 430,
		  	}
		  	self.__total_scrollview = GUI.ScrollView.New(params)
		  	self.__panel_total:AddChild(self.__total_scrollview:GetNode())
		  	PosTool.Center(self.__total_scrollview)
		end
		--设置类型
	  	for i,rankMessage in ipairs(self.__rank_data_list[self.__selected_index].rank_list) do
	  		rankMessage.rank = i
	  		rankMessage.rankType = self.__rank_type_data[self.__selected_index].rank_type
	  	end		
		self.__total_scrollview:SetDataList(self.__rank_data_list[self.__selected_index].rank_list)
		self.__roleicon_path[self.__selected_index] = Resource.PathTool.GetRoleHeadPath(self.__rank_data_list[self.__selected_index].rank_list[1].head_icon)
		if not self.__role_icon then
			self.__role_icon = Util:Sprite(self.__roleicon_path[self.__selected_index])
			self.__clipping_node:AddChild(self.__role_icon)
			self.__role_icon:SetScaleFactor(1.2)
			PosTool.Center(self.__role_icon)	
		end
		self:__RefreshViewMessage()	
	else
		if self.__before_selected_index then
			self.__tab:On(self.__before_selected_index)	
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rank_nil_tips"))
		end
	end
end

function Modules.RankView:__RefreshViewMessage()
	local cur_type = self.__rank_type_data[self.__selected_index].rank_type
	--是否显示星数的
	local is_show_star = false
	if cur_type == Macros.Game.RankType.MAIN_DUNGEON_STAR or cur_type == Macros.Game.RankType.KRUUNU then
		is_show_star = true
	end	
	self.__is_before_rank_zero = self.__is_before_rank_zero or false
	--左侧最强榜显示属性类型
	self.__text_type:SetText(CPPGameLib.GetString("rank_typeText"..cur_type) or "")
	--下方显示自己的排名
	self.__my_texttype:SetText(CPPGameLib.GetString("rank_myTypeText_"..cur_type) or "")
	--最强榜的数据
	self.__role_icon:SetImage(self.__roleicon_path[self.__selected_index])
	self.__Image_star:SetVisible(is_show_star)
	if self.__rank_data_list[self.__selected_index].rank_list[1].guild_name ~= "" then
		self.__union_name:SetText(self.__rank_data_list[self.__selected_index].rank_list[1].guild_name)
	else 
		self.__union_name:SetText(CPPGameLib.GetString("rank_haveNoUnion"))
	end	
	self.__text_playername:SetText(self.__rank_data_list[self.__selected_index].rank_list[1].role_name)	

	self.__is_before_rank_zero = self.__rank_data_list[self.__selected_index].rank == 0
	--自己的排名
	self.__my_ranknum:SetText(self.__rank_data_list[self.__selected_index].rank == 0 and CPPGameLib.GetString("rank_reflection") or self.__rank_data_list[self.__selected_index].rank)	
	self.__my_texttype:SetPosition(not self.pro_value_pos[cur_type] and self.pro_value_pos[Macros.Game.RankType.BATTLE][1] or self.pro_value_pos[cur_type][1],40)
	self.__my_typenum:SetPosition(not self.pro_value_pos[cur_type] and self.pro_value_pos[Macros.Game.RankType.BATTLE][2] or self.pro_value_pos[cur_type][2],40)
	self.__my_typenum:SetText(self.__rank_data_list[self.__selected_index].rank_value or 0)
	--角色信息
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	if cur_type == Macros.Game.RankType.BATTLE or cur_type == Macros.Game.RankType.ARENA then					
		self.__champion_value:SetText(self.__rank_data_list[self.__selected_index].rank_list[1].battle_value)	
		self.__my_typenum:SetText(role_info.battle_value or 0)		
	elseif cur_type == Macros.Game.RankType.LEVEL then			
		self.__champion_value:SetText(self.__rank_data_list[self.__selected_index].rank_list[1].level)		
	else
		local cur_star = CallModuleFunc(ModuleType.DUNGEON,"GetChapterTotalStar")				
		self.__champion_value:SetText(self.__rank_data_list[self.__selected_index].rank_list[1].context)		
	end	

end

