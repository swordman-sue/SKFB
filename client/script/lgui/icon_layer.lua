
--[[
%% @module: 通用图标
--]]

LogicGUI = LogicGUI or {}

LogicGUI.IconLayer = LogicGUI.IconLayer or BaseClass(UILayer)

--内部层次
LogicGUI.IconLayer.InnerZOrder = 
{
  ICON_BG = 1,
  ICON = 2,
  ICON_QUALITY = 3,
  PANEL_DATA = 4,

  MAX = 888,
}

--图标获取路径表
LogicGUI.IconLayer.IconPathFuncMap = 
{
  [Macros.Game.ObjType.CURRENCY] = function(id)
    local config = CPPGameLib.GetConfig("HuoBi", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.big_icon) 
  end,

  [Macros.Game.ObjType.ITEM] = function(id)
    local config = CPPGameLib.GetConfig("Item", id)
    if not config then return "" end

    if  config.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
      return Resource.PathTool.GetHeroHeadPath(config.res_id) 
    else
      return Resource.PathTool.GetItemPath(config.res_id)       
    end
  end,

  [Macros.Game.ObjType.EQUIP] = function(id)
    local config = CPPGameLib.GetConfig("Equipment", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.res_id)
  end,

  [Macros.Game.ObjType.HERO] = function(id)
    local config = CPPGameLib.GetConfig("Hero", id)
    if not config then return "" end

    return Resource.PathTool.GetHeroHeadPath(config.head_icon)
  end,

  [Macros.Game.ObjType.TREASURE] = function(id)
    local config = CPPGameLib.GetConfig("Treasure", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.res_id) 
  end,

  [Macros.Game.ObjType.AWAKEN_TOOL] = function(id)
    local config = CPPGameLib.GetConfig("AwakeningItem", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.res_id) 
  end,

  [Macros.Game.ObjType.GEM] = function(id)
    local config = CPPGameLib.GetConfig("Gem", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.icon) 
  end,

  [Macros.Game.ObjType.FAIRY] = function(id)
    local config = CPPGameLib.GetConfig("Card", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.icon) 
  end,
  
  [Macros.Game.ObjType.SPIRIT] = function(id)
    local config = CPPGameLib.GetConfig("Spirit", id)
    if not config then return "" end

    return Resource.PathTool.GetItemPath(config.icon_id) 
  end,

}

--对象数据解析函数表
LogicGUI.IconLayer.ParseFuncMap = 
{
  [Macros.Game.ObjType.ITEM] = function(obj_self)
    obj_self:__ParseItemData()
  end,
  [Macros.Game.ObjType.EQUIP] = function(obj_self)
    obj_self:__ParseEquipData()
  end,
  [Macros.Game.ObjType.HERO] = function(obj_self)
    obj_self:__ParseHeroData()
  end,
  [Macros.Game.ObjType.TREASURE] = function(obj_self)
    obj_self:__ParseTreasureData()
  end,
  [Macros.Game.ObjType.AWAKEN_TOOL] = function(obj_self)
    obj_self:__ParseAwakenToolData()
  end,
  [Macros.Game.ObjType.GEM] = function(obj_self)
    obj_self:__ParseGemData()
  end,
  [Macros.Game.ObjType.FAIRY] = function(obj_self)
    obj_self:__ParseDivinationData()
  end, 
}

function LogicGUI.IconLayer:__init(show_tips, touch_enabled,touch_swallow)
  touch_enabled = touch_enabled == nil and true or touch_enabled
  touch_swallow = touch_swallow and touch_swallow or false
  self.__show_tips = show_tips == nil and true or show_tips

  self:SetAnchorPoint(0.5, 0.5)
  self:SetTouchEnabled(touch_enabled)
  self:SetTouchSwallow(touch_swallow)
  self:SetTouchFun(nil, nil, function()
    if self.__touch_callback then
      self.__touch_callback(self.__data)
    else
      self:__ShowTips()
    end
  end)
end

function LogicGUI.IconLayer:__delete()
  if self.__appear_effect_id then
    GlobalEffectMgr:DelEffect(self.__appear_effect_id)
    self.__appear_effect_id = nil
  end
  if self.__appear_effect_timer then
    GlobalTimerQuest:CancelQuest(self.__appear_effect_timer)
    self.__appear_effect_timer = nil
  end
end

--[[
@ params：
    必须：
    id        [number]对象ID

    可选：
    type      [number]对象类型
    num       [number]对象数量
    obj_info  [table]对象数据(item_info/equip_info/hero_info/treasure_info)
--]]
function LogicGUI.IconLayer:SetData(data)
  --兼容老数据
  data.id = data.id or data.item_id  
  data.type = data.type or ObjIDTool.GetInfo(data.id)

  self.__data = data

  --品质框
  self:__ShowIconQuality()

  --阴影遮罩
  self:__ShowIconShade()

  --图标背景
  self:__ShowIconBG()

  --对象图标
  self:__ShowIcon()

  --对象特效
  self:__IconAptitudeEffect()

  --通用数据
  self:__ParseCommonData()

  --详细数据
  if LogicGUI.IconLayer.ParseFuncMap[data.type] then
      LogicGUI.IconLayer.ParseFuncMap[data.type](self)
  end
end

--适配父容器大小
function LogicGUI.IconLayer:adaptParentContentSize()
  local parent = self.__node:GetParent() 
  if not parent then
      return
  end

  local width, height = parent:GetContentWidth(), parent:GetContentHeight()
  local scaleFactor = math.max(width / self:GetWidth(), height / self:GetHeight())
  self:SetScaleFactor(scaleFactor)
end

--设置点击回调
function LogicGUI.IconLayer:SetTouchCallback(call)
  self.__touch_callback = call
end

--设置灰色
function LogicGUI.IconLayer:SetGray(isGray)
  if self.__sp_icon_bg then
    self.__sp_icon_bg:SetGray(isGray)
  end
  if self.__sp_icon_quality then
    self.__sp_icon_quality:SetGray(isGray)
  end
  if self.__sp_icon then
    self.__sp_icon:SetGray(isGray)
  end
end

--获取图标
function LogicGUI.IconLayer:GetIcon()
  return self.__sp_icon
end

--获取图标品质框
function LogicGUI.IconLayer:GetIconQuality()
  return self.__sp_icon_quality
end


--显示图标背景
function LogicGUI.IconLayer:ShowIconBG(var)
  if self.__sp_icon_bg then
    self.__sp_icon_bg:SetVisible(var)
  end
end

--显示图标
function LogicGUI.IconLayer:ShowIcon(var)
  if self.__sp_icon then
    self.__sp_icon:SetVisible(var)
  end
end

--显示品质框
function LogicGUI.IconLayer:ShowIconQuality(var)
  if self.__sp_icon_quality then
    self.__sp_icon_quality:SetVisible(var)
  end
end

--显示品质框
function LogicGUI.IconLayer:ShowIconShade(var)
  if self.__sp_icon_shade then
    self.__sp_icon_shade:SetVisible(var)
  end
end

--显示资质特效
function LogicGUI.IconLayer:ShowAptitude(var)
  if self.__aptitude_effect_id then
    GlobalEffectMgr:SetEffectVisible(self.__aptitude_effect_id , var)
  end
end

--播放出现特效
function LogicGUI.IconLayer:PlayAppearEffect()
  local function set_visible(var)
    self.__sp_icon_bg:SetVisible(var)
    self.__sp_icon:SetVisible(var)
    self.__sp_icon_quality:SetVisible(var)
    self.__panel_data:SetVisible(var)
  end

  set_visible(false)

  local effect
  self.__appear_effect_id, effect = GlobalEffectMgr:CreateUIEffect({id = 100035, loop = false}, self)
  PosTool.Center(effect)

  CPPActionManager.DelayTo(effect, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
    set_visible(true)
  end))
end





















--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--通用数据
function LogicGUI.IconLayer:SetNum(num,color,txt_size,txt_stroke)
  if self.__lab_obj_num then
    self.__lab_obj_num:SetString(num)  
    if color then
       self.__lab_obj_num:SetColor(unpack(color))
    end
    if txt_stroke then
       local stroke_width = 2
       print("ldx","txt_stroketxt_stroketxt_stroketxt_stroke: ",unpack(txt_stroke))
       self.__lab_obj_num:SetStrokeColor(unpack(txt_stroke))
       self.__lab_obj_num:SetStrokeWidth(stroke_width)
    end
    if txt_size then
       self.__lab_obj_num:SetFontSize(txt_size)
    end
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--物品数据
function LogicGUI.IconLayer:ShowItemName(var,width,height,scale)
  if self.__lab_item_name then
    self.__lab_item_name:SetVisible(var)
    self.__lab_item_name:SetScaleFactor(scale or 1)
    if width or height then
        self.__lab_item_name:SetTextWidthAndHeight(width or 0, height or 0)
      end
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--觉醒道具数据
function LogicGUI.IconLayer:ShowAwakenToolName(var,width,height)
  if self.__lab_awakentool_name then
    self.__lab_awakentool_name:SetVisible(var)
    if width or height then
        self.__lab_awakentool_name:SetTextWidthAndHeight(width or 0, height or 0)
      end
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--觉醒道具数据
function LogicGUI.IconLayer:ShowAwakenStar(awaken_star)
  self.__data.obj_info.awaken_star = awaken_star and awaken_star or self.__data.obj_info.awaken_star
  if self.__data.obj_info and self.__data.obj_info.quality_level >= Macros.Game.QualityType.ORANGE then
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "smallstar")
    local org_pos = -(self.__data.obj_info.awaken_star - 1) * 16/2
    local org_dis
    if self.__data.obj_info.awaken_star > 0 then
      org_dis = self.__data.obj_info.awaken_star == 1 and 0 or math.floor(math.abs(org_pos/(self.__data.obj_info.awaken_star-1))*2)
    end 
    for i = 1, self.__data.obj_info.awaken_star do
      if not self["__awaken_star"..i] then
        self["__awaken_star"..i]  = Util:Sprite(sp)       
        self:AddChild(self["__awaken_star"..i],Macros.Global.z_order_top)
      end
      self["__awaken_star"..i]:SetPosition((i-1)*org_dis+org_pos+self:GetNode():GetContentWidth()/2,12)
    end
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--装备数据
function LogicGUI.IconLayer:ShowEquipName(var, width, height)
  if self.__lab_equip_name then
    self.__lab_equip_name:SetVisible(var)

    if width or height then
      self.__lab_equip_name:SetTextWidthAndHeight(width or 0, height or 0)
    end
  end
end

function LogicGUI.IconLayer:ShowEquipStrengthenLev(var)
  if self.__lab_equip_strengthen_lev then
    self.__lab_equip_strengthen_lev:SetVisible(var)
  end
end

function LogicGUI.IconLayer:SetEquipStrengthenLev(var)
  self.__lab_equip_strengthen_lev:SetString(CPPGameLib.GetString("common_add4", var))
end

function LogicGUI.IconLayer:SetEquipStar( num )
    if self.__equip_star then
      self.__equip_star:RemoveSelf( true )
      self.__equip_star = nil
    end
    self.__equip_star = Util:Layout( 100 , 10 )
    self.__equip_star:SetAnchorPoint( 0.5 , 0 )
    self.__panel_data:AddChild( self.__equip_star )
    self.__equip_star:SetPosition( self.__panel_data:GetContentWidth()/2 , 0 )
    
    local type_ = ObjIDTool.GetInfo(self.__data.id)
    if type_ ~= Macros.Game.ObjType.EQUIP then
        return 
    end
    num = num or 1
    if self.__data.obj_info and self.__data.obj_info.uid then
      local info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", self.__data.obj_info.uid )
      num = info.star 
    end

    local scale = 0.8

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")
    local star = Util:Sprite(sp)
    self.__equip_star:SetContentSize( num * ( star:GetContentWidth() * scale ) , star:GetContentHeight() )

    for i = 1 , num do
        local star = Util:Sprite( sp )
        star:SetAnchorPoint( 0.5 , 0.5 )
        star:SetPosition( ( (star:GetContentWidth() * scale) /2) + ((i-1)*(star:GetContentWidth()*scale) ) , self.__equip_star:GetContentHeight()/2 )
        self.__equip_star:AddChild( star )
        star:SetScaleFactor(0.8)
    end
end



























--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--圣器数据
function LogicGUI.IconLayer:ShowTreasureName(var, width, height, offset_x, offset_y)
  if self.__lab_treasure_name then
    self.__lab_treasure_name:SetVisible(var)

    if width or height then
      self.__lab_treasure_name:SetTextWidthAndHeight(width or 0, height or 0)
    end    

    if offset_x or offset_y then
      PosTool.CenterBottom(self.__lab_treasure_name, offset_x or 0, offset_y or 0)      
    end
  end
end

function LogicGUI.IconLayer:ShowTreasureStrengthenLev(var)
  if self.__lab_treasure_strengthen_lev then
    self.__lab_treasure_strengthen_lev:SetVisible(var)
  end
end

function LogicGUI.IconLayer:SetTreasureStrengthenLev(var)
    self.__lab_treasure_strengthen_lev:SetString(var)
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--英雄数据
function LogicGUI.IconLayer:ShowHeroLev(var)
  if self.__lab_hero_lev then
    self.__lab_hero_lev:SetVisible(var)
  end
end

function LogicGUI.IconLayer:ShowHeroProfession(var)
  if self.__lab_hero_pname then
      self.__lab_hero_pname:SetVisible(var)
  end
end

function LogicGUI.IconLayer:ShowHeroName(var, width, height, offset_x, offset_y)
  if self.__lab_hero_name then
      self.__lab_hero_name:SetVisible(var)

      if width or height then
        self.__lab_hero_name:SetTextWidthAndHeight(width or 0, height or 0)
      end

      if offset_x or offset_y then
        PosTool.CenterBottom(self.__lab_hero_name, offset_x or 0, offset_y or 0)      
      end      
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--宝石数据
function LogicGUI.IconLayer:ShowGemName(var, width, height, offset_x, offset_y)
  if self.__lab_gem_name then
    self.__lab_gem_name:SetVisible(var)

    if width or height then
      self.__lab_gem_name:SetTextWidthAndHeight(width or 0, height or 0)
    end    

    if offset_x or offset_y then
      PosTool.CenterBottom(self.__lab_gem_name, offset_x or 0, offset_y or 0)      
    end
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--卡牌数据
function LogicGUI.IconLayer:ShowDivinationName(var, width, height)
  if self.__lab_card_name then
    self.__lab_card_name:SetVisible(var)

    if width or height then
      self.__lab_card_name:SetTextWidthAndHeight(width or 0, height or 0)
    end
  end
end

function LogicGUI.IconLayer:ShowDivinationStrengthenLev(var)
  if self.__lab_card_strengthen_lev then
    self.__lab_card_strengthen_lev:SetVisible(var)
  end
end

function LogicGUI.IconLayer:SetDivinationStrengthenLev(var)
    if self.__lab_card_strengthen_lev then
      self.__lab_card_strengthen_lev:SetString(CPPGameLib.GetString("common_add4", var))
    end   
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--精灵数据
function LogicGUI.IconLayer:ShowSpiritAwakenStar(starup_num)
  self.__data.obj_info.starup_num = starup_num and starup_num or self.__data.obj_info.starup_num
  if self.__data.obj_info then
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "smallstar")
    local org_pos = -(self.__data.obj_info.starup_num - 1) * 16/2
    local org_dis
    if self.__data.obj_info.starup_num > 0 then
      org_dis = self.__data.obj_info.starup_num == 1 and 0 or math.floor(math.abs(org_pos/(self.__data.obj_info.starup_num-1))*2)
    end 
    for i = 1, self.__data.obj_info.starup_num do
      if not self["__starup_star"..i] then
        self["__starup_star"..i]  = Util:Sprite(sp)       
        self:AddChild(self["__starup_star"..i],Macros.Global.z_order_top)
      end
      self["__starup_star"..i]:SetPosition((i-1)*org_dis+org_pos+self:GetNode():GetContentWidth()/2,12)
    end
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--对象图标背景
function LogicGUI.IconLayer:__ShowIconBG()
  local imgdata
  local quality = Macros.Game.QualityType.GREEN
  local _, config = ObjIDTool.GetInfo(self.__data.id)
  if config then
    --读取对象配置的品质
    quality = config.quality or config.init_quality
  end

  local obj_info = self.__data.obj_info
  if obj_info then
    --读取对象数据的品质
    quality = obj_info.quality_level or quality
  end
  quality = math.max(quality, Macros.Game.QualityType.WHITE)
  quality = math.min(quality, Macros.Game.QualityType.MAX)

  local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_"..quality)

  if not self.__sp_icon_bg then
    self.__sp_icon_bg = Util:Sprite(imgdata)
    self:AddChild(self.__sp_icon_bg, LogicGUI.IconLayer.InnerZOrder.ICON_BG)
  else
    self.__sp_icon_bg:SetImage(imgdata)
  end

  PosTool.Center(self.__sp_icon_bg)
end

--对象图标
function LogicGUI.IconLayer:__ShowIcon()
  local icon_path_func = LogicGUI.IconLayer.IconPathFuncMap[ self.__data.type ]
  if icon_path_func or self.__data.icon_path then
    local icon_path, icon_scale = self.__data.icon_path or icon_path_func(self.__data.id)
    icon_scale = icon_scale or 1
    if not self.__sp_icon then
      self.__sp_icon = Util:Sprite(icon_path)
      self.__sp_icon:SetScaleFactor(icon_scale)
      self:AddChild(self.__sp_icon, LogicGUI.IconLayer.InnerZOrder.ICON)

      local _, config = ObjIDTool.GetInfo(self.__data.id)
      if config and config.small_icon and config.small_icon == 1 then
          local piece_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "piece_icon")
          self.__small_icon = Util:Sprite(piece_icon)
          self.__small_icon:SetScaleFactor(icon_scale)
          self.__small_icon:SetAnchorPoint(0,1)
          self.__small_icon:SetPosition( 6.5 , self:GetNode():GetContentHeight() - 6.5 )
          self:AddChild(self.__small_icon, LogicGUI.IconLayer.InnerZOrder.ICON)
      end
      --是否是额外奖励
      if self.__data.is_extra then       
          local extra_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "extra_tag",true)
          self.__extra_icon = Util:Sprite(extra_icon)
          self.__extra_icon:SetScaleFactor(icon_scale)
          self.__extra_icon:SetAnchorPoint(0,1)          
          self:AddChild(self.__extra_icon, LogicGUI.IconLayer.InnerZOrder.PANEL_DATA)
          PosTool.LeftTop(self.__extra_icon)
      end
    else
      self.__sp_icon:SetImage(icon_path)
    end
    if self.__extra_icon then
      self.__extra_icon:SetVisible(self.__data.is_extra and true or false)
    end
    PosTool.Center(self.__sp_icon)
  end
end

--品质框
function LogicGUI.IconLayer:__ShowIconQuality()
  local quality = Macros.Game.QualityType.GREEN
  local _, config = ObjIDTool.GetInfo(self.__data.id)
  if config then
    --读取对象配置的品质
    quality = config.quality or config.init_quality
  end

  local obj_info = self.__data.obj_info
  if obj_info then
    --读取对象数据的品质
    quality = obj_info.quality_level or quality
  end
  quality = math.max(quality, Macros.Game.QualityType.WHITE)
  quality = math.min(quality, Macros.Game.QualityType.MAX)

  local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_"..quality)
  if not self.__sp_icon_quality then
    self.__sp_icon_quality = Util:Sprite(imgdata)
    self:AddChild(self.__sp_icon_quality, LogicGUI.IconLayer.InnerZOrder.ICON_QUALITY)
    PosTool.Center(self.__sp_icon_quality)
  else
    self.__sp_icon_quality:SetImage(imgdata)
  end

  self:__DoLayout()  
end

--阴影遮罩
function LogicGUI.IconLayer:__ShowIconShade()
  local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_shade")
  if not self.__sp_icon_shade then
    self.__sp_icon_shade = Util:Sprite(imgdata)
    self:AddChild(self.__sp_icon_shade, LogicGUI.IconLayer.InnerZOrder.ICON_QUALITY)
    PosTool.Center(self.__sp_icon_shade)
  else
    self.__sp_icon_shade:SetImage(imgdata)
  end
  self.__sp_icon_shade:SetVisible(false)
end

-- 在以下配置表中读取该装备碎片、装备、圣器碎片、圣器在各个界面显示的通用图标特效
-- 特效跟着图标走，图标出现在任何界面都会显示配置特效（除非没有配特效）
function LogicGUI.IconLayer:__IconAptitudeEffect()

  local type_ , config = ObjIDTool.GetInfo(self.__data.id)

  if self.__aptitude_effect_id then
    GlobalEffectMgr:DelEffect(self.__aptitude_effect_id)
    self.__aptitude_effect_id = nil
  end

  --物品
  if type_ == Macros.Game.ObjType.ITEM and config.equip_effect then

    local effect
    self.__aptitude_effect_id, effect = GlobalEffectMgr:CreateUIEffect( {res_name = config.equip_effect , loop = true}, self , 99 )
    PosTool.Center(effect,0,0)

  --装备
  elseif type_ == Macros.Game.ObjType.EQUIP and config.suit_effect then

    local effect
    self.__aptitude_effect_id, effect = GlobalEffectMgr:CreateUIEffect( {res_name = config.suit_effect , loop = true}, self , 99 )
    PosTool.Center(effect,0,0)

  --圣器
  elseif type_ == Macros.Game.ObjType.TREASURE and config.treasure then

    local effect
    self.__aptitude_effect_id, effect = GlobalEffectMgr:CreateUIEffect( {res_name = config.treasure , loop = true}, self , 99 )
    PosTool.Center(effect,0,0)

  --宝石
  elseif type_ == Macros.Game.ObjType.GEM and config.effects then

    local effect
    self.__aptitude_effect_id, effect = GlobalEffectMgr:CreateUIEffect( {id = config.effects , loop = true}, self , 99 )
    PosTool.Center(effect,0,0)

  end
  
end

--展示信息
function LogicGUI.IconLayer:__ShowTips()
  if not self.__show_tips then
    return
  end

  local obj_type = self.__data.type

  --物品Tips
  if obj_type == Macros.Game.ObjType.ITEM then
    local __ , config_item = ObjIDTool.GetInfo(self.__data.id)
    --装备碎片
    if config_item and config_item.type == Macros.Game.ObjType.ITEM and config_item.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS then
        TemplEquipInfo.GetInstance(self.__data)
    elseif config_item and config_item.type == Macros.Game.ObjType.ITEM and config_item.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
        CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK_DETAIL,self.__data.item_id or self.__data.id )
    elseif config_item and config_item.preview_gift then
        LogicGUI.TemplGiftInfo.New(self.__data)
    else
        LogicGUI.TemplItemInfo.New(self.__data)
    end

  --装备Tips
  elseif obj_type == Macros.Game.ObjType.EQUIP then
    if self.__data.obj_info then
        --背包中的装备
        local info = {}
        info.equip_uid = self.__data.obj_info.uid
        info.btn = false 
        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY, 1, info)
    else
        --非背包中的装备
        TemplEquipInfo.GetInstance(self.__data)
    end

  --英雄Tips
  elseif obj_type == Macros.Game.ObjType.HERO then
    CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_HANDBOOK_DETAIL, self.__data.id)

  --圣器Tips
  elseif obj_type == Macros.Game.ObjType.TREASURE then
    if self.__data.obj_info then
      local info = {}
      info.treasure_uid = self.__data.obj_info.uid
      info.btn = false 
      CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURE_LIFT, 1, info)
    else
      GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PLUNDERED_TREASURES_DETAILMESSAGE, false, self.__data.id)
    end
  elseif obj_type == Macros.Game.ObjType.AWAKEN_TOOL then
     LogicGUI.TemplItemInfo.New(self.__data)

  --宝石
  elseif obj_type == Macros.Game.ObjType.GEM then

      GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_GEM_INFO ,false , self.__data.id , "normal" )

  --卡牌
  elseif obj_type == Macros.Game.ObjType.FAIRY then
      if self.__data.obj_info then
          local obj_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination", self.__data.obj_info.uid )
          if obj_info.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP then
              GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_not_promote"))
          else
              CallModuleFunc(ModuleType.DIVINATION, "OpenView", BaseViewType.DIVINATION_INTENSIFY, 1 , self.__data.obj_info.uid )
          end
      else
          GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_CARD_INFO , false , self.__data )
      end
  end

  GlobalEventSystem:FireNextFrame(Macros.Event.DungeonModule.DUNGEON_CHALLANGE_CLOSE)
end

--布局
function LogicGUI.IconLayer:__DoLayout()
  --根据icon框设置根节点尺寸
  self:SetContentSize(self.__sp_icon_quality:GetContentWidth(), self.__sp_icon_quality:GetContentHeight())
  PosTool.Center(self.__sp_icon_quality)
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析通用数据
function LogicGUI.IconLayer:__ParseCommonData()
  --数据容器
  if not self.__panel_data then
    self.__panel_data = Util:Layout(0, 0)
    self.__panel_data:SetAnchorPoint(0.5, 0.5)
    self:AddChild(self.__panel_data, LogicGUI.IconLayer.InnerZOrder.PANEL_DATA)
  end
  self.__panel_data:SetContentSize(self.__sp_icon_bg:GetContentWidth(), self.__sp_icon_bg:GetContentHeight())
  PosTool.Center(self.__panel_data)
  
  local obj_type, config_obj = ObjIDTool.GetInfo(self.__data.id)

  --数量
  if self.__data.num or (self.__data.min_num and self.__data.max_num) then
    if not self.__lab_obj_num then
      self.__lab_obj_num = Util:Name(" ", Macros.TypefaceSize.minimum)
      self.__lab_obj_num:SetAnchorPoint(1, 0)
      self.__panel_data:AddChild(self.__lab_obj_num)
      PosTool.RightBottom(self.__lab_obj_num, -4, 0)
    end

    local num = self.__data.num
    local min_num = self.__data.min_num
    local max_num = self.__data.max_num

    if obj_type == Macros.Game.ObjType.ITEM and config_obj and config_obj.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
      if num then
        if tonumber(num) then
          num = num * config_obj.param2
        end
      else
        min_num = min_num * config_obj.param2
        max_num = max_num * config_obj.param2
        num = min_num .. "-" .. max_num
      end
    elseif not num then
      num = min_num .. "-" .. max_num
    end

    self.__lab_obj_num:SetString(num or "")

  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析物品数据
function LogicGUI.IconLayer:__ParseItemData()
  local config_item = CPPGameLib.GetConfig("Item", self.__data.id)
  if not config_item then
    return
  end

  --配置数据
  --物品名称
  if not self.__lab_item_name then
    self.__lab_item_name = Util:Label(nil, Macros.TypefaceSize.slightly , Macros.Color.keypoint, self:GetWidth())
    self.__lab_item_name:SetAnchorPoint(0.5, 1)
    self.__lab_item_name:SetVisible(false)
    self.__lab_item_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_item_name)
    PosTool.CenterBottom(self.__lab_item_name, 0, -8)      
  end
  self.__lab_item_name:SetString(config_item.name)
  self.__lab_item_name:SetColor(unpack(Macros.Quality[config_item.quality]))
  self.__lab_item_name:SetStrokeColor(unpack(Macros.Quality_Stroke[config_item.quality]))

  --详细数据
  local item_info = self.__data.obj_info 
  if item_info then
  end
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析觉醒道具数据
function LogicGUI.IconLayer:__ParseAwakenToolData()
  local config_item = CPPGameLib.GetConfig("AwakeningItem", self.__data.id)
  if not config_item then
    return
  end
  --配置数据
  --物品名称
  if not self.__lab_awakentool_name then
    self.__lab_awakentool_name = Util:Label(nil, Macros.TypefaceSize.slightly , Macros.Color.keypoint, self:GetWidth())
    self.__lab_awakentool_name:SetAnchorPoint(0.5, 1)
    self.__lab_awakentool_name:SetVisible(false)
    self.__lab_awakentool_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_awakentool_name)
    PosTool.CenterBottom(self.__lab_awakentool_name, 0, -8)      
  end
  self.__lab_awakentool_name:SetString(config_item.name)
  self.__lab_awakentool_name:SetColor(unpack(Macros.Quality[config_item.quality]))
  self.__lab_awakentool_name:SetStrokeColor(unpack(Macros.Quality_Stroke[config_item.quality]))
  self.__lab_awakentool_name:SetStrokeWidth(2)

end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析装备数据
function LogicGUI.IconLayer:__ParseEquipData()
  local config_equip = CPPGameLib.GetConfig("Equipment", self.__data.id)
  if not config_equip then
    return
  end
  --配置数据
  --装备名
  if not self.__lab_equip_name then
    self.__lab_equip_name = Util:Label(nil, Macros.TypefaceSize.slightly , Macros.Color.keypoint , self:GetWidth())
    self.__lab_equip_name:SetAnchorPoint(0.5, 1)
    self.__lab_equip_name:SetVisible(false)
    self.__lab_equip_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_equip_name)
    PosTool.CenterBottom(self.__lab_equip_name, 0, -8)      
  end
  self.__lab_equip_name:SetString(config_equip.name)
  self.__lab_equip_name:SetColor(unpack(Macros.Quality[config_equip.quality]))
  self.__lab_equip_name:SetStrokeColor(unpack(Macros.Quality_Stroke[config_equip.quality]))

  --详细数据
  local equip_info = self.__data.obj_info 
  if equip_info then
    --默认属性
    equip_info.refine_level = equip_info.refine_level or 0
    equip_info.strengthen_level = equip_info.strengthen_level or 0

    --装备名+精炼等级
    if equip_info.refine_level > 0 then
      self.__lab_equip_name:SetString(CPPGameLib.GetString("common_add6", config_equip.name, equip_info.refine_level))
    end

    --强化等级
    if not self.__lab_equip_strengthen_lev then
      self.__lab_equip_strengthen_lev = Util:Name(" ",Macros.TypefaceSize.minimum)
      self.__lab_equip_strengthen_lev:SetAnchorPoint(0, 0)
      self.__lab_equip_strengthen_lev:SetVisible(false)
      self.__panel_data:AddChild(self.__lab_equip_strengthen_lev)
      self.__lab_equip_strengthen_lev:SetPosition(4, 1)
    end
    local strengthen_lev_str = equip_info.strengthen_level > 0 and CPPGameLib.GetString("common_add4", equip_info.strengthen_level) or ""
    self.__lab_equip_strengthen_lev:SetString(strengthen_lev_str)
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析圣器数据
function LogicGUI.IconLayer:__ParseTreasureData()
  local config_treasure = CPPGameLib.GetConfig("Treasure", self.__data.id)
  if not config_treasure then
    return
  end
  --配置数据
  --圣器名
  if not self.__lab_treasure_name then
    self.__lab_treasure_name = Util:Label(nil, Macros.TypefaceSize.slightly , Macros.Color.keypoint , self:GetWidth())
    self.__lab_treasure_name:SetAnchorPoint(0.5, 1)
    self.__lab_treasure_name:SetVisible(false)
    self.__lab_treasure_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_treasure_name)
    PosTool.CenterBottom(self.__lab_treasure_name, 0, -8)      
  end
  self.__lab_treasure_name:SetString(config_treasure.name)
  self.__lab_treasure_name:SetColor(unpack(Macros.Quality[config_treasure.quality]))
  self.__lab_treasure_name:SetStrokeColor(unpack(Macros.Quality_Stroke[config_treasure.quality]))
  
  --详细数据
  local treasure_info = self.__data.obj_info 
  if treasure_info then
    --默认属性
    treasure_info.refine_level = treasure_info.refine_level or 0
    treasure_info.strengthen_level = treasure_info.strengthen_level or 0

    --圣器名+精炼等级
    if treasure_info.refine_level > 0 then
      self.__lab_treasure_name:SetString(CPPGameLib.GetString("common_add6", config_treasure.name, treasure_info.refine_level))
    else
      self.__lab_treasure_name:SetString(config_treasure.name)
    end  

    --强化等级
    if not self.__lab_treasure_strengthen_lev then
      self.__lab_treasure_strengthen_lev = Util:Name(" ",Macros.TypefaceSize.minimum)
      self.__lab_treasure_strengthen_lev:SetAnchorPoint(0, 0)
      self.__lab_treasure_strengthen_lev:SetVisible(false)
      self.__lab_treasure_strengthen_lev:SetPosition(4 , 1)
      self.__panel_data:AddChild(self.__lab_treasure_strengthen_lev)
    end
    local strengthen_lev_str = treasure_info.strengthen_level > 0 and tostring(treasure_info.strengthen_level) or ""
    self.__lab_treasure_strengthen_lev:SetString(CPPGameLib.GetString("common_add7" , strengthen_lev_str))
  end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析英雄数据
function LogicGUI.IconLayer:__ParseHeroData()
  local config_hero = CPPGameLib.GetConfig("Hero", self.__data.id)
  if not config_hero then
    return
  end

  --配置数据
  --职业
  if not self.__lab_hero_pname then
    self.__lab_hero_pname = Util:Label(nil, Macros.TypefaceSize.minimum, Macros.Color.keypoint)
    self.__lab_hero_pname:SetAnchorPoint(0.5, 1)
    self.__lab_hero_pname:SetVisible(false)
    self.__lab_hero_pname:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_hero_pname)
    PosTool.CenterBottom(self.__lab_hero_pname, 0, -8)
  end
  if self.__lab_hero_pname then
    self.__lab_hero_pname:SetString(Macros.Game.HeroProfessionNameMap[config_hero.profession])  
  end

  local quality_level = config_hero.init_quality

  --详细数据
  local hero_info = self.__data.obj_info 

  --名称
  if not self.__lab_hero_name then
    self.__lab_hero_name = Util:Label(nil, Macros.TypefaceSize.slightly , Macros.Color.keypoint, self:GetWidth())
    self.__lab_hero_name:SetVisible(false)
    self.__lab_hero_name:SetAnchorPoint(0.5, 1)
    self.__lab_hero_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_hero_name)
    PosTool.CenterBottom(self.__lab_hero_name, 0, -8) 
  end

  if hero_info then
      if hero_info.break_level and hero_info.break_level > 0 then
        self.__lab_hero_name:SetString(CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level))
      else
        self.__lab_hero_name:SetString(config_hero.name)
      end
  else
      self.__lab_hero_name:SetString(config_hero.name)
  end
    
  if hero_info then
    quality_level = hero_info.quality_level or hero_info.init_quality

    --等级
    if not self.__lab_hero_lev then
      self.__lab_hero_lev = Util:Name(" ",Macros.TypefaceSize.minimum)
      self.__lab_hero_lev:SetAnchorPoint(0, 0)
      self.__lab_hero_lev:SetPosition(4, 1)
      self.__lab_hero_lev:SetVisible(false)
      self.__panel_data:AddChild(self.__lab_hero_lev)
    end
    self.__lab_hero_lev:SetString(tostring(hero_info.level))
  end
  self.__lab_hero_name:SetColor(unpack(Macros.Quality[quality_level]))
  self.__lab_hero_name:SetStrokeColor(unpack(Macros.Quality_Stroke[quality_level]))  
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析宝石数据
function LogicGUI.IconLayer:__ParseGemData()
  local config_gem = CPPGameLib.GetConfig("Gem", self.__data.id)
  if not config_gem then
    return
  end
  --配置数据
  --宝石名
  if not self.__lab_gem_name then
      self.__lab_gem_name = Util:Label(nil, Macros.TypefaceSize.slightly, Macros.Color.keypoint , self:GetWidth())
      self.__lab_gem_name:SetAnchorPoint(0.5, 1)
      self.__lab_gem_name:SetVisible(false)
      self.__lab_gem_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
      self.__panel_data:AddChild(self.__lab_gem_name)
      PosTool.CenterBottom(self.__lab_gem_name, 0, -8)      
  end
  self.__lab_gem_name:SetString(config_gem.name)
  self.__lab_gem_name:SetColor(unpack(Macros.Quality[config_gem.quality]))
  self.__lab_gem_name:SetStrokeColor(unpack(Macros.Quality_Stroke[config_gem.quality]))

  --详细数据
  local gem_info = self.__data.obj_info 
  if gem_info then
    --默认属性
    
  end

end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--解析卡牌数据
function LogicGUI.IconLayer:__ParseDivinationData()
  local config_card = CPPGameLib.GetConfig("Card", self.__data.id)
  if not config_card then
    return
  end
  --配置数据
  --圣器名
  if not self.__lab_card_name then
    self.__lab_card_name = Util:Label(nil, Macros.TypefaceSize.slightly, Macros.Color.keypoint , self:GetWidth())
    self.__lab_card_name:SetAnchorPoint(0.5, 1)
    self.__lab_card_name:SetVisible(false)
    self.__lab_card_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.__panel_data:AddChild(self.__lab_card_name)
    PosTool.CenterBottom(self.__lab_card_name, 0, -8)      
  end
  self.__lab_card_name:SetString(config_card.name)
  self.__lab_card_name:SetColor(unpack(Macros.Quality[config_card.quality]))
  self.__lab_card_name:SetStrokeColor(unpack(Macros.Quality_Stroke[config_card.quality]))
  
  --详细数据
  local card_info = self.__data.obj_info 
  if card_info then
    --默认属性
    card_info.break_level = card_info.break_level or 0
    card_info.level = card_info.level or 0

    --圣器名+进阶等级
    if card_info.break_level > 0 then
      self.__lab_card_name:SetString(CPPGameLib.GetString("common_add6", config_card.name, card_info.break_level))
    else
      self.__lab_card_name:SetString(config_card.name)
    end  

    --强化等级
    if not self.__lab_card_strengthen_lev then
      self.__lab_card_strengthen_lev = Util:Name(" ",Macros.TypefaceSize.minimum)
      self.__lab_card_strengthen_lev:SetAnchorPoint(0, 0)
      self.__lab_card_strengthen_lev:SetVisible(false)
      self.__lab_card_strengthen_lev:SetPosition(4 , 1)
      self.__panel_data:AddChild(self.__lab_card_strengthen_lev)
    end
    local strengthen_lev_str = card_info.level > 0 and tostring(card_info.level) or "0"
    self.__lab_card_strengthen_lev:SetString(CPPGameLib.GetString("common_add7" , strengthen_lev_str))
  end

end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--[[
%% @module: 角色图标
--]]

LogicGUI = LogicGUI or {}

LogicGUI.RoleIcon = LogicGUI.RoleIcon or BaseClass(UILayer)

function LogicGUI.RoleIcon:__init(touch_enabled)
  touch_enabled = touch_enabled == nil and true or touch_enabled  
  
  self:SetAnchorPoint(0.5, 0.5)
  self:SetTouchEnabled(touch_enabled)
  self:SetTouchFun(nil, nil, function()
    if self.__touch_callback then
      self.__touch_callback(self.__data)
    end
  end)
end

function LogicGUI.RoleIcon:__delete()
end

function LogicGUI.RoleIcon:SetData(res_id)
  --背景框
  if not self.__sp_icon_bg then
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_role_head")
    self.__sp_icon_bg = Util:Sprite(imgdata)
    self:AddChild(self.__sp_icon_bg)
  end
  self:__DoLayout()

  --头像  
  local path = Resource.PathTool.GetRoleHeadPath(res_id)
  if not self.__sp_role_icon then
    self.__sp_role_icon = Util:Sprite(path)  
    self:AddChild(self.__sp_role_icon)
    PosTool.Center(self.__sp_role_icon)
  else
    self.__sp_role_icon:SetImage( path )
  end

  local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_role_border")
  if not self.__sp_role_frame then
    self.__sp_role_frame = Util:Sprite(imgdata)  
    self:AddChild(self.__sp_role_frame)
    PosTool.Center(self.__sp_role_frame)
  end
  
end

function LogicGUI.RoleIcon:SetTouchCallback(call)
  self.__touch_callback = call
end

function LogicGUI.RoleIcon:__DoLayout()
  --根据icon框设置根节点尺寸
  self:SetContentSize(self.__sp_icon_bg:GetContentWidth(), self.__sp_icon_bg:GetContentHeight())
  PosTool.Center(self.__sp_icon_bg)
end

--适配父容器大小
function LogicGUI.RoleIcon:adaptParentContentSize()
  local parent = self.__node:GetParent() 
  if not parent then
      return
  end
  local width, height = parent:GetContentWidth(), parent:GetContentHeight()
  local scaleFactor = math.max(width / self.__sp_role_frame:GetContentWidth(), height / self.__sp_role_frame:GetContentHeight())
  self.__sp_role_frame:SetScaleFactor(scaleFactor)
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--[[
%% @module: 怪物图标
--]]
