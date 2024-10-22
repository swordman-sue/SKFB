-- %% @module: 回收界面
-- %% @author: yjg
-- %% @created: 2016年9月13日16:39:26

Modules = Modules or {} 


Modules.RecycleView = Modules.RecycleView or BaseClass(BaseView)

function Modules.RecycleView:__init()
	self.__layout_name = "recovery"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_recovery")}
	self.__title_bar_params = 
	{
        title = CPPGameLib.GetString("recycle"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
            CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
            CallModuleFunc(ModuleType.RECYCLE , "SetOnTab" , 1 )
			
            self:CloseManual()
		end,
	}

    self.__tab_params_ex = {6 , true}

    local t3 = nil
    t3 , self.textLst = CallModuleFunc(ModuleType.RECYCLE , "CountTab" )

    self.__tab_view_params = t3

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.RECOVERY)
end

function Modules.RecycleView:__OpenCallback()
	self:__Getwidget()
	self:__ShowUI()
    self:__Server() 
end	

function Modules.RecycleView:__Server()    
    --红点更新事件
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE , function ( _, system_id, sub_system_id )
        if system_id ~= Macros.Game.SystemID.RECYCLE then
            return
        end

        if sub_system_id == Macros.Game.SystemID.EQUIP then
            self:SetEquipRed()
        elseif sub_system_id == Macros.Game.SystemID.HERO_BAG then
            self:SetHeroRed()
        end
    end)
end

function Modules.RecycleView:__Dispose()
end

function Modules.RecycleView:__Getwidget( ... )
	--背景 
    self.Panel_layer = self:GetWidget("Panel_layer") 
    --控制面板
    self.Panel_panel = self:GetWidget("Panel_panel") 
    --购买背景
    self.Image_shopBg = self:GetWidget("Image_shopBg") 
    --规则
    self.Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
       print("xy","(1)回收，小图标（用于介绍回收和重生功能，5个标签页共用）")
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Recycle")
        end )
    end )
    local union_rule = Util:Name(CPPGameLib.GetString("union_rule"))
    union_rule:SetAnchorPoint( 0.5,0.5 )
    union_rule:SetPosition( self.Button_rule:GetContentWidth()/2 , 5 )
    self.Button_rule:AddChild( union_rule )  


    --页签框
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")

end

function Modules.RecycleView:__ShowUI( ... )
    self.__tab:SetNoOn( true )

    local lstW = #self.textLst * 144
    self.__tab:SetContentSize( lstW , self.Panel_Backpackcbx:GetContentHeight() + 15 )
    self.__tab:SetData( self.textLst )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition( (self.Panel_Backpackcbx:GetContentWidth() / 2 ) - 0 , self.Panel_Backpackcbx:GetContentHeight()/2 + 5 )
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    self.__tab:SetTouchEnabled(false)
    
    if #self.textLst >= 5 then
        self.__tab:SetTouchEnabled(true)
    end

    local num = CallModuleFunc(ModuleType.RECYCLE , "GetOnTab")

    self.__tab:On( num or 1 )

    self:ShowOnData()

    self:SetEquipRed()

    self:SetHeroRed()
end

function Modules.RecycleView:ShowOnData( info_ )
    local page = self:GetActivatedTabView()
    if page then
        page:ShowOnData(info_)
    end
end

function Modules.RecycleView:OnButtonShop( ... )

end

function Modules.RecycleView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )
    layer:SetAnchorPoint( 0.5 , 0.5 )

    local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.white , nil , nil , Macros.Color.button_yellow )
    name:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild(name)
    PosTool.Center( name )

    return layer
end

--选中
function Modules.RecycleView:OnCallBack( index , data )

    if self.OnItemEff then
        self.OnItemEff:RemoveSelf(true)
        self.OnItemEff = nil
    end

    local item = self.__tab:GetBtn( index )

    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    self.OnItemEff = Util:Sprite( com_sp_cbox_wire )
    self.OnItemEff:SetAnchorPoint( 0.5 , 1 )
    self.OnItemEff:SetPosition( item:GetContentWidth()/2 , 3 )
    item:AddChild( self.OnItemEff )

    if index ~= unpack(self.__open_data) then
        CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
    end

    CallModuleFunc(ModuleType.RECYCLE , "SetOnTab" , index )

    self:__TabChange( index )
end

--设置装备红点
function Modules.RecycleView:SetEquipRed()

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local cnf = CPPGameLib.GetConfig("RecycleOpenLevel", 2 )

    if cnf.lv > role_level then
        return 
    end

    local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.RECYCLE, Macros.Game.SystemID.EQUIP)
    if red_num and red_num ~= 0 then
        local item = self.__tab:GetBtn(2)
        if not self.equipRed then
            local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
            self.equipRed = Util:Sprite( icon_red_point )
            self.equipRed:SetAnchorPoint( 0.5 , 0.5 )
            self.equipRed:SetPosition( item:GetContentWidth() - 5 , item:GetContentHeight() - 5 )
            item:AddChild( self.equipRed )
        else
            self.equipRed:SetVisible(true)
        end
    else
        if self.equipRed then
            self.equipRed:SetVisible(false)
        end
    end
end

--设置装备红点
function Modules.RecycleView:SetHeroRed()

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local cnf = CPPGameLib.GetConfig("RecycleOpenLevel", 1 )

    if cnf.lv > role_level then
        return 
    end

    local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.RECYCLE, Macros.Game.SystemID.HERO_BAG)
    if red_num and red_num ~= 0 then
        local item = self.__tab:GetBtn(1)
        if not self.heroRed then
            local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
            self.heroRed = Util:Sprite( icon_red_point )
            self.heroRed:SetAnchorPoint( 0.5 , 0.5 )
            self.heroRed:SetPosition( item:GetContentWidth() - 5 , item:GetContentHeight() - 5 )
            item:AddChild( self.heroRed )
        else
            self.heroRed:SetVisible(true)
        end
    else
        if self.heroRed then
            self.heroRed:SetVisible(false)
        end
    end
end

