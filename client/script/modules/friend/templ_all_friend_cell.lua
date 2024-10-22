Modules = Modules or {}

Modules.TemplAllFriendCell = Modules.TemplAllFriendCell or BaseClass(GUI.ScrollItem)


function Modules.TemplAllFriendCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.FRIEND_ALL_CELL)
end

function Modules.TemplAllFriendCell:__delete()
	
end


function Modules.TemplAllFriendCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	
	--头像
	local head_panel = self:GetWidget("Panel_head")
	head_panel:SetTouchSwallow(false)
	local role_icon = LogicGUI.RoleIcon.New(false)
	role_icon:SetData(self.__data.head_icon)
	head_panel:AddChild(role_icon:GetNode())
	role_icon:adaptParentContentSize()
	PosTool.Center(role_icon)

	self:GetWidget("Image_v"):SetZOrder(1)
	self:GetWidget("Image_v"):SetVisible(self.__data.vip_level > 0)	
	if self.__data.vip_level > 0 then
		--VIP等级
	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
	    self.__viplv = GUI.ImgFont.New(sp)
	    self.__viplv:SetText(tostring(self.__data.vip_level))
	    self.__viplv:SetAnchorPoint(0,0.5)
	    self.__viplv:SetSpan(-2)
	    self.__viplv:SetScaleFactor(0.9)
	    self.__viplv:SetZOrder(1)
	    head_panel:AddChild(self.__viplv:GetNode())
	    PosTool.RightBottom(self.__viplv,-30,15)
	end
	
	--点击头像
    WidgetUnity.SetWidgetListener(head_panel, nil, nil,function ()
    	if self.__data.friend_Type ~= Macros.Game.FRIEND_TYPE.BLACK_LIST then
    		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , {role_id = self.__data.role_id} )
    	end
    end)

	--名字
	Util:WidgetLabel(self:GetWidget("Text_name"),self.__data.role_name,Macros.TypefaceSize.largish,Macros.Color.headline )

	--等级 
	Util:WidgetLabel(self:GetWidget("Text_level"),CPPGameLib.GetString("common_lev9",self.__data.level),Macros.TypefaceSize.normal,Macros.Color.headline )

	--战力 
	Util:WidgetLabel(self:GetWidget("Text_fightText"),CPPGameLib.GetString("friend_battleText"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--战力数值
	Util:WidgetLabel(self:GetWidget("Text_fightNum"),self.__data.battle_value,Macros.TypefaceSize.normal,Macros.Color.content)

	--公会
	Util:WidgetLabel(self:GetWidget("Text_gulid"),CPPGameLib.GetString("friend_gulidText"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--公会名
	Util:WidgetLabel(self:GetWidget("Text_gulidName"),self.__data.guild_name == "" and CPPGameLib.GetString("friend_have_no_union") or self.__data.guild_name,Macros.TypefaceSize.normal,Macros.Color.content)

	--时间的显示
	local text_time = self:GetWidget("Text_outTime")
	Util:WidgetLabel(text_time,"",Macros.TypefaceSize.slightly,Macros.Color.red)
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local online_time = self.__data.offline_time
	local last_time = server_time - online_time

	if online_time == 0 then 
		text_time:SetText(CPPGameLib.GetString("friend_onlineText"))
	else
		local min = math.floor(last_time/60)
		if min == 0 then
			min = 1
		end
		if min < 60 then
			text_time:SetText(CPPGameLib.GetString("friend_timeMinText",min))
		elseif min >=60 and min < 1440 then
			text_time:SetText(CPPGameLib.GetString("friend_timeHourText",math.floor(min/60)))
		elseif min >= 1440 then
			text_time:SetText(CPPGameLib.GetString("friend_timeDayText",math.floor(min/1440)))
		end
	end 

	--中心按钮
	self.__button_center = self:GetWidget("Button_center")
	--上按钮
	self.__button_up = self:GetWidget("Button_up")
	--下按钮
	self.__button_down = self:GetWidget("Button_down")
	

	self.__data.__show_type = 2
	if self.__data.friend_Type == Macros.Game.FRIEND_TYPE.FRIEND_LIST then	
		--赠送精力	
		if self.__data.is_give_energy == Macros.Global.FALSE then			
			Util:WidgetButton(self.__button_center,CPPGameLib.GetString("friend_give_energy"),function ()
				self.__button_center:SetEnabled(false)
				CallModuleFunc(ModuleType.FRIEND,"FriendEnergyGiveRequest",self.__data.role_id,self.__index)							
			end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
		else
			self.__data.__show_type = 1
		end			
	elseif self.__data.friend_Type == Macros.Game.FRIEND_TYPE.GET_ENERY then
		--领取精力
		Util:WidgetButton(self.__button_center,CPPGameLib.GetString("friend_get_energy"),function ()			
			local eneryNum = CallModuleFunc(ModuleType.FRIEND,"GetHaveGetEneryTimes") 
		    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		    local config = CPPGameLib.GetConfig("RoleLevel",level)
		    if eneryNum >= config.friend_num_max then
		    	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_get_max_energy"))
		    	return
			end
			self.__button_center:SetEnabled(false)
			CallModuleFunc(ModuleType.FRIEND,"FriendEnergyGetRequest",{{target_id = self.__data.role_id}},self.__index,self.__data.friend_Type)						
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
	elseif self.__data.friend_Type == Macros.Game.FRIEND_TYPE.ADD_FRIEND then
		--添加好友
		Util:WidgetButton(self.__button_center,CPPGameLib.GetString("friend_applyText"),function ()
			local info = {}
			info.target_id = self.__data.role_id
			self.__button_center:SetEnabled(false)
			CallModuleFunc(ModuleType.FRIEND,"FriendAddRequest",info,self.__index,self.__data.friend_Type)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
	elseif self.__data.friend_Type == Macros.Game.FRIEND_TYPE.APPLY_LIST then
		--好友申请
		self.__data.__show_type = 3
		local function FriendApplyReply(is_accept)
			if is_accept then
				local friendNum = CallModuleFunc(ModuleType.FRIEND,"GetFriendNumber") 
			    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			    local config = CPPGameLib.GetConfig("RoleLevel",level)
			    if friendNum >= config.friend_num_max then
			    	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_max_num"))			    	
			    	return 
				end
			end
			local info = {}
			info.target_id = self.__data.role_id
			info.is_accept = is_accept and Macros.Global.TRUE or Macros.Global.FALSE
			self.__button_up:SetEnabled(false)
			self.__button_down:SetEnabled(false)
			CallModuleFunc(ModuleType.FRIEND,"FriendApplyReplyRequest",info,self.__index,self.__data.friend_Type)
		end
		Util:WidgetButton(self.__button_up,CPPGameLib.GetString("friend_agreeText"),function ()
			FriendApplyReply(true)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
		Util:WidgetButton(self.__button_down,CPPGameLib.GetString("friend_rejustText"),function ( ... )
			FriendApplyReply(false)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_red)
	elseif self.__data.friend_Type == Macros.Game.FRIEND_TYPE.BLACK_LIST then
		--移除黑名单
		Util:WidgetButton(self.__button_center,CPPGameLib.GetString("friend_remove_black"),function ()
			self.__button_center:SetEnabled(false)
			CallModuleFunc(ModuleType.FRIEND,"BlackDelRequest",self.__data.role_id,self.__index,self.__data.friend_Type)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
	end
	self:ChangeShow()
end

function Modules.TemplAllFriendCell:ChangeShow()
	print("ldx","show_typeshow_type: ",self.__data.__show_type)
	CPPGameLib.PrintTable("ldx",self.__data,"lingqul8ingdiyiling")
	--设置各控件显隐
	self:GetWidget("Image_haveGive"):SetVisible(self.__data.__show_type == 1)
	self.__button_up:SetVisible(self.__data.__show_type == 3)
	self.__button_down:SetVisible(self.__data.__show_type == 3)
	self.__button_center:SetVisible(self.__data.__show_type == 2)
end

function Modules.TemplAllFriendCell:SetEneryData(is_give_energy,show_type)
	self.__data.is_give_energy = is_give_energy
	self.__data.__show_type = show_type
	self:ChangeShow()
end

function Modules.TemplAllFriendCell:Onbtn_no()
	self:DeleteMe()
end