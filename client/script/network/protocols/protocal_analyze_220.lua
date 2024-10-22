Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--世界之心系统数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HEART_OF_WORLD_DATA_REQ] = function()
    Net.CSElvesDataReq = Net.CSElvesDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSElvesDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSElvesDataReq
end

--世界之心系统数据响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HEART_OF_WORLD_DATA_RESP] = function()
    Net.SCElvesDataResp = Net.SCElvesDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCElvesDataResp:__init()
        self:AutoConstruct({
        	"stage__C",	--阶段
			"level__C",	--等级
        })
    end
    return Net.SCElvesDataResp
end

--世界之心系统升级数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_HEART_OF_WORLD_LEVEL_UP_REQ] = function()
    Net.CSElvesLevelUpDataReq = Net.CSElvesLevelUpDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSElvesLevelUpDataReq:__init()
        self:AutoConstruct({
        })
    end
    return Net.CSElvesLevelUpDataReq
end

--世界之心系统升级数据响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_HEART_OF_WORLD_LEVEL_UP_RESP] = function()
    Net.SCElvesLevelUpDataResp = Net.SCElvesLevelUpDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCElvesLevelUpDataResp:__init()
        self:AutoConstruct({
        	"stage__C",	--阶段
			"level__C",	--等级
        })
    end
    return Net.SCElvesLevelUpDataResp
end

--精灵强化请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPIRIT_ENHANCE_REQ] = function()
    Net.CSSpiritEnhanceReq = Net.CSSpiritEnhanceReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpiritEnhanceReq:__init()
        self:AutoConstruct({
            "spirit_id__I", --精灵ID
        })
    end
    return Net.CSSpiritEnhanceReq
end

--精灵强化响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPIRIT_ENHANCE_RESP] = function()
    Net.SCSpiritEnhanceResp = Net.SCSpiritEnhanceResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpiritEnhanceResp:__init()
        self:AutoConstruct({
            "spirit_id__I", --精灵ID
        })
    end
    return Net.SCSpiritEnhanceResp
end

--精灵使用请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPIRIT_USE_REQ] = function()
    Net.CSSpiritUseReq = Net.CSSpiritUseReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpiritUseReq:__init()
        self:AutoConstruct({
            "spirit_id__I", --精灵ID
        })
    end
    return Net.CSSpiritUseReq
end

--精灵使用响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPIRIT_USE_RESP] = function()
    Net.SCSpiritUseResp = Net.SCSpiritUseResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpiritUseResp:__init()
        self:AutoConstruct({
            "spirit_id__I", --精灵ID
        })
    end
    return Net.SCSpiritUseResp
end

--精灵系统数据请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPIRIT_DATA_REQ] = function()
    Net.CSSpiritDataReq = Net.CSSpiritDataReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpiritDataReq:__init()
        self:AutoConstruct({          
        })
    end
    return Net.CSSpiritDataReq
end

--精灵系统数据响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPIRIT_DATA_RESP] = function()
    Net.SCSpiritDataResp = Net.SCSpiritDataResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpiritDataResp:__init()
        self:AutoConstruct({
            "cur_use_spirit__I",    --当前使用的精灵
            "spirit_list__T__id@I##enhance_level@H##train_stage@H##train_times@H##star@H", -- 精灵列表
            "pokedex_list__T__id@H##level@H", -- 图鉴列表
        })
    end
    return Net.SCSpiritDataResp
end

--精灵培养请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPRITE_TRAIN_REQ] = function()
    Net.CSSpiritTrainReq = Net.CSSpiritTrainReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpiritTrainReq:__init()
        self:AutoConstruct({          
            "sprite_id__I", --精灵ID
            "train_times__H",   --培养次数
        })
    end
    return Net.CSSpiritTrainReq
end

--精灵培养响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPRITE_TRAIN_RESP] = function()
    Net.SCSpiritTrainResp = Net.SCSpiritTrainResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpiritTrainResp:__init()
        self:AutoConstruct({
            "sprite_id__I", --精灵ID
            "train_times__H",   --培养次数
        })
    end
    return Net.SCSpiritTrainResp
end

--精灵升星请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPRITE_STAR_UP_REQ] = function()
    Net.CSSpiritStarUpReq = Net.CSSpiritStarUpReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpiritStarUpReq:__init()
        self:AutoConstruct({          
            "sprite_id__I", --精灵ID
        })
    end
    return Net.CSSpiritStarUpReq
end

--精灵升星响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPRITE_STAR_UP_RESP] = function()
    Net.SCSpiritStarUpResp = Net.SCSpiritStarUpResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpiritStarUpResp:__init()
        self:AutoConstruct({
            "sprite_id__I", --精灵ID
        })
    end
    return Net.SCSpiritStarUpResp
end

--精灵祝福请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPRITE_BLESS_REQ] = function()
    Net.CSSpiritBlessReq = Net.CSSpiritBlessReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpiritBlessReq:__init()
        self:AutoConstruct({          
            "bless_type__H",    --祝福类型
            "formation_pos__H", --阵营位置
            "sprite_id__I",     --精灵ID
        })
    end
    return Net.CSSpiritBlessReq
end

--精灵祝福响应
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPRITE_BLESS_RESP] = function()
    Net.SCSpiritBlessResp = Net.SCSpiritBlessResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpiritBlessResp:__init()
        self:AutoConstruct({
            "bless_type__H",    --祝福类型
            "formation_pos__H", --阵营位置
            "sprite_id__I",     --精灵ID
        })
    end
    return Net.SCSpiritBlessResp
end

--消息说明: 精灵合成请求 消息Id: 22017
Net.ProtocolRegistry[Net.ProtocolNo.C2S_SPRITE_COMPOSE_REQ] = function()
    Net.CSSpriteComposeReq = Net.CSSpriteComposeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSSpriteComposeReq:__init()
        self:AutoConstruct({
            "fragment_id__I",       --碎片ID
        })
    end
    return Net.CSSpriteComposeReq
end

--消息说明: 精灵合成响应 消息Id: 22018
Net.ProtocolRegistry[Net.ProtocolNo.S2C_SPRITE_COMPOSE_RESP] = function()
    Net.SCSpriteComposeResp = Net.SCSpriteComposeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCSpriteComposeResp:__init()
        self:AutoConstruct({
            "fragment_id__I",       --碎片ID
        })
    end
    return Net.SCSpriteComposeResp
end