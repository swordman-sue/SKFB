--
-- @brief 聊天信息
-- @author: yjg
-- @date: 2016年8月3日16:13:06
--

Modules = Modules or {}

Modules.ChatModule = Modules.ChatModule or BaseClass(BaseModule)

function Modules.ChatModule:__init()
    --保存所有的聊天信息
    self.allData = {}
    self.allData.whisper = {}  --私聊频道
    self.allData.world = {}  --世界频道
    self.allData.group = {}  --工会频道
    self.allData.team = {}  --队伍频道
    self.allData.mainChat = {}  --队伍频道

    self.record = {}

    self.whisper = nil
    self.opWhisper = false

    self.index = false

    --聊天过滤设置开关
    self.shield = {
        [1] = true,
        [2] = true,
        [3] = true,
        [4] = true,
    }
    --发送cd
    self.time = {}
    self.time.over = 0
    --类型
    self.KEY = {
        ["siliao"] = 1,
        ["shijie"] = 2,
        ["gonghui"] = 3,
        ["duiwu"] = 4,
    }
    --表情表(?)
    self:FaceInfo()
end

function Modules.ChatModule:__delete()
end

--表情定义
function Modules.ChatModule:FaceInfo()
    --定义了20个格式为 x_00 的表情
    self.FaceList = {}
    for i=1,20 do
        table.insert(self.FaceList,string.format("x_%02d", i))
    end
end

--服务器推送
function Modules.ChatModule:LiaoTianBroadcast(info)
    info.item = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    self:UpdataChat(info)
end

--发送信息的成功返回
function Modules.ChatModule:LiaoTianReturn(info)
    CPPGameLib.PrintTable(info,"发送成功")
    self:RecordCD()
end

--发送消息
function Modules.ChatModule:SendLiaoTianDate(info)
    CPPGameLib.PrintTable("xy" , info,"发送消息")
    -- 1=私聊频道 2=世界频道 3=工会频道 4=队伍频道
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.MSG_C2S_CHAT_REQ)
    --发送频道的类型
    data.chat_type = info.type or 2
    --是不是语音
    data.is_voice = info.is or 0
    --语音内容
    data.voice = info.voice or 0
    --发送对象的ID
    data.target_id = info.id or 0
    --发送对象的名字
    data.target_name = info.name or 0
    --发送的内容
    data.chat_text = LanguageFilter.ChatFilter( tostring(info.text) ) or  tostring(info.text)
    --发送
    GlobalNetAdapter:OnSend(data)
end

--消息说明: 聊天记录列表请求 消息Id: 20407
function Modules.ChatModule:ChatRecordListReq( chat_type )
    CPPGameLib.PrintTable("xy" , info , "消息说明: 聊天记录列表请求 消息Id: 20407" )

    local t1 = {
            [1] = Macros.Game.CHAT.WORLD , 
            [2] = Macros.Game.CHAT.UNION , 
    }

    for i,v in ipairs(t1) do
        local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_CHAT_RECORD_LIST_REQ)
        data.chat_type = v
        GlobalNetAdapter:OnSend(data)
    end
end

--消息说明: 聊天记录列表响应 消息Id: 20408
function Modules.ChatModule:ChatRecordListResp( info )
    for i,v in ipairs( info.record_list ) do
        v.chat_type = info.chat_type
        table.insert( self.record , v )
    end
    table.sort( self.record , function ( a , b )
        return a.time < b.time
    end )

    for i,v in ipairs( self.record ) do
        local t1 = {}
        t1.chat_type = v.chat_type
        t1.role_id = v.role_id
        t1.head_icon = v.head_icon
        t1.vip_level = v.vip_level
        t1.role_name = v.role_name
        t1.voice_id = v.voice_id or 0
        t1.chat_text = v.chat_text
        t1.cur_use_title_id = v.cur_use_title_id
        t1.server_id = v.server_id
        t1.gm_level = v.gm_level
        self:UpdataChat( t1 )
    end
end


--整理数据
function Modules.ChatModule:UpdataChat( info )
    --判断是不是自己
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    info.who = "he"
    if info.role_name == userDate.role_name then
        info.who = "me"
    end

    if info.chat_type == Macros.Game.CHAT.PRIVATE then
        info.whisper = info.role_name
        if info.role_name == userDate.role_name then
            info.role_name = string.format( CPPGameLib.GetString("wodui") , self.whisper.role_name)
        else
            info.role_name = string.format( CPPGameLib.GetString("nidui") , info.role_name)
            self.opWhisper = true
        end
        table.insert(self.allData.whisper,info)
    elseif info.chat_type == Macros.Game.CHAT.WORLD then
        table.insert(self.allData.world,info)
    elseif info.chat_type == Macros.Game.CHAT.UNION then
        table.insert(self.allData.group,info)
    elseif info.chat_type == Macros.Game.CHAT.TEAM then
        table.insert(self.allData.team,info)
    elseif info.chat_type == Macros.Game.CHAT.BOSS then
        table.insert(self.allData.team,info)
    end

    GlobalEventSystem:FireNextFrame(Macros.Event.ChatModule.CHAT_DATA , info)
    self:ShieldChannel(self.shield,info)

    --设置主城用的聊天信息
    self:SetMainChat(info)
end

--得到世界列表
function Modules.ChatModule:GetWorldLst( ... )
    return self.allData.world
end

--得到对象列表
function Modules.ChatModule:GetWhisperLst( ... )
    return self.allData.whisper
end

--得到公会列表
function Modules.ChatModule:GetGroupLst( ... )
    return self.allData.group
end

--得到队伍列表
function Modules.ChatModule:GetTeamLst( ... )
    return self.allData.team
end

--设置聊天的对象
function Modules.ChatModule:SetWhisper( info )
    self.whisper = info
end

--获得私聊对象
function Modules.ChatModule:GetWhisper( ... )
    return self.whisper
end

--获得私聊有没有点过
function Modules.ChatModule:GetOpWhisper( ... )
    return self.opWhisper
end

--设置私聊有没有点过
function Modules.ChatModule:SetOpWhisper( type )
    self.opWhisper = type or false
end

--寻找信息中是否包含了表情
function Modules.ChatModule:SeekSmile(info)
    if not info.has_seek_smile then
        for i,v in ipairs(self.FaceList) do
            --找到图集
            local dice_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHAT_EMOTION_ICON, v)
            --获得图集里的参数转化成字符串
            local dice_str = string.format("<FACEIMG IMG=%s TEXRECT={%d,%d,%d,%d} SCALE=%f>", dice_data.image_path, dice_data.x, dice_data.y, dice_data.width, dice_data.height, 2)
            --替换所有的表情
            if info.chat_text then
                local chat_text = string.gsub(info.chat_text, v , dice_str)
                info.chat_text = chat_text
            end
        end

        info.has_seek_smile = true
    end
    return info
end

--获取聊天信息(带表情解析)
function Modules.ChatModule:GetChatTextWithSmile(info)
    self:SeekSmile(info)
    return info.chat_text
end

--发送CD
function Modules.ChatModule:RecordCD( ... )
    self.time.startTime = Macros.Global.NowTime 
    self.time.over = Macros.Global.NowTime + ConfigAdapter.Common.GetChatCD()

    GlobalEventSystem:FireNextFrame(Macros.Event.ChatModule.CHAT_CD)
end

--获取
function Modules.ChatModule:GetTime()
    local btn = 0
    if #self.time<0 or self.time.over - Macros.Global.NowTime <= 0 then
        btn = 0
    else
        btn = math.abs(self.time.over - Macros.Global.NowTime)
    end
    print("xy" ,"================" , self.time.over , Macros.Global.NowTime )
    return btn 
end

--屏蔽频道
function Modules.ChatModule:ShieldChannel(shid , info)
    if shid then 
        self.shield = shid 
    end
    if not info then return end
    if self.shield[info.chat_type] == true then
    end
end

--设置主城用的信息
function Modules.ChatModule:SetMainChat(info)
    table.insert(self.allData.mainChat, info)

    if info.chat_type == Macros.Game.CHAT.PRIVATE then
        self.__is_private_chat_noticed = true
    end 

    GlobalEventSystem:FireNextFrame(Macros.Event.ChatModule.CHAT_MAIN)
end

--获得主城用的信息
function Modules.ChatModule:GetMainChat()
    return self.allData.mainChat
end

--是否进行私聊提示
function Modules.ChatModule:IsPrivateChatNotice()
    return self.__is_private_chat_noticed
end

--设置私聊提示标记
function Modules.ChatModule:SetPrivateChatNotice(var)
    self.__is_private_chat_noticed = var
end

--保存上次聊天消息
function Modules.ChatModule:SetBeforeNews( text )
    self.beforeNews = text
end

--保存上次聊天消息
function Modules.ChatModule:GetBeforeNews( )
    return self.beforeNews
end