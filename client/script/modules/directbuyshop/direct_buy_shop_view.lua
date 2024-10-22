--[[
%% @module: 直购特惠主界面
%% @author: swordmansue
%% @created: 2017-08-31
--]]


Modules = Modules or {} 

Modules.DirectBuyShopView = Modules.DirectBuyShopView or BaseClass(BaseView)

function Modules.DirectBuyShopView:__init()
	self.__layout_name = "direct_buy"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_direct_buy")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("direct_buy_shop_title"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			self:CloseManual()
		end,
	}

    self.__tab_params_ex = {6,true}

    --默认开一个
    local t1 = {}

    -- 直购特惠
    local _ , type_ = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen1" )
    if type_.isOpen == "yes" then
        table.insert( t1 , {Modules.DirectBuyTabView, "Panel_DirectBuy"} )
    end

    -- 精准推送
    local _ , type_ = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen2" )
    if type_.isOpen == "yes" then
        table.insert( t1 , {Modules.DirectBuyHotRecommentTabview, "Panel_DirectHotRecomment"} )
    end

    --活动热卖
    local _ , type_ = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen3" )
    if type_.isOpen == "yes" then
        table.insert( t1 , {Modules.DirectBuyHotTabView, "Panel_DirectHotTab"} )
    end

    self.__tab_view_params = t1


    self.__loaded_callback = function()   	
    	self:__LoadedCallback()
    end

    self.__open_callback = function()     
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIRECT_BUY_SHOP)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, true)
end

--加载完成回调
function Modules.DirectBuyShopView:__LoadedCallback()  

    local text = {}
    local _ , type_ = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen1" )
    CPPGameLib.PrintTable("xy" , type_ ,"物品合成响应")  
    if type_.isOpen == "yes" then
        table.insert( text , CPPGameLib.GetString("direct_buy_shop_tab1") )
    end

    local _ , type_ = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen2" )
    if type_.isOpen == "yes" then
        table.insert( text , CPPGameLib.GetString("direct_buy_shop_tab3"))
    end

    local _ , type_ = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen3" )
    if type_.isOpen == "yes" then
        table.insert( text , CPPGameLib.GetString("direct_buy_shop_tab4"))
    end

    if #text == 0 then
        return 
    end

    local panel_tab = self:GetWidget("Panel_Tab")
    self.__tab:SetContentSize( panel_tab:GetContentWidth() + 20 , panel_tab:GetContentHeight() + 100 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0 , 1 )

    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, "tab_normal")
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, "tab_selected")
    self.__tab:SetOnCeil( on , no , 0 , 50 )  

    self.__tab:SetPosition( panel_tab:GetContentWidth()/2 , panel_tab:GetContentHeight() - 50)
    panel_tab:AddChild(self.__tab:GetNode())

end


function Modules.DirectBuyShopView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, "tab_normal")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local label = Util:Name( data , Macros.TypefaceSize.largish )
    layer:AddChild(label)
    PosTool.Center( label , 0 , 30 )

    return layer
end

--打开回调
function Modules.DirectBuyShopView:OnCallBack( index , data )
    self:__TabChange( index )
end

--打开回调
function Modules.DirectBuyShopView:__OpenCallback( index , data )
    local tab = unpack(self.__open_data)    
    tab = tab or 1
    self.__tab:On(tab)   
end

function Modules.DirectBuyShopView:__Dispose()
    CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"SetActionType" , nil )
end

--获取打开参数
function Modules.DirectBuyShopView:GetOpenParams()
    if self.index then
        return {self.index}
    end
end
