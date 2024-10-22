
--[[
%% @module: 定制逻辑适配管理器(用于特殊版本的逻辑适配)
%% @author: swordman sue
%% @created: 2018-12-7
--]]

CustomizedLogicAdapter = CustomizedLogicAdapter or {}

CustomizedLogicAdapter.Manager = CustomizedLogicAdapter.Manager or BaseClass()

--逻辑适配类注册表
local CustomizedLogicAdapterClassMap = 
{
    [Macros.SDK.CPChannelType.NNS_BT] = {"NNS_BT", "modules/customized_logic_adapter/customized_logic_adapter_nns_bt"},
}

function CustomizedLogicAdapter.Manager.CreateInstance()
    if not GlobalCustomizedLogicAdapterMgr then
        GlobalCustomizedLogicAdapterMgr = CustomizedLogicAdapter.Manager.New()
    end
end

function CustomizedLogicAdapter.Manager.DelInstance()
    if GlobalCustomizedLogicAdapterMgr then
        GlobalCustomizedLogicAdapterMgr:DeleteMe()
        GlobalCustomizedLogicAdapterMgr = nil
    end
end

function CustomizedLogicAdapter.Manager:__init()
    local cp_channel_type = CPPSdkTools.GetCPChannelType()
    local logic_adapter_class_info = CustomizedLogicAdapterClassMap[cp_channel_type]
    if logic_adapter_class_info then
        local class_name = logic_adapter_class_info[1]
        local class_file = logic_adapter_class_info[2]
        RequireToolBatch:LoadFilesSync({file_list = {class_file}})

        local logic_adapter_class = CustomizedLogicAdapter[class_name]
        if logic_adapter_class then
            self.__adapter = logic_adapter_class.New()
            self.__adapter:SetCPChannelType(cp_channel_type)
        end
    end

    --创建默认适配器
    if not self.__adapter then
        self.__adapter = CustomizedLogicAdapter.Base.New()
    end
end

function CustomizedLogicAdapter.Manager:__delete()
end

--[[
example:
--外部调用的适配接口(调整字体颜色...)
function CustomizedLogicAdapter.Manager:Call4Color_XX(func_name, ...)
    if not self.__adapter or self.__adapter:GetCPChannelType() ~= XX then
        return
    end

    self:Call(func_name, ...)
end

--外部调用的适配接口(调整字体大小...)
function CustomizedLogicAdapter.Manager:Call4Size_XX(func_name, ...)
    if not self.__adapter or self.__adapter:GetCPChannelType() ~= XX then
        return
    end

    self:Call(func_name, ...)
end

--内部调用的适配接口(调整文本颜色) SetFontColor2, SetFontColor3....
function CustomizedLogicAdapter.XX:SetFontColor1(node, color)
end

--内部调用的适配接口(调整文本尺寸), SetFontSize2, SetFontSize3....
function CustomizedLogicAdapter.XX:SetFontSize1(node, size)
end

--内部调用的适配接口(调整位置、尺寸、图片之类)
function CustomizedLogicAdapter.XX:classname_ctrlname(node)
end
--]]

--[[
外部调用的适配接口(调整位置、尺寸、图片之类)
]]
function CustomizedLogicAdapter.Manager:Call(func_name, ...)
    if not func_name then
        LogTool.LogError("[CustomizedLogicAdapter.Manager:Call] 函数名为空")     
        return
    end

    if not self.__adapter then
        return
    end

    if self.__adapter[func_name] then
        return self.__adapter[func_name](self.__adapter, ...)
    end
end
