Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}


-- 消息说明:unlock活动数据请求 消息Id: 26801
Net.ProtocolRegistry[Net.ProtocolNo.C2S_UNLOCK_ACTIVITY_DATA_REQ] = function()
    Net.CSUnlockActivityDataReq = Net.CSUnlockActivityDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSUnlockActivityDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSUnlockActivityDataReq
end

-- 消息说明:unlock活动数据响应 消息Id: 26802
Net.ProtocolRegistry[Net.ProtocolNo.S2C_UNLOCK_ACTIVITY_DATA_RESP] = function()
    Net.SCUnlockActivityDataResp = Net.SCUnlockActivityDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCUnlockActivityDataResp:__init()
        self:AutoConstruct({
            "is_score_for_shop__H", --是否已对商城评分
            "watching_video_times__H", --观年视频次数
        })
    end
    return Net.SCUnlockActivityDataResp
end

-- 消息说明:商城评分完成通知请求 消息Id: 26803
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SHOP_SCORE_FINISH_REQ] = function()
    Net.CSShopScoreFinishReq = Net.CSShopScoreFinishReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSShopScoreFinishReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSShopScoreFinishReq
end

-- 消息说明:商城评分完成通知响应 消息Id: 26804
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SHOP_SCORE_FINISH_RESP] = function()
    Net.SCShopScoreFinishResp = Net.SCShopScoreFinishResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCShopScoreFinishResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCShopScoreFinishResp
end

-- 消息说明:观看视频通知请求 消息Id: 26805
Net.ProtocolRegistry[Net.ProtocolNo.C2S_WATCHING_VIDEO_NOTIFY_REQ] = function()
    Net.CSWatchingVideoNotifyReq = Net.CSWatchingVideoNotifyReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSWatchingVideoNotifyReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSWatchingVideoNotifyReq
end

-- 消息说明:观看视频通知响应 消息Id: 26806
Net.ProtocolRegistry[Net.ProtocolNo.S2C_WATCHING_VIDEO_NOTIFY_RESP] = function()
    Net.SCWatchingVideoNotifyResp = Net.SCWatchingVideoNotifyResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCWatchingVideoNotifyResp:__init()
        self:AutoConstruct({
        })
    end
    return Net.SCWatchingVideoNotifyResp
end