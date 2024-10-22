--[[
%% @module: 水晶商店
%% @author: yjg
%% @created: 2018年5月9日14:33:02
--]]


Modules = Modules or {} 

Modules.CrystalShopView = Modules.CrystalShopView or BaseClass(BaseView)

function Modules.CrystalShopView:__init()
	self.__layout_name = "direct_buy"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_crystal")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("crystalShop"),
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

    -- 水晶购买
    local type_ = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"BuyIsShow" )
    if type_ then
        table.insert( t1 , {Modules.CrystalBuyTabView, "Panel_DirectBuy"} )
    end

    -- 水晶充值
    if GlobalModulesMgr:IsOpen( Macros.Game.SystemID.LUXURY ) then
        table.insert( t1 , {Modules.CrystalRechargeTabView, "Panel_DirectHotRecomment"} )
    end

    self.__tab_view_params = t1


    self.__loaded_callback = function()   	
    	self:__LoadedCallback()
    end

    self.__open_callback = function()     
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIRECT_BUY_SHOP)
    -- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, true)
end

--加载完成回调
function Modules.CrystalShopView:__LoadedCallback()  

    local text = {}
    local type_ = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"BuyIsShow" )
    if type_ then
        table.insert( text , CPPGameLib.GetString("crystalShop_buy") )
    end

    if GlobalModulesMgr:IsOpen( Macros.Game.SystemID.LUXURY ) then
        table.insert( text , CPPGameLib.GetString("crystalShop_recharge"))
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

function Modules.CrystalShopView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIRECT_BUY_SHOP, "tab_normal")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local label = Util:Name( data , Macros.TypefaceSize.largish )
    layer:AddChild(label)
    PosTool.Center( label , 0 , 30 )

    return layer
end

--打开回调
function Modules.CrystalShopView:OnCallBack( index , data )
    self:__TabChange( index )
end

--打开回调
function Modules.CrystalShopView:__OpenCallback( index , data )
    local tab = unpack(self.__open_data)    
    tab = tab or 1
    self.__tab:On(tab)   

    self:BindGlobalEvent(Macros.Event.CrystalShopModule.CRYSTAL_FLIP_OVER,function (_, index)
        self.__tab:On(index)  
    end)
end

function Modules.CrystalShopView:__Dispose()

end

--获取打开参数
function Modules.CrystalShopView:GetOpenParams()
    if self.index then
        return {self.index}
    end
end
