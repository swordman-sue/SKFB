
--[[
%% @module: 背包界面（觉醒）
%% @author: yjg
%% @created: 2018年12月27日15:44:33
--]]


Modules = Modules or {}

Modules.TemplPackAwaken = Modules.TemplPackAwaken or BaseClass(BaseTabView)

function Modules.TemplPackAwaken:__init()
end

--选项卡界面创建完成回调
function Modules.TemplPackAwaken:__LoadedCallback()
    self:__Getwidget()
end

--选项卡界面打开回调
function Modules.TemplPackAwaken:__OpenCallback()
    self:__ShowUI()
    self:__Server()
end

--选项卡界面关闭回调
function Modules.TemplPackAwaken:__CloseCallback()
    if self.awakenGoodsLst then
        self.awakenGoodsLst:DeleteMe()
        self.awakenGoodsLst = nil
    end
    if self.updata_goods then
        CPPGameLib.UnbindEvent(self.updata_goods)
        self.updata_goods = nil
    end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplPackAwaken:__Dispose()
end

function Modules.TemplPackAwaken:__Server()
    self.updata_goods = self:BindGlobalEvent( Macros.Event.PackModule.UPDATE_GOODS ,function ( _ , info , is_up , itemType )
        if itemType == Macros.Game.ObjType.AWAKEN_TOOL then
            self:__SetShowUI( info , is_up )
        end
    end)
end

function Modules.TemplPackAwaken:__Getwidget()
    self.Image_content = self:GetWidget("Image_content") 
end

function Modules.TemplPackAwaken:__ShowUI()
    local params =  
    {
        item_class = Modules.TemplAwakenCell,
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
    self.awakenGoodsLst = GUI.ScrollView.New(params)   
    self.awakenGoodsLst:SetDataList( CallModuleFunc(ModuleType.Pack, "GetAllAwakenGoods") )
    self.Image_content:AddChild(self.awakenGoodsLst:GetNode())
    PosTool.Center(self.awakenGoodsLst:GetNode())
end

function Modules.TemplPackAwaken:__SetShowUI( info , is_up )
    if is_up then
        local del_index = nil
        self.awakenGoodsLst:ItemIter(function ( index , item , data )
            local info = CallModuleFunc(ModuleType.Pack, "GetAwakenGoods", data.item_id ) 
            if not info then
                del_index = index
            else
                item:SetNum( )
            end
        end)
        if del_index then
            self.awakenGoodsLst:DetachItem( del_index )
        end
    else
        self.awakenGoodsLst:AttachItem( info , nil , true )
    end

end

function Modules.TemplPackAwaken:__GetShowType()
end



Modules = Modules or {}

Modules.TemplAwakenCell = Modules.TemplAwakenCell or BaseClass(GUI.ScrollItem)

function Modules.TemplAwakenCell:__init()
    self:InitTemplate(GUI.TemplateRegistry.PACK_ITEM)     
end

function Modules.TemplAwakenCell:__delete()
end

function Modules.TemplAwakenCell:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local awaken_config = CPPGameLib.GetConfig("AwakeningItem",data.item_id)
    --上按钮
    self:GetWidget("Button_up1"):SetVisible(false)
    --下按钮
    self:GetWidget("Button_disposable1"):SetVisible(false)
    --头像
    local Panel_icon = self:GetWidget("Panel_icon")
    self.icon_layer = LogicGUI.IconLayer.New(false)
    Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)
    self.icon_layer:SetData({id = data.item_id , num = data.item_num})
    self.icon_layer.__touch_callback = function ( ... )
        LogicGUI.TemplHeroAwakenPropView.New(data.item_id,5)
    end
    --名字
    local textName = self:GetWidget("Text_name1")
    local name = Util:Name(awaken_config.name,nil,data.quality)
    name:SetPosition(textName:GetContentWidth()/2,textName:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    textName:AddChild(name)

    --文本   
    local Image_content = self:GetWidget("Image_content1")
    local dice_label = Util:RichText("",Macros.TypefaceSize.slightly,Image_content:GetContentWidth() - 20  , 0 )
    dice_label:SetPosition(10 , Image_content:GetContentHeight() - 10)
    dice_label:SetAnchorPoint(0,1)
    Image_content:AddChild(dice_label)

    --分解
    Util:WidgetButton(self:GetWidget("Button_center1"),CPPGameLib.GetString("Awaken_Equip_Resolve"),function ( )  
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , self.__data.item_id , nil, self.__data.item_id )    
    end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)
end

--数量刷新
function Modules.TemplAwakenCell:SetNum( )
    local info = CallModuleFunc(ModuleType.Pack, "GetAwakenGoods", self.__data.item_id ) 
    self.__data = info
    self.icon_layer:SetNum(info.item_num)
end
