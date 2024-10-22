--消息说明: 关卡评论列表请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MAP_COMMENT_LIST_REQ] = function()
    Net.CSMapCommentListReq = Net.CSMapCommentListReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMapCommentListReq:__init()
        self:AutoConstruct({
            "map_id__H",		--关卡ID
        })
    end
    return Net.CSMapCommentListReq
end

--消息说明: 关卡评论列表响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MAP_COMMENT_LIST_RESP] = function()
    Net.SCMapCommentListResp = Net.SCMapCommentListResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMapCommentListResp:__init()
        self:AutoConstruct({
            "map_id__H",		--关卡ID
			"comment_list__T__comment_id@H##role_name@s##text@s##vip_level@C##time@I##like_times@H##like_player_list@D|LikePlayerList|", -- 评论列表

			-- comment_id@H : 评论ID
			-- role_name@s : 评论者角色名称
			-- text@s : 评论内容
			-- vip_level@C : VIP等级
			-- time@I : 评论时间戳
			-- like_times@H : 点赞次数

			-- 点赞玩家列表说明:
			-- LikePlayerList: 
			-- role_id@I : 点赞玩家ID
        })
    end
    return Net.SCMapCommentListResp
end

--消息说明: 关卡评论请求: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MAP_COMMENT_REQ] = function()
    Net.CSMapCommentReq = Net.CSMapCommentReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMapCommentReq:__init()
        self:AutoConstruct({
            "map_id__H",		--关卡ID
			"text__s",		--评论内容
        })
    end
    return Net.CSMapCommentReq
end

--消息说明: 关卡评论响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MAP_COMMENT_RESP] = function()
    Net.SCMapCommentResp = Net.SCMapCommentResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMapCommentResp:__init()
        self:AutoConstruct({
            "map_id__H",		--关卡ID
			"comment_id__H",	--评论ID
			"time__I",		--评论时间
			"text__s",		--评论内容
        })
    end
    return Net.SCMapCommentResp
end

--消息说明: 关卡评论点赞: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_MAP_COMMENT_LIKE_REQ] = function()
    Net.CSMapCommentLikeReq = Net.CSMapCommentLikeReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CSMapCommentLikeReq:__init()
        self:AutoConstruct({
            "map_id__H",		--关卡ID
			"comment_id__H",	--评论ID
        })
    end
    return Net.CSMapCommentLikeReq
end

--消息说明: 关卡评论点赞响应: 
Net.ProtocolRegistry[Net.ProtocolNo.S2C_MAP_COMMENT_LIKE_RESP] = function()
    Net.SCMapCommentLikeResp = Net.SCMapCommentLikeResp or BaseClass(Net.BaseProtocolStruct)
    function Net.SCMapCommentLikeResp:__init()
        self:AutoConstruct({
            "map_id__H",		--关卡ID
			"comment_id__H",	--评论ID
        })
    end
    return Net.SCMapCommentLikeResp
end