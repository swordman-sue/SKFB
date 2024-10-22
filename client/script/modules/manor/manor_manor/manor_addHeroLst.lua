
--[[
%% @module: 领地巡逻英雄选择
%% @author: yjg
%% @created: 2016年10月9日15:25:12
--]]

Modules = Modules or {}
Modules.ManorAddHeroLst = Modules.ManorAddHeroLst or BaseClass(BaseView)

function Modules.ManorAddHeroLst:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("hero_lineup_select_title"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            -- self:CloseManual()
            CallModuleFunc(ModuleType.MANOR, "OpenView", BaseViewType.MANOR , unpack(self.__open_data) )
        end,
    }
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_CROSS)
end

function Modules.ManorAddHeroLst:__delete()
end

function Modules.ManorAddHeroLst:__OpenCallback()    
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.ManorAddHeroLst:__Getwidget( ... )
    --内容框
    self.Image_content = self:GetWidget("Image_content") 

    --穿戴容器
    self.Panel_changePack = self:GetWidget("Panel_changePack") 
    self.Panel_changePack:SetVisible(true)
    --穿戴组件
    self.Text_changeTips = self:GetWidget("Text_changeTips") 
    --穿戴组件
    self.Panel_cBox = self:GetWidget("Panel_cBox") 

    --列表容器
    self.Panel_Backpack = self:GetWidget("Panel_Backpack")
    self.Panel_Backpack:SetVisible(false)
    --出售按钮
    self.Button_sell = Util:WidgetButton( self:GetWidget("Button_sell") , CPPGameLib.GetString("recycle_sell") , CPPGameLib.Handler(self,self.OnSell) )
    --数量上限
    self.Button_showMax = self:GetWidget("Button_showMax") 

    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")

    self.Panel_9 = self:GetWidget("Panel_9")
end

function Modules.ManorAddHeroLst:__ShowUI( ... )
    local params =  
    {
        item_class = Modules.TempManorAddHeroLstItem,
        item_width = 460.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        item_space_c = 5,
        view_height = 482.50 ,
        touchable = true,
        touch_callback = function(item_index, item_data)

            local info = {}
            info.uid = item_data.uid
            info.info_id = item_data.info_id 

            CallModuleFunc(ModuleType.MANOR, "OpenView", BaseViewType.MANOR , unpack(self.__open_data) , info )
        end,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    self:Setlist()
end

function Modules.ManorAddHeroLst:Setlist( ... )
    local list = CallModuleFunc(ModuleType.MANOR, "FiltrationHero")
    self.lst_list:SetDataList(list)
end

Modules = Modules or {}
Modules.TempManorAddHeroLstItem = Modules.TempManorAddHeroLstItem or BaseClass(GUI.ScrollItem)

function Modules.TempManorAddHeroLstItem:__init(data,fun)
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_CROSS)  
    self:__Getwidget()
end

function Modules.TempManorAddHeroLstItem:__delete()
end

function Modules.TempManorAddHeroLstItem:__AddToParent()
end

function Modules.TempManorAddHeroLstItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self.data = data
    self:__ShowUI(data)
end

function Modules.TempManorAddHeroLstItem:__Getwidget( ... )
    --背景
    self.Image_sellItem = self:GetWidget("Image_sellItem")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    --名字
    self.Text_name = self:GetWidget("Text_name")
    --属性1
    -- self.Text_property1 = self:GetWidget("Text_property1")
    --内容
    self.Image_1 = self:GetWidget("Image_1")
    --属性3
    -- self.Text_property3 = self:GetWidget("Text_property3")
    --更换按钮
    self.Button_change =  Util:WidgetButton(   
                                                self:GetWidget("Button_change") , 
                                                CPPGameLib.GetString("hero_lineup_select") , 
                                                CPPGameLib.Handler(self,self.OnButtonChange) ,
                                                nil,nil,Macros.Color.button_yellow
                                            )
    self.Button_change:SetPosition( self.Button_change:GetPositionX() , 61.00 )
    --英雄名字
    self.Text_heroName = self:GetWidget("Text_heroName")
end

function Modules.TempManorAddHeroLstItem:__ShowUI( data )
    local type_ , info_ = ObjIDTool.GetInfo(data.info_id)
    
    local heroType = {
            [1] = "hero_pro_name_physical_near",
            [2] = "hero_pro_name_defence_near",
            [3] = "hero_pro_name_physical_remote",
            [4] = "hero_pro_name_magic_remote",
            [5] = "hero_pro_name_assist",
    }

    --名字
    local name = Util:Name( "["..CPPGameLib.GetString(heroType[info_.profession]).. "]" .. info_.name , Macros.TypefaceSize.normal , data.quality_level)
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2)
    self.Text_name:AddChild(name)

    --图标
    local icon_layer = LogicGUI.IconLayer.New(true)
    icon_layer:SetData({id = data.info_id})
    icon_layer:ShowEquipStrengthenLev(true)
    self.Panel_icon:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    --英雄名称
    if data.is_to_battle ~= 0 then
        local heroName = Util:Label(CPPGameLib.GetString("manor_formation") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
        heroName:SetAnchorPoint(1,0.5)
        heroName:SetPosition(self.Text_heroName:GetContentWidth() , self.Text_name:GetContentHeight() / 2 )
        self.Text_heroName:AddChild(heroName)
    end


    local num , max = CallModuleFunc(ModuleType.Pack, "ToHeroIdGetDebris" , data.info_id)
    local manor_possessNum = Util:LabCenter({
                                    {zi = CPPGameLib.GetString("manor_possessNum") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly},
                                    {zi = num .. "/" .. max , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly},
                                })
    manor_possessNum:SetAnchorPoint(0,0.5)
    manor_possessNum:SetPosition( 10 , self.Image_1:GetContentHeight() / 2 )
    self.Image_1:AddChild(manor_possessNum)
end

function Modules.TempManorAddHeroLstItem:GetTouchNode()
    return self.Button_change
end