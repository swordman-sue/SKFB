
--[[
%% @module: 英雄副本界面
%% @author: swordman sue
%% @created: 2016-10-27
--]]

Modules = Modules or {}

Modules.HeroDungeonTabView = Modules.HeroDungeonTabView or BaseClass(BaseTabView)

function Modules.HeroDungeonTabView:__init()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_CHAPTER)
end

--选项卡界面创建完成回调
function Modules.HeroDungeonTabView:__LoadedCallback()
	--创建章节列表
    local panel_chapters = self:GetWidget("Panel_Chapters")
    local item_w = 214
    local item_s_c = 10


    local class = Modules.TemplHeroDungeonChapter

    local params = 
    {
        item_class = class ,
        item_width = item_w ,
        item_height = 400 ,
        row = 1,
        col = 5,
        item_space_c = item_s_c,
        view_width = 1110, 
        horizon = true,
        selectable = true,
        select_callback = function(item_index, item_data)
        	local item = self.__chapters_scrollview:GetItem(item_index)
        	if not item then
        		return
        	end        	
			CPPActionManager.ScaleTo(item:GetNode(), 0.25,1.2,GlobalCallbackMgr:AddCallBackFunc(function()
				self:CallModuleFunc("OpenView", BaseViewType.HERO_DUNGEON_GAMELEVEL, item_data.index)
			end))				       	
        end,
    }
    self.__chapters_scrollview = GUI.SpecialScrollView.New(params)
    panel_chapters:AddChild(self.__chapters_scrollview:GetNode())
    PosTool.Center(self.__chapters_scrollview)	
    	
	--英雄特殊关卡居中检测
	self:BindGlobalEvent(Macros.Event.DungeonModule.HERO_DUNGEON_GAMELEVEL_CENTER_CHECK_REQ, function(_,index)
		if self.__chapters_scrollview then
			self.__chapters_scrollview:CheckItemIsOnScreenCenterByIndex(index)
		end
	end)
end

--选项卡界面打开回调
function Modules.HeroDungeonTabView:__OpenCallback()
	if not self.__is_data_inited then
		self:__HandleDungeonData()
	else
		self:__LocateChapter()
	end
end

--选项卡界面关闭回调
function Modules.HeroDungeonTabView:__CloseCallback()
	self.__chapters_scrollview:UnSelectItem()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroDungeonTabView:__Dispose()
	self.__is_data_inited = false

	if self.__chapters_scrollview then
		self.__chapters_scrollview:DeleteMe()
		self.__chapters_scrollview = nil
	end	
end

--处理副本数据
function Modules.HeroDungeonTabView:__HandleDungeonData()
	local chapter_show_list = self:CallModuleFunc("GetChapterShowList", Macros.Game.FBType.HERO_DUNGEON)
	if not chapter_show_list then
		return
	end
	if self.__is_data_inited then
		return
	end
	self.__is_data_inited = true

	--今日可挑战次数
	local extra_data = self:CallModuleFunc("GetExtraInfo", Macros.Game.FBType.HERO_DUNGEON)
	local max_times = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_TRANSCRIPT)
	local remain_times = math.max(0, max_times - extra_data.battle_times)
	Util:WidgetLabel(self:GetWidget("Text_HeroDungeon_CanChallengeTimes"), CPPGameLib.GetString("dungeon_can_challenge_times", remain_times, max_times))

    self.__chapters_scrollview:SetDataList(chapter_show_list)
    self:__LocateChapter()
end

--定位章节(居中最新章节)
function Modules.HeroDungeonTabView:__LocateChapter()
	local chapter_list = self:CallModuleFunc("GetChapterList", Macros.Game.FBType.HERO_DUNGEON)
	--设置解锁副本的章数
	self.__chapters_scrollview:SetNecessaryData(#chapter_list)

	--默认选中上次章节
	local chapter_index = self:CallModuleFunc("GetOpenChapterIndex", Macros.Game.FBType.HERO_DUNGEON)

	--将索引为Index的条目移动到屏幕中心
	self.__chapters_scrollview:MoveItemToScreenCenterByIndex(chapter_index)	
	self.__chapters_scrollview:SelectItem(chapter_index)	
end
