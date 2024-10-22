
ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Effect = ConfigAdapter.Effect or {}

function ConfigAdapter.Effect.GetEffectFiles(effect_id)
	local files_map
	local config_effect = CPPGameLib.GetConfig("Effect", effect_id, false)
	if config_effect then	
		for _, file_name in ipairs(config_effect.res_name_list) do
			files_map = files_map or {}
			files_map[file_name] = true
		end
	end
	return files_map
end

