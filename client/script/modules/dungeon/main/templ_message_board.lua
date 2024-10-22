
Modules = Modules or {}

--主线副本关卡挑战模板
Modules.TemplMessageBoard = Modules.TemplMessageBoard or BaseClass(GUI.Template)

function Modules.TemplMessageBoard:__init(chapter_index)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self.__is_auto_load = true
	self.__open_callback = function()
		self.__chapter_index = chapter_index

		--留言数据
		CallModuleFunc(ModuleType.DUNGEON,"MapCommentListReq",self.__chapter_index,true)
					
		--关卡留言标题
		Util:WidgetLabel(self:GetWidget("Text_messageName"), CPPGameLib.GetString("dungeon_messageboard_title"), Macros.TypefaceSize.headline, Macros.Color.headline )		

		--发送留言按钮
		Util:WidgetButton(self:GetWidget("Button_send"),CPPGameLib.GetString("dungeon_messageboard_send"),function ()
			local is_could_comment = self:__GetIsCouldComment()
			if not is_could_comment then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_gamelevel_comment_cd",ConfigAdapter.Common.GetGameLevelCommentCD()/3600))
	            return
			end
			local string = self.__textfield:GetText()
	        if string == "" then
	            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_messageboard_textfield_nil"))
	            return
	        end
	        
	        local info = {}
	        info.map_id = self.__chapter_index
			info.text = LanguageFilter.ChatFilter(tostring(string))
	        CallModuleFunc(ModuleType.DUNGEON,"MapCommentReq",info)
		end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow )	

		--留言关闭
		Util:WidgetButton(self:GetWidget("Button_secondClose"), "",function ()
			local selected_state = self.__checkbox_messageboard:GetSelectedState()
			Cookies.Set(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.MessageBoard), selected_state and "1" or "0")
			self:DeleteMe()
			self:FireNextFrame(Macros.Event.DungeonModule.REFRASH_MESSAGE_BOARD)
		end)

		--输入框
	    self.__textfield = Util:WidgetTextField(self:GetWidget("TextField"))
	    self.__textfield:SetPlaceHolderText(CPPGameLib.GetString("dungeon_messageboard_textfield"))
	    self.__textfield:SetPlaceHolderColor(unpack( Macros.Color.content ))
	    self.__textfield:SetTextColor(unpack( Macros.Color.content ))    
	    self.__textfield:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	    self.__textfield:SetFontName(Util.FontType)
	    self.__textfield:SetMaxLengthEnabled(true)
	    self.__textfield:SetMaxLength(ConfigAdapter.Common.GetGameLevelCommentLength()) 
	    self.__textfield:SetFontSize(Macros.TypefaceSize.largish)
	    self.__textfield:SetTouchSize(350,50)
	    self.__textfield:SetTouchAreaEnabled(true)

	    --无留言提示
	    self.__text_messagetips = self:GetWidget("Text_messageTips")
	    self.__text_messagetips:SetVisible(false)
	    Util:WidgetLabel(self.__text_messagetips,CPPGameLib.GetString("dungeon_gamelevel_messageboard_nil"), Macros.TypefaceSize.headlineII, Macros.Color.btn,0,0,Macros.Color.common_stroke)	   		
		--留言开关
		Util:WidgetLabel(self:GetWidget("Text_OpenTips"),CPPGameLib.GetString("dungeon_messageboard_open_tips"), Macros.TypefaceSize.normal, Macros.Color.keypoint )	
		--标记
		local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s", Macros.Setting.MessageBoard)) ~= "0"
		--开启文字	
		self.__txt_openmessage = self:GetWidget("Text_OpenMessage")
		self.__txt_openmessage:SetVisible(flag)
		Util:WidgetLabel(self.__txt_openmessage,CPPGameLib.GetString("server_open"), Macros.TypefaceSize.normal, Macros.Color.btn)	
		--关闭文字
		self.__txt_closemessage = self:GetWidget("Text_CloseMessage")
		self.__txt_closemessage:SetVisible(not flag)
		Util:WidgetLabel(self.__txt_closemessage,CPPGameLib.GetString("server_close"), Macros.TypefaceSize.normal, Macros.Color.btn)	
		--选择
		self.__checkbox_messageboard = self:GetWidget("CheckBox_MessageBoard")		
		self.__checkbox_messageboard:SetSelectedState(flag)
		WidgetUnity.SetCheckboxListener(self.__checkbox_messageboard,function ()
			self.__txt_openmessage:SetVisible(true)
			self.__txt_closemessage:SetVisible(false)
		end,function ()
			self.__txt_openmessage:SetVisible(false)
			self.__txt_closemessage:SetVisible(true)
		end)
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_MESSAGE_CELL)	
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_MESSAGE_BOARD, parent_node)	
end

function Modules.TemplMessageBoard:__RegistAllEvents()    
    --留言列表回调
    self:BindGlobalEvent(Macros.Event.DungeonModule.GAMELEVEL_COMMENT,function (__,comment_list)
    	self.__messageboard_data = comment_list
    	self:InitMessageScrollView()
    end)	

    --留言回调
    self:BindGlobalEvent(Macros.Event.DungeonModule.GAMELEVEL_COMMENT_RESP,function (__,info)
    	self.__textfield:SetText("")
    	info.__index = #self.__messageboard_data + 1
    	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
    	info.role_name = role_info.role_name
    	info.vip_level = role_info.vip_level
    	table.insert(self.__messageboard_data,info)
    	self.__messageboard_scrollview:AttachItem(info)    	
    	self.__messageboard_scrollview:LocateToLast()
    	self.__text_messagetips:SetVisible(#self.__messageboard_data <= 0)
    end)

    --留言点赞回调
    self:BindGlobalEvent(Macros.Event.DungeonModule.GAMELEVEL_COMMENT_PARISE_RESP,function (__,comment_id)
    	local cur_index,to_index
    	self.__messageboard_scrollview:ItemDataIter(function (index,data,item)
    		if data.comment_id == comment_id then
    			cur_index = index
    			if item then   				
    				item:ResetData()
    				return true
    			end    			
    		end  		
    	end)
		--排序
		table.sort(self.__messageboard_data,function (data1,data2)
			if data1.like_times ~= data2.like_times then
				return data1.like_times > data2.like_times
			else
				return data1.comment_id < data2.comment_id
			end
		end)
		--获取当前位置
		for i,comment_info in ipairs(self.__messageboard_data) do
			comment_info.__index = i
			if comment_info.comment_id == comment_id then
				to_index = i
			end
		end
		self.__messageboard_scrollview:MoveItemTo(cur_index,to_index)	
		--刷新标志
		self.__messageboard_scrollview:ItemIter(function (index,item)
    		if item	then
    			item:SetHotTag()
    		end	
    	end)	
    end)
end

function Modules.TemplMessageBoard:__GetIsCouldComment()
	local is_could_comment = true
	local role_name = CallModuleFunc(ModuleType.ROLE,"GetName")
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	for i,comment_info in ipairs(self.__messageboard_data) do
		if comment_info.role_name == role_name then
			if (server_time - comment_info.time) < ConfigAdapter.Common.GetGameLevelCommentCD() then
				is_could_comment = false
				break
			end
		end
	end
	return is_could_comment
end

--初始化留言
function Modules.TemplMessageBoard:InitMessageScrollView()
	self.__text_messagetips:SetVisible(#self.__messageboard_data <= 0)
	--留言容器
	local panel_comment = self:GetWidget("Panel_comment")
	local params = {
		item_class = Modules.TemplDungeonMessageCell,
	    item_width = 616,
	    item_height = 92,
	    row	= 4,
	    col	= 1,
	    item_space_r = 8,
	    view_width = 616,
	    view_height = 305,
	}
	self.__messageboard_scrollview = GUI.ScrollView.New(params)
	panel_comment:AddChild(self.__messageboard_scrollview:GetNode())
	PosTool.Center(self.__messageboard_scrollview)
	self.__messageboard_scrollview:SetDataList(self.__messageboard_data)
end

function Modules.TemplMessageBoard:__delete()   
	if self.__messageboard_scrollview then
		self.__messageboard_scrollview:DeleteMe()
		self.__messageboard_scrollview = nil
	end
end