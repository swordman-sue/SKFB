--
-- @brief 充值ID映射表
--

Modules = Modules or {}

Modules.ChargeModule = Modules.ChargeModule or BaseClass(BaseModule)

--获取充值ID
function Modules.ChargeModule.GetRechargeID(recharge_id)
    local cp_channel_type = CPPSdkTools.GetCPChannelType()

    --此处的sub_channel只为兼容旧配置表，并不是实际子渠道类型
    local sub_channel_type_map = CPPGameLib.GetConfig("AppleIAPIDMap", cp_channel_type, false)
    if not sub_channel_type_map then
        --CP商品ID
        return recharge_id
    end

    local recharge_id_map = sub_channel_type_map[1]
    if not recharge_id_map then
        --CP商品ID
        return recharge_id
    end

    local iap_recharge_info = recharge_id_map[recharge_id]
    if not iap_recharge_info then
        --CP商品ID
        return recharge_id
    end

    --苹果内购/谷歌内购商品ID
    return iap_recharge_info.iap_recharge_id
end

--获取充值ID列表
function Modules.ChargeModule.GetRechargeIDMap()
    local cp_channel_type = CPPSdkTools.GetCPChannelType()

    --此处的sub_channel只为兼容旧配置表，并不是实际子渠道类型
    local sub_channel_type_map = CPPGameLib.GetConfig("AppleIAPIDMap", cp_channel_type, false)
    if not sub_channel_type_map then
        return
    end

    return sub_channel_type_map[1]
end

--获取充值金额(默认根据用户货币类型获取对应金额)
function Modules.ChargeModule.GetMoney(config, money_key, currency_type)
    currency_type = currency_type or CPPSystemTools.GetCurrencyType()
    local suffix = currency_type == Macros.Game.CashCurrencyType.CNY and "" or "_"..currency_type
    return config[string.format("%s%s", money_key, suffix)] or config[money_key]
end

