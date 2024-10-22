--
-- @brief 聊天弹窗 
-- @author: yjg
-- @date: 2016年8月20日18:31:58
--

Modules = Modules or {}

Modules.ChatView = Modules.ChatView or BaseClass(BaseView)

function Modules.ChatView:__init( )
    self.__layout_name = "chat" 
    self.__bg_params = {type = BaseView.BGType.SOLID}

	--状态排序
	self:__SetType()

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHAT)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHAT , true )
end


function Modules.ChatView:__OpenCallback( )
	self:__Getwidget()
	self:__ShowUI(data)
	self:RecordCD()

	local channel_type = unpack(self.__open_data)
	if channel_type == Macros.Game.CHAT.PRIVATE then
        self:OpPrivately()
	end
end

function Modules.ChatView:__RegistAllEvents( )
	--新信息
	self:BindGlobalEvent(Macros.Event.ChatModule.CHAT_DATA,function ( _ ,data , type_ )
		self:SetRed()
		self:NewMessage(data , type_ )
	end)

    self:BindGlobalEvent(Macros.Event.ChatModule.CHAT_CD,function ( _ , data)
    	self:RecordCD()
    end)
end

function Modules.ChatView:__Dispose()

	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
	
	if self.screen then
		GlobalTimerQuest:CancelQuest(self.screen)
		self.screen = nil
	end 

	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end

	if self.lst_biaoQing then
		self.lst_biaoQing:DeleteMe()
		self.lst_biaoQing = nil
	end
end

function Modules.ChatView:__Getwidget()
	--大背景框
	self.Panel_bg = self:GetWidget("Panel_bg") 
	--背景
	self.Image_1 = self:GetWidget("Image_1") 
	--内容背景
	self.Image_contentBg = self:GetWidget("Image_contentBg") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--语音按钮
	self.Button_voice = Util:WidgetButton( self:GetWidget("Button_voice") , nil , CPPGameLib.Handler(self,self.OnButtonVoice) )

	--表情按钮
	self.Button_expression = Util:WidgetButton( self:GetWidget("Button_expression") , nil , CPPGameLib.Handler(self,self.OnButtonExpression) )
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--发送按钮
	self.Button_sell = Util:WidgetButton( 
											self:GetWidget("Button_sell") , 
											CPPGameLib.GetString("fasong") , 
											CPPGameLib.Handler(self,self.OnButtonSell) ,
											Macros.TypefaceSize.normal,nil,Macros.Color.button_yellow
											)
	--私聊容器
	self.Panel_chatPrivately = self:GetWidget("Panel_chatPrivately") 
	--玩家名
	self.Image_player = self:GetWidget("Image_player") 
	--玩家输入框
	self.TextField_player = Util:WidgetTextField(self:GetWidget("TextField_player"))
	self.TextField_player:SetPlaceHolderText(CPPGameLib.GetString("men_patch_"))
	self.TextField_player:SetMaxLengthEnabled(true)
	self.TextField_player:SetMaxLength(10)
	self.TextField_player:SetTextColor( Macros.Color.content )
	self.TextField_player:SetTextHorizontalAlign(LabelEx.ALIGIN_LEFT)

	--输入框
	self.Image_dialogue = self:GetWidget("Image_dialogue") 
	--私聊输入框
	
	PosTool.MoveBy( self:GetWidget("TextField_dialogue") , 0 , 3 )
	self.TextField_dialogue = Util:WidgetTextField(self:GetWidget("TextField_dialogue"))
	self.TextField_dialogue:SetPlaceHolderText(CPPGameLib.GetString("men_patch"))
	self.TextField_dialogue:SetMaxLengthEnabled(true)
	self.TextField_dialogue:SetMaxLength( 30 )
	self.TextField_dialogue:SetTextColor( Macros.Color.content )
	self.TextField_dialogue:SetTextHorizontalAlign(LabelEx.ALIGIN_LEFT)

	--普通容器
	self.Panel_common = self:GetWidget("Panel_common") 
	--聊天输入
	PosTool.MoveBy( self:GetWidget("TextField_dialogue_0") , 0 , 3 )
	self.TextField_dialogue_0 = Util:WidgetTextField(self:GetWidget("TextField_dialogue_0"))
	self.TextField_dialogue_0:SetPlaceHolderText( CPPGameLib.GetString("men_patch") )
	self.TextField_dialogue_0:SetMaxLengthEnabled(true)
	self.TextField_dialogue_0:SetMaxLength( 30 )
	self.TextField_dialogue_0:SetTextColor( unpack( Macros.Color.content ) )
	self.TextField_dialogue_0:SetTextHorizontalAlign(LabelEx.ALIGIN_LEFT)

	--表情
	self.Panel_faceSecen = self:GetWidget("Panel_faceSecen") 
	self.Panel_faceSecen:SetZOrder( 999 )
	--页签
	self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx") 
end

function Modules.ChatView:__ShowUI()
	--标题
	self.light = Util:Label( CPPGameLib.GetString("liaotian") , Macros.TypefaceSize.popup , Macros.Color.white )
	self.light:SetAnchorPoint(0.5,0.5)
	self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(self.light)

	--对话框
    self.lst_list = Modules.TemplChatListLayer.New(self.Image_contentBg)
    self.Image_contentBg:AddChild(self.lst_list:GetNode())
    PosTool.Center( self.lst_list:GetNode() , 10 )

    self.shield = nil
    self.shield = Util:Layout( Macros.Global.DesignSize.width , Macros.Global.DesignSize.height , function( ... )
    	self.shield:SetVisible( false )
    	if self.lst_biaoQing then
    		self.lst_biaoQing:SetVisible(false)
    	end
    end )
    self.shield:SetVisible(false)
    self.shield:SetAnchorPoint(0.5,0.5)
    self.Image_1:AddChild(self.shield)  
    PosTool.Center(self.shield)

    --表情层
	self.lst_biaoQing = nil
    self.lst_biaoQing = Modules.TemplChatFaceLayer.New(function(index)
	    self:DateSettle(index)
        self.lst_biaoQing:SetVisible(false)
        self.shield:SetVisible( false )
    end , self )
    self.lst_biaoQing:SetVisible(false)
    self.lst_biaoQing:SetPosition( -50 , -170)
    self.Panel_faceSecen:AddChild(self.lst_biaoQing:GetNode())   

    --页签
    self.__tab:On(1)
    self.__tab:SetDirection(true , 0.5)
    self.__tab:SetPosition( self.Panel_Backpackcbx:GetContentWidth()/2 , self.Panel_Backpackcbx:GetContentHeight() )
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    --私聊组件
    local towards = Util:Label( CPPGameLib.GetString("towards") , Macros.TypefaceSize.slightly , Macros.Color.content )
    towards:SetPosition(-10 , self.Panel_chatPrivately:GetContentHeight()/2 )
    towards:SetAnchorPoint( 0 , 0.5)
    self.Panel_chatPrivately:AddChild(towards)

    PosTool.RightTo(towards,self.Image_player,5)

    local theory = Util:Label( CPPGameLib.GetString("theory") , Macros.TypefaceSize.slightly , Macros.Color.content )
    PosTool.RightTo(self.Image_player,theory,5)
    self.Panel_chatPrivately:AddChild(theory)

    PosTool.RightTo(theory,self.Image_dialogue,5)

    self:SetRed()
end

function Modules.ChatView:__SetType()

	-- 2920 在组队的主界面场景里的聊天界面显示世界频道，并可世界聊天，并且可以看到世界聊天内容预览。
	local viewInfo = GlobalViewMgr:GetCurViewInfo()
		
	local showLst = {}

	if (viewInfo and viewInfo.view_type == "TEAM") or (viewInfo and viewInfo.view_type == "TEAM_BOSS_BATTLE") then
		showLst = {
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.WORLD),
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.TEAM),
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.UNION),
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.PRIVATE)
				}

	    self.change = {
	    				Macros.Game.CHAT.WORLD,
				        Macros.Game.CHAT.TEAM,
				        Macros.Game.CHAT.UNION,
				        Macros.Game.CHAT.PRIVATE,
				    }
	else
		showLst = {
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.WORLD ),
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.UNION ),
					CPPGameLib.GetString("pingdao" .. Macros.Game.CHAT.PRIVATE)
				}

	    self.change = {
				        Macros.Game.CHAT.WORLD,
				        Macros.Game.CHAT.UNION,
				        Macros.Game.CHAT.PRIVATE
				    }
	end

	self.toTypeIndex = {}
	for i,v in ipairs( self.change ) do
		self.toTypeIndex[v] = i
	end

	--页签
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_lt_yellow")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_lt_white")
	self.__tab_params = {
		{sp_2,sp_1} , showLst , 10,
		CPPGameLib.Handler(self, self.OnYeMei)	
	}

end

--页眉
function Modules.ChatView:OnYeMei(index)

	self.index = index

	self.Panel_chatPrivately:SetVisible(false)
	self.Panel_common:SetVisible(false)

	--世界
	if self.change[index] == Macros.Game.CHAT.WORLD then
		self.Panel_common:SetVisible(true)
	--公会
	elseif self.change[index] == Macros.Game.CHAT.UNION then
		self.Panel_common:SetVisible(true)
	--私聊
	elseif self.change[index] == Macros.Game.CHAT.PRIVATE then
		self:OpPrivately()
		self.Panel_chatPrivately:SetVisible(true)
	--公会
	elseif self.change[index] == Macros.Game.CHAT.TEAM then
		self.Panel_common:SetVisible(true)
	end

	self.onType = self.change[index]
	self.lst_list:__ShowType(self.onType)

end

--关闭按钮
function Modules.ChatView:OnButtonClose()
	if self.onType == Macros.Game.CHAT.PRIVATE then
		CallModuleFunc(ModuleType.CHAT, "SetPrivateChatNotice", nil)
	end
	self:CloseManual()
end

--语音按钮
function Modules.ChatView:OnButtonVoice()
	print("语音按钮")
	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("no_men_patch"))
end

--表情按钮
function Modules.ChatView:OnButtonExpression()
	print("表情按钮")
	self.lst_biaoQing:SetVisible(true)
	self.shield:SetVisible(true)
end

--发送按钮
function Modules.ChatView:OnButtonSell()
	print("xy" , "发送按钮" , self.onType )

    local flag = CPPSdkTools.GetCPChannelType() == Macros.SDK.CPChannelType.NNS_BT and self.onType == Macros.Game.CHAT.WORLD
    if flag then
        if not GlobalCustomizedLogicAdapterMgr:Call("CheckWorldChatEnabled") then
            GlobalTipMsg.GetInstance():Show("GM等级达到1开启")   
            return
        end
    else
        local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
        local OpenVip = ConfigAdapter.Common.GetChatOpen()
        local vipLv = CallModuleFunc(ModuleType.VIP , "GetVipLV")
        if vipLv < OpenVip or role_level < ConfigAdapter.Common.GetChatSysOpenLevel() then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("dungeon_open_condition" , ConfigAdapter.Common.GetChatSysOpenLevel() , OpenVip )  )
            return
        end
    end

    local data = {}
    data.type = self.onType     --聊天类型
    data.is = 0
    data.voice = 0
    data.text = self:DateSettle()

    if self.onType == Macros.Game.CHAT.PRIVATE then
        if self.role_id then
            data.id = self.role_id
        end

        data.name = self.TextField_player:GetText()
        local info = {}
        info.role_name = data.name
        CallModuleFunc(ModuleType.CHAT , "SetWhisper" , info)

        local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
        if userDate.role_name == data.name then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("bierendemingzi"))
            return 
        end
    end

	local message_no_white_space = LanguageFilter.ReplaceAll( self:DateSettle() , LanguageFilter.WhiteSpaceSymbol, "")
	if string.len(message_no_white_space) == 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shuruneirong"))
        return 
	end	

	local text = CallModuleFunc(ModuleType.CHAT, "GetBeforeNews")
	if text == self:DateSettle() then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dont_Repetition"))
        return 
	end
	CallModuleFunc(ModuleType.CHAT, "SetBeforeNews" , self:DateSettle() )

	if self.onType == Macros.Game.CHAT.UNION then
		local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
		if role_info.guild_id ~= 0 then
			--发送时置灰按钮
			Util:GreyButton( self.Button_sell )
			CallModuleFunc(ModuleType.CHAT, "SendLiaoTianDate",data)
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rank_haveNoUnion"))
		end
	else
		--发送时置灰按钮
		Util:GreyButton( self.Button_sell )
	    CallModuleFunc(ModuleType.CHAT, "SendLiaoTianDate",data)
	end

    --置空
    self.TextField_dialogue_0:SetText("")
    self.TextField_dialogue:SetText("")
end

--整合输入
function Modules.ChatView:DateSettle( index )
	if self.onType ~= Macros.Game.CHAT.PRIVATE then
	    local text = self.TextField_dialogue_0:GetText() .. (index or "")
	    self.TextField_dialogue_0:SetText(text)
	    return text
	else
	    local text = self.TextField_dialogue:GetText() .. (index or "")
	    self.TextField_dialogue:SetText(text)
	    return text
	end
end

--新信息
function Modules.ChatView:NewMessage( info , type )
    --如果和我现在显示的页面一样
    if info.chat_type == self.onType then
    	if not type then
	        self.lst_list:NewCell(info)
	    else
	    	for i,v in ipairs(info) do
	    		self.lst_list:NewCell(i,v)
	    	end
	    end
    end
end

function Modules.ChatView:RecordCD( ... )

    Util:GreyButton( self.Button_sell )

    self.time = CallModuleFunc(ModuleType.CHAT, "GetTime")
    if self.time == 0 then

		local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.Button_sell:SetOutline(r, g, b, a, 2 )
		self.Button_sell:SetEnabled( true )

    else
        self.screen = GlobalTimerQuest:AddPeriodQuest(
            GlobalCallbackMgr:AddCallBackFunc(function()
                self.time = CallModuleFunc(ModuleType.CHAT, "GetTime")
                if self.time <= 0 then

					local r, g, b, a = unpack(Macros.Color.button_yellow)
					self.Button_sell:SetOutline(r, g, b, a, 2 )
					self.Button_sell:SetEnabled( true )

                	self.Button_sell:SetText(CPPGameLib.GetString("fasong"))
                	if self.screen then
                    	GlobalTimerQuest:CancelQuest(self.screen)
                    	self.screen = nil
                	end 
                else
                  Util:GreyButton( self.Button_sell )
                  self.Button_sell:SetText(math.floor(self.time).."s")
                end
            end),1,-1)
    end
end

--有人点了私聊(头像)
function Modules.ChatView:OpPrivately()
    self.whisper = CallModuleFunc(ModuleType.CHAT, "GetWhisper")

    self.__tab:On( self.toTypeIndex[Macros.Game.CHAT.PRIVATE] )

	CallModuleFunc(ModuleType.CHAT, "SetOpWhisper")

    if self.whisper then
        self.TextField_player:SetText(self.whisper.role_name)
        self.role_id = self.whisper.role_id
    else
       local info = CallModuleFunc(ModuleType.CHAT, "GetWhisperLst")
       if info and #info ~= 0 then
           self.TextField_player:SetText(info[#info].whisper)
           self.role_id = info[#info].role_id
       end
    end

    self:SetRed()
end

--设置红点
function Modules.ChatView:SetRed( ... )
	local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	if self.icon_red_point then
		self.icon_red_point:RemoveSelf(true)
		self.icon_red_point = nil
	end

	local type_ = CallModuleFunc(ModuleType.CHAT, "GetOpWhisper")
	if type_ and self.onType ~= Macros.Game.CHAT.UNION then
		self.icon_red_point = Util:Sprite(icon_red_point)
		self.icon_red_point:SetPosition( 
											self.__tab.btn_list[ self.toTypeIndex[Macros.Game.CHAT.PRIVATE] ]:GetContentWidth() - 5 , 
											self.__tab.btn_list[ self.toTypeIndex[Macros.Game.CHAT.PRIVATE] ]:GetContentHeight() - 5 
										)
		self.__tab.btn_list[ self.toTypeIndex[Macros.Game.CHAT.PRIVATE] ] :AddChild(self.icon_red_point , 20)
	end

end