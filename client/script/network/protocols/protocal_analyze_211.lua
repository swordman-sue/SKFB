
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--消息说明: 请求物品列表: 消息Id: 21101
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ITEM_LIST_REQ] = function()
    Net.CSItemListReq = Net.CSItemListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSItemListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSItemListReq
end

--消息说明: 物品列表响应: 消息Id: 22102
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ITEM_LIST_RESP] = function()
    Net.SCItemListResp = Net.SCItemListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCItemListResp:__init()
        self:AutoConstruct({
            "item_list__T__item_id@I##item_num@I", -- 物品列表
        })
    end
    return Net.SCItemListResp
end

--物品使用请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ITEM_USE_REQ] = function()
    Net.CSItemUseReq = Net.CSItemUseReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSItemUseReq:__init()
        self:AutoConstruct({
            "item_id__I",   --物品ID
            "use_num__H",   --使用数量
            "select_gift_id__I",    --选择的礼包物品ID 
        })
    end
    return Net.CSItemUseReq
end

--物品使用响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ITEM_USE_RESP] = function()
    Net.SCItemUseResp = Net.SCItemUseResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCItemUseResp:__init()
        self:AutoConstruct({
            "item_id__I",   --物品ID
            "use_num__H",   --使用数量
            "item_list__T__item_id@I##item_num@I", -- 获得的物品列表
            "money_list__T__type@C##value@I", -- 获得的货币列表
        })
    end
    return Net.SCItemUseResp
end

--物品更新: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ITEM_DATA_NOTIFY] = function()
    Net.SCItemDataNotify = Net.SCItemDataNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCItemDataNotify:__init()
        self:AutoConstruct({
            "item_id__I",   --物品ID
            "use_num__I",   --物品数量
            "flag__C",      --操作标志(0=减少,1=添加)
        })
    end
    return Net.SCItemDataNotify
end

--翻牌请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CHOOSE_CARD_REQ] = function()
    Net.CSChooseCardReq = Net.CSChooseCardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSChooseCardReq:__init()
        self:AutoConstruct({
            "fp_type__C",       --翻牌类型
        })
    end
    return Net.CSChooseCardReq
end

--翻牌请求响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CHOOSE_CARD_RESP] = function()
    Net.SCChooseCardResp = Net.SCChooseCardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCChooseCardResp:__init()
        self:AutoConstruct({
            "fp_type__C",       --翻牌的类型
            "fp_item_id__I",    --翻牌的物品ID
            "create_item_id__I",    --产生的物品ID
            "create_item_num__I",   --产生的物品数量
            "currency_type__C", --产生的货币类型
            "currency_value__I",    --产生的物品值
        })
    end
    return Net.SCChooseCardResp
end

--物品合成请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ITEM_COMPOSE_REQ] = function()
    Net.CSItemComposeReq = Net.CSItemComposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSItemComposeReq:__init()
        self:AutoConstruct({
            "item_id__H",       --合成的物品ID
        })
    end
    return Net.CSItemComposeReq
end

--物品合成响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ITEM_COMPOSE_RESP] = function()
    Net.SCItemComposeResp = Net.SCItemComposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCItemComposeResp:__init()
        self:AutoConstruct({
            "item_id__H",       --合成的物品ID
        })
    end
    return Net.SCItemComposeResp
end

--物品分解请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_ITEM_DECOMPOSE_REQ] = function()
    Net.CSItemDecomposeReq = Net.CSItemDecomposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSItemDecomposeReq:__init()
        self:AutoConstruct({
            "item_id__H",       --分解的物品ID
            "item_num__H",      --分解的物品数量
        })
    end
    return Net.CSItemDecomposeReq
end

--物品分解响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_ITEM_DECOMPOSE_RESP] = function()
    Net.SCItemDecomposeResp = Net.SCItemDecomposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCItemDecomposeResp:__init()
        self:AutoConstruct({
            "item_id__H",       --分解的物品ID
            "item_num__H",      --分解的物品数量
        })
    end
    return Net.SCItemDecomposeResp
end