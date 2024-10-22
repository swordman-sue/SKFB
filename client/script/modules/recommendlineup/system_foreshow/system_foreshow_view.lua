--[[
%% @module: 功能预告界面
%% @author: ldx
%% @created: 2018-09-21
--]]


Modules = Modules or {} 

Modules.SystemForeshowView = Modules.SystemForeshowView or BaseClass(BaseView)

function Modules.SystemForeshowView:__init()
    self.__layout_name = "system_foreshow"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
      self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RECOMMENDLINEUP)    
end

function Modules.SystemForeshowView:__OpenCallback()
  local sort_list,is_no_foreshow = CallModuleFunc(ModuleType.RECOMMEND_LINEUP,"SortSystemForeshowList")
   --系统图标
   local panel_icon = self:GetWidget("Panel_Icon")
   local icon_sp = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(sort_list[1].icon_id))
   panel_icon:AddChild(icon_sp)
   PosTool.Center(icon_sp)
   icon_sp:SetScaleFactor(sort_list[1].icon_zoom)
   --开放等级
   local txt_open = Util:RichText(string.format(Macros.Color.system_foreshow_sm_stroke_rt, CPPGameLib.GetString("reinforcements_lockLevelLimit",sort_list[1].open_level)), Macros.TypefaceSize.popup)
   txt_open:SetAnchorPoint(0.5,0)
   panel_icon:AddChild(txt_open)
   PosTool.CenterBottom(txt_open,0,10)
   --系统名字
   local panel_richtext = self:GetWidget("Panel_RichText")
   local txt_system = Util:RichText(string.format(Macros.Color.system_foreshow_bg_stroke_rt,sort_list[1].system_name or ""), 50)
   txt_system:SetAnchorPoint(0,0.5)
   panel_richtext:AddChild(txt_system)
   PosTool.LeftCenter(txt_system,-20,0)
   txt_system:SetSkewX(15)
   --系统介绍
   Util:WidgetLabel(self:GetWidget("Text_SystemIntroduce"),sort_list[1].system_des or "", Macros.TypefaceSize.slightly, Macros.Color.btn, 250, 50, Macros.Color.limit_activity_stroke, 2)
   --点击任意位置继续
   Util:WidgetLabel(self:GetWidget("Text_Tips"),CPPGameLib.GetString("hero_recruit_touchscreen_2"), Macros.TypefaceSize.popup, Macros.Color.btn)  
   --
   WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil, function ()
     self:CloseManual()
   end)
end 

function Modules.SystemForeshowView:__Dispose()   
    
end