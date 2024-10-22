Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 奖励列表请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REWARD_CENTER_LIST_REQ] = function()
    Net.CSRewardCenterListReq = Net.CSRewardCenterListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRewardCenterListReq:__init()
        self:AutoConstruct({

        })
    end
    return Net.CSRewardCenterListReq
end

-- 奖励列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REWARD_CENTER_LIST_RESP] = function()
    Net.SCRewardCenterListResp = Net.SCRewardCenterListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRewardCenterListResp:__init()
        self:AutoConstruct({
            "reward_list__T__id@I##type@C##value@I##time@I##text@s##item_list@D|ItemList|##currency_list@D|CurrencyList|", -- 奖励列 表
                -- id:         奖励Id
                -- type:       类型
                -- value:      数值
                -- time:       时间
                -- text:       文本值
                -- item_list:  奖励的物品列表
                -- currency_list:  奖励的货币列表

                -- 物品说明:
                -- "item_id__I",       --物品ID
                -- "item_num__I",      --物品数量

        })
    end
    return Net.SCRewardCenterListResp
end

-- 奖励领取请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_REWARD_CENTER_GET_REQ] = function()
    Net.CSRewardCenterGetReq = Net.CSRewardCenterGetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRewardCenterGetReq:__init()
        self:AutoConstruct({
            "reward_list__T__id@I", -- 奖励列表
        })
    end
    return Net.CSRewardCenterGetReq
end

-- 奖励领取响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REWARD_CENTER_GET_RESP] = function()
    Net.SCRewardCenterGetResp = Net.SCRewardCenterGetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRewardCenterGetResp:__init()
        self:AutoConstruct({
            "reward_list__T__id@I", -- 奖励列表
        })
    end
    return Net.SCRewardCenterGetResp
end

-- 领奖中心奖励通知
Net.ProtocolRegistry[Net.ProtocolNo.S2C_REWARD_NOTICE] = function()
    Net.SCRewardNoticeResp = Net.SCRewardNoticeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRewardNoticeResp:__init()
        self:AutoConstruct({
            "id__I",        --奖励Id
            "type__C",      --奖励类型
            "value__I",     --奖励值
            "time__I",      --发放时间
            "text__s",      --文本
            "item_list__T__item_id@I##item_num@I", -- 物品奖励列表
            "currency_list__T__type@C##value@I", -- 货币奖励列表


            --type : 货币类型
            --value : 货币值
        })
    end
    return Net.SCRewardNoticeResp
end