--
-- @brief 公会大厅界面
-- @author: yjg
-- @date: 2016年10月14日10:39:01
--

Modules = Modules or {}

Modules.UnionHallView = Modules.UnionHallView or BaseClass(BaseView)

function Modules.UnionHallView:__init()
	self.__layout_name = "union_discuss"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("union"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			self:CloseManual()
		end,
	}

	self.__tab_params_ex = {15 , true}

    self.__tab_view_params = 
    {
        {Modules.TemplUnionHallHall, "Panel_hall"},
        {Modules.TemplUnionHallList, "Panel_list"},
        {Modules.TemplUnionDynamic, "Panel_dynamic"},
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end
	
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_ACCUSE,function (_, data)
    	self.__tab:On(1)
    end)

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION)
end

function Modules.UnionHallView:__OpenCallback()  
	self:__Getwidget()
	self:__ShowUI() 
end

function Modules.UnionHallView:__RegistAllEvents()
end

function Modules.UnionHallView:__Dispose()
	if self.memberLst then
		self.memberLst:DeleteMe()
		self.memberLst = nil
	end
	
	if self.trendsLst then
		self.trendsLst:DeleteMe()
		self.trendsLst = nil
	end	

	if self.unionCheck then
		self.unionCheck:DeleteMe()
		self.unionCheck = nil
	end
end

function Modules.UnionHallView:__Getwidget( ... )
	-- 主界面
	self.Image_bg = self:GetWidget("Image_bg")
	--背景
	self.Image_bg2 = self:GetWidget("Image_bg2")
	--页签
	self.Panel_tab = self:GetWidget("Panel_tab")
end

function Modules.UnionHallView:__ShowUI( ... )
	local text = {
			[1] = CPPGameLib.GetString("union_info") ,
			[2] = CPPGameLib.GetString("union_member") ,
			[3] = CPPGameLib.GetString("union_dynamic") ,			
	}

    self.__tab:SetContentSize( self.Panel_tab:GetContentWidth() , self.Panel_tab:GetContentHeight() + 15 )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0.5 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition(self.Panel_tab:GetContentWidth()/2 , self.Panel_tab:GetContentHeight()/2 + 5 )
    self.Panel_tab:AddChild(self.__tab:GetNode())

end

--细胞 
function Modules.UnionHallView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    local sizeSp = Util:Sprite( on )
	local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

	local name = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.white , nil , nil , Macros.Color.button_red )
	name:SetAnchorPoint( 0.5 , 0.5 )
	layer:AddChild(name)
	PosTool.Center( name )

	return layer
end

--选中
function Modules.UnionHallView:OnCallBack( index , data )
	
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

	local text = {
			[1] = CPPGameLib.GetString("union_hall"),
			[2] = CPPGameLib.GetString("union_member"),
			[3] = CPPGameLib.GetString("union_dynamic"),
	}
	self.__title_bar:SetTitle( text[index] )

	self:__TabChange(index)

end