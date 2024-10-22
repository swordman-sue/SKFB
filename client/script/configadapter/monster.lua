ConfigAdapter = ConfigAdapter or {}

ConfigAdapter.Monster = ConfigAdapter.Monster or {}

--初始化战斗属性字段
function ConfigAdapter.Monster.InitBattlePropKey(config_name, config)
    if not config then
        return
    end

    local config_key
    for k, v in pairs(Macros.Game.BattlePropKey) do
        if k ~= Macros.Game.BattleProp.HP then
            config[v] = config[v] or 0
        end
    end
end
