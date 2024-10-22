
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--分解回收请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECYCLE_DECOMPOSE_REQ] = function()
    Net.CSRecycleDecomposerReq = Net.CSRecycleDecomposerReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRecycleDecomposerReq:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "target_list__T__uid@I", -- 请求分解的目标uid列表
        })
    end
    return Net.CSRecycleDecomposerReq
end

--分解回收响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECYCLE_DECOMPOSE_RESP] = function()
    Net.SCRecycleDecomposeResp = Net.SCRecycleDecomposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRecycleDecomposeResp:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "target_list__T__uid@I", -- 请求分解的目标uid列表
        })
    end
    return Net.SCRecycleDecomposeResp
end

--重生回收请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECYCLE_RESET_REQ] = function()
    Net.CSRecycleResetReq = Net.CSRecycleResetReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRecycleResetReq:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "target_uid__I", -- 请求分解的目标uid
        })
    end
    return Net.CSRecycleResetReq
end

--重生回收请求: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECYCLE_RESET_RESP] = function()
    Net.SCRecycleResetResp = Net.SCRecycleResetResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRecycleResetResp:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "target_uid__I", -- 请求分解的目标uid
        })
    end
    return Net.SCRecycleResetResp
end

--分解返还请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECYCLE_DECOMPOSE_RETURN_REQ] = function()
    Net.CSRecycleDecomposeReturnReq = Net.CSRecycleDecomposeReturnReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRecycleDecomposeReturnReq:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "target_list__T__uid@I", -- 请求分解的目标uid列表
        })
    end
    return Net.CSRecycleDecomposeReturnReq
end

--分解返还响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECYCLE_DECOMPOSE_RETURN_RESP] = function()
    Net.SCRecycleDecomposeReturnResp = Net.SCRecycleDecomposeReturnResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRecycleDecomposeReturnResp:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "return_object_list__T__obj_id@I##obj_num@I", -- 返还对象列表
            "return_currency_list__T__type@C##value@I", -- 返还货币列表
        })
    end
    return Net.SCRecycleDecomposeReturnResp
end

--重生回收请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_RECYCLE_RESET_RETURN_REQ] = function()
    Net.CSRecycleResetReturnReq = Net.CSRecycleResetReturnReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSRecycleResetReturnReq:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "target_uid__I", -- 请求分解的目标uid
        })
    end
    return Net.CSRecycleResetReturnReq
end

--重生回收响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_RECYCLE_RESET_RETURN_RESP] = function()
    Net.SCRecycleResetReturnResp = Net.SCRecycleResetReturnResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCRecycleResetReturnResp:__init()
        self:AutoConstruct({
            "obj_type__C",  --回收对象类型
            "currency_type__C", --货币类型
            "currency_value__I",    --货币值
            "return_object_list__T__obj_id@I##obj_num@I", -- 返还对象列表
            "return_currency_list__T__type@C##value@I", -- 返还货币列表
        })
    end
    return Net.SCRecycleResetReturnResp
end
