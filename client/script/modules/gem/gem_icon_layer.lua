--[[
%% @module: 宝石左边ui
%% @author: yjg
%% @created: 2017年9月20日16:43:36
--]]

Modules = Modules or {}

Modules.GemIconLayer = Modules.GemIconLayer or BaseClass(UILayer)

function Modules.GemIconLayer:__init( node , func )
	self.node = node
	self:__ShowUI()
	self:__Server()  
end

function Modules.GemIconLayer:__delete()
	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end
end

function Modules.GemIconLayer:__ShowUI(  )
	local params =  
	{
	    item_class = Modules.GemIconLayerItem,
	    item_width = self.node:GetContentWidth() ,
	    item_height = ( (self.node:GetContentHeight() - 100 ) / 5) ,
	    row = 6,
	    col = 1,
	    item_space_r = 2,
	    item_space_c = 2,
	    view_height = self.node:GetContentHeight() - 50 ,
	    selectable = true,
        select_callback = function ( index , item )
        	if item.uid ~= 0 then
	            CallModuleFunc(ModuleType.GEM, "SetOnHero",item.uid)
	            self:SelectedIcon()
	        end
        end
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.node:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

	local lineup_list = CallModuleFunc(ModuleType.HERO, "GetLineupList")
	local t1 = {}
	local t2 = {}
	for i,v in ipairs(lineup_list) do
		if v.uid ~= 0 then
			table.insert( t1 , v )
		else
			table.insert( t2 , v )
		end
	end
	
	for i , v in ipairs( t2 ) do
	 	table.insert( t1 , v )
	 end 

	self.lst_list:SetDataList( t1 )

	local id = CallModuleFunc(ModuleType.GEM, "GetOnHero")
	if not id then
		self.lst_list:SelectItem(1)
		CallModuleFunc(ModuleType.GEM, "SetOnHeroUid" , lineup_list[1].uid )
	end
	self:SelectedIcon()
end

function Modules.GemIconLayer:__Server(  )
	-- --英雄选择
	-- self:BindGlobalEvent(Macros.Event.GemModule.SELECT_HERO , function( ... )
	-- 	-- self:__SetShowUI()
	-- end)
end


--选中特效
function Modules.GemIconLayer:SelectedIcon(  )
    --获得我选中的英雄信息
    local id = CallModuleFunc(ModuleType.GEM, "GetOnHero")
    --TODO：列表相关调整
    self.lst_list:ItemIter(function(i, item , itemDate )
        if item then
            --找到一样的
            if itemDate.uid == id then
	            CallModuleFunc(ModuleType.GEM, "SetOnHeroUid",itemDate.uid)
            	item:SetSelect(true)
            else
            	item:SetSelect(false)
            end
        end
    end)
end






Modules = Modules or {}
Modules.GemIconLayerItem = Modules.GemIconLayerItem or BaseClass(GUI.ScrollItem)

function Modules.GemIconLayerItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.GemIconLayerItem:__delete()

	if self.icon then
		self.icon:DeleteMe()
		self.icon = nil
	end

end

function Modules.GemIconLayerItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.GemIconLayerItem:__CreateWidgetFromLuaCode()
	self.sp9_gem_iconbg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp9_gem_iconbg") )
	self.layout = Util:Layout( self.sp9_gem_iconbg:GetContentWidth() , self.sp9_gem_iconbg:GetContentHeight() )

	return self.layout
end

function Modules.GemIconLayerItem:__ShowUI(data)
	--头像
	if data.uid ~= 0 then
		local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",data.uid)

	    --头像
	    self.icon = LogicGUI.IconLayer.New(false, false)
	    self.icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight() / 2)
	    self.icon:SetAnchorPoint(0.5, 0.5 )
	    self.layout:AddChild(self.icon:GetNode())
	    self.icon:SetData({id = hero_info.info_id })
	    -- self.icon:ShowEquipStrengthenLev(true)
	    self.icon:SetTouchSwallow(false)

	    local selected = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
	    self.selected = Util:Sprite9( selected )
	    self.icon:AddChild( self.selected , 5)
	    PosTool.Center( self.selected )
	    self.selected:SetVisible(false)
	end
end

function Modules.GemIconLayerItem:SetSelect(var)
	if self.selected then
		self.selected:SetVisible(var)
	end
end
