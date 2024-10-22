
--[[
%% @module: 副本界面
%% @author: swordman sue
%% @created: 2016-08-11
--]]

Modules = Modules or {}

Modules.DungeonView = Modules.DungeonView or BaseClass(BaseView)

local dungeon_type_systemid_list = {Macros.Game.SystemID.DUNGEON_GAMELEVEL,Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL,Macros.Game.SystemID.DUNGEON_ELITE}

function Modules.DungeonView:__init()
	self.__layout_name = "dungeon"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_dungeon")}
	
	self.__title_bar_params = 
	{
		close_func = function()
			if not self.__is_shield_close or CallModuleFunc(ModuleType.LEAD, "IsInLead") then
				self:CloseManual()
			end
		end,
	}

	self.__tab_view_params = 
	{
		{Modules.MainDungeonTabView, "Panel_MainDungeon"},
		{Modules.HeroDungeonTabView, "Panel_HeroDungeon"},
		{Modules.EliteDungeonTabView, "Panel_EliteDungeon"},
	}

	--请求红点
    self.__open_callback = function()    
		self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON,true)
end

function Modules.DungeonView:__delete()
end

function Modules.DungeonView:__OpenCallback()
	local init_tab, callback = unpack(self.__open_data)	

	--功能开启等级	
	self:__CountLockLevel()	

	--按钮容器
	self.__panel_bottom = self:GetWidget("Panel_Bottom")
	--按钮背景
	self.__image_btn = self:GetWidget("Image_btn")
	--初始化入口
	for i = 1, 3 do
		self["Button_FB"..i] = self:GetWidget("Button_FB"..i)	
		self["Button_FB"..i]:IgnoreContentAdaptWithSize(true)
		Util:WidgetButton(self["Button_FB"..i],CPPGameLib.GetString("dungeon_entry_name"..i), function()
			local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			if role_level < self.__lock_level_list[i] then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_on_role_level",self.__lock_level_list[i]))
				return
			end
			self:__TabChange(i)
		end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)		
	end	
	self:GetWidget("Panel_shield"):SetVisible(false)

	self:__SetFBButtonStatus()
	self:__SetFBButtonPosition()

	self:__TabChange(init_tab or BaseViewSubType.DUNGEON.MAIN)	

	if callback and type(callback) == "function" then
		callback()
	end
end

function Modules.DungeonView:__Dispose()
end

function Modules.DungeonView:__RegistAllEvents()
	--红点事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		if system_id == Macros.Game.SystemID.DUNGEON then
			self:__UpdateRedDot()
		end
	end)
end

function Modules.DungeonView:SetPassedChapterShield(isShield)
	self.__is_shield_close = isShield
end

function Modules.DungeonView:__CountLockLevel()
	self.__lock_level_list = {}
	self.__prepare_level_list = {}
	local module_type_list = {Macros.Game.SystemID.DUNGEON_GAMELEVEL,Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL,
	Macros.Game.SystemID.DUNGEON_ELITE}
	for i,module_id in ipairs(module_type_list) do
		local module_info = CPPGameLib.GetConfig("SystemEntry",module_id)
		table.insert(self.__lock_level_list,module_info.level)
		table.insert(self.__prepare_level_list,module_info.prepare_level)
	end
end

function Modules.DungeonView:__SetFBButtonStatus()
	local init_tab = unpack(self.__open_data)
	init_tab = init_tab or BaseViewSubType.DUNGEON.MAIN

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	for pos=1,3 do
		--开启等级
		if role_level < self.__lock_level_list[pos] then
			self["Button_FB"..pos]:SetBright(false)
			self["lock_icon_"..pos] = UIImageViewEx.CreateWithImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))
			self["lock_icon_"..pos]:SetAnchorPoint(1,1)
			self["lock_icon_"..pos]:SetScaleFactor(0.5)
			self["Button_FB"..pos]:AddChild(self["lock_icon_"..pos])	
			PosTool.RightTop(self["lock_icon_"..pos],-10,3)
		end
	end

	self:__UpdateRedDot()
end

function Modules.DungeonView:__SetFBButtonPosition()
	self.__panel_bottom:SetVisible(true)
	self.__image_btn:SetVisible(true)
	self.__title_bar:SetTitleBgVisible(false)
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local bt_num = 0
	for pos=1,3 do
		--预告等级
		if role_level < self.__prepare_level_list[pos] or (pos >= 2 and CPPSdkTools.IsInExamine()) then			
			self["Button_FB"..pos]:SetVisible(false)
		else
			bt_num = bt_num + 1
		end
	end
	if bt_num == 2 then
		self.Button_FB1:SetPosition(self.Button_FB2:GetPositionX(),self.Button_FB1:GetPositionY())
		self.Button_FB2:SetPosition(self.Button_FB3:GetPositionX(),self.Button_FB2:GetPositionY())
	elseif bt_num == 1 then
		self.__panel_bottom:SetVisible(false)
		self.__image_btn:SetVisible(false)
		self.__title_bar:SetTitleBgVisible(true)
	end
end

--选项卡切换处理
function Modules.DungeonView:__TabChange(tab_index)
	BaseView.__TabChange(self, tab_index)

	--标题
	self:SetTitle(CPPGameLib.GetString("dungeon_entry_name"..tab_index))

	--选中效果
	for i = 1, 3 do
		local data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, i == tab_index and "btn_selected" or "btn_unselected")
		self["Button_FB"..i]:SetNormalImage(data)
		local r, g, b, a = unpack( i == tab_index and Macros.Color.button_yellow or Macros.Color.button_red)
		self["Button_FB"..i]:SetOutline(r, g, b, a, 2)	
	end
end

--添加红点
function Modules.DungeonView:__AddRedDot(node, i)
	--红点
	if not self["entry_red_dot"..i] then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self["entry_red_dot"..i] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self["entry_red_dot"..i]:SetAnchorPoint(1, 1)
		node:AddChild(self["entry_red_dot"..i],2)
		PosTool.RightTop(self["entry_red_dot"..i],-10,0)
	end
	self["entry_red_dot"..i]:SetVisible(true)
end

--隐藏红点
function Modules.DungeonView:__HideRedDot(i)
	if self["entry_red_dot"..i] then
		self["entry_red_dot"..i]:SetVisible(false)
	end	
end

--更新红点
function Modules.DungeonView:__UpdateRedDot()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	for pos = 1, 3 do
		--开启等级
		if role_level >= self.__lock_level_list[pos] then
			local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.DUNGEON, dungeon_type_systemid_list[pos])
			if red_num > 0 then
				self:__AddRedDot(self["Button_FB"..pos], pos)
			else
				self:__HideRedDot(pos)
			end
		end
	end
end

--获取打开参数
function Modules.DungeonView:GetOpenParams()
	if self.__tab_index then
		return {self.__tab_index}
	end
end