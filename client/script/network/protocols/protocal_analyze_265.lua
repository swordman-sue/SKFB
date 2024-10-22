Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:热卖商品数据请求 消息Id: 26501
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HOT_SALE_GOODS_DATA_REQ] = function()
    Net.CSHotSaleGoodsDataReq = Net.CSHotSaleGoodsDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSHotSaleGoodsDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSHotSaleGoodsDataReq
end

-- 消息说明:热卖商品数据响应 消息Id: 26502
-- goods_id@H      // 商品ID
-- cur_data_idx@H  // 当前数据索引
-- buy_times@H     // 购买次数
-- take_times@H    // 领取次数
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HOT_SALE_GOODS_DATA_RESP] = function()
    Net.SCHotSaleGoodsDataResp = Net.SCHotSaleGoodsDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCHotSaleGoodsDataResp:__init()
        self:AutoConstruct({
            "goods_list__T__goods_id@H##cur_data_idx@H##buy_times@H##take_times@H", -- 商品列表
        })
    end
    return Net.SCHotSaleGoodsDataResp
end

-- 消息说明: 领取热卖商品充值奖励请求 消息Id: 26503
Net.ProtocolRegistry[Net.ProtocolNo.C2S_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_REQ] = function()
    Net.CSTakeHotSaleGoodsRechargeRewardReq = Net.CSTakeHotSaleGoodsRechargeRewardReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSTakeHotSaleGoodsRechargeRewardReq:__init()
        self:AutoConstruct({
            "goods_id__H",      --要领取的商品ID
        })
    end
    return Net.CSTakeHotSaleGoodsRechargeRewardReq
end

-- 消息说明: 领取热卖商品充值奖励响应 消息Id: 26504
Net.ProtocolRegistry[Net.ProtocolNo.S2C_TAKE_HOT_SALE_GOODS_RECHARGE_REWARD_RESP] = function()
    Net.SCTakeHotSaleGoodsRechargeRewardResp = Net.SCTakeHotSaleGoodsRechargeRewardResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCTakeHotSaleGoodsRechargeRewardResp:__init()
        self:AutoConstruct({
            "goods_id__H",      --要领取的商品ID
        })
    end
    return Net.SCTakeHotSaleGoodsRechargeRewardResp
end