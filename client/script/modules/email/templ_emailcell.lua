Modules = Modules or {}

Modules.TemplEmailCell = Modules.TemplEmailCell or BaseClass(GUI.ScrollItem)

function Modules.TemplEmailCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.EMAIL_SERVER_CELL)	
end

function Modules.TemplEmailCell:__delete()

end

function Modules.TemplEmailCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)	
	CPPGameLib.PrintTable("ldx",self.__data,"self.__data: ")
	local email_config = CPPGameLib.GetConfig("Email",self.__data.info_id,false)
	--名字
	Util:WidgetLabel(self:GetWidget("Text_name"),email_config and email_config.title or self.__data.title,Macros.TypefaceSize.largish,Macros.Color.headline )
	--时间
	local timeText = self:GetWidget("Text_time")
	Util:WidgetLabel(timeText,"",Macros.TypefaceSize.normal,Macros.Color.headline )
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local last_time = server_time - self.__data.receive_time
	local min = math.floor(last_time/60)
	if min == 0 then
		min = 1
	end
	if min < 60 then
		timeText:SetText(CPPGameLib.GetString("friend_timeMinText",min))
	elseif min >=60 and min < 1440 then
		timeText:SetText(CPPGameLib.GetString("friend_timeHourText",math.floor(min/60)))
	elseif min >= 1440 then
		timeText:SetText(CPPGameLib.GetString("friend_timeDayText",math.floor(min/1440)))
	end
	--按钮
	local bt_str = self.__data.info_id == 5 and CPPGameLib.GetString("email_goto_plunderedtreasures") or 
					(self.__data.info_id == 9 and CPPGameLib.GetString("email_reply_btn") or CPPGameLib.GetString("email_goto_friend"))
	Util:WidgetButton(self:GetWidget("Button_move"),bt_str,function ()		
		if self.__data.info_id == 5 then			
			--夺宝
			CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"OpenView",BaseViewType.PLUNDEREDTREASURES,1)
			GlobalEventSystem:FireNextFrame(Macros.Event.EmailModule.CLOSE_EMAIL_VIEW)
		elseif self.__data.info_id == 3 then
			--好友
			CallModuleFunc(ModuleType.FRIEND,"OpenView",BaseViewType.FRIEND)
			GlobalEventSystem:FireNextFrame(Macros.Event.EmailModule.CLOSE_EMAIL_VIEW)
		elseif self.__data.info_id == 9 then
			--查看客服回复详情
			LogicGUI.TemplEmailReply.New(self.__data.context_list[1].text)
		end
	end, Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_yellow)
	local is_visible = false
	if self.__data.info_id == 5 or self.__data.info_id == 3 or self.__data.info_id == 9 then
		is_visible = true
	end
	self:GetWidget("Button_move"):SetVisible(is_visible)
	--描述
	local des_txt = Util:RichText("",Macros.TypefaceSize.normal, 385, 69,Macros.Color.content_hex)
	des_txt:SetAnchorPoint(0,1)
	self:GetNode():AddChild(des_txt)
	des_txt:SetPosition(20,81)
	local data_list = {}
	CPPGameLib.PrintTable("ldx",self.__data.context_list,"self.__data.context_list: ")
	for i,content in ipairs(self.__data.context_list) do
		local turn_data = self:TurnData(content.type,content.text)
		table.insert(data_list,turn_data)
	end	
	--旧邮件特殊处理
	if self.__data.info_id == 6 and #data_list == 3 then
		table.remove(data_list,1)
	elseif self.__data.info_id == 7 and #data_list == 3 then
		table.remove(data_list,1)
	elseif self.__data.info_id == 8 and #data_list == 2 then
		table.remove(data_list,1)
	end

	local color = Macros.Color.content_hex
	local lab = GUI.RichText.GetColorWord(email_config and string.format(email_config.content,unpack(data_list)) or self.__data.text, color)
	des_txt:SetText(lab)
end

function Modules.TemplEmailCell:TurnData(type,data)
	if type == 1 then
		return tonumber(data)
	elseif type == 2 then
		return tostring(data)
	else
		local dataType,Config = ObjIDTool.GetInfo(tonumber(data))
		return GUI.RichText.GetColorWord(Config.name,Macros.Quality_HEX[Config.quality],Macros.Quality_Hex_Stroke[Config.quality],2)
	end
end