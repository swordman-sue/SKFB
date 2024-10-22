
--[[
%% @module: 声音管理器
%% @author: swordman sue
%% @created: 2017-2-27
%% @description: 播放背景音乐、音效
--]]

SoundMgr = SoundMgr or BaseClass()

function SoundMgr.CreateInstance()
    if not GlobalSoundMgr then
        GlobalSoundMgr = SoundMgr.New()
    end
end

function SoundMgr.DelInstance()
    if GlobalSoundMgr then
        GlobalSoundMgr:DeleteMe()
        GlobalSoundMgr = nil
    end
end

function SoundMgr:__init()
    --已播放的音效表
    self.__played_effect_map = {}

    --已播放的音效表(当前帧)
    self.__played_effect_map_nowframe = {}

    --音效所属的绑定ID
    self.__effect_bind_id = 0

    --音效绑定表(key：绑定ID，value：音效ID列表)
    self.__effect_bind_map = {}

    GlobalRunner:AddRunObj(self)
end

function SoundMgr:__delete()    
    self:Dump(true)

    GlobalRunner:RemoveRunObj(self)
end

function SoundMgr:Update(now_time, elapse_time)
    self.__played_effect_map_nowframe = {}

    if self.__is_pause then
        return
    end

    if self.__delay_create_list then
        local del_list, effect_id
        for i, create_info in ipairs(self.__delay_create_list) do
            create_info.elapse_time = create_info.elapse_time + elapse_time

            if create_info.elapse_time >= create_info.delay_time then
                if not create_info.bind_id then
                    --非绑定类音效
                    self:PlayEffect(create_info.res_name, create_info.loop)

                elseif self.__effect_bind_map[create_info.bind_id] then
                    --绑定类音效(循环音效)
                    effect_id = self:PlayEffect(create_info.res_name, create_info.loop)
                    if effect_id then
                        table.insert(self.__effect_bind_map[create_info.bind_id], effect_id)
                    end
                end

                del_list = del_list or {}
                table.insert(del_list, i)
            end
        end

        if del_list then
            table.sort(del_list, SortTools.ItemUpperSorter())
            for _, del_index in ipairs(del_list) do
                table.remove(self.__delay_create_list, del_index)
            end
        end
    end
end

--预加载BGM
function SoundMgr:PreloadBGM(file_name)
    if not file_name then
        return
    end    
    file_name = self:__GetFilePath(file_name, true)

    CPPSoundManager:PreloadBGMusic(file_name)
end

--播放BGM
function SoundMgr:PlayBGM(file_name, loop, volume, check_same)
    if not file_name then
        return
    end

    if not self:GetBGMEnabled() then
        self.__bgm_cache_info = self.__bgm_cache_info or {}
        self.__bgm_cache_info.file_name = file_name
        self.__bgm_cache_info.loop = loop
        self.__bgm_cache_info.volume = volume
        self.__bgm_cache_info.check_same = check_same
        return
    end

    file_name = self:__GetFilePath(file_name, true)
    loop = loop ~= false and true or false
    volume = volume or 1

    if check_same and file_name == self.__bgm_file_name then
        self:SetBGMVolume(volume)
        return
    end

    self:StopBGM(true)
    self.__bgm_file_name = file_name
    CPPSoundManager:PlayBGMusic(self.__bgm_file_name, loop)
    self:SetBGMVolume(volume)
end

--停止BGM
function SoundMgr:StopBGM(cleanup, file_name)
    cleanup = cleanup or false
    if file_name then
        file_name = self:__GetFilePath(file_name, true)
        if file_name ~= self.__bgm_file_name then
            return
        end
    end
    self.__bgm_file_name = nil

    CPPSoundManager:StopBGMusic(cleanup)
end

--设置BGM音量
function SoundMgr:SetBGMVolume(var)
    CPPSoundManager:SetBGMusicVolume(var)
end

--获取BGM音量
function SoundMgr:GetBGMVolume()
    return CPPSoundManager:GetBGMusicVolume()
end

--设置BGM开关
function SoundMgr:SetBGMEnabled(var)
    if var == nil then
        var = self:GetBGMEnabled()
    end
    Cookies.Set(nil, Macros.Setting.SoundBGMEnabled, var and Macros.Global.TRUE or Macros.Global.FALSE)

    if var then
        CPPSoundManager:ResumeBGMusic()
        if self.__bgm_cache_info then
            self:PlayBGM(self.__bgm_cache_info.file_name, self.__bgm_cache_info.loop, self.__bgm_cache_info.volume, self.__bgm_cache_info.check_same)
            self.__bgm_cache_info = nil
        end
    else
        CPPSoundManager:PauseBGMusic()
    end
end

--获取BGM开关
function SoundMgr:GetBGMEnabled()
    local bgm_enabled = Cookies.Get(nil, Macros.Setting.SoundBGMEnabled)
    bgm_enabled = (bgm_enabled and bgm_enabled ~= "") and tonumber(bgm_enabled) or Macros.Global.TRUE 
    return bgm_enabled == Macros.Global.TRUE
end

--获得当前Bgm播放状态
function SoundMgr:IsBGMusicPlaying()
    return CPPSoundManager:IsBGMusicPlaying()
end

--预加载音效
function SoundMgr:PreloadEffect(file_name)
    if not file_name then
        return
    end
    file_name = self:__GetFilePath(file_name)

    if self.__played_effect_map[file_name] then
        return
    end
    self.__played_effect_map[file_name] = true

    CPPSoundManager:PreloadEffect(file_name)
end

--播放音效
function SoundMgr:PlayEffect(file_name, loop, pitch, pan, gain)
    if not self:GetEffectEnabled() then
        return
    end

    loop = loop or false
    pitch = pitch or 1
    pan = pan or 0
    gain = gain or 1

    if not file_name then
        return
    end
    file_name = self:__GetFilePath(file_name)

    if self.__played_effect_map_nowframe[file_name] then
        return
    end

    self.__played_effect_map[file_name] = true
    self.__played_effect_map_nowframe[file_name] = true
    return CPPSoundManager:PlayEffect(file_name, loop, pitch, pan, gain)
end

--播放音效
function SoundMgr:PlayEffectByConfigID(config_id)
    if not self:GetEffectEnabled() then
        return
    end

    if not config_id then
        return
    end    

    local config_sound = CPPGameLib.GetConfig("Sound", config_id)
    if not config_sound then
        return
    end

    local bind_id
    local function gen_bind_id()
        if not bind_id then
            self.__effect_bind_id = self.__effect_bind_id + 1
            bind_id = tostring(self.__effect_bind_id)
        end
        --与CPPSoundManager:PlayEffect返回的ID区分
        return bind_id
    end

    local delay_time_list = config_sound.delay_time_list
    local loop_info
    local loop_list = config_sound.loop_list
    for i, res_name in ipairs(config_sound.res_name_list) do
        local info = {}
        info.elapse_time = 0
        info.res_name = res_name
        info.delay_time = delay_time_list and delay_time_list[i] or 0
        
        --生成绑定ID(循环音效)
        loop_info = loop_list and loop_list[i] or nil
        if loop_info and loop_info[1] == Macros.Global.TRUE then
            info.loop = true
            info.bind_id = gen_bind_id()
        end

        self.__delay_create_list = self.__delay_create_list or {}
        table.insert(self.__delay_create_list, info)
    end

    if bind_id then
        self.__effect_bind_map[bind_id] = {}
    end

    return bind_id
end

--播放系统操作音效
function SoundMgr:PlaySystemOperEffect(sys_id, tag_id)
    local config_sys_oper_sound = CPPGameLib.GetConfig("SystemOperSound", sys_id)
    if not config_sys_oper_sound then
        return
    end

    local tag_info = config_sys_oper_sound.tag_list[tag_id]
    if not tag_info then
        LogTool.LogError("[SoundMgr:PlaySystemOperEffect] 系统操作音效中找不到对应的配置(sys_id:%d, tag_id:%d)", sys_id, tag_id)
        return
    end

    if not tag_info.res_name then
        return
    end

    self:PlayEffect(tag_info.res_name)
end

--暂停音效
function SoundMgr:PauseEffect(id)
    local effect_id_list = self.__effect_bind_map[id]
    if not effect_id_list then    
        CPPSoundManager:PauseEffect(id)
    else 
        for _, effect_id in ipairs(effect_id_list) do
            CPPSoundManager:PauseEffect(effect_id)
        end
    end
end

--暂停所有音效
function SoundMgr:PauseAllEffects()
    CPPSoundManager:PauseAllEffects()
end

--恢复音效
function SoundMgr:ResumeEffect(id)
    local effect_id_list = self.__effect_bind_map[id]
    if not effect_id_list then    
        CPPSoundManager:ResumeEffect(id)
    else 
        for _, effect_id in ipairs(effect_id_list) do
            CPPSoundManager:ResumeEffect(effect_id)
        end
    end
end

--恢复所有音效
function SoundMgr:ResumeAllEffects()
    CPPSoundManager:ResumeAllEffects()
end

--停止音效
function SoundMgr:StopEffect(id)
    local effect_id_list = self.__effect_bind_map[id]
    if not effect_id_list then    
        CPPSoundManager:StopEffect(id)
    else 
        self.__effect_bind_map[id] = nil

        for _, effect_id in ipairs(effect_id_list) do
            CPPSoundManager:StopEffect(effect_id)
        end
    end
end

--停止所有音效
function SoundMgr:StopAllEffects()
    CPPSoundManager:StopAllEffects()

    self.__played_effect_map_nowframe = {}

    for file_name, _ in pairs(self.__played_effect_map) do
        CPPSoundManager:UnloadEffect(file_name)
    end
    self.__played_effect_map = {}

    self.__effect_bind_map = {}
end

--设置音效音量
function SoundMgr:SetEffectVolume(var)
    CPPSoundManager:SetEffectsVolume(var)
end

--获取音效音量
function SoundMgr:GetEffectVolume()
    return CPPSoundManager:GetEffectsVolume()
end

--设置音效开关
function SoundMgr:SetEffectEnabled(var)
    Cookies.Set(nil, Macros.Setting.SoundEffectEnabled, var and Macros.Global.TRUE or Macros.Global.FALSE)

    if var then
        self:SetEffectVolume(1)
    else
        self:SetEffectVolume(0)
    end
end

--获取音效开关
function SoundMgr:GetEffectEnabled()
    local effect_enabled = Cookies.Get(nil, Macros.Setting.SoundEffectEnabled)
    effect_enabled = (effect_enabled and effect_enabled ~= "") and tonumber(effect_enabled) or Macros.Global.TRUE 
    return effect_enabled == Macros.Global.TRUE
end

--停止、卸载背景音乐、音效
function SoundMgr:Dump(cleanup_bgm)
    if cleanup_bgm then
        self:StopBGM(true)
    end

    self:StopAllEffects()
    self.__delay_create_list = nil
end

function SoundMgr:Pause()
     if self.__is_pause then
        return
    end
    self.__is_pause = true

    -- self:PauseAllEffects()
end

function SoundMgr:Resume()
    if not self.__is_pause then
        return
    end
    self.__is_pause = false

    -- self:ResumeAllEffects()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function SoundMgr:__GetFilePath(file_name, is_bgm)
    if is_bgm then
        return Resource.PathTool.GetBGMusicPath(file_name)
    else
        return Resource.PathTool.GetSoundEffectPath(file_name)
    end
end
