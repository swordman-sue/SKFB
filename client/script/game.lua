--[[
%% @module: 逻辑层游戏模块
%% @author: swordman sue
%% @created: 2016-07-07
--]]

Game = Game or BaseClass()

Game.LuaFilesForEnterInitState = 
{
	--进入初始化状态所需文件(加载以下文件，加载完成后，进入GameState.InitState)
	"core/basemodule",	
	"core/modulesmgr",
	"core/baseview",
	"core/basetabview",
	"core/baseviewmgr",
	"core/effect/effect",
	"core/effect/effectmgr",	
	"core/scene/camera",
	"core/scene/map",	
	"core/scene/scene",
	"core/scene/scene_res",
	"core/gamestate/basestate",
	"core/gamestate/initstate",
	"core/gamestate/scenechangestate",
	"core/gamestate/gameplaystate",
	"core/gamestate/statemgr",	
	"core/sound/soundmgr",

	"gui/multi_resolution_adapter",
	"gui/template/template",
	"gui/template/templatemgr",
	"gui/progressbarlayer",
	"gui/messagetip",
	"gui/richtxt",

	"modules/modulesevents",
	"modules/modulesprotocols",
	--定制化逻辑适配相关
	"modules/customized_logic_adapter/customized_logic_adapter_base",
	"modules/customized_logic_adapter/customized_logic_adapter_mgr",
	--系统模块按需加载....	

	"network/netadapter",
	"network/protocoltool/protocolstruct",
	"network/protocoltool/protocolcustom",
	"network/protocoltool/protocolpool",
	"network/protocols/protocols_no",
	"network/protocols/protocols_no_relation",

	"resource/loadtool",
	"resource/uitextureatlas",

	"macros/game",

	"configadapter/map",

	--加载配置适配器与配置
	"configadapter/common",
	
	--通用dialog
	"lgui/tips/templ_affirm_tips",	
}

Game.LuaFilesForEnterSceneChangeState = 
{
	--游戏所需文件(进入GameState.InitState后，加载以下文件。加载完成且处于GameState.EnterGameState，则进入GameState.SceneChangeState)

	--渲染对象
	"core/render/renderobj",
	"core/render/seqframerender",
	"core/render/skeletonrender",
	
	--AI相关	
	"core/ai/baseai",
	"core/ai/heroai",
	"core/ai/heropvpai",
	"core/ai/monsterai",
	"core/ai/spiritai",
	"core/ai/mtmonsterai",
	"core/ai/mtspiritai",
	"core/ai/aimgr",

	--场景相关
	"core/scene/vo/vomgr",
	"core/scene/vo/objvo",
	"core/scene/vo/battleobjvo",
	"core/scene/vo/herovo",
	"core/scene/vo/heromirrorvo",
	"core/scene/vo/heroassistvo",
	"core/scene/vo/heropvpvo",
	"core/scene/vo/monstervo",
	"core/scene/vo/monsterpvpvo",
	"core/scene/vo/bulletvo",
	"core/scene/vo/dropboxvo",
	"core/scene/vo/spiritvo",
	"core/scene/vo/mtherovo",
	"core/scene/vo/mtspiritvo",
	"core/scene/vo/mtmonstervo",	
	"core/scene/vo/mtitemvo",
	"core/scene/obj/obj",
	"core/scene/obj/objsm",
	"core/scene/obj/battleobj",
	"core/scene/obj/battleobjsm",
	"core/scene/obj/hero",
	"core/scene/obj/herosm",	
	"core/scene/obj/heroassist",
	"core/scene/obj/heroassistsm",	
	"core/scene/obj/heropvp",
	"core/scene/obj/heropvpsm",	
	"core/scene/obj/mthero",
	"core/scene/obj/mtherosm",
	"core/scene/obj/monster",
	"core/scene/obj/mtmonster",
	"core/scene/obj/monstersm",		
	"core/scene/obj/monsterpvp",
	"core/scene/obj/monsterpvpsm",		
	"core/scene/obj/xmonster",
	"core/scene/obj/xmonstersm",	
	"core/scene/obj/mtmonster",
	"core/scene/obj/mtmonstersm",	
	"core/scene/obj/bullet",
	"core/scene/obj/bulletsm",		
	"core/scene/obj/dropbox",
	"core/scene/obj/dropboxsm",			
	"core/scene/obj/spirit",
	"core/scene/obj/spiritsm",			
	"core/scene/obj/mtitem",
	"core/scene/obj/mtitemsm",			
	"core/scene/obj/mtspirit",
	"core/scene/obj/mtspiritsm",			

	--BUFF相关
	"core/buff/basebuff",
	"core/buff/buff_mgr",
	"core/buff/buff_exile",
	"core/buff/buff_frozen",
	"core/buff/buff_silent",
	"core/buff/buff_suck_blood",
	"core/buff/buff_totems",
	"core/buff/buff_treatment",
	"core/buff/buff_vertigo",
	"core/buff/buff_absorption_shield",
	"core/buff/buff_attribute",
	"core/buff/buff_angerreply",
	"core/buff/buff_purge",

	--技能相关
	"core/skill/skill_logic",
	"core/skill/sp_skill_base",
	"core/skill/sp_skill_anger_joint",
	"core/skill/sp_skill_sprite",

	--通用控件(逻辑无关)
	"gui/imgfont",	
	"gui/touchctrl",
	"gui/feigncheckbox",
	"gui/feigncheckboxoflist",
	"gui/scroll/scrollitem",	
	"gui/scroll/scrollview",
	"gui/scroll/special_scrollview",
	"gui/scroll/arena_scrollview",
	"gui/scroll/pageview",

	"gui/shield_layer",

	--通用控件(逻辑相关)
	"lgui/icon_discount",
	"lgui/icon_layer",
	"lgui/model",	
	"lgui/titlebar_layer",
	"lgui/icon_item",
	"lgui/templ_equip_or_treasure_lift_item",

	--物品tips
	"lgui/pack/templ_item_info",
	--英雄tips
	"lgui/hero/templ_hero_info",
	--装备tips
	"lgui/equip/templ_equip_info",	

	"lgui/equip/debrisobtain_info_layer",
	"lgui/equip/equipobtain_info_layer",
	"lgui/equip/templ_equip_one_key",	
	"lgui/equip/templ_equip_master",

	"lgui/hero/templ_hero_skill_tips",
	
	"lgui/select_list_layer",
	--字体特效
	"lgui/effects/typeface_effects/typeface_effects",	
	--飘字特效
	"lgui/effects/windwords_effects",				
	--快捷购买
	"lgui/shop_part/templ_consume_buy",  	

	--聊天相关
	"lgui/chat/maincity_chat_layer",

	--精灵技能提示奖励
	"lgui/spirit/templ_spirit_skill_tips",	

	--获取途径条目
	"lgui/approach_of_achieving/approach_of_achieving_item",

	--加载配置适配器与配置
	"configadapter/common",
	"configadapter/rolelevel",
	"configadapter/skill",
	"configadapter/dungeon",
	"configadapter/gamelevel",
	"configadapter/battle",
	"configadapter/hero",
	"configadapter/item",
	"configadapter/equip",
	"configadapter/treasure",
	"configadapter/forge",
	"configadapter/spirit",
	"configadapter/effect",
	"configadapter/monster",
	"configadapter/sundry",

	Resource.PathTool.GetCnfPath("config_story"),
	Resource.PathTool.GetCnfPath("config_lead"),
	Resource.PathTool.GetCnfPath("config_lead_operation"),
	Resource.PathTool.GetCnfPath("config_necessary_lead"),
	Resource.PathTool.GetCnfPath("config_shield"),
	Resource.PathTool.GetCnfPath("config_effect"),
	Resource.PathTool.GetCnfPath("config_view_sound"),	

	--战斗相关
	"modules/battle/battle_formula",		
}

Game.LuaFilesForEnterSceneChangeStateII = 
{
	Resource.PathTool.GetCnfPath("config_skill"),
	Resource.PathTool.GetCnfPath("config_skill_buff"),
	Resource.PathTool.GetCnfPath("config_skill_level"),
	Resource.PathTool.GetCnfPath("config_monster"),
	Resource.PathTool.GetCnfPath("config_monster_ai"),
	Resource.PathTool.GetCnfPath("config_game_level"),
	Resource.PathTool.GetCnfPath("config_battle"),

	Resource.PathTool.GetCnfPath("config_item"),
	Resource.PathTool.GetCnfPath("config_hero"),
	Resource.PathTool.GetCnfPath("config_hero_level"),
	Resource.PathTool.GetCnfPath("config_hero_break"),
	Resource.PathTool.GetCnfPath("config_hero_awakening"),
	Resource.PathTool.GetCnfPath("config_hero_property"),	
}

local function define_common_tex_and_templ()
	--公共贴图集
	Game.CommonTexture = Game.CommonTexture or
	{
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON, true),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON_BG),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON_BTN),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON_PROGRESS),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON_ICON),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.COMMON_MONEYICON),

		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_NEW),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_NEW, true),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, true),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, true),
		Resource.UITextureAtlas.GetTexAtlasInfo(Resource.UITextureAtlasType.CHAT_EMOTION_ICON),
	}
	--公共模板(注：以下模板csb中使用的资源必须在公共贴图集中)
	Game.CommonTempl = Game.CommonTempl or
	{
		GUI.TemplateRegistry.ITEM_INFO,
		GUI.TemplateRegistry.TEMPL_EQUIP_INFO_PAGE,
		GUI.TemplateRegistry.TEMPL_EQUIP_DETAIY_ITEM_PROPERTY,
		GUI.TemplateRegistry.HERO_INFO,
		GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGCELL,
	}
	
	--主城公共贴图集
	Game.CityCommonTexture = Game.CityCommonTexture or
	{
	}
	--主城公共模板(注：以下模板csb中使用的资源必须在主城公共贴图集中)
	Game.CityCommonTempl = Game.CityCommonTempl or
	{
	}
end

function Game.CreateInstance()
	if not GlobalGame then
		GlobalGame = Game.New()
	end
end

function Game.DelInstance()
	if GlobalGame then
		GlobalGame:DeleteMe()
		GlobalGame = nil
	end
end

function Game:__init()	
	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.LOAD_INIT_SCRIPT_I)
	
	--加载初始LUA文件
	GlobalRequireToolBatch:LoadFiles(
		{file_list = Game.LuaFilesForEnterInitState, callback = function() self:__EnterInitState() end})

	--SDKInit回调
	CPPSdkTools.SetLuaInitCallback(GlobalCallbackMgr:AddCallBackFunc(function(code)		
	end))

	--SDKCommonHandle回调
	CPPSdkTools.SetLuaCommonHandleCallback(GlobalCallbackMgr:AddCallBackFunc(function(code, str)
		CPPSdkTools.HandleCommonHandleCallback(code, str)		
	end))

	--SDKPay回调
    CPPSdkTools.SetLuaPayCallback(GlobalCallbackMgr:AddCallBackFunc(function(code, json_str)
    	CPPSdkTools.HandlePayHandleCallback(code, json_str)
    end))	
end

function Game:__delete()
	self:__UnloadBaseKeepRes()	
	
	if self.__enter_init_state then
		--销毁界面
		GlobalViewMgr:Dump(true)

		--销毁模块管理器
		Modules.ModulesMgr.DelInstance()

		--销毁界面管理器
		BaseViewMgr.DelInstance()

		--销毁模板管理器
		GUI.TemplateMgr.DelInstance()

		--销毁场景管理器
		Scene.Scene.DelInstance()
		if self.__enter_scenechange_state then
			Buff.BuffMgr.DelInstance()
			AI.AIManager.DelInstance()
			Scene.VOMgr.DelInstance()
		end		

		--销毁特效管理器
		Effect.EffectMgr.DelInstance()

		--销毁动作管理器
		ActionManager.DelInstance()

		--销毁异步加载管理器
		AsyncLoadMgr.DelInstance()

		--销毁状态管理器
		GameState.StateMgr.DelInstance()
	
		--销毁网络适配器
		Net.NetAdapter.DelInstance()
	
		--销毁事件系统
		EventSystem.DelInstance()
	
		--销毁音频管理器
		SoundMgr.DelInstance()
	end

	self:ReleaseResource(true, true)

	CPPSdkTools.SetLuaInitCallback(0)
	CPPSdkTools.SetLuaCommonHandleCallback(0)
end

function Game:Update(now_time, elapse_time)
	if self.__release_momery_mark then
		self:__ReleaseUnusedResource()
		self.__release_momery_mark = false
	end

	HttpAutoRetryFunc(now_time, elapse_time)
end

--能否进入切换场景状态
function Game:CanEnterSceneChangeState()
	return self.__enter_scenechange_state
end

--加载初始LUA文件(进入切换场景状态所需)
function Game:LoadLuaFilesForEnterSceneChangeState(finished_callback)
	GlobalRequireToolBatch:LoadFiles(
	{
		file_list = Game.LuaFilesForEnterSceneChangeState, 
		callback = function() 		
			self:__EnterSceneChangeState() 

			if finished_callback then
				finished_callback()
			end
		end
	})
end

--加载后续LUA文件
function Game:LoadLuaFilesForEnterSceneChangeStateII()
	GlobalRequireToolBatch:LoadFiles(
	{
		file_list = Game.LuaFilesForEnterSceneChangeStateII
	})
end

--加载公共资源
function Game:LoadBaseKeepRes(finished_callback)
	define_common_tex_and_templ()

	self.__loaded_base_tex = 0
	self.__loaded_base_templ = 0

	local function load_tex_callback()
		self.__loaded_base_tex = self.__loaded_base_tex + 1
		if self.__loaded_base_tex == #Game.CommonTexture then

			--加载模板
			local function load_template_callback()
				self.__loaded_base_templ = self.__loaded_base_templ + 1
				if self.__loaded_base_templ == #Game.CommonTempl then
					
					--加载完成回调
					if finished_callback then
						finished_callback()
					end
				end
			end
			if #Game.CommonTempl > 0 then
				for _, templ_registry_info in ipairs(Game.CommonTempl) do
					GUITemplateMgr:AddCommonLayout(templ_registry_info[1], Resource.PathTool.GetUILayoutPath(templ_registry_info[2]))
				end
				GUITemplateMgr:LoadAllCommonLayout(load_template_callback)
			else
				self.__loaded_base_templ = -1
				load_template_callback()
			end
		end
	end
	
	--加载贴图集
	if #Game.CommonTexture > 0 then
		for _, tex_atlas_info in ipairs(Game.CommonTexture) do
			Resource.LoadTool.LoadUITextureAtlas(tex_atlas_info, load_tex_callback)

			self.__base_keep_list = self.__base_keep_list or {}
			table.insert(self.__base_keep_list, tex_atlas_info.texture_path)		
		end
	else
		self.__loaded_base_tex = -1
		load_tex_callback()
	end
end

--获取公共资源加载进度
function Game:GetBaseKeepResLoadProg()
	if not Game.CommonTexture or not Game.CommonTempl then
		return 0, 0
	end

	return (self.__loaded_base_tex or 0) + (self.__loaded_base_templ or 0), #Game.CommonTexture + #Game.CommonTempl
end

--释放资源
function Game:ReleaseResource(flush, by_main_stop)
	self.__release_momery_mark = true
	self.__release_by_main_stop = by_main_stop ~= nil and by_main_stop or false
	
	if flush then
		self:__ReleaseUnusedResource()
		self.__release_momery_mark = false
	end
end

--进入前台
function Game:OnEnterForeground()
	if GlobalModulesMgr then
		CallModuleFunc(ModuleType.LOGIN, "DoAfterEnterForeground")
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoAfterEnterForeground")
	end

	if GlobalSoundMgr then
		GlobalSoundMgr:SetBGMEnabled()
	end
end

--进入后台
function Game:OnEnterBackground()
	if GlobalModulesMgr then
		CallModuleFunc(ModuleType.LOGIN, "DoAfterEnterBackground")
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoAfterEnterBackground")
	end
end

--重新登陆
function Game:Relogin()
	--暂停事件派发
	GlobalEventSystem:PauseFireEvent()

	--销毁界面与界面栈
	GlobalViewMgr:Dump(true)

	--销毁所有功能模块
	GlobalModulesMgr:Dump()

	--恢复事件派发
	GlobalEventSystem:ResumeFireEvent()

	--切换到连接服务器状态
	GlobalStateMgr:ToConnectServerState()			
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--加载完成第一批LUA初始文件
function Game:__EnterInitState()	
	--初始化音频管理器
	SoundMgr.CreateInstance()

	--初始化事件系统
	EventSystem.CreateInstance()

	--初始化网络适配器
	Net.NetAdapter.CreateInstance()	

	--初始化状态管理器
	GameState.StateMgr.CreateInstance()

	--初始化模板管理器
	GUI.TemplateMgr.CreateInstance()	

	--初始化界面管理器
	BaseViewMgr.CreateInstance()	

	--初始化特效管理器
	Effect.EffectMgr.CreateInstance()

	--初始化场景管理器
	Scene.Scene.CreateInstance()

	--初始化模块管理器
	Modules.ModulesMgr.CreateInstance()

	self.__enter_init_state = true

	if CPPSdkTools.CheckUpdateVersion() then
		--二次强更检测
		CPPSdkTools.DoSdkUpdateVersion()
	else
		local function start_callback()
			--切换到连接服务器状态
			GlobalStateMgr:ToConnectServerState()
		end
		GlobalStateMgr:Start(start_callback)
	end
end

--加载完成第二批LUA初始文件
function Game:__EnterSceneChangeState()
	--初始化敏感词库
	LanguageFilter.InitMaskData()
	
	--初始化VO管理器
	Scene.VOMgr.CreateInstance()
	--初始化AI管理器
	AI.AIManager.CreateInstance()
	--初始化BUFF管理器
	Buff.BuffMgr.CreateInstance()
	--初始化UI适配管理器
	CustomizedLogicAdapter.Manager.CreateInstance()

	self.__enter_scenechange_state = true	
end

--释放无用的资源
function Game:__ReleaseUnusedResource()
	local keep_list = {}
	local tmp_keep_list

	if not self.__release_by_main_stop then
		local base_keep_list = self:__GetBaseKeepRes()
		local splogic_keep_list = self:__GetSplogicKeepRes()
		local view_keep_list = GlobalViewMgr:GetKeepRes()
		tmp_keep_list = CPPGameLib.MergeTable2List(base_keep_list, splogic_keep_list)		
		tmp_keep_list = CPPGameLib.MergeTable2List(tmp_keep_list, view_keep_list)
	end

	local record_map
	if tmp_keep_list then
		for i, v in ipairs(tmp_keep_list) do
			record_map = record_map or {}
			if not record_map[v] then
				record_map[v] = true
				table.insert(keep_list, v)
			end
		end
	end

	CPPViewHelper.ReleaseUnusedImageExcept(keep_list, #keep_list, false, self.__release_by_main_stop)

	--测试打印
	if StartConfig.IsDebug and not self.__release_by_main_stop then
		GlobalViewMgr:PrintCache()
		GUITemplateMgr:PrintCache()
		GlobalEffectMgr:PrintCache()

		LogicGUI.Model.PrintCache()
		GUI.ScrollView.PrintCache()
	end
end

--获取公共资源列表(进游戏后常驻内存)
function Game:__GetBaseKeepRes()
	return self.__base_keep_list
end

--获取特殊资源列表(特殊逻辑中常驻内存)
function Game:__GetSplogicKeepRes()
	local splogic_keep_list = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetKeepRes")
	return splogic_keep_list
end

--卸载公共资源
function Game:__UnloadBaseKeepRes()
	self.__base_keep_list = nil
	self.__loaded_base_tex = 0
	self.__loaded_base_templ = 0

	--贴图集
	if Game.CommonTexture then
		for _, tex_atlas_info in ipairs(Game.CommonTexture) do
			Resource.LoadTool.UnloadUITextureAtlas(tex_atlas_info)
		end
	end

	--模板
	if Game.CommonTempl then
		for _, templ_registry_info in ipairs(Game.CommonTempl) do
			GUITemplateMgr:UnLoadAndRemoveCommonLayout(templ_registry_info[1])
		end
	end
end


   