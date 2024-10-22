--
-- @brief 公会技能界面
-- @author: yjg
-- @date: 2016年10月24日15:47:25
--

Modules = Modules or {}

Modules.UnionSkillView = Modules.UnionSkillView or BaseClass(BaseView)

function Modules.UnionSkillView:__init()
	self.__layout_name = "union_kill"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("union_kill"),
		show_diamond = true,
		show_gold = true,
		show_contribution = true , 
		show_power = true,
		close_func = function()
			self:CloseManual()
		end,
	}

	self.__tab_params_ex = {15 , true}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

	self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_KILL_ITEM)
end

function Modules.UnionSkillView:__OpenCallback()  
	self:__Getwidget()
	self:__ShowUI()

	self:__Server() 
end

function Modules.UnionSkillView:__Dispose()
end

function Modules.UnionSkillView:__Server()
	CallModuleFunc(ModuleType.UNION , "GuildSkillListReq") 

	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_KILL, function(_, index)
		if index == Macros.Game.UNION_SKILL.PERSON then
			self:PersonSetShowLst()
		else
			self:__SetShowUI()
		end
		self:SetRed()
	end)
end

function Modules.UnionSkillView:__Getwidget( ... )
	--背景	
	self.Image_bg = self:GetWidget("Image_bg") 
	--背景	
	self.Image_bg2 = self:GetWidget("Image_bg2") 
	--内容	
	self.Image_content = self:GetWidget("Image_content") 
	--页签	
	self.Panel_tab = self:GetWidget("Panel_tab") 
	--货币背景	
	self.Image_time = self:GetWidget("Image_time") 

	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		print("xy" , "(3)军团技能，小图标（军团技能规则）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Union_Skill")
        end )
	end )
	Button_rule:SetZOrder(100)
end

function Modules.UnionSkillView:__ShowUI( ... )
	local params =  
	{
	    item_class = Modules.UnionGuildSkillViewItem,
	    item_width = 831.19 ,
	    item_height = 171.60 ,
	    row = 4,
	    col = 1,
	    view_height = self.Image_content:GetContentHeight() - 10 ,
	}
	-- 创建参数
	self.guild_skill_list = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.guild_skill_list:GetNode())
	PosTool.Center(self.guild_skill_list:GetNode())
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionKillCnf") 
	self.guild_skill_list:SetDataList( lst )

	local params =  
	{
	    item_class = Modules.UnionPersonSkillViewItem,
	    item_width = 831.19 ,
	    item_height = 171.60 ,
	    row = 4,
	    col = 1,
	    view_height = self.Image_content:GetContentHeight() - 10 ,

		touchable = true,
        touch_callback = function(item_index, item_data)
			local data = item_data
			if data.show ~= true then
				local privilege = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.SKILL )
				if privilege ~= 0 then
					self:Skip( item_data.id )
					return 
				else
					GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights") )
					return 
				end
			end

			local personLevel = CallModuleFunc(ModuleType.UNION , "GetPersonSkill" , data.id )
			local guildLevel = CallModuleFunc(ModuleType.UNION , "GetGuildSkill" , data.id )  
			local nowCnf , nexCnf = CallModuleFunc(ModuleType.UNION , "GetSkillLst" , data.id , personLevel )
			
			local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", nexCnf.cost_type_list[1] )
			if num < nexCnf.cost_value_list[1] then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( "currency_insufficient_" .. nexCnf.cost_type_list[1] ) )
				return 
			elseif personLevel >= guildLevel then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_skillMax") )
				return 
			else
				local info = {}
				info.type = Macros.Game.UNION_SKILL.PERSON
				info.skill_id = data.id
				info.skill_level = personLevel + 1
				CallModuleFunc(ModuleType.UNION , "GuildSkillLevelUpReq" , info ) 
			end
        end,
	}
	-- 创建参数
	self.person_skill_list = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.person_skill_list:GetNode())
	PosTool.Center(self.person_skill_list:GetNode())
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionKillCnf") 
	self.person_skill_list:SetDataList( lst )

    --个人贡献
   	self.union_contribute = Util:Label( CPPGameLib.GetString("union_contribute_" , 0 ) , Macros.TypefaceSize.normal , Macros.Color.green )
   	self.union_contribute:SetAnchorPoint( 0 , 0.5 )
   	self.union_contribute:SetPosition( 10 , self.Image_time:GetContentHeight()/2 )
   	self.Image_time:AddChild( self.union_contribute )

    --军团经验
    self.union_corps = Util:Label( CPPGameLib.GetString("union_corps" , 0 ) , Macros.TypefaceSize.normal , Macros.Color.green )
   	self.union_corps:SetAnchorPoint( 0 , 0.5 )
   	self.union_corps:SetPosition( 10 , self.Image_time:GetContentHeight()/2 )
   	self.Image_time:AddChild( self.union_corps )

	--页签
	local text = {
			[1] = CPPGameLib.GetString("union_killLearn") ,
			[2] = CPPGameLib.GetString("union_killManage") ,		
	}
	
	local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.SKILL ) 
	if type_ ~= 1 then
		text = {
					[1] = CPPGameLib.GetString("union_killLearn") ,
			}
	end

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
function Modules.UnionSkillView:Ceil( index , data )
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
function Modules.UnionSkillView:OnCallBack( index , data )
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

	self.index = index

	self.guild_skill_list:SetVisible(false)
	self.person_skill_list:SetVisible(false)
	self.union_contribute:SetVisible(false)
	self.union_corps:SetVisible(false)
	self.Image_time:SetVisible(false)

	if index == 1 then
		
		self.person_skill_list:SetVisible(true)
		self:__SetShowUI()
	elseif index == 2 then
		self.Image_time:SetVisible(true)
		self.union_corps:SetVisible(true)
		self.guild_skill_list:SetVisible(true)
	end
end

--设置列表
function Modules.UnionSkillView:__SetShowUI( )
	if self.index == 1 then
		local t1 = CallModuleFunc(ModuleType.UNION , "GetUnionPersonKillCnf" )
		self.person_skill_list:SetDataList( t1 )
	end

	--列表相关调整
	self.guild_skill_list:ItemDataIter(function(index, item_data)
		local item = self.guild_skill_list:GetItem( index )
		if item then
			item:__SetShowUI(item_data)
		end
	end)

	local exp = CallModuleFunc(ModuleType.UNION , "GetUnionExp" )
	local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.GUILD_CONTRIBUTION )
	self.union_contribute:SetString( CPPGameLib.GetString("union_contribute" , num ) )
	self.union_corps:SetString( CPPGameLib.GetString("union_corps" , exp ) )
end

--跳转
function Modules.UnionSkillView:Skip( id )
	self.__tab:On(2)
	local index_ = 0
	self.person_skill_list:ItemDataIter(function(index, item_data)
		if item_data.id == id then
			index_ = index
		end
	end)
	self.guild_skill_list:LocateItem(index_)
end

function Modules.UnionSkillView:PersonSetShowLst( )
	local t1 = CallModuleFunc(ModuleType.UNION , "GetUnionPersonKillCnf" )
	local t2 = {}
	for i,v in ipairs(t1) do
		t2[v.id] = v
	end

	local index_ = 0
	self.person_skill_list:ItemDataIter(function(index, item_data)
		if t2[item_data.id] then
			item_data = t2[item_data.id]
		end
		local item = self.person_skill_list:GetItem( index )
		if item then
			item:__SetShowUI(t2[item_data.id])
		end
	end)


end

function Modules.UnionSkillView:SetRed( )
	
	local skillTime = CallModuleFunc(ModuleType.UNION , "GetPersonSkillTime" )
	if skillTime ~= 0 then
		local item = self.__tab:GetBtn(1)
		if not self.skillTime then
		    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
			self.skillTime = Util:Sprite( icon_red_point )
            self.skillTime:SetAnchorPoint( 0.5 , 0.5 )
            self.skillTime:SetPosition( item:GetContentWidth() - 5 , item:GetContentHeight() - 5 )
            item:AddChild( self.skillTime )
        else
        	self.skillTime:SetVisible(true)
		end
	else
		if self.skillTime then
			self.skillTime:SetVisible(false)
		end
	end 

	local unionTime = CallModuleFunc(ModuleType.UNION , "GetSkillLstTime" )
	if unionTime ~= 0 then
		local item = self.__tab:GetBtn(2)
		if item and not self.unionTime then
		    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
			self.unionTime = Util:Sprite( icon_red_point )
            self.unionTime:SetAnchorPoint( 0.5 , 0.5 )
            self.unionTime:SetPosition( item:GetContentWidth() - 5 , item:GetContentHeight() - 5 )
            item:AddChild( self.unionTime )
        else
        	if self.unionTime then
	        	self.unionTime:SetVisible(true)
	        end
		end
	else
		if self.unionTime then
			self.unionTime:SetVisible(false)
		end
	end 
end







--公会技能
Modules = Modules or {}
Modules.UnionGuildSkillViewItem = Modules.UnionGuildSkillViewItem or BaseClass(GUI.ScrollItem)

function Modules.UnionGuildSkillViewItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.UNION_KILL_ITEM)   
	self:__Getwidget()
end

function Modules.UnionGuildSkillViewItem:__delete()
end

function Modules.UnionGuildSkillViewItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__ShowUI( data )
end

function Modules.UnionGuildSkillViewItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--线
	self.Image_wire = self:GetWidget("Image_wire") 	
	--升级
	self.Button_button = Util:WidgetButton( 
											self:GetWidget("Button_button") , 
											CPPGameLib.GetString("union_upPromote") , 
											CPPGameLib.Handler(self,self.OnButtonButton),
											nil,nil,Macros.Color.button_yellow
										)
end

function Modules.UnionGuildSkillViewItem:__ShowUI( data )

	-- 图标
	local icon_sp_item_bg_0 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_0")
	local icon_sp_item_bg = Util:Sprite( icon_sp_item_bg_0 )
	icon_sp_item_bg:SetPosition( self.Panel_icon:GetContentWidth()/2 , self.Panel_icon:GetContentHeight()/2 )
	self.Panel_icon:AddChild(icon_sp_item_bg)

	self.icon = Util:Sprite( Resource.PathTool.GetUnionSkillIconPath(data.icon) )
	self.icon:SetPosition(icon_sp_item_bg:GetContentWidth()/2 , icon_sp_item_bg:GetContentHeight()/2 )
	icon_sp_item_bg:AddChild(self.icon)

	local icon_sp_item_0 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_0")
	local icon_sp_item = Util:Sprite( icon_sp_item_0 )
	icon_sp_item:SetPosition( icon_sp_item_bg:GetContentWidth()/2 , icon_sp_item_bg:GetContentHeight()/2 )
	icon_sp_item_bg:AddChild(icon_sp_item)

	--名字
	self.name = Util:Label( data.name , Macros.TypefaceSize.largish , Macros.Color.headline )
	self.name:SetAnchorPoint( 0 , 0.5 )
	self.Text_name:AddChild( self.name )
	PosTool.Center( self.name )

	--等级
	self.level = Util:Label( CPPGameLib.GetString("common_lev9" , data.level ) , Macros.TypefaceSize.largish , Macros.Color.headline )
	PosTool.RightTo( self.name , self.level , 10 )
	self.Text_name:AddChild( self.level )

	--当前效果
	self.union_nowResult = Util:Label( CPPGameLib.GetString("union_nowResult") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	self.union_nowResult:SetAnchorPoint( 0 , 0.5 )
	self.union_nowResult:SetPosition( self.Text_name:GetPositionX() , self.Text_name:GetPositionY() - 47 )
	self.Image_item:AddChild( self.union_nowResult )

	self.union_nowResultValue = Util:Label( "100" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( self.union_nowResultValue )
	PosTool.RightTo( self.union_nowResult , self.union_nowResultValue )	

	--下级效果：
	self.union_nexResult = Util:Label( CPPGameLib.GetString("union_nexResult") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	self.union_nexResult:SetAnchorPoint( 0 , 0.5 )
	self.union_nexResult:SetPosition( self.Text_name:GetPositionX() , self.union_nowResult:GetPositionY() - self.union_nexResult:GetContentHeight() - 2 )
	self.Image_item:AddChild( self.union_nexResult )

	self.union_nexResultValue = Util:Label( "100" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( self.union_nexResultValue )
	PosTool.RightTo( self.union_nexResult , self.union_nexResultValue )	

	--军团经验
	self.union_corpsExp = Util:Label( CPPGameLib.GetString("union_corpsExp") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	self.union_corpsExp:SetAnchorPoint( 0 , 0.5 )
	self.union_corpsExp:SetPosition( self.Text_name:GetPositionX() , self.union_nexResult:GetPositionY() - self.union_corpsExp:GetContentHeight() - 2 )
	self.Image_item:AddChild( self.union_corpsExp )

	self.union_corpsExpValue = Util:Label( "100" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( self.union_corpsExpValue )
	PosTool.RightTo( self.union_corpsExp , self.union_corpsExpValue )	

	--军团等级：
	self.union_corpsLv = Util:Label( CPPGameLib.GetString("union_corpsLv") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	self.union_corpsLv:SetAnchorPoint( 0 , 0.5 )
	self.union_corpsLv:SetPosition( self.Text_name:GetPositionX() , self.union_corpsExp:GetPositionY() - self.union_corpsLv:GetContentHeight() - 2 )
	self.Image_item:AddChild( self.union_corpsLv )

	--军团等级：
	self.union_corpsLvValue = Util:Label( "100" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( self.union_corpsLvValue )
	PosTool.RightTo( self.union_corpsLv , self.union_corpsLvValue )	

	self:__SetShowUI( data )
end

function Modules.UnionGuildSkillViewItem:__SetShowUI( data )
	local level = CallModuleFunc(ModuleType.UNION , "GetGuildSkill" , data.id ) 
	local nowCnf , nexCnf = CallModuleFunc(ModuleType.UNION , "GetSkillLst" , data.id , level )
	if level ~= 0 then
		--等级
		self.level:SetString( CPPGameLib.GetString("common_lev9" , level ) )
		--当前效果
		local type_ = Util:Nature( nowCnf.add_pro_type[1] ,  nowCnf.add_pro_value[1] , nil , nil , true )
		self.union_nowResultValue:SetString( type_ )
	else
		self.level:SetString(" ")
		self.union_nowResultValue:SetString( CPPGameLib.GetString("union_noStudy") )
	end

	--下级效果
	if nexCnf then
		local type_ = Util:Nature( nexCnf.add_pro_type[1] ,  nexCnf.add_pro_value[1] , nil , nil , true )
		self.union_nexResultValue:SetString( type_ )
	else
		self.union_nexResultValue:SetString( CPPGameLib.GetString("union_skillMax") )
	end

	--公会经验
	local exp = CallModuleFunc(ModuleType.UNION , "GetUnionExp" )
	local color = Macros.Color.red_color
	if exp >= nexCnf.cost_guild_exp then
		color = Macros.Color.front_color
	end
	self.union_corpsExpValue:SetString( nexCnf.cost_guild_exp )
	self.union_corpsExpValue:SetColor(unpack(color))

	--军团等级
	local level = CallModuleFunc(ModuleType.UNION , "GetUnionLevel" )
	local color = Macros.Color.red_color
	if level >= nexCnf.need_guild_level then
		color = Macros.Color.front_color
	end
	self.union_corpsLvValue:SetString( nexCnf.need_guild_level )
	self.union_corpsLvValue:SetColor(unpack(color))
end

function Modules.UnionGuildSkillViewItem:OnButtonButton( )
	local data = self.__data

	local skillLevel = CallModuleFunc(ModuleType.UNION , "GetGuildSkill" , data.id ) 
	local nowCnf , nexCnf = CallModuleFunc(ModuleType.UNION , "GetSkillLst" , data.id , skillLevel )

	--公会经验
	local exp = CallModuleFunc(ModuleType.UNION , "GetUnionExp" )
	if exp < nexCnf.cost_guild_exp then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noCorpsExp") )
		return 
	end

	--军团等级
	local level = CallModuleFunc(ModuleType.UNION , "GetUnionLevel" )
	if level < nexCnf.need_guild_level then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noCorpsLv") )
		return
	end

	local info = {}
	info.type = Macros.Game.UNION_SKILL.GUILD
	info.skill_id = self.__data.id
	info.skill_level = skillLevel + 1
	CallModuleFunc(ModuleType.UNION , "GuildSkillLevelUpReq" , info ) 
end






































--个人技能
Modules = Modules or {}
Modules.UnionPersonSkillViewItem = Modules.UnionPersonSkillViewItem or BaseClass( Modules.UnionGuildSkillViewItem )

function Modules.UnionPersonSkillViewItem:__init()
end

function Modules.UnionPersonSkillViewItem:__delete()
end

function Modules.UnionPersonSkillViewItem:__SetShowUI( data )
	--名字后面的等级
	self.level:SetString( " " )
	--当前等级
	self.union_nowResult:SetString( CPPGameLib.GetString("union_nowGrade") )
	--当前效果
	self.union_nexResult:SetString( CPPGameLib.GetString("union_nowResult") )
	--下级效果
	self.union_corpsExp:SetString( CPPGameLib.GetString("union_nexResult") )
	--升级消耗
	self.union_corpsLv:SetString( CPPGameLib.GetString( "union_upgradeExpend" ) ) ---------4
	--按钮文字替换
	self.Button_button:SetText( CPPGameLib.GetString("union_upLevel") )

	--技能等级
	--个人
	local personLevel = CallModuleFunc(ModuleType.UNION , "GetPersonSkill" , data.id ) 
	--公会
	local guildLevel = CallModuleFunc(ModuleType.UNION , "GetGuildSkill" , data.id ) 
	self.union_nowResultValue:SetString( personLevel .. "/" .. guildLevel )

	--效果
	local nowCnf , nexCnf = CallModuleFunc(ModuleType.UNION , "GetSkillLst" , data.id , personLevel )
	if personLevel ~= 0 then
		--当前效果
		local type_ = Util:Nature( nowCnf.add_pro_type[1] ,  nowCnf.add_pro_value[1] , nil , nil , true )
		self.union_nexResultValue:SetString( type_ )
	else
		self.union_nexResultValue:SetString( CPPGameLib.GetString("union_noStudy") )
	end

	--下级效果
	if nexCnf then
		local type_ = Util:Nature( nexCnf.add_pro_type[1] ,  nexCnf.add_pro_value[1] , nil , nil , true )
		self.union_corpsExpValue:SetString( type_ )
	else
		self.union_corpsExpValue:SetString( CPPGameLib.GetString("union_skillMax") )
	end

	--显示
	if data.show ~= true then
		self.Button_button:SetText( CPPGameLib.GetString("union_toOpen") )
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
		local x = Util:Sprite( sp )
		self.Button_button:SetNormalImage( sp )
		self.Button_button:SetContentSize( x:GetContentWidth() , x:GetContentHeight() )
		local r, g, b, a = unpack( Macros.Color.button_red )
		self.Button_button:SetOutline(r, g, b, a, 2)

	else
		self.Button_button:SetText( CPPGameLib.GetString("union_upLevel") )
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
		local x = Util:Sprite( sp )
		self.Button_button:SetNormalImage( sp )
		self.Button_button:SetContentSize( x:GetContentWidth() , x:GetContentHeight() )
		local r, g, b, a = unpack( Macros.Color.button_yellow  )
		self.Button_button:SetOutline(r, g, b, a, 2)
	end

	--消耗
	if self.union_corpsLvValue then
		self.union_corpsLvValue:RemoveSelf(true)
		self.union_corpsLvValue = nil
	end
	self.union_corpsLvValue = Util:MoneyControl( nexCnf.cost_type_list[1] , 0.8 , nexCnf.cost_value_list[1] , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( self.union_corpsLvValue )
	PosTool.RightTo( self.union_corpsLv , self.union_corpsLvValue )
end

function Modules.UnionPersonSkillViewItem:GetTouchNode()
    return self.Button_button
end