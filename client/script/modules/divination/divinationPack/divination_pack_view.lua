--
-- @brief 卡牌背包 
-- @author: yjg
-- @date: 2016年8月9日17:23:47
--

Modules = Modules or {}

Modules.DivinationPackView = Modules.DivinationPackView or BaseClass(BaseView)

function Modules.DivinationPackView:__init()
  	self.__layout_name = "pack"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination_Cardpack"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
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
            CPPGameLib.GetString("divination_Card"),
        },
        10,
        CPPGameLib.Handler(self, self.OnYeMei),
        false,
        nil,nil,self.__lock_level_list
    }

    self.__open_callback = function()
    	 self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.PACK_ITEM)

  -- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BEIBAO)
end

function Modules.DivinationPackView:__OpenCallback()
    self:__Getwidget()
    self:__ShowUI() 
    -- self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))  
end

--开放等级
function Modules.DivinationPackView:__CountLockLevel()
    self.__lock_level_list = {}
    local module_type_list = {Macros.Game.SystemID.Pack,Macros.Game.SystemID.HERO_AWAKEN}
    for i,module_id in ipairs(module_type_list) do
        local module_info = CPPGameLib.GetConfig("SystemEntry",module_id)
        table.insert(self.__lock_level_list,module_info.level)
    end
end


function Modules.DivinationPackView:__Dispose()   
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
    if self.__awaken_Equip_ScrollView then
        self.__awaken_Equip_ScrollView:DeleteMe()
        self.__awaken_Equip_ScrollView = nil
    end
end

function Modules.DivinationPackView:__Getwidget()
    --下
    -- self.Image_1 = self:GetWidget("Image_1")
    --内容
    self.Image_content = self:GetWidget("Image_content")
    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")
    --出售按钮
    self.Button_handbook = self:GetWidget("Button_handbook")
    self.Button_handbook:SetVisible(false)
end

function Modules.DivinationPackView:__ShowUI()
    local params =  
    {
        item_class = Modules.TemplDivinationPackItem,
        item_width = 460.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        item_space_c = 5,
        view_height = 482.50 ,
        selectable = true,
    }
    -- 创建参数
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.__zones_scrollview:SetDataList( CallModuleFunc(ModuleType.DIVINATION, "PackData") )
    self.Image_content:AddChild(self.__zones_scrollview:GetNode())
    PosTool.Center(self.__zones_scrollview:GetNode())
    
    --页签
    self.__tab:On(1)
    self.__tab:SetPosition(0 , 10)
    self.__tab:SetCboxColor( Macros.Color.button_yellow , Macros.Color.button_red )
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_bag_num" )
    local bag_num = Util:Sprite( sp )
    bag_num:SetAnchorPoint( 0.5 , 0.5 )
    bag_num:SetPosition( self.Image_content:GetContentWidth()/2 , 20 )
    self.Image_content:AddChild( bag_num , 1 )

    local num , max = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationSumAndMax")
    self.bag_num = Util:Label( num .. "/" .. max )
    self.bag_num:SetAnchorPoint( 0.5 , 0.5 )
    bag_num:AddChild( self.bag_num )
    PosTool.Center( self.bag_num )
end

-- 切换页签
function Modules.DivinationPackView:OnYeMei(index)
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )
end




--
-- @brief 物品背包条目 
-- @author: yjg
-- @date: 2016年8月20日18:31:32
--

Modules = Modules or {}

Modules.TemplDivinationPackItem = Modules.TemplDivinationPackItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDivinationPackItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.PACK_ITEM) 
    self:__Getwidget()
end

function Modules.TemplDivinationPackItem:__delete()
end

function Modules.TemplDivinationPackItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)    

    self:__ShowUI(data)

    --从新增记录表中删除
    CallModuleFunc(ModuleType.DIVINATION, "DelFrmNewRecordMap", self.__data.uid)    
end

function Modules.TemplDivinationPackItem:__AddToParent()
end

function Modules.TemplDivinationPackItem:__Getwidget( ... )
    --背景
    self.Image_itembg1 = self:GetWidget("Image_itembg1") 
    --内容
    self.Image_content1 = self:GetWidget("Image_content1") 

    --居中的使用按钮
    self:GetWidget("Button_center1"):SetVisible(false)

    --上面的使用按钮
    self:GetWidget("Button_up1"):SetVisible(false)

    --一键使用按钮
    self:GetWidget("Button_disposable1"):SetVisible(false)

    --名字
    self.Text_name1 = self:GetWidget("Text_name1")

    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")

end

function Modules.TemplDivinationPackItem:__ShowUI( data )
    --图标
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = data.id , obj_info = data })
    self.Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)     

    --进阶
    local text = ""
    if data.advanced and data.advanced ~= 0 then
        text = "+" .. data.advanced
    end
    --名字
    local name = Util:Name(data.name .. text , Macros.TypefaceSize.normal , data.quality)
    name:SetPosition(self.Text_name1:GetContentWidth()/2,self.Text_name1:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name1:AddChild(name)

    for i = 1 , 2 do
        if data.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP then
            --经验卡牌
            local tab = {
                            { zi = CPPGameLib.GetString("divination_to_exp_txt"), ys = Macros.Color.keypoint , jg = 5 , dx = Macros.TypefaceSize.slightly },
                            { zi = data.to_exp or 0, ys = Macros.Color.content , jg = 5 , dx = Macros.TypefaceSize.slightly },
                        }
            local property = Util:LabCenter(tab)
            property:SetPosition(10 , self.Image_content1:GetContentHeight()/2 + 15 - ( (i - 1) * 30 ) )
            property:SetAnchorPoint(0,0.5)
            self.Image_content1:AddChild(property)
            break
        elseif data.property[i] then
            local type_ , num_ = Util:Nature( data.property[i].type , data.property[i].value , true )
            local tab = {
                            { zi = type_ ..  CPPGameLib.GetString("common_colon") , ys = Macros.Color.keypoint , jg = 5 , dx = Macros.TypefaceSize.slightly },
                            { zi = num_ , ys = Macros.Color.content , jg = 5 , dx = Macros.TypefaceSize.slightly },
                        }
            local property = Util:LabCenter(tab)
            property:SetPosition(10 , self.Image_content1:GetContentHeight()/2 + 15 - ( (i - 1) * 30 ) )
            property:SetAnchorPoint(0,0.5)
            self.Image_content1:AddChild(property)

        end
     end 

    --new
    self.new = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "new" , true ) )
    self.new:SetPosition( self.icon_layer:GetNode():GetContentWidth()/2 + 10 , self.icon_layer:GetNode():GetContentHeight()/2 + 40 )
    self.new:SetAnchorPoint( 0.5 , 0.5 )
    self.icon_layer:AddChild(self.new , 999 ) 
    self.new:SetVisible( false )

    local obj_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination", data.uid )
    --装备中
    if obj_info.hero_id ~= 0 then
        local data = CPPGameLib.GetConfig("Hero", obj_info.hero_id )
        local heroName = Util:Label(data.name..CPPGameLib.GetString("equip_cross") , Macros.TypefaceSize.slightly , Macros.Color.content )
        heroName:SetAnchorPoint(1,0.5)
        heroName:SetPosition(self.Image_itembg1:GetContentWidth() - 20 , self.Text_name1:GetPositionY() )
        self.Image_itembg1:AddChild(heroName)
    end


    local data = CallModuleFunc(ModuleType.DIVINATION, "GetNewRecord" , data.uid )   
    if data and data.type == true then
        self.new:SetVisible( true )
    end
end


--设置新
function Modules.TemplDivinationPackItem:SetNew( var )
    self.new:SetVisible( var or false )
end

