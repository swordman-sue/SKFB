    
-- @brief 爬塔购买
-- @author: yjg
-- @date: 2016年9月28日19:27:32


Modules = Modules or {}

Modules.TowerShop = Modules.TowerShop or BaseClass(BaseView)
function Modules.TowerShop:__init()
    self.__layout_name = "tower_shop"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("tower_shopBtn"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            self:CloseManual()
        end,
    }

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    local t1 = self:__SetShowTab()
    self.__tab_params = {
        { sp_2 , sp_1 } , t1 , 10 , CPPGameLib.Handler(self, self.OnYeMei) , false
    }

    self.__loaded_callback = function()
        self:__Getwidget()
    end

    self.__open_callback = function()
        self.__tab:SetPosition(0 , 0)
        self.Panel_tag:AddChild(self.__tab:GetNode())
        if not CallModuleFunc(ModuleType.TOWER , "GetAllShopLst") then
            CallModuleFunc(ModuleType.TOWER , "FbTowerDataReq")
            CallModuleFunc(ModuleType.TOWER, "TowerBuyNotesRequest",Macros.Game.SHOP.TOWER)
        else
            self:__OpenCallback()
        end
    end
    
    self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_UP,function (_,data)
        self:__OpenCallback()
    end)

    self:__AddPreloadTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
end

function Modules.TowerShop:__delete()
end

function Modules.TowerShop:__OpenCallback()
    local All = CallModuleFunc(ModuleType.TOWER , "GetAllShopLst" )
    if not All then
        CallModuleFunc(ModuleType.TOWER , "ShopSettle" )
    end

    --self:__Getwidget()
    self:__ShowUI()

    self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_RECORD,function (_,data)
        if self.index == Macros.Game.TowerShop.JIANGLI then
            self:OnYeMei(Macros.Game.TowerShop.JIANGLI)
        else
            self:TowerShopFiltration()
        end

        self:__SetShowUI()
    end)    
    self:__SetShowUI()
end

function Modules.TowerShop:__Getwidget( ... )
    --居中层
    self.Panel_store = self:GetWidget("Panel_store") 
    --背景层
    self.Image_1 = self:GetWidget("Image_1") 
    --左边按钮
    self.Image_back = self:GetWidget("Image_back") 
    local effect = nil
    self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_back , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_back:GetContentWidth()/2 , self.Image_back:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )
    --右边按钮
    self.Image_front = self:GetWidget("Image_front") 
    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_front , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_front:GetContentWidth()/2 , self.Image_front:GetContentHeight()/2 )
    
    --按钮光
    self.Image_selectedtab1 = self:GetWidget("Image_selectedtab1") 
    self.Image_selectedtab1:SetVisible(false)

    self.Image_selectedtab2 = self:GetWidget("Image_selectedtab2") 
    self.Image_selectedtab2:SetVisible(false)

    --充值按钮
    self.Button_money = self:GetWidget("Button_money") 
    self.Button_money:SetVisible(false)

    --列表容器
    self.Panel_tool = self:GetWidget("Panel_tool") 
    --翻页容器
    self.PageView_tool = self:GetWidget("PageView_tool") 

    --列表容器——2
    self.Panel_gift = self:GetWidget("Panel_gift") 
    self.Panel_gift:SetVisible(false)
    --列表容器——2
    self.PageView_gift = self:GetWidget("PageView_gift") 
    --页签容器
    self.Panel_tag = self:GetWidget("Panel_tab") 

end

function Modules.TowerShop:__ShowUI( ... )
    print("xy" , "__ShowUI-----------")
    --页签
    self.lst_list = {}
    self.packIcon = {}

    if unpack(self.__open_data) then
        local type_ = CallModuleFunc(ModuleType.TOWER , "GetOnShopPos" , unpack(self.__open_data) )
        self.__tab:On(type_)
    else
        self.__tab:On(1)
    end

    local red_image = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.red_image = Util:Sprite(red_image)
    self.red_image:SetPosition( self.__tab.btn_list[4]:GetContentWidth() - 5 , self.__tab.btn_list[4]:GetContentHeight() - 5 )
    self.__tab.btn_list[4]:AddChild( self.red_image )
    self.red_image:SetVisible(false)

    self.tower_ShopNot = Util:Name( CPPGameLib.GetString( "tower_ShopNot" ) , Macros.TypefaceSize.headlineII )
    self.tower_ShopNot:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_tool:AddChild( self.tower_ShopNot , 99 )
    PosTool.Center( self.tower_ShopNot )
    self.tower_ShopNot:SetVisible(false)

    self:__SetShowUI()
end

--设置页签
function Modules.TowerShop:__SetShowTab( )
    local cnf = CPPGameLib.GetConfig("TowerShop", nil, nil , true ) 
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    self.SetTabType = 0
    for k , v in pairs( cnf ) do
        if v.type == 5 and userDate.level >= v. need_level then
            self.SetTabType = self.SetTabType + 1
            break
        end
    end

    local t1 = {}
    if self.SetTabType ~= 0 then
        t1 = {  
                CPPGameLib.GetString("tower_shopCommodity"), 
                CPPGameLib.GetString("equip_quality3"),
                CPPGameLib.GetString("equip_quality4"),
                CPPGameLib.GetString("equip_quality5"),
                CPPGameLib.GetString("tower_shopAward"),
            }
    else
        t1 = {  
                CPPGameLib.GetString("tower_shopCommodity"), 
                CPPGameLib.GetString("equip_quality3"),
                CPPGameLib.GetString("equip_quality4"),
                CPPGameLib.GetString("tower_shopAward"),
            }
    end
    return t1
end

function Modules.TowerShop:__SetShowUI( ... )
    if self.tower_prestige then
      self.tower_prestige:RemoveSelf(true)
      self.tower_prestige = nil
    end

    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")

    local lst = { 
                  { zi = CPPGameLib.GetString("tower_prestige") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
                  { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.TOWER_PRESTIGE) , jg = 5 ,dx = 0.8},
                  { zi = userDate.tower_prestige , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.content },
                }
    self.tower_prestige =  Util:LabCenter( lst )
    self.tower_prestige:SetAnchorPoint(0,0.5)
    self.tower_prestige:SetPosition( self.Panel_gift:GetPositionX() - (self.Panel_gift:GetContentWidth()/2) , self.Panel_gift:GetPositionY() - 30 )
    self.Panel_store:AddChild(self.tower_prestige)



    local index = CallModuleFunc(ModuleType.TOWER,"GetNowBuyNum")
    if index ~= 0 then
        self.red_image:SetVisible(true)
    else
        self.red_image:SetVisible(false)
    end
end

function Modules.TowerShop:OnYeMei(ind)
    if self.SetTabType == 1 then
        if ind == 4 then
            self.index = 5
        elseif ind == 5 then
            self.index = 4
        else
            self.index = ind
        end
    else
        self.index = ind
    end

    --下面的小箭头
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[ind]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )

    self:TowerShopSetLst( self.index )
end

--设置列表
function Modules.TowerShop:TowerShopSetLst( index )
    for i=1,#self.packIcon do
        self.packIcon[i]:DeleteMe()
    end
    self.packIcon = {}
    self.PageView_tool:RemoveAllPages()
    self.PageView_tool:SetTouchEnabled(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))

    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local lst = CallModuleFunc(ModuleType.TOWER , "GetShopLst" , index )
    local t1 = {}
    for i , v in ipairs( lst ) do
        if userDate.level >= v. need_level then
            table.insert( t1 , v )
        end
    end
    lst = t1

    if #lst <= 0 then
        if self.tower_ShopNot then
            self.tower_ShopNot:SetVisible(true)
        end
        return 
    else
        if self.tower_ShopNot then
            self.tower_ShopNot:SetVisible(false)
        end
    end

    local cnf = Util:SegmentationTab( lst , 6 )
    local ind = 0
    local index_ = 0
    for i,v in ipairs(cnf) do
        local index = 0
        local layer = Util:Layout( self.PageView_tool:GetContentWidth() , self.PageView_tool:GetContentHeight() )
        local interval = layer:GetContentWidth() / 3 
        local y = layer:GetContentHeight() - 197 
        for ii,vv in ipairs(v) do
            index = index + 1
            index_ = index_ + 1 
            ind = ind + 1
            if index > 3 then
                index = 1
                y = 0
            end
            self.packIcon[index_] = {}
            self.packIcon[index_] = GUI.TemplTowerShopCell.New(vv)
            self.packIcon[index_]:SetPosition( (index-1) * interval , y)
            self.packIcon[index_]:GetNode():SetOpacity(0)
            layer:AddChild( self.packIcon[index_]:GetNode() )
        end
        self.PageView_tool:AddPage(layer)    
    end
    self:ShowAction()
end

--过滤已经购买的道具
function Modules.TowerShop:TowerShopFiltration( )
    for i = 1 , #self.packIcon do
        self.packIcon[i]:SetUI()
    end
end

--动作
function  Modules.TowerShop:ShowAction()
    local index = 0
    local func = nil
    local function moveAnimate( ... )
      index = index + 1
      if self.packIcon[index] then
        CPPActionManager.FadeIn(self.packIcon[index]:GetNode(),0.05,func)
      end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()

    if unpack(self.__open_data) then
        local __ , page_index = CallModuleFunc(ModuleType.TOWER , "GetOnShopPos" , unpack(self.__open_data) )
        self.PageView_tool:ScrollToPage( page_index - 1 )
    end
end



GUI = GUI or {}
GUI.TemplTowerShopCell = GUI.TemplTowerShopCell or BaseClass(GUI.Template)
function GUI.TemplTowerShopCell:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)
    self.data = data
    self:__Getwidget()
    self:__ShowUI(data)
end

function GUI.TemplTowerShopCell:__AddToParent()

end

function GUI.TemplTowerShopCell:__Getwidget()
  --背景
    self.Image_2 = self:GetWidget("Image_2") 
    --名字
    self.Panel_name = self:GetWidget("Panel_name") 

    --图标
    self.Panel_head = self:GetWidget("Panel_head") 
    --原价
    self.Text_couldBuyTimes = self:GetWidget("Text_couldBuyTimes") 
    self.Text_couldBuyTimes:SetText(" ")
    --价格背景
    self.Image_coinBg = self:GetWidget("Image_coinBg") 
    self.Image_coinBg:SetVisible(false)

    --货币图标
    self.Image_coin = self:GetWidget("Image_coin") 
    self.Image_coin:SetVisible(false)

    --价钱
    self.Text_coinNum = self:GetWidget("Text_coinNum") 
    self.Text_coinNum:SetVisible(false)

    --VIP图片
    self.Image_vip = self:GetWidget("Image_vip") 
    self.Image_vip:SetVisible(false)
      --vip字样
      self.Text_vip = self:GetWidget("Text_vip") 
    --原价容器X2
    self.Panel_before = self:GetWidget("Panel_before") 
    self.Panel_before:SetVisible(false)
      self.Text_before = self:GetWidget("Text_before") 
      self.Text_beforeNum = self:GetWidget("Text_beforeNum") 
      self.Image_beforeCoin = self:GetWidget("Image_beforeCoin") 
      self.Image_55_0 = self:GetWidget("Image_55_0") 
    --价格容器X2
    self.Panel_after = self:GetWidget("Panel_after") 
    self.Panel_after:SetVisible(false)
      self.Text_after = self:GetWidget("Text_after") 
      self.Text_afterNum = self:GetWidget("Text_afterNum") 
      self.Image_afterCoin = self:GetWidget("Image_afterCoin") 
  --已购买
    self.Image_HaveBuy = self:GetWidget("Image_HaveBuy") 
    self.Image_HaveBuy:SetVisible(false)
end

function GUI.TemplTowerShopCell:__ShowUI()
    local data = self.data

    local is_in_lead = CallModuleFunc(ModuleType.LEAD, "IsInLead")
    --头像
    local icon = LogicGUI.IconLayer.New(false)
    self:GetWidget("Panel_head"):AddChild(icon:GetNode())
    PosTool.Center(icon)
    icon:SetData({id = data.object_id , num = data.object_num })

    --价格
    self:SetPrice( data )
    --界面信息
    self:SetUI( data )

    local type,info = ObjIDTool.GetInfo(data.object_id)
    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.largish , info.quality or info.init_quality)
    name:SetPosition(self.Panel_name:GetContentWidth()/2  , self.Panel_name:GetContentHeight() /2 )
    name:SetAnchorPoint(0.5,0.5)
    self.Panel_name:AddChild(name)

    --触摸
    self.btn = Util:Layout( self.__root_widget:GetContentWidth() , self.__root_widget:GetContentHeight() , function ( ... )
      self:Transition(data)
    end)
    self.btn:SetPosition(self.__root_widget:GetContentWidth()/2 , self.__root_widget:GetContentHeight()/2 )
    self.btn:SetAnchorPoint(0.5,0.5)
    self.__root_widget:AddChild(self.btn)
 
    --新手指引特殊操作检测(爬塔商店-购买（商品ID）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TOWER_SHOP_BUY, self:GetNode(), data.id)
end

function GUI.TemplTowerShopCell:Transition( )
    local data = self.data

    local nowStar = CallModuleFunc(ModuleType.TOWER , "GetSumStar")
    local record = CallModuleFunc(ModuleType.TOWER , "GetBuyRecord" , data.id)

    if record and data.buy_num_max ~= 0 and record >= data.buy_num_max then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_limit_buy_num") )

    else
        local t1 = {}
        if data.cost_type_list then
            for i,v in ipairs(data.cost_type_list) do
                table.insert(t1,{ type = v , num = data.cost_value_list[i] })
            end
        end
        if data.cost_item_id_list then
            for i,v in ipairs(data.cost_item_id_list) do
                table.insert(t1,{ type = v , num = data.cost_item_num_list[i] })
            end
        end

        local info = {}
        info.item_id = data.object_id 
        info.type = Macros.Game.SHOP.TOWER
        info.goods_id = data.id
        info.cost = t1
        info.num = data.object_num 
        
        if data.type == 4 then
            info.max = 1
        end

        info.func = function ( pitchNum )
            if nowStar < data.need_star then
                -- 没有达到可购买星级
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_starDeficiency") )
            else
                local data = {}
                data.type = info.type
                data.param1 = info.goods_id
                data.param2 = pitchNum

                local type_ = nil
                for i,v in ipairs(info.cost) do
                    local num = 0
                    if v.price_list then
                        if v.price_list[pitchNum] then
                            for i=1,pitchNum do
                                num = num + v.price_list[i]
                            end
                        else
                            num = v.price_list[#v.price_list]
                        end
                    else
                        num = v.num * pitchNum
                    end

                    local contype , config = ObjIDTool.GetInfo( v.type )
                    if contype == Macros.Game.ObjType.ITEM then
                        local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", v.type )
                        if num > item_num then
                            type_ = v.type
                            break
                        end
                    else
                        if num > CallModuleFunc(ModuleType.ROLE,"GetPropValue",v.type) then
                            type_ = v.type
                            break
                        end
                    end
                end

                if info.max == 0 then
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_today_can_notbuy") )
                elseif not type_ then
                    CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",data)
                else
                    local contype , config = ObjIDTool.GetInfo( type_ )
                    local txt = nil
                    local item_id = nil
                    if contype == Macros.Game.ObjType.ITEM then
                        txt = "common_material_not_enough"
                        item_id = type_
                    else
                        txt = "currency_insufficient_"..type_
                        local config = CPPGameLib.GetConfig("HuoBi", type_ )
                        item_id = config.big_icon
                    end
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
                    
                    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, item_id)
                end
            end
        end
 
        GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
    end
end

--设置价格
function GUI.TemplTowerShopCell:SetPrice( )
    local cost = {}

    if self.data.cost_type_list then
        for i , v in ipairs( self.data.cost_type_list ) do
            table.insert( cost , { item_type = v , item_num = self.data.cost_value_list[i] } )
        end
    end
    if self.data.cost_item_id_list then
        for i , v in ipairs( self.data.cost_item_id_list ) do
            table.insert( cost , { item_type = v , item_num = self.data.cost_item_num_list[i] } )
        end
    end

    local interval = self.Image_2:GetContentWidth() / #cost
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "short_price_bg")

    for i , v in ipairs( cost ) do
        local bg = Util:Sprite( sp )
        bg:SetAnchorPoint( 0.5 , 0.5 )
        bg:SetPosition( (interval/2) + interval * (i - 1) ,  self.Image_coinBg:GetPositionY() )
        self.Image_2:AddChild( bg )

        local item_type , info = ObjIDTool.GetInfo( v.item_type )
        local consume = nil
        if item_type == Macros.Game.ObjType.CURRENCY then
            consume = Util:MoneyControl( v.item_type , 0.8)
        elseif item_type == Macros.Game.ObjType.ITEM then
            consume = Util:Sprite( Resource.PathTool.GetItemPath( info.res_id ) )
            consume:SetScaleFactor( 0.5 )
        end
        consume:SetPosition( 0 , bg:GetContentHeight()/2 )
        consume:SetAnchorPoint(0,0.5)
        bg:AddChild(consume)


        if info.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS or 
            info.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then

            local piece_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "piece_icon")
            local piece_icon = Util:Sprite( piece_icon_sp )
            piece_icon:SetAnchorPoint(0,1)
            piece_icon:SetPosition( 6.5 , consume:GetContentHeight() - 6.5 )
            consume:AddChild(piece_icon, LogicGUI.IconLayer.InnerZOrder.ICON)
        end

        --数量
        local consumeNum = Util:Label( v.item_num , Macros.TypefaceSize.slightly )
        consumeNum:SetPosition( consume:GetContentWidth() , consume:GetPositionY() )
        consumeNum:SetAnchorPoint( 0 , 0.5 )
        bg:AddChild( consumeNum )
    end

end

--物品数量
-- 1928 【优化】【神装商店】除奖励外的另外3个标签页的商品，购买时也需要历史最高星级达到配置值
function GUI.TemplTowerShopCell:SetUI( )
    local data = self.data

    if not self.tips then
        self.tips = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.content  )
        self.tips:SetPosition(self.Text_couldBuyTimes:GetContentWidth()/2 , self.Text_couldBuyTimes:GetContentHeight()/2  )
        self.Text_couldBuyTimes:AddChild(self.tips)
    end
    
    local nowStar = CallModuleFunc(ModuleType.TOWER , "GetSumStar")
    --星不足
    if nowStar < data.need_star then
        self.tips:SetString( CPPGameLib.GetString("tower_condition_buy",data.need_star) )
        return
    end

    --如果是奖励
    if data.type == Macros.Game.TowerShop.JIANGLI then
        local record = CallModuleFunc(ModuleType.TOWER , "GetBuyRecord" , data.id)
        --满了
        if record and record >= data.buy_num_max then
            self.Image_HaveBuy:SetVisible(true)
        else
            self.Image_HaveBuy:SetVisible(false)
            self.tips:SetString( CPPGameLib.GetString("tower_num" , data.buy_num_max - ( record or 0 ) ) )
        end
        return 
    end


    --道具数量
    local type,info = ObjIDTool.GetInfo(data.object_id)

    local num = 0
    local max = 0
    if type == Macros.Game.ObjType.ITEM then
      if info.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS then
        num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", data.object_id )
        max = CPPGameLib.GetConfig("EquipmentCompose", info.target_id).item_num
      elseif info.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
        num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", data.object_id )
        max = CPPGameLib.GetConfig("HeroCompound", info.target_id).item_num
      end
    end

    if max ~= 0 then 
        self.tips:SetString( CPPGameLib.GetString("tower_amount", num,max) )
    end
end
