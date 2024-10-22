
GUI = GUI or {}
GUI.FeignCheckBoxBtn = GUI.FeignCheckBoxBtn or BaseClass(UILayer)

--[[
	img_list			[table]{正常贴图, 选中贴图}
    title_list			[table]{标题1、标题2...}
    interval			[number]间隔
    selected_callback	[function]选中回调
    is_vert 			[bool]是否竖排
	注：需手动释放(DeleteMe)
--]]
function GUI.FeignCheckBoxBtn:__init(img_list, title_list, interval, selected_callback, is_vert, is_left_top, preparelevel_list, locklevel_list)
	self.img_list = self:__ParseImageList(img_list)
	self.title_list = title_list
	self.interval = interval
	self.selected_callback = selected_callback
	self.is_left_top = is_left_top or false
	self.__locklevel_list = locklevel_list
	self.__preparelevel_list = preparelevel_list

	local btn = Util:Button(self.img_list[1])
	self.btn_size = {}
	self.btn_size.width = btn:GetContentWidth()
	self.btn_size.height = btn:GetContentHeight()
	self.btn_list = {}

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
  	--按文字数量创建按钮 	
	for i, v in ipairs(self.title_list) do

		local text = v
		if v.text then
			text = v.text
		end

		local color = Macros.Color.btn
		if v.color then
			color = v.color
		end

		self.btn_list[i] = Util:Button(self.img_list[1], function()
			if self.__locklevel_list then
				local role_level_now = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
				if role_level_now < self.__locklevel_list[i] then
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_on_role_level",self.__locklevel_list[i]))
					return
				end
			end
			self:On(i)
		end, text , Macros.TypefaceSize.tab , color , Macros.Color.button_yellow)

		self:SetTouchSoundInfo()
		self:SetTabLockStatus(i,role_level)
		self:AddChild(self.btn_list[i], 1, i)		
	end

	if preparelevel_list and locklevel_list then
		--等级升级事件
		self.__level_up_event = self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function(_,old_level, new_level)
		        --更新锁的状态
		        self:UpdateTabLockStatus(new_level)
		        --隐藏未到开启等级的选项卡
				self:HideUnOpenTab(new_level)
		    end)
	end
	--布局
	self.is_vert = is_vert == nil and true or is_vert
	self:SetDirection(self.is_vert)
	--隐藏未到开启等级的选项卡
	self:HideUnOpenTab(role_level)
end

--给达到预放状态的选项卡设置锁的状态
function GUI.FeignCheckBoxBtn:SetTabLockStatus(pos,role_level)
	if self.__locklevel_list then
		if role_level < self.__locklevel_list[pos] then
			self.btn_list[pos]:SetBright(false)
			self["lock_icon_"..pos] = UIImageViewEx.CreateWithImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))
			self["lock_icon_"..pos]:SetAnchorPoint(1,1)
			self["lock_icon_"..pos]:SetScaleFactor(0.5)
			self.btn_list[pos]:AddChild(self["lock_icon_"..pos])	
			PosTool.RightTop(self["lock_icon_"..pos],3,3)
		end
	end
end

--隐藏未到开启等级的选项卡
function GUI.FeignCheckBoxBtn:HideUnOpenTab(new_level)	
	if self.__preparelevel_list then
		CPPGameLib.PrintTable(self.__preparelevel_list,"self.__preparelevel_list")
		self.__pos_list = {}
		for pos, btn in ipairs(self.btn_list) do
			if new_level < self.__preparelevel_list[pos] then
				self.__pos_list[pos] = pos
			end
		end	
		self:HideIndex(self.__pos_list)
	end
end

--设置按钮字体大小
function GUI.FeignCheckBoxBtn:SetFontSize(fontSize)
	for pos, btn in ipairs(self.btn_list) do
		btn:SetFontSize(fontSize)
	end
end

--更新锁的状态
function GUI.FeignCheckBoxBtn:UpdateTabLockStatus(new_level)
	if self.__locklevel_list then
		for pos, btn in ipairs(self.btn_list) do
			if new_level >= self.__locklevel_list[pos] then
				self.btn_list[pos]:SetBright(true)
				if self["lock_icon_"..pos] then
					self["lock_icon_"..pos]:RemoveSelf(true)
					self["lock_icon_"..pos] = nil
				end				
			end
		end		
	end
end


--设置方向
--true：竖排 
--false：横排
function GUI.FeignCheckBoxBtn:SetDirection(is_vert , anchor )
	if not is_vert then
		self:SetContentSize(self.btn_size.width * #self.title_list + (#self.title_list - 1) * self.interval, self.btn_size.height)
		self:SetAnchorPoint(0, 1)

		--设置横排按钮位置
		local x = self.btn_size.width / 2
		for _, btn in ipairs(self.btn_list) do
			--顶对齐
			btn:SetAnchorPoint(0.5, 0)
			PosTool.LeftTop(btn, x, 0)

			x = x + (self.btn_size.width + self.interval)
		end
	else
		self:SetContentSize(self.btn_size.width, self.btn_size.height * #self.title_list + (#self.title_list - 1) * self.interval)
		self:SetAnchorPoint(1, 1)

		--设置竖排按钮位置
		local y = self:GetHeight() - (self.btn_size.height / 2)
		for _, btn in ipairs(self.btn_list) do
			--右对齐
			btn:SetAnchorPoint(anchor or 1, 0.5)
			PosTool.RightBottom(btn, 0, y)

			y = y - (self.btn_size.height + self.interval)
		end
	end
end

--选择某个
function GUI.FeignCheckBoxBtn:On(index)
	if not self.btn_list[index] then
		LogTool.LogError("[GUI.FeignCheckBoxBtn:On] 无效的索引：%d", index)
		return
	end

	if not self.__not_forbidden then
		if self.selected_index and self.selected_index == index then		
			return
		end
	else
		self.__not_forbidden = false
	end
	if self.__forbidden_list and self.__forbidden_list[index] then	
		if self.__forbidden_str_list and self.__forbidden_str_list[index] then
			GlobalTipMsg.GetInstance():Show(self.__forbidden_str_list[index])
		end	
		return
	end
	
	self.selected_index = index
	self:SetCboxColor(Macros.Color.button_yellow,Macros.Color.button_red)
	local selected_btn = self.btn_list[self.selected_index]
	for i, btn in ipairs(self.btn_list) do
		if i ~= self.selected_index then
			btn:SetNormalImage(self.img_list[1])
			--红点位置
			if self["entry_red_dot_"..i] then
				if not self.is_left_top then
					PosTool.RightTop(self["entry_red_dot_"..i])
				else
					PosTool.LeftTop(self["entry_red_dot_"..i],20,0)
				end
			end
		end
	end
	selected_btn:SetNormalImage(self.img_list[2])
	--红点位置
	if self["entry_red_dot_"..index] then
		if not self.is_left_top then
			PosTool.RightTop(self["entry_red_dot_"..index])
		else
			PosTool.LeftTop(self["entry_red_dot_"..index],20,0)
		end
	end
	
	if self.selected_callback then
		self.selected_callback(index, selected_btn)
	end
end

--禁用某个
-- table = {
-- 	[1] = 1,
-- 	[2] = 2,
-- }
function GUI.FeignCheckBoxBtn:Forbidden(table)
	for i, v in ipairs(table) do
		if self.btn_list[v] then
			self.btn_list[v]:SetEnabled(false)
		else
			LogTool.LogError("[GUI.FeignCheckBoxBtn:Forbidden] 无效的索引：%d", v)
		end
	end
end

--设置禁用某个按钮和提示文字
-- table = {
-- 	[1] = 1,
-- 	[2] = 2,
-- }
function GUI.FeignCheckBoxBtn:SetForbiddenBtnWithString(btn_table,str_table,is_forbidden)
	self.__forbidden_list = btn_table
	for i,btn in ipairs(self.btn_list) do
		btn:SetBright(true)
	end
	if self.__forbidden_list then
		for i,v in pairs(self.__forbidden_list) do
			if self.btn_list[v] then
				self.btn_list[v]:SetBright(false)		
			end
		end
	end
	self.__forbidden_str_list = str_table
	self.__not_forbidden = is_forbidden
end

--隐藏某个
-- table = {
-- 	[1] = 1,
-- 	[2] = 2,
-- }
function GUI.FeignCheckBoxBtn:HideIndex(hide_table)
	-- for i, v in ipairs(table) do
	-- 	if self.btn_list[v] then
	-- 		self.btn_list[v]:SetVisible(false)
	-- 	else			
	-- 		LogTool.LogError("[GUI.FeignCheckBoxBtn:Forbidden] 无效的索引：%d", v)
	-- 	end
	-- end
	self:ShowIndex()
	-- if #hide_table == 0 then
	-- 	return
	-- end
	local pos_list = {}
	for i,value in ipairs(hide_table) do
		pos_list[value] = value
	end
	--本来隐藏的选项卡
	if self.__pos_list then
		for i,value in pairs(self.__pos_list) do
			if not pos_list[value] then
				pos_list[value] = value
			end
		end
	end
	if #hide_table > 0 or self.__pos_list then
		for i,btn in ipairs(self.btn_list) do
			if pos_list[i] then
				self.btn_list[i]:SetVisible(false)
			else
				self.btn_list[i]:SetVisible(true)
			end
		end
		for index,btn in ipairs(self.btn_list) do
			local countTimes = 0
			for i,v in pairs(pos_list) do
				if index > v then
					countTimes = countTimes + 1
				end
			end
			if self.is_vert then
				local y = self:GetHeight() - (self.btn_size.height / 2) - (index - 1 - countTimes)*(self.btn_size.height + self.interval)						
				PosTool.RightBottom(btn, 0, y)
			else			
				local x = self.btn_size.width / 2 + (self.btn_size.width + self.interval) * (index - 1 - countTimes)
				PosTool.LeftTop(btn, x, 0)
			end
		end
	end
end

function GUI.FeignCheckBoxBtn:ShowIndex()
	if self.is_vert then
		local y = self:GetHeight() - (self.btn_size.height / 2)
		for _, btn in ipairs(self.btn_list) do
			--右对齐
			btn:SetVisible(true)
			PosTool.RightBottom(btn, 0, y)

			y = y - (self.btn_size.height + self.interval)
		end		
	else			
		--设置横排按钮位置
		local x = self.btn_size.width / 2
		for _, btn in ipairs(self.btn_list) do
			--顶对齐
			btn:SetVisible(true)
			PosTool.LeftTop(btn, x, 0)

			x = x + (self.btn_size.width + self.interval)
		end
	end
end

function GUI.FeignCheckBoxBtn:GetBtn(index)
	return self.btn_list[index]
end

function GUI.FeignCheckBoxBtn:__delete()

end

--添加红点
function GUI.FeignCheckBoxBtn:AddRedDot(index)
	--红点
	if not self["entry_red_dot_"..index] then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self["entry_red_dot_"..index] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self["entry_red_dot_"..index]:SetAnchorPoint(1, 1)
		self.btn_list[index]:AddChild(self["entry_red_dot_"..index],2)
		if not self.is_left_top then
			PosTool.RightTop(self["entry_red_dot_"..index])
		else
			PosTool.LeftTop(self["entry_red_dot_"..index],20,0)
		end		
	end
	self["entry_red_dot_"..index]:SetVisible(true)
end

--隐藏红点
function GUI.FeignCheckBoxBtn:HideRedDot(index)
	if self["entry_red_dot_"..index] then
		self["entry_red_dot_"..index]:SetVisible(false)
	end	
end

--隐藏红点
function GUI.FeignCheckBoxBtn:SetCboxColor( color1 , color2 )

	if color1 then
		self.__color = {}
		self.__color.color1 = color1
		self.__color.color2 = color2 or color1
		color1 = self.__color.color1
		color2 = self.__color.color2
	elseif self.__color then
		color1 = self.__color.color1
		color2 = self.__color.color2
	else
		return
	end

	local stroke_width = stroke_width or 2
	for i=1 , #self.btn_list do
		local r, g, b, a = unpack( color2 )
		self.btn_list[i]:SetOutline(r, g, b, a, stroke_width)
	end

	local r, g, b, a = unpack( color1 )
	self.btn_list[self.selected_index]:SetOutline(r, g, b, a, stroke_width)
end

--设置选项卡音效
function GUI.FeignCheckBoxBtn:SetTouchSoundInfo(tag_id)
	tag_id = tag_id or 1

	for _, btn in ipairs(self.btn_list) do
		btn:SetTouchSoundInfo(Macros.Game.SystemIDForSound.TAB, tag_id)
	end
end

function GUI.FeignCheckBoxBtn:__ParseImageList(img_list)
	local real_img_list = {}

	for _, img_data in ipairs(img_list) do
		if type(img_data) == "table" then
			local real_img_data = Resource.LoadTool.GetImageData(img_data[1], img_data[2], img_data[3])
			table.insert(real_img_list, real_img_data)
		else
			table.insert(real_img_list, img_data)
		end
	end

	return real_img_list
end
