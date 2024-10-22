LogicGUI = LogicGUI or {}

LogicGUI.TemplInviteTeamMateView = LogicGUI.TemplInviteTeamMateView or BaseClass(GUI.Template)

function LogicGUI.TemplInviteTeamMateView:__init(recommend_list)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	local tab_sp1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "unselected_tag")
	local tab_sp2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TEAM, "selected_tag")
	self.__tab_params = {
		{tab_sp1, tab_sp2},
		{
			CPPGameLib.GetString("team_invite_recommend_player"),
			CPPGameLib.GetString("team_invite_invite_friend"),
        }, 
        10, 
        CPPGameLib.Handler(self, self.SelectedTab),
        false
    }
	self.__open_callback = function ()
		self.__team_mate_recommend_list = recommend_list
		self:InitBaseUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_INVITE_TEAM_TEAMMATE_CELL)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_INVITE_TEAM_TEAMMATE_VIEW)
end

function LogicGUI.TemplInviteTeamMateView:__delete()
	if self.__recommendplayer_scrollview then
		self.__recommendplayer_scrollview:DeleteMe()
		self.__recommendplayer_scrollview = nil
	end
	if self.__recommendfriend_scrollview then
		self.__recommendfriend_scrollview:DeleteMe()
		self.__recommendfriend_scrollview = nil
	end
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_CREATE_TEAM_VIEW_VISIBLE,true)
end

function LogicGUI.TemplInviteTeamMateView:__RegistAllEvents()
    --解散队伍
    self:BindGlobalEvent(Macros.Event.TeamModule.BREAK_THE_TEAM, function()
        self:DeleteMe()
    end)
    --获取好友列表
    self:BindGlobalEvent(Macros.Event.TeamModule.TEAM_DUNGEON_GET_FRIEND_LIST, function(_,friend_list)
        self:InitScrollViewByTabIndex(2,friend_list,true)
    end)
end

function LogicGUI.TemplInviteTeamMateView:InitBaseUI()
	if not self.__team_mate_recommend_list then
		return
	end
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_InviteClose"),"", function ()
		self:DeleteMe()
	end)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_InviteTitle"),CPPGameLib.GetString("team_invite_invite_teammate"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--选项卡
	local panel_tab = self:GetWidget("Panel_Tab")
	panel_tab:AddChild(self.__tab:GetNode())
	--推荐玩家容器
	self.__panel_recommendplayer = self:GetWidget("Panel_RecommendPlayer")
	--好友容器
	self.__panel_recommendfriend = self:GetWidget("Panel_RecommendFriend")
	--跳转初始页面
	self.__tab:On(1)
end

function LogicGUI.TemplInviteTeamMateView:SelectedTab(tab_index)
	--容器可见
	self.__panel_recommendplayer:SetVisible(tab_index == 1)
	self.__panel_recommendfriend:SetVisible(tab_index == 2)
	--滚动容器可见
	if self.__recommendplayer_scrollview then
		self.__recommendplayer_scrollview:SetVisible(tab_index == 1)
	end
	if self.__recommendfriend_scrollview then
		self.__recommendfriend_scrollview:SetVisible(tab_index == 2)
	end
	--标签
	if tab_index == 1 then
		if not self.__recommendplayer_scrollview then
			self:InitScrollViewByTabIndex(tab_index,self.__team_mate_recommend_list)
		end
	else
		if not self.__recommendfriend_scrollview then
			--请求好友数据
			CallModuleFunc(ModuleType.FRIEND,"FriendListRequest")
		end
	end
	--设置字体颜色
	for i=1,2 do
		self.__tab.btn_list[i]:SetOutline(1, 2, 3, 0, 2)
		self.__tab.btn_list[i]:SetTextColor(unpack(i == tab_index and Macros.Color.headline or Macros.Color.btn))
	end	
end

--初始化滚动容器
function LogicGUI.TemplInviteTeamMateView:InitScrollViewByTabIndex(init_tab_index,resp_data,is_friend)
	local panel = init_tab_index == 1 and self.__panel_recommendplayer or self.__panel_recommendfriend
	local scrollview_str = init_tab_index == 1 and "__recommendplayer_scrollview" or "__recommendfriend_scrollview"
	 --关卡选择容器
    local params = {
        item_class = Modules.TemplInviteTeamMateCell,
        item_width = 546,
        item_height = 58,
        row = 5,
        col = 1,
        item_space_r = 1,
        view_width = 546,
        view_height = 295,
    }
    self[scrollview_str] = GUI.ScrollView.New(params)
    panel:AddChild(self[scrollview_str]:GetNode())
    PosTool.Center(self[scrollview_str])
    for i,mate_data in ipairs(resp_data) do
    	mate_data.index = i
    	mate_data.invite_status = false
    	mate_data.is_friend = is_friend
    end
    self[scrollview_str]:SetDataList(resp_data)
end