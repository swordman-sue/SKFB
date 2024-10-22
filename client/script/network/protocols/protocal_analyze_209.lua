
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 购买请求
Net.ProtocolRegistry[Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_REQ] = function()
    Net.CSShopGoodsBuyReq = Net.CSShopGoodsBuyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopGoodsBuyReq:__init()
        self:AutoConstruct({
            "type__C",  --商品类型
            "param1__H",    --参数1
            "param2__H",    --参数2
            "param3__H",    --参数2
        })
    end
    return Net.CSShopGoodsBuyReq
end

--购买响应: 
Net.ProtocolRegistry[Net.ProtocolNo.MSG_S2C_SHOP_GOODS_BUY_RESP] = function()
    Net.CSShopGoodsBuyResp = Net.CSShopGoodsBuyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopGoodsBuyResp:__init()
        self:AutoConstruct({
            "type__C",  --商品类型
            "param1__H",    --参数1
            "param2__H",    --参数2
            "param3__H",    --参数3
        })
    end
    return Net.CSShopGoodsBuyResp
end

--购买记录请求: 
Net.ProtocolRegistry[Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ] = function()
    Net.CSShopGoodsBuyRecordReq = Net.CSShopGoodsBuyRecordReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopGoodsBuyRecordReq:__init()
        self:AutoConstruct({
            "type__C",  --商品类型
        })
    end
    return Net.CSShopGoodsBuyRecordReq
end

--购买记录响应: 
Net.ProtocolRegistry[Net.ProtocolNo.MSG_S2C_SHOP_GOODS_BUY_RECORD_RESP] = function()
    Net.CSShopGoodsBuyRecordResp = Net.CSShopGoodsBuyRecordResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopGoodsBuyRecordResp:__init()
        self:AutoConstruct({
            "type__C",      --商品类型
            "buy_record_list__T__goods_id@H##buy_num@H", -- 记录数据列表
        })
    end
    return Net.CSShopGoodsBuyRecordResp
end

--商品出售
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SHOP_GOODS_SELL_REQ] = function()
    Net.CSShopGoodsSellReq = Net.CSShopGoodsSellReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopGoodsSellReq:__init()
        self:AutoConstruct({
            "type__C",  --对象类型
            "goods_list__T__goods_id@I", -- 商品列表
        })
    end
    return Net.CSShopGoodsSellReq
end

--商品出售响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SHOP_GOODS_SELL_RESP] = function()
    Net.SCShopGoodsSellResp = Net.SCShopGoodsSellResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCShopGoodsSellResp:__init()
        self:AutoConstruct({
            "type__C",  --对象类型
            "goods_list__T__goods_id@I", -- 商品列表
            "currency_list__T__type@C##value@I", -- 出售获得的货币列表
        })
    end
    return Net.SCShopGoodsSellResp
end


-- 消息说明: 限时商品列表请求 消息Id: 20907
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SHOP_LIMIT_GOODS_LIST_REQ] = function()
    Net.CSShopLimitGoodsListReq = Net.CSShopLimitGoodsListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopLimitGoodsListReq:__init()
        self:AutoConstruct({
            "type__C",  --商品类型
        })
    end
    return Net.CSShopLimitGoodsListReq
end

-- 消息说明: 随时商品列表响应 消息Id: 20408
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SHOP_LIMIT_GOODS_LIST_RESP] = function()
    Net.SCShopLimitGoodsListResp = Net.SCShopLimitGoodsListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCShopLimitGoodsListResp:__init()
        self:AutoConstruct({
            "type__C",      --商品类型
            "last_refresh_time__I", --上次刷新时间
            "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
        })
    end
    return Net.SCShopLimitGoodsListResp
end

-- 消息说明: 刷新商店 消息Id: 20909
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SHOP_REFRESH_REQ] = function()
    Net.CSShopRefreshReq = Net.CSShopRefreshReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopRefreshReq:__init()
        self:AutoConstruct({
            "shop_type__C",     --商店类型
            "refresh_type__C",  --刷新类型(1=使用免费次数，2=使用物品,3=使用雄魂)
        })
    end
    return Net.CSShopRefreshReq
end

-- 消息说明: 刷新商店响应 消息Id: 20910
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SHOP_REFRESH_RESP] = function()
    Net.SCShopRefreshResp = Net.SCShopRefreshResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCShopRefreshResp:__init()
        self:AutoConstruct({
            "shop_type__C",     --商店商品类型
            "refresh_type__C",  --刷新类型(1=使用免费次数，2=使用物品,3=使用雄魂)
        })
    end
    return Net.SCShopRefreshResp
end

-- 消息说明: 英雄商店数据请求 消息Id: 20911
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HERO_SHOP_DATA_REQ] = function()
    Net.CSHeroShopDataReq = Net.CSHeroShopDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHeroShopDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSHeroShopDataReq
end

-- 消息说明: 英雄商店数据响应 消息Id: 20912
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HERO_SHOP_DATA_RESP] = function()
    Net.SCHeroShopDataResp = Net.SCHeroShopDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHeroShopDataResp:__init()
        self:AutoConstruct({
            "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
            "last_recover_time__I",     --上一次恢复时间(免费刷新次数恢复时间)
            "free_refresh_times__C",    --免费刷新次数(已经使用的)
            "cost_refresh_times__C",    --消耗刷新次数(已经使用的)
        })
    end
    return Net.SCHeroShopDataResp
end

-- 消息说明: 觉醒商店数据请求 消息Id: 20913
Net.ProtocolRegistry[Net.ProtocolNo.C2S_AWAKEN_SHOP_DATA_REQ] = function()
    Net.CSAwakenShopDataReq = Net.CSAwakenShopDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSAwakenShopDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSAwakenShopDataReq
end

-- 消息说明: 觉醒商店数据响应 消息Id: 20914
Net.ProtocolRegistry[Net.ProtocolNo.S2C_AWAKEN_SHOP_DATA_RESP] = function()
    Net.SCAwakenShopDataResp = Net.SCAwakenShopDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCAwakenShopDataResp:__init()
        self:AutoConstruct({
            "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
            "last_recover_time__I",     --上一次恢复时间(免费刷新次数恢复时间)
            "free_refresh_times__C",    --免费刷新次数(已经使用的)
            "cost_refresh_times__C",    --消耗刷新次数(已经使用的)
        })
    end
    return Net.SCAwakenShopDataResp
end

-- 消息说明: 星魂商店数据请求 消息Id: 20915
Net.ProtocolRegistry[Net.ProtocolNo.C2S_STAR_SOUL_SHOP_DATA_REQ] = function()
    Net.CSStarSoulShopDataReq = Net.CSStarSoulShopDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSStarSoulShopDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSStarSoulShopDataReq
end

-- 消息说明: 星魂商店数据响应 消息Id: 20916
Net.ProtocolRegistry[Net.ProtocolNo.S2C_STAR_SOUL_SHOP_DATA_RESP] = function()
    Net.SCStarSoulShopDataResp = Net.SCStarSoulShopDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCStarSoulShopDataResp:__init()
        self:AutoConstruct({
            "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
            "last_auto_refresh_time__I",    --最近一次自动刷前时间
            "refresh_times__C", --刷新次数(已经使用的)
        })
    end
    return Net.SCStarSoulShopDataResp
end

-- 消息说明: 直购商店数据请求 消息Id: 20917
Net.ProtocolRegistry[Net.ProtocolNo.C2S_DIRECT_SHOP_DATA_REQ] = function()
    Net.CSDirectShopDataReq = Net.CSDirectShopDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSDirectShopDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSDirectShopDataReq
end

-- 消息说明: 直购商店数据响应 消息Id: 20918
Net.ProtocolRegistry[Net.ProtocolNo.S2C_DIRECT_SHOP_DATA_RESP] = function()
    Net.SCDirectShopDataResp = Net.SCDirectShopDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCDirectShopDataResp:__init()
        self:AutoConstruct({
            "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
            "can_take_goods_list__T__goods_id@H", -- 可领取的商品列表
        })
    end
    return Net.SCDirectShopDataResp
end

-- 消息说明: 领取直购商品奖励请求 消息Id: 20919
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_DIRECT_BUY_GOODS_REQ] = function()
    Net.CSTakeDirectBuyGoodsReq = Net.CSTakeDirectBuyGoodsReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeDirectBuyGoodsReq:__init()
        self:AutoConstruct({
            "goods_id__H",  -- 领取的商品ID
        })
    end
    return Net.CSTakeDirectBuyGoodsReq
end

-- 消息说明: 领取直购商品奖励响应 消息Id: 20920
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_DIRECT_BUY_GOODS_RESP] = function()
    Net.SCTakeDirectBuyGoodsResp = Net.SCTakeDirectBuyGoodsResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeDirectBuyGoodsResp:__init()
        self:AutoConstruct({
            "goods_id__H",  -- 领取的商品ID            
        })
    end
    return Net.SCTakeDirectBuyGoodsResp
end

-- 消息说明: 水晶商店数据请求 消息Id: 20921
Net.ProtocolRegistry[Net.ProtocolNo.C2S_CRYSTAL_SHOP_DATA_REQ] = function()
    Net.CSCrystalShopDataReq = Net.CSCrystalShopDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSCrystalShopDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSCrystalShopDataReq
end

-- 消息说明: 水晶商店数据响应 消息Id: 20922
Net.ProtocolRegistry[Net.ProtocolNo.S2C_CRYSTAL_SHOP_DATA_RESP] = function()
    Net.SCCrystalShopDataResp = Net.SCCrystalShopDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCCrystalShopDataResp:__init()
        self:AutoConstruct({
            "today_refresh_times__H", --今日刷新次数(已经使用的)
            "goods_list__T__goods_id@H##buy_num@H", -- 商品列表
        })
    end
    return Net.SCCrystalShopDataResp
end
