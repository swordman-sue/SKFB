--[[
%% @module: 多分辨率适配工具
%% @author: swordman sue
%% @created: 2019-01-23
--]]

GUI = GUI or {}

GUI.MultiResolutionAdapter = GUI.MultiResolutionAdapter or {}

--横屏多分辨适配处理(界面根节点)
local MultiResolutionAdapt_LandScape

--竖屏多分辨适配处理(界面根节点)
local MultiResolutionAdapt_Portrait

--横屏多分辨适配处理(界面背景图)
local MultiResolutionAdaptForCommonBG_LandScape

--竖屏多分辨适配处理(界面背景图)
local MultiResolutionAdaptForCommonBG_Portrait

--多分辨适配处理(界面根节点)
function GUI.MultiResolutionAdapter.Adapt(root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    if not root_widget then
        LogTool.LogError("[GUI.MultiResolutionAdapter.Adapt] Invalid root_widget")
        return
    end

    local screen_resolution_policy = CPPSceneManager:GetResolutionPolicy()
    if screen_resolution_policy == CPPSceneManager.SRP_FIXED_HEIGHT then
        MultiResolutionAdapt_LandScape(root_widget, is_force_stretch, is_ignore_iphone_safe_area)

    elseif screen_resolution_policy == CPPSceneManager.SRP_FIXED_WIDTH then
        MultiResolutionAdapt_Portrait(root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    
    else
        LogTool.LogError("[MultiResolutionAdapter.Adapt] Invalid screen_resolution_policy:%d", screen_resolution_policy)
    end
end

--多分辨适配处理(界面背景图)
function GUI.MultiResolutionAdapter.AdaptForBG(common_bg, root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    if not common_bg then
        LogTool.LogError("[GUI.MultiResolutionAdapter.AdaptForBG] Invalid common_bg")
        return
    end

    if not root_widget then
        LogTool.LogError("[GUI.MultiResolutionAdapter.AdaptForBG] Invalid root_widget")
        return
    end

    local screen_resolution_policy = CPPSceneManager:GetResolutionPolicy()
    if screen_resolution_policy == CPPSceneManager.SRP_FIXED_HEIGHT then
        MultiResolutionAdaptForCommonBG_LandScape(common_bg, root_widget, is_force_stretch, is_ignore_iphone_safe_area)

    elseif screen_resolution_policy == CPPSceneManager.SRP_FIXED_WIDTH then
        MultiResolutionAdaptForCommonBG_Portrait(common_bg, root_widget, is_force_stretch, is_ignore_iphone_safe_area)

    else
        LogTool.LogError("[MultiResolutionAdapter.AdaptForBG] Invalid screen_resolution_policy:%d", screen_resolution_policy)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--横屏多分辨适配处理(界面根节点)
MultiResolutionAdapt_LandScape = function(root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    local is_do_refresh

    --兼容横竖屏过渡期中，界面超出屏幕的情况
    local root_widget_height = root_widget:GetContentHeight()
    if root_widget_height > Macros.Global.DesignSizeDef.height then
        root_widget:SetContentHeight(Macros.Global.DesignSizeDef.height)
        is_do_refresh = true
    end

    local root_widget_width = root_widget:GetContentWidth()
    root_widget_width = root_widget_width <= Macros.Global.DesignSizeDef.width and Macros.Global.DesignSizeDef.width or root_widget_width

    --处理IPhoneX刘海遮挡问题
    local design_size = (Macros.Global.IphoneSafeAreaRect and not is_ignore_iphone_safe_area) and Macros.Global.IphoneSafeAreaRect.size or Macros.Global.DesignSize

    if design_size.width >= root_widget_width or is_force_stretch then
        --横向拉伸处理
        root_widget:SetSize(design_size.width, design_size.height)
        is_do_refresh = true
    else
        --整体缩放处理
        local scale = design_size.width / root_widget_width
        root_widget:SetScaleFactor(scale)
    end

    --刷新布局(根节点尺寸变化后，需调用此函数刷新按照百分比坐标、尺寸布局的子节点)
    if is_do_refresh then
        root_widget:RefreshLayoutComponent()
    end    
end

--竖屏多分辨适配处理(界面根节点)
MultiResolutionAdapt_Portrait = function(root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    local is_do_refresh

    --兼容横竖屏过渡期中，界面超出屏幕的情况
    local root_widget_width = root_widget:GetContentWidth()
    if root_widget_width > Macros.Global.DesignSizeDef.width then
        root_widget:SetContentWidth(Macros.Global.DesignSizeDef.width)
        is_do_refresh = true
    end

    local root_widget_height = root_widget:GetContentHeight()
    root_widget_height = root_widget_height <= Macros.Global.DesignSizeDef.height and Macros.Global.DesignSizeDef.height or root_widget_height

    --处理IPhoneX刘海遮挡问题
    local design_size = (Macros.Global.IphoneSafeAreaRect and not is_ignore_iphone_safe_area) and Macros.Global.IphoneSafeAreaRect.size or Macros.Global.DesignSize

    if design_size.height >= root_widget_height or is_force_stretch then
        --纵向拉伸处理
        root_widget:SetSize(design_size.width, design_size.height)
        is_do_refresh = true
    else
        --整体缩放处理
        local scale = design_size.height / root_widget_height
        root_widget:SetScaleFactor(scale)
    end

    --刷新布局(根节点尺寸变化后，需调用此函数刷新按照百分比坐标、尺寸布局的子节点)
    if is_do_refresh then
        root_widget:RefreshLayoutComponent()
    end
end

--横屏多分辨适配处理(界面背景图)
MultiResolutionAdaptForCommonBG_LandScape = function(common_bg, root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    local root_widget_width = root_widget:GetContentWidth()
    root_widget_width = root_widget_width <= Macros.Global.DesignSizeDef.width and Macros.Global.DesignSizeDef.width or root_widget_width

    --处理IPhoneX刘海遮挡问题
    local design_size = (Macros.Global.IphoneSafeAreaRect and not is_ignore_iphone_safe_area) and Macros.Global.IphoneSafeAreaRect.size or Macros.Global.DesignSize

    --对背景图进行与根节点的反向缩放(确保它铺满屏幕)
    if design_size.width < root_widget_width and not is_force_stretch then
        local root_scale = design_size.width / root_widget_width
        local bg_scale = common_bg:GetScaleFactor()
        common_bg:SetScaleFactor(bg_scale / root_scale)
    end
end

--竖屏多分辨适配处理(界面背景图)
MultiResolutionAdaptForCommonBG_Portrait = function(common_bg, root_widget, is_force_stretch, is_ignore_iphone_safe_area)
    local root_widget_height = root_widget:GetContentHeight()
    root_widget_height = root_widget_height <= Macros.Global.DesignSizeDef.height and Macros.Global.DesignSizeDef.height or root_widget_height

    --处理IPhoneX刘海遮挡问题
    local design_size = (Macros.Global.IphoneSafeAreaRect and not is_ignore_iphone_safe_area) and Macros.Global.IphoneSafeAreaRect.size or Macros.Global.DesignSize

    --对背景图进行与根节点的反向缩放(确保它铺满屏幕)
    if design_size.height < root_widget_height and not is_force_stretch then
        local root_scale = design_size.height / root_widget_height
        local bg_scale = common_bg:GetScaleFactor()
        common_bg:SetScaleFactor(bg_scale / root_scale)
    end
end
