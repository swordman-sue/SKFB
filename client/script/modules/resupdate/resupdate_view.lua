
--[[
%% @module: 资源更新界面
%% @author: swordman sue
%% @created: 2016-09-30
--]]

Modules = Modules or {}

Modules.ResUpdateView = Modules.ResUpdateView or BaseClass(BaseView)

--更新进度量
local PercentTag = 
{
	RES_UPDATE = 80,
	COMMON_LOAD = 20,
	SCRIPT_LOAD = 0,
}

local KBFactor = 1 / 1024
local MBFactor = 1 / (1024 * 1024)

local loading_bg_width = 0.75 * Macros.Global.DesignSize.width
local loading_bg_height = 28
local loading_bar_width = loading_bg_width - 34
local loading_bar_height = 18

function Modules.ResUpdateView:__init()
	self.__layout_mode = BaseView.LayoutMode.CODE	
	self.__is_ignore_iphone_safe_area = true

	local bg_path = Resource.PathTool.GetTrdResPath("bg_resupdate.jpg")
	local sp, exist = Util:Sprite(bg_path)
	if not exist then
		bg_path = Resource.PathTool.GetResPath("start/bg_resupdate.sdz")
	end
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = bg_path, image_size = COCOSize(1136, 640)}	
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__last_updated_size = 0
    self.__last_update_speed = 0
end

function Modules.ResUpdateView:__RegistAllEvents()
	self:BindGlobalEvent(Macros.Event.GameState.REQUEST_SERVER_CONFIG, function()
		self:__SetLoadingText(string.format(Macros.Color.white_stroke_rt, CPPGameLib.GetString("request_server_config")))	
	end)
	self:BindGlobalEvent(Macros.Event.GameState.REQUEST_SERVER_LIST, function()
		self:__SetLoadingText(string.format(Macros.Color.white_stroke_rt, CPPGameLib.GetString("request_server_list")))			
	end)
	self:BindGlobalEvent(Macros.Event.Net.SERVER_CONNECTING, function()
		self:__SetLoadingText(string.format(Macros.Color.white_stroke_rt, CPPGameLib.GetString("server_connecting")))			
	end)
end

function Modules.ResUpdateView:__OpenCallback()
end

function Modules.ResUpdateView:__Dispose()
	if self.UI_loading2 then
		self:__DelEffect(self.UI_loading2)
		self.UI_loading2 = nil
	end
	if self.UI_loading3 then
		self:__DelEffect(self.UI_loading3)
		self.UI_loading3 = nil
	end
end

function Modules.ResUpdateView:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)

	--创建logo
	if StartConfig.IsManualLogo then
		local exist
		self.logo, exist = Util:Sprite(Resource.PathTool.GetTrdResPath("logo.png"))				
		if not exist then
			self.logo = Util:Sprite(Resource.PathTool.GetResPath("start/logo.png"))
		end
		if self.logo then
			layout:AddChild(self.logo)
			PosTool.Center(self.logo, 7 , 189)				
		end
	end

	-- 加载进度
	self.__percent_text = Util:Name("0%", 20)
	self.__percent_text:SetStroke(true)
	self.__percent_text:SetStrokeColor(68, 44, 12, 255)
	layout:AddChild(self.__percent_text)
	PosTool.CenterBottom(self.__percent_text, 0, 31)

	-- 加载提示
	self.__loading_text = Util:RichText("", 20)
	self.__loading_text:SetAnchorPoint(0, 0.5)
	layout:AddChild(self.__loading_text)
	PosTool.CenterBottom(self.__loading_text, -loading_bar_width/2, 64)

	-- 进度条
 	self.__percent_prog = GUI.ProgressBar9Layer.New(Resource.PathTool.GetResPath("start/bar2.png"), 
 		Resource.PathTool.GetResPath("start/bar1.png"), loading_bar_width, loading_bar_height, loading_bg_width, loading_bg_height)
 	self.__percent_prog:SetAnchorPoint(0.5, 0.5)
	layout:AddChild(self.__percent_prog:GetNode())
	PosTool.CenterBottom(self.__percent_prog, 0, 33)

	local effect
	self.UI_loading2, effect = self:__CreateEffect({res_name = "UI_loading2" }, self.__percent_prog:GetNode() )
	effect:SetAnchorPoint(0.5,0.5)
	effect:SetPosition( 0 , self.__percent_prog:GetNode():GetContentHeight()/2 )

	self.UI_loading3, effect = self:__CreateEffect({res_name = "UI_loading3" }, self.__percent_prog:GetNode() )
	effect:SetAnchorPoint(0.5 , 0.5)
	effect:SetPosition( self.__percent_prog:GetNode():GetContentWidth()/2 , self.__percent_prog:GetNode():GetContentHeight()/2 )
	GlobalEffectMgr:SetEffectVisible( self.UI_loading3 , false)
	return layout
end

function Modules.ResUpdateView:__Showloading1( percent )
	local effect = GlobalEffectMgr:GetEffect(self.UI_loading2)
	if effect then
		effect:SetPosition( ( loading_bar_width * (  percent * 0.01 ) ) + 20 , self.__percent_prog:GetNode():GetContentHeight()/2 )
		if loading_bar_width * (  percent * 0.01 ) == loading_bar_width then
			self:__Showloading2()
			GlobalEffectMgr:SetEffectVisible( self.UI_loading2 , false)
		end
	end
end

function Modules.ResUpdateView:__Showloading2( percent )
	GlobalEffectMgr:SetEffectVisible( self.UI_loading3 , true)	
end

function Modules.ResUpdateView:__SetLoadingText(content)
	self.__loading_text:SetText(content)
end

function Modules.ResUpdateView:UpdateProcess(elapse_time)
	if not self:IsOpen() then
		return 
	end

	local update_speed_text, percent
	if not self:CallModuleFunc("IsResLoadFinished") then
		local is_unzipping = self:CallModuleFunc("IsUnzipping")

		--获取更新信息
		local updated_size, need_update_size, unzipped_size, need_unzip_size, downloaded_size, need_download_size = self:CallModuleFunc("GetResUpdateInfo")
		percent = need_update_size > 0 and (updated_size / need_update_size * PercentTag.RES_UPDATE) or PercentTag.RES_UPDATE

		--设置更新进度
		self.__percent_prog:SetProgress(percent)
		self.__percent_text:SetString(string.format("%d", percent) .. "%")

		--计算更新速度
		local is_first_time = self.__speed_elapse_time == nil
		self.__speed_elapse_time = self.__speed_elapse_time or 1
		self.__speed_elapse_time = self.__speed_elapse_time + elapse_time
		if self.__speed_elapse_time >= 1 then
			local update_speed = (updated_size - self.__last_updated_size) / (is_first_time and elapse_time or self.__speed_elapse_time)
			update_speed = is_first_time and update_speed or math.max((update_speed + self.__last_update_speed) / 2, 0)
			self.__last_update_speed = update_speed
			self.__last_updated_size = updated_size
			self.__speed_elapse_time = 0
		end

		--计算剩余时间
		self.__speed_elapse_time2 = self.__speed_elapse_time2 or 0.25
		self.__speed_elapse_time2 =  self.__speed_elapse_time2 + elapse_time
		if self.__speed_elapse_time2 >= 0.08 then
			self.__speed_elapse_time2 = 0

			local remain_time_str = ""
			local remain_time = (is_unzipping and (need_unzip_size - unzipped_size) or (need_download_size - downloaded_size)) / math.max(self.__last_update_speed, 1)
			if remain_time / 60 > 1.0 then
				remain_time_str = string.format("%.0f", remain_time / 60) .. CPPGameLib.GetString("common_minute")
			end
			remain_time_str = remain_time_str .. string.format("%.0f", remain_time % 60) .. CPPGameLib.GetString("common_second")

			--设置更新说明
			if is_unzipping then
				update_speed_text = CPPGameLib.GetString("resunzip_tips")
			else
				update_speed_text = CPPGameLib.GetString("resdownload_tips", self.__last_update_speed * KBFactor, downloaded_size * MBFactor, need_download_size * MBFactor)
			end
			self:__SetLoadingText(string.format(Macros.Color.white_stroke_rt, update_speed_text))		
		end

		self:__Showloading1(percent)			

	else
		local updated_size, need_update_size = GlobalGame:GetBaseKeepResLoadProg()
		local base_res_percent = need_update_size > 0 and (updated_size / need_update_size * PercentTag.COMMON_LOAD) or PercentTag.COMMON_LOAD
		local script_percent = PercentTag.SCRIPT_LOAD
		percent = PercentTag.RES_UPDATE + base_res_percent + script_percent

		--设置更新进度
		self.__percent_prog:SetProgress(percent)
		self.__percent_text:SetString(string.format("%d", percent) .. "%")

		--设置更新说明
		local tips_key = not self:CallModuleFunc("IsCommonLoadFinished") and "rescommonload_tips" or "resscriptload_tips"
		update_speed_text = CPPGameLib.GetString(tips_key)
		self:__SetLoadingText(string.format(Macros.Color.white_stroke_rt, update_speed_text))	

		self:__Showloading1( percent )
	end

end
