
--
-- @brief 公会排行榜
-- @author: yjg
-- @date: 2016年10月21日22:10:27
--


Modules = Modules or {}

LogicGUI.TemplUnionRank = LogicGUI.TemplUnionRank or BaseClass(GUI.Template)

function LogicGUI.TemplUnionRank:__init()
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}

    self.__tab_params_ex = {15}

    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()
        self:__Server()
    end

	self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_RANK_ITEM)

    self:InitTemplate(GUI.TemplateRegistry.UNION_RANK) 
end

function LogicGUI.TemplUnionRank:__delete()
end

function LogicGUI.TemplUnionRank:__Server()
	
	local info = {}
	info.rank_type = 0
	CallModuleFunc(ModuleType.UNION , "GuildRankListReq" , info)

    --购买记录
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_RANK , function (_, data)
    	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionLevelRank")
		self.__zones_scrollview:SetDataList( lst )
		self:__SetShowUI()
    end)
end

function LogicGUI.TemplUnionRank:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--标题
	self.Image_light = self:GetWidget("Image_light")
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--页签
	self.Panel_tab = self:GetWidget("Panel_tab")
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnButtonClose) )

end

function LogicGUI.TemplUnionRank:__ShowUI()
	--标题
	local biaoti = Util:Label( CPPGameLib.GetString("union_rank") , Macros.TypefaceSize.popup , Macros.Color.white )
	biaoti:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	biaoti:SetAnchorPoint(0.5,0.5)
	self.Image_light:AddChild(biaoti)

	local text = {
			[1] = CPPGameLib.GetString("union_info") ,
	}

    self.__tab:SetContentSize( self.Panel_tab:GetContentWidth() , self.Panel_tab:GetContentHeight() )
    self.__tab:SetData( text )
    self.__tab:SetItemAlign( 0.5 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetAnchorPoint( 0.5 , 0.5  )
    self.__tab:SetPosition(self.Panel_tab:GetContentWidth()/2 + 10 , self.Panel_tab:GetContentHeight()/2 )
    self.Panel_tab:AddChild(self.__tab:GetNode())

	local rank = Util:Label( CPPGameLib.GetString("union_myRank")  , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	rank:SetPosition(self.Image_content:GetPositionX() , 15 )
	rank:SetAnchorPoint(0 , 0.5 )
	self.Image_bg:AddChild(rank)

	self.rank = Util:Label( "-----"  , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_bg:AddChild(self.rank)
	PosTool.RightTo( rank , self.rank )


	local params =  
	{
	  item_class = Modules.TemplUnionRankItem,
	  item_width = self.Image_content:GetContentWidth(),
	  item_height = 130,
	  row = 4,
	  col = 1,
	  view_height = self.Image_content:GetContentHeight() - 10,
	}
	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.__zones_scrollview:SetAnchorPoint( 0.5,0.5 )
	self.__zones_scrollview:SetPosition(self.Image_content:GetContentWidth()/2 , self.Image_content:GetContentHeight()/2)
	self.Image_content:AddChild(self.__zones_scrollview:GetNode())
end

--设置UI
function LogicGUI.TemplUnionRank:__SetShowUI( index , data )
	local info = CallModuleFunc(ModuleType.UNION , "GetUnionMyRankInfo")
	self.rank:SetString( info.rank )
end

function LogicGUI.TemplUnionRank:OnButtonClose()
	self:DeleteMe()
end

--细胞 
function LogicGUI.TemplUnionRank:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sizeSp = Util:Sprite( on )
	local layer = Util:Layout( self.Panel_tab:GetContentWidth() , sizeSp:GetContentHeight() )
		
	local union_LvRank = Util:Label(CPPGameLib.GetString("union_LvRank") , Macros.TypefaceSize.popup , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_yellow )
	union_LvRank:SetAnchorPoint( 0.5 , 0.5 )
	union_LvRank:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight()/2 )
	layer:AddChild(union_LvRank)

	return layer
end

--选中
function LogicGUI.TemplUnionRank:OnCallBack( index , data )
end

Modules = Modules or {}

Modules.TemplUnionRankItem = Modules.TemplUnionRankItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionRankItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.UNION_RANK_ITEM) 
    self:__Getwidget()
end

function Modules.TemplUnionRankItem:__delete()
end

function Modules.TemplUnionRankItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TemplUnionRankItem:__AddToParent()
end

function Modules.TemplUnionRankItem:__Getwidget( ... )
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--排行头像
	self.Panel_rankicon = self:GetWidget("Panel_rankicon")
	--公会头像
	self.Panel_icon = self:GetWidget("Panel_icon")
	--内容框
	self.Image_2 = self:GetWidget("Image_2")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--等级
	self.Text_lv = self:GetWidget("Text_lv")
end

function Modules.TemplUnionRankItem:__ShowUI( data )

	--排行图标
	if data.rank <= 3 then
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_" .. data.rank )
		local rankIcon = Util:Sprite( sp )
		rankIcon:SetAnchorPoint(0.5,0.5)
		self.Panel_rankicon:AddChild( rankIcon )
		PosTool.Center( rankIcon )
	else
		local rankIcon = Util:Label( data.rank , Macros.TypefaceSize.tab , Macros.Color.headline )
		rankIcon:SetAnchorPoint(0.5,0.5)
		self.Panel_rankicon:AddChild( rankIcon )
		PosTool.Center( rankIcon )
	end

	--公会图标
	local iconSp = "union_icon" .. data.icon_id
	self.Panel_icon:SetBackGroundImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , iconSp) )
	self.Panel_icon:SetBackGroundImageScale9Enabled(true)
	self.Panel_icon:SetBackGroundImageCapInsets( 70 , 70 , 0 , 0 )
	
	--名字
	self.Text_name = Util:WidgetLabel( self.Text_name , data.guild_name , Macros.TypefaceSize.largish , Macros.Color.headline )

	--等级
	self.Text_lv = Util:WidgetLabel( self.Text_lv , CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	local lv = Util:Label( data.level , Macros.TypefaceSize.normal , Macros.Color.content )
	lv:SetAnchorPoint(0,0.5)
	self.Image_item:AddChild( lv )
	PosTool.RightTo( self.Text_lv , lv )


	--军团长
	local tab = {
					{ zi = CPPGameLib.GetString("union_rankColonel") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = data.caption_name , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
				}
	local union_rankColonel = Util:LabCenter( tab )
	union_rankColonel:SetAnchorPoint( 0 , 0.5 )
	union_rankColonel:SetPosition( 2 , self.Image_2:GetContentHeight() - union_rankColonel:GetContentHeight() + 8 )
	self.Image_2:AddChild( union_rankColonel )

	--军团成员
	local tab = {
					{ zi = CPPGameLib.GetString("union_rankMember") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
					{ zi = data.member_num .."/" .. CPPGameLib.GetConfig("Guild", data.level ).member_max , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
				}
	local union_rankMember = Util:LabCenter( tab )
	union_rankMember:SetAnchorPoint( 0 , 0.5 )
	union_rankMember:SetPosition( 2 , union_rankColonel:GetPositionY() - union_rankMember:GetContentHeight() - 4 )
	self.Image_2:AddChild( union_rankMember )



	-- --军团副本
	-- local union_rankEctype = Util:Label( CPPGameLib.GetString("union_rankEctype") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	-- union_rankEctype:SetAnchorPoint( 0 , 0.5 )
	-- union_rankEctype:SetPosition( 2 , union_rankMember:GetPositionY() - union_rankEctype:GetContentHeight() - 4 )
	-- self.Image_2:AddChild( union_rankEctype )


end