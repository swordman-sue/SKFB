
--[[
%% @module: 叛军副本
%% @author: swordman sue
%% @created: 2016-09-23
--]]

Modules = Modules or {}

Modules.SpRebelModule = Modules.SpRebelModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpRebelModule:__init()
end

function Modules.SpRebelModule:__delete()
end

--初始化
function Modules.SpRebelModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpRebelModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)

	self:__ClearBattleInfo()
end

function Modules.SpRebelModule:Update(now_time, elapse_time)
	Modules.SpMainDungeonModule.Update(self, now_time, elapse_time)
end

--战斗结束处理
function Modules.SpRebelModule:BattleEndHandle(var)
	Modules.SpMainDungeonModule.BattleEndHandle(self, var)

	self:__ClearBattleInfo()
end

--打开胜利界面
function Modules.SpRebelModule:OpenBattleSuccessView()
    CallModuleFunc( ModuleType.BATTLE, "OpenView", BaseViewType.REBEL_RESULT_WIN )
end

--打开失败界面
function Modules.SpRebelModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.SP_BATTLE_RESULT_WIN)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpRebelModule:BattleEndReturnRelevantView()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	CallModuleFunc(ModuleType.REBEL, "OpenView", BaseViewType.REBEL, battle_info.param1)

	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpRebelModule:__StartBattle()
	Modules.SpMainDungeonModule.__StartBattle(self)

	self:__UpdateBattleInfo()
end

--血量变化
function Modules.SpRebelModule:__ObjHPChange(obj_id, cur_hp, change_hp)
	Modules.SpMainDungeonModule.__ObjHPChange(self, obj_id, cur_hp, change_hp)

	self:__UpdateBattleInfo()
end

--更新战斗信息
function Modules.SpRebelModule:__UpdateBattleInfo()
	if not self.__layer_battle_info then
		self.__layer_battle_info = UILayer.New()
		self.__layer_battle_info:SetAnchorPoint(0, 1)
		self.__layer_battle_info:SetContentSize(160 , 66)
		self.__layer_battle_info:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_battle_damage"))
		GlobalScene:AddToRenderGroup(self.__layer_battle_info, Scene.RenderQueue.GRQ_UI_UP)
		PosTool.LeftTop(self.__layer_battle_info, 0, -140)

		self.__lab_battle_info = Util:RichText(nil, Macros.TypefaceSize.slightly)
		self.__lab_battle_info:SetAnchorPoint(0, 0.5)
		self.__lab_battle_info:SetFontSpacing(6)
		self.__layer_battle_info:AddChild(self.__lab_battle_info)
	end

	--伤害、功勋
	local total_damage = CallModuleFunc(ModuleType.BATTLE, "GetTotalDamage")
	local str_damage = GUI.RichText.GetColorWord(tostring(total_damage), Macros.Color.btn_hex , Macros.Color.white_stroke_hex , 2 )
	
	local str_exploit = GUI.RichText.GetColorWord(tostring(  math.ceil(total_damage/1000) ), Macros.Color.btn_hex , Macros.Color.white_stroke_hex , 2 )
	
	self.__lab_battle_info:SetText(
	
									GUI.RichText.GetColorWord(    
																CPPGameLib.GetString("battle_rebel_info", str_damage, str_exploit), 
																Macros.Color.btn_hex , 
																Macros.Color.white_stroke_hex , 2 
																)  
									)
	PosTool.LeftCenter(self.__lab_battle_info, 6)
end

function Modules.SpRebelModule:__ClearBattleInfo()
	if self.__layer_battle_info then
		self.__layer_battle_info:DeleteMe()
		self.__layer_battle_info = nil
	end
end

--全体英雄进入战役区域的回调
function Modules.SpRebelModule:__AllHerosEnterRegionInBirth()
	Modules.SpMainDungeonModule.__AllHerosEnterRegionInBirth(self)

	self:__CheckShowDoubleEffect()
end

--战斗翻倍效果
function Modules.SpRebelModule:__CheckShowDoubleEffect()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	if battle_info and battle_info.battle_index == 1 then
		local center_index = math.floor(#self.__birth_hero_obj_ids / 2)
		local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
		for i, obj_id in ipairs(self.__birth_hero_obj_ids) do
			obj = GlobalScene:GetObj(obj_id)
			if obj then
				obj:ShowDoubleEffect(i == center_index, battle_info.param2 == Macros.Game.RebelAttackType.FULL)
			end
		end	
	end
end