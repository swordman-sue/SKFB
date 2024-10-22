LogicGUI = LogicGUI or {}

LogicGUI.TemplRecommend = LogicGUI.TemplRecommend or BaseClass(GUI.Template)

function LogicGUI.TemplRecommend:__init()
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__ShowUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.FRIEND_RECOMMEND_CELL)
	self:InitTemplate(GUI.TemplateRegistry.FRIEND_RECOMMEND)	
end

function LogicGUI.TemplRecommend:__delete()
	if self.__recommend_scrollview then
		self.__recommend_scrollview:DeleteMe()
		self.__recommend_scrollview = nil
	end

end

function LogicGUI.TemplRecommend:__ShowUI(  )
	--关闭按钮
	local btn_close = self:GetWidget("Button_close")
	WidgetUnity.SetWidgetListener(btn_close, nil, nil, function()
		self:Onbtn_no()
	end)

	--scrollerView
    local params = 
    {
        item_class = Modules.TemplRecommendCell,
        item_width = 446,
        item_height = 126,
        row = 4,
        col = 1,
        item_space_r = 5,
        view_height = 380,
    }
    self.__recommend_scrollview = GUI.ScrollView.New(params)
    local panel_zone = self:GetWidget("img_lstBg")
    panel_zone:AddChild(self.__recommend_scrollview:GetNode())
    PosTool.Center(self.__recommend_scrollview)

	--刷新
	local btn_refresh = self:GetWidget("btn_refresh")
	WidgetUnity.SetWidgetListener(btn_refresh, nil, nil, function()
    	CallModuleFunc(ModuleType.FRIEND, "FriendRecommendListRequest")
	end)
	self.__recommend_success_event = self:BindGlobalEvent(Macros.Event.FriendModule.RECOMMEND_SUCCESS, function()	
		local data_list = CallModuleFunc(ModuleType.FRIEND, "GetRecommendDataList")
		if data_list then 
			self.__recommend_scrollview:SetDataList(data_list)
		end
	end)

	--删除条目
	self.__add_event = self:BindGlobalEvent(Macros.Event.FriendModule.ADD_SUCCESS, function(_,index)	
		self.__recommend_scrollview:DetachItem(index)
	end)
end

function LogicGUI.TemplRecommend:Onbtn_no()
	self:DeleteMe()
end

--推荐好友的cell
Modules = Modules or {}

Modules.TemplRecommendCell = Modules.TemplRecommendCell or BaseClass(GUI.ScrollItem)

function Modules.TemplRecommendCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.FRIEND_RECOMMEND_CELL)	
	
end

function Modules.TemplRecommendCell:__delete()
	
end

function Modules.TemplRecommendCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	--头像
	local head_panel = self:GetWidget("Panel_head")
	local img_head = LogicGUI.RoleIcon.New(true)
	img_head:SetTouchCallback(function()
		-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
    end)
 	img_head:SetData(data.head_icon)
	head_panel:AddChild(img_head:GetNode())
	PosTool.Center(img_head)
	

	local text_name = self:GetWidget("Text_name")
	text_name:SetText(data.role_name)
	text_name:SetFontSize(Macros.TypefaceSize.largish)
	text_name:SetTextColor(unpack(Macros.Color.keypoint))

	local text_level = self:GetWidget("Text_level")
	text_level:SetText(string.format("等级:%s",data.level))

	local text_battle = self:GetWidget("Text_battle")
	text_battle:SetText(string.format("战力:%s",data.battle_value))

	local text_group = self:GetWidget("Text_group")
	text_group:SetText(string.format("公会:%s",data.guild_name))
	
	--添加好友
	local add_btn = self:GetWidget("Button_add")
	WidgetUnity.SetWidgetListener(add_btn, nil, nil, function()
		local num =	CallModuleFunc(ModuleType.FRIEND, "GetFriendNumber") --现在所拥有的好友数量
		if num +1 > 30 then   --TODO:这个好友上限是自己写死的，需要修改
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_max_num"))
			return
		end
		local data_add = {}
    	data_add.id = data.role_id
    	data_add.name = data.role_name
    	CallModuleFunc(ModuleType.FRIEND, "FriendAddRequest",data_add)
	end)

	--时间的显示     --TODO:这里时间的话，这样算是基本写好，具体还要看需求，看文字叙述
	local text_time = self:GetWidget("leave_time")
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local online_time = data.offline_time
	local last_time = server_time - online_time

	if last_time < 60 and last_time >0 then
		text_time:SetText("1分钟以前")
	elseif online_time == 0 then 
		text_time:SetText("在线")
	elseif (last_time/60) < 60 then
		text_time:SetText(string.format("%d分钟以前",math.floor(last_time/60)))
	elseif (last_time/60) > 60 and (last_time/60) <1440 then
		text_time:SetText(string.format("%d小时以前",math.floor(last_time/3600)))
	elseif (last_time/60) > 1440 and (last_time /60) <43200 then
		text_time:SetText(string.format("%d天以前",math.floor(last_time/86400)))
	end 
end

function Modules.TemplRecommendCell:SetSelected(var)
	-- self.cbox_zone:SetSelectedState(var)
end