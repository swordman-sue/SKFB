
--
-- @brief 圣器提升界面
-- @author: yjg
-- @date: 2016年8月19日14:31:01
--

Modules = Modules or {}

Modules.TreasureLiftView = Modules.TreasureLiftView or BaseClass(BaseView)

function Modules.TreasureLiftView:__init()
    self.__layout_name = "treasure_lift"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("info_treasure"),
        show_power = true,
        show_diamond = true,
        show_gold = true,
        show_level = true,
        close_func = function()
            CallModuleFunc(ModuleType.TREASURE, "DeleteAllSelectedStrengthenMaterials" )
            self:CloseManual()
        end,
    }

    --功能开启等级  
    self:__CountLockLevel()
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab_params = {
        {sp_2,sp_1},
        {
          CPPGameLib.GetString("info_treasure"),
          CPPGameLib.GetString("info_intensify"),
          CPPGameLib.GetString("equip_refine"),
        },10,CPPGameLib.Handler(self, self.OnBox),false,nil,self.__prepare_level_list,self.__lock_level_list
    }

    self.__tab_view_params = 
    {
        {Modules.TreasureLiftInfoTabView, "Panel_treasureInfo"},
        {Modules.TreasureLiftIntensifyTabView, "Panel_treasureIntensify"},
        {Modules.TreasureLiftRefineView, "Panel_treasureRefine"},
    }

    self.__loaded_callback = function()
        self:__LoadedCallback()
    end

    self.__open_callback = function()
        self:__OpenCallback()
    end  

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TREASURE,true)
end

function Modules.TreasureLiftView:__LoadedCallback()
    --背景
    self.Panel_interlayer = self:GetWidget("Panel_interlayer")
    --图标列表
    self.Image_lstBg = self:GetWidget("Image_lstBg")
    self.Image_lstBg:SetVisible(false)
    --内容图标
    self.Image_contentBg = self:GetWidget("Image_contentBg")
    --页签容器
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")

    --选项卡红点表
    self.__tab_reddot_map = {
        [Macros.Game.SystemID.TREASURE_INTENSIFY] = {key = "can_intensify", tab_index = 2}, 
        [Macros.Game.SystemID.TREASURE_REFINE] = {key = "can_refine", tab_index = 3}
    }
end

function Modules.TreasureLiftView:__OpenCallback()
    --设置选中的圣器
    CallModuleFunc(ModuleType.TREASURE, "SetOnTreasureUid" , self.__open_data[2].treasure_uid )

   --圣器进化
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TREASURE, "btn_remould",true)
    self.btn_remould = Util:Button( sp ,  function ( ... )
        CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASUREREMOULD)
    end )
    self.btn_remould:SetVisible(false)
    self.Panel_interlayer:AddChild(self.btn_remould) 
    self.btn_remould:SetPosition( self.Panel_interlayer:GetContentWidth() - 70 , self.Panel_interlayer:GetContentHeight() - 87 )
    self.btn_remould:SetScaleFactor( 0.8 )

    --页签
    self.__tab:On(self.__open_data[1] or 1)
    self.__tab:SetPosition(0 , 5)
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    local image_packbg = self:GetWidget("Image_packBg")
    image_packbg:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_equip_strengthen"), TextureResType.LOCAL)
    image_packbg = self:GetWidget("Image_packBg_0")
    image_packbg:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_equip_strengthen"), TextureResType.LOCAL)    

    if self.__open_data[2].hero_id then
        --显示圣器列表
        local params =  
        {
            item_class = Modules.EquipOrTreasureLiftItem,
            item_width = self.Image_lstBg:GetContentWidth(),
            item_height = 110,
            row = 4,
            col = 1,
            item_space_r = 5 ,
            view_width = self.Image_lstBg:GetContentWidth(),
            view_height = self.Image_lstBg:GetContentHeight() - 10 ,
            selectable = true,
            select_callback = function ( index , item )
                CallModuleFunc(ModuleType.TREASURE, "SetOnTreasureUid",item.uid)
                self:__HandleSelectTreasure()
            end,
            structre_params = {Macros.Game.ObjType.TREASURE}
        }
        self.lst_list = GUI.ScrollView.New(params)
        self.Image_lstBg:AddChild(self.lst_list:GetNode())
        self.lst_list:SetPosition(self.Image_lstBg:GetContentWidth()/2 , self.Image_lstBg:GetContentHeight()/2 - 15 )

        self.Image_lstBg:SetVisible(true)
        self.lst_list:SetDataList( CallModuleFunc(ModuleType.TREASURE, "GetTreasureSortByHeroID" , self.__open_data[2].hero_id))
    else
        --隐藏圣器列表
        self.Image_lstBg:SetVisible(false)
        self.Image_contentBg:SetPosition( self.Panel_interlayer:GetContentWidth()/2 , self.Image_contentBg:GetPositionY() )
    end

    self:__HandleSelectTreasure()
end

function Modules.TreasureLiftView:__RegistAllEvents()
    --强化
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_INTENSIFY,function (__,data)
        self:__HandleUpdate("intensify", data )
    end)

    --精炼圣器
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_REFINE,function (__,data)
        self:__HandleUpdate("refine", data )
    end)

    --删除圣器
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_REMOVE,function (__,data)
        self:__HandleUpdate("remove", data )
    end)   
end

function Modules.TreasureLiftView:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.TreasureLiftView:__CountLockLevel()
    self.__lock_level_list = {1}
    self.__prepare_level_list = {1}
    local module_type_list = {Macros.Game.SystemID.TREASURE_INTENSIFY,Macros.Game.SystemID.TREASURE_REFINE}
    for i,module_id in ipairs(module_type_list) do
      local config_system = CPPGameLib.GetConfig("SystemEntry",module_id)
      table.insert(self.__lock_level_list,config_system.level)
      table.insert(self.__prepare_level_list,config_system.prepare_level)
    end
end

function Modules.TreasureLiftView:OnBox( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")

    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -30 )

    self:__TabChange(index)
end

--事件用 刷新界面icon状态 
function Modules.TreasureLiftView:__HandleUpdate( type , data )
    if not self.lst_list then
        return
    end

    local del_index
    self.lst_list:ItemIter(function(i, item, item_data)
        --刷新红点
        item:UpdateIconRedDot()

        --刷新图标/记录待移除条目索引        
        if item_data.uid == data.uid then
            if type == "intensify" then                --强化
                item.icon:SetTreasureStrengthenLev("+" .. data.new_level )

            elseif type == "refine" then               --精炼
                item.name:SetString(item_data.name.." +"..data.refine_level)

            elseif type == "remove" then               --删除
                del_index = i
            end

            --更新选项卡红点
            self:__UpdateTabRedDot(item_data)                        
        end
    end)
    if del_index then
        self.lst_list:DetachItem(del_index)
        self.lst_list:RefreshLayout()
    end

    -- --触摸屏蔽
    -- if type == "intensify" or type == "refine" then
    --     local layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height, function ( ... )
    --         -- body
    --     end)
    --     layer:SetAnchorPoint(0,0)
    --     layer:SetPosition(0,0)
    --     self:AddChild(layer)

    --     CPPActionManager.DelayTo(layer, 2.25 , GlobalCallbackMgr:AddCallBackFunc(function()
    --         layer:RemoveSelf(true)
    --         layer = nil
    --     end))
    -- end
end

--选择某个圣器
function Modules.TreasureLiftView:__HandleSelectTreasure()
    local onTreasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")

    --刷新圣器进化按钮        
    local evolve_info = CallModuleFunc(ModuleType.TREASURE, "ToUidGetEvolveInfo", onTreasureInfo.uid)
    local config_system = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.TREASUREREMOULD)
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)   
    if evolve_info.nexInfo and role_level >= config_system.level then
        self.btn_remould:SetVisible(true)        
    elseif evolve_info.nexInfo and role_level >= config_system.prepare_level then
        self.btn_remould:SetVisible(true)
        Util:GreyButton(self.btn_remould)
    else
        self.btn_remould:SetVisible(false)
    end

    --刷新选中标记
    self.lst_list:ItemIter(function(i, item, item_data)
        if item_data.uid == onTreasureInfo.uid then
            self.lst_list:LocateItem(i - 1)

            --选中标记
            if self.selected then
                self.selected:RemoveSelf(true)
            end  
            self.selected = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected"))
            self.selected:SetPosition(item.icon:GetNode():GetContentWidth()/2 , item.icon:GetNode():GetContentHeight ()/2 )
            item.icon:AddChild(self.selected , 10)

            --更新选项卡红点
            self:__UpdateTabRedDot(item_data)

            --若选中经验圣器，则屏蔽相关选项卡
            local config_treasure = CPPGameLib.GetConfig("Treasure", onTreasureInfo.info_id)
            if config_treasure.type == Macros.Game.TreasureType.EXP then
                self.__tab:Forbidden({[1] = 2, [2] = 3})
                self.__tab:On(1)
            end

            return true
        end            
    end)
    
    CallModuleFunc(ModuleType.TREASURE, "DeleteAllSelectedStrengthenMaterials")

    --根据选中的装备，刷新右侧页面信息
    local page = self:GetActivatedTabView()
    if page then
        page:__SetShowUI()
    end
end

--更新选项卡红点
function Modules.TreasureLiftView:__UpdateTabRedDot(item_data)
    local is_reddot_valid, tab_btn
    for reddot_id, reddot_info in pairs(self.__tab_reddot_map) do
        is_reddot_valid = item_data[reddot_info.key] == "true"
        if not reddot_info.sprite_reddot and is_reddot_valid then
            tab_btn = self.__tab.btn_list[reddot_info.tab_index]
            if tab_btn then
                reddot_info.sprite_reddot = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point"))
                reddot_info.sprite_reddot:SetPosition(tab_btn:GetContentWidth() - 10, tab_btn:GetContentHeight() - 10)
                tab_btn:AddChild(reddot_info.sprite_reddot , 20)
            end
        end
        if reddot_info.sprite_reddot then
            reddot_info.sprite_reddot:SetVisible(is_reddot_valid)
        end
    end
end