
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

-- 装备列表请求
Net.ProtocolRegistry[Net.ProtocolNo.IPMENT_LIST_REQ] = function()
    Net.ItemListReq = Net.ItemListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.ItemListReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.ItemListReq
end

--装备列表响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_LIST_RESP] = function()
    Net.SCEquipmentListResp = Net.SCEquipmentListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipmentListResp:__init()
        self:AutoConstruct({
        "equipment_list__T__uid@I##info_id@H##strengthen_level@H##refine_level@C##refine_exp@I##equip_heroid@H##enhance_cost_gold@I##star@C##star_level@C",
        })
    end
    return Net.SCEquipmentListResp
end

--装备详情请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_INFO_REQ] = function()
    Net.CSEquipmentInfoReq = Net.CSEquipmentInfoReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipmentInfoReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSEquipmentInfoReq
end

--装备详情响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_INFO_RESP] = function()
    Net.SCEquipmentInfoResp = Net.SCEquipmentInfoResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipmentInfoResp:__init()
        self:AutoConstruct({
            "equipment_uid__I", --UID
            "info_id__I",       --配置ID
            "streng_level__C",  --强化等级
            "refine_level__C",  --精炼等级
            "star__C",      --星数
            "star_level__C",    --星级
        })
    end
    return Net.SCEquipmentInfoResp
end

--装备合成请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_COMPOSE_REQ] = function()
    Net.CSEquipmentComposeReq = Net.CSEquipmentComposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipmentComposeReq:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
        })
    end
    return Net.CSEquipmentComposeReq
end

--装备合成响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_COMPOSE_RESP] = function()
    Net.SCEquipmentComposeResp = Net.SCEquipmentComposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipmentComposeResp:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
        })
    end
    return Net.SCEquipmentComposeResp
end

-- --请求装备碎片列表: 
-- Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_FRAGMENT_LIST_REQ] = function()
--     Net.CSEquipmentFragmentListReq = Net.CSEquipmentFragmentListReq or BaseClass(Net.BaseProtocolStruct)
--     function Net.CSEquipmentFragmentListReq:__init()
--         self:AutoConstruct({
--             "equipment_uid__I", --装备uid
--         })
--     end
--     return Net.CSEquipmentFragmentListReq
-- end

-- --请求装备碎片列表响应: 
-- Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_FRAGMENT_LIST_RESP] = function()
--     Net.SCEquipmentFragmentListResp = Net.SCEquipmentFragmentListResp or BaseClass(Net.BaseProtocolStruct)
--     function Net.SCEquipmentFragmentListResp:__init()
--         self:AutoConstruct({
--             "fragment_list__T__id@I##num@S", -- 碎片列表
--         })
--     end
--     return Net.SCEquipmentFragmentListResp
-- end

--请求装备强化: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_STRENG_REQ] = function()
    Net.CSEquipMentstrengReq = Net.CSEquipMentstrengReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipMentstrengReq:__init()
        self:AutoConstruct({
            "type__C",  --强化的类型
            "equipment_uid__I", --装备uid
        })
    end
    return Net.CSEquipMentstrengReq
end

--请求装备强化响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_STRENG_RESP] = function()
    Net.SCEquipMentstrengResp = Net.SCEquipMentstrengResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipMentstrengResp:__init()
        self:AutoConstruct({
            "type__C",  --强化的类型
            "crit_times__C",    --暴击次数
            "strengthen_level__H",  --强化等级
            "equipment_uid__I", --装备uid
        })
    end
    return Net.SCEquipMentstrengResp
end

--装备基本信息通知: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_INFO_NOTIFY] = function()
    Net.SCEquipMentInfoNotify = Net.SCEquipMentInfoNotify or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipMentInfoNotify:__init()
        self:AutoConstruct({
            "flag__C",  --标志(更新标志:1=添加,更新,0=删除)
            "uid__I",   --装备uid
            "info_id__H",   --装备信息ID
            "strengthen_level__H",  --强化等级
            "refine_level__C",  --精练等级
            "refine_exp__I",    --精练经验
            "equip_heroid__H",  --装备的英雄信息ID
            "enhance_cost_gold__I", --强化消耗的金币
            "star__C",      --星数
            "star_level__C",    --星级
        })
    end
    return Net.SCEquipMentInfoNotify
end

--装备精炼请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_REFINE_REQ] = function()
    Net.CSEquipMentRefintReq = Net.CSEquipMentRefintReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipMentRefintReq:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
            "cost_item_list__T__item_id@I##item_num@H",
        })
    end
    return Net.CSEquipMentRefintReq
end

--装备精炼响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_REFINE_RESP] = function()
    Net.SCEquipMentRefintResp = Net.SCEquipMentRefintResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipMentRefintResp:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
            "old_refine_level__C",  --旧的精练等级
            "refine_level__C",  --当前精练等级
            "refine_exp__I",    --当前精练经验
            "add_refine_exp__I",    --本次添加的精练经验
        })
    end
    return Net.SCEquipMentRefintResp
end

--装备穿戴请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_PUT_ON_REQ] = function()
    Net.CSEquipMentPutOnReq = Net.CSEquipMentPutOnReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipMentPutOnReq:__init()
        self:AutoConstruct({
            "formation_pos__C", --阵型的位置(从0开始)
            "equipment_pos__C", --装备的位置(从0开始)
            "equipment_uid__I", --装备uid
        })
    end
    return Net.CSEquipMentPutOnReq
end

--装备穿戴响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_PUT_ON_RESP] = function()
    Net.SCEquipMentPutOnResp = Net.SCEquipMentPutOnResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipMentPutOnResp:__init()
        self:AutoConstruct({
            "formation_pos__C", --阵型的位置(从0开始)
            "equipment_pos__C", --装备的位置(从0开始)
            "equipment_uid__I", --装备uid
        })
    end
    return Net.SCEquipMentPutOnResp
end

--装备一键强化请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_ONE_KEY_ENHANCE_REQ] = function()
    Net.CSEquipmentOneKeyEnhanceReq = Net.CSEquipmentOneKeyEnhanceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipmentOneKeyEnhanceReq:__init()
        self:AutoConstruct({
            "slot__C",  --阵型槽位
        })
    end
    return Net.CSEquipmentOneKeyEnhanceReq
end

--装备一键强化响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_ONE_KEY_ENHANCE_RESP] = function()
    Net.SCEquipmentOneKeyEnhanceResp = Net.SCEquipmentOneKeyEnhanceResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipmentOneKeyEnhanceResp:__init()
        self:AutoConstruct({
            "slot__C",  --阵型槽位
            "enhance_level_list__T__old_level@H##new_level@H",
        })
    end
    return Net.SCEquipmentOneKeyEnhanceResp
end

--消息说明: 装备升星请求 消息Id: 20718
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_STAR_UP_REQ] = function()
    Net.CSEquipmentStarUpReq = Net.CSEquipmentStarUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipmentStarUpReq:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
        })
    end
    return Net.CSEquipmentStarUpReq
end

--消息说明: 装备升星请响应 消息Id: 20719
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_STAR_UP_RESP] = function()
    Net.SCEquipmentStarUpResp = Net.SCEquipmentStarUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipmentStarUpResp:__init()
        self:AutoConstruct({
            "star__C",      --当前星
            "star_level__C",    --当前星级
            "equipment_uid__I", --装备uid
        })
    end
    return Net.SCEquipmentStarUpResp
end

--消息说明: 装备升品请求 消息Id: 20720
Net.ProtocolRegistry[Net.ProtocolNo.C2S_EQUIPMENT_QUALITY_UP_REQ] = function()
    Net.CSEquipmentQualityUpReq = Net.CSEquipmentQualityUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSEquipmentQualityUpReq:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
        })
    end
    return Net.CSEquipmentQualityUpReq
end

--消息说明: 装备升品响应 消息Id: 20721
Net.ProtocolRegistry[Net.ProtocolNo.S2C_EQUIPMENT_QUALITY_UP_RESP] = function()
    Net.SCEquipmentQualityUpResp = Net.SCEquipmentQualityUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCEquipmentQualityUpResp:__init()
        self:AutoConstruct({
            "equipment_uid__I", --装备uid
        })
    end
    return Net.SCEquipmentQualityUpResp
end
