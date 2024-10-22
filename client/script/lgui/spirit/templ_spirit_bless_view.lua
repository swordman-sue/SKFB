--[[
%% @module: 精灵祝福界面
%% @author: ldx
%% @created: 2018年4月11日16:55:26
--]]

local VIEW_TYPE = {
	LINEUP_ENTER = 1,	--阵容界面进入
	SPIRIT_ENTER = 2,	--精灵界面进入
}

LogicGUI = LogicGUI or {}

LogicGUI.TemplSpriteBlessView = LogicGUI.TemplSpriteBlessView or BaseClass(GUI.Template)

function LogicGUI.TemplSpriteBlessView:__init(view_type,bless_data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__InitWidget(view_type,bless_data)
	end	
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS_CELL)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT_BLESS)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS) 
end

function LogicGUI.TemplSpriteBlessView:__delete()
	if self.__spirit_scrollview then
		self.__spirit_scrollview:DeleteMe()
		self.__spirit_scrollview = nil
	end
end

function LogicGUI.TemplSpriteBlessView:__RegistAllEvents()
	--精灵祝福
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_BLESS_RESP, function()  
		if self.__cur_view_type == VIEW_TYPE.LINEUP_ENTER then
			print("ldx","__RegistAllEvents__RegistAllEvents__RegistAllEvents ")
			self:__InitScrollViewInfo()
		else
			--初始化阵位信息
			self:__InitLineupInfo()	
		end
    end)
end

function LogicGUI.TemplSpriteBlessView:__InitWidget(view_type,bless_data)
	--默认阵容界面进入
	self.__cur_view_type = view_type or VIEW_TYPE.LINEUP_ENTER
	--阵容界面进入容器
	self.__panel_heroenterspiritbless = self:GetWidget("Panel_HeroEnterSpiritBless")
	self.__panel_heroenterspiritbless:SetVisible(self.__cur_view_type == VIEW_TYPE.LINEUP_ENTER)
	--精灵界面进入容器
	self.__panel_spiritenterspiritbless = self:GetWidget("Panel_SpiritEnterSpiritBless")
	self.__panel_spiritenterspiritbless:SetVisible(self.__cur_view_type == VIEW_TYPE.SPIRIT_ENTER)
	--初始化按钮和标题
	for i=1,2 do
		--标题
		Util:WidgetLabel(self:GetWidget("Text_title"..i), CPPGameLib.GetString("spirit_bless_title"), Macros.TypefaceSize.tab, Macros.Color.Special)		
		--按钮
		Util:WidgetButton(self:GetWidget("Button_close"..i), "",function ()
			self:DeleteMe()
		end)
	end
	--当前界面
	if self.__cur_view_type == VIEW_TYPE.LINEUP_ENTER then
		self.__bless_data = bless_data
		--提示
		Util:WidgetLabel(self:GetWidget("Text_TouchTips"), CPPGameLib.GetString("spirit_bless_tips"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
		--滚动容器容器
		local panel_spiritscrollview = self:GetWidget("Panel_SpiritScrollView")	
		local params = {
			item_class = Modules.TemplSpriteBlessCell,
		    item_width = 158,
		    item_height	= 355,
		    row = 1,
		    col	= 5,
		    horizon = true,
		    item_space_c = 16,
		    view_width = 854,
		    view_height = 355,
		}
		self.__spirit_scrollview = GUI.ScrollView.New(params)
		panel_spiritscrollview:AddChild(self.__spirit_scrollview:GetNode())
		PosTool.Center(self.__spirit_scrollview)
		self:__InitScrollViewInfo()
	else
		self.__cur_bless_sprite_id = bless_data.spirit_id
		local config_spirit = CPPGameLib.GetConfig("Spirit",bless_data.spirit_id)
		--当前精灵配置
		local cur_spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID",bless_data.spirit_id)
		--特性配置
		local config_spiritspecial = CPPGameLib.GetConfig("SpriteSpecialProperty",bless_data.spirit_id)
		--祝福技能
		Util:WidgetLabel(self:GetWidget("Text_SpiritFigureTitle"),CPPGameLib.GetString("spirit_blessingofskill_title"), Macros.TypefaceSize.largish, Macros.Color.headline)
		--祝福技能描述
		local des_str = config_spiritspecial.star_list[cur_spirit_data.star] and config_spiritspecial.star_list[cur_spirit_data.star].bless_skill_des or ""
		local rt_bless_des = Util:RichText(des_str,Macros.TypefaceSize.normal,405,0,Macros.Color.content_hex)
		rt_bless_des:SetAnchorPoint(0,1)
		self.__panel_spiritenterspiritbless:AddChild(rt_bless_des)
		rt_bless_des:SetPosition(56,504)
		--祝福属性
		Util:WidgetLabel(self:GetWidget("Text_SpiritBlessPropTitle"),CPPGameLib.GetString("spirit_blessingofprop_title"), Macros.TypefaceSize.largish, Macros.Color.headline)
		--四围属性
		local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
		local plus_prop_map = ConfigAdapter.Spirit.GetPropMapEx(spirit_data,bless_data.spirit_id,nil,true)
		for prop_type,prop_value in ipairs(plus_prop_map) do	
			--临时应急
			if prop_type <= 4 then
				Util:WidgetLabel(self:GetWidget("Text_PropType"..prop_type),CPPGameLib.GetString("nature"..prop_type)..CPPGameLib.GetString("common_colon"), Macros.TypefaceSize.normal,Macros.Color.keypoint)
				Util:WidgetLabel(self:GetWidget("Text_PropTypeNext"..prop_type),math.floor(prop_value), Macros.TypefaceSize.normal,Macros.Color.content)
			end
		end
		--初始化阵位信息
		self:__InitLineupInfo()	
	end
end

function LogicGUI.TemplSpriteBlessView:__InitScrollViewInfo()
	local spirit_bless_list = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritBlessList")
	local lineup_info = CallModuleFunc(ModuleType.HERO,"GetLineupInfo",self.__bless_data.lineup_index or 1)
	self.__cur_bless_sprite_id = lineup_info.bless_sprite_id
	--数据整理
	for i,bless_info in ipairs(spirit_bless_list) do
		bless_info.formation_pos = (self.__bless_data.lineup_index or 1) - 1
		bless_info.cur_herouid = self.__bless_data.uid or 0
		bless_info.bless_sprite_id = lineup_info.bless_sprite_id
	end
	self.__spirit_scrollview:SetDataList(spirit_bless_list)
end

function LogicGUI.TemplSpriteBlessView:__InitLineupInfo()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	--阵位英雄
	local lineup_list = CallModuleFunc(ModuleType.HERO,"GetLineupList")
	for lineup_index,lineup_info in ipairs(lineup_list) do
		--英雄名字
		local txt_heroname = self:GetWidget("Text_HeroName"..lineup_index)
		txt_heroname:SetVisible(lineup_info.uid ~= 0)
		--半身像
		local img_halfhead = self:GetWidget("Image_HalfHead"..lineup_index)
		img_halfhead:SetVisible(lineup_info.uid ~= 0)
		if lineup_info.uid ~= 0 then
			--半身像
			local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",lineup_info.uid)
			local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
			local res_name = Resource.PathTool.GetHalfHeadPath(config_hero.model_id)
			img_halfhead:IgnoreContentAdaptWithSize(true)
			img_halfhead:SetImage(res_name,TextureResType.LOCAL)
			--英雄名字				
			local tx_name = hero_info.break_level > 0 and CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level) or config_hero.name 
			Util:WidgetLabel(txt_heroname, tx_name, Macros.TypefaceSize.slightly, Macros.Quality[config_hero.init_quality], 0, 0, Macros.Quality_Stroke[config_hero.init_quality])
		end		
		--精灵半身像
		local panel_cutspirithalfhead = self:GetWidget("Panel_CutSpiritHalfHead"..lineup_index)	
		panel_cutspirithalfhead:SetVisible(lineup_info.bless_sprite_id ~= 0)
		if lineup_info.bless_sprite_id ~= 0 then
			local img_cutspirithalfhead = self:GetWidget("Image_CutSpiritHalfHead"..lineup_index)
			local res_name = Resource.PathTool.GetHalfHeadPath(lineup_info.bless_sprite_id)
			img_cutspirithalfhead:IgnoreContentAdaptWithSize(true)
			img_cutspirithalfhead:SetScaleFactor(0.5)
			img_cutspirithalfhead:SetImage(res_name,TextureResType.LOCAL)
		end
		--祝福类型
		if lineup_info.bless_sprite_id ~= 0 then			
			self["__cur_bless_type"..lineup_index] = (lineup_info.bless_sprite_id == self.__cur_bless_sprite_id) and Macros.Game.BLESS_TYPE.CANCEL_BLESS or Macros.Game.BLESS_TYPE.REPLACE_BLESS
		else
			self["__cur_bless_type"..lineup_index] = Macros.Game.BLESS_TYPE.BLESS
		end		
		--祝福按钮
		local btn_bless = self:GetWidget("Button_Bless"..lineup_index)
		btn_bless:SetVisible(lineup_info.uid ~= 0)
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,self["__cur_bless_type"..lineup_index] ~= Macros.Game.BLESS_TYPE.BLESS and "btn_btn_littleyellow" or "btn_btn_littlered")
		btn_bless:SetNormalImage(sp) --,TextureResType.LOCAL
		Util:WidgetButton(btn_bless,CPPGameLib.GetString("spirit_btn_blesstype"..self["__cur_bless_type"..lineup_index]),function ()
			--当前英雄有精灵祝福
			if lineup_info.bless_sprite_id ~= 0 then
				--根据状态
				if self["__cur_bless_type"..lineup_index] == Macros.Game.BLESS_TYPE.BLESS then
					CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__cur_bless_sprite_id,bless_type = Macros.Game.BLESS_TYPE.REPLACE_BLESS,formation_pos = (lineup_index-1)})
				else
					CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self["__cur_bless_type"..lineup_index] == Macros.Game.BLESS_TYPE.REPLACE_BLESS and self.__cur_bless_sprite_id or 0,bless_type = self["__cur_bless_type"..lineup_index],formation_pos = (lineup_index-1)})
				end    
			else
				--当前英雄没有精灵祝福
				--如果当前精灵有祝福对象则不用计算数量
				if CallModuleFunc(ModuleType.SPIRIT,"GetIsSpiritBlessBySpiritId",self.__cur_bless_sprite_id) then
					--祝福类型
					CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__cur_bless_sprite_id,bless_type = Macros.Game.BLESS_TYPE.BLESS,formation_pos = (lineup_index-1)})
				else
					--先判断是否超出祝福数量
					local cur_bless_num = CallModuleFunc(ModuleType.HERO,"GetLineupListBlessHeroNum") 
					local bless_sprite_num_max,next_open_level = ConfigAdapter.Spirit.GetCurBlessNumAndNextOpenLevel()
					if cur_bless_num >= bless_sprite_num_max then
						GlobalTipMsg.GetInstance():Show(next_open_level and CPPGameLib.GetString("spirit_bless_fulltips1",next_open_level) or CPPGameLib.GetString("spirit_bless_fulltips2"))
						return 
					end
				end
				--祝福类型
				CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__cur_bless_sprite_id,bless_type = Macros.Game.BLESS_TYPE.BLESS,formation_pos = (lineup_index-1)})
			end
		end, Macros.TypefaceSize.slightly, Macros.Color.btn,self["__cur_bless_type"..lineup_index] ~= Macros.Game.BLESS_TYPE.BLESS and Macros.Color.button_yellow or Macros.Color.button_red, 2)
		--遮罩
		self:GetWidget("Panel_BlackShield"..lineup_index):SetVisible(lineup_info.uid == 0)
		--提示文字
		local txt_noherotips = self:GetWidget("Text_NoHeroTips"..lineup_index)
		txt_noherotips:SetVisible(lineup_info.uid == 0)
		--阵位配置
		local pos_info = ConfigAdapter.Hero.GetLineupPosInfo(lineup_index)
		local is_open = pos_info and pos_info.need_level <= role_level
		Util:WidgetLabel(txt_noherotips, is_open and CPPGameLib.GetString("spirit_bless_unlineup") or CPPGameLib.GetString("spirit_lineup_openlimit",pos_info.need_level), Macros.TypefaceSize.slightly, Macros.Color.Special)
		--为上阵标记
		self:GetWidget("Image_NoHeroTips"..lineup_index):SetVisible(lineup_info.uid == 0 and not is_open and true or false)
		--未开启标记
		self:GetWidget("Image_LockTips"..lineup_index):SetVisible(lineup_info.uid == 0 and is_open and true or false)
	end
end

--[[
--选择精灵条目
]]--

Modules = Modules or {}

Modules.TemplSpriteBlessCell = Modules.TemplSpriteBlessCell or BaseClass(GUI.ScrollItem)


function Modules.TemplSpriteBlessCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS_CELL)
end

function Modules.TemplSpriteBlessCell:__delete()

end

function Modules.TemplSpriteBlessCell:SetData(data)	
	GUI.ScrollItem.SetData(self,data)
	local config_spirit = CPPGameLib.GetConfig("Spirit",self.__data.id)
	--半身像
	print("ldx","TemplSpriteBlessCellTemplSpriteBlessCell ",self.__data.id)
	local img_spirithalfhead = self:GetWidget("Image_SpiritHalfHead")
	local res_name = Resource.PathTool.GetHalfHeadPath(self.__data.id)	
	img_spirithalfhead:IgnoreContentAdaptWithSize(true)
	img_spirithalfhead:SetScaleFactor(0.5)
	img_spirithalfhead:SetImage(res_name,TextureResType.LOCAL)
	--祝福技能
	WidgetUnity.SetWidgetListener(img_spirithalfhead, nil, nil,function ()
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS_SKILL, false, self.__data.id)
	end)	
	--精灵名字
	local spirit_config = CPPGameLib.GetConfig("SpriteTrain",self.__data.id) 
	local txt_heroname = self:GetWidget("Text_HeroName")
	local txt_str = ""
	if self.__data.train_stage then
		local skill_lv = spirit_config.stage_list[self.__data.train_stage].skill_level
		txt_str = config_spirit.name..CPPGameLib.GetString("spirit_sublimate_level",skill_lv)
	else
		txt_str = config_spirit.name
	end
	Util:WidgetLabel(txt_heroname,txt_str, Macros.TypefaceSize.slightly, Macros.Quality[config_spirit.quality], 0, 0, Macros.Quality_Stroke[config_spirit.quality])
	--属性
	local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
	local plus_prop_map = ConfigAdapter.Spirit.GetPropMapEx(spirit_data,self.__data.id,nil,true)
	for prop_type,prop_value in ipairs(plus_prop_map) do
		if prop_type <= 4 then		
			Util:WidgetLabel(self:GetWidget("Text_PropTypeName"..prop_type),CPPGameLib.GetString("nature"..prop_type)..CPPGameLib.GetString("common_colon"), Macros.TypefaceSize.slightly,Macros.Color.keypoint)
			Util:WidgetLabel(self:GetWidget("Text_PropTypeNext"..prop_type),math.floor(prop_value), Macros.TypefaceSize.slightly,Macros.Color.content)
		end
	end
	--祝福英雄
	if self.__data.bless_herouid then
		local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__data.bless_herouid)
		local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
		local rt_lab
		if hero_info.break_level > 0 then
			rt_lab = string.format(Macros.Quality_Stroke_RT[config_hero.init_quality],CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level))
		else
			rt_lab = string.format(Macros.Quality_Stroke_RT[config_hero.init_quality],config_hero.name)
		end		
		local bless_hero_rt = Util:RichText(CPPGameLib.GetString("spirit_bless_heroname",rt_lab), Macros.TypefaceSize.minimum)
		self:GetNode():AddChild(bless_hero_rt)
		PosTool.CenterBottom(bless_hero_rt,0,56.5)
	end
	--判断当前的祝福状态
	if self.__data.bless_herouid then
		self.__data.bless_type = self.__data.bless_herouid == self.__data.cur_herouid and Macros.Game.BLESS_TYPE.CANCEL_BLESS or Macros.Game.BLESS_TYPE.REPLACE_BLESS
	else
		self.__data.bless_type = Macros.Game.BLESS_TYPE.BLESS
	end
	--祝福按钮
	local btn_bless = self:GetWidget("Button_bless")
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,self.__data.bless_type ~= Macros.Game.BLESS_TYPE.BLESS and "btn_btn_littleyellow" or "btn_btn_littlered")
	btn_bless:SetNormalImage(sp) --,TextureResType.LOCAL
	Util:WidgetButton(btn_bless,self.__data.is_exist and CPPGameLib.GetString("spirit_btn_blesstype"..self.__data.bless_type) or CPPGameLib.GetString("spirit_no_exist"),function ()
		--当前英雄有精灵祝福
		if self.__data.bless_sprite_id ~= 0 then
			--根据状态
			if self.__data.bless_type == Macros.Game.BLESS_TYPE.BLESS then
				CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__data.id,bless_type = Macros.Game.BLESS_TYPE.REPLACE_BLESS,formation_pos = self.__data.formation_pos})
			else
				CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__data.bless_type == Macros.Game.BLESS_TYPE.REPLACE_BLESS and self.__data.id or 0,bless_type = self.__data.bless_type,formation_pos = self.__data.formation_pos})
			end
		else
			--当前英雄没有精灵祝福
			--如果当前精灵有祝福对象则不用计算数量
			if CallModuleFunc(ModuleType.SPIRIT,"GetIsSpiritBlessBySpiritId",self.__data.id) then
				--祝福类型
				CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__data.id,bless_type = Macros.Game.BLESS_TYPE.BLESS,formation_pos = self.__data.formation_pos})
			else
				--先判断是否超出祝福数量
				local cur_bless_num = CallModuleFunc(ModuleType.HERO,"GetLineupListBlessHeroNum") 
				local bless_sprite_num_max,next_open_level = ConfigAdapter.Spirit.GetCurBlessNumAndNextOpenLevel()
				if cur_bless_num >= bless_sprite_num_max then
					GlobalTipMsg.GetInstance():Show(next_open_level and CPPGameLib.GetString("spirit_bless_fulltips1",next_open_level) or CPPGameLib.GetString("spirit_bless_fulltips2"))
					return 
				end
			end
			--祝福类型
			CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = self.__data.id,bless_type = Macros.Game.BLESS_TYPE.BLESS,formation_pos = self.__data.formation_pos})
		end
	end, Macros.TypefaceSize.slightly, Macros.Color.btn,self.__data.bless_type ~= Macros.Game.BLESS_TYPE.BLESS and Macros.Color.button_yellow or Macros.Color.button_red, 2)
	--未获得
	if not self.__data.is_exist then		
		Util:GreyButton( btn_bless )
	end
end