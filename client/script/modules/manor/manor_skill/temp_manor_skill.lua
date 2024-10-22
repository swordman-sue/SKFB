
--[[
%% @module: 领地技能
%% @author: yjg
%% @created: 2016年10月8日09:52:19
--]]

Modules = Modules or {}

Modules.TempManorSkill = Modules.TempManorSkill or BaseClass(GUI.Template)

function Modules.TempManorSkill:__init(node)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		CallModuleFunc(ModuleType.MANOR, "GetSkillLst")
		self:__Getwidget()
		self:__ShowUI()

		self:SetLst()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_MANOR_SKILL_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_MANOR_SKILL)

	self.__skill_event_handle = self:BindGlobalEvent(Macros.Event.ManorModule.MANOR_SKILL,function (_ , data)
		self:UpLst(data)
	end)	

end
function Modules.TempManorSkill:__Dispose()
	if self.__zones_scrollview then
		self.__zones_scrollview:DeleteMe()
		self.__zones_scrollview = nil 
	end

end

function Modules.TempManorSkill:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")
	--关闭
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--内容背景
	self.Image_contentBg = self:GetWidget("Image_contentBg")
end

function Modules.TempManorSkill:__ShowUI()
	local manor_kill = Util:Label(CPPGameLib.GetString("manor_kill") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	manor_kill:SetAnchorPoint(0.5 , 0.5)
	manor_kill:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )         
	self.Image_light:AddChild(manor_kill)

	local params =  
	{
	  	item_class = Modules.TempManorSkillItem,
	  	horizon = true,
	  	item_width = 283.00,
	  	item_height = 479.00,
	  	row = 1,
	  	col = 3,
	  	item_space_c = 5,
	  	view_width = self.Image_contentBg:GetContentWidth() - 10 ,
	  	-- view_height = self.Image_contentBg:GetContentHeight() - 10 ,
	}
	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.Image_contentBg:AddChild(self.__zones_scrollview:GetNode())
	PosTool.Center(self.__zones_scrollview:GetNode())

	self.num = Util:Label(CPPGameLib.GetString("manor_aggregate",0) , Macros.TypefaceSize.normal , Macros.Color.headline )
	self.num:SetAnchorPoint(0 , 0.5)
	self.num:SetPosition(self.Image_bg:GetContentWidth()/2 - (self.Image_contentBg:GetContentWidth()/2) , self.Image_bg:GetContentHeight()/2 + (self.Image_contentBg:GetContentHeight()/2) - 15 )
	self.Image_bg:AddChild(self.num)

	self:SetLst()
end

function Modules.TempManorSkill:SetLst( ... )
	local lst = CallModuleFunc(ModuleType.MANOR, "GetSkillLst")
	self.__zones_scrollview:SetDataList(lst)

	local lst = CallModuleFunc(ModuleType.MANOR, "AccumulativeTimes")
	self.num:SetString(CPPGameLib.GetString("manor_aggregate",lst))
end

function Modules.TempManorSkill:OnButtonClose( info )
	self:DeleteMe()
end

--刷新
function Modules.TempManorSkill:UpLst( info )
	local lst = CallModuleFunc(ModuleType.MANOR, "GetSkillLst")
	local data = {}
	for i,v in ipairs( lst ) do
		data[v.id] = v
	end
	self.__zones_scrollview:ItemDataIter(function( item_index , item_data , item)
		if info.city_id == item_data.id then
			item_data = data[item_data.id]
			if item then
				item:SetShowUI(item_data)
			end
		end
	end)
end






Modules = Modules or {}

Modules.TempManorSkillItem = Modules.TempManorSkillItem or BaseClass(GUI.ScrollItem)

function Modules.TempManorSkillItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_MANOR_SKILL_ITEM) 
end

function Modules.TempManorSkillItem:__delete()
end

function Modules.TempManorSkillItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
	self:__ShowUI(data)
	self:SetShowUI(data)
end

function Modules.TempManorSkillItem:__AddToParent()
end

function Modules.TempManorSkillItem:__Getwidget( data )
	--背景
	self.Image_Bg = self:GetWidget("Image_Bg")
	--图标
	self.Image_icon = self:GetWidget("Image_icon")
	--属性
	self.Image_content = self:GetWidget("Image_content")
	--名条
	self.Text_name = self:GetWidget("Text_name")
	--购买按钮
	self.Button_challenge = Util:WidgetButton(
												self:GetWidget("Button_challenge") , 
												CPPGameLib.GetString("manor_learn") ,
												function ( ... )
													self:OnBtnLearn(data)
												end,Macros.TypefaceSize.button,nil, Macros.Color.button_yellow  )
	-- 
	--价钱
	self.Panel_currency = self:GetWidget("Panel_currency")
end

function Modules.TempManorSkillItem:__ShowUI( data )

	--名字
	self.name = Util:Label(" " , Macros.TypefaceSize.normal , Macros.Color.headline )
	self.name:SetAnchorPoint(0.5 , 0.5)
	self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 + 1)
	self.Text_name:AddChild(self.name)

	--当前效果
	local manor_nowNature = Util:Label(CPPGameLib.GetString("manor_nowNature") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	manor_nowNature:SetAnchorPoint(0,0.5)
	manor_nowNature:SetPosition( 5 , self.Image_content:GetContentHeight() - manor_nowNature:GetContentHeight()/2 - 5 )
	self.Image_content:AddChild(manor_nowNature)

	self.manor_nature = Util:RichText( "" , Macros.TypefaceSize.normal )
	self.manor_nature:SetAnchorPoint(0,0.5)
	self.manor_nature:SetPosition( 5 , manor_nowNature:GetPositionY() - (manor_nowNature:GetContentHeight()/2) - (self.manor_nature:GetContentHeight()/2) - 10 )
	self.Image_content:AddChild(self.manor_nature)

	--升级效果
	local manor_nexNature = Util:Label(CPPGameLib.GetString("manor_nexNature") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	manor_nexNature:SetAnchorPoint(0,0.5)
	manor_nexNature:SetPosition( 5 , self.manor_nature:GetPositionY() - (self.manor_nature:GetContentHeight()/2) - (manor_nexNature:GetContentHeight()/2) - 10 )
	self.Image_content:AddChild(manor_nexNature)

	self.manor_nexnature = Util:RichText( "" , Macros.TypefaceSize.normal )
	self.manor_nexnature:SetAnchorPoint(0,0.5)
	self.manor_nexnature:SetPosition( 5 , manor_nexNature:GetPositionY() - (manor_nexNature:GetContentHeight()/2) - (self.manor_nexnature:GetContentHeight()/2) - 10 )
	self.Image_content:AddChild(self.manor_nexnature)

	--巡逻多少小时后可升级
	self.manor_upKill = Util:Label( " " , Macros.TypefaceSize.normal , Macros.Color.red_color )
	self.manor_upKill:SetAnchorPoint( 0.5 , 0.5 )
	self.manor_upKill:SetPosition( self.Image_Bg:GetContentWidth()/2 , self.Image_Bg:GetContentHeight()/2 - 110 )
	self.Image_Bg:AddChild(self.manor_upKill)
end

function Modules.TempManorSkillItem:SetShowUI( data )
	--名字
	self.name:SetString( data.ruins_skill_name .. " " .. CPPGameLib.GetString("common_lev6" , data.nowLevel ) )

	--图标
	local city = CPPGameLib.GetConfig("LandCity", data.id )
	self.Image_icon:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR_BG, city.skill_icon .. "_kill" ) )

	--当前效果
	local num = string.format( Macros.Color.front_color_rt , data.nowAddition.."%" )
	local text = CPPGameLib.GetString("manor_nature" , num )
	text = string.format( Macros.Color.content_rt , text )
	self.manor_nature:SetText(text)

	--下级效果
	local num = string.format( Macros.Color.front_color_rt , data.nexAddition.."%" )
	local text = CPPGameLib.GetString("manor_nature" , num )
	text = string.format( Macros.Color.content_rt , text )
	if data.nexAddition == 0 then
		text = string.format( Macros.Color.content_rt , CPPGameLib.GetString("manor_nowMaxNature") )
	end
	self.manor_nexnature:SetText(text)

	local time = CallModuleFunc(ModuleType.MANOR, "AccumulativeTimes")
	--巡逻多少小时后可升级
	if data.nexTime > time then
		self.manor_upKill:SetString( CPPGameLib.GetString("manor_upKill", data.nexTime) )
		self.Button_challenge:SetText( CPPGameLib.GetString("manor_noopen") )
		Util:GreyButton( self.Button_challenge )
	end

	if self.Affirm then
		self.Affirm:RemoveSelf(true)
		self.Affirm = nil
	end

	--只要下级的加成不是0
	if data.nexAddition ~= 0 then
		self.Affirm = Util:LabCenter( { 
										{ zi = CPPGameLib.GetString("rebel_consume") , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.keypoint },
										{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) ,dx = 0.6},
										{ zi = data.diamond , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content , jg = 5 },
									} )
		self.Affirm:SetPosition(self.Image_Bg:GetContentWidth()/2 , self.Image_Bg:GetContentHeight()/2 - 145  )
		self.Image_Bg:AddChild(self.Affirm)
	else
		self.Affirm = Util:LabCenter( { 
										{ zi = CPPGameLib.GetString("manor_nowMaxLevel") , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.keypoint },
									} )
		self.Affirm:SetPosition(self.Image_Bg:GetContentWidth()/2 , self.Image_Bg:GetContentHeight()/2 - 145  )
		self.Image_Bg:AddChild(self.Affirm)
	end

	if data.open == "Not" then
		self.Button_challenge:SetText( CPPGameLib.GetString("manor_noopen") )
		Util:GreyButton( self.Button_challenge )
	end

	WidgetUnity.SetWidgetListener( self:GetWidget("Button_challenge") , nil, nil, function()
		self:OnBtnLearn(data)
	end, nil, 0.5 * 1000)
end

function Modules.TempManorSkillItem:OnBtnLearn( data )
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	if data.diamond <= userDate.diamond then
		if data.nexAddition ~= 0 then
			CallModuleFunc(ModuleType.MANOR, "LandSkillLevelupReq",data)
		else
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("manor_nowMaxLevel") )
		end
	else
		local txt = "currency_insufficient_" .. Macros.Game.RolePropType.DIAMOND
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
	end
end