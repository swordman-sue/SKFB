
--[[
%% @module: 场景模块
%% @author: swordman sue
%% @created: 2017-09-25
%% @description: 处理多人场景相关
--]]

Modules = Modules or {}

Modules.SceneModule = Modules.SceneModule or BaseClass(BaseModule)

function Modules.SceneModule:__init()
end

function Modules.SceneModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--场景进入请求
function Modules.SceneModule:EnterSceneReq(p_type, p_id, to_last_scene)
    -- print("skfb", "EnterSceneReq", p_type, p_id, to_last_scene)
    if self.__is_enteter_scene and to_last_scene then
        self:MonsterListNotice({monster_list = self.__monster_map})
        self:ItemListNotice({item_list = self.__item_map})
        self:EnterSceneResp(
            {player_list = self.__player_map, map_x = self.__my_pos_x, map_y = self.__my_pos_y, type = self.__scene_type, context = self.__scene_context})
        return
    end

    self:LeaveSceneResp()

    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCENE_ENTER_REQ)
    protocol.type = p_type
    protocol.context = p_id
    GlobalNetAdapter:OnSend(protocol)
end

--场景进入响应
function Modules.SceneModule:EnterSceneResp(protocol_data)
    -- print("skfb", "EnterSceneResp", protocol_data.type, protocol_data.context, protocol_data.map_x, protocol_data.map_y)

    self.__scene_type = protocol_data.type
    self.__scene_context = protocol_data.context
    self.__my_pos_x = protocol_data.map_x
    self.__my_pos_y = protocol_data.map_y

    if self.__player_map ~= protocol_data.player_list then
        self.__player_map = {}
        for _, player_info in pairs(protocol_data.player_list) do
            self.__player_map[player_info.role_id] = player_info
        end
    end

    self.__is_enteter_scene = true

    GlobalScene:ChangeScene(protocol_data.type, protocol_data.context)        
end

--场景进入通知
function Modules.SceneModule:EnterSceneNotice(protocol_data)
    if not self.__is_enteter_scene then
        return
    end

    self.__player_map[protocol_data.role_id] = protocol_data
    if protocol_data.role_id == CallModuleFunc(ModuleType.ROLE, "GetRoleID") then
        self.__my_pos_x = protocol_data.map_x
        self.__my_pos_y = protocol_data.map_y
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleEnterSceneNotice", protocol_data)
end

--场景离开请求
function Modules.SceneModule:LeaveSceneReq()
    if not self.__is_enteter_scene then
        return
    end

    local scene_info = GlobalVOMgr:GetSceneInfo()
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCENE_LEAVE_REQ)
    protocol.type = scene_info.type
    GlobalNetAdapter:OnSend(protocol)        
end

--场景离开响应
function Modules.SceneModule:LeaveSceneResp(protocol_data)
    -- print("skfb", "LeaveSceneResp", protocol_data)

    self.__player_map = nil
    self.__monster_map = nil
    self.__item_map = nil
    self.__scene_type = nil
    self.__scene_context = nil
    self.__is_enteter_scene = false
    self:ClearRecordOfObjID()
end

--场景离开通知
function Modules.SceneModule:LeaveSceneNotice(protocol_data)
    -- print("skfb", "LeaveSceneNotice", protocol_data.role_id)
    if not self.__is_enteter_scene then
        return
    end

    if CallModuleFunc(ModuleType.ROLE, "GetRoleID") == protocol_data.role_id then
        self:LeaveSceneResp()
        return                
    end

    if self.__player_map then
        self.__player_map[protocol_data.role_id] = nil
    end
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleLeaveSceneNotice", protocol_data)
end

--英雄行走请求
function Modules.SceneModule:HeroGotoReq(src_pos, dst_pos)
    if not self.__is_enteter_scene then
        return
    end

    local scene_info = GlobalVOMgr:GetSceneInfo()
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HERO_GOTO_REQ)
    protocol.type = scene_info.type
    protocol.src_x, protocol.src_y = GlobalScene:WorldToPixel(src_pos.x, src_pos.y)
    protocol.dest_x, protocol.dest_y = GlobalScene:WorldToPixel(dst_pos.x, dst_pos.y)
    GlobalNetAdapter:OnSend(protocol)    
end

--英雄行走响应
function Modules.SceneModule:HeroGotoResp(protocol_data)
    if not self.__is_enteter_scene then
        return
    end

    self.__my_pos_x, self.__my_pos_y = protocol_data.dest_x, protocol_data.dest_y
end

--英雄行走通知
function Modules.SceneModule:HeroGotoNotice(protocol_data)
    if not self.__is_enteter_scene then
        return
    end

    local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
    if role_id == protocol_data.role_id then
        return
    end

    local player_info = self.__player_map[protocol_data.role_id]
    if player_info then
        player_info.map_x, player_info.map_y = protocol_data.dest_x, protocol_data.dest_y
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleHeroGotoNotice", protocol_data)
end

--怪物列表通知(场景进入响应前下发)
function Modules.SceneModule:MonsterListNotice(protocol_data)
    if self.__monster_map == protocol_data.monster_list then
        return
    end

    -- CPPGameLib.PrintTable("skfb", protocol_data.monster_list, "MonsterListNotice")

    self.__monster_map = self.__monster_map or {}
    for _, monster_info in pairs(protocol_data.monster_list) do
        if not self.__monster_map[monster_info.uid] and monster_info.total_hp - monster_info.damage > 0 then
            self.__monster_map[monster_info.uid] = monster_info

            --添加怪物
            CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleMonsterAddNotice", monster_info)
        end
    end
end

--怪物数据通知
function Modules.SceneModule:MonsterDataNotice(protocol_data)
    if not self.__is_enteter_scene then
        return
    end

    local monster_info = self.__monster_map and self.__monster_map[protocol_data.uid] or nil
    if not monster_info then
        return
    end

    -- CPPGameLib.PrintTable("skfb", protocol_data, "MonsterDataNotice")

    monster_info.damage = protocol_data.damage
    monster_info.total_hp = protocol_data.total_hp
    monster_info.start_battle_time = protocol_data.start_battle_time
    if monster_info.damage >= monster_info.total_hp then
        --删除怪物
        self.__monster_map[monster_info.uid] = nil

        --杀怪事件
        self:FireNextFrame(Macros.Event.SceneObj.MT_MONSTER_KILLED)
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleMonsterDataNotice", monster_info)
end

--场景物品列表通知(场景进入响应前下发)
function Modules.SceneModule:ItemListNotice(protocol_data)
    if self.__item_map == protocol_data.item_list then
        return
    end
    -- print("skfb", "ItemListNotice")

    self.__item_map = self.__item_map or {}
    for _, item_info in pairs(protocol_data.item_list) do
        if not self.__item_map[item_info.uid] then
            self.__item_map[item_info.uid] = item_info

            --添加场景物品
            CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleItemAddNotice", item_info)
        end
    end    
end

--场景物品消失通知
function Modules.SceneModule:ItemDisappearNotice(protocol_data)
    -- print("skfb", "ItemDisappearNotice ", protocol_data.uid)    
    if not self.__is_enteter_scene then
        return
    end

    if self.__item_map then
        local item_info = self.__item_map[protocol_data.uid]
        if item_info then
            CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleItemDisappearNotice", item_info)
        end
        self.__item_map[protocol_data.uid] = nil
    end
end

--场景物品锁定请求
function Modules.SceneModule:ItemLockReq(uid)
    -- print("skfb", "ItemLockReq", uid)
    if not self.__is_enteter_scene then
        return
    end

    local scene_info = GlobalVOMgr:GetSceneInfo()
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCENE_ITEM_LOCK_REQ)
    protocol.type = scene_info.type
    protocol.uid = uid
    GlobalNetAdapter:OnSend(protocol)    
end

--场景物品锁定响应
function Modules.SceneModule:ItemLockResp(protocol_data)
    -- print("skfb", "ItemLockResp", protocol_data.uid)
    if not self.__is_enteter_scene then
        return
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleItemLockResp", protocol_data)
end

--场景物品锁定通知
function Modules.SceneModule:ItemLockNotify(protocol_data)
    -- print("skfb", "ItemLockNotify", protocol_data.uid)
    if not self.__is_enteter_scene then
        return
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleItemLockNotify", protocol_data)
end

--场景物品拾取请求
function Modules.SceneModule:ItemPickupReq(uid)
    -- print("skfb", "ItemPickupReq", uid)
    if not self.__is_enteter_scene then
        return
    end

    local scene_info = GlobalVOMgr:GetSceneInfo()
    local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SCENE_ITEM_PICK_UP_REQ)
    protocol.type = scene_info.type
    protocol.uid = uid
    GlobalNetAdapter:OnSend(protocol)        
end

--场景物品拾取响应
function Modules.SceneModule:ItemPickupResp(protocol_data)
    -- print("skfb", "ItemPickupResp ", protocol_data.uid)

    if not self.__is_enteter_scene then
        return
    end   

    --奖励弹窗
    if #protocol_data.item_list > 0 then
        local reward_list = {}
        for i, item_info in ipairs(protocol_data.item_list) do
            table.insert(reward_list, {id = item_info.id , num = item_info.num})
        end
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list )      
    end

    --拾取事件
    self:FireNextFrame(Macros.Event.SceneObj.MT_ITEM_PICK_UP)

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleItemPickupResp", protocol_data)
end

--场景物品拾取通知
function Modules.SceneModule:ItemPickupNotify(protocol_data)
    -- print("skfb", "ItemPickupNotify ", protocol_data.uid, protocol_data.role_id)
    if not self.__is_enteter_scene then
        return
    end   

    --拾取事件
    self:FireNextFrame(Macros.Event.SceneObj.MT_ITEM_PICK_UP)

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleItemPickupNotify", protocol_data)
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--离开场景并回到主城
function Modules.SceneModule:LeaveReturnCity()
    self:LeaveSceneReq()
    GlobalScene:ChangeScene(Macros.Game.FBType.MAIN_CITY)
end

--获取玩家列表
function Modules.SceneModule:GetPlayerMap()
    return self.__player_map
end

--获取怪物列表
function Modules.SceneModule:GetMonsterList()
    return self.__monster_map
end

--获取场景物品列表
function Modules.SceneModule:GetItemList()
    return self.__item_map
end

--获取我的位置
function Modules.SceneModule:GetMyPos()
    return self.__my_pos_x, self.__my_pos_y
end

--将本地对象ID转换为服务器对象ID
function Modules.SceneModule:ConvertCID2SID(type, c_obj_id)
    if self.__cs_obj_id_map and self.__cs_obj_id_map[type] then
        return self.__cs_obj_id_map[type][c_obj_id] or 0
    end
    return 0
end

--将服务器对象ID转换为本地对象ID
function Modules.SceneModule:ConvertSID2CID(type, s_obj_id)
    if self.__sc_obj_id_map and self.__sc_obj_id_map[type] then
        return self.__sc_obj_id_map[type][s_obj_id] or 0
    end
    return 0
end

--记录双端对象ID映射关系
function Modules.SceneModule:RecordObjID(type, c_obj_id, s_obj_id)
    self.__cs_obj_id_map = self.__cs_obj_id_map or {}
    self.__cs_obj_id_map[type] = self.__cs_obj_id_map[type] or {}
    self.__sc_obj_id_map = self.__sc_obj_id_map or {}
    self.__sc_obj_id_map[type] = self.__sc_obj_id_map[type] or {}

    self.__cs_obj_id_map[type][c_obj_id] = s_obj_id
    self.__sc_obj_id_map[type][s_obj_id] = c_obj_id
end

--清空双端对象ID映射关系
function Modules.SceneModule:ClearRecordOfObjID()
    self.__cs_obj_id_map = nil
    self.__sc_obj_id_map = nil
end

--获取服务器对象ID对应的客户端对象
function Modules.SceneModule:GetObjBySID(type, s_obj_id)
    local obj_id = CallModuleFunc(ModuleType.SCENE, "ConvertSID2CID", type, s_obj_id)
    local obj = GlobalScene:GetObj(obj_id)
    return obj
end

--判断是否在战斗
function Modules.SceneModule:JudgeIsInBattle(start_battle_time, is_boss)
    if not start_battle_time then
        return
    end

    if is_boss then
        return
    end

    --战斗时间
    if start_battle_time > 0 then 
        local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")    
        return (cur_server_time - start_battle_time) <= ConfigAdapter.Common.GetMonsterStormCastleBattleCD()
    else
        return false
    end
end
