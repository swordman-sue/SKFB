
--[[
%% @module: 圣器争夺红点
%% @author: ldx
%% @created: 2019年1月04日
--]]
Modules.Red.Plunderedtreasures = Modules.Red.Plunderedtreasures or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Plunderedtreasures:__init( ... )
end

--红点统计逻辑
function Modules.Red.Plunderedtreasures:__Statistics()
    self.red = 0
    
	--默认显示的和背包有碎片的加入队列中
	local treasuresList = {}
	for treasureId,info in pairs(CPPGameLib.GetConfig("TreasureCompose", nil, nil , true )) do
		if info.is_default_show == 1 then
			table.insert(treasuresList,treasureId)	
		else
			for i,item_id in ipairs(info.cost_item_list) do
				if CallModuleFunc(ModuleType.Pack,"GetCommonGoods",item_id) then
					table.insert(treasuresList,treasureId)	
					break
				end				
			end	
		end		
	end

	--可合成圣器数量
	for index,treasureId in ipairs(treasuresList) do
		local info = CPPGameLib.GetConfig("TreasureCompose",treasureId)
		local is_could_compose = true
		for i,pieceId in ipairs(info.cost_item_list) do
			local pieceNum = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",pieceId)
			if pieceNum < 1 then
				is_could_compose = false
				break
			end
		end

		if is_could_compose then
			self.red = self.red + 1
			if self.red > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
				return
			end		
			break
		end
	end

	if self.red > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
		return
	end

	--计算进度奖励的红点
	self.red = self.red + CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "CountProgressRewardRedDot")
end