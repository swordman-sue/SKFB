
--[[
%% @module: 协议池
%% @author: swordman sue
%% @created: 2016-07-26
--]]

Net = Net or {}

Net.ProtocolPool = Net.ProtocolPool or BaseClass()

function Net.ProtocolPool:__init()
	Net.ProtocolPool.Instance = self

    --协议对象表
    self.__protocol_obj_map = nil

    --协议号表
    self.__protocol_no_map = {}
    for _, protocol_no in pairs(Net.ProtocolNo) do
        self.__protocol_no_map[protocol_no] = true
    end
end

function Net.ProtocolPool:__delete()
    if self.__protocol_obj_map then
        for _, protocol in pairs(self.__protocol_obj_map) do
            protocol:DeleteMe()
        end
        self.__protocol_obj_map = nil
    end
end

function Net.ProtocolPool:GetProtocol(protocol_no, for_recv)
    if not protocol_no then
        LogTool.LogError("[Net.ProtocolPool:GetProtocol] protocol_no 为空")
        return
    end

    if not self.__protocol_no_map[protocol_no] then
        if not for_recv then
            LogTool.LogError("[Net.ProtocolPool:GetProtocol] protocol_no 未定义")
        end
        return
    end

    self.__protocol_obj_map = self.__protocol_obj_map or {}
    
    if not self.__protocol_obj_map[protocol_no] then
        --支持采用protobuf协议进行的网络交互
        if MessageAdapter.USE_PROTOBUF_FOR_NET_MSG then
            self.__protocol_obj_map[protocol_no] = Net.ProtobufStruct.New(protocol_no)

        --自定义协议
        else
            --加载协议文件
            local protocol_analyze_file = "network/protocols/protocal_analyze_" .. math.floor(protocol_no / 100)
            RequireToolBatch:LoadFilesSync({file_list = {protocol_analyze_file}})    

            --创建协议对象
            local construct_func = Net.ProtocolRegistry[protocol_no]
            if not construct_func then
                LogTool.LogError("[Net.ProtocolPool:GetProtocol] 找不到对应的协议注册信息(protocol_no:%d)", protocol_no)
                return nil
            end
            local class = construct_func()
            self.__protocol_obj_map[protocol_no] = class.New(protocol_no)
        end
    end

    local protocol_obj = self.__protocol_obj_map[protocol_no]

    --支持采用protobuf协议进行的网络交互
    if MessageAdapter.USE_PROTOBUF_FOR_NET_MSG then
        return for_recv and protocol_obj or protocol_obj.encode_data    
    else
        return protocol_obj
    end
end
