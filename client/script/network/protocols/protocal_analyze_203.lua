
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 邮件列表请求
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EMAIL_LIST_REQ] = function()
    Net.CSEmailListReq = Net.CSEmailListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEmailListReq:__init()
        self:AutoConstruct({
            
        })
    end
    return Net.CSEmailListReq
end

--邮件列表响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EMAIL_LIST_RESP] = function()
    Net.CSEmailListResp = Net.CSEmailListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEmailListResp:__init()
        self:AutoConstruct({
            "email_list__T__info_id@H##receive_time@I##context_list@D|MailContextList|##title@s##text@s", -- 邮件数据列表
        })
    end
    return Net.CSEmailListResp
end

-- --购买记录请求: 
-- Net.ProtocolRegistry[Net.ProtocolNo.MSG_C2S_SHOP_GOODS_BUY_RECORD_REQ] = function()
--     Net.CSShopGoodsBuyRecordReq = Net.CSShopGoodsBuyRecordReq or BaseClass(Net.BaseProtocolStruct)
--     function Net.CSShopGoodsBuyRecordReq:__init()
--         self:AutoConstruct({
--             "type__C",  --商品类型
--         })
--     end
--     return Net.CSShopGoodsBuyRecordReq
-- end

--新邮件
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EMAIL_NEW_NOTIFY] = function()
    Net.CSEmailNewNotify = Net.CSEmailNewNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEmailNewNotify:__init()
        self:AutoConstruct({
            "info_id__H",       --邮件配置id
            "receive_time__I",  --接收时间
            "context_list__T__type@C##text@s", -- 上下文数据列表
        })
    end
    return Net.CSEmailNewNotify
end

