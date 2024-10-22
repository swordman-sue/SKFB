--[[
%% @module: 自动释放工具
%% @author: swordman sue
%% @created: 2018-12-7
%% @description: 用于管理需要自动释放的对象
    通过AddObj，让对象持有者托管对象并返回对象索引，外部通过对象索引操控对象
    通过GetObj，获取对象
    通过DelObj，让对象持有者销毁对象
    通过DelAllObjs，让对象持有者销毁所有对象
--]]

AutoReleaseTool = AutoReleaseTool or {}

--[[
添加对象
@param owner                        对象持有者(BaseView/Template/UILayer派生类实例)
@param obj                          对象(需自动释放的对象，注：1、确保支持GetNode、DeleteMe接口。2、在调用此接口前，确保obj的节点已添加到父容器)
@param obj_key[option]              对象索引
@param priority4autorelease[option] 对象自动销毁的优先级(优先销毁数值小的对象)
@return obj_key                     对象索引
]]
function AutoReleaseTool.AddObj(owner, obj, obj_key, priority4autorelease)
    if not owner then
        LogTool.LogError("[AutoReleaseTool.AddObj] invalid param owner")
        return
    end

    if not obj then
        LogTool.LogError("[AutoReleaseTool.AddObj] invalid param obj")
        return
    end

    local node = obj:GetNode()
    if not node then
        LogTool.LogError("[AutoReleaseTool.AddObj] make sure that obj has node")
        return
    end
    if not node:GetParent() then
        LogTool.LogError("[AutoReleaseTool.AddObj] make sure that obj's node has added to parent before this function was called")
        return
    end

    owner.auto_release_obj_map = owner.auto_release_obj_map or {}
    if obj_key and owner.auto_release_obj_map[obj_key] then
        LogTool.LogError("[AutoReleaseTool.AddObj] the obj_key:%s has existed", obj_key)
        return
    end

    owner.auto_release_obj_key = owner.auto_release_obj_key or 0
    owner.auto_release_obj_key = owner.auto_release_obj_key + 1
    obj_key = obj_key or ("auto_release_obj_key" .. owner.auto_release_obj_key)
    obj.priority4autorelease = priority4autorelease or MathTool.MAX_NUM
    owner.auto_release_obj_map[obj_key] = obj

    return obj_key
end

--[[
获取对象
@param owner        对象持有者
@param obj_key      对象索引
]]
function AutoReleaseTool.GetObj(owner, obj_key)
    if not owner then
        LogTool.LogError("[AutoReleaseTool.GetObj] invalid param owner")
        return
    end

    if not obj_key then
        LogTool.LogError("[AutoReleaseTool.GetObj] invalid param obj_key")
        return
    end

    if not owner.auto_release_obj_map then
        return
    end
    return owner.auto_release_obj_map[obj_key]
end

--[[
删除对象
@param owner        对象持有者
@param obj_key      对象索引
]]
function AutoReleaseTool.DelObj(owner, obj_key)
    if not owner then
        LogTool.LogError("[AutoReleaseTool.DelObj] invalid param owner")
        return
    end

    if not obj_key then
        LogTool.LogError("[AutoReleaseTool.DelObj] invalid param obj_key")
        return
    end

    if not owner.auto_release_obj_map then
        return
    end
    local obj = owner.auto_release_obj_map[obj_key]
    if obj then
        obj:DeleteMe()
        owner.auto_release_obj_map[obj_key] = nil
    end
end

--[[
删除所有对象
@param owner        对象持有者
]]
function AutoReleaseTool.DelAllObjs(owner)
    if not owner then
        LogTool.LogError("[AutoReleaseTool.DelAllObjs] invalid param owner")
        return
    end

    if not owner.auto_release_obj_map then
        return
    end

    local order_dependent_list, order_independent_list
    for _, obj in pairs(owner.auto_release_obj_map) do
        if obj.priority4autorelease < MathTool.MAX_NUM then
            order_dependent_list = order_dependent_list or {}
            table.insert(order_dependent_list, obj)
        else
            order_independent_list = order_independent_list or {}
            table.insert(order_independent_list, obj)
        end
    end    
    if order_dependent_list then
        table.sort(order_dependent_list, SortTools.KeyLowerSorter("priority4autorelease"))    
        for _, obj in ipairs(order_dependent_list) do
            obj:DeleteMe()
        end
    end
    if order_independent_list then
        for _, obj in ipairs(order_independent_list) do
            obj:DeleteMe()
        end
    end

    owner.auto_release_obj_map = nil
end
