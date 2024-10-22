
--[[
%% @module: 场景加载界面
%% @author: swordman sue
%% @created: 2017-03-23
--]]

Modules = Modules or {}

Modules.SceneLoadingView = Modules.SceneLoadingView or BaseClass(Modules.UILoadingView)

--更新进度量
local PercentTag = 
{
	MAP = 50,
	SP_RES = 50,
}

local loading_bg_width = 0.75 * Macros.Global.DesignSize.width
local loading_bg_height = 28
local loading_bar_width = loading_bg_width - 34
local loading_bar_height = 18

function Modules.SceneLoadingView:__init()
	self.__layout_mode = BaseView.LayoutMode.CODE	
	self.__close_mode = BaseView.CloseMode.VISIBLE
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP
	self.__open_loading = false
	self.is_auto_close = false
end

function Modules.SceneLoadingView:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)

	-- 加载进度
	self.__percent_text = LabelEx.CreateWithString("0%", Util.FontType, 20)
	self.__percent_text:SetStroke(true)
	self.__percent_text:SetStrokeColor(68, 44, 12, 255)
	layout:AddChild(self.__percent_text)
	PosTool.CenterBottom(self.__percent_text, 0, 33)

	-- 加载提示
	self.__loading_text = Util:RichText("", 24, 0, 0)
	layout:AddChild(self.__loading_text)
	PosTool.CenterBottom(self.__loading_text, 0, 76)
	
	-- 进度条
 	self.__percent_prog = GUI.ProgressBar9Layer.New(
 		Resource.PathTool.GetResPath("start/bar2.png"), Resource.PathTool.GetResPath("start/bar1.png"), 
 		loading_bar_width, loading_bar_height, loading_bg_width, loading_bg_height)
 	self.__percent_prog:SetAnchorPoint(0.5, 0.5)
	layout:AddChild(self.__percent_prog:GetNode())
	PosTool.CenterBottom(self.__percent_prog, 0, 33)

	local effect
	self.UI_loading2, effect_1 = self:__CreateEffect({res_name = "UI_loading2" }, self.__percent_prog:GetNode() )
	effect_1:SetAnchorPoint(0.5,0.5)
	effect_1:SetPosition( 0 , self.__percent_prog:GetNode():GetContentHeight()/2 )

	local effect
	self.UI_loading3, effect_2 = self:__CreateEffect({res_name = "UI_loading3" }, self.__percent_prog:GetNode() )
	effect_2:SetAnchorPoint(1,0.5)
	effect_2:SetPosition( self.__percent_prog:GetNode():GetContentWidth()/2 , self.__percent_prog:GetNode():GetContentHeight()/2 )
	GlobalEffectMgr:SetEffectVisible( self.UI_loading3 , false)

	return layout
end

function Modules.SceneLoadingView:__OpenCallback()
	if self.__timeout_timer then
		GlobalTimerQuest:CancelQuest(self.__timeout_timer)
	end
	self.__timeout_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
	    function()
	    	self:__UpdateProcess(100)
	    	self.__timeout_timer = nil
	    end), Scene.SceneChangeTimeout)

	local bg = nil
	if CPPSdkTools.IsSDKLoadingBG() then
		bg = Resource.PathTool.GetTrdResPath("bg_sceneloading.jpg")
	else
		local bg_index = MathTool.GetRandom(1, 4)
		bg = Resource.PathTool.GetUIResPath("sceneloading/bg" .. bg_index, true)
	end
    self:ResetBG({type = BaseView.BGType.IMAGE, image_path = bg, image_size = COCOSize(640, 1136)})
end

function Modules.SceneLoadingView:__CloseCallback()
	if self.__timeout_timer then
		GlobalTimerQuest:CancelQuest(self.__timeout_timer)
		self.__timeout_timer = nil
	end
	if self.UI_loading2 then
		self:__DelEffect(self.UI_loading2)
		self.UI_loading2 = nil
	end
	if self.UI_loading3 then
		self:__DelEffect(self.UI_loading3)
		self.UI_loading3 = nil
	end
end

--更新进度
function Modules.SceneLoadingView:__UpdateProcess(custom_percent)
	if not self:IsOpen() then
		return
	end

	if not self.__timeout_timer then
		return
	end

	local map_loaded_num, map_need_load_num = GlobalScene:GetMapLoadInfo()
	local sp_res_loaded_num, sp_res_need_load_num = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetResLoadInfo")

	local map_percent = map_need_load_num > 0 and (map_loaded_num / map_need_load_num * PercentTag.MAP) or PercentTag.MAP
	local sp_res_percent = sp_res_need_load_num > 0 and (sp_res_loaded_num / sp_res_need_load_num * PercentTag.SP_RES) or PercentTag.SP_RES

	--设置更新进度
	local percent = map_percent + sp_res_percent
	self.__percent_prog:SetProgress(custom_percent or percent)
	self.__percent_text:SetString(string.format("%d", custom_percent or percent) .. "%")	

	self:Showloading1( custom_percent or percent )
end

function Modules.SceneLoadingView:Update(elapse_time)
	Modules.UILoadingView.Update(self, elapse_time)
	self:__UpdateProcess()
end


function Modules.SceneLoadingView:Showloading1( percent )
	local effect_1 = GlobalEffectMgr:GetEffect(self.UI_loading2)
	if effect_1 then
		effect_1:SetPosition( ( loading_bar_width * (  percent * 0.01 ) ) + 20 , self.__percent_prog:GetNode():GetContentHeight()/2 )
		if loading_bar_width * (  percent * 0.01 ) == loading_bar_width then
			self:Showloading2()
			GlobalEffectMgr:SetEffectVisible( self.UI_loading2 , false)
		end
	end
end

function Modules.SceneLoadingView:Showloading2( percent )
	GlobalEffectMgr:SetEffectVisible( self.UI_loading3 , true)
end