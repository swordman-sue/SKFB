--[[
%% @module: 精灵设置
%% @author: yjg
%% @created: 2017年6月22日21:08:39
--]]


LogicGUI = LogicGUI or {}

LogicGUI.TemplSpriteSet = LogicGUI.TemplSpriteSet or BaseClass(GUI.Template)

function LogicGUI.TemplSpriteSet:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self.__open_callback = function ()
	    self:__Getwidget()
	    self:__ShowUI()
	end	

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_SET_INFO)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_SET) 
end

function LogicGUI.TemplSpriteSet:__delete()
end

function LogicGUI.TemplSpriteSet:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--技能选择列表
	self.Image_lstBg = self:GetWidget("Image_lstBg") 
    --翻页容器
    local PageView = self:GetWidget("PageView_1")
    PageView:SetVisible(false)

    local Pvbg = Util:Layout( PageView:GetContentWidth() , PageView:GetContentHeight() )
    Pvbg:SetPosition( PageView:GetPositionX() , PageView:GetPositionY() )
    Pvbg:SetAnchorPoint( 0 , 0 )
    self.Image_bg:AddChild( Pvbg )

    self.PageView_1 = GUI.PageView.New(
                                        {
                                            view_width = Pvbg:GetContentWidth(),
                                            view_height = Pvbg:GetContentHeight(),
                                            turn_page_func = function( ... )
	                                            self:SetSpecialKillIcon()
                                            end
                                        }
                                    )
    Pvbg:AddChild(self.PageView_1:GetNode())
    PosTool.Center(self.PageView_1)

	--专属技能
	self.Image_exclusive = self:GetWidget("Image_exclusive")
	--通用技能
	self.Image_General = self:GetWidget("Image_General")

	--通用技能标题
	self.Image_Common = self:GetWidget("Image_Common")
	--通用技能列表
	self.Panel_Common = self:GetWidget("Panel_Common")	

	--左边按钮
	self.Button_left = Util:WidgetButton( self:GetWidget("Button_left") , nil , function ( ... )
		local index = self.PageView_1:GetCurPageIndex()
		self.PageView_1:ScrollToPage(index - 1)
	end ) 
	self.Button_left:SetZOrder(99)
	--右边按钮
	self.Button_right = Util:WidgetButton( self:GetWidget("Button_right") , nil , function ( ... )
		local index = self.PageView_1:GetCurPageIndex()
		self.PageView_1:ScrollToPage(index + 1)
	end ) 
	self.Button_right:SetZOrder(99)
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TemplSpriteSet:__ShowUI()
	--标题
	self.light = Util:Label( CPPGameLib.GetString("spirit_set") , Macros.TypefaceSize.popup , Macros.Color.white  )
	self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	self.Image_light:AddChild(self.light)

    --专属技能
    local spirit_exclusive = Util:Label( CPPGameLib.GetString("spirit_exclusive") , Macros.TypefaceSize.normal , Macros.Color.headline )
    spirit_exclusive:SetAnchorPoint( 0.5 , 1 )
    spirit_exclusive:SetPosition( self.Image_exclusive:GetContentWidth()/2 , self.Image_exclusive:GetContentHeight() - 5 )
    self.Image_exclusive:AddChild( spirit_exclusive )
    --通用技能
    local spirit_general = Util:Label( CPPGameLib.GetString("spirit_general")  , Macros.TypefaceSize.normal , Macros.Color.headline )
    spirit_general:SetAnchorPoint( 0.5 , 1 )
    spirit_general:SetPosition( self.Image_General:GetContentWidth()/2 , self.Image_General:GetContentHeight() - 5 )
    self.Image_General:AddChild( spirit_general )

    --通用技能
    local spirit_general = Util:Label( CPPGameLib.GetString("spirit_general")  , Macros.TypefaceSize.largish , Macros.Color.white )
    spirit_general:SetAnchorPoint( 0 , 0.5 )
    spirit_general:SetPosition( 5 , self.Image_Common:GetContentHeight()/2 )
    self.Image_Common:AddChild( spirit_general )

    local params =
    {
        item_class = LogicGUI.TemplCommonKillItem,
        item_width = 254.00,
        item_height = 110 ,
        row = 4,
        col = 1,
	    selectable = true,
	    select_callback	= function (item_index,item_data)
		    self:SetCommonKillIcon( item_index )
	    end,
        view_height = self.Panel_Common:GetContentHeight() ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_Common:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    --翻页
	self.lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritCnf")


	for i , v in ipairs( self.lst ) do
		v.page_class = LogicGUI.TemplSpriteSetInfo
	end
	self.lst_list:SetDataList( self.lst )

	self.PageView_1:SetDataList( self.lst )

	local data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
	for i , v in ipairs( self.lst ) do
		if v.id == data.cur_use_spirit then
			self.PageView_1:ForceDoLayout()
			self.PageView_1:ScrollToPage( i - 1 )

			--设置技能
			self:SetSpecialKillIcon()
			--设置通用技能
			self:SetCommonKillIcon( i )
			--设置选中条目
			self.lst_list:SelectItem(i)
		end
	end
end

function LogicGUI.TemplSpriteSet:OnButtonClose()
	local index = self.PageView_1:GetCurPageIndex() + 1
	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , self.lst[index].id )
	if lst then
		CallModuleFunc( ModuleType.SPIRIT , "SpiritUseReq" , self.lst[index].id)
	end
	self:DeleteMe()
end

--设置主动被动技
function LogicGUI.TemplSpriteSet:SetSpecialKillIcon( )
	local index = self.PageView_1:GetCurPageIndex() + 1

	CPPGameLib.PrintTable("xy" , self.lst   ,"物品合成响应")  

	local cnf = CPPGameLib.GetConfig("Spirit", self.lst[index].id )
	if not cnf then
		return 
	end

	--专属
	if not self.special_skill then

		local icon = nil
		icon , self.special_skill , self.special_touch = self:NewKillIcon( cnf.special_skill_id ) 
		self.Image_exclusive:AddChild(icon)
		PosTool.Center(icon , 0-(icon:GetContentWidth()/2) - 10 , -10 )

		local sp_initiative = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_initiative" , true) )
		sp_initiative:SetAnchorPoint( 0.5 , 1 )
		sp_initiative:SetPosition( icon:GetContentWidth() , icon:GetContentHeight() )
		icon:AddChild( sp_initiative , 5 )
		sp_initiative:SetPosition( icon:GetContentWidth() , icon:GetContentHeight() )
	else
		self.special_skill:SetImage( Resource.PathTool.GetSpiritSkillIconPath( cnf.special_skill_id ) )
	end

	--自动
	if not self.auto_skill then
		local icon = nil
		icon  , self.auto_skill , self.auto_touch = self:NewKillIcon( cnf.auto_skill_id , true ) 
		self.Image_exclusive:AddChild(icon)
		PosTool.Center(icon , (icon:GetContentWidth()/2) + 10 , -10 )

		local sp_passivity = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_passivity" , true) )
		sp_passivity:SetAnchorPoint( 0.5 , 1 )
		sp_passivity:SetPosition( icon:GetContentWidth() , icon:GetContentHeight() )
		icon:AddChild( sp_passivity , 5 )

		sp_passivity:SetPosition( icon:GetContentWidth() , icon:GetContentHeight() )
	else
		self.auto_skill:SetImage( Resource.PathTool.GetSpiritSkillIconPath( cnf.auto_skill_id ) )
	end

	self:__SetTips( self.special_touch , cnf.special_skill_id )
	self:__SetTips( self.auto_touch , cnf.auto_skill_id )

	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , self.lst[index].id )
	if not lst then
		self.special_skill:SetGray(true)
		self.auto_skill:SetGray(true)
		return 
	else
		self.special_skill:SetGray(false)
		self.auto_skill:SetGray(false)
	end

end

--设置通用技
function LogicGUI.TemplSpriteSet:SetCommonKillIcon( index )

	local cnf = CPPGameLib.GetConfig("Spirit", self.lst[index].id )
	if not cnf then
		return
	end
	--专属
	if not self.common_skill then
		local icon = nil
		icon , self.common_skill , self.common_touch = self:NewKillIcon( cnf.common_skill_id ) 
		self.Image_General:AddChild( icon )
		PosTool.Center( icon , 0 , -10 )

		local sp_initiative = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_initiative" , true) )
		sp_initiative:SetAnchorPoint( 0.5 , 1 )
		sp_initiative:SetPosition( icon:GetContentWidth() , icon:GetContentHeight() )
		icon:AddChild( sp_initiative , 5 )
		sp_initiative:SetPosition( icon:GetContentWidth() , icon:GetContentHeight() )
	else
		self.common_skill:SetImage( Resource.PathTool.GetSpiritSkillIconPath( cnf.common_skill_id ) )
	end

	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , self.lst[index].id )
	if not lst then
		self.common_skill:SetGray(true)
		return 
	else
		self.common_skill:SetGray(false)
	end

	CallModuleFunc(ModuleType.SPIRIT,"SetCommonKillId" , cnf.common_skill_id )
	self:__SetTips( self.common_touch , cnf.common_skill_id )
end

--创建精灵图标
function LogicGUI.TemplSpriteSet:NewKillIcon( id , auto )
	--技能底
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "bg_skill")
	local skill_bg = Util:Sprite(sp)

	--技能框
	local sp = nil
	if not auto then
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "skill_frame")
	else
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_ICON_NEW, "icon_blank_frame")
	end
	local skill_frame = Util:Sprite(sp) 
	skill_bg:AddChild(skill_frame , 3)
	PosTool.Center(skill_frame)

	--主动技能
	local initiative = Util:Sprite(Resource.PathTool.GetSpiritSkillIconPath( id ))
	initiative:SetAnchorPoint(0.5, 0.5)
	if not auto then
		skill_bg:AddChild(initiative , 2)
		PosTool.Center( initiative )
	else
		skill_frame:AddChild(initiative)
		PosTool.Center( initiative )
	end

	local layou = Util:Layout(skill_frame:GetContentWidth(), skill_frame:GetContentHeight())
	layou:SetAnchorPoint(0.5,0.5)
	layou:SetTouchEnabled(true)
	skill_frame:AddChild(layou)
	PosTool.Center(layou)

	return skill_bg , initiative , layou
end

function LogicGUI.TemplSpriteSet:__SetTips( layou , id )
	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , id )
	WidgetUnity.SetWidgetListener(layou ,
										function()	
											 self.__is_show = true
											 self.__is_touch_show = true
											 CPPActionManager.DelayTo(layou , 0.2,GlobalCallbackMgr:AddCallBackFunc(function ()
												self:__JudgeIsShow( id , lst and (lst.enhance_level + 1) or 1 , layou , true )
											end))			
										end, nil,
										function()
											self.__is_show = false
											self:__HideSkillTips()
										end,
										function()
											self.__is_show = false
											self:__HideSkillTips()
										end
								)
end

function LogicGUI.TemplSpriteSet:__JudgeIsShow(common_skillid,enhance_level,skill_frame,is_special)
	if self.__is_show then
		self.__is_touch_show = false
		self:__ShowSkillTips(common_skillid,enhance_level,skill_frame,is_special)
	end
end

--显示技能Tips
function LogicGUI.TemplSpriteSet:__ShowSkillTips(skill_id,skill_lev,skill_icon,is_special)
	if not self.__skill_tips then
		self.__skill_tips = LogicGUI.TemplSpiritSkillTips.New()		
	end
	self.__skill_tips:SetAnchorPoint(is_special and 0 or 1, 0.5)
	self.__skill_tips:Retain()
	self.__skill_tips:RemoveSelf(false)
	self.__root_widget:AddChild(self.__skill_tips:GetNode(),5)
	self.__skill_tips:Release()
	local pos = NodeUnity.ConvertPosBetweenTwoContainer(skill_icon,self.__root_widget, COCOPoint(is_special and (skill_icon:GetContentWidth() + 8) or -8, skill_icon:GetContentHeight() / 2))
	self.__skill_tips:SetPosition(pos.x, pos.y)
	
	self.__skill_tips:SetData(skill_id, skill_lev,is_special)
end

--隐藏技能Tips
function LogicGUI.TemplSpriteSet:__HideSkillTips()
	if self.__skill_tips then
		self.__skill_tips:DeleteMe()			
		self.__skill_tips = nil
	end
end



--[[
%% @module: 精灵设置信息
%% @author: yjg
%% @created: 2017年6月22日21:13:47
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplSpriteSetInfo = LogicGUI.TemplSpriteSetInfo or BaseClass(GUI.Template)

function LogicGUI.TemplSpriteSetInfo:__init( data )
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_SET_INFO) 
end

function LogicGUI.TemplSpriteSetInfo:__delete()
end

function LogicGUI.TemplSpriteSetInfo:__AddToParent()
end

function LogicGUI.TemplSpriteSetInfo:SetData( data )
	self.data = data
	self:__Getwidget()
	self:__ShowUI()
end

function LogicGUI.TemplSpriteSetInfo:__Getwidget()
	--背景
	self.Panel_1 = self:GetWidget("Panel_1") 
	--精灵模型
	self.Panel_bottom = self:GetWidget("Panel_bottom")
end

function LogicGUI.TemplSpriteSetInfo:__ShowUI()

	--精灵
    local spirit_model = LogicGUI.Model.New({info_id = self.data.id , parent = self.Panel_bottom , model_type = LogicGUI.ModelType.SPRITE})      
    spirit_model:SetPosition( self.Panel_bottom:GetContentWidth()/2 , 50 )
    spirit_model:SetScaleFactor(1.5)

	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , self.data.id )
	if not lst then
		spirit_model:SetGray(true)
	else
		spirit_model:SetGray(false)
	end

    --名字
    local name = Util:Name( self.data.name , Macros.TypefaceSize.normal , self.data.quality )
    name:SetPosition( self.Panel_bottom:GetContentWidth()/2 , 190 )
    self.Panel_bottom:AddChild( name )
end



--[[
%% @module: 精灵通用技能
%% @author: yjg
%% @created: 2017年6月23日10:22:56
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplCommonKillItem = LogicGUI.TemplCommonKillItem or BaseClass(GUI.ScrollItem)

function LogicGUI.TemplCommonKillItem:__init( data )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function LogicGUI.TemplCommonKillItem:__delete()
end

function LogicGUI.TemplCommonKillItem:__AddToParent()
end

function LogicGUI.TemplCommonKillItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout(254.00 , 110)
	local bg_sp9_get_approach = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_get_approach")
	self.bg = Util:Sprite9( bg_sp9_get_approach )
	self.bg:SetContentSize( self.layout:GetContentWidth() - 5 , self.layout:GetContentHeight() - 5 )
	self.bg:SetAnchorPoint(0.5 , 0.5)
	self.layout:AddChild(self.bg)
	PosTool.Center( self.bg )
	return self.layout
end

function LogicGUI.TemplCommonKillItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__ShowUI()
end

function LogicGUI.TemplCommonKillItem:__ShowUI()

	local cnf = CPPGameLib.GetConfig("Spirit", self.__data.id )
	if not cnf then
		return
	end
	local bg , icon = self:NewKillIcon( cnf.common_skill_id )
	bg:SetAnchorPoint( 0 , 0.5 )
	bg:SetPosition(  20 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild( bg )

	local config_skill = CPPGameLib.GetConfig("Skill", cnf.common_skill_id )
	if not config_skill then
		return
	end
	local name = Util:Label( config_skill.name , Macros.TypefaceSize.normal , Macros.Color.keypoint , 120 )
	PosTool.RightTo( bg , name , 30 )
	self.layout:AddChild( name )

	self.sp_on = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_on" , true) )
	self.sp_on:SetAnchorPoint( 0 , 1 )
	self.sp_on:SetPosition( 2 , self.layout:GetContentHeight() - 2)
	self.layout:AddChild( self.sp_on )
	self.sp_on:SetVisible(false)

	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , self.__data.id )
	if not lst then
		icon:SetGray(true)
		self:SetSelectedNo()
	else
		icon:SetGray(false)
	end

end

--创建精灵图标
function LogicGUI.TemplCommonKillItem:NewKillIcon( id )
	--技能底
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "bg_skill")
	local skill_bg = Util:Sprite(sp)

	--主动技能
	local initiative = Util:Sprite(Resource.PathTool.GetSpiritSkillIconPath( id ))
	initiative:SetAnchorPoint(0.5, 0.5)
	skill_bg:AddChild(initiative)
	PosTool.Center( initiative )

	--技能框
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "skill_frame")
	local skill_frame = Util:Sprite(sp)
	skill_bg:AddChild(skill_frame)
	PosTool.Center(skill_frame)

	return skill_bg , initiative
end

function LogicGUI.TemplCommonKillItem:SetSelected(var)
	if var == true then
		self:SetSelectedOn()
	else
		self:SetSelectedNo()
	end
end

function LogicGUI.TemplCommonKillItem:IsTouchEnable()
	local lst = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID" , self.__data.id )
	if not lst then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("spirit_notget_tips") )  
		return false
	else
		return true
	end
end

function LogicGUI.TemplCommonKillItem:ForceSetSelected( data )
	local id = CallModuleFunc(ModuleType.SPIRIT,"GetCommonKillId" )
	if data.common_skill_id == id then
		self:SetSelectedOn()
	else
		self:SetSelectedNo()
	end
end

function LogicGUI.TemplCommonKillItem:SetSelectedOn( data )
	local bg_sp9_get_approach = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_yellowframe")
	self.bg:SetImage( bg_sp9_get_approach )
	self.bg:SetContentSize( self.layout:GetContentWidth() - 5 , self.layout:GetContentHeight() - 5 )
	self.bg:SetAnchorPoint(0.5 , 0.5)

	self.sp_on:SetVisible(true)
end

function LogicGUI.TemplCommonKillItem:SetSelectedNo( data )
	local bg_sp9_get_approach = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_get_approach")
	self.bg:SetImage( bg_sp9_get_approach )
	self.bg:SetContentSize( self.layout:GetContentWidth() - 5 , self.layout:GetContentHeight() - 5 )
	self.bg:SetAnchorPoint(0.5 , 0.5)

	self.sp_on:SetVisible(false)
end

