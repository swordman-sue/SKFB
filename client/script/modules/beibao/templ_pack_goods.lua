
--[[
%% @module: 背包界面(物品)
%% @author: yjg
%% @created: 2018年12月25日20:02:49
--]]


Modules = Modules or {}

Modules.TemplPackGoods = Modules.TemplPackGoods or BaseClass(BaseTabView)

function Modules.TemplPackGoods:__init()
end

--选项卡界面创建完成回调
function Modules.TemplPackGoods:__LoadedCallback()
    self:__Getwidget()
    self:__Server()
end

--选项卡界面打开回调
function Modules.TemplPackGoods:__OpenCallback()
    self:__ShowUI()
end

--选项卡界面关闭回调
function Modules.TemplPackGoods:__CloseCallback()
    if self.packGoodsLst then
        self.packGoodsLst:DeleteMe()
        self.packGoodsLst = nil
    end
    if self.updata_goods then
        CPPGameLib.UnbindEvent(self.updata_goods)
        self.updata_goods = nil
    end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplPackGoods:__Dispose()
end

function Modules.TemplPackGoods:__Server()
    self.updata_goods = self:BindGlobalEvent(Macros.Event.PackModule.UPDATE_GOODS,function ( _ , info , is_up , itemType )
        if itemType == Macros.Game.ObjType.ITEM then
            self:__SetShowUI( info , is_up )
        end
    end)
end

function Modules.TemplPackGoods:__Getwidget()
    self.Image_content = self:GetWidget("Image_content") 
end

function Modules.TemplPackGoods:__ShowUI()
    local params =  
    {
        item_class = Modules.TemplPackItem,
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
    self.packGoodsLst = GUI.ScrollView.New(params)
    self.packGoodsLst:SetDataList( CallModuleFunc(ModuleType.Pack, "GetAllCommonGoods") )
    self.Image_content:AddChild(self.packGoodsLst:GetNode())
    PosTool.Center(self.packGoodsLst:GetNode())
end

function Modules.TemplPackGoods:__SetShowUI( info , is_up )
    if is_up then
        local del_index = nil
        self.packGoodsLst:ItemIter(function ( index , item , data )
            local info = CallModuleFunc(ModuleType.Pack, "GetCommonGoods", data.item_id ) 
            if not info then
                del_index = index
            else
                item:SetNum( )
            end
        end)
        if del_index then
            self.packGoodsLst:DetachItem( del_index )
        end
    else
        self.packGoodsLst:AttachItem( info , nil , true )
    end

end

function Modules.TemplPackGoods:__GetShowType()
end

--
-- @brief 物品背包条目 
-- @author: yjg
-- @date: 2016年8月20日18:31:32
--

Modules = Modules or {}

Modules.TemplPackItem = Modules.TemplPackItem or BaseClass(GUI.ScrollItem)

function Modules.TemplPackItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.PACK_ITEM) 
    self:__Getwidget()
end

function Modules.TemplPackItem:__delete()
end

function Modules.TemplPackItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)    

    self:__ShowUI(data)

    --从新增记录表中删除
    CallModuleFunc(ModuleType.Pack, "DelFrmNewRecordMap", self.__data.item_id)    
end

function Modules.TemplPackItem:__AddToParent()
end

function Modules.TemplPackItem:__Getwidget( ... )
    --背景
    self.Image_itembg1 = self:GetWidget("Image_itembg1") 
    --内容
    self.Image_content1 = self:GetWidget("Image_content1") 
    --居中的使用按钮
    self.Button_center1 = Util:WidgetButton(   
                                                self:GetWidget("Button_center1") , 
                                                CPPGameLib.GetString("btn_use") , 
                                                CPPGameLib.Handler(self,self.OnButtonCenter),
                                                Macros.TypefaceSize.largish,  nil , Macros.Color.button_yellow 
                                                )
    self.Button_center1:SetVisible(false)
    --上面的使用按钮
    self.Button_up1 = Util:WidgetButton( 
                                            self:GetWidget("Button_up1") , 
                                            CPPGameLib.GetString("btn_use") , 
                                            CPPGameLib.Handler(self,self.OnButtonCenter),
                                            Macros.TypefaceSize.largish,  nil , Macros.Color.button_yellow
                                         )
    self.Button_up1:SetVisible(false)
    --一键使用按钮
    self.Button_disposable1 = Util:WidgetButton( 
                                                    self:GetWidget("Button_disposable1") , 
                                                    CPPGameLib.GetString("pack_oneuseHeadline") , 
                                                    CPPGameLib.Handler(self,self.OnButtonDisposable),
                                                    Macros.TypefaceSize.largish,  nil , Macros.Color.button_red
                                                    )
    self.Button_disposable1:SetVisible(false)
    --名字
    self.Text_name1 = self:GetWidget("Text_name1")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
end

function Modules.TemplPackItem:__ShowUI( data )
    --图标
    self.icon_layer = LogicGUI.IconLayer.New(true)
    self.icon_layer:SetData({id = data.item_id , num = data.item_num})
    self.Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)  

    --名字
    local name = Util:Name(data.name , Macros.TypefaceSize.normal , data.quality)
    name:SetPosition(self.Text_name1:GetContentWidth()/2,self.Text_name1:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name1:AddChild(name)

    --文本
    local dice_label = Util:Label( data.description , Macros.TypefaceSize.slightly , Macros.Color.content , self.Image_content1:GetContentWidth() - 20  , self.Image_content1:GetContentHeight() - 12 )
    dice_label:SetPosition(10 , self.Image_content1:GetContentHeight() - 10)
    dice_label:SetAnchorPoint(0,1)
    self.Image_content1:AddChild(dice_label)

    --时间
    self.time = Util:Label( " " , Macros.TypefaceSize.slightly , Macros.Color.red_color )
    self.Text_name1:AddChild(self.time)
    PosTool.RightTo( name , self.time , 10)

    --如果是可使用的
    if data.is_can_use == 1 then

        self.Button_center1:SetText(CPPGameLib.GetString("btn_use"))
        WidgetUnity.SetWidgetListener( self.Button_center1 , nil, nil, function ( ... )
            self:OnButtonCenter()
        end)

        --如果只是道具
        if data.type == Macros.Game.ItemBigType.NORMAL then
            self.Button_center1:SetVisible(true)

            --新手指引特殊操作检测(背包使用物品（物品ID）)
            CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PACK_USE_ITEM, self.Button_center1, data.item_id)

        --如果是礼包
        elseif data.type == Macros.Game.ItemBigType.GIFT and data.item_num ~= 1 and data.sub_type ~= 2 then
            self.Button_up1:SetVisible(true)
            self.Button_disposable1:SetVisible(true)

            --新手指引特殊操作检测(背包使用礼包（物品ID）)
            CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PACK_USE_GIFT, self.Button_up1, data.item_id)
        else
            self.Button_center1:SetVisible(true)

            --新手指引特殊操作检测(背包使用物品（物品ID）)
            CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PACK_USE_ITEM, self.Button_center1, data.item_id)

            --新手指引特殊操作检测(背包使用礼包（物品ID）)
            CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PACK_USE_GIFT, self.Button_center1, data.item_id)
        end 

    elseif data.sub_type ~= 0 and data.systemid then
        self.Button_center1:SetVisible(true)
        if data.goforwardbtnname then
            self.Button_center1:SetText( data.goforwardbtnname )
        end
        if data.goforward then
            WidgetUnity.SetWidgetListener( self.Button_center1 , nil, nil, function ( ... )
                local openType = GlobalModulesMgr:IsOpen( data.goforward[1] , true )
                if openType == false then
                    return
                end
                local assist_info = CPPGameLib.GetConfig("SystemEntry", data.goforward[1] )
                CallModuleFunc(assist_info.module_type, "OpenView", assist_info.view_type or assist_info.module_type , data.goforward[2] )
            end)    
        elseif #data.systemid ~= 0 then
            WidgetUnity.SetWidgetListener( self.Button_center1 , nil, nil, function ( ... )
                local openType = GlobalModulesMgr:IsOpen( data.systemid[1] , true )
                if openType == false then
                    return
                end
                GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, data.systemid[1])
            end) 
        end

    end
    
    --new
    self.new = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "new" , true ) )
    self.new:SetPosition( self.icon_layer:GetNode():GetContentWidth()/2 + 10 , self.icon_layer:GetNode():GetContentHeight()/2 + 40 )
    self.new:SetAnchorPoint( 0.5 , 0.5 )
    self.icon_layer:AddChild(self.new , 999 ) 
    self.new:SetVisible( false )

    local data = CallModuleFunc(ModuleType.Pack, "GetNewRecord" , data.item_id )   
    if data and data.type == true then
        self.new:SetVisible( true )
    end

    --红点
    if self.Button_center1 then
        local red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
        self.icon_red_point = Util:Sprite( red_point )
        self.icon_red_point:SetAnchorPoint( 0.5 , 0.5 )
        self.Button_center1:AddChild( self.icon_red_point )
        self.icon_red_point:SetPosition( self.Button_center1:GetContentWidth() , self.Button_center1:GetContentHeight() )
        self.icon_red_point:SetVisible(false)
    end

    --精灵特殊
    if self.__data.sub_type ==  Macros.Game.NormalItemSubType.SPIRIT_DEBRIS then
        self:SpriteSpecialShow()
    end 

    self:SetTime( )
end

--使用
function Modules.TemplPackItem:OnButtonCenter( ... )
    --如果类型是普通道具 与 可以在背包使用
    if self.__data.type == 1 and self.__data.is_can_use == 1 then
        -- print("如果类型是普通道具 与 可以在背包使用")
        --精灵特殊处理
        if self.__data.sub_type ==  Macros.Game.NormalItemSubType.SPIRIT_DEBRIS then
            self:SpriteSpecialOn()
            return
        end 

        --如果数量大于1
        if self.__data.item_num == 1 then
            local data = {}
            data.item_id = self.__data.item_id
            data.item_num = 1
            data.select_gift_id = 0
            CallModuleFunc(ModuleType.Pack, "ItemUseReq",data)   
        else     
            local onuse = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , self.__data.item_id )
        end

    --否则类型是礼包 而且还是多选礼包
    elseif self.__data.type == 2 and self.__data.sub_type == 2 then
        -- print("否则类型是礼包 而且还是多选礼包")
        local onuse = GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.PACK_SELECT , false , self.__data )

    --再否则 只是礼包
    else
        CallModuleFunc(ModuleType.SUNDRY, "GiftBagPackVacancy", self.__data.item_id , nil , true , function ( text )
            if #text == 0 then
                local data = {}
                data.item_id = self.__data.item_id
                data.item_num = 1
                data.select_gift_id = 0
                CallModuleFunc(ModuleType.Pack, "ItemUseReq",data)     
            end
        end )

    end
end

--一键使用
function Modules.TemplPackItem:OnButtonDisposable( ... )
    local onuse = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , self.__data.item_id )
end

--设置新
function Modules.TemplPackItem:SetTime( )
    local text , time = CallModuleFunc(ModuleType.Pack, "ItemValidTime", self.__data.item_id )   
    if text and time ~= 0 then
        --突破
        local func
        local function ShowTime( ... )
            local text , time = CallModuleFunc(ModuleType.Pack, "ItemValidTime", self.__data.item_id )  
            if time ~= 0 then 
                self.time:SetString( text or "" )
                CPPActionManager.DelayTo(self.time , 1 , func )
            else
                self.time:SetString( text or "" )
            end        
        end
        func = GlobalCallbackMgr:AddCallBackFunc( ShowTime )
        ShowTime()
    end
end

--数量刷新
function Modules.TemplPackItem:SetNum( )
    local info = CallModuleFunc(ModuleType.Pack, "GetCommonGoods", self.__data.item_id ) 
    self.__data = info
    self.icon_layer:SetNum(info.item_num)
end


------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--道具特殊处理
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

--精灵特殊处理
function Modules.TemplPackItem:SpriteSpecialShow( )
    --道具数量
    local item_num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", self.__data.item_id )

    --该精灵信息
    local spriteId = self.__data.param1
    local spirit_info = CallModuleFunc( ModuleType.SPIRIT,"GetSpiritInfoByID", spriteId )

    --我持有这个精灵
    if spirit_info then
        self.icon_red_point:SetVisible(false)
        self.Button_center1:SetText(CPPGameLib.GetString("spirit_GoUpStar"))
    else
        --如果数量够
        if item_num >= self.__data.param2 then
            self.icon_red_point:SetVisible(true)
            self.Button_center1:SetText(CPPGameLib.GetString("plunderedtreasures_compose"))
        else
            self.Button_center1:SetText(CPPGameLib.GetString("equip_ToGet"))
        end
    end

end

--选中
function Modules.TemplPackItem:SpriteSpecialOn( )
    local spriteId = self.__data.param1
    --道具数量
    local item_num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", self.__data.item_id )
    local _ , info = ObjIDTool.GetInfo( spriteId )

    local spirit_info = CallModuleFunc( ModuleType.SPIRIT,"GetSpiritInfoByID", spriteId )
    if spirit_info then
        CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT , spriteId )
        return
    end

    --如果数量不够
    if item_num < self.__data.param2 then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("spirit_compound_nopiece" , self.__data.param2 , info.name ) )
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.data.info_id)
    else
        local text = CPPGameLib.GetString("pack_sprite_compound" , self.__data.param2 , self.__data.name , info.name )
        LogicGUI.TemplAffirmTips.New({
            type = LogicGUI.TemplAffirmTipsType.YES_NO,
            content = text,
            confirm_text = CPPGameLib.GetString("common_confirm"),
            confirm_func = function()
                local info = {}
                info.fragment_id = self.__data.item_id 
                CallModuleFunc( ModuleType.SPIRIT,"SpriteComposeReq", info )
            end,
            show_close_btn = false,
        })  
    end
end