Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 消息说明:宝石列表请求 消息Id: 25301
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GEM_LIST_REQ] = function()
    Net.CSGemListReq = Net.CSGemListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGemListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSGemListReq
end


-- 消息说明:宝石列表响应 消息Id: 25302
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_LIST_RESP] = function()
    Net.SCGemListResp = Net.SCGemListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemListResp:__init()
        self:AutoConstruct({
            "gem_list__T__id@I##num@I", -- 宝石列表
            "gem_hole_open_list__T__hole@H", -- 孔位开启列表  列表5个 HOLE 1-8
        })
    end
    return Net.SCGemListResp
end

-- 消息说明:宝石合成请求 消息Id: 25303
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GEM_COMPOSE_REQ] = function()
    Net.CSGemComposeReq = Net.CSGemComposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGemComposeReq:__init()
        self:AutoConstruct({
            "gem_num__H",   --要合成的宝石数量
            "gem_id__I",    --宝石Id
        })
    end
    return Net.CSGemComposeReq
end

-- 消息说明:宝石合成响应 消息Id: 25304
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_COMPOSE_RESP] = function()
    Net.SCGemComposeResp = Net.SCGemComposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemComposeResp:__init()
        self:AutoConstruct({
            "gem_num__H",   --要合成的宝石数量
            "gem_id__I",    --宝石Id
        })
    end
    return Net.SCGemComposeResp
end

-- 消息说明:宝石镶嵌请求 消息Id: 25305
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GEM_INLAY_REQ] = function()
    Net.CSGemInlayReq = Net.CSGemInlayReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGemInlayReq:__init()
        self:AutoConstruct({
            "formation_slot__H",    --阵型槽位
            "gem_inlay_list__T__id@I##hole@H", -- 宝石镶嵌列表
        })
    end
    return Net.CSGemInlayReq
end

-- 消息说明:宝石镶嵌响应 消息Id: 25306
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_INLAY_RESP] = function()
    Net.SCGemInlayResp = Net.SCGemInlayResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemInlayResp:__init()
        self:AutoConstruct({
            "formation_slot__H",    --阵型槽位
            "gem_inlay_list__T__id@I##hole@H", -- 宝石镶嵌列表
        })
    end
    return Net.SCGemInlayResp
end

-- 消息说明:宝石卸下请求 消息Id: 25307
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GEM_UNLOAD_REQ] = function()
    Net.CSGemUnloadReq = Net.CSGemUnloadReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGemUnloadReq:__init()
        self:AutoConstruct({
            "formation_slot__H",    --阵型槽位
            "gem_hole__H",  --宝石孔位
        })
    end
    return Net.CSGemUnloadReq
end

-- 消息说明:宝石卸下响应 消息Id: 25308
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_UNLOAD_RESP] = function()
    Net.SCGemUnloadResp = Net.SCGemUnloadResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemUnloadResp:__init()
        self:AutoConstruct({
            "formation_slot__H",    --阵型槽位
            "gem_hole__H",  --宝石孔位
        })
    end
    return Net.SCGemUnloadResp
end

-- 消息说明:宝石孔位开启请求 消息Id: 25309
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GEM_HOLE_OPEN_REQ] = function()
    Net.CSGemHoleOpenReq = Net.CSGemHoleOpenReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGemHoleOpenReq:__init()
        self:AutoConstruct({
            "formation_slot__H",    --阵型槽位
            "gem_hole__H",  --宝石孔位
        })
    end
    return Net.CSGemHoleOpenReq
end

-- 消息说明:宝石孔位开启响应 消息Id: 25310
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_HOLE_OPEN_RESP] = function()
    Net.SCGemHoleOpenResp = Net.SCGemHoleOpenResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemHoleOpenResp:__init()
        self:AutoConstruct({
            "formation_slot__H",    --阵型槽位
            "gem_hole__H",  --宝石孔位
        })
    end
    return Net.SCGemHoleOpenResp
end

-- 消息说明:宝石数据通知 消息Id: 25311
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_DATA_NOTIFY] = function()
    Net.SCGemDataNotify = Net.SCGemDataNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemDataNotify:__init()
        self:AutoConstruct({
            "gem_id__I",    --宝石ID
            "gem_num__I",   --宝石数量
            "flag__C",  --操作标志(0=减少,1=添加或者更新)
        })
    end
    return Net.SCGemDataNotify
end

-- 消息说明:宝石分解请求 消息Id: 25312
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GEM_DECOMPOSE_REQ] = function()
    Net.CSGemDecomposeReq = Net.CSGemDecomposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSGemDecomposeReq:__init()
        self:AutoConstruct({
            "gem_id__I",    --宝石ID
            "gem_num__I",   --宝石数量
        })
    end
    return Net.CSGemDecomposeReq
end

-- 消息说明:宝石分解响应 消息Id: 25313
Net.ProtocolRegistry[Net.ProtocolNo.S2C_GEM_DECOMPOSE_RESP] = function()
    Net.SCGemDecomposeResp = Net.SCGemDecomposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCGemDecomposeResp:__init()
        self:AutoConstruct({
            "gem_id__I",    --宝石ID
            "gem_num__I",   --宝石数量
        })
    end
    return Net.SCGemDecomposeResp
end
