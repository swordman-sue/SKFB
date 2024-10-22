
Modules = Modules or {}

--服务器模板
Modules.TemplLoginServerItem = Modules.TemplLoginServerItem or BaseClass(GUI.ScrollItem)

function Modules.TemplLoginServerItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.LOGIN_SERVER_ITEM)	

	self.btn_server = self:GetWidget("Button_Server")
	self.img_role = self:GetWidget("Image_Role")
	self.text_role_lv = self:GetWidget("Text_RoleLev")
	self.text_role_lv:SetPosition(self.text_role_lv:GetPositionX() - 15,self.text_role_lv:GetPositionY())
	self.img_role:SetVisible(false)
	self.text_role_lv:SetVisible(false)	
end

function Modules.TemplLoginServerItem:__delete()
end

function Modules.TemplLoginServerItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	local img_status = self:GetWidget("Image_Status")
	local imgdata = CallModuleFunc(ModuleType.LOGIN, "GetServerStatusImgData", data.status)
	img_status:SetImage(imgdata)

	local text_server_id = self:GetWidget("Text_ServerID")
	Util:WidgetLabel(text_server_id, "", Macros.TypefaceSize.popup , Macros.Color.keypoint)
	local text_server_name = self:GetWidget("Text_ServerName")
	Util:WidgetLabel(text_server_name, "", Macros.TypefaceSize.popup , Macros.Color.keypoint)

	local server_id = data.id
	local server_id_name = string.format(CPPGameLib.GetString("login_server_id_name"), server_id)

	text_server_id:SetText(server_id_name)
	text_server_name:SetText(data.name)

	local data_ = CallModuleFunc(ModuleType.LOGIN , "GetLastLoginInfoByID" , data.id )
	local is_serveropen = CallModuleFunc(ModuleType.LOGIN , "IsServerOpen" , data.id )
	if data_ or not is_serveropen then
		--有玩家数据或者开服预告
		text_server_id:SetPosition( 
										text_server_id:GetPositionX() ,
										text_server_id:GetPositionY() + 15 
									)
		text_server_name:SetPosition( 
										text_server_name:GetPositionX() , 
										text_server_name:GetPositionY() + 15
									)

		self.text_role_lv:SetVisible(true)
		--字符串
		local show_str = ""
		if data_ then
			show_str = data_.role_name .."(" .. data_.role_level.. CPPGameLib.GetString("common_level_suffix")..")"
		elseif not is_serveropen then
			show_str = TimeTool.GetTimeFormat(data.open_time, 18).." "..TimeTool.GetTimeFormat(data.open_time, 15)..CPPGameLib.GetString("common_OpenServer")
		end
		Util:WidgetLabel( 
							self.text_role_lv , 
							show_str,
						    Macros.TypefaceSize.normal , 
						    Macros.Color.red_color
						)
	end
end

function Modules.TemplLoginServerItem:GetTouchNode()
	return self.btn_server
end

--最近登陆的服务器模板
Modules.TemplLastServerItem = Modules.TemplLastServerItem or BaseClass(GUI.Template)

function Modules.TemplLastServerItem:__init(parent_node, x, y)
	self:InitTemplate(GUI.TemplateRegistry.LOGIN_SERVER_ITEM, parent_node)
	self:SetPosition(x, y)
	self:SetAnchorPoint(0.5 , 0.5)

	self.btn_server = self:GetWidget("Button_Server")
	self.img_role = self:GetWidget("Image_Role")
	self.text_role_lv = self:GetWidget("Text_RoleLev")
	self.text_role_lv:SetPosition(self.text_role_lv:GetPositionX() - 15,self.text_role_lv:GetPositionY())
	self.img_role:SetVisible(false)
	self.text_role_lv:SetVisible(true)

	self.btn_server:SetTouchEnabled(true)
	WidgetUnity.SetWidgetListener(self.btn_server, nil, nil, function()
		if self.__select_callback then
			self.__select_callback(self.__data.id)
		end
	end)
end

function Modules.TemplLastServerItem:__delete()
end

function Modules.TemplLastServerItem:SetData(data)
	Modules.TemplLoginServerItem.SetData(self, data)


	-- self:GetWidget("Text_ServerID"):SetPosition( 
	-- 												self:GetWidget("Text_ServerID"):GetPositionX() ,
	-- 												self:GetWidget("Text_ServerID"):GetPositionY() + 10 
	-- 											)
	-- self:GetWidget("Text_ServerName"):SetPosition( 
	-- 												self:GetWidget("Text_ServerName"):GetPositionX() , 
	-- 												self:GetWidget("Text_ServerName"):GetPositionY() + 10 )
	-- Util:WidgetLabel( 
	-- 					self.text_role_lv , 
	-- 					data.role_name .."(" .. data.role_level .. CPPGameLib.GetString("common_level_suffix")..")" ,
	-- 				    Macros.TypefaceSize.slightly , 
	-- 				    Macros.Color.red_color
	-- 				)
end

function Modules.TemplLastServerItem:SetSelectCallback(callback)
	self.__select_callback = callback
end



--推荐的服务器模板
Modules.TemplRecommendServerItem = Modules.TemplRecommendServerItem or BaseClass(Modules.TemplLastServerItem)

function Modules.TemplRecommendServerItem:__init()
	-- self.img_role:SetVisible(false)
	self.text_role_lv:SetVisible(false)	
end

function Modules.TemplRecommendServerItem:SetData(data , data_ )
	Modules.TemplLoginServerItem.SetData(self, data)

	-- if data_ then
	-- 	self.text_role_lv:SetVisible(true)

	-- 	self:GetWidget("Text_ServerID"):SetPosition( 
	-- 													self:GetWidget("Text_ServerID"):GetPositionX() ,
	-- 													self:GetWidget("Text_ServerID"):GetPositionY() + 10 
	-- 												)
	-- 	self:GetWidget("Text_ServerName"):SetPosition( 
	-- 													self:GetWidget("Text_ServerName"):GetPositionX() , 
	-- 													self:GetWidget("Text_ServerName"):GetPositionY() + 10 )
	-- 	Util:WidgetLabel( 
	-- 						self.text_role_lv , 
	-- 						data_.role_name .."(" .. data_.role_level.. CPPGameLib.GetString("common_level_suffix")..")" ,
	-- 					    Macros.TypefaceSize.slightly , 
	-- 					    Macros.Color.red_color
	-- 					)
	-- end
end
