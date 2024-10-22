--[[
%% @module: 首冲入口
%% @author: ldx
%% @created: 2018-12-21
--]]

Modules = Modules or {}

Modules.FirstRechargeEntry = Modules.FirstRechargeEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.FirstRechargeEntry:__init()
	self:SetFirstRechargeBtn()
end

function Modules.FirstRechargeEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)
	
	--首充按钮刷新
	self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE, function()
		self:SetFirstRechargeBtn()
	end)
end

--设置首充按钮
function Modules.FirstRechargeEntry:SetFirstRechargeBtn(system_id)
	--入口控件
	local system_id = system_id or Macros.Game.SystemID.FIRST_RECHARGE
	local entry_id = CallModuleFunc(ModuleType.MAIN_CITY, "TranslateSystemIDToEntryID",system_id)
	local entry_info = CPPGameLib.GetConfig("CityIcon",entry_id,false)

	local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")
	local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
	table.sort( t1, function ( a , b )
		return a.context < b.context
	end )
	local index = 0
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")

    table.sort( t1 , function ( a , b )
        return a.context < b.context
    end )
    for i , v in ipairs( t1 ) do
        local record = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeDataRecord" , v.id )
        if not record then
        	index = i
            break
        end
	end

	local module_txt = nil
	local sp = Resource.PathTool.GetSystemEntryIconPath( self.__entry_info.res_name)
	if index == 1 then
		sp = Resource.PathTool.GetSystemEntryIconPath( "btn_first_recharge_1" )
		module_txt = CPPGameLib.GetString( "activity_recharge_hero" )
	elseif index == 2 then
		sp = Resource.PathTool.GetSystemEntryIconPath( "btn_first_recharge_1" )
		module_txt = CPPGameLib.GetString( "activity_recharge_hero2" )
	elseif index == 3 then
		sp = Resource.PathTool.GetSystemEntryIconPath( "btn_first_recharge_2")
		module_txt = CPPGameLib.GetString( "activity_recharge_equip" )
	else
		sp = Resource.PathTool.GetSystemEntryIconPath( entry_info.res_name )
		module_txt = entry_info.module_txt
	end

	if module_txt then
		self.__entry_txt:SetString(module_txt)
	end

	--首充特效特效
	self:CreateEffect("UI_shouchongtubiao")	

	self.__entry_btn:SetNormalImage(sp , TextureResType.LOCAL)	
end

--设置首充按钮
function Modules.FirstRechargeEntry:CreateEffect(res_name)
	if not self:IsEffectExist(res_name) then
		--首充特效特效		
		local effect
		self.__entry_effect_map[res_name],effect = self:__CreateEffect({res_name = "UI_shouchongtubiao"}, self.__entry_btn , 99 )
		PosTool.Center( effect, 0 , -6)
	end
end

function Modules.FirstRechargeEntry:__HandleEntryTouch()
	CallModuleFunc(ModuleType.CHARGE , "SetFirstRechargeRed" , 0 )

	Modules.BaseMainCityEntry.__HandleEntryTouch(self)
end

function Modules.FirstRechargeEntry:__ModuleTxtSpHandle()
	local module_txt = self.__entry_info.module_txt
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")
    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
    local index = 0
    for i , v in ipairs( t1 ) do
	    if max >= v.context then
	    	index = index + 1
	    end
    end

    if index == 0 then
    	module_txt = CPPGameLib.GetString( "activity_recharge_hero" )
    elseif index == 1 then
    	module_txt = CPPGameLib.GetString( "activity_recharge_equip" )
    end		
    return module_txt	
end