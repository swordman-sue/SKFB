
--[[
%% @module: 主界面相关数据定义
--]]

Modules = Modules or {}

Modules.MaincityModule = Modules.MaincityModule or BaseClass(BaseModule)

--区域类型
Modules.MaincityModule.RegionType = 
{
    BOTTOM_LEFT = 1,
    BOTTOM_RIGHT = 2,
    TOP_LEFT = 3,
    TOP_RIGHT = 4,
    CENTER_RIGHT = 5,
    CENTER = 6,
}

--一级区域布局信息表
Modules.MaincityModule.RegionLayoutMap = 
{
    --左下区域
    [Modules.MaincityModule.RegionType.BOTTOM_LEFT] = 
    {
        is_top_level_region = true,

        row = 1, 
        col_init_func = function()
            return CallModuleFunc(ModuleType.MAIN_CITY, "GetBotttomEntryLimitNumByRegion", Modules.MaincityModule.RegionType.BOTTOM_LEFT)
        end,
        item_w = 90, item_h = 90, item_margin_x = 90, item_margin_y = 0,
        entry_widget_anchor_x = 0, entry_widget_anchor_y = 0, entry_widget_border_x = -14,
        entry_txt_offset_y = 4,
        entry_red_dot_offset_x = -6,
        entry_red_dot_offset_y = -8,
        entry_show_beg_index = 1,   
        entry_show_end_index_init_func = function()
            --加上更多
            return CallModuleFunc(ModuleType.MAIN_CITY, "GetBotttomEntryLimitNumByRegion", Modules.MaincityModule.RegionType.BOTTOM_LEFT) + 1
        end,

        pos_func = PosTool.LeftBottom,        
    },

    --右下区域
    [Modules.MaincityModule.RegionType.BOTTOM_RIGHT] = 
    {
        is_top_level_region = true,

        row = 1, 
        col_init_func = function()
            return CallModuleFunc(ModuleType.MAIN_CITY, "GetBotttomEntryLimitNumByRegion", Modules.MaincityModule.RegionType.BOTTOM_RIGHT)
        end,        
        item_w = 90, item_h = 90, item_margin_x = -110, item_margin_y = 0,
        entry_widget_anchor_x = 1, entry_widget_anchor_y = 0, entry_widget_border_x = -20,
        entry_txt_size = Macros.TypefaceSize.largish,
        entry_txt_offset_y = 4,
        entry_red_dot_offset_x = 5,
        entry_red_dot_offset_y = 19,
        entry_show_beg_index = 1, 

        pos_func = PosTool.RightBottom,
    },

    --左上区域
    [Modules.MaincityModule.RegionType.TOP_LEFT] = 
    {
        is_top_level_region = true,
    
        row = 8888, col = 4,
        item_w = 90, item_h = 90, item_margin_x = 78, item_margin_y = -85,
        entry_btn_offset_y = 5,
        entry_widget_border_x = -14,
        entry_widget_anchor_y = 1,
        entry_txt_offset_y = 2,
        entry_txt_size = Macros.TypefaceSize.minimum,
        entry_red_dot_offset_x = -18,
        entry_red_dot_offset_y = -25,

        pos_func = PosTool.LeftTop,
    },

    --右上区域
    [Modules.MaincityModule.RegionType.TOP_RIGHT] = 
    {
        is_top_level_region = true,

        row = 8888, col = 6, 
        item_w = 90, item_h = 34, item_margin_x = -78, item_margin_y = -75, 
        entry_widget_anchor_x = 1, entry_widget_anchor_y = 1,
        entry_widget_border_x = 14,
        entry_btn_anchor_y = 0, entry_btn_offset_y = 10,
        entry_txt_size = Macros.TypefaceSize.minimum,
        entry_red_dot_offset_x = -13,
        entry_red_dot_offset_y = 34,

        pos_func = PosTool.RightTop,
    },    

    --右中区域
    [Modules.MaincityModule.RegionType.CENTER_RIGHT] = 
    {
        is_top_level_region = true,

        row = 8888, col = 1, 
        item_w = 90, item_h = 90, item_margin_y = 106,
        entry_widget_bg = "bg_entry_preLook",
        entry_widget_anchor_x = 0.5,
        entry_btn_offset_x = -2,
        entry_btn_offset_y = 3,
        entry_txt_offset_x = -3,
        entry_txt_offset_y = -3,

        pos_func = PosTool.CenterBottom,
    },

    --中区域
    [Modules.MaincityModule.RegionType.CENTER] = 
    {
        is_top_level_region = true,
    
        row = 1, col = 8888, 
        item_w = 72, item_h = 72, item_margin_x = 0,
        entry_widget_bg = "bg_limit_activity",
        entry_widget_anchor_x = 0.5,    
        entry_btn_offset_y = 4,
        entry_txt_offset_y = 2,
        entry_txt_size = Macros.TypefaceSize.minimum,
        module_txt_stroke = Macros.Color.limit_activity_stroke,
        is_special_func = true,
    },
}

--二级区域布局信息表(限定每个一级区域中的二级区域入口数为1)
Modules.MaincityModule.SecondRegionLayoutMap = 
{
    --正下区域
    [Modules.MaincityModule.RegionType.BOTTOM_LEFT] = 
    {
        is_second_region = true,

        row = 8888, col = 4,
        item_w = 90, item_h = 90, item_margin_x = 90, item_margin_y = -90,
        entry_widget_anchor_x = 0, entry_widget_anchor_y = 1,
        entry_txt_offset_y = 0,
        entry_btn_offset_y = 0,
        entry_red_dot_offset_x = -12,
        entry_red_dot_offset_y = -13,   
        entry_show_beg_index_init_func = function()
            return CallModuleFunc(ModuleType.MAIN_CITY, "GetBotttomEntryLimitNumByRegion", Modules.MaincityModule.RegionType.BOTTOM_LEFT) + 2
        end,

        pos_func = PosTool.LeftTop,
    },
}

--扩展区域布局信息表(不限定每个一级区域中的扩展区域入口数)
Modules.MaincityModule.ExtendRegionLayoutMap = 
{
    --正下区域
    [Modules.MaincityModule.RegionType.BOTTOM_LEFT] = 
    {
        is_extend_region = true,

        row = 8888, col = 4,
        item_w = 90, item_h = 90, item_margin_x = 90, item_margin_y = -90,
        entry_widget_anchor_x = 0, entry_widget_anchor_y = 1,
        entry_txt_offset_y = 0,
        entry_btn_offset_y = 0,
        entry_show_beg_index = 1,   
        is_updir = true,

        pos_func = PosTool.LeftTop,
    },

    --右上区域
    [Modules.MaincityModule.RegionType.TOP_RIGHT] = 
    {
        is_extend_region = true,

        row = 8888, col = 4,
        item_w = 90, item_h = 90, item_margin_x = -76, item_margin_y = -76, 
        entry_widget_anchor_x = 1, entry_widget_anchor_y = 1,entry_btn_anchor_x = 1,
        entry_btn_anchor_y = 0, entry_btn_offset_y = 20,
        entry_txt_offset_y = 10,
        entry_red_dot_offset_x = -10,
        entry_red_dot_offset_y = -5,    
        is_updir = false,

        pos_func = PosTool.RightTop,
    },

    --右下区域
    [Modules.MaincityModule.RegionType.BOTTOM_RIGHT] = 
    {
        is_extend_region = true,
    
        row = 8888, col = 4,
        item_w = 90, item_h = 90, item_margin_x = 90, item_margin_y = -90,
        entry_widget_anchor_x = 0, entry_widget_anchor_y = 1,
        entry_txt_offset_y = 0,entry_btn_offset_y = 0,
        entry_show_beg_index = 1, 
        extend_container_offset_x = -80,
        is_updir = true,
        
        pos_func = PosTool.LeftTop,        
    },    
}

--左下方一级区域“更多”入口ID
Modules.MaincityModule.BOTTOM_LEFT_MORE_ENERY_ID = 20001

--IOS审核状态下的背景列表

--初始化布局信息的默认参数
function Modules.MaincityModule.InitRegionLayoutInfo(region_layout_info)
    if not region_layout_info then
        return
    end

    if not region_layout_info.col and region_layout_info.col_init_func then
        region_layout_info.col = region_layout_info.col_init_func()
    end

    if not region_layout_info.entry_show_beg_index and region_layout_info.entry_show_beg_index_init_func then
        region_layout_info.entry_show_beg_index = region_layout_info.entry_show_beg_index_init_func()
    end

    if not region_layout_info.entry_show_end_index and region_layout_info.entry_show_end_index_init_func then
        region_layout_info.entry_show_end_index = region_layout_info.entry_show_end_index_init_func()
    end

    --入口索引
    region_layout_info.index = 0

    --入口控件锚点
    region_layout_info.entry_widget_anchor_x = region_layout_info.entry_widget_anchor_x or 0
    region_layout_info.entry_widget_anchor_y = region_layout_info.entry_widget_anchor_y or 0

    --入口控件与边界的间隔
    region_layout_info.entry_widget_border_x = region_layout_info.entry_widget_border_x or 0
    region_layout_info.entry_widget_border_y = region_layout_info.entry_widget_border_y or 0    

    --入口控件间隔
    region_layout_info.item_margin_x = region_layout_info.item_margin_x or 0
    region_layout_info.item_margin_y = region_layout_info.item_margin_y or 0

    --入口按钮锚点
    region_layout_info.entry_btn_anchor_x = region_layout_info.entry_btn_anchor_x or 0.5
    region_layout_info.entry_btn_anchor_y = region_layout_info.entry_btn_anchor_y or 0.5    

    --入口按钮偏移
    region_layout_info.entry_btn_offset_x = region_layout_info.entry_btn_offset_x or 0
    region_layout_info.entry_btn_offset_y = region_layout_info.entry_btn_offset_y or 10

    --入口文字偏移
    region_layout_info.entry_txt_offset_x = region_layout_info.entry_txt_offset_x or 0
    region_layout_info.entry_txt_offset_y = region_layout_info.entry_txt_offset_y or 0

    --入口文字字号
    region_layout_info.entry_txt_size = region_layout_info.entry_txt_size or Macros.TypefaceSize.slightly

    --入口起止索引(entry_show_end_index后的入口归纳到二级入口)
    region_layout_info.entry_show_beg_index = region_layout_info.entry_show_beg_index or 1
    region_layout_info.entry_show_end_index = region_layout_info.entry_show_end_index or 8888

    --入口红点偏移
    region_layout_info.entry_red_dot_offset_x = region_layout_info.entry_red_dot_offset_x or 0
    region_layout_info.entry_red_dot_offset_y = region_layout_info.entry_red_dot_offset_y or 0
end

--获取区域布局信息
function Modules.MaincityModule.GetRegionLayoutInfo(layout_map, region_type, do_init)
    if not layout_map or not region_type then
        return
    end

    local region_layout_info = layout_map[region_type]
    if do_init then
        Modules.MaincityModule.InitRegionLayoutInfo(region_layout_info)
    end

    return region_layout_info
end