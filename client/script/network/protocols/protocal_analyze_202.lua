
Net = Net or {}

Net.ProtocolRegistry = Net.ProtocolRegistry or {}

--帐号登陆: 
Net.ProtocolRegistry[Net.ProtocolNo.C2S_GM_REQ] = function()
    Net.CMGmReq = Net.CMGmReq or BaseClass(Net.BaseProtocolStruct)
    function Net.CMGmReq:__init()
        self:AutoConstruct({
            "id__H",            --命令Id
            "param1__I",        --参数1
            "param2__I",        --参数2
            "param3__I",        --参数2
        })
    end
    return Net.CMGmReq
end