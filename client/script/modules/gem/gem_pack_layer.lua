--[[
%% @module: 宝石左边ui
%% @author: yjg
%% @created: 2017年9月20日16:43:36
--]]

Modules = Modules or {}

Modules.GemPackLayer = Modules.GemPackLayer or BaseClass(UILayer)

function Modules.GemPackLayer:__init( node , func )
	self.node = node
	self:__ShowUI() 
	self:__Server() 
end

function Modules.GemPackLayer:__delete()
	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end
end

function Modules.GemPackLayer:__ShowUI(  )

	local sp9_gem_iconbg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp9_gem_iconbg") )

	local params =  
	{
	    item_class = Modules.GemPackLayerItem,
	    item_width = (self.node:GetContentWidth()/3) - 2.5 ,
	    item_height = (self.node:GetContentHeight()/6) - 5 ,
	    row = 6,
	    col = 3,
	    item_space_r = 5,
	    item_space_c = 2,
	    view_height = self.node:GetContentHeight() - 5  ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.node:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

-- 1995 宝石背包
-- 任务描述
-- 1.有多少个宝石就显示多少个，其余空白的格子不显示
-- 2.当拥有的宝石超过显示上限之后，可以下拉界面，显示更多
	local lst = CallModuleFunc(ModuleType.GEM , "GetGemLit" )
	if #lst < 18 then
		for i = 1 , 18 - #lst do
			table.insert( lst , { empty = true } )
		end
	end
	self.lst_list:SetDataList( lst )
end


function Modules.GemPackLayer:__Server(  )
	--刷新
	self.up_content = self:BindGlobalEvent(Macros.Event.GemModule.UP_CONTENT , function ( _ , data )
		self.lst_list:ItemIter(function (index, item)
			if item then
				item:__SetShowUI()
			end
		end)
	end)

	--重置
	self.up_lst = self:BindGlobalEvent(Macros.Event.GemModule.UP_LST , function ( _ , data  )
		local lst = CallModuleFunc(ModuleType.GEM , "GetGemLit" )
		if #lst < 18 then
			for i = 1 , 18 - #lst do
				table.insert( lst , { empty = true } )
			end
		end
		self.lst_list:SetDataList( lst )
	end)
end


Modules = Modules or {}
Modules.GemPackLayerItem = Modules.GemPackLayerItem or BaseClass(GUI.ScrollItem)

function Modules.GemPackLayerItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.GemPackLayerItem:__delete()
	if self.icon_layer then
		self.icon_layer:DeleteMe()
		self.icon_layer = nil
	end
end

function Modules.GemPackLayerItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.GemPackLayerItem:__CreateWidgetFromLuaCode()
	self.sp9_gem_iconbg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp9_gem_iconbg") )

	self.__layout = Util:Layout( self.sp9_gem_iconbg:GetContentWidth() , self.sp9_gem_iconbg:GetContentHeight() )

    self.__layout:AddChild( self.sp9_gem_iconbg )
    PosTool.Center(self.sp9_gem_iconbg)

	return self.__layout
end

function Modules.GemPackLayerItem:__ShowUI(data)
	if not data.gem_id then
		return 
	end

    self.icon_layer = LogicGUI.IconLayer.New( false )
    self.icon_layer:SetData({id = data.gem_id , num = data.num })
    self.__layout:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)   
    self.icon_layer:SetTouchSwallow(false) 

    self.icon_layer:ShowIconQuality(false)
    self.icon_layer:ShowIconShade(false)
    self.icon_layer:ShowIconBG(false)

	local layour = Util:Layout( self.__layout:GetContentWidth() , self.__layout:GetContentHeight() , function ( ... )
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_GEM_INFO , false , data.gem_id )
	end)
	layour:SetAnchorPoint( 0.5 , 0.5 )
    self.icon_layer:AddChild( layour )
    PosTool.Center(layour)

    layour:SetTouchSwallow(false) 
end

function Modules.GemPackLayerItem:__SetShowUI(data)
	local lst = CallModuleFunc(ModuleType.GEM , "GetPack" , self.__data.gem_id)
	if self.icon_layer and lst then
		self.icon_layer:SetData({id = lst.id , num = lst.num })
	end
end